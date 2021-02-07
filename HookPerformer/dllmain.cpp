#include "HookPerformer.h"
#include <fstream>
#include <string>

//for static lib
#include "../RadoHook/HookEngine.h"
#ifndef _WIN64
#pragma comment(lib, "RadoHook_x32.lib")
#else
#pragma comment(lib, "RadoHook_x64.lib")
#endif

extern std::ofstream g_reportStream;
extern HMODULE g_hWin32;
extern NtUserCreateWindowEx_FunType g_NtUserCreateWindowEx;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  reason, LPVOID lpReserved)
{
    switch (reason)
    {
		//logToReportFile("DLL_PROCESS_ATTACH");
    case DLL_PROCESS_ATTACH:
	{
		
		char pathBuff[MAX_PATH];
		if (!::GetEnvironmentVariableA("REPORT_FILE_PATH", pathBuff, MAX_PATH))
		{
			//logToReportFile("GetEnvironmentVariableA: false");
			return FALSE;
		}

		g_reportStream.open(pathBuff);
		//logToFile("Success to assign pathbuff");

		//NtUserCreateWindowEx
		g_hWin32 = ::LoadLibraryA("win32u.dll");
		if (!g_hWin32)
		{
			logToFile("failed to get handle to win32u.dll module");
			return FALSE;
		}

		g_NtUserCreateWindowEx = (NtUserCreateWindowEx_FunType) ::GetProcAddress(g_hWin32, "NtUserCreateWindowEx");
		if (!g_NtUserCreateWindowEx)
		{
			logToFile("failed to get NtUserCreateWindowEx function");
			return FALSE;
		}

		if (!createHook(g_NtUserCreateWindowEx, myNtUserCreateWindowEx))
		{
			logToFile("NtUserCreateWindowEx Hook creation failed");
			return FALSE;
		}
		//end NtUserCreateWindowEx

		/*if (!createHook(::SendMessageA, mySendMessageA))
		{
			logToFile("OpenFile Hook creation failed");
			return FALSE;
		}

		if (!createHook(::SendMessageW, mySendMessageW))
		{
			logToFile("OpenFile Hook creation failed");
			return FALSE;
		}*/
		logToFile("Success to create hoook");

		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		removeHook(g_NtUserCreateWindowEx);
		::FreeLibrary(g_hWin32);
		g_reportStream.close();
        break;
    }
    return TRUE;
}