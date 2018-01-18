// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XsollaPluginShop.h"

#include "XsollaPluginSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class UXsollaPluginSettings : public UObject
{
    GENERATED_BODY()

public:
    UXsollaPluginSettings(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(config, EditAnywhere)
        FString ServerUrl;

    UPROPERTY(config, EditAnywhere)
        EIntegrationType IntegrationType;

    UPROPERTY(config, EditAnywhere)
        FString ProjectId;

    UPROPERTY(config, EditAnywhere)
        bool bSandboxMode;
};