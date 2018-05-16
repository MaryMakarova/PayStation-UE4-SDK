#include "XsollaBPLibrary.h"

#include "XsollaPlugin.h"

UXsollaBPLibrary::UXsollaBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UXsollaBPLibrary::CreateXsollaShop(
    EShopSizeEnum shopSize,
    FString userId,
    FOnPaymantSucceeded onSucceeded,
    FOnPaymantCanceled onCanceled,
    FOnPaymantFailed onFailed)
{
    XsollaPlugin::GetShop()->Create(shopSize, userId, onSucceeded, onCanceled, onFailed);
}

void UXsollaBPLibrary::CreateXsollaShopWithToken(
    FString token,
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded onSucceeded,
    FOnPaymantCanceled onCanceled,
    FOnPaymantFailed onFailed)
{
    XsollaPlugin::GetShop()->CreateWithToken(token, shopSize, onSucceeded, onCanceled, onFailed);
}

void UXsollaBPLibrary::SetNumberProperty(FString prop, int value, bool bOverride/*= true*/)
{
    XsollaPlugin::GetShop()->SetNumberProperty(prop, value, bOverride);
}

void UXsollaBPLibrary::SetBoolProperty(FString prop, bool value, bool bOverride/*= true*/)
{
    XsollaPlugin::GetShop()->SetBoolProperty(prop, value, bOverride);
}

void UXsollaBPLibrary::SetStringProperty(FString prop, FString value, bool bOverride/*= true*/)
{
    XsollaPlugin::GetShop()->SetStringProperty(prop, value, bOverride);
}
