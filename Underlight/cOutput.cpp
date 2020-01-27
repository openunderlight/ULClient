// Copyright Lyra LLC, 1997. All rights reserved.

#define STRICT

#include "cOutput.h"
#include "Utils.h"
#include "resource.h"
#include <time.h>
#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "shlobj.h"
#include "tchar.h"

// define the following to enable buffering on debug output
#define BUFFER_DEBUG_OUT

//////////////////////////////////////////////////////////////////
// External

extern cOutput *output;
extern HINSTANCE hInstance;

/////////////////////////////////////////////////////
/// Class Definition

// Constructer

cOutput::cOutput(TCHAR *fn, bool append_to_file, bool fForceFlush /*= false */)
{

	append = append_to_file;
	m_fForceFlush = fForceFlush;
	_tcscpy(filename, fn);
	_tcscat(filename, _T(".out"));
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		PathAppend(szPath, _T("\\Underlight\\"));
		CreateDirectory(szPath, NULL);
		PathAppend(szPath, filename);
	}
	// check file size; rename if necessary
	HANDLE hFile;
	BY_HANDLE_FILE_INFORMATION info;
	hFile = CreateFile(szPath, GENERIC_READ , FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile) 
	{
		GAME_ERROR(IDS_DEBUGOUT_ERR);
		return;
	}

	GetFileInformationByHandle(hFile, &info);

	CloseHandle(hFile);

	if (info.nFileSizeLow > 1048576) 
	{
		SYSTEMTIME dsttime;
		GetDSTTime(&dsttime);
		TCHAR backup_filename[_MAX_DIR];
		
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath_backup)))
		{
			PathAppend(szPath, _T("\\Underlight\\"));
			CreateDirectory(szPath, NULL);
			PathAppend(szPath, backup_filename);
		}
		LoadString(hInstance, IDS_BACKUP_LOG, message, sizeof(message));
		_stprintf(szPath_backup, message, fn, dsttime.wMonth, dsttime.wDay, dsttime.wYear);
		int result = rename(szPath, szPath_backup);
		int qqq=0;
	}
	
	
	// insure a directory component exists. if not, create it
	// mket 11/02/01
	TCHAR dir[_MAX_DIR];

#ifdef _UNICODE

	_wsplitpath( fn, NULL, dir, NULL, NULL );
	if (wcslen(dir)) 
		CreateDirectory(dir, NULL);

#else

	_splitpath( fn, NULL, dir, NULL, NULL );
	if (strlen(dir)) 
		CreateDirectory(dir, NULL);

#endif

	if (append)
		if (fForceFlush)
			fh =_tfopen(szPath,_T("a+c"));
		else
			fh =_tfopen(szPath,_T("a+"));
	else
		if (fForceFlush)
			fh =_tfopen(szPath,_T("wc"));
		else
			fh =_tfopen(szPath,_T("w"));

	if (fh == NULL )
	{
		GAME_ERROR(IDS_DEBUGOUT_ERR);
		return;
	}

	// Set time zone from TZ environment variable. If TZ is not set,
	// the operating system is queried to obtain the default value
	// for the variable.
	_tzset();

	Write(_T("\n"), true);
	LoadString(hInstance, IDS_STARTLOG , temp_message, sizeof(temp_message));
	Write(temp_message, true);

	return;
}


void cOutput::Write(TCHAR *data, bool long_date /* = false */)
{
	fwrite(data, _tcslen(data),1,fh);
	if (m_fForceFlush)
		fflush(fh);

	unsigned int pos =_tcsrchr(data,'\n') - data;
	if (pos==_tcslen(data)-1)
		WriteStamp(long_date);
}

void cOutput::WriteStamp(bool long_date /* = false */)
{
	// local time as a string
	struct tm *newtime;
	time_t aclock;
	time (&aclock);					// Get time in seconds
	newtime = localtime (&aclock);	// Convert time to struct
	TCHAR timebuf[128];

	if (long_date)
	_stprintf(temp_message, _T("%.24s: "), asctime (newtime));
	else
	_stprintf(temp_message, _T("%s: "), _tstrtime(timebuf));

	fwrite(temp_message, _tcslen(temp_message),1,fh);
	if (m_fForceFlush)
		fflush(fh);

	return;
}

void cOutput::ReInit(void)
{
	fclose(fh);

	if (append)
		fh =_tfopen(szPath,_T("a+"));
	else
		fh =_tfopen(szPath,_T("w"));

	if (fh == NULL )
	{
		GAME_ERROR(IDS_DEBUGOUT_ERR);
		return;
	}

	return;
}

// Destructor
cOutput::~cOutput(void)
{
#ifndef AGENT
	LoadString(hInstance, IDS_ENDLOG , temp_message, sizeof(temp_message));
	Write(temp_message, true);

	Write(_T(""), true);
#endif !AGENT

	fclose(fh);

#ifdef AGENT
	TlsSetValue(tlsOutput, NULL);
#endif AGENT

}

// helpers - we need an outside function so that we can use
// the variable length arguments for the printf #define
void __cdecl DebugOut(TCHAR *args,...)
{
	TCHAR DebugBuffer[DEFAULT_MESSAGE_SIZE];

	va_list ap;
	va_start(ap,args);
_vstprintf(DebugBuffer,args,ap);
	va_end(ap);

	OutputDebugString(DebugBuffer);
	if(_tcslen(DebugBuffer) && DebugBuffer[_tcslen(DebugBuffer)-1]!='\n')
		OutputDebugString(_T("\n"));

	int i = _tcslen(DebugBuffer);
#if defined (UL_DEBUG) || defined (GAMEMASTER)
	cOutput* pOutput = output;
	if (!pOutput) // do nothing if variable not defined or if not gm
#endif
		return;

	fwrite(DebugBuffer,_tcslen(DebugBuffer),1,output->FileHandle());
#ifndef BUFFER_DEBUG_OUT
	if(output) output->ReInit();
#endif
}

// Check invariants

#ifdef CHECK_INVARIANTS
void cOutput::CheckInvariants(int line, TCHAR *file)
{

}
#endif