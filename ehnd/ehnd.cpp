// ehnd.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "ehnd.h"

FARPROC apfnEzt[100];
FARPROC apfnMsv[100];

bool EhndInit(void)
{
    // 중복 초기화 방지
    if (initOnce) return false;
    else initOnce = true;

    // 설정 로드
    pConfig->LoadConfig();

    // 기존 로그 삭제
    if (pConfig->GetFileLogStartupClear())
    {
        wchar_t lpFileName[MAX_PATH];
        if (pConfig->GetFileLogEztLoc())
            GetLoadPath(lpFileName, MAX_PATH);
        else GetExecutePath(lpFileName, MAX_PATH);
        wcscat_s(lpFileName, L"\\ehnd_kor_log.log");
        DeleteFile(lpFileName);
    }

    CreateLogWin(g_hInst);
    ShowLogWin(pConfig->GetConsoleSwitch());
    LogStartMsg();

    GetRealWC2MB();
    GetRealMB2WC();

    if (!hook()) return false;
    return true;
}

// 이지트랜스 API
__declspec(naked) void K2J_Initialize(void)
{
    __asm JMP apfnEzt[4 * 0];
}
void __stdcall K2J_InitializeEx(int data0, LPSTR key)
{
    SetLogText(L"K2J_InitializeEx : 이지트랜스 초기화\n");

    EhndInit();
    __asm
    {
        PUSH DWORD PTR DS : [key]
            PUSH data0
            CALL apfnEzt[4 * 1]
    }
}
__declspec(naked) void K2J_FreeMem(void)
{
    __asm JMP apfnEzt[4 * 2];
}
__declspec(naked) void K2J_GetPriorDict(void)
{
    __asm JMP apfnEzt[4 * 3];
}
__declspec(naked) void K2J_GetProperty(void)
{
    __asm JMP apfnEzt[4 * 4];
}
void __stdcall K2J_ReloadUserDict(void)
{
    __asm CALL apfnEzt[4 * 5];
}
__declspec(naked) void K2J_SetPriorDict(void)
{
    __asm JMP apfnEzt[4 * 6];
}

__declspec(naked) void K2J_SetProperty(void)
{
    __asm JMP apfnEzt[4 * 7];
}
__declspec(naked) void K2J_StopTranslation(void)
{
    __asm JMP apfnEzt[4 * 8];
}
__declspec(naked) void K2J_Terminate(void)
{
    __asm JMP apfnEzt[4 * 9];
}
__declspec(naked) void K2J_TranslateChat(void)
{
    __asm JMP apfnEzt[4 * 10];
}
__declspec(naked) void K2J_TranslateMM(void)
{
    __asm JMP apfnEzt[4 * 11];
}
__declspec(naked) void K2J_TranslateMMEx(void)
{
    __asm JMP apfnEzt[4 * 12];
}
__declspec(naked) void *msvcrt_free(void *_Memory)
{
    __asm JMP apfnMsv[4 * 0];
}
__declspec(naked) void *msvcrt_malloc(size_t _Size)
{
    __asm JMP apfnMsv[4 * 1];
}
__declspec(naked) void *msvcrt_fopen(char *path, char *mode)
{
    __asm JMP apfnMsv[4 * 2];
}

