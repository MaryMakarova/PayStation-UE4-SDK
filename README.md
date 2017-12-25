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
4. Call `XsollaPlugin::GetShop()->Create(EShopSizeEnum::VE_Large|VE_Medium|VE_Small, OnSucceeded, OnCanceled, OnFailed);`

Code example
```c++
#include "XsollaPlugin.h"

...

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();

    // delegates are not binded, bind it before shop creating
    FOnPaymantSucceeded OnSucceeded;
    FOnPaymantCanceled OnCanceled;
    FOnPaymantFailed OnFailed;

    XsollaPlugin::GetShop()->Create(EShopSizeEnum::VE_Large, OnSucceeded, OnCanceled, OnFailed);
}
```
