#include "XsollaPluginWebBrowserWrapper.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Slate/SlateGameResources.h"
#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"

#define LOCTEXT_NAMESPACE "XsollaPluginWebBrowserWrapper"

UXsollaPluginWebBrowserWrapper::UXsollaPluginWebBrowserWrapper(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

	BrowserSlot.AttachWidget(SAssignNew(SpinnerImage, SSpinningImage).Image(slate_spinner));
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
								GEngine->GameViewport->RemoveViewportWidgetContent(this->MainContent.ToSharedRef());

								FInputModeGameOnly inputModeGameOnly;
								GEngine->GetFirstLocalPlayerController(GetWorld())->SetInputMode(inputModeGameOnly);

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
		
	GEngine->GameViewport->AddViewportWidgetContent(MainContent.ToSharedRef());
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
	BrowserSlot.DetachWidget();
	BrowserSlot.AttachWidget(WebBrowserWidget.ToSharedRef());
	BrowserSlot.FillWidth(ContentSize.X);

	CloseButton->SetVisibility(EVisibility::Visible);

	BrowserSlotMarginLeft.FillWidth((ViewportSize.X - ContentSize.X) / 2);
	BrowserSlotMarginRight.FillWidth((ViewportSize.X - ContentSize.X) / 2 - ButtonSize);

	FInputModeUIOnly inputModeUIOnly;
	inputModeUIOnly.SetWidgetToFocus(WebBrowserWidget.ToSharedRef());
	GEngine->GetFirstLocalPlayerController(GetWorld())->SetInputMode(inputModeUIOnly);

	OnUrlChanged.Broadcast(InText);
}
void UXsollaPluginWebBrowserWrapper::HandleOnLoadCompleted()
{
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

#undef LOCTEXT_NAMESPACE