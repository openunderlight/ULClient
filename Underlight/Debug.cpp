
// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "Central.h"
#include <windows.h>
#include <stdio.h>
#include "cGameServer.h"
#include "Realm.h"
#include "Interface.h"
#include "Dialogs.h"
#include "Options.h"
#include "Resource.h"
#include "Utils.h"

#ifdef AGENT
#include "AMsg_AgentInfo.h"
#endif

//////////////////////////////////////////////////////////////
// Constants

const unsigned int AGENT_RESTART_INTERVAL = 300000;

//////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cGameServer *gs;
extern options_t options;
extern bool exiting;

//////////////////////////////////////////////////////////////
// Local Global Variables


void __cdecl Info( UINT rid,  int line, TCHAR *sourcefile)
{
	TCHAR buffer[DEFAULT_MESSAGE_SIZE];
	LoadString(hInstance, rid, buffer, sizeof(buffer));
	Info(buffer, line, sourcefile);
}

void __cdecl Info( TCHAR *msg,  int line, TCHAR *sourcefile)
{
	TCHAR buf[DEFAULT_MESSAGE_SIZE];

	if (msg == NULL)
	{
		LoadString(hInstance, IDS_INFO1, temp_message, sizeof(temp_message));
		_stprintf(buf, _T(""), line, sourcefile);
	}
	else
	{
#ifdef UL_DEBUG
		LoadString(hInstance, IDS_INFO2, temp_message, sizeof(temp_message));
		_stprintf(buf, temp_message, msg, line, sourcefile);
#else
		LoadString(hInstance, IDS_INFO3, temp_message, sizeof(temp_message));
		_stprintf(buf, temp_message, msg);
#endif
	}

	_tprintf(_T("%s"), buf);

}

void __cdecl Warn( UINT rid, int line, TCHAR* sourcefile)
{
	TCHAR buffer[DEFAULT_MESSAGE_SIZE];
	LoadString(hInstance, rid, buffer, sizeof(buffer));
	Warn(buffer, line, sourcefile);
}


void __cdecl Warn( TCHAR *msg,  int line, TCHAR *sourcefile)
{
	TCHAR buf[DEFAULT_MESSAGE_SIZE];
	
	if (msg == NULL)
	{
		LoadString(hInstance, IDS_UNKNOWN_WARNING, temp_message, sizeof(temp_message));
		_stprintf(buf, temp_message, line, sourcefile);
	}
	else
	{
		LoadString(hInstance, IDS_KNOWN_WARNING, temp_message, sizeof(temp_message));
		_stprintf(buf, temp_message, msg, line, sourcefile);
	}
	 

	//if (options.network && gs && gs->LoggedIntoGame())
	//	gs->Talk(buf, RMsg_Speech::REPORT_DEBUG, Lyra::ID_UNKNOWN, false);

//	display->DisplayMessage(buf, true);

	_tprintf(_T("%s"), buf);

}


// this is just a helper that loads & formats an error string
void __cdecl ErrAndExit2( int type, UINT rid, int line, TCHAR* sourcefile, int err, ...)
{
	TCHAR buffer1[DEFAULT_MESSAGE_SIZE];
	TCHAR buffer2[DEFAULT_MESSAGE_SIZE];
	LoadString(hInstance, rid, buffer1, sizeof(buffer1));
	va_list ap;
	va_start(ap,err);
	_vstprintf(buffer2,buffer1,ap);
	va_end(ap);
	ErrAndExit(type, buffer2, line, sourcefile, err);
}

void __cdecl ErrAndExit( int type, UINT rid, int line, TCHAR* sourcefile, int err)
{
	TCHAR buffer[DEFAULT_MESSAGE_SIZE];
	LoadString(hInstance, rid, buffer, sizeof(buffer));
	ErrAndExit(type, buffer, line, sourcefile, err);
}

void __cdecl ErrAndExit( int type, TCHAR *errmsg, int line, TCHAR* sourcefile, int err )
{
	TCHAR err_descrip[DEFAULT_MESSAGE_SIZE];
	TCHAR report_buf[DEFAULT_MESSAGE_SIZE];
	TCHAR display_buf[DEFAULT_MESSAGE_SIZE];
	int iRc;

#ifndef AGENT
	static bool previous_error = false;
	if (previous_error)
	{
		exit(-1);
		return;
	}
	previous_error = true;
#endif

	if (errmsg == NULL)
		LoadString(hInstance, IDS_UNKNOWN_ERROR, err_descrip, sizeof(err_descrip));
	else
		_tcscpy(err_descrip, errmsg);

	switch (type)
	{
		case SOCKETS_ERR:
			if (err)
				iRc = err;
			else
				iRc = WSAGetLastError();

		LoadString (hInstance, IDS_NO_SERVER, disp_message, sizeof(disp_message));
		_stprintf(display_buf, disp_message, err_descrip);
#ifdef UL_DEBUG
		LoadString(hInstance, IDS_NETWORK_ERROR, disp_message, sizeof(disp_message));
		_stprintf(report_buf, disp_message, iRc, line, sourcefile);
#endif UL_DEBUG

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
					iRc, LANG_SYSTEM_DEFAULT, err_descrip,
					sizeof(err_descrip), NULL);
		LoadString(hInstance, IDS_NETWORK_ERROR2, disp_message, sizeof(disp_message));
		_stprintf(report_buf, disp_message, iRc, err_descrip, line, sourcefile );

			break;
		case WINDOWS_ERR:
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
					GetLastError(), LANG_SYSTEM_DEFAULT, err_descrip,
					sizeof(err_descrip), NULL);
		case GENERIC_ERR:
		default:
			LoadString (hInstance, IDS_FATAL_ERROR, disp_message, sizeof(disp_message));
		_stprintf(display_buf, disp_message, err_descrip);
			LoadString (hInstance, IDS_FATAL_ERROR, disp_message, sizeof(disp_message));
		_stprintf(report_buf, disp_message, err_descrip, line, sourcefile);
			break;
	}
	if (output)
	{
		LoadString(hInstance, IDS_ERROR, temp_message, sizeof(temp_message));
		_stprintf(disp_message, temp_message, report_buf);
		_tprintf(_T("%s"), disp_message);
	}

#ifdef AGENT
	if (!PrimaryThread())
	{	// abort & set to restart again after an interval
		// randomize restart time to avoid agents restarting in one room
		agent_info[AgentIndex()].restart_time = LyraTime() + AGENT_RESTART_INTERVAL + (rand()%AGENT_RESTART_INTERVAL);
		_tprintf(_T("Agent %d aborted at time %d\n"), agent_info[AgentIndex()].id, LyraTime());
		DeInitAgent(AgentIndex());
		agent_info[AgentIndex()].status = AMsg_AgentInfo::STATUS_ABORTED;
		_endthread();
		return;
	}
#else
//	if (options.network && gs && gs->LoggedIntoGame())
//		gs->Talk(report_buf, RMsg_Speech::REPORT_DEBUG, Lyra::ID_UNKNOWN);

    exiting = true;
	_stprintf(message, display_buf);
	LyraDialogBox(hInstance, IDD_FATAL_ERROR, NULL, (DLGPROC)FatalErrorDlgProc);
	Exit();
	exit(-1);
#endif
	return;
}
