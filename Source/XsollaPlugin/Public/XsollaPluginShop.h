#pragma once

#include "XsollaPluginBPLibrary.h"
#include "Slate.h"
#include "XsollaPluginHttpTool.h"
#include "XsollaPluginWebBrowserWrapper.h"
#include "XsollaPluginShop.generated.h"

UCLASS()
class UXsollaPluginShop : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	void CreateShop(FOnPaymantSucceeded OnSucceeded, FOnPaymantCanceled OnCanceled, FOnPaymantFailed OnFailed);
	void OnGetTokenRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void ReceiveUpdateTimer();

private:
	void GetToken(FString shopJson);

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

	FString ExternalId;
};