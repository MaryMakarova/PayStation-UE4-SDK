#pragma once

#include "XsollaHttpTool.h"
#include "WebBrowserWrapper.h"

#include "XsollaPayStation.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnPayStationClosed);

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
	* @param onClose - On payment canceled delegate.
    */
    void Create(EShopSizeEnum shopSize, FString userId, FOnPayStationClosed onClose);

	/**
	* Open shop in the system default web brower. 
	* Should be used on macOS 10.14 (Mojave), because CEF causing application crash
	*
	* @param userId - user ID
	*/
#if PLATFORM_MAC
	void CreateInSystemDefaultBrowser(FString userId);
#endif

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

    FString ApiUrl          = "https://secure.xsolla.com/paystation2";
    FString SandboxApiUrl   = "https://sandbox-secure.xsolla.com/paystation2";

    FString ShopUrl;
    
    FString XsollaToken;
    bool bIsSandbox;

    FOnPayStationClosed onClose;
};