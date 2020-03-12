// dllmain.cpp : DLL ���� ���α׷��� �������� �����մϴ�.
#include "stdafx.h"
HINSTANCE g_hInst;
config *pConfig;
HMODULE hEzt, hMsv;
int g_initTick;
BOOL initOnce = false;

BOOL APIENTRY DllMain(HINSTANCE hInstance,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = hInstance;

        // init ehnd
        pConfig = new config();

        char szInitTick[12];
        g_initTick = GetTickCount() + rand();
        _itoa_s(g_initTick, szInitTick, 10);
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        FreeLibrary(hEzt);
        FreeLibrary(hMsv);
        break;

    }
    return TRUE;
}
