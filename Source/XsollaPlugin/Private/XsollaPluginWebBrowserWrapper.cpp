#include "XsollaPluginWebBrowserWrapper.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

UXsollaPluginWebBrowserWrapper::UXsollaPluginWebBrowserWrapper(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

void UXsollaPluginWebBrowserWrapper::NativeConstruct()
{
	Super::NativeConstruct();

	TSharedPtr<SBox> InspectorBox;
	TSharedPtr<SVerticalBox> MainContent = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(700.0f)
		.Padding(50)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(700.0f)
			//.AutoWidth()
			.Padding(50)
			//.HAlign(HAlign_Center)
			[	
				SNew(SWebBrowser)
				.InitialURL(InitialURL)
				.ShowControls(false)
				.SupportsTransparency(bSupportsTransparency)
				.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
				.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadCompleted))
				.OnLoadError(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadError))
				.OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow))	
			]	
		];
		
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