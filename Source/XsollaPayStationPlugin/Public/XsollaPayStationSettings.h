// Copyright 2019 Xsolla Inc. All Rights Reserved.
// <support@xsolla.com>
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
        FString ServerUrl;

    UPROPERTY(config, EditAnywhere)
        bool bSandboxMode;

    UPROPERTY(config, EditAnywhere)
        float ButtonSize = 50.0f;
};