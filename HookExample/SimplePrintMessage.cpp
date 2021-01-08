#include <cstdlib>
#include <cstdio>

#include "windows.h"

//hook api
using CreateHook_FunType = bool(__cdecl *)(LPVOID, LPVOID);
using DisableHook_FunType = bool(__cdecl *)(LPVOID);
using GetBufferWithOriginFun_FunType = BYTE * (__cdecl *)(LPVOID);
using RestoreOriginFun_FunType = BYTE * (__cdecl *)(LPVOID, BYTE[16]);
using RestoreHook_FunType = BYTE * (__cdecl *)(LPVOID, BYTE[16]);

using GetCurrentDirectoryA_FunType = DWORD(WINAPI *)(DWORD, LPSTR);

CreateHook_FunType createHook = nullptr;
DisableHook_FunType disableHook = nullptr;
GetBufferWithOriginFun_FunType getBufferWithOriginFun = nullptr;
RestoreOriginFun_FunType restoreOriginFun = nullptr;
RestoreHook_FunType restoreHook = nullptr;


bool loadHookApi(HMODULE& hRadoHook);
void unloadHookApi(HMODULE hMod);

DWORD WINAPI myGetCurrentDirectoryA(DWORD nBufferLength, LPSTR buffer);

int main()
{
	HMODULE hRadoHook;
	if (!loadHookApi(hRadoHook))
		return -1;

	if (!createHook(::GetCurrentDirectoryA, myGetCurrentDirectoryA))
	{
		printf("IsDebuggerPresent Hook creation failed");
		return -1;
	}

	char buffer[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, buffer);


	
	printf("Current dir: %s!", buffer);

	disableHook(::GetCurrentDirectoryA);

	unloadHookApi(hRadoHook);
	return 0;
}

DWORD WINAPI myGetCurrentDirectoryA(DWORD nBufferLength, LPSTR buffer)
{
	GetCurrentDirectoryA_FunType pOryginFunProlog =
		(GetCurrentDirectoryA_FunType)getBufferWithOriginFun((LPVOID)::GetCurrentDirectoryA);

	DWORD count = pOryginFunProlog(nBufferLength, buffer);

	char myMessage[] = "Be carefull. There is a HOOK - ";
	constexpr size_t myMesssageLength = sizeof(myMessage) - 1;

	char tmpBuff[MAX_PATH];
	::strncpy_s(tmpBuff, MAX_PATH, buffer, count);

	::strncpy_s(buffer, nBufferLength, myMessage, myMesssageLength);
	::strncpy_s(buffer + myMesssageLength, nBufferLength - myMesssageLength, tmpBuff, count);

	
	return count + myMesssageLength;
}

bool loadHookApi(HMODULE& hRadoHook)
{

	hRadoHook = ::LoadLibraryA("RadoHook.dll");
	if (!hRadoHook)
	{
		printf("Failed to load HookEngine\n");
		return false;
	}

	createHook = (CreateHook_FunType) ::GetProcAddress(hRadoHook, "CreateHook");
	disableHook = (DisableHook_FunType) ::GetProcAddress(hRadoHook, "DisableHook");
	getBufferWithOriginFun = (GetBufferWithOriginFun_FunType) ::GetProcAddress(hRadoHook, "GetBufferWithOriginFun");
	restoreOriginFun = (RestoreOriginFun_FunType) ::GetProcAddress(hRadoHook, "RestoreOriginFun");
	restoreHook = (RestoreHook_FunType) ::GetProcAddress(hRadoHook, "RestoreHook");

	if (!createHook || !disableHook || !getBufferWithOriginFun || !restoreOriginFun || !restoreHook)
	{
		printf("Failed to import functions");
		::FreeLibrary(hRadoHook);
		return false;
	}
	return true;
}

void unloadHookApi(HMODULE hMod)
{
	::FreeLibrary(hMod);
}