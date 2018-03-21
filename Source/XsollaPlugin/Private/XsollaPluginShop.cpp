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
    FString userId,
    FOnPaymantSucceeded OnSucceeded, 
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    // show browser wrapper
    BrowserWrapper = CreateWidget<UXsollaPluginWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UXsollaPluginWebBrowserWrapper::StaticClass());

    // set delegates
    this->OnSucceeded = OnSucceeded;
    this->OnCanceled = OnCanceled;
    this->OnFailed = OnFailed;

    BrowserWrapper->OnShopClosed.BindLambda([this]() { this->OnShopClosed(); return; });

    // load data from config
    LoadConfig(GetDefault<UXsollaPluginSettings>()->IntegrationType);

    // generate external_id
    ExternalId = FBase64::Encode(FString::SanitizeFloat(GEngine->GameViewport->GetWorld()->GetRealTimeSeconds() + FMath::Rand()));
    SetStringProperty("settings.external_id", ExternalId);

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

    SetStringProperty("user.id.value", userId);

    SetDefaultTokenProperties();

    BrowserWrapper->AddToViewport(9999);

    if (GetDefault<UXsollaPluginSettings>()->IntegrationType == EIntegrationType::VE_SERVER)
    {
        // set return url for "back to game" button handling
        SetStringProperty("settings.return_url", "https://www.unrealengine.com");

        // get string from json
        FString outputString;
        TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&outputString);
        FJsonSerializer::Serialize(TokenRequestJson.ToSharedRef(), Writer);

        UE_LOG(LogTemp, Warning, TEXT("Token JSON: %s"), *outputString);

        TSharedRef<IHttpRequest> request = HttpTool->PostRequest(ServerUrl + FString("/token"), outputString);
        request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
            if (bWasSuccessful) {
                UE_LOG(LogTemp, Warning, TEXT("Http tool: /token recieved token: %s"), *(Response->GetContentAsString()));

                SetToken(Response->GetContentAsString());
                BrowserWrapper->SetShopUrl(ShopUrl);
                BrowserWrapper->LoadURL(ShopUrl);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Http tool: /token failed to get token."));

                BrowserWrapper->Clear();
            }
        });
        HttpTool->Send(request);

        UE_LOG(LogTemp, Warning, TEXT("Http tool: /token post request sent"));
    }
    else
    {
        // add user id 
        TSharedRef<IHttpRequest> request = HttpTool->PostRequest(ServerUrl + FString("/user/add"), userId);
        HttpTool->Send(request);
        UE_LOG(LogTemp, Warning, TEXT("Http tool: /user/add post request sent"));

        // get string from json
        FString outputString;
        TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&outputString);
        FJsonSerializer::Serialize(TokenRequestJson.ToSharedRef(), Writer);

        SetAccessData(outputString);

        //UE_LOG(LogTemp, Warning, TEXT("%s"), *ShopUrl);

        BrowserWrapper->SetShopUrl(ShopUrl);
        BrowserWrapper->LoadURL(ShopUrl);
    }
}

void UXsollaPluginShop::CreateWithToken(
    FString token,
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded OnSucceeded,
    FOnPaymantCanceled OnCanceled,
    FOnPaymantFailed OnFailed)
{
    // show browser wrapper
    BrowserWrapper = CreateWidget<UXsollaPluginWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UXsollaPluginWebBrowserWrapper::StaticClass());

    // set delegates
    this->OnSucceeded = OnSucceeded;
    this->OnCanceled = OnCanceled;
    this->OnFailed = OnFailed;

    BrowserWrapper->OnShopClosed.BindLambda([this]() { this->OnShopClosed(); return; });

    // load data from config
    LoadConfig(EIntegrationType::VE_SERVER);

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

    // add wrapper to viewport
    BrowserWrapper->AddToViewport(9999);

    // join token to shop url
    SetToken(token);

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

void UXsollaPluginShop::LoadConfig(EIntegrationType type)
{
    // load properties drom global game config
    GConfig->GetBool(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("bSandboxMode"), bIsSandbox, GGameIni);
    if (bIsSandbox)
    {
        if (type == EIntegrationType::VE_SERVELESS)
            ShopUrl = "https://sandbox-secure.xsolla.com/paystation2/?access_data=";
        else 
            ShopUrl = "https://sandbox-secure.xsolla.com/paystation2/?access_token=";
    }
    else
    {
        if (type == EIntegrationType::VE_SERVELESS)
            ShopUrl = "https://secure.xsolla.com/paystation2/?access_data=";
        else
            ShopUrl = "https://secure.xsolla.com/paystation2/?access_token=";
    }

    ProjectId = GetDefault<UXsollaPluginSettings>()->ProjectId;
    ServerUrl = GetDefault<UXsollaPluginSettings>()->ServerUrl;
}

void UXsollaPluginShop::SetToken(FString token)
{
    if (!ShopUrl.EndsWith("="))
    {
        ShopUrl.Replace(*XsollaToken, *token);
    }
    else
    {
        ShopUrl += token;
    }

    XsollaToken = token;
}

void UXsollaPluginShop::SetAccessData(FString data)
{
    if (!ShopUrl.EndsWith("="))
    {
        ShopUrl.Replace(*XsollaAccessString, *data);
    }
    else
    {
        ShopUrl += data;
    }

    XsollaAccessString = data;
}

void UXsollaPluginShop::SetDefaultTokenProperties()
{
    //SetStringProperty("user.email.value", FString("example@example.com"), false);
    SetNumberProperty("settings.project_id", FCString::Atoi(*ProjectId), false);
    if (bIsSandbox)
        SetStringProperty("settings.mode", FString("sandbox"), false);
    SetStringProperty("settings.ui.version", FString("desktop"), false);
}

void UXsollaPluginShop::OnShopClosed()
{
    BrowserWrapper->Clear();

    FInputModeGameAndUI inputModeGameAndUI;
    //GEngine->GetFirstLocalPlayerController(GetWorld())->SetInputMode(inputModeGameAndUI);

    FString route = "http://52.59.15.45:3333/payment";

    if (ExternalId.IsEmpty())
    {
        if (OnFailed.IsBound())
        {
            OnFailed.Execute(FString("External id is not setted"));
        }
        return;
    }

    TSharedRef<IHttpRequest> Request = HttpTool->PostRequest(route, ExternalId);

    Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
        if (bWasSuccessful)
        {
            UE_LOG(LogTemp, Warning, TEXT("Http tool: /payment payment verified"));

            FTransactionDetails transactionDetails;

            if (Response->GetResponseCode() == 200)
            {
                transactionDetails.TransactionStatus = "DONE";

                if (OnSucceeded.IsBound())
                {
                    OnSucceeded.Execute(transactionDetails);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Http tool: /payment failed to verify payment."));

                transactionDetails.TransactionStatus = "FAILED";

                if (OnFailed.IsBound())
                {
                    OnFailed.Execute(FString("Transaction failed"));
                }
            }
        }
    });

    HttpTool->Send(Request);
    UE_LOG(LogTemp, Warning, TEXT("Http tool: /payment post request sent"));

    // clear token json
    TokenRequestJson = MakeShareable(new FJsonObject);
}