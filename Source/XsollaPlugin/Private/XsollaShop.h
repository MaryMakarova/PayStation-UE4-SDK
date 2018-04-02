#pragma once

#include "XsollaHttpTool.h"
#include "XsollaWebBrowserWrapper.h"

#include "XsollaShop.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPaymantSucceeded, FTransactionDetails, transactionDetails);
DECLARE_DYNAMIC_DELEGATE(FOnPaymantCanceled);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPaymantFailed, FString, errorText);

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

UENUM(BlueprintType)
enum class EIntegrationType : uint8
{
    VE_SERVER      UMETA(DisplayName = "Server"),
    VE_SERVELESS   UMETA(DisplayName = "Serveless")
};

UCLASS()
class XSOLLAPLUGIN_API UXsollaShop : public UObject
{
    friend class UXsollaWebBrowserWrapper;

    GENERATED_UCLASS_BODY()

public:
    /**
    * Creates shop wrapper, set default properties and delegates.
    *
    * @param shopSize - Size of shop page and wrapper.
    * @param userId - user ID
    * @param onSucceeded - On payment succeeded delegate.
    * @param onCanceled - On payment canceled delegate.
    * @param onFailed - On payment failed delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
    void Create(
        EShopSizeEnum shopSize,
        FString userId, 
        FOnPaymantSucceeded onSucceeded,
        FOnPaymantCanceled onCanceled, 
        FOnPaymantFailed onFailed);

    /**
    * Open shop with token.
    *
    * @param token - Xsolla shop token.
    * @param externalId - external id for transaction.
    * @param shopSize - Size of shop page and wrapper.
    * @param onSucceeded - On payment succeeded delegate.
    * @param onCanceled - On payment canceled delegate.
    * @param onFailed - On payment failed delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
        void CreateWithToken(
            FString token, 
            EShopSizeEnum shopSize, 
            FOnPaymantSucceeded onSucceeded,
            FOnPaymantCanceled onCanceled,
            FOnPaymantFailed onFailed);

    
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
    void LoadConfig(EIntegrationType type);
    void SetToken(FString token);
    void SetAccessData(FString data);
    void SetDefaultTokenProperties();
    void OnShopClosed();

private:
    XsollaPluginHttpTool * HttpTool;

    FString ApiUrl          = "https://secure.xsolla.com/paystation3";
    FString SandboxApiUrl   = "https://sandbox-secure.xsolla.com/paystation3";

    FString ShopUrl;
    FString ServerUrl;
    
    FString XsollaToken;
    FString XsollaAccessString;
    FString ProjectId;
    FString ExternalId;
    bool bIsSandbox;

    UXsollaWebBrowserWrapper* BrowserWrapper;

    TSharedPtr<FJsonObject> TokenRequestJson;

    FOnPaymantSucceeded OnSucceeded;
    FOnPaymantCanceled OnCanceled;
    FOnPaymantFailed OnFailed;
};