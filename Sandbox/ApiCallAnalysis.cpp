#include "ApiCallAnalysis.h"
#include "LogHelper.h"

bool performApiCallAnalysis(std::wstring wzFileName, std::string apiCallsReportPath)
{
	//create suspended process
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	si.cb = sizeof(si);

	const char envVarName[] = "REPORT_FILE_PATH=";
	const size_t envVarNameLenght = sizeof(envVarName) - 1;

	char * envVarBuffer = new char[sizeof(envVarName) + apiCallsReportPath.size() + 1]{ 0 };
	::memcpy(envVarBuffer, envVarName, envVarNameLenght);
	::memcpy(envVarBuffer + envVarNameLenght, apiCallsReportPath.data(), apiCallsReportPath.size());

	LogHelper::PrintLog(LogLevel::Info, envVarBuffer);
	LogHelper::PrintLog(LogLevel::Info, L"Process to run:  " + wzFileName);
	if (!::CreateProcessW(wzFileName.data(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, (LPVOID)envVarBuffer, NULL, &si, &pi))
	{
		LogHelper::PrintLog(LogLevel::Error, "Failed to create process to perform ApiCallAnalysis");
		return false;
	}

	delete[] envVarBuffer;

	
	//get process handle
	const std::wstring libPath = L"HookPerformer.dll";
	//getchar();
	if (!injectDll(pi.hProcess, libPath))
	{
		LogHelper::PrintLog(LogLevel::Error, "Failed to inject dll");
		if (!::TerminateProcess(pi.hProcess, 0))
		{
			LogHelper::PrintLog(LogLevel::Error, "Can't terminate process " + GetLastErrorAsString());
		}
		return false;
	}
	// Modify suspended process

	::ResumeThread(pi.hThread);
	LogHelper::PrintLog(LogLevel::Info, "Success to run and resume thread");
	DWORD dwRet = ::WaitForSingleObject(pi.hProcess, INFINITE);
	
	return true;
}

DWORD injectDll(HANDLE hProcess, std::wstring libPath)
{
	DWORD hLibModule;
	HMODULE hKernel32 = ::GetModuleHandleW(L"Kernel32");

	size_t libPathBufferSize = (libPath.size() + 1) * sizeof(wchar_t);
	if (libPath.size() > MAX_PATH)
	{
		LogHelper::PrintLog(LogLevel::Error, "Incorrect dll path to inject");
		return NULL;
	}
	WCHAR libPathBuffer[MAX_PATH];
	wcsncpy_s(libPathBuffer, libPath.data(), libPath.size());

	// 1. Allocate memory in the remote process for szLibPath
	// 2. Write szLibPath to the allocated memory
	VOID* pRemoteLibPathAddress = ::VirtualAllocEx(hProcess, NULL, libPathBufferSize,
		MEM_COMMIT, PAGE_READWRITE);

	::WriteProcessMemory(hProcess, pRemoteLibPathAddress, (VOID*)libPath.data(),
		libPathBufferSize, NULL);

	//Load DLL into the remote process

	LPTHREAD_START_ROUTINE loadLibraryWProcAddress =
		(LPTHREAD_START_ROUTINE) ::GetProcAddress(hKernel32, "LoadLibraryW");

	HANDLE hThread = ::CreateRemoteThread(hProcess, NULL, 0,
		loadLibraryWProcAddress, pRemoteLibPathAddress, 0, NULL);

	if (hThread == NULL)
	{
		LogHelper::PrintLog(LogLevel::Error, "Error. Can't createRemoteThread: " + GetLastErrorAsString());
		return NULL;
	}

	::WaitForSingleObject(hThread, INFINITE);

	//Get handle of the loaded module
	::GetExitCodeThread(hThread, &hLibModule);

	if (!hLibModule)
	{
		if (::GetLastError() == 183)
		{
			//incorrect architecture
			std::cout << "ERROR: Incorrect architecture. " << ((sizeof(size_t) == 4) ? "Please use x64 version" : "Please use x32 version") << std::endl;
		}
		else
		{
			LogHelper::PrintLog(LogLevel::Warning, "hLibModule is nullptr: " + GetLastErrorAsString());
		}
	}

	::CloseHandle(hThread);
	::VirtualFreeEx(hProcess, pRemoteLibPathAddress, libPathBufferSize, MEM_RELEASE);
	return hLibModule;
}

std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}