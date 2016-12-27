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
//#include "RogerWilco.h"





/////////////////////////////////////////////////////////////////
// Global Variables

extern options_t options;
extern bool show_training_messages;
extern HINSTANCE hInstance;
extern cDDraw* cDD;
extern pmare_t pmare_info[3];

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
	BOOL tcp_checked = FALSE;

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

			Button_SetCheck(GetDlgItem(hDlg, IDC_SOUND),  options.sound);

			Button_SetCheck(GetDlgItem(hDlg, IDC_EXTRA_SCROLL),  options.extra_scroll);
//			Button_SetCheck(GetDlgItem(hDlg, IDC_BIND_LOCAL),  options.bind_local);
			Button_SetCheck(GetDlgItem(hDlg, IDC_TCP_ONLY),  options.tcp_only);
			_stprintf(message, _T("%d"), options.bind_local_tcp);
			Edit_SetText(GetDlgItem(hDlg, IDC_BIND_TCP), message);
			if( options.tcp_only )
			{
				EnableWindow( GetDlgItem( hDlg, IDC_BIND_UDP ), FALSE );
			}
			_stprintf(message, _T("%d"), options.bind_local_udp);
			Edit_SetText(GetDlgItem(hDlg, IDC_BIND_UDP), message);

			if (options.resolution == 640)
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES640), 1);
			else if (options.resolution == 800)
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES800), 1);
			else if (options.resolution == 1024)
				Button_SetCheck(GetDlgItem(hDlg, IDC_GRAPHIC_RES1024), 1);

			Button_SetCheck(GetDlgItem(hDlg, IDC_RESTART_LOCATION),  options.restart_last_location);

			ShowWindow(GetDlgItem(hDlg, IDC_ENABLE_RW), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_ROGERWILCO), SW_HIDE);

//#ifdef UL_DEV
//			ShowWindow(GetDlgItem(hDlg, IDC_GRAPHIC_RES640), SW_SHOW);
//			ShowWindow(GetDlgItem(hDlg, IDC_GRAPHIC_RES800), SW_SHOW);
//			ShowWindow(GetDlgItem(hDlg, IDC_GRAPHIC_RES1024), SW_SHOW);
//#else
//			ShowWindow(GetDlgItem(hDlg, IDC_GRAPHIC_RES640), SW_HIDE);
//			ShowWindow(GetDlgItem(hDlg, IDC_GRAPHIC_RES800), SW_HIDE);
//			ShowWindow(GetDlgItem(hDlg, IDC_GRAPHIC_RES1024), SW_HIDE);
//#endif

#ifdef CHINESE
			ShowWindow(GetDlgItem(hDlg, IDC_RESTART_LOCATION), SW_SHOW);
#else
			ShowWindow(GetDlgItem(hDlg, IDC_RESTART_LOCATION), SW_HIDE);
#endif

#ifndef UL_DEBUG
			EnableWindow(GetDlgItem(hDlg, IDC_ROGERWILCO), false);
			EnableWindow(GetDlgItem(hDlg, IDC_ENABLE_RW), false);
#else
			Button_SetCheck(GetDlgItem(hDlg, IDC_ENABLE_RW),  options.rw);			Button_SetCheck(GetDlgItem(hDlg, IDC_SOUND),  options.sound);
#endif
			Button_SetCheck(GetDlgItem(hDlg, IDC_EXCLUSIVE), options.exclusive);
#ifdef PMARE
			Button_SetCheck(GetDlgItem(hDlg, IDC_TRAINING), false);
			ShowWindow(GetDlgItem(hDlg, IDC_TRAINING), SW_HIDE);
#else
			Button_SetCheck(GetDlgItem(hDlg, IDC_TRAINING), options.welcome_ai);
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


