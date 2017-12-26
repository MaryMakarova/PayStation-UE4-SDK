# Xsolla Plugin

### Install 
Clone or download and unpack plugin to `{YourProject}/Plugins/`

### Setting up
Open plugin settings `Setting > Project Settings > Xsolla Plugin > General`, then set `merchant id`, `project id` and `api key`. If you want to use shop in sandbox mode, check `Sandbox Mode` checkbox. 

### How to use in blueprint
`Xsolla Plugin BP Library` has 1 blueprint function.
* `Get Xsolla Plugin Shop` - returns `Xsolla Plugin Shop` reference.

`Xsolla Plugin Shop` has 2 available blueprint functions:
* `Create` - Gets shop token, set default properties and creates widget with shop content. Takes shop size and delegates for payment succeeded|canceled|failed as parameters. 
* `Set(Num|Bool|String)Property` - Set properties in json, which is sent as token parameters, see [token](https://developers.xsolla.com/ru/api_v2.html#token). Should be called before `Create` function.

### Example
![Blueprint example](https://github.com/SomeInc/XsollaUE4Plugin/blob/master/blueprint_example.png)

### How to use in c++
#### Visual Studio
1. Generate project files *(right click on yourproject.uproject > Generate Visual Studio project files)*.
2. Add `XsollaPlugin` to the `ExtraModuleNames` in `YourProject.Target.cs` and `YourProjectEditor.Target.cs`.
3. Add `XsollaPlugin` to the `PublicDependencyModuleNames` or `PrivateDependencyModuleNames` in `YourModule.Build.cs`*(module where you want to use the plugin)*.
3. Include `XsollaPlugin.h`.
4. Call `XsollaPlugin::GetShop()->Create(EShopSizeEnum::VE_Large, OnSucceeded, OnCanceled, OnFailed);` 
Shop size can be `EShopSizeEnum::VE_Large`, `EShopSizeEnum::VE_Medium`, `EShopSizeEnum::VE_Small`.

Code example
```c++
---------------- HEADER -----------------
...

    UFUNCTION(BlueprintCallable)
    void OnSucceededCallback(FTransactionDetails transactionDetails);

    UFUNCTION(BlueprintCallable)
    void OnCanceledCallback();

    UFUNCTION(BlueprintCallable)
    void OnFailedCallback(FString errorText, int32 errorCode);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
...
```

```c++
---------------- SOURCE -----------------
...

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    FOnPaymantSucceeded OnSucceeded;
    FOnPaymantCanceled OnCanceled;
    FOnPaymantFailed OnFailed;

    OnSucceeded.BindUFunction(this, "OnSucceededCallback");
    OnCanceled.BindUFunction(this, "OnCanceledCallback");
    OnFailed.BindUFunction(this, "OnFailedCallback");

    XsollaPlugin::GetShop()->Create(EShopSizeEnum::VE_Large, OnSucceeded, OnCanceled, OnFailed);
    
}

void AMyActor::OnSucceededCallback(FTransactionDetails transactionDetails)
{
    UE_LOG(LogTemp, Warning, TEXT("Payment succeded. Transaction id: %d"), transactionDetails.TransactionId);
}

void AMyActor::OnCanceledCallback()
{
    UE_LOG(LogTemp, Warning, TEXT("Payment canceled"));
}

void AMyActor::OnFailedCallback(FString errorText, int32 errorCode)
{
    UE_LOG(LogTemp, Warning, TEXT("Payment failed. Error text: %s"), *errorText);
}

...
```
