#pragma once

#include "Engine.h"

#include "XsollaPluginTransactionDetails.generated.h"

USTRUCT(BlueprintType)
struct FTransactionDetails
{
    GENERATED_BODY()

    // transaction section
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString TransactionStatus;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        int32 TransactionId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString TransactionProjectId;

    // payment section
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        int32 PaymentMethodId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString PaymentMethodName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        int32 PaymentAmount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString PaymentCurrency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        int32 PaymentSalesTaxPercent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        int32 PaymentSalesTaxAmount;

    // user section
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        int32 UserId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString UserEmail;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString UserCountry;

    // purchase section
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        int32 PurchaseVirtualCurrencyAmount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString PurchaseVirtualCurrencyName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString PurchaseVirtualItems;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        int32 PurchaseSimpleCheckoutAmount;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString PurchaseSimpleCheckoutCurrency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString PurchaseSubscriptionName;
};