// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "XsollaPluginBPLibrary.h"
#include "XsollaPluginShop.h"

UXsollaPluginBPLibrary::UXsollaPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

int32 UXsollaPluginBPLibrary::CreateXsollaShop()
{
	UXsollaPluginShop* shop = NewObject<UXsollaPluginShop>(UXsollaPluginShop::StaticClass());

	shop->CreateShop();

	return -1;
}

