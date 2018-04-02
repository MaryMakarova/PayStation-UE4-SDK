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
    * @param hash - Hash based on type, username and password.
    * @param request - Http request object.
    */
    void SetAuthorizationHash(FString hash, TSharedRef<IHttpRequest>& request);

    /**
    * Set request URL with template [base url] + [subrout url].
    *
    * @param route - route url.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> RequestWithRoute(FString route);

    /**
    * Set default request headers.
    *
    * @param request - Request object.
    */
    void SetRequestHeaders(TSharedRef<IHttpRequest>& request);

    /**
    * Creates GET request object.
    *
    * @param route - route url.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> GetRequest(FString route);

    /**
    * Creates POST request object.
    *
    * @param route - route url.
    * @param contentJsonString - JSON content in string.
    *
    * @return Request object
    */
    TSharedRef<IHttpRequest> PostRequest(FString route, FString contentJsonString);

    /**
    * Process request.
    *
    * @param request - Request object.
    */
    void Send(TSharedRef<IHttpRequest>& request);

    /**
    * Check is response is valid. If not, log error code and response content.
    *
    * @param response - Response object.
    * @param bWasSuccessful - Is request was successful.
    *
    * @return Is response valid
    */
    bool ResponseIsValid(FHttpResponsePtr response, bool bWasSuccessful);

private:
    FHttpModule* Http;

    FString AuthorizationHeader = TEXT("Authorization");
};