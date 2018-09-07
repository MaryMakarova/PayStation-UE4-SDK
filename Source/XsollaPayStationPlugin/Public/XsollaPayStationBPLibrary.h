#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaPayStation.h"

#include "XsollaPayStationBPLibrary.generated.h"

UCLASS()
class UXsollaPayStationBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

public:
    /**
    * Creates shop wrapper, set default properties and delegates.
    *
    * @param shopSize - Size of shop page and wrapper.
    * @param userId - user ID.
    * @param onSucceeded - On payment succeeded delegate.
    * @param onClose - On payment canceled delegate.
    * @param onFailed - On payment failed delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "XsollaPayStation")
        static void OpenXsollaPayStation(
            EShopSizeEnum shopSize,
            FOnPaymentSucceeded onPaymentSucceeded,
            FOnPayStationClosed onPayStationClosed,
            FOnPaymentFailed onPaymentFailed);

    UFUNCTION(BlueprintCallable, Category = "XsollaPayStation")
        static void SetNumberProperty(FString prop, int value, bool bOverride = true);

    /**
    * Set bool property in token json.
    *
    * @param prop - Property name.
    * @param value - Bool value to set.
    * @param bOverride - Can the method overrides property value if exists.
    */
    UFUNCTION(BlueprintCallable, Category = "XsollaPayStation")
        static void SetBoolProperty(FString prop, bool value, bool bOverride = true);

    /**
    * Set string property in token json.
    *
    * @param prop - Property name.
    * @param value - String value to set.
    * @param bOverride - Can the method overrides property value if exists.
    */
    UFUNCTION(BlueprintCallable, Category = "XsollaPayStation")
        static void SetStringProperty(FString prop, FString value, bool bOverride = true);
};
