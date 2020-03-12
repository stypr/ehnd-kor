// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"
#define EHND_VER "V3.10-kor"

#pragma warning(disable: 4838)

// Windows ��� ����:
#include <windows.h>
#include <WinBase.h>
#include <windef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <Psapi.h>

#include <iostream>
#include <vector>
#include <chrono>
using namespace std;
using namespace chrono;
#define PREFILTER 1
#define POSTFILTER 2

#define NORMAL_LOG 0
#define ERROR_LOG 10
#define DETAIL_LOG 20
#define TIME_LOG 30
#define SKIPLAYER_LOG 40
#define USERDIC_LOG 50

#define USERDIC_COMM 1
#define USERDIC_NOUN 2

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include "ehnd.h"
#include "hook.h"
#include "log.h"
#include "config.h"

extern HINSTANCE g_hInst;
extern config *pConfig;
extern int g_initTick;
extern char g_DicPath[MAX_PATH];

extern LPBYTE lpfnRetn;
extern LPBYTE lpfnfopen;
extern LPBYTE lpfnwc2mb;
extern int wc2mb_type;
extern LPBYTE lpfnmb2wc;
extern int mb2wc_type;

extern HMODULE hEzt, hMsv;
extern BOOL initOnce;

#ifdef _UNICODE
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#else
#define WIDEN(x)
#endif

#define D(x) deformatted_string(x).c_str()
