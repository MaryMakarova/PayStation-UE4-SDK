#pragma once

#include "XsollaPluginHttpTool.h"
#include "XsollaPluginWebBrowserWrapper.h"

#include "XsollaPluginShop.generated.h"

/**
* Shop size enum. Available sizes - Small, Medium, Large. 
*/
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
    /**
    * Creates shop wrapper, set default properties and delegates.
    *
    * @param shopSize - Size of shop page and wrapper.
    * @param OnSucceeded - On payment succeeded delegate.
    * @param OnCanceled - On payment canceled delegate.
    * @param OnFailed - On payment failed delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    void Create(
        EShopSizeEnum shopSize,
        FOnPaymantSucceeded OnSucceeded,
        FOnPaymantCanceled OnCanceled, 
        FOnPaymantFailed OnFailed);

    
    /**
     * Set number property in token json.
     * 
     * @param prop - Property name.
     * @param value - Int value to set.
     * @param bOverride - Can the method overrides property value if exists.
     */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    void SetNumberProperty(FString prop, int value, bool bOverride = true);

    /**
    * Set bool property in token json.
    *
    * @param prop - Property name.
    * @param value - Bool value to set.
    * @param bOverride - Can the method overrides property value if exists.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    void SetBoolProperty(FString prop, bool value, bool bOverride = true);

    /**
    * Set string property in token json.
    *
    * @param prop - Property name.
    * @param value - String value to set.
    * @param bOverride - Can the method overrides property value if exists.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    void SetStringProperty(FString prop, FString value, bool bOverride = true);

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