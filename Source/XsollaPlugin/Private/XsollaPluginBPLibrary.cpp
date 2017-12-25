#include "XsollaPluginBPLibrary.h"

UXsollaPluginBPLibrary::UXsollaPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UXsollaPluginShop* UXsollaPluginBPLibrary::GetXsollaPlugin()
{
    return (UXsollaPluginShop*)UXsollaPluginShop::StaticClass()->GetDefaultObject();
}

