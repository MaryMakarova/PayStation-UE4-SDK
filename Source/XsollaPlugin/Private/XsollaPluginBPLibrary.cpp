#include "XsollaPluginBPLibrary.h"

UXsollaPluginBPLibrary::UXsollaPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UXsollaPluginShop* UXsollaPluginBPLibrary::GetXsollaPluginShop()
{
    return (UXsollaPluginShop*)UXsollaPluginShop::StaticClass()->GetDefaultObject();
}

