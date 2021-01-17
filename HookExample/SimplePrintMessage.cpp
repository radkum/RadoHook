#include <cstdlib>
#include <cstdio>
#include "windows.h"

DWORD WINAPI myGetCurrentDirectoryA(DWORD nBufferLength, LPSTR buffer);

//for static lib
#include "../RadoHook/HookEngine.h"
#ifndef _WIN64
#pragma comment(lib, "RadoHook_x32.lib")
#else
#pragma comment(lib, "RadoHook_x64.lib")
#endif


//for dynamic lib


int main()
{
	//static lib
	if (!createHook(::GetCurrentDirectoryA, myGetCurrentDirectoryA))
	{
		printf("IsDebuggerPresent Hook creation failed");
		return -1;
	}


	char buffer[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, buffer);
	printf("Current dir: %s!", buffer);

	removeHook(::GetCurrentDirectoryA);

	return 0;
}

DWORD WINAPI myGetCurrentDirectoryA(DWORD nBufferLength, LPSTR buffer)
{
	
	DWORD count = CallOriginFun(::GetCurrentDirectoryA, nBufferLength, buffer);

	char myMessage[] = "Be carefull. There is a HOOK - ";
	constexpr size_t myMesssageLength = sizeof(myMessage) - 1;

	char tmpBuff[MAX_PATH];
	::strncpy_s(tmpBuff, MAX_PATH, buffer, count);

	::strncpy_s(buffer, nBufferLength, myMessage, myMesssageLength);
	::strncpy_s(buffer + myMesssageLength, nBufferLength - myMesssageLength, tmpBuff, count);

	
	return count + myMesssageLength;
}