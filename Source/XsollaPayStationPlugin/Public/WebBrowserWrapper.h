// Copyright 2019 Xsolla Inc. All Rights Reserved.
// <support@xsolla.com>
#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Slate/SlateGameResources.h"
#include "XsollaHttpTool.h"

#include <vector>

#include "WebBrowserWrapper.generated.h"

DECLARE_DELEGATE(FOnShopClosed);

class IWebBrowserWindow;

UCLASS()
class XSOLLAPAYSTATIONPLUGIN_API UWebBrowserWrapper : public UUserWidget
{
    GENERATED_UCLASS_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadCompleted);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadError);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseWindow);

    /**
    * Called when widget create.
    */
    virtual void NativeConstruct() override;

    /**
    * Open shop 
    */
    void Open();

    /**
    * Load new url.
    *
    * @param newURL - Url to load.
    */
    void LoadURL(FString newURL);

    /**
    * Set browser wrapper size.
    *
    * @param w - Width.
    * @param h - Height.
    */
    void SetBrowserSize(float w, float h);

    /**
    * Remove widget from viewport
    *
    * @param bCheckTransactionResult - Check transaction result after shop close or not
    */
    FReply CloseShop();

    /**
    * Removes all widgets
    */
    void Clear();

public:
    UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
    FOnUrlChanged OnUrlChanged;

    UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
    FOnLoadCompleted OnLoadCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
    FOnLoadError OnLoadError;

    UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
    FOnCloseWindow OnCloseWindow;

    FOnShopClosed OnShopClosed;

private:
    void ComposeShopWrapper();

    void HandleOnUrlChanged(const FText& text);
    void HandleOnLoadCompleted();
    void HandleOnLoadError();
    bool HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& browserWindow);
    FReply HandleOnHomeButtonClicked();

    bool HandleOnPopupCreate(const TWeakPtr<class IWebBrowserWindow>& window, const TWeakPtr<class IWebBrowserPopupFeatures>& feat);

private:
    TSharedPtr<class SConstraintCanvas>     MainContent;
    TSharedPtr<class SWebBrowser>           WebBrowserWidget;
    TSharedPtr<class SVerticalBox>          Background;
    TSharedPtr<class SOverlay>              BrowserOverlay; 
    TSharedPtr<class SButton>               CloseButton;
    TSharedPtr<class SButton>               HomeButton;

    FVector2D   ViewportSize;
    FVector2D   ContentSize;
    FString     ShopSize;
    float       ButtonSize;

    FString     InitialURL = "";
    bool        bSupportsTransparency = false;

    std::vector<TSharedPtr<class SWebBrowser>> PopupWidgets;

    TSharedPtr<SWidget> PrevFocusedWidget;
    bool                bPrevGameViewportInputIgnoring;
    bool                bPrevShouldUseHighPresCapture = true;
    bool                bPrevShowMouseCursor = false;
};