#include "XsollaPluginShop.h"
#include "XsollaPluginSettings.h"
#include "Misc/Base64.h"
#include "BigInt.h"

UXsollaPluginShop::UXsollaPluginShop(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	HttpTool = new XsollaPluginHttpTool();

	// xsolla api stuffs
	bIsSandbox = GetDefault<UXsollaPluginSettings>()->bSandboxMode;
	if (bIsSandbox)
	{
		ShopUrl = "https://sandbox-secure.xsolla.com/paystation2/?access_token=";
	}
	else
	{
		ShopUrl = "https://secure.xsolla.com/paystation2/?access_token=";
	}

	MerchantId = GetDefault<UXsollaPluginSettings>()->MerchantId;
	ProjectId = GetDefault<UXsollaPluginSettings>()->ProjectId;
	ApiKey = GetDefault<UXsollaPluginSettings>()->ApiKey;
}

void UXsollaPluginShop::CreateShop(
	FString shopSize,
	FString shopDesign,
	FOnPaymantSucceeded OnSucceeded, 
	FOnPaymantCanceled OnCanceled,
	FOnPaymantFailed OnFailed)
{
	// delegates setting
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

	/* SHOP JSON */
	// user section
	TSharedPtr<FJsonObject> userIdJsonObj = MakeShareable(new FJsonObject);
	userIdJsonObj->SetStringField("value", "1234567");
	userIdJsonObj->SetBoolField("hidden", true);

	TSharedPtr<FJsonObject> userJsonObj = MakeShareable(new FJsonObject);
	userJsonObj->SetObjectField("id", userIdJsonObj);

	TSharedPtr<FJsonObject> emailJsonObj = MakeShareable(new FJsonObject);
	emailJsonObj->SetStringField("value", "example@example.com");
	emailJsonObj->SetBoolField("allow_modify", true);
	emailJsonObj->SetBoolField("hidden", false);

	userJsonObj->SetObjectField("email", emailJsonObj);

	// settings section
	TSharedPtr<FJsonObject> settingsJsonObj = MakeShareable(new FJsonObject);

	ExternalId = FBase64::Encode(FString::SanitizeFloat(GEngine->GameViewport->GetWorld()->GetRealTimeSeconds() + FMath::Rand()));
	UE_LOG(LogTemp, Warning, TEXT("Ex id: %s"), *ExternalId);

	settingsJsonObj->SetStringField("external_id", ExternalId);
	settingsJsonObj->SetStringField("return_url", "https://www.unrealengine.com");
	settingsJsonObj->SetNumberField("project_id", FCString::Atoi(*ProjectId));
	if (bIsSandbox)
	{
		settingsJsonObj->SetStringField("mode", "sandbox");
	}

	TSharedPtr<FJsonObject> settingsUiJsonObj = MakeShareable(new FJsonObject);
	settingsUiJsonObj->SetStringField("size", shopSize);
	settingsUiJsonObj->SetStringField("version", shopDesign);
	settingsJsonObj->SetObjectField("ui", settingsUiJsonObj);

	// purchase json
	TSharedPtr<FJsonObject> purchaseJsonObj = MakeShareable(new FJsonObject);


	// combine into main section
	TSharedPtr<FJsonObject> requestJsonObj = MakeShareable(new FJsonObject);
	requestJsonObj->SetObjectField("user", userJsonObj);
	requestJsonObj->SetObjectField("settings", settingsJsonObj);

	// get string from json
	FString outputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&outputString);
	FJsonSerializer::Serialize(requestJsonObj.ToSharedRef(), Writer);

	// get shop token
	GetToken(outputString);
}

void UXsollaPluginShop::GetToken(FString shopJson)
{
	FString route = "https://api.xsolla.com/merchant/merchants/";
	route += MerchantId;
	route += "/token";

	TSharedRef<IHttpRequest> Request = HttpTool->PostRequest(route, shopJson);

	Request->OnProcessRequestComplete().BindUObject(this, &UXsollaPluginShop::OnGetTokenRequestComplete);
	HttpTool->SetAuthorizationHash(FString("Basic ") + FBase64::Encode(MerchantId + FString(":") + ApiKey), Request);

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
		UE_LOG(LogTemp, Warning, TEXT("token: %s"), *ShopUrl);

		BrowserWrapper->SetExternalId(ExternalId);
		BrowserWrapper->SetShopUrl(ShopUrl);
		BrowserWrapper->LoadURL(ShopUrl);
	}
}