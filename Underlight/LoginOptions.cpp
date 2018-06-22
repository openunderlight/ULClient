// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include "Realm.h"
#include "Resource.h"
#include "cGameServer.h"
#include "LoginOptions.h"
#include "cDDraw.h"
#include "Options.h"
#include "Dialogs.h"
//#include "RogerWilco.h"

/////////////////////////////////////////////////////////////////
// Global Variables

extern options_t options;
extern bool show_training_messages;
extern HINSTANCE hInstance;
extern cDDraw* cDD;
extern pmare_t pmare_info[3];
extern int numJsonFiles;

// these are for getting the Windows Version #
//extern unsigned int _osver; // only _winmajor is used here
//extern unsigned int _winmajor;
//extern unsigned int _winminor;
//extern unsigned int _winver;


//////////////////////////////////////////////////////////////////
// Functions

//////////////////////////////////////////////////
// Login Dialog
BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HKEY reg_key;
	DWORD result;

	switch(Message)
	{
		case WM_DESTROY:
			break;

		case WM_INITDIALOG:
		{
			//RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(true),0, 
			//				NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
			//LoadOutOfGameRegistryOptionValues(reg_key, false);
			//RegCloseKey(reg_key);
			
			RECT rect1, rect2; 
			GetWindowRect(GetParent(hDlg), &rect1);
			GetWindowRect(hDlg, &rect2);
			int pos_x = rect1.left + ((rect1.right - rect1.left) - (rect2.right - rect2.left))/2;
			int pos_y = rect1.top + ((rect1.bottom - rect1.top) -  (rect2.bottom - rect2.top))/2;
			//SetWindowPos(hDlg, HWND_TOPMOST,pos_x, pos_y, 0,0, SWP_NOSIZE);
			SetWindowPos(hDlg, HWND_TOPMOST, 15, 10, 0,0, SWP_NOSIZE);

			Button_SetCheck(GetDlgItem(hDlg, IDC_RESTART_LOCATION),  options.restart_last_location);

			ShowWindow(GetDlgItem(hDlg, IDC_ENABLE_RW), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_ROGERWILCO), SW_HIDE);

#ifdef CHINESE
			ShowWindow(GetDlgItem(hDlg, IDC_RESTART_LOCATION), SW_SHOW);
#else
			ShowWindow(GetDlgItem(hDlg, IDC_RESTART_LOCATION), SW_HIDE);
#endif

#ifndef UL_DEBUG
			EnableWindow(GetDlgItem(hDlg, IDC_ROGERWILCO), false);
			EnableWindow(GetDlgItem(hDlg, IDC_ENABLE_RW), false);
#else
			Button_SetCheck(GetDlgItem(hDlg, IDC_ENABLE_RW),  options.rw);			
			Button_SetCheck(GetDlgItem(hDlg, IDC_SOUND),  options.sound);
#endif
//			Button_SetCheck(GetDlgItem(hDlg, IDC_CREATE_CHARACTER), options.create_character);

#ifdef UL_DEBUG
			//MessageBox(NULL, "debug", "debug", MB_OK);
			ShowWindow(GetDlgItem(hDlg, IDC_NETWORK),SW_SHOW);
			Button_SetCheck(GetDlgItem(hDlg, IDC_NETWORK),  options.network);

			ShowWindow(GetDlgItem(hDlg, IDC_DEBUG),SW_SHOW);
			Button_SetCheck(GetDlgItem(hDlg, IDC_DEBUG),  options.debug);

#ifdef UL_DEBUG_MASTER
			ShowWindow(GetDlgItem(hDlg, IDC_LIVE_SERVER),SW_SHOW);
			ShowWindow(GetDlgItem(hDlg, IDC_SPECIFY_SERVER),SW_SHOW);
			ShowWindow(GetDlgItem(hDlg, IDC_IP_ADDRESS),SW_SHOW);
			ShowWindow(GetDlgItem(hDlg, IDC_AGREEMENT_TEXT), SW_HIDE);
#endif UL_DEBUG_MASTER

#endif// UL_DEBUG

#ifdef UL_DEV
			ShowWindow(GetDlgItem(hDlg, IDC_DEV_SERVER1),SW_SHOW);
			ShowWindow(GetDlgItem(hDlg, IDC_DEV_SERVER2),SW_SHOW);
			ShowWindow(GetDlgItem(hDlg, IDC_CUSTOM_DEV_SERVER), SW_SHOW);
			
			if (options.dev_server == 1)
			{
				Button_SetCheck(GetDlgItem(hDlg, IDC_DEV_SERVER1), 1);
			}
			else if (options.dev_server == 2)
			{
				Button_SetCheck(GetDlgItem(hDlg, IDC_DEV_SERVER2), 1);
			}
			else if (options.dev_server == 3)
			{
				Button_SetCheck(GetDlgItem(hDlg, IDC_CUSTOM_DEV_SERVER), 1);
				ShowWindow(GetDlgItem(hDlg, IDC_CUSTOM_IP), SW_SHOW);
				Edit_SetText(GetDlgItem(hDlg, IDC_CUSTOM_IP), options.custom_ip);
			}

			_stprintf(message, _T("%s"), options.custom_ip);
			Edit_SetText(GetDlgItem(hDlg, IDC_CUSTOM_IP), message);
#endif // UL_DEV

			TCHAR huge_text[4096];
			TCHAR disp1[1024]; TCHAR disp2[1024]; TCHAR disp3[1024]; TCHAR disp4[1024]; 
			LoadString (hInstance, IDS_AGREEMENT_TEXT1, disp1, sizeof(disp1));
			LoadString (hInstance, IDS_AGREEMENT_TEXT2, disp2, sizeof(disp2));
			LoadString (hInstance, IDS_AGREEMENT_TEXT3, disp3, sizeof(disp3));
			LoadString (hInstance, IDS_AGREEMENT_TEXT4, disp4, sizeof(disp4));
		_stprintf(huge_text, _T("%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s"), disp1, disp2, disp3, disp4);

			SendMessage(GetDlgItem(hDlg, IDC_AGREEMENT_TEXT), WM_SETTEXT, 0, (LPARAM) huge_text); 
			ShowWindow(GetDlgItem(hDlg, IDC_PASSWORD), SW_SHOW);

			// now set username/password based on current combo box selection
			for (int i = 0; i < MAX_STORED_ACCOUNTS; i++)	{
				ComboBox_AddString(GetDlgItem(hDlg, IDC_USERNAME), options.username[i]);
			}
		
			SetWindowText( GetDlgItem(hDlg, IDC_PASSWORD), options.password[options.account_index]);
			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_USERNAME), options.account_index);

			//SetWindowText( GetDlgItem(hDlg, IDC_USERNAME), options.username[options.account_index]);
		}
		case WM_SET_LOGIN_DEFAULTS:
		{
			Button_SetCheck(GetDlgItem(hDlg, IDC_SOUND), options.sound);
			Button_SetCheck(GetDlgItem(hDlg, IDC_EXTRA_SCROLL), options.extra_scroll);
#ifdef PMARE
			Button_SetCheck(GetDlgItem(hDlg, IDC_TRAINING), false);
			ShowWindow(GetDlgItem(hDlg, IDC_TRAINING), SW_HIDE);
#else
			Button_SetCheck(GetDlgItem(hDlg, IDC_TRAINING), options.welcome_ai);
#endif
			if (options.tcp_only)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_BIND_UDP), FALSE);
			}
			_stprintf(message, _T("%d"), options.bind_local_udp);
			Edit_SetText(GetDlgItem(hDlg, IDC_BIND_UDP), message);

			if (options.resolution == 800) {
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES640), 0);
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES800), 1);
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES1024), 0);
			}
			else {
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES640), 0);
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES800), 0);
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES1024), 1);
			}
