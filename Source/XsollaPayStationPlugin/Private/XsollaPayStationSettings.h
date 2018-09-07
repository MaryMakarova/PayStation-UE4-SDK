#pragma once

#include "CoreMinimal.h"
#include "XsollaPayStation.h"

#include "XsollaPayStationSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class UXsollaPayStationSettings : public UObject
{
    GENERATED_BODY()

public:
    UXsollaPayStationSettings(const FObjectInitializer& objectInitializer);

    UPROPERTY(config, EditAnywhere)
        FString GetTokenUrl;

	UPROPERTY(config, EditAnywhere)
		FString GetPaymentStatusUrl;

    UPROPERTY(config, EditAnywhere)
        bool bSandboxMode;

    UPROPERTY(config, EditAnywhere)
        float ButtonSize = 50.0f;
};