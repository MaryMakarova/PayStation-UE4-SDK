#pragma once

#include "Runtime/UMG/Public/UMG.h"
#include "Slate/SlateGameResources.h"
#include "XsollaPluginHttpTool.h"
#include "XsollaPluginTransactionDetails.h"

#include "XsollaPluginWebBrowserWrapper.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPaymantSucceeded, FTransactionDetails, transactionDetails);
DECLARE_DYNAMIC_DELEGATE(FOnPaymantCanceled);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPaymantFailed, FString, errorText);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTransactionsGetSucceeded, FTransactionDetails, transactionDetails);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTransactionsGetFailed, FString, errorText);

class IWebBrowserWindow;

UCLASS()
class XSOLLAPLUGIN_API UXsollaPluginWebBrowserWrapper : public UUserWidget
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
    * Load new url.
    *
    * @param NewURL - Url to load.
    */
    void LoadURL(FString NewURL);

    /**
    * Set shop url.
    *
    * @param str - Shop url string.
    */
    void SetShopUrl(FString str);

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
    void CloseShop(bool bCheckTransactionResult);

public:
    UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
        FOnUrlChanged OnUrlChanged;

    UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
        FOnLoadCompleted OnLoadCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
        FOnLoadError OnLoadError;

    UPROPERTY(BlueprintAssignable, Category = "Web Browser|Event")
        FOnCloseWindow OnCloseWindow;

    // shop delegates
    FOnPaymantSucceeded OnSucceeded;
    FOnPaymantCanceled OnCanceled;
    FOnPaymantFailed OnFailed;

    FString ExternalId;

private:
    void LoadSlateResources();
    void ComposeShopWrapper();

    void HandleOnUrlChanged(const FText& Text);
    void HandleOnLoadCompleted();
    void HandleOnLoadError();
    bool HandleOnCloseWindow(const TWeakPtr<IWebBrowserWindow>& BrowserWindow);

private:
    TSharedPtr<class SVerticalBox>		MainContent;
    TSharedPtr<class SWebBrowser>		WebBrowserWidget;
    TSharedPtr<class SSpinningImage>	SpinnerImage;
    TSharedPtr<class SButton>			CloseButton;
    TSharedPtr<class SButton>			HomeButton;
    TSharedPtr<class SVerticalBox>		Background;

    const FSlateBrush*                  SlateCloseBrush;
    const FSlateBrush*                  SlateBackBrush;
    const FSlateBrush*                  SlateSpinnerBrush;

    SHorizontalBox::FSlot& BrowserSlot				= SHorizontalBox::Slot();
    SHorizontalBox::FSlot& BrowserSlotMarginLeft	= SHorizontalBox::Slot();
    SHorizontalBox::FSlot& BrowserSlotMarginRight	= SHorizontalBox::Slot();

    float		ButtonSize;
    FVector2D	ViewportSize;
    FVector2D	ContentSize;
    FString		ShopSize;

    FString		InitialURL = "";
    FString		ShopUrl;
    bool		bSupportsTransparency = true;
};