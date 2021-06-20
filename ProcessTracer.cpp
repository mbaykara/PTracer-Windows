/*
NAME
ProcessTracer

DESCRIPTION
About the simplest debugger which is useful!

COPYRIGHT
Copyright (C) 2011 by Roger Orr <rogero@howzatt.demon.co.uk>
Copyright (C) 2021 by Mehmet Ali Baykara <mehmetalibaykara@gmail.com>


This software is distributed in the hope that it will be useful, but
without WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
Permission is granted to anyone to make or distribute verbatim
copies of this software provided that the copyright notice and
this permission notice are preserved, and that the distributor
grants the recipent permission for further distribution as permitted
by this notice.
Comments and suggestions are always welcome.
Please report bugs to rogero@howzatt.demon.co.uk.
*/


#include "ProcessTracer.h" 
#include<atlstr.h>

#ifdef _UNICODE
#define tcout wcout
#define tcerr wcerr
#else
#define tcout cout
#define tcerr cerr
#endif
#define SIZE (10)

ProcessTracer::ProcessTracer()
{
	_putenv("_NO_DEBUG_HEAP=1");
}
ProcessTracer::~ProcessTracer() {}

ProcessTracer::ProcessTracer(int argc, TCHAR **argv)
{
	_putenv("_NO_DEBUG_HEAP=1");
	this->PTraceCreateProcess(argc, argv);
}

/*
* Depending on debug event code the corresponding case will be triggerd
* Debug event code will be perform by minwinbase.h api
* Modified function from @RogerOrr
* Essential logic process creation is from @RogerOrr
*/

void ProcessTracer::Run(bool isVerbose)
{
	bool attached = false;
	m_isVerbose = isVerbose;


	do
	{
		DEBUG_EVENT DebugEvent;
		DWORD continueFlag = DBG_CONTINUE;
		if (!WaitForDebugEvent(&DebugEvent, INFINITE)) { throw std::runtime_error("Debug loop aborted");}
		switch (DebugEvent.dwDebugEventCode)
		{
		case CREATE_PROCESS_DEBUG_EVENT:
			OnCreateProcess(DebugEvent.dwProcessId, DebugEvent.dwThreadId, DebugEvent.u.CreateProcessInfo);
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			OnExitProcess(DebugEvent.dwProcessId, DebugEvent.u.ExitProcess, m_isVerbose);
			break;
		case EXCEPTION_DEBUG_EVENT:
			if (!attached){	attached = true;}
			else if(DebugEvent.u.Exception.ExceptionRecord.ExceptionCode == STATUS_WX86_BREAKPOINT && m_isVerbose){	std::cout << "WOW64 initialised" << "\n";}
			else{continueFlag = (DWORD)DBG_EXCEPTION_NOT_HANDLED;}
			break;
		default:
			if (m_isVerbose)
			{
				std::cerr << "Undefined debug event: " << DebugEvent.dwDebugEventCode << "\n";
			}
		}
		if (!ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId, continueFlag))
		{
			throw std::runtime_error("Error continuing debug event");
		}
	} while (!m_IsInitRunning);

	WriteToJSON();
	for (size_t i = 0; i < batfile.size(); i++)
	{
		bat << batfile[i];
		bat << "\n";
	}

}

void ProcessTracer::WriteToJSON()
{
	std::string r = m_init.GetJSON();
	std::replace(r.begin(), r.end(), '\\', '/');
	//std::cout << r << "\n";
	myfile_nolib << r;
	nlohmann::json j = njson.parse(r);
	std::cout << j.front().dump(2);
	myfile_withlib << j.front().dump(2);
}

/*
* @return the directory where the process triggerd by.
*/
std::string ProcessTracer::GetCurrentDirectory() 
{
	char buff[MAX_PATH];
	_getcwd(buff, MAX_PATH);
	std::string currentDir(buff);
	return currentDir;
}

