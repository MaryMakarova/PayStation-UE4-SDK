#include "XsollaPluginBPLibrary.h"
#include "XsollaPluginShop.h"
#include "XsollaPlugin.h"

UXsollaPluginBPLibrary::UXsollaPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UXsollaPluginShop* UXsollaPluginBPLibrary::GetXsollaPlugin()
{
    return (UXsollaPluginShop*)UXsollaPluginShop::StaticClass()->GetDefaultObject();
}

