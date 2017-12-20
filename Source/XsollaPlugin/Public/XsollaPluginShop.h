#pragma once

#include "XsollaPluginBPLibrary.h"
#include "SlateBasics.h"
#include "XsollaPluginHttpTool.h"
#include "XsollaPluginWebBrowserWrapper.h"
#include "XsollaPluginShop.generated.h"

UCLASS()
class XSOLLAPLUGIN_API UXsollaPluginShop : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	void CreateShop(
		FString shopSize, 
		FOnPaymantSucceeded OnSucceeded,
		FOnPaymantCanceled OnCanceled, 
		FOnPaymantFailed OnFailed);

	
	/**
	 * prop format is "{object}.{value}"
	 */
	bool SetProperty(FString prop, int value);
	bool SetProperty(FString prop, bool value);
	bool SetProperty(FString prop, FString value);
	bool SetProperty(FString prop, const ANSICHAR* value);

private:
	void GetToken(FString shopJson);
	void OnGetTokenRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

private:
	XsollaPluginHttpTool * HttpTool;

	FString XsollaToken;
	FString ShopUrl;
	FString MerchantId;
	FString ApiKey;
	FString ProjectId;
	bool bIsSandbox;

	UXsollaPluginWebBrowserWrapper* BrowserWrapper;

	FOnPaymantSucceeded		OnSucceeded;
	FOnPaymantCanceled		OnCanceled;
	FOnPaymantFailed		OnFailed;

	FString					ExternalId;

	TSharedPtr<FJsonObject> TokenJson;
};