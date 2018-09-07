#include "XsollaPayStation.h"

#include "XsollaPayStationSettings.h"
#include "Misc/Base64.h"

UXsollaPayStation::UXsollaPayStation(const FObjectInitializer& objectInitializer)
    :Super(objectInitializer)
{
    HttpTool = new XsollaPayStationHttpTool();
    TokenRequestJson = MakeShareable(new FJsonObject);
}

void UXsollaPayStation::Create(
    EShopSizeEnum shopSize,
    FOnPaymentSucceeded onSucceeded, 
    FOnPayStationClosed onClose,
    FOnPaymentFailed onFailed)
{
    // create browser wrapper
    BrowserWrapper = CreateWidget<UWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UWebBrowserWrapper::StaticClass());

    // unbind delegates
    this->OnSucceeded.Unbind();
    this->onClose.Unbind();
    this->OnFailed.Unbind();

    // set delegates
    this->OnSucceeded = onSucceeded;
    this->onClose = onClose;
    this->OnFailed = onFailed;

    BrowserWrapper->OnShopClosed.BindUObject(this, &UXsollaPayStation::OnShopClosed);

    // load data from config
    LoadConfig();

    // generate external_id
    ExternalId = FBase64::Encode(FString::SanitizeFloat(GEngine->GameViewport->GetWorld()->GetRealTimeSeconds() + FMath::Rand()));
    SetStringProperty("settings.external_id", ExternalId);

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

    BrowserWrapper->Open();

	// set return url for "back to game" button handling
	SetStringProperty("settings.return_url", "https://www.unrealengine.com");

	// get string from json
	FString outputString;
	TSharedRef< TJsonWriter<> > writer = TJsonWriterFactory<>::Create(&outputString);
	FJsonSerializer::Serialize(TokenRequestJson.ToSharedRef(), writer);

	UE_LOG(LogTemp, Warning, TEXT("Token JSON: %s"), *outputString);

	// make request to get token
	TSharedRef<IHttpRequest> tokenRequest = HttpTool->PostRequest(GetTokenUrl, outputString);
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

    bIsShopOpen = true;
}

void UXsollaPayStation::SetNumberProperty(FString prop, int value, bool bOverride/*= true */)
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

void UXsollaPayStation::SetBoolProperty(FString prop, bool value, bool bOverride/*= true */)
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

void UXsollaPayStation::SetStringProperty(FString prop, FString value, bool bOverride/*= true */)
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

void UXsollaPayStation::LoadConfig()
{
    // load properties drom global game config
    GConfig->GetBool(TEXT("/Script/XsollaPayStationPlugin.XsollaPayStationSettings"), TEXT("bSandboxMode"), bIsSandbox, GGameIni);
    if (bIsSandbox)
    {
        ShopUrl = SandboxApiUrl + "/?access_token=";
    }
    else
    {
        ShopUrl = ApiUrl + "/?access_token=";
    }

	GetTokenUrl = GetDefault<UXsollaPayStationSettings>()->GetTokenUrl;
	GetPaymentStatusUrl = GetDefault<UXsollaPayStationSettings>()->GetPaymentStatusUrl;
}

void UXsollaPayStation::SetToken(FString token)
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

void UXsollaPayStation::OnShopClosed()
{
    BrowserWrapper->Clear();

    FString route = GetPaymentStatusUrl;

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