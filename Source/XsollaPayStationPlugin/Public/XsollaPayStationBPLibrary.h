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
        static void OpenXsollaPayStation(EShopSizeEnum shopSize, FString userId, FOnPayStationClosed onPayStationClosed);
};
