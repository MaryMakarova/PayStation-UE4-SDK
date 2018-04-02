#include "XsollaHttpTool.h"

XsollaPluginHttpTool::XsollaPluginHttpTool()
{
    Http = &FHttpModule::Get();
}

void XsollaPluginHttpTool::SetAuthorizationHash(FString hash, TSharedRef<IHttpRequest>& request)
{
    request->SetHeader(AuthorizationHeader, hash);
}

TSharedRef<IHttpRequest> XsollaPluginHttpTool::RequestWithRoute(FString route)
{
    TSharedRef<IHttpRequest> Request = Http->CreateRequest();
    Request->SetURL(route);
    SetRequestHeaders(Request);

    return Request;
}

void XsollaPluginHttpTool::SetRequestHeaders(TSharedRef<IHttpRequest>& request)
{
    request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    request->SetHeader(TEXT("Accept"), TEXT("application/json"));
}

TSharedRef<IHttpRequest> XsollaPluginHttpTool::GetRequest(FString route)
{
    TSharedRef<IHttpRequest> Request = RequestWithRoute(route);
    Request->SetVerb("GET");

    return Request;
}

TSharedRef<IHttpRequest> XsollaPluginHttpTool::PostRequest(FString route, FString contentJsonString)
{
    TSharedRef<IHttpRequest> Request = RequestWithRoute(route);
    Request->SetVerb("POST");
    Request->SetContentAsString(contentJsonString);

    return Request;
}

void XsollaPluginHttpTool::Send(TSharedRef<IHttpRequest>& request)
{
    request->ProcessRequest();
}

bool XsollaPluginHttpTool::ResponseIsValid(FHttpResponsePtr response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !response.IsValid())
        return false;

    if (EHttpResponseCodes::IsOk(response->GetResponseCode()))
        return true;
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d, %s"), response->GetResponseCode(), *response->GetContentAsString());
        return false;
    }
}