#include "XsollaPayStationBPLibrary.h"

#include "XsollaPayStation.h"

UXsollaPayStationBPLibrary::UXsollaPayStationBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UXsollaPayStationBPLibrary::OpenXsollaPayStation(EShopSizeEnum shopSize, FString userId, FOnPayStationClosed onPayStationClosed)
{
    if (!XsollaPayStationPlugin::Get()->bIsShopOpen)
    {
		XsollaPayStationPlugin::Get()->Create(shopSize, userId, onPayStationClosed);
    }
}