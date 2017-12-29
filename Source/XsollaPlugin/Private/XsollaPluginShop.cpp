#include "XsollaPluginShop.h"

#include "XsollaPluginSettings.h"
#include "Misc/Base64.h"

UXsollaPluginShop::UXsollaPluginShop(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    HttpTool = new XsollaPluginHttpTool();
    TokenRequestJson = MakeShareable(new FJsonObject);
}

void UXsollaPluginShop::Create(
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded OnSucceeded, 
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    this->OnSucceeded = OnSucceeded;
    this->OnCanceled = OnCanceled;
    this->OnFailed = OnFailed;

    // load properties drom global game config
    GConfig->GetBool(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("bSandboxMode"), bIsSandbox, GGameIni);
    if (bIsSandbox)
    {
        ShopUrl = "https://sandbox-secure.xsolla.com/paystation2/?access_data=";
    }
    else
    {
        ShopUrl = "https://secure.xsolla.com/paystation2/?access_data=";
    }

    ProjectId = GetDefault<UXsollaPluginSettings>()->ProjectId;

    // show browser wrapper
    BrowserWrapper = CreateWidget<UXsollaPluginWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UXsollaPluginWebBrowserWrapper::StaticClass());

    // set shop interface size
    switch (shopSize)
    {
        case EShopSizeEnum::VE_Small:
        {
            SetStringProperty("settings.ui.size", FString("medium"), false);
            BrowserWrapper->SetBrowserSize(620, 630);
        }

        case EShopSizeEnum::VE_Medium:
        {
            SetStringProperty("settings.ui.size", FString("medium"), false);
            BrowserWrapper->SetBrowserSize(740, 760);
        }

        case EShopSizeEnum::VE_Large:
        {
            BrowserWrapper->SetBrowserSize(820, 840);
            SetStringProperty("settings.ui.size", FString("large"), false);
    }
    }

    BrowserWrapper->AddToViewport(9999);

    // shop delegates
    BrowserWrapper->OnSucceeded = OnSucceeded;
    BrowserWrapper->OnFailed = OnFailed;
    BrowserWrapper->OnCanceled = OnCanceled;

    // set token properties
    SetStringProperty("user.id.value", FString("12345"), false);
    //SetBoolProperty("user.id.hidden", true);

    SetStringProperty("user.email.value", FString("example@example.com"), false);
    //SetBoolProperty("user.email.hidden", false);

    //SetStringProperty("settings.return_url", FString("https://www.unrealengine.com"));
    SetNumberProperty("settings.project_id", FCString::Atoi(*ProjectId), false);
    if (bIsSandbox) 
        SetStringProperty("settings.mode", FString("sandbox"), false);

    SetStringProperty("settings.ui.version", FString("desktop"), false);
    SetStringProperty("settings.ui.theme", FString("default"), false);

    // get string from json
    FString outputString;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&outputString);
    FJsonSerializer::Serialize(TokenRequestJson.ToSharedRef(), Writer);

    if (!ShopUrl.EndsWith("="))
    {
        ShopUrl.Replace(*XsollaAccessString, *outputString);
    }
    else
    {
        ShopUrl += outputString;
    }

    XsollaAccessString = outputString;

    UE_LOG(LogTemp, Warning, TEXT("Link: %s"), *ShopUrl);

    BrowserWrapper->SetShopUrl(ShopUrl);
    BrowserWrapper->LoadURL(ShopUrl);
}