#ifdef PMARE
			ComboBox_ResetContent(GetDlgItem(hDlg, IDC_PMARE_LIST));
			ShowWindow(GetDlgItem(hDlg, IDC_URL), SW_HIDE);
			LoadString(hInstance, IDS_PMARE_BOGROM, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_PMARE_LIST), message);
			LoadString(hInstance, IDS_PMARE_AGOKNIGHT, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_PMARE_LIST), message);
			LoadString(hInstance, IDS_PMARE_SHAMBLIX, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_PMARE_LIST), message);

			// get time in seconds to determine if session can be resume
			SYSTEMTIME cur_time;
			//unsigned int cur_time = (LyraTime()/1000);
			GetLocalTime(&cur_time);

			// if we have a session stored in the registry, and it has 
			// been less than 24 hours, allow them to resume
			//			sprintf(message, "%d : %d", options.pmare_session_start, cur_time);
			//			MessageBox(NULL, message, "Hello", MB_OK);
			if ((options.pmare_type > Avatars::MIN_NIGHTMARE_TYPE) &&
				(Within48Hours(options.pmare_session_start, cur_time)))
			{
				LoadString(hInstance, IDS_PMARE_RESUME, message, sizeof(message));
				float price = (float)options.pmare_price;
				price = price / 100;
				_stprintf(disp_message, message, price);
				ComboBox_AddString(GetDlgItem(hDlg, IDC_PMARE_LIST), disp_message);
				ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_PMARE_LIST), 3); // Default to resume session
			}
			else {
				ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_PMARE_LIST), 0); // Default to weakest option
			}
