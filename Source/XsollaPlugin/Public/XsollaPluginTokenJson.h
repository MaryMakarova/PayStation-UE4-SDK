#pragma once

#include "CoreMinimal.h"

#include "XsollaPluginTokenJson.generated.h"

/* user.id */
USTRUCT()
struct FTokenUserId
{
	GENERATED_BODY()

	UPROPERTY() FString value;
	UPROPERTY() bool hidden;
	UPROPERTY() bool allow_modify;
};

/* user.name */
USTRUCT()
struct FTokenUserName
{
	GENERATED_BODY()

	UPROPERTY() FString value;
	UPROPERTY() bool hidden;
	UPROPERTY() bool allow_modify;
};

/* user.email */
USTRUCT()
struct FTokenUserEmail
{
	GENERATED_BODY()

	UPROPERTY() FString value;
	UPROPERTY() bool hidden;
	UPROPERTY() bool allow_modify;
};

/* user.phone */
USTRUCT()
struct FTokenUserPhone
{
	GENERATED_BODY()

	UPROPERTY() FString value;
	UPROPERTY() bool hidden;
	UPROPERTY() bool allow_modify;
};

/* user.country */
USTRUCT()
struct FTokenUserCountry
{
	GENERATED_BODY()

	UPROPERTY() FString value;
	UPROPERTY() bool allow_modify;
};

/* user.steam_id */
USTRUCT()
struct FTokenUserSteamId
{
	GENERATED_BODY()

	UPROPERTY() FString value;
};

/* user.tracking_id */
USTRUCT()
struct FTokenUserTrackingId
{
	GENERATED_BODY()

	UPROPERTY() FString value;
};

/* user.utm */
USTRUCT()
struct FTokenUserUtm
{
	GENERATED_BODY()

	UPROPERTY() FString utm_source;
	UPROPERTY() FString utm_medium;
	UPROPERTY() FString utm_campaign;
	UPROPERTY() FString utm_term;
	UPROPERTY() FString utm_content;
};

/* user */
USTRUCT()
struct FTokenUser
{
	GENERATED_BODY()

	UPROPERTY() FTokenUserId id;
	UPROPERTY() FTokenUserName name;
	UPROPERTY() FTokenUserEmail email;
	UPROPERTY() FTokenUserPhone  phone;
	UPROPERTY() FTokenUserCountry country;
	UPROPERTY() FTokenUserSteamId steam_id;
	UPROPERTY() FTokenUserTrackingId tracking_id;
	UPROPERTY() FTokenUserUtm utm;
};

/* settings.ui.desktop.header */
USTRUCT()
struct FTokenHeader
{
	GENERATED_BODY()

	UPROPERTY() bool is_visible;
	UPROPERTY() bool visible_logo;
	UPROPERTY() bool visible_name;
	UPROPERTY() FString type;
};

/* settings.ui.desktop.subscription_list */
USTRUCT()
struct FTokenSubscriptionList
{
	GENERATED_BODY()

	UPROPERTY() FString layout;
	UPROPERTY() FString description;
	UPROPERTY() FString display_local_price;
};

/* settings.ui.desktop.virtual_items_list */
USTRUCT()
struct FTokenVirtualItemsList
{
	GENERATED_BODY()

	UPROPERTY() FString layout;
	UPROPERTY() bool button_with_price;
	UPROPERTY() FString view;
};

/* settings.ui.desktop.virtual_currency_list */
USTRUCT()
struct FTokenVirtualCurrencyList
{
	GENERATED_BODY()

	UPROPERTY() FString description;
	UPROPERTY() bool button_with_price;
	UPROPERTY() FString view;
};

/* settings.ui.desktop */
USTRUCT()
struct FTokenDesktop
{
	GENERATED_BODY()

	UPROPERTY() FString size;
	UPROPERTY() FString theme;
	UPROPERTY() FString version;
	UPROPERTY() FTokenHeader header;
	UPROPERTY() FTokenSubscriptionList subscription_list;
	UPROPERTY() FTokenVirtualItemsList virtual_items_list;
	UPROPERTY() FTokenVirtualCurrencyList virtual_currency_list;
};

/* settings.ui.components.virtual_items */
USTRUCT()
struct FTokenVirtualItems
{
	GENERATED_BODY()

	UPROPERTY() int order;
	UPROPERTY() bool hidden;
	UPROPERTY() FString selected_group;
	UPROPERTY() FString selected_item;
};

/* settings.ui.components.virtual_currency */
USTRUCT()
struct FTokenVirtualCurrency
{
	GENERATED_BODY()

	UPROPERTY() bool custom_amount;
	UPROPERTY() int order;
	UPROPERTY() bool hidden;
};

/* settings.ui.components.subscriptions */
USTRUCT()
struct FTokenSubscriptions
{
	GENERATED_BODY()

	UPROPERTY() int order;
	UPROPERTY() bool hidden;
};

/* settings.ui.components */
USTRUCT()
struct FTokenComponents
{
	GENERATED_BODY()

	UPROPERTY() FTokenVirtualItems virtual_items;
	UPROPERTY() FTokenVirtualCurrency virtual_currency;
	UPROPERTY() FTokenSubscriptions subscriptions;
};

