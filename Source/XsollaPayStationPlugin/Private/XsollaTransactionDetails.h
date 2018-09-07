#pragma once

#include "CoreMinimal.h"

#include "XsollaTransactionDetails.generated.h"

USTRUCT(BlueprintType)
struct FVirtualCurrency
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "virtual_currency")
        FString name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "virtual_currency")
        FString sku;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "virtual_currency")
        float quantity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "virtual_currency")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "virtual_currency")
        float amount;
};

USTRUCT(BlueprintType)
struct FSubscription
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "subscription")
        FString plan_id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "subscription")
        int subscription_id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "subscription")
        FString product_id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "subscription")
        FString date_create;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "subscription")
        FString date_next_charge;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "subscription")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "subscription")
        float amount;
};

USTRUCT(BlueprintType)
struct FCheckout
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "checkout")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "checkout")
        float amount;
};

USTRUCT(BlueprintType)
struct FItem
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "item")
        FString sku;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "item")
        float amount;
};

USTRUCT(BlueprintType)
struct FVirtualItems
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "virtual_items")
        TArray<FItem> items;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "virtual_items")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "virtual_items")
        float amount;
};

USTRUCT(BlueprintType)
struct FGift
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "gift")
        FString giver_ID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "gift")
        FString receiver_ID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "gift")
        FString receiver_email;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "gift")
        FString message;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "gift")
        bool hide_giver_from_receiver;
};

USTRUCT(BlueprintType)
struct FTotal
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "total")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "total")
        float amount;
};

USTRUCT(BlueprintType)
struct FPromotion
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "promotion")
        FString technical_name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "promotion")
        FString id;
};

USTRUCT(BlueprintType)
struct FCoupon
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "coupon")
        FString coupon_code;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "coupon")
        FString campaign_code;
};

USTRUCT(BlueprintType)
struct FPurchase
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "purchase")
        FVirtualCurrency virtual_currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "purchase")
        FSubscription subscription;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "purchase")
        FCheckout checkout;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "purchase")
        FVirtualItems virtual_items;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "purchase")
        FGift gift;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "purchase")
        FTotal total;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "purchase")
        TArray<FPromotion> promotions;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "purchase")
        FCoupon coupon;
};

USTRUCT(BlueprintType)
struct FUser
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "user")
        FString ip;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "user")
        FString phone;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "user")
        FString email;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "user")
        FString id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "user")
        FString name;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "user")
        FString country;
};

USTRUCT(BlueprintType)
struct FTransaction
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "transaction")
        int id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "transaction")
        FString external_id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "transaction")
        FString payment_date;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "transaction")
        int payment_method;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "transaction")
        int dry_run;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "transaction")
        int agreement;
};

USTRUCT(BlueprintType)
struct FPayment
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_details")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_details")
        float amount;
};

USTRUCT(BlueprintType)
struct FVat
{
    GENERATED_BODY()
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "vat")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "vat")
        float amount;
};

USTRUCT(BlueprintType)
struct FPayout
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payout")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payout")
        float amount;
};

USTRUCT(BlueprintType)
struct FXsollaFee
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "xsolla_fee")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "xsolla_fee")
        float amount;
};

USTRUCT(BlueprintType)
struct FPaymentMethodFee
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_method_fee")
        FString currency;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_method_fee")
        float amount;
};

USTRUCT(BlueprintType)
struct FPaymentDetails
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_details")
        FPayment payment;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_details")
        FVat vat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_details")
        float payout_currency_rate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_details")
        FPayout payout;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_details")
        FXsollaFee xsolla_fee;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "payment_details")
        FPaymentMethodFee payment_method_fee;
};

USTRUCT(BlueprintType)
struct FTransactionDetails
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FString notification_type;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FPurchase purchase;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FUser user;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FTransaction transaction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transaction details")
        FPaymentDetails payment_details;
};