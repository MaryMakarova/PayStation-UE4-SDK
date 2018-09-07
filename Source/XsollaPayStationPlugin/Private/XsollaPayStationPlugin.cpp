#include "XsollaPayStationPlugin.h"

#include "XsollaPayStationSettings.h"
#include "XsollaPayStation.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "Developer/Settings/Public/ISettingsSection.h"

#include "WebBrowserModule.h"

#define LOCTEXT_NAMESPACE "FXsollaPayStationPluginModule"

class FXsollaPayStationPluginModule : public IXsollaPayStationPluginModule
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterSettings();
    void UnregisterSettings();
};

IMPLEMENT_MODULE(FXsollaPayStationPluginModule, XsollaPayStationPlugin)

void FXsollaPayStationPluginModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    RegisterSettings();

    // preload web browser module to avoid freeze
    FModuleManager::LoadModuleChecked<IWebBrowserModule>("WebBrowser").GetSingleton();
}

void FXsollaPayStationPluginModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
        
    if (UObjectInitialized())
    {
        UnregisterSettings();
    }
}

void FXsollaPayStationPluginModule::RegisterSettings()
{
    // Registering some settings is just a matter of exposing the default UObject of
    // your desired class, feel free to add here all those settings you want to expose
    // to your LDs or artists.

    if (ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        // Register the settings
        ISettingsSectionPtr settingsSection = settingsModule->RegisterSettings("Project", "Plugins", "Xsolla PayStation",
            LOCTEXT("RuntimeGeneralSettingsName", "Xsolla PayStation"),
            LOCTEXT("RuntimeGeneralSettingsDescription", "Base configuration"),
            GetMutableDefault<UXsollaPayStationSettings>()
        );
    }
}

void FXsollaPayStationPluginModule::UnregisterSettings()
{
    // Ensure to unregister all of your registered settings here, hot-reload would
    // otherwise yield unexpected results.

    if (ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        settingsModule->UnregisterSettings("Project", "Xsolla PayStation", "General");
    }
}

#undef LOCTEXT_NAMESPACE