#ifdef PMARE
			ShowWindow(GetDlgItem(hDlg, IDC_URL), SW_HIDE);
			LoadString (hInstance, IDS_PMARE_BOGROM, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_PMARE_LIST), message);
			LoadString (hInstance, IDS_PMARE_AGOKNIGHT, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_PMARE_LIST), message);
			LoadString (hInstance, IDS_PMARE_SHAMBLIX, message, sizeof(message));
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
				LoadString (hInstance, IDS_PMARE_RESUME, message, sizeof(message));
				float price = (float)options.pmare_price;
				price = price/100;
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

			SendMessage(GetDlgItem(hDlg, IDC_AGREEMENT_TEXT), WM_SETTEXT, 0, (LPARAM) huge_text); 
			ShowWindow(GetDlgItem(hDlg, IDC_PASSWORD), SW_SHOW);

			// now set username/password based on current combo box selection
			for (int i = 0; i < MAX_STORED_ACCOUNTS; i++)	{
				ComboBox_AddString(GetDlgItem(hDlg, IDC_USERNAME), options.username[i]);
			}
		
			SetWindowText( GetDlgItem(hDlg, IDC_PASSWORD), options.password[options.account_index]);
			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_USERNAME), options.account_index);

			//SetWindowText( GetDlgItem(hDlg, IDC_USERNAME), options.username[options.account_index]);
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
			
			case IDC_TCP_ONLY:
				tcp_checked = Button_GetCheck( GetDlgItem( hDlg, IDC_TCP_ONLY ) );
				EnableWindow( GetDlgItem( hDlg, IDC_BIND_UDP ), !tcp_checked );
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
				options.exclusive		= Button_GetCheck(GetDlgItem(hDlg, IDC_EXCLUSIVE)); 
				options.welcome_ai		= Button_GetCheck(GetDlgItem(hDlg, IDC_TRAINING)); 
				options.extra_scroll	= Button_GetCheck(GetDlgItem(hDlg, IDC_EXTRA_SCROLL)); 
				options.tcp_only		= Button_GetCheck(GetDlgItem(hDlg, IDC_TCP_ONLY)); 
				Edit_GetText(GetDlgItem(hDlg, IDC_BIND_TCP), message, sizeof(message)); 								
				options.bind_local_tcp	= _ttol(message);
				Edit_GetText(GetDlgItem(hDlg, IDC_BIND_UDP), message, sizeof(message)); 								
				if(!options.tcp_only)
					options.bind_local_udp	= _ttol(message);
				else
					options.bind_local_udp = DEFAULT_UDP_PORT;

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
				// if pmare_avatar_type > Shamblix, it means resume previous session
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

				RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(true), 0, NULL, 0, 
								KEY_ALL_ACCESS, NULL, &reg_key, &result);
				SaveOutOfGameRegistryOptionValues(reg_key);
				RegCloseKey(reg_key);

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
					SetWindowText( GetDlgItem(hDlg, IDC_PASSWORD), options.password[options.account_index]);
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
void __cdecl SaveOutOfGameRegistryOptionValues(HKEY reg_key)
{
	RegSetValueEx(reg_key, _T("welcome_ai"), 0, REG_DWORD,  
		(unsigned char *)&(options.welcome_ai), sizeof(options.welcome_ai));
	RegSetValueEx(reg_key, _T("account_index"), 0, REG_DWORD, 
		(unsigned char *)&(options.account_index), sizeof(options.account_index));
	TCHAR buffer[64];
	for (int i=0; i<MAX_STORED_ACCOUNTS; i++) {
		_stprintf(buffer, _T("username_%d"), i);
		RegSetValueEx(reg_key, buffer, 0, REG_SZ, 
			(unsigned char *)options.username[i], Lyra::PLAYERNAME_MAX);
		_stprintf(buffer, _T("password_%d"), i);
		RegSetValueEx(reg_key, buffer, 0, REG_SZ, 
			(unsigned char *)options.password[i], Lyra::PASSWORD_MAX);
	}
	RegSetValueEx(reg_key, _T("sound"), 0, REG_DWORD,  
		(unsigned char *)&(options.sound), sizeof(options.sound));
	RegSetValueEx(reg_key, _T("extra_scroll"), 0, REG_DWORD,  
		(unsigned char *)&(options.extra_scroll), sizeof(options.extra_scroll));
	RegSetValueEx(reg_key, _T("tcp_only"), 0, REG_DWORD,  
		(unsigned char *)&(options.tcp_only), sizeof(options.tcp_only));
	RegSetValueEx(reg_key, _T("bind_local_tcp"), 0, REG_DWORD,  
		(unsigned char *)&(options.bind_local_tcp), sizeof(options.bind_local_tcp));
	RegSetValueEx(reg_key, _T("bind_local_udp"), 0, REG_DWORD,  
		(unsigned char *)&(options.bind_local_udp), sizeof(options.bind_local_udp));
	RegSetValueEx(reg_key, _T("restart_last_location"), 0, REG_DWORD,  
		(unsigned char *)&(options.restart_last_location), sizeof(options.restart_last_location));
	RegSetValueEx(reg_key, _T("resolution"), 0, REG_DWORD,  
		(unsigned char *)&(options.resolution), sizeof(options.resolution));
#ifdef UL_DEV
	RegSetValueEx(reg_key, _T("dev_server"), 0, REG_DWORD,
		(unsigned char *)&(options.dev_server), sizeof(options.dev_server));
	RegSetValueEx(reg_key, _T("custom_server_ip"), 0, REG_SZ,
		(unsigned char *) &(options.custom_ip), sizeof(options.custom_ip));
#endif
	RegSetValueEx(reg_key, _T("rogerwilco"), 0, REG_DWORD,  
		(unsigned char *)&(options.rw), sizeof(options.rw));
	RegSetValueEx(reg_key, _T("network"), 0, REG_DWORD,  
		(unsigned char *)&(options.network), sizeof(options.network));
	RegSetValueEx(reg_key, _T("debug"), 0, REG_DWORD,  
		(unsigned char *)&(options.debug), sizeof(options.debug));
	RegSetValueEx(reg_key, _T("gameserver"), 0, REG_SZ,  //ROUND_ROBIN 
		(unsigned char *)options.game_server, sizeof(options.game_server));
	RegSetValueEx(reg_key, _T("exclusive"), 0, REG_DWORD,  
		(unsigned char *)&(options.exclusive), sizeof(options.exclusive));
	RegSetValueEx(reg_key, _T("pmare_session_start"), 0, REG_BINARY,  
		(unsigned char *)&(options.pmare_session_start), sizeof(options.pmare_session_start));

}

