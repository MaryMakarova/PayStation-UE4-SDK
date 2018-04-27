#include "XsollaWebBrowserWrapper.h"

#include "XsollaShop.h"
#include "XsollaSettings.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Misc/Base64.h"
#include "XsollaTelegramScheme.h"

#define LOCTEXT_NAMESPACE "XsollaPluginWebBrowserWrapper"

UXsollaWebBrowserWrapper::UXsollaWebBrowserWrapper(const FObjectInitializer& objectInitializer)
    : Super(objectInitializer),
    ButtonSize(70.0f) // close and home buttons size in Slate units
{
    bIsVariable = true;
}

void UXsollaWebBrowserWrapper::NativeConstruct()
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

void UXsollaWebBrowserWrapper::LoadURL(FString newURL)
{
    if (WebBrowserWidget.IsValid())
    {
        WebBrowserWidget->LoadURL(newURL);
    }
}

void UXsollaWebBrowserWrapper::LoadSlateResources()
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

void UXsollaWebBrowserWrapper::ComposeShopWrapper()
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
                            .ContentPadding(FMargin(0, 0))
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
                            .ContentPadding(FMargin(0, 0))
                            .ButtonColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 0)))
                            .OnClicked_Lambda([this]() { this->HandleOnHomeButtonClicked(); return FReply::Handled(); })
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

void UXsollaWebBrowserWrapper::CloseShop(bool bCheckTransactionResult)
{
    if (OnShopClosed.IsBound())
    {
        OnShopClosed.Execute();
    }
}

void UXsollaWebBrowserWrapper::HandleOnUrlChanged(const FText& inText)
{
    UE_LOG(LogTemp, Warning, TEXT("New url: %s"), *(WebBrowserWidget->GetUrl()));

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

    OnUrlChanged.Broadcast(inText);
}
void UXsollaWebBrowserWrapper::HandleOnLoadCompleted()
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
void UXsollaWebBrowserWrapper::HandleOnLoadError()
{
    OnLoadError.Broadcast();
}
bool UXsollaWebBrowserWrapper::HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& newBrowserWindow)
{
    UE_LOG(LogTemp, Warning, TEXT("Window closed."));

    OnCloseWindow.Broadcast();
    return true;
}

void UXsollaWebBrowserWrapper::SetBrowserSize(float w, float h)
{
    ContentSize.X = w;
    ContentSize.Y = h;
}

void UXsollaWebBrowserWrapper::Clear()
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

void UXsollaWebBrowserWrapper::HandleOnHomeButtonClicked()
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
    else
    {
        this->LoadURL(XsollaPlugin::GetShop()->ShopUrl);
    }
}

bool UXsollaWebBrowserWrapper::HandleOnPopupCreate(const TWeakPtr<IWebBrowserWindow>& window, const TWeakPtr<IWebBrowserPopupFeatures>& feat)
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