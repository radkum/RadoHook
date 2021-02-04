#include <iostream>
#include <string>

#include <Windows.h>

#include "LogHelper.h"
#include "ApiCallAnalysis.h"

void clearTxtFile(std::string path);
std::wstring s2ws(const std::string& str);
BOOL FileExistsA(LPCSTR szPath);

int main(int argc, char* argv[])
{
	std::string szFileName;
	std::string reportsDir = "reports";

	if (argc != 2 && argc != 3)
	{
		std::cout << "SpyCreateWindow.exe <process_to_spy> or" << std::endl;
		std::cout << "SpyCreateWindow.exe <process_to_spy> <report_dir>" << std::endl;
		return -1;
		
	}

	//get file to analyze
	if (argc >= 2)
	{
		szFileName = std::string(argv[1]);

		if (!FileExistsA(szFileName.data()))
		{
			std::cout << "Given exe file not exists" << std::endl;
			return -3;
		}
	}

	//get output dir
	if (argc == 2)
	{
		std::cout << "Default reports directory: \"reports\" " << std::endl;
	}
	else if (argc == 3)
	{
		reportsDir = std::string(argv[2]);
	}

	else
	{
		szFileName = std::string(argv[1]);
		std::cout << "CheckAntiDebuggingTricks.exe <process_to_check>" << std::endl;
		return -2;
	}
	std::wstring wzFileName = s2ws(szFileName);

	//create reports dir
	
	if (!::CreateDirectoryA(reportsDir.data(), NULL))
	{
		if (::GetLastError() == ERROR_ALREADY_EXISTS)
		{
			std::cout << "WARNING: Given output dir exists" << std::endl;
		}
		else
		{
			std::cout << "Can't create \"" << reportsDir << "\" dir" << std::endl;
			return -2;
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
		std::cout << "Failed to perform api call analysis: " << GetLastErrorAsString() << std::endl;
		return -1;
	}

	LogHelper::deinit();

	std::cout << "SUCCESS"<< std::endl;
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