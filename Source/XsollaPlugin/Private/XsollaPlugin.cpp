// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "XsollaPlugin.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "XsollaPluginSettings.h"

#define LOCTEXT_NAMESPACE "FXsollaPluginModule"

void FXsollaPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	RegisterSettings();
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

bool FXsollaPluginModule::HandleSettingsSaved()
{
	UXsollaPluginSettings* Settings = GetMutableDefault<UXsollaPluginSettings>();
	bool ResaveSettings = false;

	// You can put any validation code in here and resave the settings in case an invalid
	// value has been entered

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	return true;
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

		// Register the save handler to your settings, you might want to use it to
		// validate those or just act to settings changes.
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FXsollaPluginModule::HandleSettingsSaved);
		}
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
	
IMPLEMENT_MODULE(FXsollaPluginModule, XsollaPlugin)