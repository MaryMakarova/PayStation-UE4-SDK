#include "XsollaPluginWebBrowserWrapper.h"

#include "XsollaPluginShop.h"
#include "XsollaPluginSettings.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Misc/Base64.h"
#include "IWebBrowserWindow.h"
#include "WebBrowserModule.h"
#include "IWebBrowserDialog.h"
#include "IWebBrowserSchemeHandler.h"
#include "WebBrowserModule.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#include <winreg.h>
#endif

class TelegramSchemeHandler : public IWebBrowserSchemeHandler
{
public:
    class TelegramHeaders : public IWebBrowserSchemeHandler::IHeaders
    {
    public:
        virtual void SetMimeType(const TCHAR* MimeType) {}
        virtual void SetStatusCode(int32 StatusCode) {}
        virtual void SetContentLength(int32 ContentLength) {}
        virtual void SetRedirect(const TCHAR* Url) {}
        virtual void SetHeader(const TCHAR* Key, const TCHAR* Value) {}
    };

public:
    TelegramSchemeHandler(FString Verb, FString Url) {}

    /**
    * Process an incoming request.
    * @param   Verb            This is the verb used for the request (GET, PUT, POST, etc).
    * @param   Url             This is the full url for the request being made.
    * @param   OnHeadersReady  You must execute this delegate once the response headers are ready to be retrieved with GetResponseHeaders.
    *                            You may execute it during this call to state headers are available now.
    * @return You should return true if the request has been accepted and will be processed, otherwise false to cancel this request.
    */
    virtual bool ProcessRequest(const FString& Verb, const FString& Url, const FSimpleDelegate& OnHeadersReady) 
    {
#if PLATFORM_WINDOWS
        HKEY hKey;

        RegOpenKey(HKEY_CLASSES_ROOT, _T("tdesktop.tg\\shell\\open\\command"), &hKey);

        TCHAR regValue[1024];
        DWORD regValueLen = sizeof(regValue);

        RegQueryValueEx(hKey, _T(""), NULL, NULL, reinterpret_cast<LPBYTE>(&regValue), &regValueLen);

        FString telegramCommand(regValue);
        FString binaryPath;
        FString params = FString("-- ") + Url;

        telegramCommand.RemoveFromStart(TEXT("\""));
        telegramCommand.Split(TEXT("\""), &binaryPath, NULL);

        FPlatformProcess::CreateProc(*binaryPath, *params, true, false, false, nullptr, 0, nullptr, nullptr);
#endif

        return false;
    }

    /**
    * Retrieves the headers for this request.
    * @param   OutHeaders      The interface to use to set headers.
    */
    virtual void GetResponseHeaders(IHeaders& OutHeaders) 
    {

    }

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
    virtual bool ReadResponse(uint8* OutBytes, int32 BytesToRead, int32& BytesRead, const FSimpleDelegate& OnMoreDataReady) 
    {
        return false;
    }

    /**
    * Called if the request should be canceled.
    */
    virtual void Cancel() 
    {

    }
};

class TelegramSchemeHandlerFactory : public IWebBrowserSchemeHandlerFactory
{
public:
    TelegramSchemeHandlerFactory()
    {

    }

    virtual TUniquePtr<IWebBrowserSchemeHandler> Create(FString Verb, FString Url)
    {
        return TUniquePtr<TelegramSchemeHandler>(new TelegramSchemeHandler(Verb, Url));
    }
};

#define LOCTEXT_NAMESPACE "XsollaPluginWebBrowserWrapper"

UXsollaPluginWebBrowserWrapper::UXsollaPluginWebBrowserWrapper(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
    ButtonSize(70.0f) // close and home buttons size in Slate units
{
    bIsVariable = true;
}

void UXsollaPluginWebBrowserWrapper::NativeConstruct()
{
    Super::NativeConstruct();

    ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
    ContentSize.Y = ContentSize.Y > ViewportSize.Y ? ViewportSize.Y : ContentSize.Y;

    LoadSlateResources();

    SAssignNew(WebBrowserWidget, SWebBrowser)
        .InitialURL(InitialURL)
        .ShowControls(false)
        .ViewportSize(ContentSize)
        .SupportsTransparency(bSupportsTransparency)
        .OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
        .OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadCompleted))
        .OnLoadError(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadError))
        .OnCloseWindow(BIND_UOBJECT_DELEGATE(FOnCloseWindowDelegate, HandleOnCloseWindow))
        .OnCreateWindow(BIND_UOBJECT_DELEGATE(FOnCreateWindowDelegate, HandleOnPopupCreate))
        .OnSuppressContextMenu_Lambda([]() { return true; });

    ComposeShopWrapper();

    // telegram protocol 
    IWebBrowserModule::Get().GetSingleton()->RegisterSchemeHandlerFactory("tg", "", new TelegramSchemeHandlerFactory());

    PrevFocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);
    bPrevGameViewportInputIgnoring = GEngine->GameViewport->IgnoreInput();

    GEngine->GameViewport->SetIgnoreInput(true);

    ULocalPlayer* player = GEngine->GetFirstGamePlayer(GEngine->GameViewport);
}

