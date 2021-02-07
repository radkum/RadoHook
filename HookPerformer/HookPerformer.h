#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <fstream>

#include "../RadoHook/HookEngine.h"

typedef struct _LARGE_STRING
{
	ULONG Length;
	ULONG MaximumLength : 31;
	ULONG bAnsi : 1;
	PVOID Buffer;
} LARGE_STRING, *PLARGE_STRING;

void logToFile(const char*);

HWND NTAPI myNtUserCreateWindowEx(DWORD dwExStyle, PLARGE_STRING plstrClassName, PLARGE_STRING plstrClsVersion,
	PLARGE_STRING plstrWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,
	HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwFlags, PVOID acbiBuffer, DWORD unknown1, PVOID unknown2);

HFILE WINAPI myOpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle);
LRESULT WINAPI mySendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI mySendMessageW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

using NtUserCreateWindowEx_FunType = decltype(&myNtUserCreateWindowEx);

char* allocateAndGetCStringFromPLargeString(PLARGE_STRING str);