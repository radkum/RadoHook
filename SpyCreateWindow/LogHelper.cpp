#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include <codecvt>

#include "LogHelper.h"

std::ofstream LogHelper::logFile;

bool LogHelper::init(const char* logFilePath)
{
	logFile.open(logFilePath);
	if (!logFile)
	{
		logFile.open("logFile.txt");
		if (!logFile)
		{
			return false;
		}
	}
	
	return true;
}

void LogHelper::deinit()
{
	if (logFile)
		logFile.close();
}

void LogHelper::PrintLog(LogLevel lvl, const char* msg)
{
	const char * logLevelStr = "";
	if (lvl == LogLevel::Warning)
	{
		logLevelStr = "Warning: ";
	}
	else if (lvl == LogLevel::Error)
	{
		logLevelStr = "ERROR: ";
	}

	logFile << logLevelStr << msg << std::endl;
}

void LogHelper::PrintLog(LogLevel lvl, std::string msg)
{
	PrintLog(lvl, msg.data());
}

void LogHelper::PrintLog(LogLevel lvl, const wchar_t* msg)
{
	const char * logLevelStr = "";
	if (lvl == LogLevel::Warning)
	{
		logLevelStr = "Warning: ";
	}
	else if (lvl == LogLevel::Error)
	{
		logLevelStr = "ERROR: ";
	}
	
	logFile << logLevelStr << msg << std::endl;
}

void LogHelper::PrintLog(LogLevel lvl, std::wstring msg)
{
	char* buff = new char[msg.size() + 1];
	sprintf_s(buff, msg.size() + 1, "%ls", msg.data());
	PrintLog(lvl, buff);
	delete[] buff;
}

void LogHelper::PrintLog(LogLevel lvl, const char* msg, int val)
{
	std::string stringMsg = msg + std::to_string(val);
	PrintLog(lvl, stringMsg.data());
}

void LogHelper::PrintLog(LogLevel lvl, const wchar_t* msg, int val)
{
	std::wstring wstringMsg = msg + std::to_wstring(val);
	PrintLog(lvl, wstringMsg.data());
}

//helper funcion
std::string LogHelper::ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}