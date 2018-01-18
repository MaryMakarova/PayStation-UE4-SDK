#include "XsollaPluginBPLibrary.h"

#include "XsollaPlugin.h"

UXsollaPluginBPLibrary::UXsollaPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UXsollaPluginBPLibrary::CreateXsollaShop(
    EShopSizeEnum shopSize,
    FString userId,
    FOnPaymantSucceeded OnSucceeded,
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    XsollaPlugin::GetShop()->Create(shopSize, userId, OnSucceeded, OnCanceled, OnFailed);
}

void UXsollaPluginBPLibrary::CreateXsollaShopWithToken(
    FString token,
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded OnSucceeded,
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    XsollaPlugin::GetShop()->CreateWithToken(token, shopSize, OnSucceeded, OnCanceled, OnFailed);
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

//void UXsollaPluginBPLibrary::GetTransactionHistoryForUser(FString userId, FOnTransactionsGetSucceeded onSucceeded, FOnTransactionsGetFailed onFailed)
//{
//
//}
