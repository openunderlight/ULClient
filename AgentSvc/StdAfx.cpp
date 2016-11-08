// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>

#include "Realm.h"

extern TCHAR agent_working_directory[_MAX_PATH];
extern CServiceModule _Module;

bool CServiceModule::SetWorkingDirectory(void)
{
	// change to a working directory
	memset(agent_working_directory,0,sizeof(agent_working_directory));
	HKEY reg_key = NULL;
	unsigned long result,size;
	DWORD reg_type;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, AGENT_REGISTRY_KEY,0, 
		NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
	if (result == REG_OPENED_EXISTING_KEY)
	{
		size = sizeof(agent_working_directory);
		RegQueryValueEx(reg_key, _T("agent_working_directory"), NULL, &reg_type,
			(unsigned char *)agent_working_directory, &size);
		RegCloseKey(reg_key);
	}
	else
		return false;
	
	if(_tchdir(agent_working_directory))
		return false;

	return true;
	
}


void __cdecl MonitorAgents (void* param)
{

	HANDLE hProcess = (HANDLE)param;
	int num_deaths = 0;
    // Wait until child process exits.
	while (num_deaths < 9999)
	{
		WaitForSingleObject( hProcess, INFINITE );
		num_deaths++;
		Sleep(180000);
		_Module.LogEvent(_T("Restarting agent daemon after exit!\n"));
		hProcess = _Module.SpawnAgents();
	}
}

HANDLE CServiceModule::SpawnAgents(void)
{
	CloseHandle(m_pi.hProcess);
	CloseHandle(m_pi.hThread);
    // Start the child process. 
    if( !CreateProcess( NULL, // No module name (use command line). 
#ifdef _DEBUG
        _T("agentsvrd.exe"), // Command line. 
#else
        _T("agentsvr.exe"), // Command line. 
#endif
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        agent_working_directory,             
        &m_si,              // Pointer to STARTUPINFO structure.
        &m_pi )             // Pointer to PROCESS_INFORMATION structure.
    ) 
    {
		LogEvent(_T("Could not create agent daemon process!\n"));
		return NULL;
    }
	else
		return m_pi.hProcess;
}

