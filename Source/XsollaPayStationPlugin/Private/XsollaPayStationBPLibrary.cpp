#include "XsollaPayStationBPLibrary.h"

#include "XsollaPayStation.h"

#if PLATFORM_MAC
#include <array>
#include <string>
#include <cstdlib>
#include <cstdio>
#endif

#if PLATFORM_MAC
bool IsMacOSVersionEqualOrHigher(int majorVer, int minorVer = -1, int patchVer = -1)
{
	// buffer for storing popen output
	std::array<char, 128> buffer;

	// result string with os information in format MAJOR.MINOR.[PATCH]
	std::string result;

	// parsed version
	std::vector<int> version;

	FILE* pipe = popen("sw_vers -productVersion", "r");
	if (!pipe)
	{
		return false;
	}

	while (fgets(buffer.data(), 128, pipe) != NULL)
	{
		result += buffer.data();
	}

	pclose(pipe);

	size_t pos;
	std::string token;
	while ((pos = result.find( std::string(".") )) != std::string::npos)
	{
		token = result.substr(0, pos);
		version.push_back( std::stoi(token) );
		result.erase(0, pos + std::string(".").length());
	}

	UE_LOG(LogTemp, Warning, TEXT("macOS version: %d.%d"), version[0], version[1]);

	bool bIsEqualOrHigher = false;

	if (version.size() > 0)
	{
		bIsEqualOrHigher = version[0] >= majorVer;
	}

	if (version.size() > 1 && minorVer != -1)
	{
		bIsEqualOrHigher = version[1] >= minorVer;
	}

	if (version.size() > 2 && patchVer != -1)
	{
		bIsEqualOrHigher = version[2] >= patchVer;
	}

	return bIsEqualOrHigher;
}
#endif

UXsollaPayStationBPLibrary::UXsollaPayStationBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UXsollaPayStationBPLibrary::OpenXsollaPayStation(EShopSizeEnum shopSize, FString userId, FOnPayStationClosed onPayStationClosed)
{
#if PLATFORM_MAC
	if (IsMacOSVersionEqualOrHigher(10, 14))
	{
		XsollaPayStationPlugin::Get()->CreateInSystemDefaultBrowser(userId);
	}
	else
#endif
	{
		if (!XsollaPayStationPlugin::Get()->bIsShopOpen)
		{
			XsollaPayStationPlugin::Get()->Create(shopSize, userId, onPayStationClosed);
		}
	}
}