/*
* For each new process, retrieve all process information by invoking corresponding methods
* Modified function from @RogerOrr
* Essential logic process creation is from @RogerOrr
*/
void ProcessTracer::OnCreateProcess(DWORD processId, DWORD threadId, CREATE_PROCESS_DEBUG_INFO const& createProcess)
{
	m_hProcess = createProcess.hProcess;
	threadHandles[threadId] = createProcess.hThread;
	m_engine.init(m_hProcess);
	m_engine.loadModule(createProcess.hFile, createProcess.lpBaseOfImage, std::string());
	m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

	GetProcessInformation(processId, m_engine.addressToString(createProcess.lpStartAddress), true);
	pids.push_back(processId);
	if(m_isVerbose) GetApplicationPath();
	
	std::string currentPath = GetCurrentDirectory();
	std::string args = GetCommandLineArgs(m_hProcess);
	batfile.push_back(args);
	Process newProcess(std::stoi(GetPID()), std::stoi(GetParentPID()), args, currentPath);
	if (m_init.GetChildren().size() == 0)
	{
		m_init.InsertChild(newProcess);
	}
	m_init.TraverseAndInsertChild(newProcess);

	if (createProcess.hFile)
	{
		CloseHandle(createProcess.hFile);
	}
	

}
/*
* Allows to get full path of running current application/process
*/
void ProcessTracer::GetApplicationPath()
{
	TCHAR programPath[MAX_PATH];
	if (GetModuleFileNameEx(m_hProcess, NULL, programPath, MAX_PATH) == 0) 
	{
		std::tcerr << "Failed to get path.\n";
	}
	else 
	{
		std::tcout << "Application Path:  \n" << programPath << "\n";
	}
}

/*
 evaluated from https://stackoverflow.com/questions/7446887/get-command-line-string-of-64-bit-process-from-32-bit-process
*/
std::string ProcessTracer::GetCommandLineArgs(HANDLE handle) 
{
	DWORD err = 0;
	SYSTEM_INFO system_info;
	GetNativeSystemInfo(&system_info);
	DWORD ProcessParametersOffset = system_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? 0x20 : 0x10;
	DWORD CommandLineOffset = system_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? 0x70 : 0x40;
	// read basic info to get ProcessParameters address, we only need the beginning of PEB
	DWORD pebSize = ProcessParametersOffset + 8;
	PBYTE peb = new BYTE[pebSize]{};
	DWORD ppSize = CommandLineOffset + 16;
	PBYTE pp = new BYTE[ppSize]{};
	PWSTR cmdLine;
	PROCESS_BASIC_INFORMATION pbi{};
	// get process information
	auto query = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
	err = query(handle, 0, &pbi, sizeof(pbi), NULL);
	if (err != 0) 
	{
		CloseHandle(handle);
		return false;
	}
	// read PEB
	if (!ReadProcessMemory(handle, pbi.PebBaseAddress, peb, pebSize, NULL)) 
	{
		CloseHandle(handle);
		return false;
	}
	// read ProcessParameters
	auto parameters = (PBYTE*)*(LPVOID*)(peb + ProcessParametersOffset); // address in remote process adress space
	if (!ReadProcessMemory(handle, parameters, pp, ppSize, NULL)) 
	{
		CloseHandle(handle);
		return false;
	}
	auto pCommandLine = (UNICODE_STRING*)(pp + CommandLineOffset);
	cmdLine = (PWSTR) new char[pCommandLine->MaximumLength];
	if (!ReadProcessMemory(handle, pCommandLine->Buffer, cmdLine, pCommandLine->MaximumLength, NULL)) 
	{
		CloseHandle(handle);
		return false;
	}
	
	m_cmd = ProcessTracer::ToString(cmdLine);
	delete[] cmdLine, peb, pp;
	std::cout <<"::::::::::::::::::: "<< m_cmd << std::endl;
	return m_cmd;
}
/*
* Takes current process snapshot  via Windows Tool Help Library
*/
void ProcessTracer::GetProcessInformation(DWORD processId,std::string processAddress, bool isEnabled) 
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error\n";
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap); 
	}
	
	while (Process32Next(hProcessSnap, &pe32))
	{
		
		if (pe32.th32ProcessID == processId)
		{
					SetParentPID(std::to_string(pe32.th32ParentProcessID));
					parent.push_back(std::stoi(GetParentPID()));
					SetPID(std::to_string(pe32.th32ProcessID));
					if (isEnabled) 
					{
						std::wcout << "Process ID      : " << pe32.th32ProcessID << "\n";
						std::wcout << "Process name    : " << pe32.szExeFile << "\n";
						std::wcout << "Thread counts   : " << pe32.cntThreads << "\n";
						std::wcout << "Parent ID       : " << pe32.th32ParentProcessID << "\n";
						
					}
		}
	}
	CloseHandle(hProcessSnap);
}

