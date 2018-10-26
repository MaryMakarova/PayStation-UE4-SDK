#include "WebBrowserWrapper.h"

#include "XsollaPayStation.h"
#include "XsollaPayStationSettings.h"
#include "SWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Misc/Base64.h"
#include "XsollaTelegramScheme.h"
#include "XsollaPayStationPlugin.h"

#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"
#include "Runtime/Engine/Classes/Engine/RendererSettings.h"

#define LOCTEXT_NAMESPACE "WebBrowserWrapper"

FSlateDynamicImageBrush* g_CloseBrush;
FSlateDynamicImageBrush* g_BackBrush;

UWebBrowserWrapper::UWebBrowserWrapper(const FObjectInitializer& objectInitializer)
    : Super(objectInitializer),
    ButtonSize(GetDefault<UXsollaPayStationSettings>()->ButtonSize)
{
    bIsVariable = true;

    UTexture2D* closeImage;
    UTexture2D* backImage;

    ConstructorHelpers::FObjectFinder<UTexture2D> closeImageFinder(TEXT("Texture2D'/XsollaPayStationPlugin/Buttons/close'"));
    ConstructorHelpers::FObjectFinder<UTexture2D> backImageFinder(TEXT("Texture2D'/XsollaPayStationPlugin/Buttons/back'"));

    closeImage = closeImageFinder.Object;
    backImage = backImageFinder.Object;

    g_CloseBrush = new FSlateDynamicImageBrush(closeImage, FVector2D(512, 512), FName("close"));
    g_BackBrush = new FSlateDynamicImageBrush(backImage, FVector2D(512, 512), FName("back"));
}

void UWebBrowserWrapper::NativeConstruct()
{
    Super::NativeConstruct();
}

void UWebBrowserWrapper::Open()
{
    ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

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
}

void UWebBrowserWrapper::LoadURL(FString newURL)
{
    if (WebBrowserWidget.IsValid())
    {
        WebBrowserWidget->LoadURL(newURL);
    }
}

void UWebBrowserWrapper::ComposeShopWrapper()
{
    float viewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
    FSlateRenderTransform renderTransform(FScale2D(viewportScale), FVector2D(0.f, 0.f));

    BrowserOverlay = SNew(SOverlay);
    BrowserOverlay->AddSlot(0)
    [
        WebBrowserWidget.ToSharedRef()
    ];

    FVector2D offset = (ViewportSize - ContentSize * viewportScale) / 2 / viewportScale;

    MainContent = 
            SNew(SConstraintCanvas)
            .RenderTransform(renderTransform)
            .RenderTransformPivot(FVector2D(0.f, 0.f))
            + SConstraintCanvas::Slot()
            .Offset(FMargin(offset.X / viewportScale, offset.Y / viewportScale, ContentSize.X / viewportScale, ContentSize.Y / viewportScale))
            .Anchors(FAnchors(0.f, 0.f))
            .Alignment(FVector2D(0.f, 0.f))
            [
                BrowserOverlay.ToSharedRef()
            ]
            + SConstraintCanvas::Slot()
            .Anchors(FAnchors(0.f, 0.f))
            .Offset(FMargin((ContentSize.X + offset.X) / viewportScale, offset.Y / viewportScale, ButtonSize / viewportScale, ButtonSize / viewportScale))
            .Alignment(FVector2D(0.f, 0.f))
            [
                SAssignNew(CloseButton, SButton)
                .ButtonColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 0)))
                .ContentPadding(FMargin(5.f, 5.f))
                .Visibility(EVisibility::Hidden)
                .OnClicked_UObject(this, &UWebBrowserWrapper::CloseShop)
                .Content()
                [
                    SNew(SImage).Image(g_CloseBrush)
                ]
            ]
            + SConstraintCanvas::Slot()
            .Anchors(FAnchors(0.f, 0.f))
            .Offset(FMargin((ContentSize.X + offset.X) / viewportScale, (offset.Y + ButtonSize) / viewportScale, ButtonSize / viewportScale, ButtonSize / viewportScale))
            .Alignment(FVector2D(0.f, 0.f))
            [
                SAssignNew(HomeButton, SButton)
                .ButtonColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 0)))
                .ContentPadding(FMargin(5.f, 5.f))
                .Visibility(EVisibility::Hidden)
                .OnClicked_UObject(this, &UWebBrowserWrapper::HandleOnHomeButtonClicked)
                .Content()
                [
                    SNew(SImage).Image(g_BackBrush)
                ]
            ];

    Background = 
        SNew(SVerticalBox)
        + SVerticalBox::Slot().FillHeight(1.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1.f)
            [
                SNew(SColorBlock).Color(FLinearColor(0.f, 0.f, 0.f, 0.5f))
            ]
        ];

    GEngine->GameViewport->AddViewportWidgetContent(Background.ToSharedRef(), 9998);
    GEngine->GameViewport->AddViewportWidgetContent(MainContent.ToSharedRef(), 9999);

    bPrevShowMouseCursor = UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor;
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

    //FSlateApplication::Get().SetKeyboardFocus(WebBrowserWidget.ToSharedRef());

    ULocalPlayer* player = GEngine->GetFirstGamePlayer(GEngine->GameViewport);

    if (player != NULL && MainContent.IsValid() && WebBrowserWidget.IsValid())
    {
        player->GetSlateOperations().SetUserFocus(WebBrowserWidget.ToSharedRef());
        //player->GetSlateOperations().LockMouseToWidget(MainContent.ToSharedRef());
        //player->GetSlateOperations().ReleaseMouseCapture();
    }
}