void UXsollaPluginWebBrowserWrapper::LoadURL(FString NewURL)
{
    if (WebBrowserWidget.IsValid())
    {
        WebBrowserWidget->LoadURL(NewURL);
    }
}

void UXsollaPluginWebBrowserWrapper::LoadSlateResources()
{
    TSharedRef<FSlateGameResources> slateButtonResources = FSlateGameResources::New(
        FName("ButtonStyle"),
        "/XsollaPlugin/Buttons",
        "/XsollaPlugin/Buttons"
    );
    FSlateGameResources& buttonStyle = slateButtonResources.Get();
    SlateCloseBrush = buttonStyle.GetBrush(FName("close_red_brush"));
    SlateBackBrush = buttonStyle.GetBrush(FName("back_brush"));

    TSharedRef<FSlateGameResources> slateSpinnerResources = FSlateGameResources::New(
        FName("SpinnerStyle"),
        "/XsollaPlugin/LoadingSpinner",
        "/XsollaPlugin/LoadingSpinner"
    );
    FSlateGameResources& spinnerStyle = slateSpinnerResources.Get();
    SlateSpinnerBrush = spinnerStyle.GetBrush(FName("spinner_brush"));
}

void UXsollaPluginWebBrowserWrapper::ComposeShopWrapper()
{
    //BrowserSlot.AttachWidget(SAssignNew(SpinnerImage, SSpinningImage).Image(SlateSpinnerBrush).Period(3.0f));

    BrowserSlot.AttachWidget(SAssignNew(BrowserOverlay, SOverlay));

    BrowserOverlay->AddSlot(0).AttachWidget(WebBrowserWidget.ToSharedRef());

    MainContent = 
        SNew(SVerticalBox)
        + SVerticalBox::Slot().FillHeight((ViewportSize.Y - ContentSize.Y) / 2)
        + SVerticalBox::Slot().FillHeight(ContentSize.Y)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth((ViewportSize.X - ContentSize.X) / 2)
            + SHorizontalBox::Slot().FillWidth(ContentSize.X + ButtonSize)
            [
                SNew(SHorizontalBox)
                + BrowserSlot.FillWidth(ContentSize.X)
                + SHorizontalBox::Slot().FillWidth(ButtonSize)
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Left)
                    [
                        SNew(SBox).HeightOverride(ButtonSize).WidthOverride(ButtonSize)
                        [
                            SAssignNew(CloseButton, SButton)
                            .Visibility(EVisibility::Hidden)
                            .ButtonColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 0)))
                            .OnClicked_Lambda([this]() { this->CloseShop(false); return FReply::Handled(); })
                            .Content()
                            [
                                SNew(SImage)
                                .Image(SlateCloseBrush)
                            ]
                        ]
                    ]
                    + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Left)
                    [
                        SNew(SBox).HeightOverride(ButtonSize).WidthOverride(ButtonSize)
                        [
                            SAssignNew(HomeButton, SButton)
                            .Visibility(EVisibility::Hidden)
                            .ButtonColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 0)))
                            .OnClicked_Lambda([this]() { this->HandleOnHomeButtonClicked();; return FReply::Handled(); })
                            .Content()
                            [
                                SNew(SImage)
                                .Image(SlateBackBrush)
                            ]
                        ]
                    ]
                ]
            ]
            + SHorizontalBox::Slot().FillWidth((ViewportSize.X - ContentSize.X) / 2 - ButtonSize)
        ]
        + SVerticalBox::Slot().FillHeight((ViewportSize.Y - ContentSize.Y) / 2);

    Background = 
        SNew(SVerticalBox)
        + SVerticalBox::Slot().FillHeight(1.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1.0f)
            [
                SNew(SColorBlock).Color(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f))
            ]
        ];

    GEngine->GameViewport->AddViewportWidgetContent(MainContent.ToSharedRef(), 10);
    GEngine->GameViewport->AddViewportWidgetContent(Background.ToSharedRef(), 9);
}

void UXsollaPluginWebBrowserWrapper::CloseShop(bool bCheckTransactionResult)
{
    if (OnShopClosed.IsBound())
    {
        OnShopClosed.Execute();
    }
}