/* settings.ui.user_account.info */
USTRUCT()
struct FTokenUserAccountInfo
{
	GENERATED_BODY()

	UPROPERTY() int order;
	UPROPERTY() bool enable;
};

/* settings.ui.user_account.history */
USTRUCT()
struct FTokenUserAccountHistory
{
	GENERATED_BODY()

	UPROPERTY() int order;
	UPROPERTY() bool enable;
};

/* settings.ui.user_account.payment_accounts */
USTRUCT()
struct FTokenUserAccountPaymentAccounts
{
	GENERATED_BODY()

	UPROPERTY() int order;
	UPROPERTY() bool enable;
};

/* settings.ui.user_account.subscriptions */
USTRUCT()
struct FTokenUserAccountSubscriptions
{
	GENERATED_BODY()

	UPROPERTY() int order;
	UPROPERTY() bool enable;
};

/* settings.ui.user_account */
USTRUCT()
struct FTokenUserAccount
{
	GENERATED_BODY()

	UPROPERTY() FTokenUserAccountInfo info;
	UPROPERTY() FTokenUserAccountHistory history;
	UPROPERTY() FTokenUserAccountPaymentAccounts payment_accounts;
	UPROPERTY() FTokenUserAccountSubscriptions subscriptions;
};

/* settings.ui */
USTRUCT()
struct FTokenUi
{
	GENERATED_BODY()

	UPROPERTY() FString size;
	UPROPERTY() FString theme;
	UPROPERTY() FString version;
	UPROPERTY() FTokenDesktop desktop;
	UPROPERTY() FString license_url;
	UPROPERTY() FTokenComponents components;
	UPROPERTY() FString mode;
	UPROPERTY() FTokenUserAccount user_account;
};

/* settings */
USTRUCT()
struct FTokenSettings
{
	GENERATED_BODY()

	UPROPERTY() FString external_id;
	UPROPERTY() int project_id;
	UPROPERTY() FString language;
	UPROPERTY() FString return_url;
	UPROPERTY() FString currency;
	UPROPERTY() FString mode;
	UPROPERTY() int payment_method;
	UPROPERTY() FString payment_widget;
	UPROPERTY() FTokenUi ui;
	UPROPERTY() bool shipping_enabled;
};

/* purchase.virtual_currency */
USTRUCT()
struct FTokenPurchaseVirtualCurrency
{
	GENERATED_BODY()

	UPROPERTY() float quantity;
	UPROPERTY() FString currency;
};

/* purchase.virtual_items.items.{item} */
USTRUCT()
struct FTokenPurchaseVirtualItemsArrayItem
{
	GENERATED_BODY()

	UPROPERTY() FString sku;
	UPROPERTY() int amount;
};

/* purchase.virtual_items */
USTRUCT()
struct FTokenPurchaseVirtualItems
{
	GENERATED_BODY()

	UPROPERTY() FString currency;
	UPROPERTY() TArray<FTokenPurchaseVirtualItemsArrayItem> items;
	// UPROPERTY() TArray<FTokenPurchaseVirtualItemsArrayItem> available_groups;
};

/* purchase.subscription */
USTRUCT()
struct FTokenPurchaseSubscription
{
	GENERATED_BODY()

	UPROPERTY() FString plan_id;
	UPROPERTY() FString product_id;
	UPROPERTY() FString currency;
	UPROPERTY() int trial_days;
};

/* purchase.pin_codes.codes.{item} */
USTRUCT()
struct FTokenPurchasePinCodesItem
{
	GENERATED_BODY()

	UPROPERTY() FString digital_content;
	UPROPERTY() FString drm;
};

/* purchase.pin_codes */
USTRUCT()
struct FTokenPurchasePinCodes
{
	GENERATED_BODY()

	UPROPERTY() FString currency;
	UPROPERTY() TArray<FTokenPurchasePinCodesItem> codes;
};

/* purchase.checkout */
USTRUCT()
struct FTokenPurchaseCheckout
{
	GENERATED_BODY()

	UPROPERTY() FString currency;
	UPROPERTY() float amount;
};

/* purchase.description */
USTRUCT()
struct FTokenPurchaseDescription
{
	GENERATED_BODY()

	UPROPERTY() FString value;
};

/* purchase */
USTRUCT()
struct FTokenPurchase
{
	GENERATED_BODY()

	UPROPERTY() FTokenPurchaseVirtualCurrency virtual_currency;
	UPROPERTY() FTokenPurchaseVirtualItems virtual_items;
	UPROPERTY() FTokenPurchaseSubscription subscription;
	UPROPERTY() FTokenPurchasePinCodes pin_codes;
	UPROPERTY() FTokenPurchaseCheckout checkout;
	UPROPERTY() FTokenPurchaseDescription description;
};

USTRUCT()
struct FTokenXsollaPluginTokenJson
{
	GENERATED_BODY()

	UPROPERTY() FTokenUser user;
	UPROPERTY() FTokenSettings settings;
	UPROPERTY() FTokenPurchase purchase;
};