FReply UWebBrowserWrapper::CloseShop()
{
    if (OnShopClosed.IsBound())
    {
        OnShopClosed.Execute();
    }

    return FReply::Handled();
}

void UWebBrowserWrapper::HandleOnUrlChanged(const FText& inText)
{
    UE_LOG(LogTemp, Warning, TEXT("New url: %s"), *(WebBrowserWidget->GetUrl()));

    if (!WebBrowserWidget->GetUrl().StartsWith(XsollaPayStationPlugin::GetShop()->ApiUrl.LeftChop(1)) 
		&& !WebBrowserWidget->GetUrl().StartsWith(XsollaPayStationPlugin::GetShop()->SandboxApiUrl.LeftChop(1)))
    {
        HomeButton->SetVisibility(EVisibility::Visible);
    }

    if (WebBrowserWidget->GetUrl().StartsWith(XsollaPayStationPlugin::GetShop()->ApiUrl.LeftChop(1))
		|| WebBrowserWidget->GetUrl().StartsWith(XsollaPayStationPlugin::GetShop()->SandboxApiUrl.LeftChop(1)))
    {
        HomeButton->SetVisibility(EVisibility::Hidden);
    }

    OnUrlChanged.Broadcast(inText);

    if (WebBrowserWidget->GetUrl().Contains("www.unrealengine"))
    {
        CloseShop();
    }
}

void UWebBrowserWrapper::HandleOnLoadCompleted()
{
    CloseButton->SetVisibility(EVisibility::Visible);

    OnLoadCompleted.Broadcast();
}

void UWebBrowserWrapper::HandleOnLoadError()
{
    OnLoadError.Broadcast();
}

bool UWebBrowserWrapper::HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& newBrowserWindow)
{
    UE_LOG(LogTemp, Warning, TEXT("Window closed."));

    OnCloseWindow.Broadcast();
    return true;
}

void UWebBrowserWrapper::SetBrowserSize(float w, float h)
{
    ContentSize.X = w;
    ContentSize.Y = h;
}

void UWebBrowserWrapper::Clear()
{
    ULocalPlayer* player = GEngine->GetFirstGamePlayer(GEngine->GameViewport);

    if (player != NULL)
    {
        GEngine->GameViewport->SetIgnoreInput(bPrevGameViewportInputIgnoring);

        if (PrevFocusedWidget.IsValid())
        {
            player->GetSlateOperations().SetUserFocus(PrevFocusedWidget.ToSharedRef());
        }

        // player->GetSlateOperations().LockMouseToWidget(PrevFocusedWidget.ToSharedRef());
        player->GetSlateOperations().ReleaseMouseCapture();
        player->GetSlateOperations().ReleaseMouseLock();

        UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = bPrevShowMouseCursor;
    }

    if (MainContent.IsValid() && Background.IsValid())
    {
        GEngine->GameViewport->RemoveViewportWidgetContent(MainContent.ToSharedRef());
        GEngine->GameViewport->RemoveViewportWidgetContent(Background.ToSharedRef());
    }

    PopupWidgets.clear();

	XsollaPayStationPlugin::GetShop()->bIsShopOpen = false;
}

FReply UWebBrowserWrapper::HandleOnHomeButtonClicked()
{
    if (!PopupWidgets.empty())
    {
        BrowserOverlay->RemoveSlot(PopupWidgets.size());
        PopupWidgets.pop_back();

        if (PopupWidgets.empty())
        {
			if (WebBrowserWidget->GetUrl().StartsWith(XsollaPayStationPlugin::GetShop()->ApiUrl.LeftChop(1)) || WebBrowserWidget->GetUrl().StartsWith(XsollaPayStationPlugin::GetShop()->SandboxApiUrl.LeftChop(1)))
			{
                HomeButton->SetVisibility(EVisibility::Hidden);
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Popup closed"));
    }
    else
    {
        this->LoadURL(XsollaPayStationPlugin::GetShop()->ShopUrl);
    }

    return FReply::Handled();
}

bool UWebBrowserWrapper::HandleOnPopupCreate(const TWeakPtr<IWebBrowserWindow>& window, const TWeakPtr<IWebBrowserPopupFeatures>& feat)
{
    UE_LOG(LogTemp, Warning, TEXT("HandleOnPopupCreate()"));

    TSharedPtr<IWebBrowserWindow> popupWindow = window.Pin();
    TSharedPtr<SWebBrowser> popupBrowser = SNew(SWebBrowser);

    SWebBrowser::FArguments args;
    args.ViewportSize(ContentSize);
    args.SupportsTransparency(bSupportsTransparency);
    args.ShowControls(false);
    //args.OnShowDialog_Lambda([](const TWeakPtr<IWebBrowserDialog>& dial) { return EWebBrowserDialogEventResponse::Ignore; });
    args.OnCreateWindow(BIND_UOBJECT_DELEGATE(FOnCreateWindowDelegate, HandleOnPopupCreate));
    args.OnSuppressContextMenu_Lambda([]() { return true; });
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