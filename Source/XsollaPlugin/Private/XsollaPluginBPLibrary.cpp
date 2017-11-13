// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "XsollaPluginBPLibrary.h"
#include "XsollaPlugin.h"
#include "XsollaPluginWebBrowser.h"
#include "Runtime/UMG/Public/UMG.h"

UXsollaPluginBPLibrary::UXsollaPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

int32 UXsollaPluginBPLibrary::CreateXsollaShop(FOnPaymantSucceeded OnSucceeded, FOnPaymantCanceled OnCanceled, FOnPaymantFailed OnFailed)
{
	UXsollaPluginWebBrowser * browser = CreateWidget<UXsollaPluginWebBrowser>(GEngine->GetWorld(), UXsollaPluginWebBrowser::StaticClass());
	browser->GetToken();
	browser->AddToViewport();

	return -1;
}

