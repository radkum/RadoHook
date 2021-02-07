#include "HookPerformer.h"

std::ofstream g_reportStream;
NtUserCreateWindowEx_FunType g_NtUserCreateWindowEx = nullptr;
HMODULE g_hWin32 = nullptr;

void logToFile(const char* msg)
{
	g_reportStream << msg << std::endl;
}

HWND NTAPI myNtUserCreateWindowEx(DWORD dwExStyle, PLARGE_STRING plstrClassName, PLARGE_STRING plstrClsVersion,
	PLARGE_STRING plstrWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,
	HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwFlags, PVOID acbiBuffer, DWORD unknown1, PVOID unknown2)
{
	char buffer[1024] = { 0 };

	char* className = allocateAndGetCStringFromPLargeString(plstrClassName);
	char* windowName = allocateAndGetCStringFromPLargeString(plstrWindowName);
	sprintf_s(buffer, "NtUserCreateWindowEx - plstrClassName: %s, plstrWindowName: %s",
		className, windowName);

	delete[] className;
	delete[] windowName;

	logToFile(buffer);
	return CallOriginFun(g_NtUserCreateWindowEx, dwExStyle, plstrClassName, plstrClsVersion, plstrWindowName,
		dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwFlags, acbiBuffer, unknown1, unknown2);
}

HFILE WINAPI myOpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle)
{
	logToFile("openFile");
	return CallOriginFun(::OpenFile, lpFileName, lpReOpenBuff, uStyle);
}

LRESULT WINAPI mySendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	char buffer[1024] = { 0 };
	sprintf_s(buffer, "SendMessageA(%p, %d, %p, %p)", hWnd, Msg, (PVOID)wParam, (PVOID)lParam);
	logToFile(buffer);
	return CallOriginFun(::SendMessageA, hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI mySendMessageW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	char buffer[1024] = { 0 };
	sprintf_s(buffer, "SendMessageW(%p, %d, %p, %p)", hWnd, Msg, (PVOID)wParam, (PVOID)lParam);
	logToFile(buffer);
	return CallOriginFun(::SendMessageW, hWnd, Msg, wParam, lParam);
}

char* allocateAndGetCStringFromPLargeString(PLARGE_STRING str)
{
	char* toRet;
	if (HIWORD(str) == 0)
	{
		toRet = new char[2 * sizeof(DWORD) + 3]{ 0 };
		sprintf_s(toRet, 2 * sizeof(DWORD) + 2, "0x%X", str);
	}
	else if (str->bAnsi)
	{
		size_t bufLen = str->MaximumLength;
		toRet = new char[bufLen] { 0 };
		::strncpy_s(toRet, bufLen, (char*)str->Buffer, str->Length);
	}
	else
	{
		size_t bufLen = str->MaximumLength / sizeof(wchar_t);
		toRet = new char[bufLen] { 0 };
		wchar_t* tmp = new wchar_t[bufLen] { 0 };

		::wcsncpy_s(tmp, bufLen, (wchar_t*)str->Buffer, str->Length / sizeof(wchar_t));
		sprintf_s(toRet, bufLen, "%ls", tmp);
		delete[] tmp;
	}

	return toRet;
}