#include "XsollaPlugin.h"

#include "XsollaPluginSettings.h"
#include "XsollaPluginShop.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "Developer/Settings/Public/ISettingsSection.h"

#define LOCTEXT_NAMESPACE "FXsollaPluginModule"

class FXsollaPluginModule : public IXsollaPluginModule
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterSettings();
    void UnregisterSettings();
};

IMPLEMENT_MODULE(FXsollaPluginModule, XsollaPlugin)

void FXsollaPluginModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    RegisterSettings();

    UXsollaPluginShop* Shop = NewObject<UXsollaPluginShop>(UXsollaPluginShop::StaticClass());
}

void FXsollaPluginModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
        
    if (UObjectInitialized())
    {
        UnregisterSettings();
    }
}

void FXsollaPluginModule::RegisterSettings()
{
    // Registering some settings is just a matter of exposing the default UObject of
    // your desired class, feel free to add here all those settings you want to expose
    // to your LDs or artists.

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        // Create the new category
        ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

        SettingsContainer->DescribeCategory("Xsolla",
            LOCTEXT("RuntimeWDCategoryName", "Xsolla Plugin"),
            LOCTEXT("RuntimeWDCategoryDescription", "Xsolla plugin settings"));

        // Register the settings
        ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Xsolla", "General",
            LOCTEXT("RuntimeGeneralSettingsName", "General"),
            LOCTEXT("RuntimeGeneralSettingsDescription", "Base configuration"),
            GetMutableDefault<UXsollaPluginSettings>()
        );
    }
}

void FXsollaPluginModule::UnregisterSettings()
{
    // Ensure to unregister all of your registered settings here, hot-reload would
    // otherwise yield unexpected results.

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Xsolla", "General");
    }
}

#undef LOCTEXT_NAMESPACE