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

    /**
    * Set request authorization header. 
    * Syntax of header is [Authorization: <type> <credentials>]
    *
    * @param Hash - Hash based on type, username and password.
    * @param Request - Http request object.
    */
    void SetAuthorizationHash(FString Hash, TSharedRef<IHttpRequest>& Request)
    {
        Request->SetHeader(AuthorizationHeader, Hash);
    }

    /**
    * Set request URL with template [base url] + [subrout url].
    *
    * @param Subroute - Subrout url.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute)
    {
        TSharedRef<IHttpRequest> Request = Http->CreateRequest();
        Request->SetURL(ApiBaseUrl + Subroute);
        SetRequestHeaders(Request);
        return Request;
    }

    /**
    * Set default request headers.
    *
    * @param Request - Request object.
    */
    void SetRequestHeaders(TSharedRef<IHttpRequest>& Request)
    {
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
    }

    /**
    * Creates GET request object.
    *
    * @param Subroute - Subroute url.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> GetRequest(FString Subroute)
    {
        TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
        Request->SetVerb("GET");
        return Request;
    }

    /**
    * Creates POST request object.
    *
    * @param Subroute - Subroute url.
    * @param ContentJsonString - JSON content in string.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString)
    {
        TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
        Request->SetVerb("POST");
        Request->SetContentAsString(ContentJsonString);
        return Request;
    }

    /**
    * Process request.
    *
    * @param Request - Request object.
    */
    void Send(TSharedRef<IHttpRequest>& Request)
    {
        Request->ProcessRequest();
    }

    /**
    * Check is response is valid. If not, log error code and response content.
    *
    * @param Response - Response object.
    * @param bWasSuccessful - Is request was successful.
    *
    * @return Is response valid
    */
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