void LoadOutOfGameRegistryOptionValues(HKEY reg_key, bool force)
{
	DWORD keyresult, size, reg_type;

	size = sizeof(options.account_index);
	keyresult = RegQueryValueEx(reg_key, _T("account_index"), NULL, &reg_type,
		(unsigned char *)&(options.account_index), &size);
	if ((keyresult != ERROR_SUCCESS) || force ||
		(options.account_index >= MAX_STORED_ACCOUNTS))
		options.account_index = 0;

	// HACK: read in username/password from single storage system
	// and use as defaults if username_0 isn't present

	TCHAR legacy_username[Lyra::PLAYERNAME_MAX];
	TCHAR legacy_password[Lyra::PASSWORD_MAX];
	size = Lyra::PLAYERNAME_MAX;
	keyresult = RegQueryValueEx(reg_key, _T("username"), NULL, &reg_type,
		(unsigned char *)legacy_username, &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		_tcscpy(legacy_username, _T(""));

	size = Lyra::PASSWORD_MAX;
	keyresult = RegQueryValueEx(reg_key, _T("password"), NULL, &reg_type,
		(unsigned char *)legacy_password, &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		_tcscpy(legacy_password, _T(""));

	// now read the list of stored usernames and passwords

	TCHAR buffer[64];
	for (int i=0; i<MAX_STORED_ACCOUNTS; i++) {
		_stprintf(buffer, _T("username_%d"), i);
		size = Lyra::PLAYERNAME_MAX;
		keyresult = RegQueryValueEx(reg_key, buffer, NULL, &reg_type,
			(unsigned char *)options.username[i], &size);

		if ((keyresult != ERROR_SUCCESS) || force) {
			if ((0 == i) && (keyresult != ERROR_SUCCESS)) // copy across legacy value
				_tcscpy(options.username[i], legacy_username);
			else
				_tcscpy(options.username[i], _T(""));
		}

		_stprintf(buffer, _T("password_%d"), i);
		size = Lyra::PASSWORD_MAX;
		keyresult = RegQueryValueEx(reg_key, buffer, NULL, &reg_type,
			(unsigned char *)options.password[i], &size);

		if ((keyresult != ERROR_SUCCESS) || force) {
			if ((0 == i) && (keyresult != ERROR_SUCCESS)) // copy across legacy value
				_tcscpy(options.password[i], legacy_password);
			else
				_tcscpy(options.password[i], _T(""));
		}
	}

	size = sizeof(options.rw);
	keyresult = RegQueryValueEx(reg_key, _T("rogerwilco"), NULL, &reg_type,
		(unsigned char *)&(options.rw), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.rw = TRUE;

	size = sizeof(options.sound);
	keyresult = RegQueryValueEx(reg_key, _T("sound"), NULL, &reg_type,
		(unsigned char *)&(options.sound), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.sound = TRUE;

	size = sizeof(options.extra_scroll);
	keyresult = RegQueryValueEx(reg_key, _T("extra_scroll"), NULL, &reg_type,
		(unsigned char *)&(options.extra_scroll), &size);

	size = sizeof(options.tcp_only);
	keyresult = RegQueryValueEx(reg_key, _T("tcp_only"), NULL, &reg_type,
		(unsigned char *)&(options.tcp_only), &size);
	if ((keyresult != ERROR_SUCCESS) || force) 
		options.tcp_only = TRUE;


	size = sizeof(options.bind_local_tcp);
	keyresult = RegQueryValueEx(reg_key, _T("bind_local_tcp"), NULL, &reg_type,
		(unsigned char *)&(options.bind_local_tcp), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.bind_local_tcp = 0;

	size = sizeof(options.bind_local_udp);
	keyresult = RegQueryValueEx(reg_key, _T("bind_local_udp"), NULL, &reg_type,
		(unsigned char *)&(options.bind_local_udp), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.bind_local_udp = DEFAULT_UDP_PORT;


#ifdef CHINESE
	size = sizeof(options.restart_last_location);
	keyresult = RegQueryValueEx(reg_key, _T("restart_last_location"), NULL, &reg_type,
		(unsigned char *)&(options.restart_last_location), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.restart_last_location = FALSE;
#else
		options.restart_last_location = FALSE;
#endif

	size = sizeof(options.resolution);
	keyresult = RegQueryValueEx(reg_key, _T("resolution"), NULL, &reg_type,
		(unsigned char *)&(options.resolution), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.resolution = 640;

#ifdef UL_DEV
	size = sizeof(options.dev_server);
	keyresult = RegQueryValueEx(reg_key, _T("dev_server"), NULL, &reg_type,
		(unsigned char*) &(options.dev_server), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.dev_server = 1;	

	size = sizeof(options.custom_ip);
	keyresult = RegQueryValueEx(reg_key, _T("custom_server_ip"), NULL, &reg_type,
		(unsigned char *)options.custom_ip, &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		LoadString(hInstance, IDS_CUSTOM_IP_DEFAULT, options.custom_ip, sizeof(options.custom_ip));
#endif

	size = sizeof(options.network);
	keyresult = RegQueryValueEx(reg_key, _T("network"), NULL, &reg_type,
		(unsigned char *)&(options.network), &size);
#ifndef AGENT
#if defined (UL_DEBUG) || defined (GAMEMASTER)
	if ((keyresult != ERROR_SUCCESS) || force)
#endif // non-gm's and agents MUST run networked
#endif
		options.network = INTERNET;

	size = sizeof(options.debug);
	keyresult = RegQueryValueEx(reg_key, _T("debug"), NULL, &reg_type,
		(unsigned char *)&(options.debug), &size);
#if defined (UL_DEBUG)
	if ((keyresult != ERROR_SUCCESS) || force)
#endif // non-debug builds can't goto debug server
		options.debug = FALSE;

#ifdef PMARE_BETA // beta builds goto debug server so shutdowns don't stop the whole game
	options.debug = TRUE;
#endif

//ROUND_ROBIN
	size = sizeof(options.game_server);
	keyresult = RegQueryValueEx(reg_key, _T("gameserver"), NULL, &reg_type,
		(unsigned char *)options.game_server, &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		LoadString(hInstance, IDS_LIVE_GAME_SERVER_IP, options.game_server, sizeof(options.game_server)) ;

	size = sizeof(options.exclusive);
	keyresult = RegQueryValueEx(reg_key, _T("exclusive"), NULL, &reg_type,
		(unsigned char *)&(options.exclusive), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.exclusive = TRUE;

	size = sizeof(options.welcome_ai);
	keyresult = RegQueryValueEx(reg_key, _T("welcome_ai"), NULL, &reg_type,
		(unsigned char *)&(options.welcome_ai), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.welcome_ai = TRUE;

#ifdef AGENT
	options.welcome_ai = FALSE;
#endif

	show_training_messages = options.welcome_ai;

	// save them back, in case a default was set
	SaveOutOfGameRegistryOptionValues(reg_key); 
}


