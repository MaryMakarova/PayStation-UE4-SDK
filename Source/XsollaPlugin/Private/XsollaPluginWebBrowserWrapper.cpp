#include "XsollaPluginWebBrowserWrapper.h"

#include "XsollaPluginShop.h"
#include "XsollaPluginSettings.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Misc/Base64.h"

#define LOCTEXT_NAMESPACE "XsollaPluginWebBrowserWrapper"

UXsollaPluginWebBrowserWrapper::UXsollaPluginWebBrowserWrapper(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
    ButtonSize(50.0f)
{
    bIsVariable = true;
}

void UXsollaPluginWebBrowserWrapper::NativeConstruct()
{
    Super::NativeConstruct();

    ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
    ContentSize.Y = ContentSize.Y > ViewportSize.Y ? ViewportSize.Y : ContentSize.Y;

    LoadSlateResources();

    SAssignNew(WebBrowserWidget, SWebBrowser)
        .InitialURL(InitialURL)
        .ShowControls(false)
        .ViewportSize(ContentSize)
        .SupportsTransparency(bSupportsTransparency)
        .OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
        .OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadCompleted))
        .OnLoadError(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadError))
        .OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow))
        .OnSuppressContextMenu_Lambda([]() { return true; });

    ComposeShopWrapper();

    FInputModeUIOnly inputModeUIOnly;
    GEngine->GetFirstLocalPlayerController(GetWorld())->SetInputMode(inputModeUIOnly);
}

void UXsollaPluginWebBrowserWrapper::LoadURL(FString NewURL)
{
    if (WebBrowserWidget.IsValid())
    {
        return WebBrowserWidget->LoadURL(NewURL);
    }
}

void UXsollaPluginWebBrowserWrapper::LoadSlateResources()
{
    TSharedRef<FSlateGameResources> slateButtonResources = FSlateGameResources::New(
        FName("ButtonStyle"),
        "/XsollaPlugin/Buttons",
        "/XsollaPlugin/Buttons"
    );
    FSlateGameResources& buttonStyle = slateButtonResources.Get();
    SlateCloseBrush = buttonStyle.GetBrush(FName("close_red_brush"));
    SlateBackBrush = buttonStyle.GetBrush(FName("back_brush"));

    TSharedRef<FSlateGameResources> slateSpinnerResources = FSlateGameResources::New(
        FName("SpinnerStyle"),
        "/XsollaPlugin/LoadingSpinner",
        "/XsollaPlugin/LoadingSpinner"
    );
    FSlateGameResources& spinnerStyle = slateSpinnerResources.Get();
    SlateSpinnerBrush = spinnerStyle.GetBrush(FName("spinner_brush"));
}

void UXsollaPluginWebBrowserWrapper::ComposeShopWrapper()
{
    BrowserSlot.AttachWidget(SAssignNew(SpinnerImage, SSpinningImage).Image(SlateSpinnerBrush).Period(3.0f));
    BrowserSlot.FillWidth(ContentSize.Y);

    BrowserSlotMarginLeft.FillWidth((ViewportSize.X - ContentSize.Y) / 2);
    BrowserSlotMarginRight.FillWidth((ViewportSize.X - ContentSize.Y) / 2 - ButtonSize);

    MainContent = 
        SNew(SVerticalBox)
        + SVerticalBox::Slot().FillHeight((ViewportSize.Y - ContentSize.Y) / 2)
        + SVerticalBox::Slot().FillHeight(ContentSize.Y)
        [
            SNew(SHorizontalBox)
            + BrowserSlotMarginLeft
            + BrowserSlot
            + SHorizontalBox::Slot().FillWidth(ButtonSize)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot().FillHeight(ButtonSize)
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot().FillHeight(ButtonSize)
                    [
                        SAssignNew(CloseButton, SButton)
                        .Visibility(EVisibility::Hidden)
                        .ButtonColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 0)))
                        .OnClicked_Lambda([this]() { this->CloseShop(true); return FReply::Handled(); })
                        .Content()
                        [
                            SNew(SImage)
                            .Image(SlateCloseBrush)
                        ]
                    ]
                    + SVerticalBox::Slot()
                    .FillHeight(ButtonSize)
                    [
                        SAssignNew(HomeButton, SButton)
                        .Visibility(EVisibility::Hidden)
                        .ButtonColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 0)))
                        .OnClicked_Lambda([this]() { LoadURL(ShopUrl); return FReply::Handled(); })
                        .Content()
                        [
                            SNew(SImage)
                            .Image(SlateBackBrush)
                        ]
                    ]
                ]
                + SVerticalBox::Slot().FillHeight(ContentSize.Y - ButtonSize)
            ]   
            + BrowserSlotMarginRight
        ]
        + SVerticalBox::Slot().FillHeight((ViewportSize.Y - ContentSize.Y) / 2);

    Background = 
        SNew(SVerticalBox)
        + SVerticalBox::Slot().FillHeight(ViewportSize.Y)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(ViewportSize.X)
            [
                SNew(SColorBlock).Color(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f))
            ]
        ];

    GEngine->GameViewport->AddViewportWidgetContent(MainContent.ToSharedRef(), 10);
    GEngine->GameViewport->AddViewportWidgetContent(Background.ToSharedRef(), 9);
}

