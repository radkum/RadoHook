#include <iostream>
#include <string>

#include <Windows.h>

#include "LogHelper.h"
#include "ApiCallAnalysis.h"

void clearTxtFile(std::string path);
std::wstring s2ws(const std::string& str);
BOOL FileExistsA(LPCSTR szPath);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

enum SanbboxErrorCodes {
	InputPathIsNull = 1,
	GivenFileNotExists,
	ReportDirAlreadyExists,
	FailedToPerformApiAnalysis,
};

__declspec(dllexport) int sandboxFile(char* suspectedFile, int suspectedFileLen, char* reportsDirectory, int reportsDirLen)
{
	if (suspectedFile == nullptr) {
		return InputPathIsNull;
	}
	std::string szFileName = std::string(suspectedFile,suspectedFileLen);
	
	std::string reportsDir = "reports";
	if (reportsDirectory != nullptr) {
		reportsDir = std::string(reportsDirectory, reportsDirLen);
	} 

	if (!FileExistsA(szFileName.data()))
	{
		//std::cout << "Given exe file not exists" << std::endl;
		return GivenFileNotExists;
	}

	std::wstring wzFileName = s2ws(szFileName);

	//create reports dir
	if (!::CreateDirectoryA(reportsDir.data(), NULL))
	{
		if (::GetLastError() == ERROR_ALREADY_EXISTS)
		{
			//std::cout << "WARNING: Given output dir exists" << std::endl;
		}
		else
		{
			//std::cout << "Can't create \"" << reportsDir << "\" dir" << std::endl;
			return ReportDirAlreadyExists;
		}
	}

	if (reportsDir[reportsDir.size() - 1] != '\\')
	{
		reportsDir += '\\';
	}

	std::string logOutputName = "logOutput.txt";
	std::string logOutputPath = reportsDir + "logOutput.txt";
	//clearTxtFile(logOutputPath);

	LogHelper::init(logOutputPath.data());

	std::string apiCallsReportName = "apiCallsReport.txt";
	std::string apiCallsReportPath = reportsDir + "apiCallsReport.txt";
	//clearTxtFile(apiCallsReportPath);

	if (!performApiCallAnalysis(wzFileName, apiCallsReportPath))
	{
		std::cout << "Failed to perform api call analysis " << std::endl;
		LogHelper::PrintLog(LogLevel::Error, "Failed to perform api call analysis " + ::GetLastErrorAsString());
		return FailedToPerformApiAnalysis;
	}

	LogHelper::deinit();

	//std::cout << "SUCCESS" << std::endl;
	return 0;
}

std::wstring s2ws(const std::string& str)
{
	int cchWideChar = ::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(cchWideChar, 0);
	::MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], cchWideChar);
	return wstrTo;
}

BOOL FileExistsA(LPCSTR szPath)
{
	DWORD dwAttrib = GetFileAttributesA(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void clearTxtFile(std::string path)
{
	std::ofstream ofs;
	ofs.open(path, std::ofstream::out | std::ofstream::trunc);
	ofs.close();
}