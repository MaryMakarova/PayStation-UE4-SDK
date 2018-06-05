#include "XsollaTelegramScheme.h"

#include <string>

bool TelegramSchemeHandler::ProcessRequest(const FString& verb, const FString& url, const FSimpleDelegate& onHeadersReady)
{
#if PLATFORM_WINDOWS
    HKEY hKey;

    RegOpenKey(HKEY_CLASSES_ROOT, _T("tdesktop.tg\\shell\\open\\command"), &hKey);

    TCHAR regValue[1024];
    DWORD regValueLen = sizeof(regValue);

    RegQueryValueEx(hKey, _T(""), NULL, NULL, reinterpret_cast<LPBYTE>(&regValue), &regValueLen);

    FString telegramCommand(regValue);
    FString binaryPath;
    FString params = FString("-- ") + url;

    if (!telegramCommand.IsEmpty())
    {
        telegramCommand.RemoveFromStart(TEXT("\""));
        telegramCommand.Split(TEXT("\""), &binaryPath, NULL);

        FPlatformProcess::CreateProc(*binaryPath, *params, true, false, false, nullptr, 0, nullptr, nullptr);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cant find telegram."));
    }
#endif

#if PLATFORM_MAC
    FString telegramCommand = "open -a Telegram.app -- " + url;
    system(TCHAR_TO_ANSI(*telegramCommand));
#endif

    return false;
}