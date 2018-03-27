#include "XsollaPluginWebBrowserWrapper.h"

#include "XsollaPluginShop.h"
#include "XsollaPluginSettings.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Misc/Base64.h"

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
        .OnSuppressContextMenu_Lambda([]() { return true; })
        .OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforeNewWindow));

    ComposeShopWrapper();

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

    BrowserSlot.AttachWidget(WebBrowserWidget.ToSharedRef());

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

bool UXsollaPluginWebBrowserWrapper::HandleOnBeforeNewWindow(FString Url, FString param)
{
    AsyncTask(ENamedThreads::GameThread, [=]() 
    {
        PopupWidgets.push_back(
            SNew(SWebBrowser)
            .InitialURL(Url)
            .ShowControls(false)
            .ViewportSize(ContentSize)
            .SupportsTransparency(bSupportsTransparency)
        );

        BrowserSlot.DetachWidget();
        BrowserSlot.AttachWidget(PopupWidgets.back().ToSharedRef());

        HomeButton->SetVisibility(EVisibility::Visible);

        UE_LOG(LogTemp, Warning, TEXT("New popup with url: %s"), *Url);
    });
    
    return false;
}

void UXsollaPluginWebBrowserWrapper::HandleOnHomeButtonClicked()
{
    if (!PopupWidgets.empty())
    {
        PopupWidgets.pop_back();

        if (PopupWidgets.empty())
        {
            BrowserSlot.DetachWidget();
            BrowserSlot.AttachWidget(WebBrowserWidget.ToSharedRef());

            if (WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->ApiUrl) || WebBrowserWidget->GetUrl().StartsWith(XsollaPlugin::GetShop()->SandboxApiUrl))
            {
                HomeButton->SetVisibility(EVisibility::Hidden);
            }
        }
        else
        {
            BrowserSlot.DetachWidget();
            BrowserSlot.AttachWidget(PopupWidgets.back().ToSharedRef());
        }

        UE_LOG(LogTemp, Warning, TEXT("Popup closed"));
    }
}

#undef LOCTEXT_NAMESPACE