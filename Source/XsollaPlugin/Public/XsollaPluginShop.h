#pragma once

#include "SlateBasics.h"
#include "XsollaPluginTransactionDetails.h"
#include "XsollaPluginHttpTool.h"
#include "XsollaPluginWebBrowserWrapper.h"
#include "XsollaPluginShop.generated.h"

UENUM(BlueprintType)
enum class EShopSizeEnum : uint8
{
    VE_Small 	UMETA(DisplayName = "Small"),
    VE_Medium 	UMETA(DisplayName = "Medium"),
    VE_Large	UMETA(DisplayName = "Large")
};

UCLASS()
class UXsollaPluginShop : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    void CreateShop(
        EShopSizeEnum shopSize,
        FOnPaymantSucceeded OnSucceeded,
        FOnPaymantCanceled OnCanceled, 
        FOnPaymantFailed OnFailed);

    
    /**
     * prop format is "{object}.{value}"
     */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    bool SetNumberProperty(FString prop, int value, bool bOverride = true);

    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    bool SetBoolProperty(FString prop, bool value, bool bOverride = true);

    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    bool SetStringProperty(FString prop, FString value, bool bOverride = true);

private:
    void OpenShop(FString tokenRequestJson);
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

    TSharedPtr<FJsonObject> TokenRequestJson;
};