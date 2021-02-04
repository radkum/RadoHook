#include "HookPerformer.h"

std::ofstream g_reportStream;

void logToFile(const char* msg)
{
	g_reportStream << msg << std::endl;
}

HWND WINAPI myCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, 
	int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	char buffer[1024] = { 0 };
	sprintf_s(buffer, "hooked CreateWindowExW. lpWindowName: %ls", lpWindowName);
	logToFile(buffer);
	return CallOriginFun(::CreateWindowExW, dwExStyle, lpClassName, lpWindowName, dwStyle,
		X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}