void UXsollaPluginWebBrowserWrapper::HandleOnUrlChanged(const FText& InText)
{
    UE_LOG(LogTemp, Warning, TEXT("URL: %s"), *(WebBrowserWidget->GetUrl()));

    if (WebBrowserWidget->GetUrl().Contains("www.unrealengine"))
    {
        CloseShop(false);
    }

    if (!WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->ApiUrl) && !WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->SandboxApiUrl))
    {
        HomeButton->SetVisibility(EVisibility::Visible);
    }

    if (WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->ApiUrl) || WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->SandboxApiUrl))
    {
        HomeButton->SetVisibility(EVisibility::Hidden);
    }

    OnUrlChanged.Broadcast(InText);
}
void UXsollaPluginWebBrowserWrapper::HandleOnLoadCompleted()
{
    if (WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->ApiUrl) || WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->SandboxApiUrl))
    {
        //BrowserSlot.DetachWidget();
        //BrowserSlot.AttachWidget(WebBrowserWidget.ToSharedRef());
        //BrowserSlot.Padding((ViewportSize.X - ContentSize.X) / 2, (ViewportSize.Y - ContentSize.Y) / 2);

        CloseButton->SetVisibility(EVisibility::Visible);

        ULocalPlayer* player = GEngine->GetFirstGamePlayer(GEngine->GameViewport);

        if (player != NULL)
        {
            player->GetSlateOperations().SetUserFocus(WebBrowserWidget.ToSharedRef());
            player->GetSlateOperations().LockMouseToWidget(MainContent.ToSharedRef());
            player->GetSlateOperations().ReleaseMouseCapture();
        }
    }

    OnLoadCompleted.Broadcast();
}
void UXsollaPluginWebBrowserWrapper::HandleOnLoadError()
{
    OnLoadError.Broadcast();
}
bool UXsollaPluginWebBrowserWrapper::HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& NewBrowserWindow)
{
    UE_LOG(LogTemp, Warning, TEXT("Window closed."));

    OnCloseWindow.Broadcast();
    return true;
}

void UXsollaPluginWebBrowserWrapper::SetBrowserSize(float w, float h)
{
    ContentSize.X = w;
    ContentSize.Y = h;
}

void UXsollaPluginWebBrowserWrapper::Clear()
{
    ULocalPlayer* player = GEngine->GetFirstGamePlayer(GEngine->GameViewport);

    if (player != NULL)
    {
        GEngine->GameViewport->SetIgnoreInput(bPrevGameViewportInputIgnoring);

        player->GetSlateOperations().SetUserFocus(PrevFocusedWidget.ToSharedRef());
        // player->GetSlateOperations().LockMouseToWidget(PrevFocusedWidget.ToSharedRef());
        player->GetSlateOperations().ReleaseMouseCapture();
        player->GetSlateOperations().ReleaseMouseLock();
    }

    if (MainContent.IsValid() && Background.IsValid())
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(MainContent.ToSharedRef());
        GEngine->GameViewport->RemoveViewportWidgetContent(Background.ToSharedRef());
    }
}

void UXsollaPluginWebBrowserWrapper::HandleOnHomeButtonClicked()
{
    if (!PopupWidgets.empty())
    {
        BrowserOverlay->RemoveSlot(PopupWidgets.size());
        PopupWidgets.pop_back();

        if (PopupWidgets.empty())
        {
            if (WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->ApiUrl) || WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->SandboxApiUrl))
            {
                HomeButton->SetVisibility(EVisibility::Hidden);
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Popup closed"));
    }
}

bool UXsollaPluginWebBrowserWrapper::HandleOnPopupCreate(const TWeakPtr<IWebBrowserWindow>& window, const TWeakPtr<IWebBrowserPopupFeatures>& feat)
{
    UE_LOG(LogTemp, Warning, TEXT("HandleOnPopupCreate()"));

    TSharedPtr<IWebBrowserWindow> popupWindow = window.Pin();
    TSharedPtr<SWebBrowser> popupBrowser = SNew(SWebBrowser);

    SWebBrowser::FArguments args;
    args.ViewportSize(ContentSize);
    args.SupportsTransparency(bSupportsTransparency);
    args.ShowControls(false);
    //args.OnShowDialog_Lambda([](const TWeakPtr<IWebBrowserDialog>& dial) { return EWebBrowserDialogEventResponse::Ignore; });
    args.OnCloseWindow_Lambda([=](const TWeakPtr<IWebBrowserWindow>& win)
    { 
        BrowserOverlay->RemoveSlot(PopupWidgets.size());
        win.Pin()->CloseBrowser(true);
        PopupWidgets.pop_back();

        if (PopupWidgets.empty())
        {
            HomeButton->SetVisibility(EVisibility::Hidden);
        }

        return false; 
    });

    popupBrowser->Construct(args, popupWindow);

    PopupWidgets.push_back(popupBrowser);
    BrowserOverlay->AddSlot(PopupWidgets.size()).AttachWidget(popupBrowser.ToSharedRef());

    HomeButton->SetVisibility(EVisibility::Visible);

    return true;
}

#undef LOCTEXT_NAMESPACE