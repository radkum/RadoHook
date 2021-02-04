#include "HookEngine.h"

#include <iostream>

HookEngine* hookEngine = nullptr;
size_t hookCount = 0;

bool createHook(LPVOID pOriginFun, LPVOID pNewFun)
{
	if (hookCount == 0 || hookEngine == nullptr)
	{
		hookEngine = new HookEngine;
	}

	bool ret = hookEngine->createHook(pOriginFun, pNewFun);
	if (ret) hookCount++;

	return ret;
}

bool removeHook(LPVOID pOriginFun)
{
	bool ret = false;
	if (hookEngine)
		hookEngine->removeHook(pOriginFun);

	if (ret) hookCount--;

	return ret;
}

/*
absolute jmp
	to consider:
	 9A cp CALL ptr16:32 Call far, absolute, address given in operand
*/
void HookEngine::createJmp(ULONG_PTR pFun, BYTE outJmpBuffer[JMP_BUF_SIZE])
{
	//set buffer
	::memset(outJmpBuffer, 0xCC, JMP_BUF_SIZE);

	BYTE* ptr = outJmpBuffer;

	*(WORD*)ptr = Far_Jmp_Opc;
	ptr += sizeof(Far_Jmp_Opc);
#ifndef _WIN64
	//x32
	*(DWORD*)ptr = (DWORD)ptr + sizeof(DWORD);
#else
	*(DWORD*)ptr = 0;
#endif
	ptr += sizeof(DWORD);

	*(ULONG_PTR*)ptr = pFun;
}

bool HookEngine::createHook(LPVOID pOriginFun, LPVOID pNewFun)
{
	//up -> ULONG_PTR
	ULONG_PTR upNewFun = (ULONG_PTR)pNewFun;

	//set read write permits 
	DWORD oldProtect = 0;
	ASSERT_BOOL(::VirtualProtect(pOriginFun, JMP_BUF_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect));

	//copy to originFunPrologBuf 16 first bytes of origin fun
	::memcpy(m_mapNewFunToOriginFunPrologBuf[pNewFun].data(), pOriginFun, JMP_BUF_SIZE);

	//replace origin fun prolog with jmp to our newFun
	createJmp(upNewFun, (BYTE*)pOriginFun);

	//restore old permits for orginFun
	DWORD dummy = 0;
	ASSERT_BOOL(::VirtualProtect(pOriginFun, JMP_BUF_SIZE, oldProtect, &dummy));

	m_mapOriginFunToNewFun[pOriginFun] = pNewFun;
	return true;
}

//methods
bool HookEngine::restoreOriginFun(LPVOID pOriginFun, BYTE outJmpBuffer[JMP_BUF_SIZE])
{
	//at the beggining check, if this funciton is hooked
	ASSERT_BOOL(m_mapOriginFunToNewFun.count(pOriginFun) > 0);

	//set read write permits 
	DWORD oldProtect = 0, dummy = 0;
	ASSERT_BOOL(::VirtualProtect(pOriginFun, JMP_BUF_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect));
	
	//copy jmp to buffer
	::memcpy(outJmpBuffer, pOriginFun, JMP_BUF_SIZE);

	LPVOID pNewFun = m_mapOriginFunToNewFun[pOriginFun];
	::memcpy(pOriginFun, m_mapNewFunToOriginFunPrologBuf[pNewFun].data(), JMP_BUF_SIZE);
	ASSERT_BOOL(::VirtualProtect(pOriginFun, JMP_BUF_SIZE, oldProtect, &dummy));
	return true;
}

bool HookEngine::restoreHook(LPVOID pOriginFun, BYTE jmpBuffer[JMP_BUF_SIZE])
{
	//at the beggining check, if this funciton is hooked
	ASSERT_BOOL(m_mapOriginFunToNewFun.count(pOriginFun) > 0);

	//set read write permits 
	DWORD oldProtect = 0, dummy = 0;

	ASSERT_BOOL(::VirtualProtect(pOriginFun, JMP_BUF_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect));

	//restore hook
	::memcpy(pOriginFun, jmpBuffer, JMP_BUF_SIZE);

	//restore permits
	ASSERT_BOOL(::VirtualProtect(pOriginFun, JMP_BUF_SIZE, oldProtect, &dummy));
	return true;
}

bool HookEngine::removeHook(LPVOID pOriginFun)
{
	//at the beggining check, if this funciton is hooked
	ASSERT_BOOL(m_mapOriginFunToNewFun.count(pOriginFun) > 0);

	DWORD oldProtect = 0;
	::VirtualProtect(pOriginFun, JMP_BUF_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);

	LPVOID pNewFun = m_mapOriginFunToNewFun[pOriginFun];

	ASSERT_BOOL(m_mapNewFunToOriginFunPrologBuf.count(pNewFun) > 0);
	::memcpy(pOriginFun, m_mapNewFunToOriginFunPrologBuf[pNewFun].data(), JMP_BUF_SIZE);

	DWORD dummy = 0;
	ASSERT_BOOL(::VirtualProtect(pOriginFun, JMP_BUF_SIZE, oldProtect, &dummy));

	m_mapNewFunToOriginFunPrologBuf.erase(pNewFun);
	m_mapOriginFunToNewFun.erase(pOriginFun);
	return true;
}