void UXsollaPluginShop::CreateWithToken(
    FString token,
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded OnSucceeded,
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    this->OnSucceeded = OnSucceeded;
    this->OnCanceled = OnCanceled;
    this->OnFailed = OnFailed;

    // load properties drom global game config
    GConfig->GetBool(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("bSandboxMode"), bIsSandbox, GGameIni);
    if (bIsSandbox)
    {
        ShopUrl = "https://sandbox-secure.xsolla.com/paystation2/?access_token=";
    }
    else
    {
        ShopUrl = "https://secure.xsolla.com/paystation2/?access_token=";
    }

    GConfig->GetString(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("ProjectId"), ProjectId, GGameIni);

    // show browser wrapper
    BrowserWrapper = CreateWidget<UXsollaPluginWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UXsollaPluginWebBrowserWrapper::StaticClass());

    // set shop interface size
    switch (shopSize)
    {
        case EShopSizeEnum::VE_Small:
            BrowserWrapper->SetBrowserSize(620, 630);
        case EShopSizeEnum::VE_Medium:
            BrowserWrapper->SetBrowserSize(740, 760);
        case EShopSizeEnum::VE_Large:
            BrowserWrapper->SetBrowserSize(820, 840);
    }

    BrowserWrapper->AddToViewport(9999);

    if (!ShopUrl.EndsWith("="))
    {
        ShopUrl.Replace(*XsollaToken, *token);
    }
    else
    {
        ShopUrl += token;
    }

    XsollaToken = token;

    BrowserWrapper->SetShopUrl(ShopUrl);
    BrowserWrapper->LoadURL(ShopUrl);
}

void UXsollaPluginShop::SetNumberProperty(FString prop, int value, bool bOverride/*= true */)
{
    TArray<FString> subStrings;
    TSharedPtr<FJsonObject> currentObj = TokenRequestJson.ToSharedRef();

    prop.ParseIntoArray(subStrings, TEXT("."));

    for (int i = 0; i < subStrings.Num(); ++i)
    {
        if (i + 1 == subStrings.Num())
        {
            if (currentObj->HasField(subStrings.Last()))
            {
                if (bOverride)
                    currentObj->SetNumberField(subStrings.Last(), value);
                else
                    continue;
            }
            else
            {
                currentObj->SetNumberField(subStrings.Last(), value);
            }

            continue;
        }

        if (!currentObj->HasField(subStrings[i]))
        {
            TSharedPtr<FJsonObject> subObject = MakeShareable(new FJsonObject);
            currentObj->SetObjectField(subStrings[i], subObject.ToSharedRef());
        }

        currentObj = currentObj->GetObjectField(subStrings[i]).ToSharedRef();
    }
}

void UXsollaPluginShop::SetBoolProperty(FString prop, bool value, bool bOverride/*= true */)
{
    TArray<FString> subStrings;
    TSharedPtr<FJsonObject> currentObj = TokenRequestJson.ToSharedRef();

    prop.ParseIntoArray(subStrings, TEXT("."));

    for (int i = 0; i < subStrings.Num(); ++i)
    {
        if (i + 1 == subStrings.Num())
        {
            if (currentObj->HasField(subStrings.Last()))
            {
                if (bOverride)
                    currentObj->SetBoolField(subStrings.Last(), value);
                else
                    continue;
            }
            else
            {
                currentObj->SetBoolField(subStrings.Last(), value);
            }

            continue;
        }

        if (!currentObj->HasField(subStrings[i]))
        {
            TSharedPtr<FJsonObject> subObject = MakeShareable(new FJsonObject);
            currentObj->SetObjectField(subStrings[i], subObject.ToSharedRef());
        }

        currentObj = currentObj->GetObjectField(subStrings[i]).ToSharedRef();
    }
}

void UXsollaPluginShop::SetStringProperty(FString prop, FString value, bool bOverride/*= true */)
{
    TArray<FString> subStrings;
    TSharedPtr<FJsonObject> currentObj = TokenRequestJson.ToSharedRef();

    prop.ParseIntoArray(subStrings, TEXT("."));

    for (int i = 0; i < subStrings.Num(); ++i)
    {
        if (i + 1 == subStrings.Num())
        {
            if (currentObj->HasField(subStrings.Last()))
            {
                if (bOverride)
                    currentObj->SetStringField(subStrings.Last(), value);
                else
                    continue;
            }
            else
            {
                currentObj->SetStringField(subStrings.Last(), value);
            }

            continue;
        }

        if (!currentObj->HasField(subStrings[i]))
        {
            TSharedPtr<FJsonObject> subObject = MakeShareable(new FJsonObject);
            currentObj->SetObjectField(subStrings[i], subObject.ToSharedRef());
        }

        currentObj = currentObj->GetObjectField(subStrings[i]).ToSharedRef();
    }
}
