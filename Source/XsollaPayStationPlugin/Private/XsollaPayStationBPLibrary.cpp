#include "XsollaPayStationBPLibrary.h"

#include "XsollaPayStation.h"

UXsollaPayStationBPLibrary::UXsollaPayStationBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UXsollaPayStationBPLibrary::OpenXsollaPayStation(
    EShopSizeEnum shopSize,
    FOnPaymentSucceeded onPaymentSucceeded,
    FOnPayStationClosed onPayStationClosed,
    FOnPaymentFailed onPaymentFailed)
{
    if (!XsollaPayStationPlugin::GetShop()->bIsShopOpen)
    {
		XsollaPayStationPlugin::GetShop()->Create(shopSize, onPaymentSucceeded, onPayStationClosed, onPaymentFailed);
    }
}

void UXsollaPayStationBPLibrary::SetNumberProperty(FString prop, int value, bool bOverride/*= true*/)
{
	XsollaPayStationPlugin::GetShop()->SetNumberProperty(prop, value, bOverride);
}

void UXsollaPayStationBPLibrary::SetBoolProperty(FString prop, bool value, bool bOverride/*= true*/)
{
	XsollaPayStationPlugin::GetShop()->SetBoolProperty(prop, value, bOverride);
}

void UXsollaPayStationBPLibrary::SetStringProperty(FString prop, FString value, bool bOverride/*= true*/)
{
	XsollaPayStationPlugin::GetShop()->SetStringProperty(prop, value, bOverride);
}
