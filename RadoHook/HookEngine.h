#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#include <map>
#include <array>

#define ASSERT_BOOL(x)		if((x) == false) return false;
#define JMP_BUF_SIZE		16

class HookEngine
{
private:
	//members
	static constexpr WORD Far_Jmp_Opc = 0x25FF;
	std::map<LPVOID, std::array<BYTE, JMP_BUF_SIZE>> m_mapNewFunToOriginFunPrologBuf;
	std::map<LPVOID, LPVOID> m_mapOriginFunToNewFun;

	//methods
	void createJmp(ULONG_PTR pFun, BYTE jmpBuffer[JMP_BUF_SIZE]);

public:
	bool createHook(LPVOID pOriginFun, LPVOID pNewFun);
	bool removeHook(LPVOID pOriginFun);

	bool restoreOriginFun(LPVOID pOriginFun, BYTE jmpBuffer[JMP_BUF_SIZE]);
	bool restoreHook(LPVOID pOriginFun, BYTE jmpBuffer[JMP_BUF_SIZE]);
};

//exported
extern HookEngine* hookEngine;
bool createHook(LPVOID pOriginFun, LPVOID pNewFun);
bool removeHook(LPVOID pOriginFun);

template<typename T, typename... Targs>
auto CallOriginFun(T fun, Targs... args)
{
	BYTE jmpBuffer[JMP_BUF_SIZE];
	hookEngine->restoreOriginFun((LPVOID)fun, jmpBuffer);
	auto x = fun(args...);
	hookEngine->restoreHook((LPVOID)::GetCurrentDirectoryA, jmpBuffer);

	return x;
}