#else 
			ShowWindow(GetDlgItem(hDlg, IDC_STATIC_PMARE), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_PMARE_LIST), SW_HIDE);
#endif
			return TRUE;
		}
		case WM_COMMAND:
			result = LOWORD(wParam);
			switch (LOWORD(wParam))
			{
			case IDC_ROGERWILCO:
#ifndef AGENT //RW_ENABLED
				//RWVoice_Configure(NULL, NULL);
#endif
				return TRUE;
			case IDC_MANUAL:
				LoadString (hInstance, IDS_GAME_MANUAL_URL, message, sizeof(message));
				ShellExecute(NULL, _T("open"), message, NULL, NULL, SW_SHOWNORMAL);
				return TRUE;

			case IDC_BILLING:
				LoadString (hInstance, IDS_BILLING_URL, message, sizeof(message));
				ShellExecute(NULL, _T("open"), message, NULL, NULL, SW_SHOWNORMAL);
				return TRUE;

			case IDC_LAUNCH_OPTIONS:
				DialogBox(hInstance, MAKEINTRESOURCE(IDD_LAUNCH_OPTIONS), NULL, (DLGPROC)LaunchOptionsDlgProc);
				return TRUE;
			case IDC_DEV_SERVER1:
			case IDC_DEV_SERVER2:
				ShowWindow(GetDlgItem(hDlg, IDC_CUSTOM_IP), SW_HIDE);
				return TRUE;
			case IDC_CUSTOM_DEV_SERVER:
				ShowWindow(GetDlgItem(hDlg, IDC_CUSTOM_IP), SW_SHOW);
				return TRUE;
			case IDC_OK:
				{
#ifdef UNICODE
				wcstombs(options.game_server, _T("\0"), sizeof(options.game_server)); 
#else
				strcpy(options.game_server, "\0"); 
#endif
				options.server_port = 0;
				//int j = strlen(options.game_server);

				options.sound			= Button_GetCheck(GetDlgItem(hDlg, IDC_SOUND));
				options.rw				= Button_GetCheck(GetDlgItem(hDlg, IDC_ENABLE_RW));
				options.welcome_ai		= Button_GetCheck(GetDlgItem(hDlg, IDC_TRAINING)); 
				options.extra_scroll	= Button_GetCheck(GetDlgItem(hDlg, IDC_EXTRA_SCROLL)); 
				Edit_GetText(GetDlgItem(hDlg, IDC_BIND_TCP), message, sizeof(message)); 								
				options.bind_local_tcp	= _ttol(message);
				Edit_GetText(GetDlgItem(hDlg, IDC_BIND_UDP), message, sizeof(message)); 								
#ifndef AGENT
				options.tcp_only = TRUE;
#endif

#ifdef CHINESE
				options.restart_last_location = Button_GetCheck(GetDlgItem(hDlg, IDC_RESTART_LOCATION));
#endif
				
				if (Button_GetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES800)))
					options.resolution = 800;
				else if (Button_GetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES1024)))
					options.resolution = 1024;
				else
					options.resolution = 640;

#ifdef UL_DEV
				if (Button_GetCheck(GetDlgItem(hDlg, IDC_DEV_SERVER1)))
					options.dev_server = 1;
				else if (Button_GetCheck(GetDlgItem(hDlg, IDC_DEV_SERVER2)))
					options.dev_server = 2;
				else if (Button_GetCheck(GetDlgItem(hDlg, IDC_CUSTOM_DEV_SERVER)))
				{
					options.dev_server = 3;
					GetWindowText(GetDlgItem(hDlg, IDC_CUSTOM_IP), options.custom_ip, sizeof(options.custom_ip));
				}
