// Fill out your copyright notice in the Description page of Project Settings.

#include "XsollaPluginSettings.h"

UXsollaPluginSettings::UXsollaPluginSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UXsollaPluginSettings::PostInitProperties()
{
    Super::PostInitProperties();

    ApiKeyDecrypted = XsollaPluginEncryptTool::DecryptString(ApiKey);
}

void UXsollaPluginSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    //Super::PostEditChangeProperty(PropertyChangedEvent);

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