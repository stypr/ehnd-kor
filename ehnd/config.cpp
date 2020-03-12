#include "stdafx.h"
#include "config.h"

config::config()
{
    // Default Font to Korean font
    wcscpy_s(cfg_console_fontname, L"±¼¸²");
}

bool config::LoadConfig()
{
    // Disable Console for Production
    SetConsoleSwitch(false);
    SetConsoleMaxLine(300);

    // Disable File logging for Production
    SetFileLogSwitch(false);
    SetFileLogSize(300);
    SetFileLogEztLoc(true);
    SetFileLogStartupClear(true);

    if (!firstInit) WriteLog(NORMAL_LOG, L"LoadConfig : Success.\n");
    else firstInit = false;
    return true;
}