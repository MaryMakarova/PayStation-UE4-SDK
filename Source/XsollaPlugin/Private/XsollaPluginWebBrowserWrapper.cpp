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
        WebBrowserWidget->LoadURL(NewURL);
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
                        .OnClicked_Lambda([this]() { this->CloseShop(false); return FReply::Handled(); })
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

    XsollaPluginHttpTool * httpTool = new XsollaPluginHttpTool;


    FString route = "http://52.59.15.45:3333/payment";

    if (ExternalId.IsEmpty())
    {
        OnFailed.Execute(FString("External id is not setted"));
        return;
    }

    TSharedRef<IHttpRequest> Request = httpTool->PostRequest(route, ExternalId);

    Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
        UE_LOG(LogTemp, Warning, TEXT("Http tool: /payment response recieved with code: %d"), Response->GetResponseCode());
        if (bWasSuccessful) {
            FTransactionDetails transactionDetails;

            if (Response->GetResponseCode() == 200) {
                transactionDetails.TransactionStatus = "DONE";
                OnSucceeded.Execute(transactionDetails);
            }
            else {
                transactionDetails.TransactionStatus = "FAILED";
                OnFailed.Execute(FString("Transaction failed"));
            }
        }
    });

    httpTool->Send(Request);
    UE_LOG(LogTemp, Warning, TEXT("Http tool: /payment post request sent"));
}

void UXsollaPluginWebBrowserWrapper::HandleOnUrlChanged(const FText& InText)
{
    if (WebBrowserWidget->GetUrl().Contains("www.unrealengine"))
    {
        CloseShop(false);
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