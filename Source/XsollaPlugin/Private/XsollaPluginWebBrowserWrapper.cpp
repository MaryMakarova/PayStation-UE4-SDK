#include "XsollaPluginWebBrowserWrapper.h"
#include "SWebBrowser.h"
#include "XsollaPluginShop.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Slate/SlateGameResources.h"
#include "XsollaPluginSettings.h"
#include "Misc/Base64.h"
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"

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

	ContentSize = ViewportSize * 0.8;
	ContentSize.X = ContentSize.X > 720.0f ? 720.0f : ContentSize.X;

	TSharedRef<FSlateGameResources> SlateButtonResources = FSlateGameResources::New(
		FName("ButtonStyle"), 
		"/XsollaPlugin/CloseButton", 
		"/XsollaPlugin/CloseButton"
	);
	FSlateGameResources& ButtonStyle = SlateButtonResources.Get();
	const FSlateBrush* slate_close_red = ButtonStyle.GetBrush(FName("close_red_brush"));

	TSharedRef<FSlateGameResources> SlateSpinnerResources = FSlateGameResources::New(
		FName("SpinnerStyle"),
		"/XsollaPlugin/LoadingSpinner",
		"/XsollaPlugin/LoadingSpinner"
	);
	FSlateGameResources& SpinnerStyle = SlateSpinnerResources.Get();
	const FSlateBrush* slate_spinner = SpinnerStyle.GetBrush(FName("spinner_brush"));

	SAssignNew(WebBrowserWidget, SWebBrowser)
		.InitialURL(InitialURL)
		.ShowControls(false)
		.ViewportSize(ContentSize)
		.SupportsTransparency(bSupportsTransparency)
		.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
		.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadCompleted))
		.OnLoadError(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadError))
		.OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow));

	BrowserSlot.AttachWidget(SAssignNew(SpinnerImage, SSpinningImage).Image(slate_spinner).Period(3.0f));
	BrowserSlot.FillWidth(ContentSize.Y);

	BrowserSlotMarginLeft.FillWidth((ViewportSize.X - ContentSize.Y) / 2);
	BrowserSlotMarginRight.FillWidth((ViewportSize.X - ContentSize.Y) / 2 - ButtonSize);

	MainContent = SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight((ViewportSize.Y - ContentSize.Y) / 2)
			+ SVerticalBox::Slot()
			.FillHeight(ContentSize.Y)
		    [
				SNew(SHorizontalBox)
				+ BrowserSlotMarginLeft
				+ BrowserSlot
				+ SHorizontalBox::Slot()
					.FillWidth(ButtonSize)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.FillHeight(ButtonSize)
						[
							SAssignNew(CloseButton, SButton)
							.Visibility(EVisibility::Hidden)
							.ButtonColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 0)))
							.OnClicked_Lambda([this]()
							{
								this->CloseShop();

								return FReply::Handled();
							})
							.Content()
							[
								SNew(SImage)
								.Image(slate_close_red)
							]
						]
						+ SVerticalBox::Slot()
						.FillHeight(ContentSize.Y - ButtonSize)
					]
				+ BrowserSlotMarginRight
			]
			+ SVerticalBox::Slot()
			.FillHeight((ViewportSize.Y - ContentSize.Y) / 2);

	Background = SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			 .FillHeight(ViewportSize.Y)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				 .FillWidth(ViewportSize.X)
				[
					SNew(SColorBlock).Color(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f))
				]
			];
		
	GEngine->GameViewport->AddViewportWidgetContent(MainContent.ToSharedRef(), 10);
	GEngine->GameViewport->AddViewportWidgetContent(Background.ToSharedRef(), 9);

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


