#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "XsollaPluginWebBrowserWrapper.generated.h"

class IWebBrowserWindow;

UCLASS()
class XSOLLAPLUGIN_API UXsollaPluginWebBrowserWrapper : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadCompleted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadError);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseWindow);

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnUrlChanged OnUrlChanged;

	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnLoadCompleted OnLoadCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnLoadError OnLoadError;

	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnCloseWindow OnCloseWindow;

	void LoadURL(FString NewURL);

private:
	TSharedPtr<class SWebBrowser> WebBrowserWidget;

	FString InitialURL = "google.com";
	bool bSupportsTransparency = true;

	void HandleOnUrlChanged(const FText& Text);
	void HandleOnLoadCompleted();
	void HandleOnLoadError();
	bool HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& BrowserWindow);
};