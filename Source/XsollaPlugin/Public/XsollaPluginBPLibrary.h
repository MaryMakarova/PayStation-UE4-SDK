#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "XsollaPluginShop.h"

#include "XsollaPluginBPLibrary.generated.h"

UCLASS()
class UXsollaPluginBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

public:

    /**
    * Creates shop wrapper, set default properties and delegates.
    *
    * @param shopSize - Size of shop page and wrapper.
    * @param OnSucceeded - On payment succeeded delegate.
    * @param OnCanceled - On payment canceled delegate.
    * @param OnFailed - On payment failed delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
        static void CreateXsollaShop(
            EShopSizeEnum shopSize,
            FOnPaymantSucceeded OnSucceeded,
            FOnPaymantCanceled OnCanceled,
            FOnPaymantFailed OnFailed);

    /**
    * Creates shop wrapper, set default properties and delegates.
    *
    * @param token - Xsolla shop token.
    * @param OnSucceeded - On payment succeeded delegate.
    * @param OnCanceled - On payment canceled delegate.
    * @param OnFailed - On payment failed delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
        static void CreateXsollaShopWithToken(
            FString token,
            FString externalId,
            EShopSizeEnum shopSize,
            FOnPaymantSucceeded OnSucceeded,
            FOnPaymantCanceled OnCanceled,
            FOnPaymantFailed OnFailed);

    UFUNCTION(BlueprintCallable, Category = "Xsolla")
        static void SetNumberProperty(FString prop, int value, bool bOverride = true);

    /**
    * Set bool property in token json.
    *
    * @param prop - Property name.
    * @param value - Bool value to set.
    * @param bOverride - Can the method overrides property value if exists.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
        static void SetBoolProperty(FString prop, bool value, bool bOverride = true);

    /**
    * Set string property in token json.
    *
    * @param prop - Property name.
    * @param value - String value to set.
    * @param bOverride - Can the method overrides property value if exists.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
        static void SetStringProperty(FString prop, FString value, bool bOverride = true);

    /**
    * Returns transaction history for user to callback on succeed.
    *
    * @param userId - User ID.
    * @param onSucceeded - On succeded delegate.
    * @param onFailed - On failed delegate.
    */
    UFUNCTION(BlueprintCallable, Category = "Xsolla")
        static void GetTransactionHistoryForUser(FString userId, FOnTransactionsGetSucceeded onSucceeded, FOnTransactionsGetFailed onFailed);
};
