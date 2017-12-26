// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XsollaPluginEncryptTool.h"

#include "XsollaPluginSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class UXsollaPluginSettings : public UObject
{
    GENERATED_BODY()

public:
    UXsollaPluginSettings(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(config, EditAnywhere, Category = API)
        FString MerchantId;

    UPROPERTY(config, EditAnywhere, Category = API)
        FString ProjectId;

    UPROPERTY(config /*EditAnywhere,*/ /*Category = API*/)
        FString ApiKey;

    UPROPERTY(/*config*/ EditAnywhere, Category = API)
        FString ApiKeyDecrypted;

    UPROPERTY(config, EditAnywhere, Category = API)
        bool bSandboxMode;

    virtual void PostInitProperties() override
    {
        Super::PostInitProperties();

        ApiKeyDecrypted = XsollaPluginEncryptTool::DecryptString(ApiKey);
    }

    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
    {
        if (PropertyChangedEvent.Property->GetFName().ToString().Compare("ApiKeyDecrypted") == 0)
        {
            FString encryptedString;
            FString stringToEncrypt = ApiKeyDecrypted;
        
            encryptedString = XsollaPluginEncryptTool::EncryptString(stringToEncrypt);
            ApiKey = encryptedString;

            GConfig->SetString(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("ApiKey"), *encryptedString, GGameIni);
            GConfig->Flush(false);
        }
    }
};