/*
* catch exit code for terminated process
* Modified function from @RogerOrr
*/
void ProcessTracer::OnExitProcess(DWORD processId, EXIT_PROCESS_DEBUG_INFO const & exitProcess, bool m_isVerbose)
{
	
	if (pids.front() == processId) 
	{
		m_IsInitRunning = true;
	} 
	if (m_isVerbose)std::cout << "\nPID " << processId << " EXIT CODE " << exitProcess.dwExitCode << "\n";
	
}

/*
* Process creating trigger with cli arguments.
* Windows Debug API will be attached here.
* By changing DEBUG_PROCESS, other option for Debug API could be used as needed
* Modified function from @RogerOrr
*/
void ProcessTracer::PTraceCreateProcess(int argc, TCHAR** begin)
{

	++begin;
	--argc;
	TCHAR** end = begin + argc;
	CString cmdLine;
	
	for (TCHAR** it = begin; it != end; ++it)
	{
		if (!cmdLine.IsEmpty()) cmdLine += ' ';
	
		if (_tcschr(*it, ' '))
		{
			cmdLine += '"';
			cmdLine += *it;
			cmdLine += '"';
		}
		else
		{
			cmdLine += *it;
		}
	}
	
	std::string cliArgs = CharToString();
	ParseArgs(cliArgs);
	STARTUPINFO startupInfo = { sizeof(startupInfo) };
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	startupInfo.wShowWindow = SW_SHOWNORMAL;
	PROCESS_INFORMATION ProcessInformation = { 0 };
	
	if (!CreateProcess(0, const_cast<TCHAR *>(cmdLine.GetString()), 0, 0, true,	DEBUG_PROCESS,	0, 0, &startupInfo, &ProcessInformation))
	{
		std::ostringstream oss;
		oss << GetLastError();
#ifdef UNICODE
		size_t len = _tcslen(*begin) + 1;
		char *str = new char[len];
		wcstombs(str, *begin, len);
		throw std::runtime_error(std::string("No such an application to start: ") + str + ": " + oss.str());
		delete str;
#else
		throw std::runtime_error(std::string("Unable to start ") + *begin + ": " + oss.str());
#endif
	}
	CloseHandle(ProcessInformation.hProcess);
	CloseHandle(ProcessInformation.hThread);
}

/*
* Windows specific data type conversion
* This case WChar to std::string
*/
std::string ProcessTracer::CharToString()
{
	LPWSTR* arguments;
	char buffer[500];
	int numberArgs;
	std::string cliArgs;
	arguments = CommandLineToArgvW(GetCommandLineW(), &numberArgs);
	if (NULL == arguments)
	{
		wprintf(L"CommandLineToArgvW failed\n");
	}
	else for (int i = 1, c = 0; i < numberArgs; i++, c++)
	{
		wcstombs(buffer, arguments[i], 500);
		cliArgs += buffer;
	}
	return cliArgs;
}

/*
* Basic command line arguments parsing
*/
void ProcessTracer::ParseArgs(std::string cliArgs)
{
	if (cliArgs == "--help" || cliArgs == "-h")
	{
		std::cout << "PTracer is a command line application which observes any applications processes on Windows 10.\n";
		std::cout << "PTracer observe a process and its child process\n";
		std::cout << "\n";
		std::cout << "Options\n\n";
		std::cout << "--help, -h      			 help and options\n";
		std::cout << "--version      				 see version\n";
		std::cout << "--verbose, -v 				 verbose\n";
		std::cout << "--output, -o 				 output as json\n\n";
		std::cout << "Usage:\n";
		std::cout << "PTrace [application] \n";
		std::cout << "\n";
		exit(1);
	}else if (cliArgs == "--version")
	{
		std::cout << "version: 0.1.0";
		exit(1);
	}
	else if (cliArgs == "--verbose")
	{
		std::cout << "not implemented yet :(";
		exit(1);
	}
	else if (cliArgs == "--output")
	{
		std::cout << "not implemented yet :(";
		exit(1);
	}else if(cliArgs == "-i" || cliArgs == "--info")
	{
		m_isEnabledInfo = true;
		std::cout << "not :(";
	}

}




