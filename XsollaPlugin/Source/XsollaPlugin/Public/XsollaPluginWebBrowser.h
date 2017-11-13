// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/JsonUtilities/Public/JsonUtilities.h"
#include "Runtime/Json/Public/Json.h"
#include "XsollaPluginWebBrowser.generated.h"

class IWebBrowserWindow;

/**
*
*/
UCLASS()
class XSOLLAPLUGIN_API UXsollaPluginWebBrowser : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadCompleted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadError);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseWindow);

	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		void GetToken();

	/**
	* Load the specified URL
	*
	* @param NewURL New URL to load
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		void LoadURL(FString NewURL);

	/**
	* Load a string as data to create a web page
	*
	* @param Contents String to load
	* @param DummyURL Dummy URL for the page
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		void LoadString(FString Contents, FString DummyURL);

	/**
	* Executes a JavaScript string in the context of the web page
	*
	* @param ScriptText JavaScript string to execute
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		void ExecuteJavascript(const FString& ScriptText);

	/**
	* Get the current title of the web page
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		FText GetTitleText() const;

	/**
	* Gets the currently loaded URL.
	*
	* @return The URL, or empty string if no document is loaded.
	*/
	UFUNCTION(BlueprintCallable, Category = "Web Browser")
		FString GetUrl() const;

	/** Called when the Url changes. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnUrlChanged OnUrlChanged;

	/** Called when the page load completed. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnLoadCompleted OnLoadCompleted;

	/** Called when the page load error. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnLoadError OnLoadError;

	/** Called when close the window. */
	UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
		FOnCloseWindow OnCloseWindow;

public:
	//~ Begin UWidget interface
	virtual void SynchronizeProperties() override;
	// End UWidget interface

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere, Category = Appearance)
		FString InitialURL;

	/** Should the browser window support transparency. */
	UPROPERTY(EditAnywhere, Category = Appearance)
		bool bSupportsTransparency;

protected:
	TSharedPtr<class SWebBrowser> WebBrowserWidget;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	void HandleOnUrlChanged(const FText& Text);
	void HandleOnLoadCompleted();
	void HandleOnLoadError();
	bool HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& BrowserWindow);

private:
	FHttpModule* Http;

	// xsolla stuffs
	FString ApiBaseUrl = "https://api.xsolla.com/merchant/merchants/";
	FString ShopUrl;
	FString MerchantId;
	FString ApiKey;
	FString ProjectId;
	bool bIsSandbox;


	FString AuthorizationHeader = TEXT("Authorization");
	void SetAuthorizationHash(FString Hash, TSharedRef<IHttpRequest>& Request);

	TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);
	void SetRequestHeaders(TSharedRef<IHttpRequest>& Request);

	TSharedRef<IHttpRequest> GetRequest(FString Subroute);
	TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString);
	void Send(TSharedRef<IHttpRequest>& Request);

	bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

	void OnLoadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	template <typename StructType>
	void GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput);
	template <typename StructType>
	void GetStructFromJsonString(FHttpResponsePtr Response, StructType& StructOutput);
};
