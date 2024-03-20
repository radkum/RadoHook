#pragma once
#include <fstream>

enum class LogLevel
{
	Info,
	Warning,
	Error,
};

class LogHelper
{
private:
	static std::ofstream logFile;

private:
	static std::string ws2s(const std::wstring& wstr);

public:
	static bool init(const char* logFilePath);
	static void deinit();

	static void PrintLog(LogLevel lvl, const char* msg);
	static void PrintLog(LogLevel lvl, std::string msg);
	static void PrintLog(LogLevel lvl, const wchar_t* msg);
	static void PrintLog(LogLevel lvl, std::wstring msg);
	static void PrintLog(LogLevel lvl, const char* msg, int val);
	static void PrintLog(LogLevel lvl, const wchar_t* msg, int val);
};