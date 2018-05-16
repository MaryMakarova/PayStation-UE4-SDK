#pragma once

#include "CoreMinimal.h"
#include "XsollaShop.h"

#include "XsollaSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class UXsollaPluginSettings : public UObject
{
    GENERATED_BODY()

public:
    UXsollaPluginSettings(const FObjectInitializer& objectInitializer);

    UPROPERTY(config, EditAnywhere)
        FString ServerUrl;

    UPROPERTY(config, EditAnywhere)
        EIntegrationType IntegrationType;

    UPROPERTY(config, EditAnywhere)
        FString ProjectId;

    UPROPERTY(config, EditAnywhere)
        bool bSandboxMode;

    UPROPERTY(config, EditAnywhere)
        float ButtonSize = 50.0f;
};