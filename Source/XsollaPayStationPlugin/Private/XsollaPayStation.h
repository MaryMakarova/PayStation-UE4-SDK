#pragma once

#include "XsollaHttpTool.h"
#include "WebBrowserWrapper.h"

#include "XsollaPayStation.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPaymentSucceeded, FTransactionDetails, transactionDetails);
DECLARE_DYNAMIC_DELEGATE(FOnPayStationClosed);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPaymentFailed, FString, errorText);

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
class XSOLLAPAYSTATIONPLUGIN_API UXsollaPayStation : public UObject
{
    friend class UWebBrowserWrapper;

    GENERATED_UCLASS_BODY()

public:
    /**
    * Creates shop wrapper, set default properties and delegates.
    *
    * @param shopSize - Size of shop page and wrapper.
    * @param userId - user ID
    * @param onSucceeded - On payment succeeded delegate.
    * @param onClose - On payment canceled delegate.
    * @param onFailed - On payment failed delegate.
    */
    void Create(
        EShopSizeEnum shopSize,
        FOnPaymentSucceeded onSucceeded,
        FOnPayStationClosed onClose,
        FOnPaymentFailed onFailed);
    
    /**
     * Set number property in token json.
     * 
     * @param prop - Property name.
     * @param value - Int value to set.
     * @param bOverride - Can the method overrides property value if exists.
     */
    void SetNumberProperty(FString prop, int value, bool bOverride = true);

    /**
    * Set bool property in token json.
    *
    * @param prop - Property name.
    * @param value - Bool value to set.
    * @param bOverride - Can the method overrides property value if exists.
    */
    void SetBoolProperty(FString prop, bool value, bool bOverride = true);

    /**
    * Set string property in token json.
    *
    * @param prop - Property name.
    * @param value - String value to set.
    * @param bOverride - Can the method overrides property value if exists.
    */
    void SetStringProperty(FString prop, FString value, bool bOverride = true);

private:
    void LoadConfig();
    void SetToken(FString token);
    void OnShopClosed();

public:
    bool bIsShopOpen = false;

private:
    XsollaPayStationHttpTool * HttpTool;

    UPROPERTY()
    UWebBrowserWrapper* BrowserWrapper;

    FString ApiUrl          = "https://secure.xsolla.com/paystation3";
    FString SandboxApiUrl   = "https://sandbox-secure.xsolla.com/paystation3";

    FString ShopUrl;
    FString GetTokenUrl;
	FString GetPaymentStatusUrl;
    
    FString XsollaToken;
    FString ExternalId;
    bool bIsSandbox;

    TSharedPtr<FJsonObject> TokenRequestJson;

    FOnPaymentSucceeded OnSucceeded;
    FOnPayStationClosed onClose;
    FOnPaymentFailed OnFailed;
};