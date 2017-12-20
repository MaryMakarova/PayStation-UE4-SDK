#include "XsollaPluginBPLibrary.h"
#include "XsollaPluginShop.h"

UXsollaPluginBPLibrary::UXsollaPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

int32 UXsollaPluginBPLibrary::CreateXsollaShop(
	FOnPaymantSucceeded OnSucceeded, 
	FOnPaymantCanceled OnCanceled, 
	FOnPaymantFailed OnFailed, 
	FString shopSize)
{
	UXsollaPluginShop* shop = NewObject<UXsollaPluginShop>(UXsollaPluginShop::StaticClass());

	shop->CreateShop(shopSize, OnSucceeded, OnCanceled, OnFailed);

	return -1;
}