void UXsollaPluginWebBrowserWrapper::CloseShop(bool bCheckTransactionResult)
{
    GEngine->GameViewport->RemoveViewportWidgetContent(MainContent.ToSharedRef());
    GEngine->GameViewport->RemoveViewportWidgetContent(Background.ToSharedRef());

    FInputModeGameAndUI inputModeGameAndUI;
    GEngine->GetFirstLocalPlayerController(GetWorld())->SetInputMode(inputModeGameAndUI);

    if (bCheckTransactionResult)
    {
        XsollaPluginHttpTool * httpTool = new XsollaPluginHttpTool;

        FString MerchantId = GetDefault<UXsollaPluginSettings>()->MerchantId;
        FString ProjectId = GetDefault<UXsollaPluginSettings>()->ProjectId;
        FString ApiKey = GetDefault<UXsollaPluginSettings>()->ApiKey;

        FString route = "https://api.xsolla.com/merchant/v2/merchants/";
        route += MerchantId;
        route += "/reports/transactions/search.json";
        route += "?external_id=";
        route += ExternalId;
        route += "&type=all";

        TSharedRef<IHttpRequest> Request = httpTool->GetRequest(route);

        Request->OnProcessRequestComplete().BindUObject(this, &UXsollaPluginWebBrowserWrapper::OnTransactionResponse);
        httpTool->SetAuthorizationHash(FString("Basic ") + FBase64::Encode(MerchantId + FString(":") + XsollaPluginEncryptTool::DecryptString(ApiKey)), Request);

        httpTool->Send(Request);
    }
}

void UXsollaPluginWebBrowserWrapper::HandleOnUrlChanged(const FText& InText)
{
    if (WebBrowserWidget->GetUrl().Contains("www.unrealengine"))
    {
        CloseShop(true);
    }

    if (!WebBrowserWidget->GetUrl().Contains("xsolla"))
    {
        HomeButton->SetVisibility(EVisibility::Visible);
        CloseButton->SetVisibility(EVisibility::Collapsed);
    }

    if (WebBrowserWidget->GetUrl().Contains("xsolla"))
    {
        HomeButton->SetVisibility(EVisibility::Collapsed);
        if (CloseButton->GetVisibility() == EVisibility::Collapsed)
            CloseButton->SetVisibility(EVisibility::Visible);
    }

    OnUrlChanged.Broadcast(InText);
}
void UXsollaPluginWebBrowserWrapper::HandleOnLoadCompleted()
{
    if (WebBrowserWidget->GetUrl().Contains("xsolla"))
    {
        BrowserSlot.DetachWidget();
        BrowserSlot.AttachWidget(WebBrowserWidget.ToSharedRef());
        BrowserSlot.FillWidth(ContentSize.X - ButtonSize);

        CloseButton->SetVisibility(EVisibility::Visible);

        if ((ViewportSize.X - ContentSize.X) / 2 > 0)
        {
            BrowserSlotMarginLeft.FillWidth((ViewportSize.X - ContentSize.X) / 2);
            BrowserSlotMarginRight.FillWidth((ViewportSize.X - ContentSize.X) / 2 - ButtonSize);
        }
        else
        {
            BrowserSlotMarginLeft.FillWidth(0);
            BrowserSlotMarginRight.FillWidth(0);
        }

        FInputModeUIOnly inputModeUIOnly;
        inputModeUIOnly.SetWidgetToFocus(WebBrowserWidget.ToSharedRef());
        GEngine->GetFirstLocalPlayerController(GetWorld())->SetInputMode(inputModeUIOnly);
    }

    OnLoadCompleted.Broadcast();
}
void UXsollaPluginWebBrowserWrapper::HandleOnLoadError()
{
    OnLoadError.Broadcast();
}
bool UXsollaPluginWebBrowserWrapper::HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& NewBrowserWindow)
{
    OnCloseWindow.Broadcast();
    return true;
}

