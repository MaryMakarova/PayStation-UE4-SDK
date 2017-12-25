#pragma once

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Runtime/Json/Public/Json.h"
#include "Runtime/JsonUtilities/Public/JsonUtilities.h"

class XsollaPluginHttpTool
{
public:
    XsollaPluginHttpTool();

    /**
    * Set request authorization header. 
    * Syntax of header is [Authorization: <type> <credentials>]
    *
    * @param Hash - Hash based on type, username and password.
    * @param Request - Http request object.
    */
    void SetAuthorizationHash(FString Hash, TSharedRef<IHttpRequest>& Request);

    /**
    * Set request URL with template [base url] + [subrout url].
    *
    * @param Subroute - Subrout url.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);

    /**
    * Set default request headers.
    *
    * @param Request - Request object.
    */
    void SetRequestHeaders(TSharedRef<IHttpRequest>& Request);

    /**
    * Creates GET request object.
    *
    * @param Subroute - Subroute url.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> GetRequest(FString Subroute);

    /**
    * Creates POST request object.
    *
    * @param Subroute - Subroute url.
    * @param ContentJsonString - JSON content in string.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString);

    /**
    * Process request.
    *
    * @param Request - Request object.
    */
    void Send(TSharedRef<IHttpRequest>& Request);

    /**
    * Check is response is valid. If not, log error code and response content.
    *
    * @param Response - Response object.
    * @param bWasSuccessful - Is request was successful.
    *
    * @return Is response valid
    */
    bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

private:
    FHttpModule* Http;

    FString AuthorizationHeader = TEXT("Authorization");
    FString ApiBaseUrl = "";
};