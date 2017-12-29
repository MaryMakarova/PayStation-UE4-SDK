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
class XSOLLAPLUGIN_API UXsollaPluginShop : public UObject
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
    * Open shop with token.
    *
    * @param token - Xsolla shop token.
    * @param externalId - external id for transaction.
    * @param shopSize - Size of shop page and wrapper.
    * @param OnSucceeded - On payment succeeded delegate.
    * @param OnCanceled - On payment canceled delegate.
    * @param OnFailed - On payment failed delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
        void CreateWithToken(
            FString token, 
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
    XsollaPluginHttpTool * HttpTool;

    FString ShopUrl;
    FString XsollaToken;
    FString XsollaAccessString;
    FString ProjectId;
    bool bIsSandbox;

    UXsollaPluginWebBrowserWrapper* BrowserWrapper;

    FOnPaymantSucceeded		OnSucceeded;
    FOnPaymantCanceled		OnCanceled;
    FOnPaymantFailed		OnFailed;

    TSharedPtr<FJsonObject> TokenRequestJson;
};