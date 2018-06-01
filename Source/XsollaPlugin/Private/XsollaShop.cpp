#include "XsollaShop.h"

#include "XsollaSettings.h"
#include "Misc/Base64.h"

UXsollaShop::UXsollaShop(const FObjectInitializer& objectInitializer)
    :Super(objectInitializer)
{
    HttpTool = new XsollaPluginHttpTool();
    TokenRequestJson = MakeShareable(new FJsonObject);
}

void UXsollaShop::Create(
    EShopSizeEnum shopSize,
    FString userId,
    FOnPaymantSucceeded onSucceeded, 
    FOnPaymantCanceled onClose,
    FOnPaymantFailed onFailed)
{
    // create browser wrapper
    BrowserWrapper = CreateWidget<UXsollaWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UXsollaWebBrowserWrapper::StaticClass());

    // unbind delegates
    this->OnSucceeded.Unbind();
    this->onClose.Unbind();
    this->OnFailed.Unbind();

    // set delegates
    this->OnSucceeded = onSucceeded;
    this->onClose = onClose;
    this->OnFailed = onFailed;

    BrowserWrapper->OnShopClosed.BindUObject(this, &UXsollaShop::OnShopClosed);

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
            SetStringProperty("settings.ui.size", FString("small"), true);
            BrowserWrapper->SetBrowserSize(620, 630);
            break;
        }

        case EShopSizeEnum::VE_Medium:
        {
            SetStringProperty("settings.ui.size", FString("medium"), true);
            BrowserWrapper->SetBrowserSize(740, 760);
            break;
        }

        case EShopSizeEnum::VE_Large:
        {
            BrowserWrapper->SetBrowserSize(820, 840);
            SetStringProperty("settings.ui.size", FString("large"), true);
            break;
        }
    }

    // set user id
    SetStringProperty("user.id.value", userId);

    SetDefaultTokenProperties();

    BrowserWrapper->Open();

    if (GetDefault<UXsollaPluginSettings>()->IntegrationType == EIntegrationType::VE_SERVER)
    {
        // set return url for "back to game" button handling
        SetStringProperty("settings.return_url", "https://www.unrealengine.com");

        // get string from json
        FString outputString;
        TSharedRef< TJsonWriter<> > writer = TJsonWriterFactory<>::Create(&outputString);
        FJsonSerializer::Serialize(TokenRequestJson.ToSharedRef(), writer);

        UE_LOG(LogTemp, Warning, TEXT("Token JSON: %s"), *outputString);

        // make request to get token
        TSharedRef<IHttpRequest> tokenRequest = HttpTool->PostRequest(ServerUrl + FString("/token"), outputString);
        tokenRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
        {
            if (bWasSuccessful) 
            {
                if (!response->GetContentAsString().IsEmpty())
                {
                    UE_LOG(LogTemp, Warning, TEXT("Http tool: /token recieved token: %s"), *(response->GetContentAsString()));
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Http tool: /token failed to get token, probably wrong Project Id"));
                }
                
                SetToken(response->GetContentAsString());
                BrowserWrapper->LoadURL(ShopUrl);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Http tool: /token failed to get token."));

                // close browser
                BrowserWrapper->Clear();

                // clear token json
                TokenRequestJson = MakeShareable(new FJsonObject);
            }
        });
        HttpTool->Send(tokenRequest);

        UE_LOG(LogTemp, Warning, TEXT("Http tool: /token post request sent"));
    }
    else
    {
        // make request to add user on server for payment verification
        TSharedRef<IHttpRequest> userAddRequest = HttpTool->PostRequest(ServerUrl + FString("/user/add"), userId);
        HttpTool->Send(userAddRequest);
        UE_LOG(LogTemp, Warning, TEXT("Http tool: /user/add post request sent"));

        // get string from json
        FString outputString;
        TSharedRef< TJsonWriter<> > writer = TJsonWriterFactory<>::Create(&outputString);
        FJsonSerializer::Serialize(TokenRequestJson.ToSharedRef(), writer);

        SetAccessData(outputString);

        //UE_LOG(LogTemp, Warning, TEXT("%s"), *ShopUrl);

        BrowserWrapper->LoadURL(ShopUrl);
    }

    bIsShopOpen = true;
}