void UXsollaPluginWebBrowserWrapper::OnTransactionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    TSharedPtr<FJsonValue> transactionJsonObj;
    FString content = Response->GetContentAsString();
    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*content);

    if (FJsonSerializer::Deserialize(JsonReader, transactionJsonObj))
    {
        if (transactionJsonObj->AsArray().Num() != 0)
        {
            TSharedPtr<FJsonObject> root = transactionJsonObj->AsArray()[0]->AsObject();

            FTransactionDetails transactionDetails;

            transactionDetails.TransactionStatus                = root->GetObjectField("transaction")->GetStringField("status");
            transactionDetails.TransactionId                    = root->GetObjectField("transaction")->GetNumberField("id");
            transactionDetails.TransactionProjectId             = root->GetObjectField("project")->GetStringField("id");
            transactionDetails.PaymentMethodId                  = root->GetObjectField("payment_method")->GetNumberField("id");
            transactionDetails.PaymentMethodName                = root->GetObjectField("payment_method")->GetStringField("name");
            transactionDetails.UserId                           = root->GetObjectField("user")->GetIntegerField("id");
            transactionDetails.UserEmail                        = root->GetObjectField("user")->GetStringField("email");
            transactionDetails.UserCountry                      = root->GetObjectField("user")->GetStringField("country");
            transactionDetails.PaymentCurrency                  = root->GetObjectField("payment_details")->GetObjectField("payment")->GetStringField("currency");
            transactionDetails.PaymentAmount                    = root->GetObjectField("payment_details")->GetObjectField("payment")->GetIntegerField("amount");
            transactionDetails.PaymentSalesTaxAmount            = root->GetObjectField("payment_details")->GetObjectField("sales_tax")->GetIntegerField("amount");
            transactionDetails.PaymentSalesTaxPercent           = root->GetObjectField("payment_details")->GetObjectField("sales_tax")->GetIntegerField("percent");
            transactionDetails.PurchaseVirtualItems             = root->GetObjectField("purchase")->GetStringField("virtual_items");
            transactionDetails.PurchaseVirtualCurrencyAmount    = root->GetObjectField("purchase")->GetObjectField("virtual_currency")->GetIntegerField("amount");
            transactionDetails.PurchaseVirtualCurrencyName      = root->GetObjectField("purchase")->GetObjectField("virtual_currency")->GetStringField("name");
            transactionDetails.PurchaseSimpleCheckoutAmount     = root->GetObjectField("purchase")->GetObjectField("simple_checkout")->GetIntegerField("amount");
            transactionDetails.PurchaseSimpleCheckoutCurrency   = root->GetObjectField("purchase")->GetObjectField("simple_checkout")->GetStringField("currency");
            transactionDetails.PurchaseSubscriptionName         = root->GetObjectField("purchase")->GetObjectField("subscription")->GetStringField("name");

            //FJsonObjectConverter::JsonObjectToUStruct<FTransactionDetails>(root.ToSharedRef(), &transactionDetails, 0, 0);

            if (transactionDetails.TransactionStatus == "done")
            {
                this->OnSucceeded.Execute(0, transactionDetails);
            }
        }
        else
        {
            this->OnCanceled.Execute();
        }
    }
}

void UXsollaPluginWebBrowserWrapper::SetExternalId(FString str) 
{ 
    ExternalId = str;
}

void UXsollaPluginWebBrowserWrapper::SetShopUrl(FString str) 
{
    ShopUrl = str;
}

void UXsollaPluginWebBrowserWrapper::SetBrowserSize(float w, float h)
{
    ContentSize.X = w;
    ContentSize.Y = h;
}

#undef LOCTEXT_NAMESPACE