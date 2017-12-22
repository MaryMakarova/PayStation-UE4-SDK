#include "XsollaPluginShop.h"
#include "XsollaPluginSettings.h"
#include "Misc/Base64.h"
#include "BigInt.h"

UXsollaPluginShop::UXsollaPluginShop(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    HttpTool = new XsollaPluginHttpTool();
    TokenRequestJson = MakeShareable(new FJsonObject);

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

    GConfig->GetString(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("MerchantId"), MerchantId, GGameIni);
    GConfig->GetString(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("ProjectId"), ProjectId, GGameIni);
    GConfig->GetString(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("ApiKey"), ApiKey, GGameIni);
}

void UXsollaPluginShop::CreateShop(
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded OnSucceeded, 
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    this->OnSucceeded = OnSucceeded;
    this->OnCanceled = OnCanceled;
    this->OnFailed = OnFailed;

    // show browser wrapper
    BrowserWrapper = CreateWidget<UXsollaPluginWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UXsollaPluginWebBrowserWrapper::StaticClass());

    // set shop interface size
    if (shopSize == EShopSizeEnum::VE_Small)
    {
        SetStringProperty("settings.ui.size", FString("small"), false);
        BrowserWrapper->SetBrowserSize(620, 630);
    }
    else if (shopSize == EShopSizeEnum::VE_Medium)
    {
        SetStringProperty("settings.ui.size", FString("medium"), false);
        BrowserWrapper->SetBrowserSize(740, 760);
    }
    else if (shopSize == EShopSizeEnum::VE_Large)
    {
        SetStringProperty("settings.ui.size", FString("large"), false);
        BrowserWrapper->SetBrowserSize(820, 840);
    }

    BrowserWrapper->AddToViewport(9999);

    // shop delegates
    BrowserWrapper->OnSucceeded = OnSucceeded;
    BrowserWrapper->OnFailed = OnFailed;
    BrowserWrapper->OnCanceled = OnCanceled;

    // set token properties
    SetStringProperty("user.id.value", FString("12345678"), false);
    SetBoolProperty("user.id.hidden", true);

    SetStringProperty("user.email.value", FString("example@example.com"), false);
    SetBoolProperty("user.email.allow_modify", true);
    SetBoolProperty("user.email.hidden", false);

    ExternalId = FBase64::Encode(FString::SanitizeFloat(GEngine->GameViewport->GetWorld()->GetRealTimeSeconds() + FMath::Rand()));

    SetStringProperty("settings.external_id", ExternalId);
    SetStringProperty("settings.return_url", FString("https://www.unrealengine.com"));
    SetNumberProperty("settings.project_id", FCString::Atoi(*ProjectId), false);
    if (bIsSandbox) 
        SetStringProperty("settings.mode", FString("sandbox"), false);

    SetStringProperty("settings.ui.version", FString("desktop"), false);
    SetStringProperty("settings.ui.theme", FString("default"), false);

    // get string from json
    FString outputString;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&outputString);
    FJsonSerializer::Serialize(TokenRequestJson.ToSharedRef(), Writer);

    UE_LOG(LogTemp, Warning, TEXT("JSON: %s"), *outputString);

    // load shop
    OpenShop(outputString);
}

void UXsollaPluginShop::OpenShop(FString tokenRequestJson)
{
    FString route = "https://api.xsolla.com/merchant/merchants/";
    route += MerchantId;
    route += "/token";

    TSharedRef<IHttpRequest> Request = HttpTool->PostRequest(route, tokenRequestJson);

    Request->OnProcessRequestComplete().BindUObject(this, &UXsollaPluginShop::OnGetTokenRequestComplete);
    HttpTool->SetAuthorizationHash(FString("Basic ") + FBase64::Encode(MerchantId + FString(":") + XsollaPluginEncryptTool::DecryptString(ApiKey)), Request);

    HttpTool->Send(Request);
}

void UXsollaPluginShop::OnGetTokenRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!HttpTool->ResponseIsValid(Response, bWasSuccessful))
    {
        OnCanceled.Execute(Response->GetResponseCode());
        return;
    }

    TSharedPtr<FJsonObject> JsonParsed;
    TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());

    if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
    {
        if ( !ShopUrl.EndsWith("=") )
        {
            ShopUrl.Replace(*XsollaToken, *(JsonParsed->GetStringField("token")));
        }
        else
        {
            ShopUrl += JsonParsed->GetStringField("token");
        }

        XsollaToken = JsonParsed->GetStringField("token");

        BrowserWrapper->SetExternalId(ExternalId);
        BrowserWrapper->SetShopUrl(ShopUrl);
        BrowserWrapper->LoadURL(ShopUrl);
    }
}

bool UXsollaPluginShop::SetNumberProperty(FString prop, int value, bool bOverride/*= true */)
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

    return true;
}

bool UXsollaPluginShop::SetBoolProperty(FString prop, bool value, bool bOverride/*= true */)
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

    return true;
}

bool UXsollaPluginShop::SetStringProperty(FString prop, FString value, bool bOverride/*= true */)
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

    return true;
}
