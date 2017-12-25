#include "XsollaPluginHttpTool.h"

XsollaPluginHttpTool::XsollaPluginHttpTool()
{
    Http = &FHttpModule::Get();
}

void XsollaPluginHttpTool::SetAuthorizationHash(FString Hash, TSharedRef<IHttpRequest>& Request)
{
    Request->SetHeader(AuthorizationHeader, Hash);
}

TSharedRef<IHttpRequest> XsollaPluginHttpTool::RequestWithRoute(FString Subroute)
{
    TSharedRef<IHttpRequest> Request = Http->CreateRequest();
    Request->SetURL(ApiBaseUrl + Subroute);
    SetRequestHeaders(Request);
    return Request;
}

void XsollaPluginHttpTool::SetRequestHeaders(TSharedRef<IHttpRequest>& Request)
{
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
}

TSharedRef<IHttpRequest> XsollaPluginHttpTool::GetRequest(FString Subroute)
{
    TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
    Request->SetVerb("GET");
    return Request;
}

TSharedRef<IHttpRequest> XsollaPluginHttpTool::PostRequest(FString Subroute, FString ContentJsonString)
{
    TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
    Request->SetVerb("POST");
    Request->SetContentAsString(ContentJsonString);
    return Request;
}

void XsollaPluginHttpTool::Send(TSharedRef<IHttpRequest>& Request)
{
    Request->ProcessRequest();
}

bool XsollaPluginHttpTool::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
        return false;

    if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
        return true;
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d, %s"), Response->GetResponseCode(), *Response->GetContentAsString());
        return false;
    }
}