#include "HookPerformer.h"
#include "shellapi.h"
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

		if (!createHook(::Sleep, mySleep))
		{
			logToFile("Sleep Hook creation failed");
			return FALSE;
		}

		if (!createHook(::BlockInput, myBlockInput))
		{
			logToFile("BlockInput Hook creation failed");
			return FALSE;
		}

		if (!createHook(::SetCursorPos, mySetCursorPos))
		{
			logToFile("SetCursorPos Hook creation failed");
			return FALSE;
		}

		if (!createHook(::ShellExecuteA, myShellExecuteA))
		{
			logToFile("ShellExecuteA Hook creation failed");
			return FALSE;
		}

		if (!createHook(::RegSetValueExA, myRegSetValueExA))
		{
			logToFile("RegSetValueExA Hook creation failed");
			return FALSE;
		}

		if (!createHook(::Beep, myBeep))
		{
			logToFile("Beep Hook creation failed");
			return FALSE;
		}

		if (!createHook(::MessageBoxA, myMessageBoxA))
		{
			logToFile("MessageBoxA Hook creation failed");
			return FALSE;
		}
		logToFile("Success to create hoook");

		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		removeHook(::Sleep);
		removeHook(::BlockInput);
		removeHook(::SetCursorPos);
		removeHook(::ShellExecuteA);
		removeHook(::RegSetValueExA);
		removeHook(::Beep);
		//removeHook(::MessageBoxA);
		::FreeLibrary(g_hWin32);
		g_reportStream.close();
        break;
    }
    return TRUE;
}