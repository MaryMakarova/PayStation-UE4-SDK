#include "XsollaPayStationBPLibrary.h"

#include "XsollaPayStation.h"

UXsollaPayStationBPLibrary::UXsollaPayStationBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UXsollaPayStationBPLibrary::OpenXsollaPayStation(EShopSizeEnum shopSize, FString userId, FOnPayStationClosed onPayStationClosed)
{
    if (!XsollaPayStationPlugin::GetShop()->bIsShopOpen)
    {
		XsollaPayStationPlugin::GetShop()->Create(shopSize, userId, onPayStationClosed);
    }
}