#endif

#ifdef PMARE
				int pmare_avatar_type = 3 + ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_PMARE_LIST));
				// if pmare_avatar_type > Horron, it means resume previous session
				if (pmare_avatar_type <= Avatars::SHAMBLIX) {
					options.pmare_type = pmare_avatar_type;
					options.pmare_price = (int)((pmare_info[options.pmare_type - 3].charge)*100 + .1);
					options.pmare_start_type = pmare_avatar_type;
				}
				else
					options.pmare_type = Avatars::PMARE_RESUME;

#endif
//				options.create_character= Button_GetCheck(GetDlgItem(hDlg, IDC_CREATE_CHARACTER)); 
#ifdef UL_DEBUG
				options.network			= Button_GetCheck(GetDlgItem(hDlg, IDC_NETWORK)); 
				options.debug			= Button_GetCheck(GetDlgItem(hDlg, IDC_DEBUG)); 
				if (options.debug)
					 options.server_port = 7599;

#endif


#ifdef UL_DEV
//				LoadString(hInstance, IDS_DEV_GAMED_URL, options.gamed_URL, sizeof(options.gamed_URL));
				LoadString(hInstance, IDS_DEV_PATCH_FILE_URL, options.patch_URL, sizeof(options.patch_URL));
				LoadString(hInstance, IDS_DEV_GAME_SERVER_IP1, options.game_server, sizeof(options.game_server)) ;

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_LIVE_SERVER)))
				{
					LoadString(hInstance, IDS_LIVE_PATCH_FILE_URL, options.patch_URL, sizeof(options.patch_URL));
					//					LoadString(hInstance, IDS_LIVE_GAMED_URL, options.gamed_URL, ) ;
					LoadString(hInstance, IDS_LIVE_GAME_SERVER_IP, options.game_server, sizeof(options.game_server));
				}
				else if (Button_GetCheck(GetDlgItem(hDlg, IDC_DEV_SERVER2)))
					//GetWindowText(GetDlgItem(hDlg, IDC_IP_ADDRESS), options.game_server, sizeof(options.game_server));
					LoadString(hInstance, IDS_DEV_GAME_SERVER_IP2, options.game_server, sizeof(options.game_server));
				else if (Button_GetCheck(GetDlgItem(hDlg, IDC_CUSTOM_DEV_SERVER)))
					memcpy(options.game_server,options.custom_ip, sizeof(options.custom_ip));
#else
//				LoadString(hInstance,options.gamed_URL, LIVE_GAMED_URL);
//#define MANACCOM
// we keep a separate patch file to use against the retail builds

#ifdef MANACCOM
				LoadString(hInstance, IDS_LIVE_MANACCOM_PATCH_FILE_URL, options.patch_URL, sizeof(options.patch_URL));
#else
				LoadString(hInstance, IDS_LIVE_PATCH_FILE_URL, options.patch_URL, sizeof(options.patch_URL));
#endif
				LoadString(hInstance, IDS_LIVE_GAME_SERVER_IP, options.game_server, sizeof(options.game_server)); 

#endif // UL_DEV
				show_training_messages = options.welcome_ai;
				