void *__stdcall K2J_TranslateMMNTW(int data0, LPCWSTR szIn)
{
    DWORD dwStart, dwEnd;
    LPWSTR szOut;
    wstring wsText, wsOriginal;
    int i_len;
    LPWSTR lpKOR;
    LPSTR szJPN, szKOR;

    wsOriginal = szIn;
    wsText = szIn;

    CheckLogSize();
    CheckConsoleLine();

    if (wsText.length()) WriteLog(NORMAL_LOG, L"[REQUEST] %s\n\n", D(wsText));
    if (wcslen(szIn))
    {

        WriteLog(NORMAL_LOG, L"[PRE] %s\n\n", D(wsText));

        i_len = _WideCharToMultiByte(949, 0, wsText.c_str(), -1, NULL, NULL, NULL, NULL);
        WriteLog(NORMAL_LOG, L"[SIZE-949] %d\n\n", i_len);

        szJPN = (LPSTR)msvcrt_malloc((i_len + 1) * 3);

        if (szJPN == NULL)
        {
            WriteLog(ERROR_LOG, L"K2J_TranslateMMNT : Memory Allocation Error.\n");
            return 0;
        }
        _WideCharToMultiByte(949, 0, wsText.c_str(), -1, szJPN, i_len, NULL, NULL);

        WriteLog(NORMAL_LOG, L"[PRE-EUCKR] %s\n\n", D(wsText));

        dwStart = GetTickCount();

        __asm
        {
            PUSH DWORD PTR DS : [szJPN]
                PUSH data0
                CALL apfnEzt[4 * 13]
                MOV DWORD PTR DS : [szKOR], EAX
        }

        dwEnd = GetTickCount();

        WriteLog(TIME_LOG, L"K2J_TranslateMMNT : --- Elasped Time : %dms ---\n", dwEnd - dwStart);

        msvcrt_free(szJPN);

        i_len = _MultiByteToWideChar(932, MB_PRECOMPOSED, szKOR, -1, NULL, NULL);
        WriteLog(NORMAL_LOG, L"[SIZE-932] %d\n\n", i_len);

        lpKOR = (LPWSTR)msvcrt_malloc((i_len + 1) * 2);
        if (lpKOR == NULL)
        {
            WriteLog(ERROR_LOG, L"K2J_TranslateMMNT : Memory Allocation Error.\n");
            return 0;
        }
        _MultiByteToWideChar(932, 0, szKOR, -1, lpKOR, i_len);

        wsText = lpKOR;
        msvcrt_free(szKOR);
        msvcrt_free(lpKOR);


        WriteLog(NORMAL_LOG, L"[TRANS] %s\n\n", D(wsText));
        WriteLog(NORMAL_LOG, L"[POST] %s\n\n", D(wsText));
    }
    else if (wcslen(szIn))
    {
        WriteLog(NORMAL_LOG, L"[COMMAND] %s\n\n", D(wsText));
    }

    WriteLog(NORMAL_LOG, L"[SIZE-wstext] %d\n\n", wsText.length());

    szOut = (LPWSTR)msvcrt_malloc((wsText.length() + 1) * 4 + 4);
    if (szOut == NULL)
    {
        WriteLog(ERROR_LOG, L"K2J_TranslateMMNT : Memory Allocation Error.\n");
        return 0;
    }
    wcscpy_s(szOut, (wsText.length() + 1) * 2, wsText.c_str());
    return (void *)szOut;
}

void *__stdcall K2J_TranslateMMNT(int data0, LPCSTR szIn)
{
    LPSTR szOut;
    wstring wsText, wsOriginal;
    int i_len;
    LPWSTR lpJPN, lpKOR;

    lpKOR = 0;
    i_len = _MultiByteToWideChar(949, MB_PRECOMPOSED, szIn, -1, NULL, NULL);
    lpKOR = (LPWSTR)msvcrt_malloc((i_len + 1) * 3);
    if (lpKOR == NULL)
    {
        WriteLog(ERROR_LOG, L"K2J_TranslateMMNT : Memory Allocation Error.\n");
        return 0;
    }
    _MultiByteToWideChar(949, 0, szIn, -1, lpKOR, i_len);
    wsText = lpKOR;
    msvcrt_free(lpKOR);

    lpJPN = (LPWSTR)K2J_TranslateMMNTW(data0, wsText.c_str());

    // cp949 내보내기 
    i_len = _WideCharToMultiByte(932, 0, lpJPN, -1, NULL, NULL, NULL, NULL);
    szOut = (LPSTR)msvcrt_malloc((i_len + 1) * 3);
    if (szOut == NULL)
    {
        WriteLog(ERROR_LOG, L"K2J_TranslateMMNT : Memory Allocation Error.\n");
        return 0;
    }
    _WideCharToMultiByte(932, 0, lpJPN, -1, szOut, i_len, NULL, NULL);
    msvcrt_free(lpJPN);

    return (void *)szOut;
}

bool GetLoadPath(LPWSTR Path, int Size)
{
    GetModuleFileName(g_hInst, Path, Size);
    if (Path[0] == 0) return false;
    int i = wcslen(Path);
    while (i--)
    {
        if (Path[i] == L'\\')
        {
            Path[i] = 0;
            break;
        }
    }
    return true;
}

bool GetExecutePath(LPWSTR Path, int Size)
{
    GetModuleFileName(GetModuleHandle(NULL), Path, Size);
    if (Path[0] == 0) return false;
    int i = wcslen(Path);
    while (i--)
    {
        if (Path[i] == L'\\')
        {
            Path[i] = 0;
            break;
        }
    }
    return true;
}

wstring replace_all(const wstring &str, const wstring &pattern, const wstring &replace)
{
    wstring result = str;
    wstring::size_type pos = 0, offset = 0;

    while ((pos = result.find(pattern, offset)) != wstring::npos)
    {
        result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
        offset = pos + replace.size();
    }
    return result;
}

wstring deformatted_string(const wstring &str)
{
    wchar_t pattern[][2] = { L"%", L"\\" };
    wchar_t replace[][3] = { L"%%", L"\\\\" };
    wstring result = str;
    wstring::size_type pos = 0, offset = 0;

    for (int i = 0; i < 1; i++)
    {
        while ((pos = result.find(pattern[i], offset)) != wstring::npos)
        {
            result.replace(result.begin() + pos, result.begin() + pos + 1, replace[i]);
            offset = pos + 2;
        }
    }
    return result;
}
