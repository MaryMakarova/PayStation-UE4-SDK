#include "XsollaPluginShop.h"
#include "XsollaPluginSettings.h"
#include "Misc/Base64.h"
#include "BigInt.h"

UXsollaPluginShop::UXsollaPluginShop(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	HttpTool = new XsollaPluginHttpTool();
	TokenJson = MakeShareable(new FJsonObject);

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
	FString shopSize,
	FOnPaymantSucceeded OnSucceeded, 
	FOnPaymantCanceled OnCanceled,
	FOnPaymantFailed OnFailed)
{
	this->OnSucceeded = OnSucceeded;
	this->OnCanceled = OnCanceled;
	this->OnFailed = OnFailed;

	// show browwser wrapper
	BrowserWrapper = CreateWidget<UXsollaPluginWebBrowserWrapper>(GEngine->GameViewport->GetWorld(), UXsollaPluginWebBrowserWrapper::StaticClass());

	if (shopSize.Compare("small") == 0)
	{
		BrowserWrapper->SetBrowserSize(620, 630);
	}
	else if (shopSize.Compare("medium") == 0)
	{
		BrowserWrapper->SetBrowserSize(740, 760);
	}
	else if (shopSize.Compare("large") == 0)
	{
		BrowserWrapper->SetBrowserSize(820, 840);
	}

	BrowserWrapper->AddToViewport(9999);

	// shop delegates
	BrowserWrapper->OnSucceeded = OnSucceeded;
	BrowserWrapper->OnFailed = OnFailed;
	BrowserWrapper->OnCanceled = OnCanceled;

	// set token properties
	SetProperty("user.id.value", "12345678");
	SetProperty("user.id.hidden", true);

	SetProperty("user.email.value", "example@example.com");
	SetProperty("user.email.allow_modify", true);
	SetProperty("user.email.hidden", false);

	ExternalId = FBase64::Encode(FString::SanitizeFloat(GEngine->GameViewport->GetWorld()->GetRealTimeSeconds() + FMath::Rand()));

	SetProperty("settings.external_id", ExternalId);
	SetProperty("settings.return_url", "https://www.unrealengine.com");
	SetProperty("settings.project_id", FCString::Atoi(*ProjectId));
	if (bIsSandbox) 
		SetProperty("settings.mode", "sandbox");

	SetProperty("settings.ui.size", shopSize);
	SetProperty("settings.ui.version", "desktop");
	SetProperty("settings.ui.theme", "dark");

	// get string from json
	FString outputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&outputString);
	FJsonSerializer::Serialize(TokenJson.ToSharedRef(), Writer);

	UE_LOG(LogTemp, Warning, TEXT("JSON: %s"), *outputString);

	// get shop
	GetToken(outputString);
}

void UXsollaPluginShop::GetToken(FString shopJson)
{
	FString route = "https://api.xsolla.com/merchant/merchants/";
	route += MerchantId;
	route += "/token";

	TSharedRef<IHttpRequest> Request = HttpTool->PostRequest(route, shopJson);

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
		FString tokenString = JsonParsed->GetStringField("token");
		ShopUrl += tokenString;
		XsollaToken = tokenString;

		BrowserWrapper->SetExternalId(ExternalId);
		BrowserWrapper->SetShopUrl(ShopUrl);
		BrowserWrapper->LoadURL(ShopUrl);
	}
}

bool UXsollaPluginShop::SetProperty(FString prop, int value)
{
	TArray<FString> subStrings;
	TSharedPtr<FJsonObject> currentObj = TokenJson.ToSharedRef();

	prop.ParseIntoArray(subStrings, TEXT("."));

	for (int i = 0; i < subStrings.Num(); ++i)
	{
		if (i + 1 == subStrings.Num())
		{
			currentObj->SetNumberField(subStrings.Last(), value);
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

bool UXsollaPluginShop::SetProperty(FString prop, bool value)
{
	TArray<FString> subStrings;
	TSharedPtr<FJsonObject> currentObj = TokenJson.ToSharedRef();

	prop.ParseIntoArray(subStrings, TEXT("."));

	for (int i = 0; i < subStrings.Num(); ++i)
	{
		if (i + 1 == subStrings.Num())
		{
			currentObj->SetBoolField(subStrings.Last(), value);
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

bool UXsollaPluginShop::SetProperty(FString prop, FString value)
{
	TArray<FString> subStrings;
	TSharedPtr<FJsonObject> currentObj = TokenJson.ToSharedRef();

	prop.ParseIntoArray(subStrings, TEXT("."));

	for (int i = 0; i < subStrings.Num(); ++i)
	{
		if (i + 1 == subStrings.Num())
		{
			currentObj->SetStringField(subStrings.Last(), value);
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

bool UXsollaPluginShop::SetProperty(FString prop, const ANSICHAR* value)
{
	TArray<FString> subStrings;
	TSharedPtr<FJsonObject> currentObj = TokenJson.ToSharedRef();

	prop.ParseIntoArray(subStrings, TEXT("."));

	for (int i = 0; i < subStrings.Num(); ++i)
	{
		if (i + 1 == subStrings.Num())
		{
			currentObj->SetStringField(subStrings.Last(), FString(ANSI_TO_TCHAR(value)));
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
