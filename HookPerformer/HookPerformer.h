#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
// Windows Header Files
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <fstream>

#include "../RadoHook/HookEngine.h"

void logToFile(const char*);

HWND WINAPI myCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
	int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);