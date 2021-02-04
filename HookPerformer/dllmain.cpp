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

//extern std::string g_reportFileName;
extern std::ofstream g_reportStream;

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
		logToFile("Success to assign pathbuff");


		if (!createHook(::CreateWindowExW, myCreateWindowExW))
		{
			logToFile("CreateWindowExW Hook creation failed");
			return FALSE;
		}

		logToFile("Success to create hoook");

		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		removeHook(::CreateWindowExW);
		g_reportStream.close();
        break;
    }
    return TRUE;
}