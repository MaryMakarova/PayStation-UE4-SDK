// Copyright 2019 Xsolla Inc. All Rights Reserved.
// <support@xsolla.com>
#include "XsollaPayStation.h"

#include "XsollaPayStationSettings.h"
#include "Misc/Base64.h"

UXsollaPayStation::UXsollaPayStation(const FObjectInitializer& objectInitializer)
    :Super(objectInitializer)
{
    HttpTool = new XsollaPayStationHttpTool();
}

void UXsollaPayStation::Create(EShopSizeEnum shopSize, FString userId, FOnPayStationClosed onClose)
{
    // create browser wrapper
    BrowserWrapper = CreateWidget<UWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UWebBrowserWrapper::StaticClass());

    // set delegates
	this->onClose.Unbind();
    this->onClose = onClose;

    BrowserWrapper->OnShopClosed.BindUObject(this, &UXsollaPayStation::OnShopClosed);

    // load data from config
    LoadConfig();

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

	// make request to get token
	TSharedRef<IHttpRequest> tokenRequest = HttpTool->PostRequest(GetDefault<UXsollaPayStationSettings>()->ServerUrl, userId);
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
		}
	});
	HttpTool->Send(tokenRequest);

	UE_LOG(LogTemp, Warning, TEXT("Http tool: /token post request sent"));

    bIsShopOpen = true;
}

#if PLATFORM_MAC
void UXsollaPayStation::CreateInSystemDefaultBrowser(FString userId)
{
	LoadConfig();

	// make request to get token
	TSharedRef<IHttpRequest> tokenRequest = HttpTool->PostRequest(GetDefault<UXsollaPayStationSettings>()->ServerUrl, userId);
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
			
			// open url in default browser
			FString command = FString(TEXT("open ")) + ShopUrl;
			system( TCHAR_TO_UTF8(*command) );
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Http tool: /token failed to get token."));
		}
	});
	HttpTool->Send(tokenRequest);
}
#endif

void UXsollaPayStation::LoadConfig()
{
    // load properties from global game config
    GConfig->GetBool(TEXT("/Script/XsollaPayStationPlugin.XsollaPayStationSettings"), TEXT("bSandboxMode"), bIsSandbox, GGameIni);

	ShopUrl = (bIsSandbox ? SandboxApiUrl : ApiUrl) + "/?access_token=";
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

    bIsShopOpen = false;

    if (onClose.IsBound())
    {
        onClose.Execute();
    }
}