#pragma once

#ifdef _M_X64
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif // _M_X64

#include <direct.h>
#include<limits.h>

#include <iostream>
#include<vector>
#include <string>
#include <fstream>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <windows.h>
#include <tchar.h>
#include <psapi.h> 
#include <tlhelp32.h>
#include <stdint.h>
#include <iomanip>
#include <iomanip>
#include <codecvt>
#include <locale>

#include "SimpleSymbolEngine.h"
#include "NtQueries.h"
#include"Process.h"
#include "json.hpp"



#define BUFSIZE 4096


class ProcessTracer
{
private:
	HANDLE m_hProcess;
	DWORD m_parent;
	bool m_isVerbose = false;
	bool m_isEnabledInfo = false;
	bool m_isExecuting = true;
	bool m_IsInitRunning = false;
	std::map<DWORD, HANDLE> threadHandles;
	std::map <LPVOID, std::string > dllFileName;
	SimpleSymbolEngine m_engine;
	std::vector<int> pids;
	std::vector<int> parent;
	std::string process_id;
	std::string parent_id;
	std::string m_cmd;
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconverter;
	Process m_init; // root process
	nlohmann::json njson; // extern json lib
	std::vector<std::string> batfile;
	
public:
	
	ProcessTracer();
	~ProcessTracer();
	ProcessTracer(int argc, TCHAR **argv);
	void Run(bool isDllEnabled);
	void WriteToJSON();
	void OnCreateProcess(DWORD processId, DWORD threadId, CREATE_PROCESS_DEBUG_INFO const & createProcess);
	void GetApplicationPath();
	void OnExitProcess(DWORD threadId, EXIT_PROCESS_DEBUG_INFO const & exitProcess, bool m_isVerbose);
	void PTraceCreateProcess(int argc, TCHAR ** begin);
	void OnException(DWORD threadId, DWORD firstChance, EXCEPTION_RECORD const& exception);
	void GetProcessInformation(DWORD pid,std::string processAddress, bool m_isVerbose);
	void ParseArgs(std::string str);
	void SetParentPID(std::string s) { parent_id = s; }
	void SetPID(std::string s) { process_id = s; }
	std::string GetCommandLineArgs(HANDLE handle);
	std::string CharToString();
	std::string ToString(std::wstring wstr){ return strconverter.to_bytes(wstr);}
	std::string GetCurrentDirectory();
	std::string GetParentPID() { return parent_id; }
	std::string GetPID() { return process_id; }
	std::ofstream myfile_nolib{ "compile_db_no_library.json" };
	std::ofstream myfile_withlib{ "compile_db_via_library.json" };
	std::ofstream bat{ "compile.bat" };
};