void UXsollaShop::CreateWithToken(
    FString token,
    EShopSizeEnum shopSize,
    FOnPaymantSucceeded onSucceeded,
    FOnPaymantCanceled onClose,
    FOnPaymantFailed onFailed)
{
    // create browser wrapper
    BrowserWrapper = CreateWidget<UXsollaWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UXsollaWebBrowserWrapper::StaticClass());

    // set delegates
    this->OnSucceeded = onSucceeded;
    this->onClose = onClose;
    this->OnFailed = onFailed;

    BrowserWrapper->OnShopClosed.BindLambda([this]() { this->OnShopClosed(); return; });

    // load data from config
    LoadConfig(EIntegrationType::VE_SERVER);

    // set shop interface size
    switch (shopSize)
    {
        case EShopSizeEnum::VE_Small:
        {
            BrowserWrapper->SetBrowserSize(620, 630);
            break;
        }       
        case EShopSizeEnum::VE_Medium:
        {
            BrowserWrapper->SetBrowserSize(740, 760);
            break;
        }    
        case EShopSizeEnum::VE_Large:
        {
            BrowserWrapper->SetBrowserSize(820, 840);
            break;
        }      
    }

    // add wrapper to viewport
    BrowserWrapper->Open();

    // join token to shop url
    SetToken(token);

    BrowserWrapper->LoadURL(ShopUrl);

    bIsShopOpen = true;
}

void UXsollaShop::SetNumberProperty(FString prop, int value, bool bOverride/*= true */)
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

void UXsollaShop::SetBoolProperty(FString prop, bool value, bool bOverride/*= true */)
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

void UXsollaShop::SetStringProperty(FString prop, FString value, bool bOverride/*= true */)
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

void UXsollaShop::LoadConfig(EIntegrationType type)
{
    // load properties drom global game config
    GConfig->GetBool(TEXT("/Script/XsollaPlugin.XsollaPluginSettings"), TEXT("bSandboxMode"), bIsSandbox, GGameIni);
    if (bIsSandbox)
    {
        if (type == EIntegrationType::VE_SERVELESS)
            ShopUrl = SandboxApiUrl + "/?access_data=";
        else 
            ShopUrl = SandboxApiUrl + "/?access_token=";
    }
    else
    {
        if (type == EIntegrationType::VE_SERVELESS)
            ShopUrl = ApiUrl + "/?access_data=";
        else
            ShopUrl = ApiUrl + "/?access_token=";
    }

    ProjectId = GetDefault<UXsollaPluginSettings>()->ProjectId;
    ServerUrl = GetDefault<UXsollaPluginSettings>()->ServerUrl;
}

void UXsollaShop::SetToken(FString token)
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

void UXsollaShop::SetAccessData(FString data)
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

void UXsollaShop::SetDefaultTokenProperties()
{
    //SetStringProperty("user.email.value", FString("example@example.com"), false);
    SetNumberProperty("settings.project_id", FCString::Atoi(*ProjectId), false);
    if (bIsSandbox)
        SetStringProperty("settings.mode", FString("sandbox"), false);
    SetStringProperty("settings.ui.version", FString("desktop"), false);
}

void UXsollaShop::OnShopClosed()
{
    BrowserWrapper->Clear();

    FString route = ServerUrl + "/payment";

    if (ExternalId.IsEmpty())
    {
        if (OnFailed.IsBound())
        {
            OnFailed.Execute(FString("External id is not setted"));
        }
        return;
    }

    TSharedRef<IHttpRequest> Request = HttpTool->PostRequest(route, ExternalId);

    Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) 
    {
        if (bWasSuccessful)
        {
            if (Response->GetResponseCode() == 200)
            {
                UE_LOG(LogTemp, Warning, TEXT("Http tool: /payment payment verified"));

                FTransactionDetails transactionDetails;
                FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &transactionDetails, 0, 0);

                if (OnSucceeded.IsBound())
                {
                    OnSucceeded.Execute(transactionDetails);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Http tool: /payment failed to verify payment."));

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

    bIsShopOpen = false;

    if (onClose.IsBound())
    {
        onClose.Execute();
    }
}