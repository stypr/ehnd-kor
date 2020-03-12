#include "stdafx.h"
#include "hook.h"
#include "ehnd.h"

LPBYTE lpfnRetn, lpfnfopen;
LPBYTE lpfnwc2mb, lpfnmb2wc;
LPBYTE lpfnWordInfo;
int wc2mb_type = 0, mb2wc_type = 0;

bool hook()
{
    HMODULE hDll, hDll2;
    TCHAR lpEztPath[MAX_PATH], lpDllPath[MAX_PATH];
    LPCSTR aEztFunction[] = {
        "K2J_Initialize",
        "K2J_InitializeEx",
        "K2J_FreeMem",
        "K2J_GetPriorDict",
        "K2J_GetProperty",
        "K2J_ReloadUserDict",
        "K2J_SetPriorDict",
        "K2J_SetProperty",
        "K2J_StopTranslation",
        "K2J_Terminate",
        "K2J_TranslateChat",
        "K2J_TranslateMM",
        "K2J_TranslateMMEx",
        "K2J_TranslateMMNT" };
    LPCSTR aMsvFunction[] = {
        "free",
        "malloc",
        "fopen" };
    int i;

    GetLoadPath(lpEztPath, MAX_PATH);

    wcscpy_s(lpDllPath, lpEztPath);
    wcscat_s(lpDllPath, L"\\k2jengine.dlx");
    hDll = LoadLibrary(lpDllPath);
    if (!hDll)
    {
        MessageBox(0, L"K2JEngine.dlx Load Failed", L"EzTransHook", MB_ICONERROR);
        return false;
    }

    for (i = 0; i < _countof(aEztFunction); i++)
    {
        apfnEzt[i] = GetProcAddress(hDll, aEztFunction[i]);
        if (!apfnEzt[i])
        {
            MessageBox(0, L"K2JEngine.dlx Function Load Failed", L"EzTransHook", MB_ICONERROR);
            return false;
        }
    }

    lpDllPath[0] = 0;
    GetSystemDirectory(lpDllPath, MAX_PATH);
    wcscat_s(lpDllPath, L"\\msvcrt.dll");
    hDll2 = LoadLibrary(lpDllPath);
    if (!hDll2)
    {
        MessageBox(0, L"MSVCRT.DLL Load Failed", L"EzTransHook", MB_ICONERROR);
        return false;
    }

    for (i = 0; i < _countof(aMsvFunction); i++)
    {
        apfnMsv[i] = GetProcAddress(hDll2, aMsvFunction[i]);
        if (!apfnMsv[i])
        {
            MessageBox(0, L"MSVCRT.DLL Function Load Failed", L"EzTransHook", MB_ICONERROR);
            return false;
        }
    }

    hEzt = hDll;
    hMsv = hDll2;

    return true;
}

int search_ptn(LPWORD ptn, size_t ptn_size, LPBYTE *addr)
{
    HMODULE hDll = GetModuleHandle(L"k2Jengine.dlx");
    if (hDll == NULL) MessageBox(0, L"K2JEngine.dlx Load Failed", L"", 0);

    MODULEINFO dllInfo;
    GetModuleInformation(GetCurrentProcess(), hDll, &dllInfo, sizeof(dllInfo));

    // Modified BMH - http://en.wikipedia.org/wiki/Boyer-Moore-Horspool_algorithm

    UINT i;
    int scan;
    LPBYTE p;

    UINT defSkipLen;
    UINT skipLen[UCHAR_MAX + 1];
    UINT searchSuccessCount;

    UINT ptnEnd = ptn_size - 1;
    while ((HIBYTE(ptn[ptnEnd]) != 0x00) && (ptnEnd > 0)) {
        ptnEnd--;
    }
    defSkipLen = ptnEnd;
    for (i = 0; i < ptnEnd; i++)
    {
        if (HIBYTE(ptn[i]) != 0x00) {
            defSkipLen = ptnEnd - i;
        }
    }

    for (i = 0; i < UCHAR_MAX + 1; i++)
    {
        skipLen[i] = defSkipLen;
    }

    for (i = 0; i < ptnEnd; i++)
    {
        if (HIBYTE(ptn[i]) == 0x00)
        {
            skipLen[LOBYTE(ptn[i])] = ptnEnd - i;
        }
    }

    searchSuccessCount = 0;
    p = (LPBYTE)dllInfo.lpBaseOfDll;
    LPBYTE searchEnd = (LPBYTE)dllInfo.lpBaseOfDll + dllInfo.SizeOfImage;

    while (p + ptn_size < searchEnd)
    {
        scan = ptnEnd;
        while (scan >= 0)
        {
            if ((HIBYTE(ptn[scan]) == 0x00) && (LOBYTE(ptn[scan]) != p[scan]))
                break;
            if (scan == 0)
            {
                *addr = p;
                searchSuccessCount++;
            }
            scan--;
        }
        p += skipLen[p[ptnEnd]];
    }
    if (searchSuccessCount != 1) addr = 0;
    return searchSuccessCount;
}

