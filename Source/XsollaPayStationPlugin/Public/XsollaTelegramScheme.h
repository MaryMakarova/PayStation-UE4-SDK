#pragma once

#include "IWebBrowserWindow.h"
#include "WebBrowserModule.h"
#include "IWebBrowserDialog.h"
#include "IWebBrowserSchemeHandler.h"

class TelegramSchemeHandler : public IWebBrowserSchemeHandler
{
public:
    class TelegramHeaders : public IWebBrowserSchemeHandler::IHeaders
    {
    public:
        virtual void SetMimeType(const TCHAR* mimeType) {}
        virtual void SetStatusCode(int32 statusCode) {}
        virtual void SetContentLength(int32 contentLength) {}
        virtual void SetRedirect(const TCHAR* url) {}
        virtual void SetHeader(const TCHAR* key, const TCHAR* value) {}
    };

public:
    TelegramSchemeHandler(FString verb, FString url) {}

    /**
    * Process an incoming request.
    * @param   Verb            This is the verb used for the request (GET, PUT, POST, etc).
    * @param   Url             This is the full url for the request being made.
    * @param   OnHeadersReady  You must execute this delegate once the response headers are ready to be retrieved with GetResponseHeaders.
    *                            You may execute it during this call to state headers are available now.
    * @return You should return true if the request has been accepted and will be processed, otherwise false to cancel this request.
    */
    virtual bool ProcessRequest(const FString& verb, const FString& url, const FSimpleDelegate& onHeadersReady);

    /**
    * Retrieves the headers for this request.
    * @param   OutHeaders      The interface to use to set headers.
    */
    virtual void GetResponseHeaders(IHeaders& outHeaders) {}

    /**
    * Retrieves the headers for this request.
    * @param   OutBytes            You should copy up to BytesToRead of data to this ptr.
    * @param   BytesToRead         The maximum number of bytes that can be copied to OutBytes.
    * @param   BytesRead           You should set this to the number of bytes that were copied.
    *                                This can be set to zero, to indicate more data is not ready yet, and OnMoreDataReady must then be
    *                                executed when there is.
    * @param   OnMoreDataReady     You should execute this delegate when more data is available to read.
    * @return You should return true if more data needs to be read, otherwise false if this is the end of the response data.
    */
    virtual bool ReadResponse(uint8* outBytes, int32 bytesToRead, int32& bytesRead, const FSimpleDelegate& onMoreDataReady)
    {
        return false;
    }

    /**
    * Called if the request should be canceled.
    */
    virtual void Cancel() {}
};

class TelegramSchemeHandlerFactory : public IWebBrowserSchemeHandlerFactory
{
public:
    TelegramSchemeHandlerFactory() {}

    virtual TUniquePtr<IWebBrowserSchemeHandler> Create(FString verb, FString url)
    {
        return TUniquePtr<TelegramSchemeHandler>(new TelegramSchemeHandler(verb, url));
    }
};