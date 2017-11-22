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

	const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

	FVector2D contentSize(viewportSize * 0.8);
	contentSize.X = contentSize.X > 720.0f ? 720.0f : contentSize.X;

	float buttonSize = 50.0f;

	TSharedRef<FSlateGameResources> SlateResources = FSlateGameResources::New(
		FName("ButtonStyle"), 
		"/XsollaPlugin/CloseButton", 
		"/XsollaPlugin/CloseButton"
	);
	FSlateGameResources& Style = SlateResources.Get();
	const FSlateBrush* slate_close_red = Style.GetBrush(FName("close_red_brush"));

	MainContent = SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight((viewportSize.Y - contentSize.Y) / 2)
			+ SVerticalBox::Slot()
			.FillHeight(contentSize.Y)
		    [
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth((viewportSize.X - contentSize.X) / 2)
				+ SHorizontalBox::Slot()
				.FillWidth(contentSize.X)
				[
					SAssignNew(WebBrowserWidget, SWebBrowser)
						.InitialURL(InitialURL)
						.ShowControls(false)
						.ViewportSize(contentSize)
						.SupportsTransparency(bSupportsTransparency)
						.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
						.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadCompleted))
						.OnLoadError(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadError))
						.OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow))
				]
				+ SHorizontalBox::Slot()
					.FillWidth(buttonSize)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.FillHeight(buttonSize)
						[
							SNew(SButton)
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
						.FillHeight(contentSize.Y - buttonSize)
					]
				+ SHorizontalBox::Slot()
				.FillWidth((viewportSize.X - contentSize.X) / 2 - buttonSize)
			]
			+ SVerticalBox::Slot()
			.FillHeight((viewportSize.Y - contentSize.Y) / 2);
		
	GEngine->GameViewport->AddViewportWidgetContent(MainContent.ToSharedRef());

	FInputModeUIOnly inputModeUIOnly;
	inputModeUIOnly.SetWidgetToFocus(WebBrowserWidget.ToSharedRef());
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