//				sprintf(message, "%s", options.game_server);
//				MessageBox(NULL, message, message, MB_OK);

				// now check the username and password; if they're not already in
				// the list, add them

				TCHAR username[Lyra::PLAYERNAME_MAX];
				TCHAR password[Lyra::PASSWORD_MAX];
				GetWindowText( GetDlgItem(hDlg, IDC_USERNAME), username, Lyra::PLAYERNAME_MAX);
				username[Lyra::PLAYERNAME_MAX-1] = '\0';
				GetWindowText( GetDlgItem(hDlg, IDC_PASSWORD), password, Lyra::PASSWORD_MAX);
				password[Lyra::PASSWORD_MAX-1] = '\0';

				int empty_slot = -1;
				bool found = false;
				for (int i=0; i<MAX_STORED_ACCOUNTS; i++) {
					if (!_tcscmp(username, options.username[i])) {
						found = true;
						options.account_index = i;
						_tcscpy(options.password[i], password);
						break;
					}
					if ((_tcslen(options.username[i]) < 2) && (-1 == empty_slot))
						empty_slot = i;
				}

				if (!found) { // not found in list; store at first empty slot
					if (-1 == empty_slot)  // no empties in list; pick one arbitrarily
						empty_slot = rand()%MAX_STORED_ACCOUNTS;

					_tcscpy(options.username[empty_slot], username);
					_tcscpy(options.password[empty_slot], password);
					options.account_index = empty_slot;
				}
				SaveOutOfGameRegistryOptionValues();				

				/*
				// now save version # in separate reg key
				RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T(VERSION_KEY), 0, NULL, 0, 
								KEY_ALL_ACCESS, NULL, &reg_key, &result);
				RegSetValueEx(reg_key, _T("Version"), 0, REG_DWORD,  
					(unsigned char *)&(GAME_CURRENT_VERSION), sizeof(GAME_CURRENT_VERSION));
				RegCloseKey(reg_key);
				*/


				EndDialog(hDlg, TRUE);
				return TRUE;
				}
			case 2: // hardcode for close window via X button in upper right
			case IDC_CANCEL:
				EndDialog(hDlg, FALSE);
				return FALSE;

			case IDC_USERNAME:
				if (HIWORD(wParam) == LBN_SELCHANGE) {
					options.account_index = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_USERNAME));
					SetWindowText(GetDlgItem(hDlg, IDC_PASSWORD), options.password[options.account_index]);
					LoadJSONOptionValues(options.username[options.account_index]);
					SendMessage(hDlg, WM_SET_LOGIN_DEFAULTS, 0, 0);
				}
			}
			break;
	}
	return FALSE;		
} 

BOOL CALLBACK LaunchOptionsDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_DESTROY:
			break;

		case WM_INITDIALOG:
		{	
			RECT rect1, rect2; 
			GetWindowRect(GetParent(hDlg), &rect1);
			GetWindowRect(hDlg, &rect2);
			int pos_x = rect1.left + ((rect1.right - rect1.left) - (rect2.right - rect2.left))/2;
			int pos_y = rect1.top + ((rect1.bottom - rect1.top) -  (rect2.bottom - rect2.top))/2;
			//SetWindowPos(hDlg, HWND_TOPMOST,pos_x, pos_y, 0,0, SWP_NOSIZE);
			SetWindowPos(hDlg, HWND_TOPMOST, 15, 10, 0,0, SWP_NOSIZE);

			// now load the instructions from the string table
			LoadString (hInstance, IDS_EXCLUSIVE_MODE, message, sizeof(message));
			SetWindowText( GetDlgItem(hDlg, IDC_EXCLUSIVE_MODE), message);
			LoadString (hInstance, IDS_SOUND_ENABLED, message, sizeof(message));
			SetWindowText( GetDlgItem(hDlg, IDC_SOUND_ENABLED), message);
			LoadString (hInstance, IDS_START_TRAINING, message, sizeof(message));
			SetWindowText( GetDlgItem(hDlg, IDC_START_TRAINING), message);
			LoadString (hInstance, IDS_EXTRA_SCROLL, message, sizeof(message));
			SetWindowText( GetDlgItem(hDlg, IDC_EXTRA_SCROLL_TEXT), message);
			LoadString (hInstance, IDS_BIND_TEXT, message, sizeof(message));
			SetWindowText( GetDlgItem(hDlg, IDC_BIND_TEXT), message);
			LoadString (hInstance, IDS_UDP_PROXY_TEXT, message, sizeof(message));
			SetWindowText( GetDlgItem(hDlg, IDC_UDP_PROXY_TEXT), message);


			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_OK:
				case 2: // hardcode for close window via X button in upper right
					EndDialog(hDlg, TRUE);
					return TRUE;
				case IDC_FIREWALL_FAQ:
					LoadString (hInstance, IDS_FIREWALL_FAQ_URL, message, sizeof(message));
					ShellExecute(NULL, _T("open"), message, NULL, NULL, SW_SHOWNORMAL);
					return FALSE;
			}
			break;
		}
	}

	return FALSE;		
} 


// writes the out of game options to the registry
void __cdecl SaveOutOfGameRegistryOptionValues(void)
{
#ifndef AGENT
	cJSON* globals = WriteGlobalJSONOptionValues();
	if (globals)
	{
		WriteJSONFile(globals, "globals.json");
		cJSON_Delete(globals);
		return;
	}
#endif
}