void UXsollaPluginWebBrowserWrapper::HandleOnUrlChanged(const FText& InText)
{
	if (WebBrowserWidget->GetUrl().Contains("www.unrealengine"))
	{
		CloseShop();
	}

	OnUrlChanged.Broadcast(InText);
}
void UXsollaPluginWebBrowserWrapper::HandleOnLoadCompleted()
{
	if (WebBrowserWidget->GetUrl().Contains("xsolla"))
	{
		BrowserSlot.DetachWidget();
		BrowserSlot.AttachWidget(WebBrowserWidget.ToSharedRef());
		BrowserSlot.FillWidth(ContentSize.X);

		CloseButton->SetVisibility(EVisibility::Visible);

		BrowserSlotMarginLeft.FillWidth((ViewportSize.X - ContentSize.X) / 2);
		BrowserSlotMarginRight.FillWidth((ViewportSize.X - ContentSize.X) / 2 - ButtonSize);

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

			UTransactionDetails* transactionDetails = NewObject<UTransactionDetails>(UTransactionDetails::StaticClass());

			TSharedPtr<FJsonObject> transaction = root->GetObjectField("transaction");
			transactionDetails->TransactionStatus = transaction->GetStringField("status");
			transactionDetails->TransactionId = transaction->GetIntegerField("id");

			TSharedPtr<FJsonObject> transactionProject = transaction->GetObjectField("project");
			transactionDetails->TransactionProjectId = transactionProject->GetStringField("id");

			TSharedPtr<FJsonObject> transactionPaymentMethod = transaction->GetObjectField("payment_method");
			transactionDetails->PaymentMethodId = transactionPaymentMethod->GetIntegerField("id");
			transactionDetails->PaymentMethodName = transactionPaymentMethod->GetStringField("name");

			TSharedPtr<FJsonObject> user = root->GetObjectField("user");
			transactionDetails->UserId = user->GetIntegerField("id");
			transactionDetails->UserEmail = user->GetStringField("email");
			transactionDetails->UserCountry = user->GetStringField("country");

			TSharedPtr<FJsonObject> paymentDetails = root->GetObjectField("payment_details");

			TSharedPtr<FJsonObject> payment = paymentDetails->GetObjectField("payment");
			transactionDetails->PaymentCurrency = payment->GetStringField("currency");
			transactionDetails->PaymentAmount = payment->GetIntegerField("amount");
			
			TSharedPtr<FJsonObject> salesTax = paymentDetails->GetObjectField("sales_tax");
			transactionDetails->PaymentSalesTaxAmount = salesTax->GetIntegerField("amount");
			transactionDetails->PaymentSalesTaxPercent = salesTax->GetIntegerField("percent");

			TSharedPtr<FJsonObject> purchase = root->GetObjectField("purchase");
			transactionDetails->PurchaseVirtualItems = purchase->GetStringField("virtual_items");

			TSharedPtr<FJsonObject> virtualCurrency = purchase->GetObjectField("virtual_currency");
			transactionDetails->PurchaseVirtualCurrencyAmount = virtualCurrency->GetIntegerField("amount");
			transactionDetails->PurchaseVirtualCurrencyName = virtualCurrency->GetStringField("name");

			TSharedPtr<FJsonObject> simpleCheckout = purchase->GetObjectField("simple_checkout");
			transactionDetails->PurchaseSimpleCheckoutAmount = simpleCheckout->GetIntegerField("amount");
			transactionDetails->PurchaseSimpleCheckoutCurrency = simpleCheckout->GetStringField("currency");

			TSharedPtr<FJsonObject> subscription = purchase->GetObjectField("subscription");
			transactionDetails->PurchaseSubscriptionName = subscription->GetStringField("name");

			if (transactionDetails->TransactionStatus == "done")
			{
				this->OnSucceeded.Execute(0, transactionDetails);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No transactions"));
		}
	}
}

void UXsollaPluginWebBrowserWrapper::CloseShop()
{
	GEngine->GameViewport->RemoveViewportWidgetContent(MainContent.ToSharedRef());
	GEngine->GameViewport->RemoveViewportWidgetContent(Background.ToSharedRef());

	FInputModeGameAndUI inputModeGameAndUI;
	GEngine->GetFirstLocalPlayerController(GetWorld())->SetInputMode(inputModeGameAndUI);

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
	httpTool->SetAuthorizationHash(FString("Basic ") + FBase64::Encode(MerchantId + FString(":") + ApiKey), Request);

	httpTool->Send(Request);
}

#undef LOCTEXT_NAMESPACE