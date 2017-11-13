// Fill out your copyright notice in the Description page of Project Settings.

#include "XsollaPluginSettings.h"

UXsollaPluginSettings::UXsollaPluginSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MerchantId("")
	, ProjectId("")
	, ApiKey("")
	, bSandboxMode(false)
{
}