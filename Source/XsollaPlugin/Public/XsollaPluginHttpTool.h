#pragma once

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Json/Public/Json.h"
#include "Runtime/JsonUtilities/Public/JsonUtilities.h"

class XsollaPluginHttpTool
{
public:
    XsollaPluginHttpTool()
    {
        Http = &FHttpModule::Get();
    }

    void SetAuthorizationHash(FString Hash, TSharedRef<IHttpRequest>& Request)
    {
        Request->SetHeader(AuthorizationHeader, Hash);
    }

    TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute)
    {
        TSharedRef<IHttpRequest> Request = Http->CreateRequest();
        Request->SetURL(ApiBaseUrl + Subroute);
        SetRequestHeaders(Request);
        return Request;
    }

    void SetRequestHeaders(TSharedRef<IHttpRequest>& Request)
    {
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
    }

    TSharedRef<IHttpRequest> GetRequest(FString Subroute)
    {
        TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
        Request->SetVerb("GET");
        return Request;
    }

    TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString)
    {
        TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
        Request->SetVerb("POST");
        Request->SetContentAsString(ContentJsonString);
        return Request;
    }

    void Send(TSharedRef<IHttpRequest>& Request)
    {
        Request->ProcessRequest();
    }

    bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful)
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

private:
    FHttpModule* Http;

    FString AuthorizationHeader = TEXT("Authorization");
    FString ApiBaseUrl = "";
};