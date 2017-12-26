#include "XsollaPluginBPLibrary.h"

#include "XsollaPlugin.h"

UXsollaPluginBPLibrary::UXsollaPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UXsollaPluginBPLibrary::CreateXsollaShop(
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded OnSucceeded,
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    XsollaPlugin::GetShop()->Create(shopSize, OnSucceeded, OnCanceled, OnFailed);
}

void UXsollaPluginBPLibrary::CreateXsollaShopWithToken(
    FString token,
    FString externalId,
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded OnSucceeded,
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    XsollaPlugin::GetShop()->CreateWithToken(token, externalId, shopSize, OnSucceeded, OnCanceled, OnFailed);
}

void UXsollaPluginBPLibrary::SetNumberProperty(FString prop, int value, bool bOverride/*= true*/)
{
    XsollaPlugin::GetShop()->SetNumberProperty(prop, value, bOverride);
}

void UXsollaPluginBPLibrary::SetBoolProperty(FString prop, bool value, bool bOverride/*= true*/)
{
    XsollaPlugin::GetShop()->SetBoolProperty(prop, value, bOverride);
}

void UXsollaPluginBPLibrary::SetStringProperty(FString prop, FString value, bool bOverride/*= true*/)
{
    XsollaPlugin::GetShop()->SetStringProperty(prop, value, bOverride);
}

void UXsollaPluginBPLibrary::GetTransactionHistoryForUser(FString userId, FOnTransactionsGetSucceeded onSucceeded, FOnTransactionsGetFailed onFailed)
{

}