bool GetRealWC2MB(void)
{
    HMODULE hDll = GetModuleHandle(L"kernel32.dll");
    lpfnwc2mb = (LPBYTE)GetProcAddress(hDll, "WideCharToMultiByte");

    // KERNEL32 HOOK
    // 756770D0 >  8BFF            MOV EDI, EDI
    // 756770D2    55              PUSH EBP
    // 756770D3    8BEC            MOV EBP, ESP

    BOOL bMatch = true;
    WORD ptn_kernel32[] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x5D };

    for (int i = 0; i < _countof(ptn_kernel32); i++)
        if (*(lpfnwc2mb + i) != ptn_kernel32[i])
        {
            bMatch = false;
            break;
        }

    if (bMatch)
    {
        lpfnwc2mb += 6;
        wc2mb_type = 1;
        return true;
    }

    // AILAYER HOOK
    // 230083D2    55              PUSH EBP
    // 230083D3    8BEC            MOV EBP, ESP
    // 230083D5    833D A8360123 0>CMP DWORD PTR DS : [230136A8], 0
    // 230083DC    74 11           JE SHORT AlLayer.230083EF
    // 230083DE    817D 08 E9FD000>CMP DWORD PTR SS : [EBP + 8], 0FDE9
    // 230083E5    74 08           JE SHORT AlLayer.230083EF


    bMatch = true;
    WORD ptn_ailayer[] = { 0x55, 0x8B, 0xEC, 0x83, 0x3D };

    for (int i = 0; i < _countof(ptn_ailayer); i++)
        if (*(lpfnwc2mb + i) != ptn_ailayer[i])
        {
            bMatch = false;
            break;
        }

    if (bMatch)
    {
        if (*(lpfnwc2mb + 0x1E) == 0xA1)
        {
            LPBYTE l1, l2, l3;
            char *p = (char *)&l1;
            for (int i = 0; i < 4; i++)
                p[i] = *(lpfnwc2mb + 0x1F + i);
            p = (char *)&l2;
            for (int i = 0; i < 4; i++)
                p[i] = *(l1 + i);
            l2 += 0x15C;
            p = (char *)&l3;
            for (int i = 0; i < 4; i++)
                p[i] = *(l2 + i);

            bMatch = true;
            for (int i = 0; i < _countof(ptn_kernel32); i++)
                if (*(l3 + i) != ptn_kernel32[i])
                {
                    bMatch = false;
                    break;
                }

            if (bMatch)
            {
                lpfnwc2mb = l3 + 6;
                wc2mb_type = 1;
                return true;
            }
        }
    }
    return true;
}

bool GetRealMB2WC(void)
{
    HMODULE hDll = GetModuleHandle(L"kernel32.dll");
    lpfnmb2wc = (LPBYTE)GetProcAddress(hDll, "MultiByteToWideChar");

    // KERNEL32 HOOK
    // 756770D0 >  8BFF            MOV EDI, EDI
    // 756770D2    55              PUSH EBP
    // 756770D3    8BEC            MOV EBP, ESP

    BOOL bMatch = true;
    WORD ptn_kernel32[] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC, 0x5D };

    for (int i = 0; i < _countof(ptn_kernel32); i++)
        if (*(lpfnmb2wc + i) != ptn_kernel32[i])
        {
            bMatch = false;
            break;
        }

    if (bMatch)
    {
        lpfnmb2wc += 6;
        mb2wc_type = 1;
        return true;
    }

    // AILAYER HOOK
    // 230083D2    55              PUSH EBP
    // 230083D3    8BEC            MOV EBP, ESP
    // 230083D5    833D A8360123 0>CMP DWORD PTR DS : [230136A8], 0
    // 230083DC    74 11           JE SHORT AlLayer.230083EF
    // 230083DE    817D 08 E9FD000>CMP DWORD PTR SS : [EBP + 8], 0FDE9
    // 230083E5    74 08           JE SHORT AlLayer.230083EF

    bMatch = true;
    WORD ptn_ailayer[] = { 0x55, 0x8B, 0xEC, 0x83, 0x3D };

    for (int i = 0; i < _countof(ptn_ailayer); i++)
        if (*(lpfnmb2wc + i) != ptn_ailayer[i])
        {
            bMatch = false;
            break;
        }

    if (bMatch)
    {
        if (*(lpfnmb2wc + 0x40) == 0xA1)
        {
            LPBYTE l1, l2, l3;
            char *p = (char*)&l1;
            for (int i = 0; i < 4; i++)
                p[i] = *(lpfnmb2wc + 0x41 + i);
            p = (char*)&l2;
            for (int i = 0; i < 4; i++)
                p[i] = *(l1 + i);
            l2 += 0x144;
            p = (char*)&l3;
            for (int i = 0; i < 4; i++)
                p[i] = *(l2 + i);

            bMatch = true;
            for (int i = 0; i < _countof(ptn_kernel32); i++)
                if (*(l3 + i) != ptn_kernel32[i])
                {
                    bMatch = false;
                    break;
                }

            if (bMatch)
            {
                lpfnmb2wc = l3 + 6;
                mb2wc_type = 1;
                return true;
            }
        }
    }
    return true;
}

__declspec(naked) int __stdcall _WideCharToMultiByte(UINT a, DWORD b, LPCWSTR c, int d, LPSTR e, int f, LPCSTR g, LPBOOL h)
{
    if (wc2mb_type == 1)
    {
        __asm
        {
            MOV EDI, EDI // kernelbase
                PUSH EBP
                MOV EBP, ESP
                POP EBP
        }
    }
    __asm JMP lpfnwc2mb
}

__declspec(naked) int __stdcall _MultiByteToWideChar(UINT a, DWORD b, LPCSTR c, int d, LPWSTR e, int f)
{
    if (mb2wc_type == 1)
    {
        __asm
        {
            MOV EDI, EDI // kernelbase
                PUSH EBP
                MOV EBP, ESP
                POP EBP
        }
    }
    __asm JMP lpfnmb2wc
}
