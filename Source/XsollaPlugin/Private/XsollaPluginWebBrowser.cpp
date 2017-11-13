// Fill out your copyright notice in the Description page of Project Settings.

#include "XsollaPluginWebBrowser.h"
#include "XsollaPlugin.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "XsollaPluginSettings.h"
#include "Misc/Base64.h"

#define LOCTEXT_NAMESPACE "WebBrowser"

/////////////////////////////////////////////////////
// UCPWebBrowser

UXsollaPluginWebBrowser::UXsollaPluginWebBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
	Http = &FHttpModule::Get();

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

void UXsollaPluginWebBrowser::GetToken()
{
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

	TSharedPtr<FJsonObject> settingsJsonObj = MakeShareable(new FJsonObject);
	settingsJsonObj->SetNumberField("project_id", FCString::Atoi(*ProjectId));
	if (bIsSandbox)
	{
		settingsJsonObj->SetStringField("mode", "sandbox");
	}

	// root object
	TSharedPtr<FJsonObject> requestJsonObj = MakeShareable(new FJsonObject);
	requestJsonObj->SetObjectField("user", userJsonObj);
	requestJsonObj->SetObjectField("settings", settingsJsonObj);

	FString outputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&outputString);
	FJsonSerializer::Serialize(requestJsonObj.ToSharedRef(), Writer);

	FString route = MerchantId;
	route += "/token";
	TSharedRef<IHttpRequest> Request = PostRequest(route, outputString);
	Request->OnProcessRequestComplete().BindUObject(this, &UXsollaPluginWebBrowser::OnLoadResponse);
	SetAuthorizationHash(FString("Basic ") + FBase64::Encode(MerchantId + FString(":") + ApiKey), Request);
	Send(Request);
}

TSharedRef<IHttpRequest> UXsollaPluginWebBrowser::RequestWithRoute(FString Subroute) 
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(ApiBaseUrl + Subroute);
	SetRequestHeaders(Request);
	return Request;
}

void UXsollaPluginWebBrowser::SetRequestHeaders(TSharedRef<IHttpRequest>& Request) 
{
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
}

TSharedRef<IHttpRequest> UXsollaPluginWebBrowser::GetRequest(FString Subroute)
{
	TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
	Request->SetVerb("GET");
	return Request;
}

TSharedRef<IHttpRequest> UXsollaPluginWebBrowser::PostRequest(FString Subroute, FString ContentJsonString)
{
	TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
	Request->SetVerb("POST");
	Request->SetContentAsString(ContentJsonString);
	return Request;
}

void UXsollaPluginWebBrowser::Send(TSharedRef<IHttpRequest>& Request) 
{
	Request->ProcessRequest();
}

bool UXsollaPluginWebBrowser::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) 
{
	if (!bWasSuccessful || !Response.IsValid()) 
		return false;

	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) 
		return true;
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
		return false;
	}
}

void UXsollaPluginWebBrowser::SetAuthorizationHash(FString Hash, TSharedRef<IHttpRequest>& Request) 
{
	Request->SetHeader(AuthorizationHeader, Hash);
}



/**************************************************************************************************************************/



template <typename StructType>
void UXsollaPluginWebBrowser::GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput) 
{
	FJsonObjectConverter::UStructToJsonObjectString(StructType::StaticStruct(), &FilledStruct, StringOutput, 0, 0);
}

template <typename StructType>
void UXsollaPluginWebBrowser::GetStructFromJsonString(FHttpResponsePtr Response, StructType& StructOutput)
{
	StructType StructData;
	FString JsonString = Response->GetContentAsString();
	FJsonObjectConverter::JsonObjectStringToUStruct<StructType>(JsonString, &StructOutput, 0, 0);
}



/**************************************************************************************************************************/

void UXsollaPluginWebBrowser::OnLoadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) 
{
	if (!ResponseIsValid(Response, bWasSuccessful))
		return;

	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		FString tokenString = JsonParsed->GetStringField("token");
		ShopUrl += tokenString;
		UE_LOG(LogTemp, Warning, TEXT("token: %s"), *ShopUrl);
		LoadURL(ShopUrl); 
	}
}

void UXsollaPluginWebBrowser::LoadURL(FString NewURL)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->LoadURL(NewURL);
	}
}

void UXsollaPluginWebBrowser::LoadString(FString Contents, FString DummyURL)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->LoadString(Contents, DummyURL);
	}
}

void UXsollaPluginWebBrowser::ExecuteJavascript(const FString& ScriptText)
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->ExecuteJavascript(ScriptText);
	}
}

FText UXsollaPluginWebBrowser::GetTitleText() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetTitleText();
	}

	return FText::GetEmpty();
}

FString UXsollaPluginWebBrowser::GetUrl() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetUrl();
	}

	return FString();
}

void UXsollaPluginWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	WebBrowserWidget.Reset();
}

TSharedRef<SWidget> UXsollaPluginWebBrowser::RebuildWidget()
{
	if (IsDesignTime())
	{
		return BuildDesignTimeWidget(SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Web Browser", "Web Browser"))
			]);
	}
	else
	{
		WebBrowserWidget = SNew(SWebBrowser)
			.InitialURL(InitialURL)
			.ShowControls(false)
			.SupportsTransparency(bSupportsTransparency)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
			.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadCompleted))
			.OnLoadError(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadError))
			.OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow));

		return WebBrowserWidget.ToSharedRef();
	}
}

void UXsollaPluginWebBrowser::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (WebBrowserWidget.IsValid())
	{

	}
}

void UXsollaPluginWebBrowser::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}
void UXsollaPluginWebBrowser::HandleOnLoadCompleted()
{
	OnLoadCompleted.Broadcast();
}
void UXsollaPluginWebBrowser::HandleOnLoadError()
{
	OnLoadError.Broadcast();
}
bool UXsollaPluginWebBrowser::HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& NewBrowserWindow)
{
	OnCloseWindow.Broadcast();
	return true;
}
#if WITH_EDITOR

const FText UXsollaPluginWebBrowser::GetPaletteCategory()
{
	return LOCTEXT("Xsolla", "Xsolla");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE



