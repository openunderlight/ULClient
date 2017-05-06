
// Dialogs: dialog box functions

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include <limits.h>
#include "Utils.h"
#include "Resource.h"
#include "cGameServer.h"
#include "cControlPanel.h"
#include "LyraDefs.h"
#include "cChat.h"
#include "cParty.h"
#include "cDSound.h"
#include "Mouse.h"
#include "Options.h"
#include "cAgentServer.h"
#include "SharedConstants.h"
#include "cActorList.h"
#include "cPlayer.h"
#include "cDDraw.h"
#include "cPostQuest.h"
#include "Realm.h"
#include "Dialogs.h"
#include "LmItem.h"
#include "LmItemDefs.h"
#include "LmItemHdr.h"
#include "cOutput.h"
#include "cKeymap.h"
#include "Mouse.h"
#include "cQuestBuilder.h"
#include "Move.h"
#include "Interface.h"
#include "cArts.h"
#include "Realm.h"
#include "cLevel.h"
#include "cEffects.h"

/////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern cAgentServer *as;
extern HINSTANCE hInstance;
extern cActorList *actors;
extern cChat *display;
extern cControlPanel *cp;
extern cPlayer *player;
extern cDDraw *cDD;
extern cDSound *cDS;
extern cArts *arts;
extern cEffects *effects;
extern cLevel *level;
extern cPostQuest *postquest;
extern cQuestBuilder *quests;
extern HWND hwnd_acceptreject;

extern bool talkdlg;
extern bool pmare_talkdlg;
extern bool helpdlg;
extern bool creditsdlg;
extern bool optiondlg;
extern bool metadlg;
extern bool itemdlg;
extern bool gm_itemhelpdlg;
extern bool quest_helpdlg;
extern bool gmteleportdlg;
extern bool player_itemhelpdlg;
extern bool quest_itemhelpdlg;
extern bool keyboarddlg;
extern bool exiting;
extern bool chooseguilddlg;
extern bool entervaluedlg;
extern bool locateavatardlg;
extern bool agentdlg;
extern bool createplayerdlg;
extern bool avatardlg;
extern bool avatarmfdlg;
extern bool warningyesnodlg;
extern bool acceptrejectdlg;
extern bool writescrolldlg;
extern bool ignorelistdlg;
extern int nonfataldlg;
extern bool useppointdlg;
extern bool grantppointdlg;
extern bool ppoint_helpdlg;


extern art_dlg_callback_t acceptreject_callback;
extern art_dlg_callback_t entervalue_callback;
extern art_dlg_callback_t chooseguild_callback;
extern art_dlg_callback_t grantpp_callback;

extern bool leveleditor;
extern options_t options;
extern mouse_look_t mouse_look;
extern mouse_move_t mouse_move;
extern ppoint_t pp; // personality points use tracker
extern cKeymap *keymap;
extern cNeighbor *test_avatar;

monster_breed_t monster_breeds[NUM_MONSTER_BREEDS] = 
{
	{IDS_ONE,IDS_BREED_DESC_ONE,0,1},
	{IDS_TWO,IDS_BREED_DESC_TWO,2,3},
	{IDS_THREE,IDS_BREED_DESC_THREE,4,5},
	{IDS_FOUR,IDS_BREED_DESC_FOUR,6,7}
};

const TCHAR pmare_separator[2] = _T("*");


/////////////////////////////////////////////////
// Local Global Variables

static HWND hwnd_locateavatar = NULL;
static HWND hwnd_ignorelist = NULL;
static HWND hwnd_avatar = NULL;

static char windowsVersion[50];

//////////////////////////////////////////////////////////////////
// Constants

const int KEYMAP_FUNCS_PER_COLUMN = 6;
const int MAX_REPLY_DELAY = 20000; // auto-reject after 20 seconds of query

/////////////////////////////////////////////////
// Functions

HWND TopMost(void)
{
	return HWND_TOPMOST;
}

void getWindowsVersion(char * ver) {
	DWORD dwVersion = 0;
	DWORD dwMajorVersion = 0;
	DWORD dwMinorVersion = 0;
	DWORD dwBuild = 0;

	dwVersion = GetVersion();

	// Get the Windows version

	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// Get the build number

	if (dwVersion < 0x80000000)
		dwBuild = (DWORD)(HIWORD(dwVersion));

	sprintf(ver, "Platform: %d.%d (%d)", dwMajorVersion, dwMinorVersion, dwBuild);

	return;
}

void CALLBACK AcceptRejectTimerCallback (HWND hWindow, UINT uMSG, UINT idEvent, DWORD dwTime)
{	// auto-reject on timeout
	if (hwnd_acceptreject)
		SendMessage(hwnd_acceptreject, WM_COMMAND, (WPARAM)IDC_REJECT, 0);

	return;
}

BOOL CALLBACK AcceptRejectDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static int timer_id;
	static bool art_callback;
	static bool made_choice;
	static dlg_callback_t callback;
	static cNeighbor *n;

	int i;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			// make sure there is a response
			if (actors->ValidNeighbor(n))
				n->SetHwnd_AR(NULL);
			if (!made_choice)
			{
				i = 0;
				if (art_callback)
					(arts->*(acceptreject_callback))(&i);
				else if (callback)
					callback(&i);
			}
			acceptrejectdlg = false;
			KillTimer( cDD->Hwnd_Main(), ACCEPTREJECT_TIMER );
			timer_id = 0;
			hwnd_acceptreject = NULL;
			break;

		case WM_INITDIALOG:
			n = NO_ACTOR;
			callback = NULL;
			art_callback = false;
			acceptrejectdlg = true;
			made_choice = false;
			hwnd_acceptreject = hDlg;
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			SetWindowText(GetDlgItem(hDlg, IDC_ACCEPTREJECT_TEXT), message);
			// set up timer for timeout
			if (!(timer_id = SetTimer(cDD->Hwnd_Main(), ACCEPTREJECT_TIMER, MAX_REPLY_DELAY,
				(TIMERPROC)AcceptRejectTimerCallback)))
				WINDOWS_ERROR();

			return TRUE;

		case WM_SET_ART_CALLBACK: // called by art waiting for callback
			art_callback = true;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_REJECT, 0);
#endif
			return TRUE;

		case WM_SET_CALLBACK: // called by art waiting for callback
			callback = (dlg_callback_t)lParam;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_REJECT, 0);
#endif
			return TRUE;

		case WM_SET_AR_NEIGHBOR:
			n = (cNeighbor*)lParam;
			if (actors->ValidNeighbor(n))
				n->SetHwnd_AR(hDlg);
			else
				n = NO_ACTOR;
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_ACCEPT, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_REJECT, 0);
					return TRUE;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_ACCEPT:
				made_choice = true;
				i = 1; // accept = 1
				if (art_callback)
					(arts->*(acceptreject_callback))(&i);
				else if (callback)
					callback(&i);
				DestroyWindow(hDlg);
				return FALSE;
			case IDC_REJECT:
				made_choice = true;
				cDS->PlaySound(LyraSound::REJECTED);
				i = 0; // reject = 0
				if (art_callback)
					(arts->*(acceptreject_callback))(&i);
				else if (callback)
					callback(&i);
				DestroyWindow(hDlg);
				return FALSE;
			}
			return TRUE;
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

// Functions to read/write a macro to/from the registry
void RegistryReadMacro(int macro_number, macro_t macro)
{

	HKEY	reg_key;
	DWORD result;
	DWORD reg_type,size;
	TCHAR	macro_name[20];

	*macro = '\0';
	if ( macro_number < 0 || macro_number > MAX_MACROS)
		return;

	_stprintf(macro_name,_T("macro_%d"),macro_number);

	RegOpenKeyEx(HKEY_CURRENT_USER, RegPlayerKey(), 0, KEY_ALL_ACCESS, &reg_key );
	size = sizeof macro_t;
	result = RegQueryValueEx(reg_key, macro_name, NULL, &reg_type,
								(unsigned char *)macro, &size);
	RegCloseKey(reg_key);

}

static void RegistryWriteMacro(int macro_number, macro_t macro)
{

	HKEY	reg_key;
	DWORD result;
	TCHAR	macro_name[20];

	if (player->CanUseChatMacros() == false && macro_number < 0 || macro_number > MAX_MACROS)
		return;

	_stprintf(macro_name,_T("macro_%d"),macro_number);

	RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &reg_key, &result);
	RegSetValueEx(reg_key, macro_name, 0, REG_BINARY, (unsigned char *)macro, sizeof macro_t);
	RegCloseKey(reg_key);

}

BOOL CALLBACK ChatMacrosDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
static macro_t macros[MAX_MACROS];
static int current_macro =0;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			break;


		case WM_INITDIALOG:
		{
			SetWindowPos(hDlg, HWND_TOPMOST, cDD->DlgPosX(hDlg)+15, cDD->ViewY()+2, 0, 0, SWP_NOSIZE);

			for (int i=0;i < MAX_MACROS; i++)
			{
				TCHAR str[16];
				LoadString (hInstance, IDS_ALT, disp_message, sizeof(disp_message));

				_stprintf(str, disp_message, i);
				ListBox_AddString(GetDlgItem(hDlg, IDC_MACRO_NUMBERS), str);

				RegistryReadMacro(i,macros[i]);
			}
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_MACRO_NUMBERS),current_macro);
			SetWindowText(GetDlgItem(hDlg, IDC_SPEECH), macros[current_macro]);
			Edit_LimitText(GetDlgItem(hDlg, IDC_SPEECH), (Lyra::MAX_SPEECHLEN/2)-Lyra::PLAYERNAME_MAX-3);
			return TRUE;
		}

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_CAPTURECHANGED:
			return 0;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_MACRO_NUMBERS:
					GetWindowText(GetDlgItem(hDlg, IDC_SPEECH), macros[current_macro], Lyra::MAX_SPEECHLEN-Lyra::PLAYERNAME_MAX);
					current_macro = ListBox_GetCurSel(GetDlgItem(hDlg,IDC_MACRO_NUMBERS));
					SetWindowText(GetDlgItem(hDlg, IDC_SPEECH), macros[current_macro]);
				break;

				case IDC_OK:
					{
					current_macro = ListBox_GetCurSel(GetDlgItem(hDlg,IDC_MACRO_NUMBERS));
					GetWindowText(GetDlgItem(hDlg, IDC_SPEECH), macros[current_macro], Lyra::MAX_SPEECHLEN-Lyra::PLAYERNAME_MAX);
					for (int i = 0; i < MAX_MACROS; i++)
						if (_tcslen(macros[i])>0)
							RegistryWriteMacro(i,macros[i]);
					}

				case IDC_CANCEL:
					DestroyWindow(hDlg);
				return FALSE;
				break;
			}
		}
		default:
			break;
	}
	return FALSE;
}


// Dialog Box Procedure for Talking
BOOL CALLBACK TalkDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int target;
	cNeighbor *n;
	static HWND hwnd_talk, hwnd_speech;
	static WNDPROC  lpfn_speech;
	// ToDo: make LList
	const int MAX_SENTENCE = 10;
	static TCHAR sentence_buffers[MAX_SENTENCE][(Lyra::MAX_SPEECHLEN / 2)];
	static int cur_sentence = 0;	// increment/wrap at exit
	static TCHAR* sentence = (TCHAR*)sentence_buffers;
	static realmid_t neighborid[64]; // store neighbor id's for whisper
	static bool stripdot = false; // strip trailing dot?
	static bool stripret = false; // strip trailing return?
	static HFONT hEditFont;

	static int init_buffer=1;
	if (init_buffer)
	{
		memset(sentence_buffers,0,sizeof(sentence_buffers));
		init_buffer = 0;
	}


	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			talkdlg = false;
			if(hDlg == hwnd_talk)
				DeleteObject(hEditFont);
			break;

		case WM_STRIP_DOT:
			stripdot = true;
			return TRUE;

		case WM_STRIP_RETURN:
			stripret = true;
			return TRUE;

		case WM_PRIOR_LINE:	// roll backward talk buffer
		{
			// ToDo : roll only to non-empty slots
			cur_sentence--;
			if (cur_sentence < 0)
				cur_sentence = MAX_SENTENCE-1;
			sentence = sentence_buffers[cur_sentence];
			Edit_SetText(GetDlgItem(hDlg, IDC_SPEECH), sentence);
			Edit_SetSel(GetDlgItem(hDlg, IDC_SPEECH), 0, -1); // tacky, but this positions
			Edit_SetSel(GetDlgItem(hDlg, IDC_SPEECH), -1, -1);// position the cursor to beginning
			Edit_ScrollCaret(GetDlgItem(hDlg, IDC_SPEECH));
			return TRUE;
		}

		case WM_NEXT_LINE:	// roll forward talk buffer
		{
			// ToDo : roll only to non-empty slots
			cur_sentence++;
			if (cur_sentence >= MAX_SENTENCE)
				cur_sentence = 0;
			sentence = sentence_buffers[cur_sentence];
			Edit_SetText(GetDlgItem(hDlg, IDC_SPEECH), sentence);
			Edit_SetSel(GetDlgItem(hDlg, IDC_SPEECH), 0, -1); // tacky, but this positions
			Edit_SetSel(GetDlgItem(hDlg, IDC_SPEECH), -1, -1);// position the cursor to end
			Edit_ScrollCaret(GetDlgItem(hDlg, IDC_SPEECH));
			return TRUE;
		}

		case WM_INITDIALOG:
		{
			talkdlg = true;
			stripdot = stripret = false;
			sentence = sentence_buffers[cur_sentence];
			memset(sentence,0,sizeof(sentence_buffers[cur_sentence]));
			hwnd_talk = hDlg;
			hwnd_speech = GetDlgItem(hDlg, IDC_SPEECH);

			Edit_LimitText(hwnd_speech, (Lyra::MAX_SPEECHLEN/2)-Lyra::PLAYERNAME_MAX-3);

			// no bug/cheat reports in training b/c no gs login
			if (options.welcome_ai)
			{
				ShowWindow(GetDlgItem(hDlg, IDC_SPECIAL_TALK), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_SPECIAL_TALKLIST), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_REPORT), SW_HIDE);
			}
#ifdef GAMEMASTER
			ShowWindow(GetDlgItem(hDlg,IDC_RAW_EMOTE), SW_SHOW);

			//#else
//			if (level->ID() == 20) // no whispers in Thresh
//				ShowWindow(GetDlgItem(hDlg, IDC_WHISPER), SW_HIDE);
#endif

			// Add neighbors names for whispers
			target=0;
			for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
			{ // create an array of neighbor id's for retrieval at whisper time
				if (n->Avatar().Hidden()) // don't display invisible GM's
					continue; 
				ListBox_AddString(GetDlgItem(hDlg, IDC_NEIGHBORS), n->Name());
				neighborid[target] = n->ID();
				if (cp->SelectedNeighbor() == n)
					ListBox_SetCurSel(GetDlgItem(hDlg, IDC_NEIGHBORS), target);
				target++;
				//_tprintf("adding neighbor %s to box...\n",n->GetName());
			}
			actors->IterateNeighbors(DONE);

			// Add bug/cheat/rp report entries to special talk listbox
			LoadString (hInstance, IDS_BUGREPORT, message, sizeof(message));
			ListBox_AddString(GetDlgItem(hDlg, IDC_SPECIAL_TALKLIST), message);
			LoadString (hInstance, IDS_CHEATREPORT, message, sizeof(message));
			ListBox_AddString(GetDlgItem(hDlg, IDC_SPECIAL_TALKLIST), message);
			LoadString (hInstance, IDS_RPREPORT, message, sizeof(message));
			ListBox_AddString(GetDlgItem(hDlg, IDC_SPECIAL_TALKLIST), message);
			// default to RP report
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_SPECIAL_TALKLIST), 2);


			// Subclass the edit control so we get the char keystrokes
			lpfn_speech = SubclassWindow(hwnd_speech, TalkEditWProc);
			SendMessage(hwnd_speech, WM_PASSPROC, 0, (LPARAM) lpfn_speech );
			SendMessage(hwnd_speech, WM_PARENT, 0, (LPARAM) hwnd_talk );

			Button_SetCheck(GetDlgItem(hDlg, IDC_TALK), 1);

			SetFocus(hwnd_speech);
			SetWindowPos(hDlg, HWND_TOPMOST, cDD->DlgPosX(hDlg)+15, cDD->ViewY()+2, 0, 0, SWP_NOSIZE);

			// Create Font for edit control
			{
				const TCHAR CHAT_FONT_NAME[16]=_T("Arial");
				LOGFONT logFont;
				memset(&logFont, 0, sizeof(LOGFONT));

				logFont.lfHeight = 15;
				logFont.lfWidth = 0;
				logFont.lfWeight = 500;
				logFont.lfEscapement = 0;
				logFont.lfItalic = 0;
				logFont.lfUnderline = 0;
				logFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
				logFont.lfClipPrecision = CLIP_STROKE_PRECIS;
				logFont.lfQuality = DEFAULT_QUALITY;
				_tcscpy(logFont.lfFaceName, CHAT_FONT_NAME);

				// set the control to use this font
				hEditFont = CreateFontIndirect(&logFont);
				SendMessage(hwnd_speech, WM_SETFONT, WPARAM(hEditFont), 0);
			}

			return TRUE;
		}

		case WM_PAINT:
			if ((hDlg == hwnd_talk) && TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_CAPTURECHANGED:
			SetFocus(hwnd_speech);
			return 0;

		case WM_MOUSEMOVE:
			if (mouse_look.looking || mouse_move.moving)
			{
				StopMouseMove();
				StopMouseLook();
			}
			break;

		case WM_COMMAND:
		{
			int message_length, i, j;
			if ((HIWORD(wParam) == LBN_SELCHANGE) ||
				(HIWORD(wParam) == LBN_SELCANCEL))
			{
				if (LOWORD(wParam) == IDC_NEIGHBORS)
				{
					if (ListBox_GetCurSel(GetDlgItem(hDlg, IDC_NEIGHBORS)) > -1)
					{
						Button_SetCheck(GetDlgItem(hDlg, IDC_WHISPER), 1);
						Button_SetCheck(GetDlgItem(hDlg, IDC_TALK), 0);
						Button_SetCheck(GetDlgItem(hDlg, IDC_SHOUT), 0);
						Button_SetCheck(GetDlgItem(hDlg, IDC_EMOTE), 0);
						Button_SetCheck(GetDlgItem(hDlg, IDC_SPECIAL_TALK), 0);
					}
				} 
				else if (LOWORD(wParam) == IDC_SPECIAL_TALKLIST) 
				{
					if (ListBox_GetCurSel(GetDlgItem(hDlg, IDC_SPECIAL_TALKLIST)) > -1)
					{
						Button_SetCheck(GetDlgItem(hDlg, IDC_WHISPER), 0);
						Button_SetCheck(GetDlgItem(hDlg, IDC_TALK), 0);
						Button_SetCheck(GetDlgItem(hDlg, IDC_SHOUT), 0);
						Button_SetCheck(GetDlgItem(hDlg, IDC_EMOTE), 0);
						Button_SetCheck(GetDlgItem(hDlg, IDC_SPECIAL_TALK), 1);
					}					
				}
				SetFocus(hwnd_speech);	// reset focus

			}
			else
			{
				int num = LOWORD(wParam);

				switch (num)//(LOWORD(wParam))
			{
			case IDC_SPEAK:

				GetWindowText(GetDlgItem(hDlg, IDC_SPEECH), sentence, Lyra::MAX_SPEECHLEN-Lyra::PLAYERNAME_MAX-2);
				message_length = _tcslen(sentence);

				if (stripdot) // strip off dot at the end
				{
					// The termination case is probably at the end...
					if ((sentence[message_length-5] == VK_RETURN) && (sentence[message_length-3] == '.'))
					{
						sentence[message_length-5] = '\0';
						message_length -= 5;
					}
					else // but if it's not, the user backed into their message text...
						for (i = 0; i < message_length; i++)
							if ((sentence[i] == VK_RETURN) && (sentence[i+2] == '.')
								&& (sentence[i+3] == VK_RETURN))
							{
								sentence[i] = '\0';
								message_length = i+1;
								break;
							}
				}
				else if (stripret) // strip off the return
				{
					// The return's probably at the end...
					if (sentence[message_length-2] == VK_RETURN)
					{
						sentence[message_length-2] = '\0';
						message_length -= 2;
					}
					else // but if it's not, the user backed into their message text...
						for (i = 0; i < message_length; i++)
							if (sentence[i] == VK_RETURN)
							{
								for (j = i; (sentence[j] != '\0') && (sentence[j+1] != '\0'); j++)
									sentence[j] = sentence[j+2];
								break;
							}
				}

				// strip off extra returns at the end of a message
				message_length = _tcslen(sentence);
				for (i = message_length-2; i > 0; i--)
					if (sentence[i] == VK_RETURN)
						sentence[i] = '\0';
					else
						break;

				// make sure player isn't faking an emote for someone else
				if (Button_GetCheck(GetDlgItem(hDlg, IDC_EMOTE)))
				{
					for (i = 0; i < message_length; i++)
						if (sentence[i] == '>' || sentence[i] == '›')
						{
							sentence[i] = '\0';
							break;
						}
				}

				// make sure player cannot emote as soulsphere by entering talk dlg and selecting emote
				if ((player->flags & ACTOR_SOULSPHERE) && Button_GetCheck(GetDlgItem(hDlg, IDC_EMOTE)))
				{
					LoadString (hInstance, IDS_SOULSPHERE_NO_EMOTE, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
						(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
					DestroyWindow(hDlg);
					return TRUE;
				}

				// talk action is legal
				if (options.network && gs && gs->LoggedIntoGame())
				{
					if (Button_GetCheck(GetDlgItem(hDlg, IDC_TALK)))
						gs->Talk(sentence,RMsg_Speech::SPEECH,Lyra::ID_UNKNOWN);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_SHOUT)))
						gs->Talk(sentence,RMsg_Speech::SHOUT,Lyra::ID_UNKNOWN, true);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_WHISPER)))
					{
						target = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_NEIGHBORS));
						if (target == -1) // whisper to self
							display->DisplaySpeech(sentence, player->Name(), RMsg_Speech::WHISPER);
						else
						{
							n = actors->LookUpNeighbor(neighborid[target]);
							if (n && n->CanWhisper())
							{
								gs->Talk(sentence,RMsg_Speech::WHISPER, neighborid[target], true);
								// 1 in 10 change whisper emote is shown to all
#ifndef GAMEMASTER // GM's don't sent out whisper emote
								int show_emote = rand()%10;
								if (0 == show_emote)
								{
									LoadString (hInstance, IDS_WHISPER_EMOTE, disp_message, sizeof(disp_message));
									_stprintf(message, disp_message, n->Name());
									gs->Talk(message, RMsg_Speech::WHISPER_EMOTE, neighborid[target], false);
								}
#endif // gamemaster.
							}
							else
							{
								LoadString (hInstance, IDS_NEIGHBOR_TOO_FAR, message, sizeof(message));
								display->DisplayMessage(message);
								SetActiveWindow(cDD->Hwnd_Main());
								SetFocus(cDD->Hwnd_Main());
								//CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								//	cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
								break;
							}
						}
					}
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_EMOTE)))
						gs->Talk(sentence,RMsg_Speech::EMOTE, Lyra::ID_UNKNOWN);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_RAW_EMOTE)))
						gs->Talk(sentence,RMsg_Speech::RAW_EMOTE, Lyra::ID_UNKNOWN);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_SPECIAL_TALK)))
					{
						target = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_SPECIAL_TALKLIST));
						if (target == 0) { // bug report
							getWindowsVersion(windowsVersion);
							strcat(sentence, "\n");
							strcat(sentence, windowsVersion);
							gs->Talk(sentence, RMsg_Speech::REPORT_BUG, Lyra::ID_UNKNOWN, true);
						}
						else if (target == 1) // cheat report
							gs->Talk(sentence,RMsg_Speech::REPORT_CHEAT ,Lyra::ID_UNKNOWN, true);
						else if (target == 2) // role playing report
							gs->Talk(sentence,RMsg_Speech::RP, Lyra::ID_UNKNOWN, true);				
					}
				} 
				else 
				{ // not logged in yet - either in training, or debugging
					if (Button_GetCheck(GetDlgItem(hDlg, IDC_TALK)))
						display->DisplaySpeech(sentence, player->Name(), RMsg_Speech::SPEECH, true);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_SHOUT)))
					{
						LoadString (hInstance, IDS_PLAYER_SHOUT, message, sizeof(message));
						display->DisplaySpeech(sentence, message, RMsg_Speech::SHOUT, true);
					}
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_WHISPER)))
						display->DisplaySpeech(sentence, player->Name(), RMsg_Speech::WHISPER, true);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_EMOTE)))
						display->DisplaySpeech(sentence, player->Name(), RMsg_Speech::EMOTE, true);
				}


				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
					(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
				DestroyWindow(hDlg);

				cur_sentence++;
				if (cur_sentence >= MAX_SENTENCE)
					cur_sentence = 0;

				return TRUE;

			case IDC_CANCEL:
				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
					(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
				DestroyWindow(hDlg);
				return FALSE;
			case IDC_MACROS:
				CreateLyraDialog(hInstance, IDD_CHAT_MACROS, 	cDD->Hwnd_Main(), (DLGPROC)ChatMacrosDlgProc);
				return TRUE;
				}
			}
			break;
		default:
			break;
	}
	}
	return FALSE;
}

// helper to count the number of phrases in canned pmare speech and emotes
// each canned pmare speech/emote is separated by a '*"
int NumPhrases(TCHAR* text)
{	
	TCHAR* remainder = text;
	TCHAR* next = remainder;
	int num_phrases = 0;
	while (1)
	{
		next = _tcsstr(remainder, _T("*"));
		if (next == NULL)
			break;
		num_phrases ++;
		next++;
		remainder = next;
	}

	return num_phrases;

}

// Dialog Box Procedure for PMare canned speech and emotes
BOOL CALLBACK PMareTalkDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwnd_pmare_talk, hwnd_pmare_speech, hwnd_pmare_emotes, hwnd_pmare_speech_maren;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			pmare_talkdlg = false;
			break;

		case WM_INITDIALOG:
		{
			pmare_talkdlg = true;
			hwnd_pmare_talk = hDlg;
			hwnd_pmare_emotes = GetDlgItem(hDlg, IDC_PMARE_EMOTES);
			hwnd_pmare_speech = GetDlgItem(hDlg, IDC_PMARE_SPEECH);
			hwnd_pmare_speech_maren = GetDlgItem(hDlg, IDC_PMARE_SPEECH_MAREN);

			TCHAR* pmare_emote;
			// all canned emotes are stored in the string table, separated
			// by '*'
			
			// currently there are two pmare emote strings
			int total_canned_emotes = 0;
			for (int i=0; i<2; i++)
			{
				if (i == 0)
					LoadString (hInstance, IDS_PMARE_EMOTES1, message, sizeof(message));
				else if (i == 1)
					LoadString (hInstance, IDS_PMARE_EMOTES2, message, sizeof(message));
				pmare_emote = (TCHAR*)message;
				int num_canned_emotes = NumPhrases(pmare_emote);
				total_canned_emotes += num_canned_emotes;

				for (int i=0; i<num_canned_emotes; i++)
				{
					TCHAR* next = _tcsstr(pmare_emote, _T("*"));
					_tcscpy(next, _T(""));
					ListBox_AddString(hwnd_pmare_emotes, pmare_emote);
					next++;
					pmare_emote = next;
				}
			}

			TCHAR* pmare_speech;
			TCHAR* pmare_speech_maren;
			// now we need to load both the english and maren versions
			// of the speech; the listbox containing maren is hidden
			LoadString (hInstance, IDS_PMARE_SPEECH_ENGLISH, message, sizeof(message));
			LoadString (hInstance, IDS_PMARE_SPEECH_MAREN, disp_message, sizeof(disp_message));
			pmare_speech = (TCHAR*)message;
			pmare_speech_maren = (TCHAR*)disp_message;
			int num_canned_speech = NumPhrases(pmare_speech);

			for (int i=0; i<num_canned_speech; i++)
			{   // first load in the English for display to the player
				TCHAR* next = _tcsstr(pmare_speech, pmare_separator);
				_tcscpy(next, _T(""));
				ListBox_AddString(hwnd_pmare_speech, pmare_speech);
				next++;
				pmare_speech = next;
				// now load in the Maren for display to others
				next = _tcsstr(pmare_speech_maren, pmare_separator);
				_tcscpy(next, _T(""));
				ListBox_AddString(hwnd_pmare_speech_maren, pmare_speech_maren);
				next++;
				pmare_speech_maren = next;

			}

			ListBox_SetCurSel(hwnd_pmare_emotes, (rand()%total_canned_emotes));
			ListBox_SetCurSel(hwnd_pmare_speech, (rand()%num_canned_speech));
						
			SetWindowPos(hDlg, HWND_TOPMOST, cDD->DlgPosX(hDlg), cDD->ViewY()+10, 0, 0, SWP_NOSIZE);

			return TRUE;
		}

		case WM_PAINT:
			if ((hDlg == hwnd_pmare_talk) && TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_MOUSEMOVE:
			if (mouse_look.looking || mouse_move.moving)
			{
				StopMouseMove();
				StopMouseLook();
			}
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					if (GetFocus() == GetDlgItem(hDlg, IDC_PMARE_EMOTES))
						PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_PMARE_EMOTE, 0);
					else
						PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_SPEAK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_COMMAND:
		{
			int num = LOWORD(wParam);

			switch (num)
			{
			case IDC_PMARE_EMOTE:
				// make sure player cannot emote as soulsphere by entering talk dlg and selecting emote
				if ((player->flags & ACTOR_SOULSPHERE) && Button_GetCheck(GetDlgItem(hDlg, IDC_PMARE_EMOTE)))
				{
					LoadString (hInstance, IDS_SOULSPHERE_NO_EMOTE, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
						(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
					DestroyWindow(hDlg);
					return TRUE;
				}
				// pmare emote action is legal
				ListBox_GetText(hwnd_pmare_emotes, ListBox_GetCurSel(hwnd_pmare_emotes), message);
				gs->Talk(message,RMsg_Speech::EMOTE, Lyra::ID_UNKNOWN);

				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
					(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
				DestroyWindow(hDlg);

				return TRUE;

			case IDC_SPEAK:
			{	// the hidden listbox containing Maren contains the text sent to others
				int index = ListBox_GetCurSel(hwnd_pmare_speech);
				ListBox_GetText(hwnd_pmare_speech_maren, index, message);
				gs->Talk(message,RMsg_Speech::SPEECH, Lyra::ID_UNKNOWN);
				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
					(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
				DestroyWindow(hDlg);

				return TRUE;
			}

			case IDC_CANCEL:
				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
					(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
				DestroyWindow(hDlg);
				return FALSE;
				}
			}
			break;
		default:
			break;
	}
	return FALSE;
}


// Subclassed window procedure for edit control in talk dialog
// WM_PASSPROC passes in the original window proc handle
// WM_PARENT passes in the window handle for the parent talk dialog box

LRESULT WINAPI TalkEditWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static WNDPROC lpfn_wproc;
	static HWND hwnd_parent;
	static TCHAR last,nexttolast;

	if (HBRUSH brush = SetControlColors(hwnd, message, wParam, lParam))
		return (LRESULT)brush;

	switch(message)
	{
		case WM_PASSPROC:
			lpfn_wproc = (WNDPROC) lParam;
			last = nexttolast = '\0';
			return (LRESULT) 0;

		case WM_PARENT:
			hwnd_parent = (HWND) lParam;
			return (LRESULT) 0;

		case WM_MOUSEMOVE:
			if (mouse_look.looking || mouse_move.moving)
			{
				StopMouseMove();
				StopMouseLook();
			}
			break;

		case WM_SYSKEYUP: // Alt-something, possibly a macro..
		  if (player->CanUseChatMacros()&& wParam >= '0' && wParam <= '9')
		{
			macro_t macro;
			int macro_number = wParam - '0';
			RegistryReadMacro(macro_number,macro);
			for (unsigned int i =0; i < _tcslen(macro); i++)   // paste macro into window by
				PostMessage(hwnd, WM_CHAR,macro[i],0); // posting char msgs. to itself.
			return TRUE;
		}
		break;

		case WM_KEYUP:
			switch (wParam)
			{
				case VK_UP:
					{
						int line = Edit_GetLineCount(hwnd);
						if (line==1)
						{
							PostMessage(hwnd_parent, WM_PRIOR_LINE, 0, 0);
							return (LRESULT) 0;
						}
						break;
					}

				case VK_DOWN:
					{
						int line = Edit_GetLineCount(hwnd);
						if (line==1)
						{
							PostMessage(hwnd_parent, WM_NEXT_LINE, 0, 0);
							return (LRESULT) 0;
						}
						break;
					}

				case VK_PRIOR:
					PostMessage(hwnd_parent, WM_PRIOR_LINE, 0, 0);
					return (LRESULT) 0;
				case VK_NEXT:
					PostMessage(hwnd_parent, WM_NEXT_LINE, 0, 0);
					return (LRESULT) 0;
					case VK_ESCAPE:
					PostMessage(hwnd_parent, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return (LRESULT) 0;
			};
			break;
			case WM_CHAR:
			switch (wParam)
			{
			case VK_RETURN:
				if ((last == '.') && (nexttolast == VK_RETURN)) // strip off the . and send!
				{
					PostMessage(hwnd_parent, WM_STRIP_DOT, 0, 0);
					PostMessage(hwnd_parent, WM_COMMAND, (WPARAM) IDC_SPEAK, 0);
				} // strip off the return and send - but NOT if message dialog is up 
				  // otherwise the message dialog is not visible on whisper range errors 
				else if (!options.multiline)	
				{
					PostMessage(hwnd_parent, WM_STRIP_RETURN, 0, 0);
					PostMessage(hwnd_parent, WM_COMMAND, (WPARAM) IDC_SPEAK, 0);
				}
				break;
			default:
				break;
			}
			nexttolast = last; // set last and next to last chars
			last = (TCHAR)wParam;
			break;
		break;
	}

	return CallWindowProc( lpfn_wproc, hwnd, message, wParam, lParam);
}



// window proc for help dialog box
BOOL CALLBACK HelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hHelpBackground;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;
	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			helpdlg = false;
			break;

		case WM_INITDIALOG:
			helpdlg = true;
			SetFocus(hDlg);
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			hHelpBackground = CreateWindowsBitmap(IDD_HELP);
			return TRUE;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
					case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			};
			break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			BITMAP bm;
			RECT r;
			GetClientRect(hDlg,&r);
			GetObject(hHelpBackground, sizeof(BITMAP), &bm);
			HDC dc= BeginPaint(hDlg, &Paint);
			BlitBitmap(dc,hHelpBackground,&r, NOSTRETCH);
			EndPaint(hDlg,&Paint);
		}
		return 0;


		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					DeleteObject(hHelpBackground);
					DestroyWindow(hDlg);
					break;
				case IDC_KEYBOARD:
					if (!keyboarddlg)
					{
						keyboarddlg = true;
						CreateLyraDialog(hInstance, IDD_KEYBOARD_CONFIG,
							cDD->Hwnd_Main(), (DLGPROC)KeyboardConfigDlgProc);
					}
					break;
			}
	}
	return FALSE;

}

// window proc for credits dialog box
BOOL CALLBACK CreditsDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hCreditsBackground;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;
	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			creditsdlg = false;
			break;

		case WM_INITDIALOG:
			creditsdlg = true;
			SetFocus(hDlg);
			SetWindowPos(hDlg, TopMost(), 0, cDD->ViewY(),
				0, 0, SWP_NOSIZE);
			ResizeLabel(hDlg, effects->EffectWidth(IDD_CREDITS), effects->EffectHeight(IDD_CREDITS));

			hCreditsBackground = CreateWindowsBitmap(IDD_CREDITS);
			return TRUE;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
					case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			};
			break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			BITMAP bm;
			RECT r;
			GetClientRect(hDlg,&r);
			GetObject(hCreditsBackground, sizeof(BITMAP), &bm);
			HDC dc= BeginPaint(hDlg, &Paint);
			BlitBitmap(dc,hCreditsBackground,&r, NOSTRETCH);
			EndPaint(hDlg,&Paint);
		}
		return 0;


		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					DeleteObject(hCreditsBackground);
					DestroyWindow(hDlg);
					break;
			}
	}
	return FALSE;

}

static void ExitCallback(void *value)
{
  int i = *((int*)value);
  if (i == 1)
    StartExit ();
}

// window proc for meta dialog box
BOOL CALLBACK MetaDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			metadlg = false;
			break;

		case WM_INITDIALOG:
			metadlg = true;
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			SetFocus(hDlg);
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
					case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_RETURN, 0);
					return (LRESULT)0;
				case VK_RETURN:
#ifdef UL_DEBUG
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_EXIT, 0);
#else
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_RETURN, 0);
#endif
					return (LRESULT)0;
			};
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_GOTOHELP:
					if (!helpdlg)
					{
						helpdlg = true;
						CreateLyraDialog(hInstance, IDD_HELP,
							cDD->Hwnd_Main(), (DLGPROC)HelpDlgProc);
					}
					break;
				case IDC_GOTOOPTIONS:
					if (!optiondlg)
					{
						optiondlg = true;
						CreateLyraDialog(hInstance, IDD_OPTIONS,
							cDD->Hwnd_Main(), (DLGPROC)OptionsDlgProc);
					}
					break;

				case IDC_GOTOCREDITS:
					if (!creditsdlg)
					{
						creditsdlg = true;
						CreateLyraDialog(hInstance, IDD_CREDITS,
							cDD->Hwnd_Main(), (DLGPROC)CreditsDlgProc);
					}
					break;

				case IDC_RETURN:
					DestroyWindow(hDlg);
					break;

				case IDC_EXIT:
					DestroyWindow(hDlg);
          if (!exiting) {
			StartExit ();
          }
					return FALSE;
				default:
					break;
			}

		case WM_MOUSEMOVE:
			if (mouse_look.looking || mouse_move.moving)
			{
				StopMouseMove();
				StopMouseLook();
			}
			break;
	}
	return FALSE;

}


// window proc for gm item help dialog box
BOOL CALLBACK GMItemHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			gm_itemhelpdlg = false;
			break;

		case WM_INITDIALOG:
			gm_itemhelpdlg = true;
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			SetFocus(hDlg);
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
					case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
				return (LRESULT) 0;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					DestroyWindow(hDlg);
					break;
			}
	}
	return FALSE;

}

// window proc for player item help dialog box
BOOL CALLBACK PlayerItemHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			player_itemhelpdlg = false;
			break;

		case WM_INITDIALOG:
			player_itemhelpdlg = true;
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			SetFocus(hDlg);
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
					case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			};
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					DestroyWindow(hDlg);
					break;
			}
	}
	return FALSE;
}

// window proc for quest item help dialog box
BOOL CALLBACK QuestItemHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			quest_itemhelpdlg = false;
			break;

		case WM_INITDIALOG:
			player_itemhelpdlg = true;
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			SetFocus(hDlg);
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			};
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					DestroyWindow(hDlg);
					break;
			}
	}
	return FALSE;
}



// window proc for quest help dialog box
BOOL CALLBACK QuestHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			quest_helpdlg = false;
			break;

		case WM_INITDIALOG:
			quest_helpdlg = true;
			SetWindowText(GetDlgItem(hDlg, IDC_QUEST_HELP), message);
			SetWindowText(GetDlgItem(hDlg, IDC_QUEST_HELP2), disp_message);
			SetWindowPos(hDlg, TopMost(), cDD->ScaletoRes(5), cDD->ScaletoRes(5),
				0, 0, SWP_NOSIZE);
			SetFocus(hDlg);
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
				return (LRESULT) 0;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					DestroyWindow(hDlg);
					break;
			}
	}
	return FALSE;

}

const int NUM_PPOINT_HELP_PAGES = 4;
// window proc for ppoint help dialog box
BOOL CALLBACK PPointHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;
	static int page = 0;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			ppoint_helpdlg = false;
			break;

		case WM_INITDIALOG:
			ppoint_helpdlg = true;
			page = 0;
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			TCHAR huge_text[1024];
			LoadString(hInstance, IDS_PPOINT_HELP, huge_text, sizeof(huge_text));
			SetWindowText(GetDlgItem(hDlg, IDC_PPOINT_HELP), huge_text);
			LoadString(hInstance, IDS_PPOINT_NEXT, huge_text, sizeof(huge_text));
			SetWindowText(GetDlgItem(hDlg, IDC_PPOINT_NEXT), huge_text);

			//SendMessage(GetDlgItem(hDlg, IDC_PPOINT_HELP), WM_SETTEXT, 0, (LPARAM) huge_text); 

			SetFocus(hDlg);
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
				return (LRESULT) 0;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					page++;
					if (page == NUM_PPOINT_HELP_PAGES)
						page = 0;
					TCHAR huge_text[1024];
					ShowWindow(GetDlgItem(hDlg, IDC_PPOINT_HELP), SW_HIDE);
					LoadString(hInstance, IDS_PPOINT_HELP + page, huge_text, sizeof(huge_text));
					SetWindowText(GetDlgItem(hDlg, IDC_PPOINT_HELP), huge_text);
					ShowWindow(GetDlgItem(hDlg, IDC_PPOINT_HELP), SW_SHOW);
					break;
				case IDC_CANCEL:
					DestroyWindow(hDlg);
					break;
			}
	}
	return FALSE;

}


// window proc for create item dialog box
BOOL CALLBACK CreateItemDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
#ifndef AGENT
	static WNDPROC lpfn_tab;
	static HWND hwnd_tab, hwnd_forge;
	static int opened_by = -1;
	static bool art_callback;
	static int called_by = -1;
	int i,j;
	LPNMHDR notify;
	const int PROPERTY_FIELD_LENGTH = 6;
	TC_ITEM tab_item;
	const int TAB_HEIGHT = 20;
	const int TAB_WIDTH = 50;
	static int num_tokens_held = 0;
	static bool combineItem = false;
	static int curr_function = -1;
	static int curr_translation = -1;
	static int curr_value = -1;

	// iteration facilitators
	UINT property_tags[5] = {IDC_PROPERTY1_HEADER, IDC_PROPERTY2_HEADER,
		IDC_PROPERTY3_HEADER, IDC_PROPERTY4_HEADER, IDC_PROPERTY5_HEADER};
	UINT property_fields[5] = {IDC_PROPERTY1, IDC_PROPERTY2,
		IDC_PROPERTY3, IDC_PROPERTY4, IDC_PROPERTY5};

	// data storage structures that preserve state of fields on tab control
	struct item_effect_t {
		int effect_type;
		int property[LyraItem::MAX_FIELDS_PER_FUNCTION]; // Listbox index
		int property_value[LyraItem::MAX_FIELDS_PER_FUNCTION]; // translation table value
	};

	typedef TCHAR property_t[PROPERTY_FIELD_LENGTH];
	struct item_effect_string_t {
		property_t property[LyraItem::MAX_FIELDS_PER_FUNCTION];
	};

	static item_effect_t item_effect[3];
	static item_effect_string_t item_effect_string[3];

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{

		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			itemdlg = false;
			break;

		case WM_INITDIALOG: {
			itemdlg = true;
			art_callback = false;
			for (unsigned int i = 0; i < 3; i++)
				item_effect[i].effect_type = LyraItem::NO_FUNCTION;

			hwnd_forge = hDlg;

			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);

			TabCtrl_SetItemSize(GetDlgItem(hDlg, IDC_FUNCTION_TAB), TAB_WIDTH, TAB_HEIGHT);

			Edit_LimitText(GetDlgItem(hDlg, IDC_ITEM_NAME), LmItem::NAME_LENGTH-1);
			Edit_LimitText(GetDlgItem(hDlg, IDC_CHARGES), 3);

			// colors for List 1 are focus stat specific
			for (i=0; i<NUM_ACTOR_COLORS; i++)
			{
				ComboBox_AddString(GetDlgItem(hDlg, IDC_COLOR_COMBO2), ColorName(i));
				ComboBox_SetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO2), i, i);
			}

			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO2), 0);

			_stprintf(temp_message, _T("%d"), 0);
			Edit_SetText(GetDlgItem(hDlg, IDC_PT_COST), temp_message);

			for (i=1; i <= NumTalismans(); i++)
			{
				if (TalismanForgable(i))
				{
					int index = ComboBox_AddString(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO),  TalismanNameAt(i));
					ComboBox_SetItemData(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO),index, TalismanBitmapAt(i));
				}
			}
#ifdef GAMEMASTER
			// default to invis for GMs
			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO), 29);
#else		// default to first item in list
			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO), 0);
#endif
			
			ShowWindow(GetDlgItem(hDlg, IDC_ITEM_USE_PT), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_ITEM_REMAKE), SW_HIDE);
			LoadString(hInstance, IDS_SELECT_FUNCTION, message, sizeof(message));

			ComboBox_AddString(GetDlgItem(hDlg, IDC_TYPE_COMBO), message);
			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO), 0);

			ShowWindow(GetDlgItem(hDlg, IDC_PROPERTY1), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_PROPERTY2), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_PROPERTY3), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_PROPERTY4), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_PROPERTY5), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_ANY_CHARGES), SW_HIDE);

			return TRUE;
			}

		case WM_PAINT:
			if ((hDlg == hwnd_tab) || (hDlg == hwnd_forge))
			{
				if (TileBackground(hDlg))
					return (LRESULT)0;
			}
			break;


		case WM_INIT_ITEMFIELDS: { // called to set item fields when editing quests
			// NOTE: BMP 11/01 the ability to edit quests has been eliminated due to possible abuse
			// this code is left here in case it may be reinstated in the future
			cGoal* quest = (cGoal*)lParam;
			TCHAR buffer[64];
			if (NO_ITEM != quest) 
			{
				ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO1), quest->Color1());
				ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO2), quest->Color2());
				_stprintf(buffer, _T("%d"), quest->Charges());
				Edit_SetText(GetDlgItem(hDlg, IDC_CHARGES), buffer);					
			}

			break;

			}

		case WM_INIT_ITEMCREATOR: {
			called_by = lParam;
			opened_by = -1;
			switch (lParam)
			{
				case CreateItem::QUEST_ITEM: {
					opened_by = CreateItem::QUEST_ITEM;

					ComboBox_DeleteString(GetDlgItem(hDlg, IDC_TYPE_COMBO), 0);
					LoadString(hInstance, IDS_ANY_FUNCTION, message, sizeof(message));
					ComboBox_AddString(GetDlgItem(hDlg, IDC_TYPE_COMBO), message);
					ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO), 0);

					LoadString(hInstance, IDS_QUEST_FORGE_TEXT, message, sizeof(message));
					Edit_SetText(GetDlgItem(hDlg, IDC_FORGE_TEXT), message);

					// subclass the tab control so it gets painted correctly
					hwnd_tab = GetDlgItem(hDlg, IDC_FUNCTION_TAB);
					lpfn_tab = SubclassWindow(hwnd_tab, CreateItemDlgProc);
					RECT tab_rect = {0, 0, 300, 200};
					InvalidateRect(hwnd_tab, &tab_rect, TRUE);
					UpdateWindow(hwnd_tab);

					for (i=0; i<NUM_ACTOR_COLORS; i++)
					{ 
						ComboBox_AddString(GetDlgItem(hDlg, IDC_COLOR_COMBO1), ColorName(i));
						ComboBox_SetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO1), i, i);

					}

					// now add the extra "Any Color" labels

					LoadString(hInstance, IDS_ANY_COLOR, message, sizeof(message));
					ComboBox_AddString(GetDlgItem(hDlg, IDC_COLOR_COMBO1), message);
					ComboBox_SetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO1), ANY_COLOR, NUM_ACTOR_COLORS);
					ComboBox_AddString(GetDlgItem(hDlg, IDC_COLOR_COMBO2), message);
					ComboBox_SetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO2), ANY_COLOR, NUM_ACTOR_COLORS);
					
					ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO1), ANY_COLOR);// rand()%NUM_ACTOR_COLORS);
					ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO2), ANY_COLOR); //rand()%NUM_ACTOR_COLORS);

					tab_item.mask = TCIF_TEXT;
					LoadString (hInstance, IDS_ITEM_FUNCTION, disp_message, sizeof(disp_message));
					tab_item.pszText = disp_message;
					TabCtrl_InsertItem(GetDlgItem(hDlg, IDC_FUNCTION_TAB), 0, &tab_item);
					for (int i = 1; i < LyraItem::NumItemFunctions(); i++)
						if (LyraItem::FunctionCreateByForge(i))
						{
							ComboBox_AddString(GetDlgItem(hDlg, IDC_TYPE_COMBO), LyraItem::FunctionName(i));
							ComboBox_SetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), (ComboBox_GetCount(GetDlgItem(hDlg, IDC_TYPE_COMBO))-1), i);
						}

					LoadString (hInstance, IDS_1, message, sizeof(message));
					Edit_SetText(GetDlgItem(hDlg, IDC_CHARGES), message);					
					ShowWindow (GetDlgItem(hDlg, IDC_ITEM_DESCRIP), SW_HIDE);
					LoadString (hInstance, IDS_DUMMY_ITEM, message, sizeof(message));
					Edit_SetText(GetDlgItem(hDlg, IDC_ITEM_NAME), message);
					
					ShowWindow (GetDlgItem(hDlg, IDC_ITEM_NAME), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NAME_HEADER), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_FORGE_STATIC), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_QUEST_STATIC), SW_SHOW);
					ShowWindow(GetDlgItem(hDlg, IDC_ANY_CHARGES), SW_SHOW);
				
					LoadString (hInstance, IDS_ANY_GRAPHIC, message, sizeof(message));
					int index = ComboBox_AddString(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO),  message);
					ComboBox_SetItemData(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO), index, LyraBitmap::NONE);
					ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO), index); 
					}
					break; 

				case CreateItem::GM_ITEM: {
					opened_by = CreateItem::GM_ITEM;
					LoadString(hInstance, IDS_FORGE_TEXT, message, sizeof(message));
					Edit_SetText(GetDlgItem(hDlg, IDC_FORGE_TEXT), message);

					for (i=0; i<NUM_ACTOR_COLORS; i++)
					{ // gm's can choose from all colors
						ComboBox_AddString(GetDlgItem(hDlg, IDC_COLOR_COMBO1), ColorName(i));
						ComboBox_SetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO1), i, i);
					}

					ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO1), 0);
					

					tab_item.mask = TCIF_TEXT;
					tab_item.pszText = disp_message;
					LoadString (hInstance, IDS_FUNCTION1, disp_message, sizeof(disp_message));
					TabCtrl_InsertItem(GetDlgItem(hDlg, IDC_FUNCTION_TAB), 0, &tab_item);

					tab_item.mask = TCIF_TEXT;
					LoadString (hInstance, IDS_FUNCTION2, disp_message, sizeof(disp_message));
					TabCtrl_InsertItem(GetDlgItem(hDlg, IDC_FUNCTION_TAB), 1, &tab_item);

					tab_item.mask = TCIF_TEXT;
					LoadString (hInstance, IDS_FUNCTION3, disp_message, sizeof(disp_message));
					TabCtrl_InsertItem(GetDlgItem(hDlg, IDC_FUNCTION_TAB), 2, &tab_item);

					ShowWindow (GetDlgItem(hDlg, IDC_ITEM_ARTIFACT), SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg, IDC_ITEM_NOPICKUP), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_ITEM_DESCRIP), SW_SHOWNORMAL);
					

					for (i = 1; i < LyraItem::NumItemFunctions(); i++) // skip 0 ('none')
						if (LyraItem::FunctionCreateByGM(i))
						{
							ComboBox_AddString(GetDlgItem(hDlg, IDC_TYPE_COMBO), LyraItem::FunctionName(i));
							ComboBox_SetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), (ComboBox_GetCount(GetDlgItem(hDlg, IDC_TYPE_COMBO))-1), i);
						}
					ShowWindow (GetDlgItem(hDlg, IDC_FORGE_STATIC), SW_SHOW);
					ShowWindow (GetDlgItem(hDlg, IDC_QUEST_STATIC), SW_HIDE);
					break;
				}

				case CreateItem::FORGE_ITEM: {
					cItem* power_tokens[Lyra::INVENTORY_MAX];
					int num_tokens = arts->CountPowerTokens((cItem**)power_tokens, Guild::NO_GUILD);
					if (num_tokens)
						ShowWindow(GetDlgItem(hDlg, IDC_ITEM_USE_PT), SW_SHOW);
					
					opened_by = CreateItem::FORGE_ITEM;
					LoadString(hInstance, IDS_FORGE_TEXT, message, sizeof(message));
					Edit_SetText(GetDlgItem(hDlg, IDC_FORGE_TEXT), message);

					// subclass the tab control so it gets painted correctly
					hwnd_tab = GetDlgItem(hDlg, IDC_FUNCTION_TAB);
					lpfn_tab = SubclassWindow(hwnd_tab, CreateItemDlgProc);
					RECT tab_rect = {0, 0, 300, 200};
					InvalidateRect(hwnd_tab, &tab_rect, TRUE);
					UpdateWindow(hwnd_tab);

					j=0;
					for (i=player->FocusStat()-1; i<NUM_ACTOR_COLORS; i+=4)
					{ 
						ComboBox_AddString(GetDlgItem(hDlg, IDC_COLOR_COMBO1), ColorName(i));
						ComboBox_SetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO1), j, i);
						j++;
					}
					ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO1), 0);

					tab_item.mask = TCIF_TEXT;
					LoadString (hInstance, IDS_ITEM_FUNCTION, disp_message, sizeof(disp_message));
					tab_item.pszText = disp_message;
					combineItem = Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_COMBINE));
					TabCtrl_InsertItem(GetDlgItem(hDlg, IDC_FUNCTION_TAB), 0, &tab_item);
					for (int i = 1; i < LyraItem::NumItemFunctions(); i++)
						if (LyraItem::FunctionCreateByForge(i))
						{
							ComboBox_AddString(GetDlgItem(hDlg, IDC_TYPE_COMBO), LyraItem::FunctionName(i));
							ComboBox_SetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), (ComboBox_GetCount(GetDlgItem(hDlg, IDC_TYPE_COMBO))-1), i);
						}

					_stprintf(message, _T("%d"), player->Skill(Arts::FORGE_TALISMAN));
					//SendMessage(GetDlgItem(hDlg, IDC_CHARGES), EM_SETREADONLY, 1, 0);
					
					ShowWindow(GetDlgItem(hDlg, IDC_STATIC2), SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg, IDC_PT_COST), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_ITEM_DESCRIP), SW_SHOWNORMAL);
					if (player->Skill(Arts::COMBINE) > 0)
						ShowWindow(GetDlgItem(hDlg, IDC_ITEM_COMBINE), SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg, IDC_ITEM_REMAKE), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_FORGE_STATIC), SW_SHOW);
					ShowWindow (GetDlgItem(hDlg, IDC_QUEST_STATIC), SW_HIDE);
					}
					break;
				}
			}
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return 0;
					case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return 0;
			};
			break;

		case WM_SET_ART_CALLBACK: // called by object waiting for callback
			art_callback = true;
			break;

		case WM_SET_USE_PT:
			num_tokens_held = lParam;
			break;
			
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{

			case IDC_ITEM_NOPICKUP:
			{
				bool disable = !Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_NOPICKUP));
				EnableWindow(GetDlgItem(hDlg, IDC_ITEM_ARTIFACT), disable);
				break;
			}
			case IDC_ITEM_ARTIFACT:
			{
				bool disable = !Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_ARTIFACT));
				EnableWindow(GetDlgItem(hDlg, IDC_ITEM_NOPICKUP), disable);
				break;
			}

			case IDC_ITEM_COMBINE:
			{
				combineItem = Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_COMBINE));

				// refresh the pt cost
				_stprintf(temp_message, _T("%d"), PowerTokenCostToForge(curr_translation, curr_value, combineItem));
				ShowWindow(GetDlgItem(hDlg, IDC_PT_COST), SW_HIDE);
				Edit_SetText(GetDlgItem(hDlg, IDC_PT_COST), temp_message);
				ShowWindow(GetDlgItem(hDlg, IDC_PT_COST), SW_NORMAL);
				break;
			}
			case IDC_PROPERTY1:
			case IDC_PROPERTY2:
			case IDC_PROPERTY3:
			{
				if (HIWORD(wParam) == LBN_SELCHANGE && CreateItem::FORGE_ITEM == called_by)
				{
					int temp_cost, ptCost = 0;
					curr_function = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO)));

					// only count PT requirement for shields, elemens, alterors, and chakrams
					if (curr_function == LyraItem::CHANGE_STAT_FUNCTION || curr_function == LyraItem::EFFECT_PLAYER_FUNCTION || 
						curr_function == LyraItem::ARMOR_FUNCTION || curr_function == LyraItem::MISSILE_FUNCTION)
					{
						for (int j = 0; j < LyraItem::FunctionEntries(curr_function); j++)
						{
							curr_value = ComboBox_GetItemData(GetDlgItem(hDlg, property_fields[j]), ComboBox_GetCurSel(GetDlgItem(hDlg, property_fields[j])));
							temp_cost = PowerTokenCostToForge(LyraItem::EntryTranslation(curr_function, j), curr_value, combineItem);
							ptCost = MAX(ptCost, temp_cost);
						}

						_stprintf(temp_message, _T("%d"), ptCost);
						ShowWindow(GetDlgItem(hDlg, IDC_PT_COST), SW_HIDE);
						Edit_SetText(GetDlgItem(hDlg, IDC_PT_COST), temp_message);
						ShowWindow(GetDlgItem(hDlg, IDC_PT_COST), SW_NORMAL);
					}
				}
				break;
			}
			case IDC_TYPE_COMBO:
					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						if (CreateItem::FORGE_ITEM == called_by)
						{
							_stprintf(temp_message, _T("%d"), 0);
							ShowWindow(GetDlgItem(hDlg, IDC_PT_COST), SW_HIDE);
							Edit_SetText(GetDlgItem(hDlg, IDC_PT_COST), temp_message);
							ShowWindow(GetDlgItem(hDlg, IDC_PT_COST), SW_NORMAL);
						}

						int curr_selection = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO));
						int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), curr_selection);

						for (int i = 0; i < LyraItem::MAX_FIELDS_PER_FUNCTION - 1; i++)
						{
							ShowWindow(GetDlgItem(hDlg, property_tags[i]), SW_HIDE);
							ShowWindow(GetDlgItem(hDlg, property_fields[i]), SW_HIDE);
						}

						// Read in info from effect definition

						for (i = 0; i < LyraItem::FunctionEntries(curr_effect); i++)
						{
							// This is literally disgusting.
							// Here's the thing: items can have 6 fields, but forge only has 5 properties
							// Rather than adding property fields to forge -- that'd be annoying -- we simply break here
							// You should never try to forge an item with >5 FunctionEntries!
							if (i >= 5)
								break;
							// Set label text, show the appopriate tags and fields
							SetWindowText(GetDlgItem(hDlg, property_tags[i]), LyraItem::EntryName(curr_effect, i));
							ShowWindow(GetDlgItem(hDlg, property_tags[i]), SW_SHOWNORMAL);
							ShowWindow(GetDlgItem(hDlg, property_fields[i]), SW_SHOWNORMAL);

							// Load drop-down boxes with appropriate values
							ComboBox_ResetContent(GetDlgItem(hDlg, property_fields[i]));
							//if (LyraItem::EntryTranslation(curr_effect, i) == 0)
								//ComboBox_LimitText(GetDlgItem(hDlg, property_fields[i]),PROPERTY_FIELD_LENGTH);
							//else
							if (LyraItem::EntryTranslation(curr_effect, i) != 0)
							{
								int successCnt = 0;
								int num_trans = NumberTranslations(LyraItem::EntryTranslation(curr_effect, i));
								int start_at = LyraItem::EntryMinValue(curr_effect, i);
								for (int j = start_at; j < start_at + num_trans; j++)
								{
									if ((opened_by != CreateItem::FORGE_ITEM) || (CanPlayerForgeValue(LyraItem::EntryTranslation(curr_effect, i), j, num_tokens_held)))
									{
										TranslateValue(LyraItem::EntryTranslation(curr_effect, i), j);
										SendMessage((GetDlgItem(hDlg, property_fields[i])), CB_ADDSTRING, 0L, (LPARAM)(LPCTSTR)(message));
										ComboBox_SetItemData(GetDlgItem(hDlg, property_fields[i]), (ComboBox_GetCount(GetDlgItem(hDlg, property_fields[i]))-1), j);
										successCnt++;
									}
								}

								int defaultIdx = 0;
								if (LyraItem::EntryTranslation(curr_effect, i) == LyraItem::TRANSLATION_MODIFIER)
								{
									defaultIdx = successCnt / 2;
								}
								ComboBox_SetCurSel(GetDlgItem(hDlg, property_fields[i]), defaultIdx);
							}
						}
						  }
						  break;

			case IDC_ITEMHELP:
			{
				if ((opened_by == CreateItem::FORGE_ITEM) && !player_itemhelpdlg)
				{
					player_itemhelpdlg = true;
					HWND hDlg = CreateLyraDialog(hInstance, IDD_PLAYER_ITEM_HELP,	cDD->Hwnd_Main(), (DLGPROC)PlayerItemHelpDlgProc);
				}
				else if ((opened_by == CreateItem::QUEST_ITEM) && !quest_itemhelpdlg)
				{
					player_itemhelpdlg = true;
					HWND hDlg = CreateLyraDialog(hInstance, IDD_QUEST_ITEM_HELP,	cDD->Hwnd_Main(), (DLGPROC)PlayerItemHelpDlgProc);
				}
				else if (!gm_itemhelpdlg)
				{
					gm_itemhelpdlg = true;
					HWND hDlg = CreateLyraDialog(hInstance, IDD_GM_ITEM_HELP,  cDD->Hwnd_Main(), (DLGPROC)GMItemHelpDlgProc);
				}
			}
				break;
      case IDC_ANY_CHARGES:
      {
        HWND hwndControl = GetDlgItem (hDlg, IDC_CHARGES);
        if (hwndControl == GetFocus()) {
          // Don't trap the user!
          SendMessage(hDlg, WM_NEXTDLGCTL, 0, FALSE);
        }
        // Button_GetCheck returns the LAST status of the button... 
        bool disable = !Button_GetCheck(GetDlgItem(hDlg, IDC_ANY_CHARGES));
        EnableWindow (hwndControl, disable);
      } break;


			case IDC_OK: {

						LmItem info;
						LmItemHdr header;

						header.Init(0, 0);

						int curr_selection, numcharges, erritemnum;
						TCHAR itemname[LmItem::NAME_LENGTH];
						TCHAR itemcharges[4];
						TCHAR *stopstring;
						int item_format[3] = {0, 0, 0};
						bool effect_error = false;

						// Make sure last set of effect data has been saved to the struct...
						if (ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO)) != LB_ERR)
						{	// save only if there is a selected effect
							int current_tab = TabCtrl_GetCurSel(GetDlgItem(hDlg, IDC_FUNCTION_TAB));

							item_effect[current_tab].effect_type =
								ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO));

							int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[current_tab].effect_type);

							for (int i = 0; i < LyraItem::FunctionEntries(curr_effect); i++)
							{
								if (ComboBox_GetCount(GetDlgItem(hDlg, property_fields[i])) == 0)
								{
									GetWindowText(GetDlgItem(hDlg, property_fields[i]),
										item_effect_string[current_tab].property[i], PROPERTY_FIELD_LENGTH);
									item_effect_string[current_tab].property[i][PROPERTY_FIELD_LENGTH-1] = '\0';
									item_effect[current_tab].property[i] =
										_tcstol (item_effect_string[current_tab].property[i],
										&stopstring, 10);
									item_effect[current_tab].property_value[i] = item_effect[current_tab].property[i];
								}
								else
								{
									item_effect[current_tab].property[i] =
										ComboBox_GetCurSel(GetDlgItem(hDlg, property_fields[i]));
									item_effect[current_tab].property_value[i] =
										ComboBox_GetItemData(GetDlgItem(hDlg, property_fields[i]), item_effect[current_tab].property[i]);
								}
							}
						}

						
						// Check for a name
						GetWindowText( GetDlgItem(hDlg, IDC_ITEM_NAME), itemname, LmItem::NAME_LENGTH);
						itemname[LmItem::NAME_LENGTH-1] = '\0';
						if (itemname[0] == '\0')
						{
							LoadString (hInstance, IDS_NAME_ITEM, message, sizeof(message));
							CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
							break;
						}


						// Check for charges
						GetWindowText( GetDlgItem(hDlg, IDC_CHARGES), itemcharges, 4);
						itemcharges[3] = '\0';
						numcharges = _tcstol (itemcharges, &stopstring, 10);
            if (Button_GetCheck(GetDlgItem(hDlg, IDC_ANY_CHARGES)))
              numcharges = 1;

						if ((itemcharges[0] == '\0') || (numcharges < 1) || (numcharges > 255))
						{
							LoadString (hInstance, IDS_CHARGES, message, sizeof(message));
							CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
							break;
						}

						int temp_cost = 0;
						int ptCost = 0;

						// Check for out-of-range values on all tabs.  Can't handle on
						// TCN_SELCHANGING because MS sucks.
						for (int i = 0; i < 3; i++)
						{
							int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[i].effect_type);
							for (int j = 0; j < LyraItem::FunctionEntries(curr_effect); j++)
							{
								if (item_effect[i].property_value[j] < LyraItem::EntryMinValue(curr_effect, j))
								{
									LoadString (hInstance, IDS_PROPERTY1, disp_message, sizeof(disp_message));
									_stprintf(message, disp_message, j+1, i+1, LyraItem::EntryMinValue(curr_effect, j));
									CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
										cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
									return FALSE;
								}
								else if (item_effect[i].property_value[j] > LyraItem::EntryMaxValue(curr_effect, j))
								{
									LoadString (hInstance, IDS_PROPERTY2, disp_message, sizeof(disp_message));
									_stprintf(message, disp_message, j+1, i+1, LyraItem::EntryMaxValue(curr_effect, j));
									CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
										cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
									return FALSE;
								}
							}
						}


						// Sort the effects to be in decreasing byte size
						bool sorting = true;
						item_effect_t sort_effects;

						while (sorting)
						{
							sorting = false;
							for (int i = 0; i < 2; i++)
							{
								int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[i].effect_type);
								bool teleporter = (curr_effect == LyraItem::TELEPORTER_FUNCTION);
								if (teleporter && (numcharges > 1))
								{
									LoadString (hInstance, IDS_TP, message, sizeof(message));
									CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
									cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
									return FALSE;
								}

								int next_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[i+1].effect_type);
								if (LyraItem::FunctionSize(curr_effect) <
									LyraItem::FunctionSize(next_effect))
								{
									sort_effects.effect_type = item_effect[i+1].effect_type;
									for (int j = 0; j < LyraItem::MAX_FIELDS_PER_FUNCTION; j++)
									{
										sort_effects.property[j] = item_effect[i+1].property[j];
										sort_effects.property_value[j] = item_effect[i+1].property_value[j];
									}

									item_effect[i+1].effect_type = item_effect[i].effect_type;
									for (j = 0; j < LyraItem::MAX_FIELDS_PER_FUNCTION; j++)
									{
										item_effect[i+1].property[j] = item_effect[i].property[j];
										item_effect[i+1].property_value[j] = item_effect[i].property_value[j];
									}

									item_effect[i].effect_type = sort_effects.effect_type;
									for (j = 0; j < LyraItem::MAX_FIELDS_PER_FUNCTION; j++)
									{
										item_effect[i].property[j] = sort_effects.property[j];
										item_effect[i].property_value[j] = sort_effects.property_value[j];
									}

									sorting = true;
								}
							}
						}


						// Validate remaining vital input before starting to create stuff
						if ((item_effect[0].effect_type == LyraItem::NO_FUNCTION) &&
							(opened_by !=  CreateItem::QUEST_ITEM))
						{
							LoadString (hInstance, IDS_NO_EFFECT, message, sizeof(message));
							CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
							break;
						}

					
						int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[0].effect_type);
							
						// maximum amount of charges for this function
						int effect_max_charges = MaxChargesForFunction(curr_effect);

						// Max charges is the lowest of the effect limit and the forge talisman skill level
						if ((CreateItem::FORGE_ITEM == called_by) && 
							(numcharges > min(effect_max_charges, player->Skill(Arts::FORGE_TALISMAN))))
						{
							LoadString (hInstance, IDS_BAD_CHARGES, disp_message, sizeof(disp_message));
							_stprintf(message, disp_message, effect_max_charges);
							CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
							break;
						}

						// double the charges after validation
						if (combineItem && CreateItem::FORGE_ITEM == called_by) {
							numcharges *= 2;

							// make sure we don't exceed 100 charges
							if (numcharges > 100)
								numcharges = 100;
						}

						// Get the item graphic
						curr_selection = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO));

						int graphic = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO), curr_selection);

						// if meta essence, hard set graphic
						if (ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[0].effect_type)
							== LyraItem::META_ESSENCE_FUNCTION)
							header.SetGraphic(LyraBitmap::META_ESSENCE);
						else if (graphic == LyraBitmap::NONE)
							// for quest "Any Graphic" items, use blade as dummy
							// or the cItem code gets really unhappy
							header.SetGraphic(LyraBitmap::BLADE);
						else
							header.SetGraphic(graphic);

						// Get the item colors
						int color1 = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO1), ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO1)));
						int color2 = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO2), ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO2)));

						header.SetColor1(color1);
						header.SetColor2(color2);

						// Set the item format
						int item_format_sum = 0;
						for (i = 0; i < 3; i++)
						{
							int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[i].effect_type);
							if (item_effect[i].effect_type != LyraItem::NO_FUNCTION)
							{
								item_format[i] = LyraItem::FunctionSize(curr_effect);
								item_format_sum += item_format[i];
							}
						}

						if (item_format_sum > 11)
						{
							LoadString (hInstance, IDS_BAD_FORMAT, message, sizeof(message));
							CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
							break;
						}
						else
							header.SetStateFormat(LyraItem::FormatType(item_format[0], item_format[1], item_format[2]));


						// Get and set the item flags
						if (Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_ARTIFACT)))
							header.SetFlag(LyraItem::FLAG_NOREAP | LyraItem::FLAG_ALWAYS_DROP);
						else if (Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_NOPICKUP)))
							header.SetFlag(LyraItem::FLAG_NOREAP);
						// Figure next three flags out based on selected effect types
						int immutable = 0;
						int changecharges = 0;
						int num_effects_mapped = 0;
						bool sendstate = false;

						for (i = 0; i < 3; i++)
						{
							int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[i].effect_type);
							if (item_format[i] != 0)
							{
								num_effects_mapped++;
								if (LyraItem::FunctionAlwaysSendState(curr_effect))
									sendstate = true;
								if (LyraItem::FunctionImmutable(curr_effect))
									immutable++;
								else if (LyraItem::FunctionChangeCharges(curr_effect))
									changecharges++;
							}
						}

						if (sendstate)
							header.SetFlag(LyraItem::FLAG_SENDSTATE);

						if ((changecharges + immutable) == num_effects_mapped)
						{
							if (immutable == num_effects_mapped)
								header.SetFlag(LyraItem::FLAG_IMMUTABLE);
							else
								header.SetFlag(LyraItem::FLAG_CHANGE_CHARGES);
						}

#ifdef GAMEMASTER
						// Grab the description flag
						if (Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_DESCRIP)))
#endif
							header.SetFlag(LyraItem::FLAG_HASDESCRIPTION);

						// Initialize the item state
						info.Init(header, itemname, 0, 0, 0);

						int offset, size;
						char* created_item;

						// Get the rest of the effect information
						for (i = 0; i < 3; i++)
						{
							if (item_effect[i].effect_type == LyraItem::NO_FUNCTION)
								break;

							offset = 0;

							int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[i].effect_type);
							created_item = (char *)malloc(LyraItem::FunctionSize(curr_effect));
							memcpy (created_item+offset, &(curr_effect), 1);
							offset += 1;

							for (int j = 0; j < LyraItem::FunctionEntries(curr_effect); j++)
							{
								// Write out the function values, properly packed and aligned
								size = LyraItem::EntrySize(curr_effect, j);
								memcpy (created_item+offset, &(item_effect[i].property_value[j]), size);
								offset += size;
								
								temp_cost = PowerTokenCostToForge(LyraItem::EntryTranslation(curr_effect, j), item_effect[i].property_value[j], combineItem);
								ptCost = MAX(ptCost, temp_cost);
							}

							int func_size = LyraItem::FunctionSize(curr_effect);
							if (info.SetStateField(i, created_item, func_size) < 0)
							{
								erritemnum = i;
								effect_error = true;
								free (created_item);
								break;
							}

							free (created_item);
						}

						if (effect_error)
						{
							LoadString (hInstance, IDS_STATE_ERR, disp_message, sizeof(disp_message));
							_stprintf(message, disp_message, erritemnum);
							CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
							break;
						}

						// validate we can afford to create the item...the inclusion of combine makes things tricky to prevent up front
						if ((CreateItem::FORGE_ITEM == called_by) && (num_tokens_held < ptCost))
						{
							LoadString(hInstance, IDS_NOT_ENOUGH_PT, message, sizeof(message));
							CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
							break;
						}

						// Set the charges
						info.SetCharges(numcharges);

						const void *state;
						for (i=0; i<info.NumFields(); i++)
						{
							state = info.StateField(i);
							int func = (*((unsigned char*)state));
							switch (*((unsigned char*)state))
							{
								case LyraItem::TELEPORTER_FUNCTION:
								{
									lyra_item_teleporter_t teleport;									
									memcpy(&teleport, state, sizeof(teleport));
									teleport.x = (short)player->x;
									teleport.y = (short)player->y;
									teleport.level_id = level->ID();
									info.SetStateField(func, &teleport, sizeof(teleport));
									memcpy(info.StateField(i), &teleport, sizeof(teleport));
									break;
								}
							// for missile type items, fill in the bitmap field properly; 
							// should be MELEE_MISSILE or FIREBALL_MISSILE, depending on velocity
							case LyraItem::MISSILE_FUNCTION:
								{
									lyra_item_missile_t missile;									
									memcpy(&missile, state, sizeof(missile));
									if (MELEE_VELOCITY == missile.velocity)
										missile.bitmap_id = LyraBitmap::DREAMBLADE_MISSILE;
									else
										missile.bitmap_id = LyraBitmap::FIREBALL_MISSILE;
									info.SetStateField(func, &missile, sizeof(missile));
									memcpy(info.StateField(i), &missile, sizeof(missile));

								}
							}
						}
						// We're ready -- put the item in player inventory
						if (!(Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_DESCRIP))))
						{
							// do any post-dialog initialization for created item
							int status = ITEM_CREATING;
							bool temp = false;
							TCHAR descrip[ITEM_DESCRIP_LENGTH];
							if (opened_by == CreateItem::QUEST_ITEM)
							{
								status = ITEM_DUMMY;
								temp = true;
							}
							else  if (opened_by == CreateItem::FORGE_ITEM)
							{
								// Put who forged a regular item if they did not opt to provide a description
								_stprintf(descrip, "Forged by %s", player->Name());
							}
							
							cItem* item = CreateItem(player->x, player->y, player->angle, info, 0, 
								temp, GMsg_PutItem::DEFAULT_TTL, descrip, status);
							if (item == NULL)
								i = 0; // failure
							else
								i = 1;
							if (art_callback)
								arts->EndForgeTalisman(&i, ptCost);
							else if (opened_by == CreateItem::QUEST_ITEM)
							{
								postquest->EndPost(item, color1, color2, graphic);
							}
						}
						else
						{
							HWND hNextDlg =  CreateLyraDialog(hInstance, IDD_WRITE_SCROLL,
								cDD->Hwnd_Main(), (DLGPROC)WriteScrollDlgProc);
							arts->WaitForDialog(hNextDlg, Arts::FORGE_TALISMAN);

							SendMessage(hNextDlg, WM_SET_ITEM, 0, (LPARAM)(&info));
							SendMessage(hNextDlg, WM_SET_SCROLL_FORGE_CALLBACK, 0, 0);
							SendMessage(hNextDlg, WM_SET_USE_PT, 0, ptCost);
						}
						DestroyWindow(hDlg);

						return TRUE;

						break;
						}

				case IDC_CANCEL:
				{
					if (art_callback)
						arts->CancelArt();

					DestroyWindow(hDlg);
					return FALSE;
				}

				default:
					break;
			}
			break;

		case WM_NOTIFY:
			notify = (LPNMHDR) lParam;
			switch (notify->code)
			{
			case TCN_SELCHANGING:
				{ // Tab control selection is changing -- must save data

					TCHAR* stopstring;

					int current_tab = TabCtrl_GetCurSel(GetDlgItem(hDlg, IDC_FUNCTION_TAB));
					if ((ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO)) == LB_ERR) ||
						(ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO)) == LyraItem::NO_FUNCTION))
					{	// save only if there is a selected effect
						item_effect[current_tab].effect_type = LyraItem::NO_FUNCTION;
						return FALSE;
						break;
					}

					item_effect[current_tab].effect_type =
						ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO));

					int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[current_tab].effect_type);

					for (int i = 0; i < LyraItem::FunctionEntries(curr_effect); i++)
					{
						if (ComboBox_GetCount(GetDlgItem(hDlg, property_fields[i])) == 0)
						{
							GetWindowText(GetDlgItem(hDlg, property_fields[i]),
								item_effect_string[current_tab].property[i], PROPERTY_FIELD_LENGTH);
							item_effect_string[current_tab].property[i][PROPERTY_FIELD_LENGTH-1] = '\0';
							item_effect[current_tab].property[i] =
								_tcstol (item_effect_string[current_tab].property[i],
								&stopstring, 10);
							item_effect[current_tab].property_value[i] =  item_effect[current_tab].property[i];
						}
						else
						{
							item_effect[current_tab].property[i] =
								ComboBox_GetCurSel(GetDlgItem(hDlg, property_fields[i]));
							item_effect[current_tab].property_value[i] =
								ComboBox_GetItemData(GetDlgItem(hDlg, property_fields[i]), item_effect[current_tab].property[i]);
						}
					}

					return FALSE;
				}
				break;

			case TCN_SELCHANGE:
				{ // Tab control has been changed, load existing data for this tab

					int current_tab = TabCtrl_GetCurSel(GetDlgItem(hDlg, IDC_FUNCTION_TAB));

					// set the effect type
					if (item_effect[current_tab].effect_type != LyraItem::NO_FUNCTION)
						ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO),
							item_effect[current_tab].effect_type);
					else
						ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO), -1);


					int curr_selection = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_TYPE_COMBO));
					int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), curr_selection);

					for (int i = 0; i < LyraItem::MAX_FIELDS_PER_FUNCTION; i++)
					{
						ShowWindow(GetDlgItem(hDlg, property_tags[i]), SW_HIDE);
						ShowWindow(GetDlgItem(hDlg, property_fields[i]), SW_HIDE);
					}

					// Read in info from effect definition

					for (i = 0; i < LyraItem::FunctionEntries(curr_effect); i++)
						{	 // Set label text, show the appopriate tags and fields
							SetWindowText(GetDlgItem(hDlg, property_tags[i]), LyraItem::EntryName(curr_effect, i));
							ShowWindow(GetDlgItem(hDlg, property_tags[i]), SW_SHOWNORMAL);
							ShowWindow(GetDlgItem(hDlg, property_fields[i]), SW_SHOWNORMAL);

							// Load drop-down boxes with appropriate values
							ComboBox_ResetContent(GetDlgItem(hDlg, property_fields[i]));
							//if (LyraItem::EntryTranslation(curr_effect, i) == 0)
								//ListBox_LimitText(GetDlgItem(hDlg, property_fields[i]),PROPERTY_FIELD_LENGTH);
							//else
							if (LyraItem::EntryTranslation(curr_effect, i) != 0)
							{
								int num_trans = NumberTranslations(LyraItem::EntryTranslation(curr_effect, i));
								int start_at = LyraItem::EntryMinValue(curr_effect, i);
								for (int j = start_at; j < start_at + num_trans; j++)
								{
									if ((opened_by != CreateItem::FORGE_ITEM) || (CanPlayerForgeValue(LyraItem::EntryTranslation(curr_effect, i), j, num_tokens_held)))
									{
										TranslateValue(LyraItem::EntryTranslation(curr_effect, i), j);
										SendMessage((GetDlgItem(hDlg, property_fields[i])), CB_ADDSTRING, 0L, (LPARAM)(LPCTSTR)(message));
										ComboBox_SetItemData(GetDlgItem(hDlg, property_fields[i]), (ComboBox_GetCount(GetDlgItem(hDlg, property_fields[i]))-1), j);
									}
								}
							}
						}

					if (item_effect[current_tab].effect_type != LyraItem::NO_FUNCTION)
					{ // new tab already has data associated, load it
						int curr_effect = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_TYPE_COMBO), item_effect[current_tab].effect_type);
						for (int i = 0; i < LyraItem::FunctionEntries(curr_effect); i++)
						{
							if (ComboBox_GetCount(GetDlgItem(hDlg, property_fields[i])) == 0)
								SetWindowText(GetDlgItem(hDlg, property_fields[i]),
									item_effect_string[current_tab].property[i]);
							else
								ComboBox_SetCurSel(GetDlgItem(hDlg, property_fields[i]),
									item_effect[current_tab].property[i]);
						}
					}
					else
					{ // no data for this tab yet, set empty fields
						for (int i = 0; i < LyraItem::MAX_FIELDS_PER_FUNCTION; i++)
							ComboBox_SetCurSel(GetDlgItem(hDlg, property_fields[i]), -1);
					}

				}
				break;
			default:
				break;
			}
			return (LRESULT) 0;

	}
	if (hDlg == GetDlgItem(hDlg, IDC_FUNCTION_TAB))
		return CallWindowProc( lpfn_tab, hDlg, Message, wParam, lParam);
	else
#endif
		return FALSE;

}



// Helpers for keyboard config handler
int GetButtonIDC (UINT vk)
{
	switch (vk)
	{
			case 0xc0: return IDC_BACKQUOTE; case '1': return IDC_1;
			case '2': return IDC_2; 			case '3': return IDC_3;
			case '4': return IDC_4; 			case '5': return IDC_5;
			case '6': return IDC_6; 			case '7': return IDC_7;
			case '8': return IDC_8; 			case '9': return IDC_9;
			case '0': return IDC_0; 			case 0xbd: return IDC_MINUS;
			case 0xbb: return IDC_EQUAL;		case VK_BACK: return IDC_BACKSPACE;
			case VK_TAB: return IDC_TAB;		case 'Q': return IDC_Q;
			case 'W': return IDC_W; 			case 'E': return IDC_E;
			case 'R': return IDC_R; 			case 'T': return IDC_T;
			case 'Y': return IDC_Y; 			case 'U': return IDC_U;
			case 'I': return IDC_I; 			case 'O': return IDC_O;
			case 'P': return IDC_P; 			case 0xdb: return IDC_LTBRACKET;
			case 0xdd: return IDC_RTBRACKET; case 0xdc: return IDC_BACKSLASH;
			case 'A': return IDC_A; 			case 'S': return IDC_S;
			case 'D': return IDC_D; 			case 'F': return IDC_F;
			case 'G': return IDC_G; 			case 'H': return IDC_H;
			case 'J': return IDC_J; 			case 'K': return IDC_K;
			case 'L': return IDC_L; 			case 0xba: return IDC_COLON;
			case 0xde: return IDC_QUOTE;		case VK_SHIFT: return IDC_SHIFT;
			case 'Z': return IDC_Z; 			case 'X': return IDC_X;
			case 'C': return IDC_C; 			case 'V': return IDC_V;
			case 'B': return IDC_B; 			case 'N': return IDC_N;
			case 'M': return IDC_M; 			case 0xbc: return IDC_COMMA;
			case 0xbe: return IDC_PERIOD; 	case 0xbf: return IDC_SLASH;
			case VK_INSERT: return IDC_INSERT;	case VK_HOME: return IDC_HOME;
			case VK_PRIOR: return IDC_PAGEUP;	case VK_DELETE: return IDC_DELETE;
			case VK_END: return IDC_END;		case VK_NEXT: return IDC_PAGEDOWN;
			case VK_UP: return IDC_UP; 		case VK_LEFT: return IDC_LEFT;
			case VK_DOWN: return IDC_DOWN;		case VK_RIGHT: return IDC_RIGHT;
			case VK_CLEAR: return IDC_CLEAR; case VK_CONTROL: return IDC_LTCTRL;
			case VK_MENU: return IDC_ALT; 	case VK_SPACE: return IDC_SPACE;
			case VK_DIVIDE: return IDC_NUMDIV;	case VK_MULTIPLY: return IDC_NUMMULT;
			case VK_SUBTRACT: return IDC_NUMSUB;	case VK_ADD: return IDC_NUMADD;
			case VK_NUMPAD1: return IDC_NUM1;	case VK_NUMPAD2: return IDC_NUM2;
			case VK_NUMPAD3: return IDC_NUM3;	case VK_NUMPAD4: return IDC_NUM4;
			case VK_NUMPAD5: return IDC_NUM5;	case VK_NUMPAD6: return IDC_NUM6;
			case VK_NUMPAD7: return IDC_NUM7;	case VK_NUMPAD8: return IDC_NUM8;
			case VK_NUMPAD9: return IDC_NUM9;	case VK_NUMPAD0: return IDC_NUM0;
			case VK_DECIMAL: return IDC_DECIMAL;	case VK_LBUTTON: return IDC_MOUSELEFT;
			case VK_MBUTTON: return IDC_MOUSEMIDDLE;	case VK_RBUTTON: return IDC_MOUSERIGHT;
			default: return -1;
	}

}


UINT GetButtonVK (int idc)
{
	switch (idc)
	{
			case IDC_BACKQUOTE: return 0xc0; case IDC_1: return '1';
			case IDC_2: return '2'; 			case IDC_3: return '3';
			case IDC_4: return '4'; 			case IDC_5: return '5';
			case IDC_6: return '6'; 			case IDC_7: return '7';
			case IDC_8: return '8'; 			case IDC_9: return '9';
			case IDC_0: return '0'; 			case IDC_MINUS: return 0xbd;
			case IDC_EQUAL: return 0xbb;		case IDC_BACKSPACE: return VK_BACK;
			case IDC_TAB: return VK_TAB;		case IDC_Q: return 'Q';
			case IDC_W: return 'W'; 			case IDC_E: return 'E';
			case IDC_R: return 'R'; 			case IDC_T: return 'T';
			case IDC_Y: return 'Y'; 			case IDC_U: return 'U';
			case IDC_I: return 'I'; 			case IDC_O: return 'O';
			case IDC_P: return 'P'; 			case IDC_LTBRACKET: return 0xdb;
			case IDC_RTBRACKET: return 0xdd; case IDC_BACKSLASH: return 0xdc;
			case IDC_A: return 'A'; 			case IDC_S: return 'S';
			case IDC_D: return 'D'; 			case IDC_F: return 'F';
			case IDC_G: return 'G'; 			case IDC_H: return 'H';
			case IDC_J: return 'J'; 			case IDC_K: return 'K';
			case IDC_L: return 'L'; 			case IDC_COLON: return 0xba;
			case IDC_QUOTE: return 0xde;		case IDC_SHIFT: return VK_SHIFT;
			case IDC_Z: return 'Z'; 			case IDC_X: return 'X';
			case IDC_C: return 'C'; 			case IDC_V: return 'V';
			case IDC_B: return 'B'; 			case IDC_N: return 'N';
			case IDC_M: return 'M'; 			case IDC_COMMA: return 0xbc;
			case IDC_PERIOD: return 0xbe; 	case IDC_SLASH: return 0xbf;
			case IDC_INSERT: return VK_INSERT;	case IDC_HOME: return VK_HOME;
			case IDC_PAGEUP: return VK_PRIOR;	case IDC_DELETE: return VK_DELETE;
			case IDC_END: return VK_END;		case IDC_PAGEDOWN: return VK_NEXT;
			case IDC_UP: return VK_UP; 		case IDC_LEFT: return VK_LEFT;
			case IDC_DOWN: return VK_DOWN;		case IDC_RIGHT: return VK_RIGHT;
			case IDC_CLEAR: return VK_CLEAR; case IDC_LTCTRL: return VK_CONTROL;
			case IDC_ALT: return VK_MENU; 	case IDC_SPACE: return VK_SPACE;
			case IDC_NUMDIV: return VK_DIVIDE;	case IDC_NUMMULT: return VK_MULTIPLY;
			case IDC_NUMSUB: return VK_SUBTRACT;	case IDC_NUMADD: return VK_ADD;
			case IDC_NUM1: return VK_NUMPAD1;	case IDC_NUM2: return VK_NUMPAD2;
			case IDC_NUM3: return VK_NUMPAD3;	case IDC_NUM4: return VK_NUMPAD4;
			case IDC_NUM5: return VK_NUMPAD5;	case IDC_NUM6: return VK_NUMPAD6;
			case IDC_NUM7: return VK_NUMPAD7;	case IDC_NUM8: return VK_NUMPAD8;
			case IDC_NUM9: return VK_NUMPAD9;	case IDC_NUM0: return VK_NUMPAD0;
			case IDC_DECIMAL: return VK_DECIMAL;	case IDC_MOUSELEFT: return VK_LBUTTON;
			case IDC_MOUSEMIDDLE: return VK_MBUTTON;	case IDC_MOUSERIGHT: return VK_RBUTTON;
			default: return 9999;
	}

}


bool IsNumpadKey(UINT vk)
{
	if (((vk >= VK_NUMPAD0) && (vk <= VK_NUMPAD9))
		|| (vk == VK_DECIMAL))
		return true;
	else
		return false;
}


bool IsMoveMapping(int mapping)
{
	if (((mapping >= LyraKeyboard::LOOK_UP) && (mapping <= LyraKeyboard::RUN))
		|| ((mapping >= LyraKeyboard::STRAFE) && (mapping <= LyraKeyboard::SIDESTEP_RIGHT)))
		return true;
	else
		return false;
}


void UpdateKeymapDisplay (HWND hDlg)
{
	int button, counter;
	int art_number = Arts::NONE;

	int curr_selection = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO));
	int current_mapping = ListBox_GetItemData(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), curr_selection);

	if (current_mapping >= LyraKeyboard::ART)
	{
		art_number = current_mapping - LyraKeyboard::ART;

		for (counter = 0; counter < keymap->num_keys(); counter++)
		{
			button = GetButtonIDC(keymap->GetKey(counter));
			if (keymap->GetArt(counter) == art_number)
				Button_SetCheck(GetDlgItem(hDlg, button), TRUE);
			else
				Button_SetCheck(GetDlgItem(hDlg, button), FALSE);
		}
	}
	else
	{ // find and highlight keys mapped to new selection
		for (counter = 0; counter < keymap->num_keys(); counter++)
		{
			button = GetButtonIDC(keymap->GetKey(counter));
			if (keymap->GetFunc(counter) == current_mapping)
				Button_SetCheck(GetDlgItem(hDlg, button), TRUE);
			else
				Button_SetCheck(GetDlgItem(hDlg, button), FALSE);
		}
	}
}


BOOL CALLBACK KeyboardConfigDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwnd_keyboard = NULL;

	int curr_selection, current_key, button, counter, num_keys;
	int found_key = -1;
	keymap_t *map;
	UINT vk;
	HKEY reg_key = NULL;
	unsigned long result;
	int art_number;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			keyboarddlg = false;
			hwnd_keyboard = NULL;
			break;

		case WM_INITDIALOG:
			{
			keyboarddlg = true;
			hwnd_keyboard = hDlg;

			for (counter = 0; counter < keymap->num_keys(); counter++)
			{
				button = GetButtonIDC(keymap->GetKey(counter));
				Button_SetCheck(GetDlgItem(hDlg, button), TRUE);
			}

			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);

			ListBox_SetColumnWidth(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), 130);

			// Add the regular mappable functions to the listbox
			int added_counter = 0;

			for (counter = 0; counter < NUMBER_MAPPABLE_FUNCTIONS-1; counter++)
				if (keymap_names[counter].name != IDS_RESERVED)
				{
					LoadString(hInstance, keymap_names[counter].name, message, sizeof(message));
					ListBox_AddString(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), message);
					ListBox_SetItemData(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), added_counter, counter);
					added_counter++;
				}

			// The arts list must be the last set of mappable options!	Add new keys above!
			for (counter = 0; counter < NUM_ARTS; counter++)
				if (player->Skill(counter) > 0)
				{
					ListBox_AddString(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), arts->Descrip(counter));
					ListBox_SetItemData(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), added_counter, counter+LyraKeyboard::ART);
					added_counter++;
				}

			ShowWindow (GetDlgItem(hDlg, IDC_SHOWMAINKEYS), SW_HIDE);

			return TRUE;
			}

		case WM_PAINT:
			if ((hDlg == hwnd_keyboard) && TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_OK:
				{
					// save new keymapping to the registry
					num_keys = keymap->num_keys();
					map = new keymap_t[num_keys];
					keymap->GetMap(map);

					RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0,
						NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);

					RegSetValueEx(reg_key, _T("number_keys_mapped"), 0, REG_DWORD,
						(unsigned char *)&(num_keys), sizeof(num_keys));
					RegSetValueEx(reg_key, _T("key_mappings"), 0, REG_BINARY,
						(unsigned char *)map, (num_keys*sizeof(keymap_t)));
					delete map;
					RegCloseKey(reg_key);

					DestroyWindow(hDlg);
					break;
				}

			case IDC_DOOMLAYOUT:
				ResetKeyboardDefaults(hDlg, 0);
				break;

			case IDC_MOUSELAYOUT:
				ResetKeyboardDefaults(hDlg, 1);
				break;

				/*
			case IDC_DEFAULT:
					 if (HIWORD(wParam) == LBN_SELCHANGE)
				{
					int curr_selection = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_DEFAULT));

					ListBox_SetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), -1);

					for (counter = 0; counter < keymap->num_keys(); counter++)
					{
						button = GetButtonIDC(keymap->GetKey(counter));
						Button_SetCheck(GetDlgItem(hDlg, button), FALSE);
					}

					keymap->SetDefaultKeymap(curr_selection);

					for (counter = 0; counter < keymap->num_keys(); counter++)
					{
						button = GetButtonIDC(keymap->GetKey(counter));
						Button_SetCheck(GetDlgItem(hDlg, button), TRUE);
					}
				}
				break;
				*/

			case IDC_SHOWALL:
				{

					curr_selection = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO));

					ListBox_SetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), -1);
					for (counter = 0; counter < keymap->num_keys(); counter++)
					{
						button = GetButtonIDC(keymap->GetKey(counter));
						Button_SetCheck(GetDlgItem(hDlg, button), TRUE);
					}

					break;

				}

			case IDC_SCROLL_LEFT:
				{
					curr_selection = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO));

					if ((curr_selection - KEYMAP_FUNCS_PER_COLUMN) < 0)
						ListBox_SetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), 0);
					else
						ListBox_SetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), (curr_selection - KEYMAP_FUNCS_PER_COLUMN));

					SendMessage(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), WM_ACTIVATE,
						(WPARAM) WA_CLICKACTIVE, (LPARAM) hDlg);

					UpdateKeymapDisplay(hDlg);

					break;

				}

			case IDC_SCROLL_RIGHT:
				{
					int max_selection = ListBox_GetCount(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO)) - 1;
					curr_selection = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO));

					if (curr_selection == -1)
						ListBox_SetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), 0);
					else if ((curr_selection + KEYMAP_FUNCS_PER_COLUMN) > max_selection)
						ListBox_SetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), max_selection);
					else
						ListBox_SetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), (curr_selection + KEYMAP_FUNCS_PER_COLUMN));

					SendMessage(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), WM_ACTIVATE,
						(WPARAM) WA_CLICKACTIVE, (LPARAM) hDlg);

					UpdateKeymapDisplay(hDlg);

					break;

				}

			case IDC_KEY_EFFECT_COMBO:
				if (HIWORD(wParam) == LBN_SELCHANGE) // on change in list box
					UpdateKeymapDisplay(hDlg);
					 break;

			case IDC_SHOWNUMPAD:
				{

					curr_selection = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO));

					ShowWindow (GetDlgItem(hDlg, IDC_F1), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F2), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F3), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F4), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F5), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F5), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F6), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F7), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F8), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F9), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F10), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F11), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_F12), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_BACKQUOTE), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_1), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_2), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_3), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_4), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_5), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_6), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_7), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_8), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_9), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_0), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_MINUS), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_EQUAL), SW_HIDE); ShowWindow (GetDlgItem(hDlg, IDC_BACKSPACE), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_TAB), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_Q), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_W), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_E), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_R), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_T), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_Y), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_U), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_I), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_O), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_P), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_LTBRACKET), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_RTBRACKET), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_BACKSLASH), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_CAPLOCK), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_A), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_S), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_D), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_G), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_H), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_J), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_K), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_L), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_COLON), SW_HIDE); ShowWindow (GetDlgItem(hDlg, IDC_QUOTE), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_KEYRETURN), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_SHIFT), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_Z), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_X), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_C), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_V), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_B), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_N), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_M), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_COMMA), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_PERIOD), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_SLASH), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_LTCTRL), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_ALT), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_SPACE), SW_HIDE); ShowWindow (GetDlgItem(hDlg, IDC_SHOWNUMPAD), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_SHOWNUMPAD), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_DOOMLAYOUT), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_MOUSELAYOUT), SW_HIDE);

					ShowWindow (GetDlgItem(hDlg, IDC_NUMLOCK), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_NUMDIV), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMMULT), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_NUMSUB), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMADD), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_HOME), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_UP), SW_SHOWNORMAL); ShowWindow (GetDlgItem(hDlg, IDC_PAGEUP), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_LEFT), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_CLEAR), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_RIGHT), SW_SHOWNORMAL); ShowWindow (GetDlgItem(hDlg, IDC_END), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_DOWN), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_PAGEDOWN), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMRETURN), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_INSERT), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_DELETE), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_NUMLOCK2), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMDIV2), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_NUMMULT2), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMSUB2), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_NUMADD2), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMRETURN2), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_NUM7), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM8), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_NUM9), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM4), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_NUM5), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM6), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_NUM1), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM2), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_NUM3), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM0), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_DECIMAL), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_MOUSELEFT), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_MOUSEMIDDLE), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_MOUSERIGHT), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_MOUSETEXT), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_SHOWMAINKEYS), SW_SHOWNORMAL);

				}
				break;

			case IDC_SHOWMAINKEYS:
				{

					curr_selection = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO));

					ShowWindow (GetDlgItem(hDlg, IDC_F1), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F2), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F3), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F4), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F5), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F5), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F6), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F7), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F8), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F9), SW_HIDE); 	ShowWindow (GetDlgItem(hDlg, IDC_F10), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_F11), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_F12), SW_HIDE);

					/*
					ShowWindow (GetDlgItem(hDlg, IDC_F1), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_F2), SW_SHOWNORMAL); 	ShowWindow (GetDlgItem(hDlg, IDC_F3), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_F4), SW_SHOWNORMAL); 	ShowWindow (GetDlgItem(hDlg, IDC_F5), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_F5), SW_SHOWNORMAL); 	ShowWindow (GetDlgItem(hDlg, IDC_F6), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_F7), SW_SHOWNORMAL); 	ShowWindow (GetDlgItem(hDlg, IDC_F8), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_F9), SW_SHOWNORMAL); 	ShowWindow (GetDlgItem(hDlg, IDC_F10), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_F11), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_F12), SW_SHOWNORMAL);
					*/
					ShowWindow (GetDlgItem(hDlg, IDC_BACKQUOTE), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_1), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_2), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_3), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_4), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_5), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_6), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_7), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_8), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_9), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_0), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_MINUS), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_EQUAL), SW_SHOWNORMAL); ShowWindow (GetDlgItem(hDlg, IDC_BACKSPACE), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_TAB), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_Q), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_W), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_E), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_R), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_T), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_Y), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_U), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_I), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_O), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_P), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_LTBRACKET), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_RTBRACKET), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_BACKSLASH), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_CAPLOCK), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_A), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_S), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_D), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_F), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_G), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_H), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_J), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_K), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_L), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_COLON), SW_SHOWNORMAL); ShowWindow (GetDlgItem(hDlg, IDC_QUOTE), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_KEYRETURN), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_SHIFT), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_Z), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_X), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_C), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_V), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_B), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_N), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_M), SW_SHOWNORMAL);		ShowWindow (GetDlgItem(hDlg, IDC_COMMA), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_PERIOD), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_SLASH), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_LTCTRL), SW_SHOWNORMAL);	ShowWindow (GetDlgItem(hDlg, IDC_ALT), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_SPACE), SW_SHOWNORMAL); ShowWindow (GetDlgItem(hDlg, IDC_SHOWNUMPAD), SW_SHOWNORMAL);
					ShowWindow (GetDlgItem(hDlg, IDC_DOOMLAYOUT), SW_SHOWNORMAL); ShowWindow (GetDlgItem(hDlg, IDC_MOUSELAYOUT), SW_SHOWNORMAL);

					ShowWindow (GetDlgItem(hDlg, IDC_NUMLOCK), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_NUMDIV), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMMULT), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_NUMSUB), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMADD), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_HOME), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_UP), SW_HIDE); ShowWindow (GetDlgItem(hDlg, IDC_PAGEUP), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_LEFT), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_CLEAR), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_RIGHT), SW_HIDE); ShowWindow (GetDlgItem(hDlg, IDC_END), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_DOWN), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_PAGEDOWN), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMRETURN), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_INSERT), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_DELETE), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_NUMLOCK2), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMDIV2), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_NUMMULT2), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMSUB2), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_NUMADD2), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUMRETURN2), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_NUM7), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM8), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_NUM9), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM4), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_NUM5), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM6), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_NUM1), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM2), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_NUM3), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_NUM0), SW_HIDE);		ShowWindow (GetDlgItem(hDlg, IDC_DECIMAL), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_MOUSELEFT), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_MOUSEMIDDLE), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_MOUSERIGHT), SW_HIDE);	ShowWindow (GetDlgItem(hDlg, IDC_MOUSETEXT), SW_HIDE);
					ShowWindow (GetDlgItem(hDlg, IDC_SHOWMAINKEYS), SW_HIDE);
				}
				break;


			default:
				{ // handle all the actual key mapping events!
					vk = GetButtonVK(LOWORD(wParam));
					if (vk != 9999)
					{ // proceed only if the message item ID has a valid virtual key association
// 				_tprintf("hiword: %u, loword: %u\n",HIWORD(wParam),LOWORD(wParam));

						curr_selection = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO));
						int current_mapping = ListBox_GetItemData(GetDlgItem(hDlg, IDC_KEY_EFFECT_COMBO), curr_selection);

						current_key = MAPPING_NOT_FOUND;
						for (counter = 0; counter < keymap->num_keys(); counter++)
							if (vk == keymap->GetKey(counter))
							{
								current_key = counter;
								break;
							}

						if (curr_selection == LB_ERR)
						{
							// trying to deselect the key, remove mapping if it exists
							if (current_key != MAPPING_NOT_FOUND)
								keymap->InvalidateMapping(vk);
							Button_SetCheck(GetDlgItem(hDlg, LOWORD(wParam)), FALSE);
						}
						else
						{ // there is a selected function
							if (!Button_GetCheck(GetDlgItem(hDlg, LOWORD(wParam))))
							{
								keymap->InvalidateMapping(vk);
								Button_SetCheck(GetDlgItem(hDlg, LOWORD(wParam)), FALSE);
							}
							else
							{ // also trying to map the selected function to key
								art_number = Arts::NONE;

								if (current_mapping >= LyraKeyboard::ART)
								{
									art_number = current_mapping - LyraKeyboard::ART;
									current_mapping = LyraKeyboard::ART;
								}

								keymap->AddMapping(vk, current_mapping, art_number);

								Button_SetCheck(GetDlgItem(hDlg, LOWORD(wParam)), TRUE);

								if (IsMoveMapping(keymap->FindMapping(VK_SHIFT))
									&& IsNumpadKey(vk))
								{
									keymap->InvalidateMapping(VK_SHIFT);
									Button_SetCheck(GetDlgItem(hDlg, IDC_SHIFT), FALSE);
									LoadString (hInstance, IDS_NO_SHIFTRUN, message, sizeof(message));
									CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
										cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
								}
							}
						}
					}
					break;
				}

			}
			break;
	}

		return FALSE;
}

void ResetKeyboardDefaults(HWND hDlg, int default_config)
{
	int button, counter;

	for (counter = 0; counter < keymap->num_keys(); counter++)
	{
		button = GetButtonIDC(keymap->GetKey(counter));
		Button_SetCheck(GetDlgItem(hDlg, button), FALSE);
	}

	keymap->SetDefaultKeymap(default_config);

	for (counter = 0; counter < keymap->num_keys(); counter++)
	{
		button = GetButtonIDC(keymap->GetKey(counter));
		Button_SetCheck(GetDlgItem(hDlg, button), TRUE);
	}
	return;
}

BOOL CALLBACK ChooseDestinationDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static bool art_callback;
	static dlg_callback_t callback;
	int i;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch (Message)
	{
	case WM_GETDLGCODE:
		return DLGC_WANTMESSAGE;

	case WM_DESTROY:
		chooseguilddlg = false;
		break;

	case WM_INITDIALOG: 
	{
		chooseguilddlg = true;
		callback = NULL;
		art_callback = false;
		SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
		SetFocus(GetDlgItem(hDlg, IDC_DESTINATIONS));

		for (i = 0; i <= NumLocations(); i++)
		{
			if (TeleportLocationAvailable(i))
			{
				int index = ComboBox_AddString(GetDlgItem(hDlg, IDC_DESTINATIONS), LocationNameAt(i));
				ComboBox_SetItemData(GetDlgItem(hDlg, IDC_DESTINATIONS), index, i);
			}
		}

		ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_DESTINATIONS), 0);

		return TRUE;
	}
	case WM_SET_ART_CALLBACK: // called by art waiting for callback
		art_callback = true;
#ifdef AGENT // always reject
		PostMessage(hDlg, WM_COMMAND, (WPARAM)IDC_CANCEL, 0);
#endif
		return TRUE;
	case WM_PAINT:
		if (TileBackground(hDlg))
			return (LRESULT)0;
		break;

	case WM_KEYUP:
		switch (LOWORD(wParam))
		{
		case VK_RETURN:
			PostMessage(hDlg, WM_COMMAND, (WPARAM)IDC_OK, 0);
			return TRUE;
		case VK_ESCAPE:
			PostMessage(hDlg, WM_COMMAND, (WPARAM)IDC_CANCEL, 0);
			return TRUE;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_OK:

			i = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_DESTINATIONS), ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_DESTINATIONS)));
			strcpy(message, LocationCoordinateAt(i));

			if (art_callback)
				(arts->*(chooseguild_callback))(message);
			else if (callback)
				callback(message);

			DestroyWindow(hDlg);
			return TRUE;

		case IDC_CANCEL:
			i = Guild::NO_GUILD;
			if (art_callback)
				(arts->*(chooseguild_callback))(&i);
			else if (callback)
				callback(&i);
			DestroyWindow(hDlg);
			return FALSE;

		default:
			break;
		}
	}
	return FALSE;

}

///////////////////////////////////////////////////////////////////////////////////////////////
// Choose Guild Dialog

BOOL CALLBACK ChooseGuildDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static bool art_callback;
	static dlg_callback_t callback;
	static int mappings[NUM_GUILDS]; // maps list box entries to guild flags
	static int num_guilds;
	int i;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			chooseguilddlg = false;
			break;

		case WM_INITDIALOG:
			chooseguilddlg = true;
			callback = NULL;
			art_callback = false;
			memset(mappings, 0, sizeof(mappings));
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			num_guilds = 0;
			SetFocus(GetDlgItem(hDlg, IDC_GUILDS));

			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_SET_ART_CALLBACK: // called by art waiting for callback
			art_callback = true;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
#endif
			return TRUE;

		case WM_SET_CALLBACK: // called by art waiting for callback
			callback = (dlg_callback_t)lParam;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
#endif
			return TRUE;

		case WM_ADD_INITIATES: // add guilds player is an initiate in
			for (i=0; i<NUM_GUILDS; i++)
				if (player->GuildRank(i) == Guild::INITIATE)
				{
					mappings[num_guilds] = i;
					num_guilds++;
					ListBox_AddString(GetDlgItem(hDlg, IDC_GUILDS), GuildName(i));
				}
			if (ListBox_GetCurSel(GetDlgItem(hDlg, IDC_GUILDS)) == -1)
				 ListBox_SetCurSel(GetDlgItem(hDlg, IDC_GUILDS), 0);
			break;

		case WM_ADD_KNIGHTS: // add guilds player is a knight in
			for (i=0; i<NUM_GUILDS; i++)
				if (player->GuildRank(i) == Guild::KNIGHT)
				{
					mappings[num_guilds] = i;
					num_guilds++;
					ListBox_AddString(GetDlgItem(hDlg, IDC_GUILDS), GuildName(i));
				}
			if (ListBox_GetCurSel(GetDlgItem(hDlg, IDC_GUILDS)) == -1)
				 ListBox_SetCurSel(GetDlgItem(hDlg, IDC_GUILDS), 0);
			break;

		case WM_ADD_RULERS: // add guilds player is a ruler in
			for (i=0; i<NUM_GUILDS; i++)
				if (player->GuildRank(i) == Guild::RULER)
				{
					mappings[num_guilds] = i;
					num_guilds++;
					ListBox_AddString(GetDlgItem(hDlg, IDC_GUILDS), GuildName(i));
				}
			if (ListBox_GetCurSel(GetDlgItem(hDlg, IDC_GUILDS)) == -1)
				 ListBox_SetCurSel(GetDlgItem(hDlg, IDC_GUILDS), 0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					i = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_GUILDS));
					if (i == -1)
						i = Guild::NO_GUILD;
					else
						i = mappings[i];
					if (art_callback)
						(arts->*(chooseguild_callback))(&i);
					else if (callback)
						callback(&i);
					DestroyWindow(hDlg);
					return TRUE;

				case IDC_CANCEL:
					i = Guild::NO_GUILD;
					if (art_callback)
						(arts->*(chooseguild_callback))(&i);
					else if (callback)
						callback(&i);
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Summon Dialog
//
// Breaking this from Enter Value Dialog since this will eventually be expanded beyond the capability of that dialog

BOOL CALLBACK SummonDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static bool art_callback;
	static dlg_callback_t callback;


	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch (Message)
	{
	case WM_GETDLGCODE:
		return DLGC_WANTMESSAGE;

	case WM_DESTROY:
		entervaluedlg = false;
		break;

	case WM_INITDIALOG:
		SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
		Edit_SetText(GetDlgItem(hDlg, IDC_VALUE), message);
		//LoadString(hInstance, IDS_TELEPORT_DLG_MSG, message, sizeof(message));
		strcpy(message, "Enter Teleport Coordinates (x; y; level)");
		SetWindowText(GetDlgItem(hDlg, IDC_VALUE_PROMPT), message);
		callback = NULL;
		art_callback = false;
		entervaluedlg = true;
		SetFocus(GetDlgItem(hDlg, IDC_VALUE));
		return TRUE;

	case WM_PAINT:
		if (TileBackground(hDlg))
			return (LRESULT)0;
		break;

	case WM_KEYUP:
		switch (LOWORD(wParam))
		{
		case VK_RETURN:
			PostMessage(hDlg, WM_COMMAND, (WPARAM)IDC_OK, 0);
			return TRUE;
		case VK_ESCAPE:
			PostMessage(hDlg, WM_COMMAND, (WPARAM)IDC_CANCEL, 0);
			return TRUE;
		}
		break;

	case WM_SET_ART_CALLBACK: // called by art waiting for callback
		art_callback = true;
#ifdef AGENT // always reject
		PostMessage(hDlg, WM_COMMAND, (WPARAM)IDC_CANCEL, 0);
#endif
		return TRUE;

	case WM_SET_CALLBACK: // waiting for callback
		callback = (dlg_callback_t)lParam;
#ifdef AGENT // always reject
		PostMessage(hDlg, WM_COMMAND, (WPARAM)IDC_CANCEL, 0);
#endif
		return TRUE;


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_OK:
			GetDlgItemText(hDlg, IDC_VALUE, message, sizeof(message));
			if (art_callback)
				(arts->*(entervalue_callback))(message);
			else if (callback)
				callback(message);
			DestroyWindow(hDlg);
			return TRUE;

		case IDC_CANCEL:
			if (art_callback)
				arts->CancelArt();
			else if (callback)
				callback(NULL);
			DestroyWindow(hDlg);
			return FALSE;

		default:
			break;
		}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Power Token Dialog

// expects text to show in message; returns value in message,
// or NULL if cancelled
BOOL CALLBACK PowerTokenDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static bool art_callback;
	static dlg_callback_t callback;

	static int mappings[NUM_GUILDS]; // maps list box entries to guild flags
	static int num_guilds;
	int i, num_charges, skill = 0;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			entervaluedlg = false;
			break;

		case WM_INITDIALOG:
			memset(mappings, 0, sizeof(mappings));
			num_guilds = 0;
			
			skill = player->SkillSphere(Arts::POWER_TOKEN);

			for (i = skill+1; i>=1; i--)
			{
				_stprintf(temp_message, _T("%d"), i);
				int index = ComboBox_AddString(GetDlgItem(hDlg, IDC_VALUE), temp_message);
				ComboBox_SetItemData(GetDlgItem(hDlg, IDC_VALUE), index, i);
			}

			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_VALUE), 0);

			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			SetWindowText(GetDlgItem(hDlg, IDC_VALUE_PROMPT), message);
			callback = NULL;
			art_callback = false;
			entervaluedlg = true;
			
			SetFocus(GetDlgItem(hDlg, IDC_GUILDS));
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_SET_ART_CALLBACK: // called by art waiting for callback
			art_callback = true;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
#endif
			return TRUE;

		case WM_SET_CALLBACK: // waiting for callback
			callback = (dlg_callback_t)lParam;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
#endif
			return TRUE;
		
		case WM_ADD_INITIATES: // add guilds player is an initiate in
			for (i = 0; i<NUM_GUILDS; i++)
				if (player->GuildRank(i) == Guild::INITIATE)
				{
					mappings[num_guilds] = i;
					num_guilds++;
					ComboBox_AddString(GetDlgItem(hDlg, IDC_GUILDS), GuildName(i));
				}
			if (ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_GUILDS)) == -1)
				ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_GUILDS), 0);
			break;

		case WM_ADD_KNIGHTS: // add guilds player is a knight in
			for (i = 0; i<NUM_GUILDS; i++)
				if (player->GuildRank(i) == Guild::KNIGHT)
				{
					mappings[num_guilds] = i;
					num_guilds++;
					ComboBox_AddString(GetDlgItem(hDlg, IDC_GUILDS), GuildName(i));
				}
			if (ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_GUILDS)) == -1)
				ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_GUILDS), 0);
			break;

		case WM_ADD_RULERS: // add guilds player is a ruler in
			for (i = 0; i<NUM_GUILDS; i++)
				if (player->GuildRank(i) == Guild::RULER)
				{
					mappings[num_guilds] = i;
					num_guilds++;
					ComboBox_AddString(GetDlgItem(hDlg, IDC_GUILDS), GuildName(i));
				}
			if (ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_GUILDS)) == -1)
				ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_GUILDS), 0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:

					i = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_GUILDS));
					num_charges = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_VALUE), ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_VALUE)));
					if (i == -1)
						i = Guild::NO_GUILD;
					else
						i = mappings[i];
					
					_stprintf(message, _T("%d;%d"), i, num_charges);

					if (art_callback)
						(arts->*(entervalue_callback))(message);
					else if (callback)
						callback(message);
					DestroyWindow(hDlg);
					return TRUE;

				case IDC_CANCEL:
					if (art_callback)
						arts->CancelArt();
					else if (callback)
						callback(NULL);
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Enter Value Dialog

// expects text to show in message; returns value in message,
// or NULL if cancelled
BOOL CALLBACK EnterValueDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static bool art_callback;
	static dlg_callback_t callback;


	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			entervaluedlg = false;
			break;

		case WM_INITDIALOG:
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			SetWindowText(GetDlgItem(hDlg, IDC_VALUE_PROMPT), message);
			callback = NULL;
			art_callback = false;
			entervaluedlg = true;
			SetFocus(GetDlgItem(hDlg, IDC_VALUE));
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_SET_ART_CALLBACK: // called by art waiting for callback
			art_callback = true;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
#endif
			return TRUE;

		case WM_SET_CALLBACK: // waiting for callback
			callback = (dlg_callback_t)lParam;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
#endif
			return TRUE;


		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					GetDlgItemText(hDlg, IDC_VALUE, message, sizeof(message));
					if (art_callback)
						(arts->*(entervalue_callback))(message);
					else if (callback)
						callback(message);
					DestroyWindow(hDlg);
					return TRUE;

				case IDC_CANCEL:
					if (art_callback)
						arts->CancelArt();
					else if (callback)
						callback(NULL);
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Enter Text Dialog
BOOL CALLBACK EnterTextDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
static bool entertextdlg;

void CreateTextDialog(UINT IDS_prompt, TCHAR *initial_text, dlg_callback_t on_OK_callback, int size)
{
	if (entertextdlg)
		return;

	HWND hDlg = CreateLyraDialog(hInstance, IDD_ENTER_TEXT, cDD->Hwnd_Main(), (DLGPROC)EnterTextDlgProc);
	SendMessage(hDlg, WM_SET_CALLBACK, 0, (LPARAM)on_OK_callback);

	LoadString (hInstance, IDS_prompt, message, sizeof(message));
	SetWindowText(GetDlgItem(hDlg, IDC_TEXT_PROMPT), message);

	SetWindowText(GetDlgItem(hDlg, IDC_TEXT), initial_text);
	SendMessage(GetDlgItem(hDlg,IDC_TEXT),EM_SETLIMITTEXT,(WPARAM) size,0);
}

BOOL CALLBACK EnterTextDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static dlg_callback_t callback;
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			callback = NULL;
			entertextdlg = false;
			break;

		case WM_INITDIALOG:
			entertextdlg = true;
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			return TRUE;

		case WM_SET_CALLBACK:
			callback = (dlg_callback_t)lParam;
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
				{
					if (callback)
					{
						TCHAR text[2048];
						GetDlgItemText(hDlg, IDC_TEXT, text, sizeof(text));
						callback(text);
					}

					DestroyWindow(hDlg);
					return TRUE;
				}

				case IDC_CANCEL:
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// Locate Avatar Dialog

// callback when adding to the buddy list
void AddBuddyCallback(void *value)
{
	if (locateavatardlg && value)
		PostMessage(hwnd_locateavatar, WM_ADD_BUDDY, 0,  (LPARAM)value);
}


BOOL CALLBACK LocateAvatarDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static other_t lastLoc;
	static int num_buddies = 0;
	static int buddy_list_lengths[6];
	static other_t buddies[GMsg_LocateAvatar::MAX_PLAYERS];
	int i,j;
	unsigned long result,size;
	DWORD reg_type;
	HKEY reg_key;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			locateavatardlg = false;
			hwnd_locateavatar = NULL;
			break;

		case WM_INITDIALOG:
		{
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			hwnd_locateavatar = hDlg;
			locateavatardlg = true;

			RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0,
							NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);

			size = sizeof(num_buddies);
			result = RegQueryValueEx(reg_key, _T("num_buddies"), NULL, &reg_type,
				(unsigned char *)(&num_buddies), &size);
			if (result != ERROR_SUCCESS)
				num_buddies = 0;

			size = GMsg_LocateAvatar::MAX_PLAYERS*sizeof(other_t);
			result = RegQueryValueEx(reg_key, _T("watch_list"), NULL, &reg_type,
				(unsigned char *)buddies, &size);
			if ((result != ERROR_SUCCESS) || (!num_buddies))
			{
				for (i=0; i<GMsg_LocateAvatar::MAX_PLAYERS; i++)
					_tcscpy(buddies[i].name, _T(""));
				num_buddies = 0;
			}
//			size = sizeof(num_buddies)*6;
//			result = RegQueryValueEx(reg_key, "buddy_list_lengths", NULL, &reg_type, 
//				(unsigned char *)buddy_list_lengths, &size);

			RegCloseKey(reg_key);
			for (i=0; i<num_buddies; i++)
				ListBox_AddString(GetDlgItem(hDlg, IDC_WATCH_LIST), buddies[i].name);

			if (num_buddies)
				ListBox_SetCurSel(GetDlgItem(hDlg, IDC_WATCH_LIST), 0);
			
			if (lastLoc.name != NULL)
				Edit_SetText(GetDlgItem(hDlg, IDC_AVATARS_NAME), lastLoc.name);
			SetFocus(GetDlgItem(hDlg, IDC_AVATARS_NAME));
			return TRUE;
		}

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_LBUTTONDOWN:
		{
			RECT r;
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);
			GetWindowRect(hDlg, &r);
			x += r.left;
			y += r.top + 4; // cheat factor

			GetWindowRect(GetDlgItem(hDlg, IDC_WATCH_LIST_ARROWS), &r);
			// if within the arrow...
			if ((x >= r.left) && (x <= r.right) && (y >= r.top) && (y <= r.bottom))
			{
				int sel = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_WATCH_LIST));
				int old_sel = sel;
				if (y <= (r.top + ((r.bottom - r.top)/2)))
					sel--;
				else
					sel++;
				if (sel < 0)
					sel = 0;
				else if (sel >= ListBox_GetCount(GetDlgItem(hDlg, IDC_WATCH_LIST)))
					sel = ListBox_GetCount(GetDlgItem(hDlg, IDC_WATCH_LIST)) - 1;
				ListBox_SetCurSel(GetDlgItem(hDlg, IDC_WATCH_LIST), sel);
				if (sel != old_sel)
					PostMessage(hDlg, WM_COMMAND,
					(WPARAM)MAKEWPARAM(IDC_WATCH_LIST, LBN_SELCHANGE),
					(LPARAM)GetDlgItem(hDlg, IDC_WATCH_LIST));
				InvalidateRect(GetDlgItem(hDlg, IDC_WATCH_LIST), NULL, TRUE);
			}
			break;
		}

		case WM_ADD_BUDDY:
		{
			TCHAR *buddy = ((TCHAR*)lParam);

			for (i=0; i<num_buddies; i++)
				if (0 == _tcscmp(buddy, buddies[i].name))
				{
					LoadString (hInstance, IDS_ALREADY_ON_LIST, message, sizeof(message));
					CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					return TRUE;
				}

			ListBox_AddString(GetDlgItem(hDlg, IDC_WATCH_LIST), buddy);
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_WATCH_LIST), num_buddies);
			_tcscpy(buddies[num_buddies].name, buddy);
			num_buddies++;
			RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0,
							NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
			RegSetValueEx(reg_key, _T("num_buddies"), 0, REG_DWORD,
				(unsigned char *)&(num_buddies), sizeof(num_buddies));
			RegSetValueEx(reg_key, _T("watch_list"), 0, REG_BINARY,
				(unsigned char *)buddies, GMsg_LocateAvatar::MAX_PLAYERS*sizeof(other_t));
			RegCloseKey(reg_key);

			return TRUE;
		}

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_LOCATE, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;


		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_LOCATE:
					GetDlgItemText(hDlg, IDC_AVATARS_NAME, message, sizeof(message));
					if ((_tcslen(message) == 0) || (GetFocus() == GetDlgItem(hDlg, IDC_WATCH_LIST)))
					{
						i = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_WATCH_LIST));
						if (i != -1)
						{
							if (arts->DoingLocate()){
								lastLoc = buddies[i];
								arts->EndLocate(buddies[i].name);
							}
							else 
								arts->CancelArt();
						}
						else
							arts->CancelArt();
					}
					else
					{
						if (arts->DoingLocate()){
							_tcscpy(lastLoc.name, message);
							arts->EndLocate(message);
						}
						else{
							LoadString (hInstance, IDS_LOCATE_PROB, message, sizeof(message));
							display->DisplayMessage(message, false);
						}

					}

					DestroyWindow(hDlg);
					return TRUE;

				case IDC_LOCATE_ALL:
					if (!num_buddies)
					{
						LoadString (hInstance, IDS_NEED_BUDDIES, message, sizeof(message));
						HWND hDlg = CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
											cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
						return TRUE;
					}
					if (arts->DoingLocate())
						arts->EndLocate(NULL);
					DestroyWindow(hDlg);
					return TRUE;

// Jared 3-15-00 
// Functionality changed. If player has already typed a name in the main text box before
// clicking "Add," just add that name instead of presenting them with a new dialog. 
				case IDC_ADD:
					if (num_buddies >= GMsg_LocateAvatar::MAX_PLAYERS)
					{
						LoadString (hInstance, IDS_BUDDY_LIST_FULL, message, sizeof(message));
						CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					}
					else if (!entervaluedlg)
					{
						GetDlgItemText(hDlg, IDC_AVATARS_NAME, message, sizeof(message));
						if (_tcslen(message) == 0){
							LoadString (hInstance, IDS_ADD_BUDDY, message, sizeof(message));
							HWND hDlg = CreateLyraDialog(hInstance, IDD_ENTER_VALUE,
												cDD->Hwnd_Main(), (DLGPROC)EnterValueDlgProc);
							SendMessage(hDlg, WM_SET_CALLBACK, 0, (LPARAM)AddBuddyCallback);
						}
						else
						{							
							PostMessage(hwnd_locateavatar, WM_ADD_BUDDY, 0,  (LPARAM)message);
						}

					}
					return TRUE;

// Jared 3-15-00
// Backup of old functionality
/*
				case IDC_ADD:
					if (num_buddies >= GMsg_LocateAvatar::MAX_PLAYERS)
					{
						LoadString (hInstance, IDS_BUDDY_LIST_FULL, message, sizeof(message));
						CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					}
					else if (!entervaluedlg)
					{
						LoadString (hInstance, IDS_ADD_BUDDY, message, sizeof(message));
						HWND hDlg = CreateLyraDialog(hInstance, IDD_ENTER_VALUE,
											cDD->Hwnd_Main(), (DLGPROC)EnterValueDlgProc);
						SendMessage(hDlg, WM_SET_CALLBACK, 0,	(LPARAM)AddBuddyCallback);
					}
					return TRUE;
*/

				case IDC_REMOVE:
					j = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_WATCH_LIST));
					if (j == -1)
					{
						LoadString (hInstance, IDS_SELECT_BUDDY, message, sizeof(message));
						CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					}
					else
					{
						ListBox_DeleteString(GetDlgItem(hDlg, IDC_WATCH_LIST), j);
						for (i=j; i<num_buddies-1; i++)
							_tcscpy(buddies[i].name, buddies[i+1].name);
						num_buddies--;
						_tcscpy(buddies[num_buddies].name, _T(""));
						if (num_buddies)
						{
							if (j == num_buddies)
								j--;
							ListBox_SetCurSel(GetDlgItem(hDlg, IDC_WATCH_LIST), j);
						}
					}
					RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0,
							NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
					RegSetValueEx(reg_key, _T("num_buddies"), 0, REG_DWORD,
						(unsigned char *)&(num_buddies), sizeof(num_buddies));
					RegSetValueEx(reg_key, _T("watch_list"), 0, REG_BINARY,
						(unsigned char *)buddies, GMsg_LocateAvatar::MAX_PLAYERS*sizeof(other_t));
					RegCloseKey(reg_key);
					return TRUE;

				case IDC_CANCEL:
					arts->CancelArt();
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// Ignore List Dialog

// callback when adding to the ignore list
void AddEnemyCallback(void *value)
{
	if (ignorelistdlg && value)
		PostMessage(hwnd_ignorelist, WM_ADD_BUDDY, 0,  (LPARAM)value);
}


BOOL CALLBACK IgnoreListDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int i,j, num_buddies;
	unsigned long result;
	HKEY reg_key;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			ignorelistdlg = false;
			hwnd_ignorelist = NULL;
			break;

		case WM_INITDIALOG:
		{
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			hwnd_ignorelist = hDlg;
			ignorelistdlg = true;

			RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0,
							NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
			RegCloseKey(reg_key);
			for (i=0; i<options.num_bungholes; i++)
				ListBox_AddString(GetDlgItem(hDlg, IDC_IGNORE_LIST), options.bungholes[i].name);

			if (options.num_bungholes)
				ListBox_SetCurSel(GetDlgItem(hDlg, IDC_IGNORE_LIST), 0);

			return TRUE;
		}

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_LBUTTONDOWN:
		{
			RECT r;
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);
			GetWindowRect(hDlg, &r);
			x += r.left;
			y += r.top + 4; // cheat factor

			GetWindowRect(GetDlgItem(hDlg, IDC_WATCH_LIST_ARROWS), &r);
			// if within the arrow...
			if ((x >= r.left) && (x <= r.right) && (y >= r.top) && (y <= r.bottom))
			{
				int sel = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_IGNORE_LIST));
				int old_sel = sel;
				if (y <= (r.top + ((r.bottom - r.top)/2)))
					sel--;
				else
					sel++;
				if (sel < 0)
					sel = 0;
				else if (sel >= ListBox_GetCount(GetDlgItem(hDlg, IDC_IGNORE_LIST)))
					sel = ListBox_GetCount(GetDlgItem(hDlg, IDC_IGNORE_LIST)) - 1;
				ListBox_SetCurSel(GetDlgItem(hDlg, IDC_IGNORE_LIST), sel);
				if (sel != old_sel)
					PostMessage(hDlg, WM_COMMAND,
					(WPARAM)MAKEWPARAM(IDC_IGNORE_LIST, LBN_SELCHANGE),
					(LPARAM)GetDlgItem(hDlg, IDC_IGNORE_LIST));
				InvalidateRect(GetDlgItem(hDlg, IDC_IGNORE_LIST), NULL, TRUE);
			}
			break;
		}

		case WM_ADD_BUDDY:
		{
			TCHAR *bunghole = ((TCHAR*)lParam);
			bunghole[Lyra::PLAYERNAME_MAX-1] = '\0';

			for (i=0; i<options.num_bungholes; i++)
				if (0 == _tcscmp(bunghole, options.bungholes[i].name))
				{
					LoadString (hInstance, IDS_ALREADY_ON_LIST, message, sizeof(message));
					CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
								cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					return TRUE;
				}

			ListBox_AddString(GetDlgItem(hDlg, IDC_IGNORE_LIST), bunghole);
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_IGNORE_LIST), options.num_bungholes);
			_tcscpy(options.bungholes[options.num_bungholes].name, bunghole);
			options.num_bungholes++;

			RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0,
				NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
			RegSetValueEx(reg_key, _T("num_ignores"), 0, REG_DWORD,
				(unsigned char *)&(options.num_bungholes), sizeof(options.num_bungholes));
			RegSetValueEx(reg_key, _T("ignore_list"), 0, REG_BINARY,
				(unsigned char *)options.bungholes, MAX_IGNORELIST*sizeof(other_t));
			RegCloseKey(reg_key);

			return TRUE;
		}

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
			}
			break;


		case WM_COMMAND:
			switch (LOWORD(wParam))
			{

				case IDC_ADD:
					if (options.num_bungholes >= MAX_IGNORELIST)
					{
						LoadString (hInstance, IDS_BUDDY_LIST_FULL, message, sizeof(message));
						CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					}
					else if (!entervaluedlg)
					{
						LoadString (hInstance, IDS_ADD_BUDDY, message, sizeof(message));
						HWND hDlg = CreateLyraDialog(hInstance, IDD_ENTER_VALUE,
											cDD->Hwnd_Main(), (DLGPROC)EnterValueDlgProc);
						SendMessage(hDlg, WM_SET_CALLBACK, 0,	(LPARAM)AddEnemyCallback);
					}
					return TRUE;

				case IDC_REMOVE:
					j = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_IGNORE_LIST));
					num_buddies = ListBox_GetCount(GetDlgItem(hDlg, IDC_IGNORE_LIST));
					if (j == -1)
					{
						LoadString (hInstance, IDS_SELECT_BUDDY, message, sizeof(message));
						CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					}
					else if (num_buddies > 0)
					{
						ListBox_DeleteString(GetDlgItem(hDlg, IDC_IGNORE_LIST), j);
						for (i=j; i<options.num_bungholes-1; i++)
							_tcscpy(options.bungholes[i].name, options.bungholes[i+1].name);
						options.num_bungholes--;
						_tcscpy(options.bungholes[options.num_bungholes].name, _T(""));
						if (options.num_bungholes)
						{
							if (j == options.num_bungholes)
								j--;
							ListBox_SetCurSel(GetDlgItem(hDlg, IDC_IGNORE_LIST), j);
						}
					}
					RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0,
							NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
					RegSetValueEx(reg_key, _T("num_ignores"), 0, REG_DWORD,
						(unsigned char *)&(options.num_bungholes), sizeof(options.num_bungholes));
					RegSetValueEx(reg_key, _T("ignore_list"), 0, REG_BINARY,
						(unsigned char *)options.bungholes, MAX_IGNORELIST*sizeof(other_t));
					RegCloseKey(reg_key);
					return TRUE;

				case IDC_OK:
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Write Scroll Dialog

// Because we can be called here from three places (Inscribe, Quest,
// Forge, or QuestBuilder), we need to keep callback variables to keep it straight

BOOL CALLBACK WriteScrollDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static LmItem associated_item;
	static bool forge_callback;
	static bool quest_callback;
	static int ptCost;
	static bool questbuilder_callback;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			writescrolldlg = false;
			break;

		case WM_SET_ITEM:
		{
			associated_item = *((LmItem*)lParam);
			ShowWindow(GetDlgItem(hDlg, IDC_NAME_HEADER), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_ITEM_NAME), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_CHARGES_HEADER), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_COLORS_STATIC), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_COLOR_COMBO1), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_COLOR_COMBO2), SW_HIDE);
			return TRUE;
		}

		case WM_SET_USE_PT:
		{
			ptCost = (int)lParam;
			return TRUE;
		}

		case WM_INITDIALOG:
		{

			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			writescrolldlg = true;
			forge_callback = false;
			quest_callback = false;
			questbuilder_callback = false;

			if (arts->CurrentArt() == Arts::FORGE_TALISMAN) 
				forge_callback = true;
			else if (arts->CurrentArt() == Arts::QUEST) 
				quest_callback = true;
			else if (quests->Active())
				questbuilder_callback = true;

			if (questbuilder_callback)
			{	// hide scroll-specific windows when using for questbuilder callback
				ShowWindow(GetDlgItem(hDlg, IDC_CHARGES_HEADER), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COLORS_STATIC), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_NAME_HEADER), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_ITEM_NAME), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COLOR_COMBO1), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_COLOR_COMBO2), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_KEYWORDS_STATIC), SW_SHOWNORMAL);
				LoadString (hInstance, IDS_KEYWORDS, temp_message, sizeof(temp_message));
				Edit_SetText(GetDlgItem(hDlg, IDC_KEYWORDS_STATIC), temp_message);
			} 
			else
			{
				ShowWindow(GetDlgItem(hDlg, IDC_CHARGES_HEADER), SW_SHOWNORMAL);
				ShowWindow(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), SW_SHOWNORMAL);
				ShowWindow(GetDlgItem(hDlg, IDC_COLORS_STATIC), SW_SHOWNORMAL);
				ShowWindow(GetDlgItem(hDlg, IDC_NAME_HEADER), SW_SHOWNORMAL);
				ShowWindow(GetDlgItem(hDlg, IDC_ITEM_NAME), SW_SHOWNORMAL);
				ShowWindow(GetDlgItem(hDlg, IDC_COLOR_COMBO1), SW_SHOWNORMAL);
				ShowWindow(GetDlgItem(hDlg, IDC_COLOR_COMBO2), SW_SHOWNORMAL);
				ShowWindow(GetDlgItem(hDlg, IDC_KEYWORDS_STATIC), SW_HIDE);
			}

			if (quest_callback)
			{
				ShowWindow(GetDlgItem(hDlg, IDC_WRITE_KEYWORDS_STATIC), SW_HIDE);				
				ShowWindow(GetDlgItem(hDlg, IDC_WRITE_SCROLL_STATIC), SW_HIDE);
			}
			else if (questbuilder_callback)
			{
				ShowWindow(GetDlgItem(hDlg, IDC_WRITE_SCROLL_STATIC), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_WRITE_QUEST_STATIC), SW_HIDE);
			}
			else
			{
				ShowWindow(GetDlgItem(hDlg, IDC_WRITE_KEYWORDS_STATIC), SW_HIDE);				
				ShowWindow(GetDlgItem(hDlg, IDC_WRITE_QUEST_STATIC), SW_HIDE);
			}

			// set associated item to a default state

			LoadString (hInstance, IDS_UNNAMED, temp_message, sizeof(temp_message));
			associated_item.Init(LmItemHdr::DEFAULT_INSTANCE, temp_message, 0, 0, 0);

			int skill = player->Skill(Arts::WRITE_SCROLL);
			if (quest_callback)
				skill = player->Skill(Arts::TRAIN);

			LoadString (hInstance, IDS_INFINITE, temp_message, sizeof(temp_message));
			ListBox_AddString(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), temp_message);
			ListBox_SetItemData(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), 0, 254);

			for (int i=1; i<skill+1; i++)
			{
				_stprintf(message, _T("%d"), i);
				ListBox_AddString(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), message);
				ListBox_SetItemData(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), i, i);
			}

			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), 0);

			if (!quest_callback)
			{
				LoadString (hInstance, IDS_CODEX, temp_message, sizeof(temp_message));
				Edit_SetText(GetDlgItem(hDlg, IDC_ITEM_NAME), temp_message);
			}
			else
			{
				LoadString (hInstance, IDS_QUEST2, temp_message, sizeof(temp_message));
				_stprintf(message, temp_message, player->Name());
				Edit_SetText(GetDlgItem(hDlg, IDC_ITEM_NAME), message);
			}

			Edit_LimitText(GetDlgItem(hDlg, IDC_ITEM_NAME), LmItem::NAME_LENGTH-1);

			// colors
			for (int i=0; i<NUM_ACTOR_COLORS; i++)
			{
				ListBox_AddString(GetDlgItem(hDlg, IDC_COLOR_COMBO1), ColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_COLOR_COMBO2), ColorName(i));
			}
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO1), 0);
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO2), 0);

			ShowWindow(GetDlgItem(hDlg, IDC_ITEM_ARTIFACT), SW_HIDE);

#ifdef GAMEMASTER
			if (!questbuilder_callback) 
				ShowWindow(GetDlgItem(hDlg, IDC_ITEM_ARTIFACT), SW_SHOWNORMAL);
#endif
			Edit_LimitText(GetDlgItem(hDlg, IDC_SCROLL), Lyra::MAX_ITEMDESC-1);
			SetFocus(GetDlgItem(hDlg, IDC_SCROLL));

			return TRUE;
		}

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_SET_SCROLL_FORGE_CALLBACK:
			forge_callback = true;
			break;

		case WM_SET_SCROLL_QUESTBUILDER_CALLBACK:
			forge_callback = true;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
				{
					LoadString (hInstance, IDS_UNNAMED, temp_message, sizeof(temp_message));

					if (_tcscmp(associated_item.Name(), temp_message) != 0)
					{ // adding a description to a GM-forged item
						int i;
						LmItem lmitem = associated_item;
						TCHAR descrip[Lyra::MAX_ITEMDESC];
						GetDlgItemText(hDlg, IDC_SCROLL, descrip, sizeof(descrip));

						cItem* item = CreateItem(player->x, player->y, player->angle,
							lmitem, 0, false, GMsg_PutItem::DEFAULT_TTL,
							descrip);
						if (item == NULL)
							i = 0; // failure
						else
							i = 1;
						if (forge_callback)
							arts->EndForgeTalisman(&i, ptCost);
					}
					else
					{ // using the write scroll art
						scroll_t scroll_type;
						GetDlgItemText(hDlg, IDC_SCROLL, scroll_type.descrip, sizeof(scroll_type.descrip));
						GetDlgItemText(hDlg, IDC_ITEM_NAME, scroll_type.name, sizeof(scroll_type.name));
						if (!_tcslen(scroll_type.name))
						{
							if (quest_callback)
								LoadString (hInstance, IDS_QUEST_CODEX, scroll_type.name, sizeof(scroll_type.name));
							else
								LoadString (hInstance, IDS_CODEX, scroll_type.name, sizeof(scroll_type.name));

						}
						int sel = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES));
						scroll_type.color1 = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO1));
						scroll_type.color2 = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO2));
						scroll_type.num_charges = ListBox_GetItemData(GetDlgItem(hDlg, IDC_NUM_SCROLL_CHARGES), sel);
						// Get and set the item flags
						if (Button_GetCheck(GetDlgItem(hDlg, IDC_ITEM_ARTIFACT)))
							scroll_type.artifact = true;
						else
							scroll_type.artifact = false;

						if (quest_callback)
							arts->EndQuest(&scroll_type);
						else if (questbuilder_callback)
						{
							int color1 = ListBox_GetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO1), ListBox_GetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO1)));
							int color2 = ListBox_GetItemData(GetDlgItem(hDlg, IDC_COLOR_COMBO2), ListBox_GetCurSel(GetDlgItem(hDlg, IDC_COLOR_COMBO2)));
							int graphic = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO), ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_GRAPHIC_COMBO)));
							postquest->EndPost(&scroll_type, color1, color2, graphic);
						}
						else
							arts->EndWriteScroll(&scroll_type);
					}

					DestroyWindow(hDlg);
					return TRUE;
				}

				case IDC_CANCEL:
					arts->CancelArt();
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}





// convenience function to set dlg box attributes according to player's avatar
void SetAvatarValues(HWND hDlg)
{

	ListBox_SetCurSel(GetDlgItem(hDlg, IDC_REGION0), player->Avatar().Color0());
	ListBox_SetCurSel(GetDlgItem(hDlg, IDC_REGION1), player->Avatar().Color1());
	ListBox_SetCurSel(GetDlgItem(hDlg, IDC_REGION2), player->Avatar().Color2());
	ListBox_SetCurSel(GetDlgItem(hDlg, IDC_REGION3), player->Avatar().Color3());
	ListBox_SetCurSel(GetDlgItem(hDlg, IDC_REGION4), player->Avatar().Color4());

	if (player->Avatar().Teacher() || player->Avatar().Apprentice())
		Button_SetCheck(GetDlgItem(hDlg, IDC_SHIELD),1);
	else
		Button_SetCheck(GetDlgItem(hDlg, IDC_SHIELD),0);

	Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_NOTHING),1);
	Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_NOTHING),1);
	Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_HOUSE), 0);
	Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_HOUSE), 0);
	Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_SPHERE), 0);
	Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_SPHERE), 0);

	switch (player->Avatar().ShowGuild())
	{
		case Patches::SHOW_BOTH:
			Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_HOUSE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_HOUSE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_NOTHING), 0);
			Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_NOTHING), 0);
			break;
		case Patches::SHOW_FRONT:
			Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_HOUSE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_NOTHING), 0);
			break;
		case Patches::SHOW_BACK:
			Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_HOUSE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_NOTHING), 0);
			break;
		case Patches::DONT_SHOW:
		default:
			break;
	}

	switch (player->Avatar().ShowSphere())
	{
		case Patches::SHOW_BOTH:
			Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_SPHERE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_SPHERE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_NOTHING), 0);
			Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_NOTHING), 0);
			break;
		case Patches::SHOW_FRONT:
			Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_SPHERE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_FRONT_NOTHING), 0);
			break;
		case Patches::SHOW_BACK:
			Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_SPHERE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_BACK_NOTHING), 0);
			break;
		case Patches::DONT_SHOW:
		default:
			break;
	}

	/*
	// disable guild select box if guild patch is not showing
	if (Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_HOUSE)) ||
		Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_HOUSE)))
	{
		ShowWindow(GetDlgItem(hDlg, IDC_HOUSE_ID), SW_SHOWNORMAL);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC_HOUSE_ID), SW_SHOWNORMAL);
	}
		//EnableWindow(GetDlgItem(hDlg, ), TRUE);
	else
	{
		ShowWindow(GetDlgItem(hDlg, IDC_HOUSE_ID), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC_HOUSE_ID), SW_HIDE);
	}
		//EnableWindow(GetDlgItem(hDlg, IDC_HOUSE_ID), FALSE);
		*/
}

// called when the user presses O.K in the Avatar Describe dialog box
void AvatarDescripCallback(void *new_descrip)
{
	gs->SetAvatarDescrip((TCHAR*)new_descrip);

	if (gs && gs->LoggedIntoGame())
		gs->SendAvatarDescrip();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Avatar Customization Dialog


BOOL CALLBACK AvatarDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static LmAvatar old_avatar, curr_avatar;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			avatardlg = false;
			hwnd_avatar = NULL;
			break;

		case WM_INITDIALOG:
		{
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			avatardlg = true;
			hwnd_avatar = hDlg;

			cp->SetMode(AVATAR_TAB,false,false);

			// if in multiple houses, show houses drop-down
			int house_count = 0;
			for (unsigned i=0; i<NUM_GUILDS; i++)
				if (player->IsInitiate(i) ||
					player->IsKnight(i) ||
					player->IsRuler(i))
				{
					ListBox_AddString(GetDlgItem(hDlg, IDC_HOUSE_ID), GuildName(i));
					ListBox_SetItemData(GetDlgItem(hDlg, IDC_HOUSE_ID), house_count, i);
					if (i == player->Avatar().GuildID())
						ListBox_SetCurSel(GetDlgItem(hDlg, IDC_HOUSE_ID), i);
					house_count++;
				}

			if (house_count < 2)
			{
				ShowWindow(GetDlgItem(hDlg, IDC_STATIC_HOUSE_ID), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_HOUSE_ID), SW_HIDE);
				if (house_count) //		auto-select for a single house
					ListBox_SetCurSel(GetDlgItem(hDlg, IDC_HOUSE_ID), 0);
				else
				{ // can't select guild patch if not in a house...
					ShowWindow(GetDlgItem(hDlg, IDC_FRONT_HOUSE), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDC_BACK_HOUSE), SW_HIDE);
				}
			}

			if (!player->IsTeacher() && !player->IsApprentice())
				ShowWindow(GetDlgItem(hDlg, IDC_SHIELD), SW_HIDE);			

			old_avatar = curr_avatar = player->Avatar();

			for (int i=0; i<NUM_ACTOR_COLORS; i++)
			{
				ListBox_AddString(GetDlgItem(hDlg, IDC_REGION0), ColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_REGION1), ColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_REGION2), ColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_REGION3), ColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_REGION4), ColorName(i));
			}

			ResizeLabel(GetDlgItem(hDlg, IDC_ARMS), effects->EffectWidth(IDC_ARMS), effects->EffectHeight(IDC_ARMS));
			ResizeLabel(GetDlgItem(hDlg, IDC_LEGS), effects->EffectWidth(IDC_LEGS), effects->EffectHeight(IDC_LEGS));
			ResizeLabel(GetDlgItem(hDlg, IDC_HAIR), effects->EffectWidth(IDC_HAIR), effects->EffectHeight(IDC_HAIR));
			ResizeLabel(GetDlgItem(hDlg, IDC_BODY), effects->EffectWidth(IDC_BODY), effects->EffectHeight(IDC_BODY));
			ResizeLabel(GetDlgItem(hDlg, IDC_SKIN), effects->EffectWidth(IDC_SKIN), effects->EffectHeight(IDC_SKIN));
			ResizeLabel(GetDlgItem(hDlg, IDC_AVATAR_TITLE), effects->EffectWidth(IDC_AVATAR_TITLE), effects->EffectHeight(IDC_AVATAR_TITLE));

			SetAvatarValues(hDlg);

			return TRUE;
		}

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return (LRESULT) 0;
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			}
			break;

		case WM_COMMAND:

			if (((HIWORD(wParam) == LBN_SELCHANGE) || (HIWORD(wParam) == BN_CLICKED)) &&
				!(player->flags & ACTOR_TRANSFORMED))
			{

					curr_avatar.SetColor0(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_REGION0)));
					curr_avatar.SetColor1(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_REGION1)));
					curr_avatar.SetColor2(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_REGION2)));
					curr_avatar.SetColor3(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_REGION3)));
					curr_avatar.SetColor4(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_REGION4)));

					// Set teacher display
					if (Button_GetCheck(GetDlgItem(hDlg, IDC_SHIELD)))
					{
						if (player->IsTeacher()) {
							curr_avatar.SetTeacher(1);
							curr_avatar.SetApprentice(0);

							if (0 < player->Skill(Arts::TRAIN_SELF))
								curr_avatar.SetMasterTeacher(1);
						}
						else if (player->IsApprentice()) {
							curr_avatar.SetApprentice(1);
						}
						else
						{
							curr_avatar.SetApprentice(0);
							curr_avatar.SetTeacher(0);
							curr_avatar.SetMasterTeacher(0);
						}
					}
					else
					{
						curr_avatar.SetApprentice(0);
						curr_avatar.SetTeacher(0);
						curr_avatar.SetMasterTeacher(0);
					}


					// set sphere color (dreamsmith status)
					if (((Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_SPHERE))) ||
						(Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_SPHERE)))) &&
						player->IsDreamSmith())
						curr_avatar.SetDreamSmith(1);
					else
						curr_avatar.SetDreamSmith(0);

					// set sphere color (Wordsmith status)
					if (((Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_SPHERE))) ||
						(Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_SPHERE)))) &&
						player->IsWordSmith())
						curr_avatar.SetWordSmith(1);
					else
						curr_avatar.SetWordSmith(0);

					// set sphere color (dreamstrike status)
					if (((Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_SPHERE))) ||
						(Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_SPHERE))) ||
						(Button_GetCheck(GetDlgItem(hDlg, IDC_SHIELD)))) &&
						player->Skill(Arts::DREAMSTRIKE))
						curr_avatar.SetDreamstrike(1);
					else
						curr_avatar.SetDreamstrike(0);

					// set sphere display
					if ((Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_SPHERE))) &&
						(Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_SPHERE))))
						curr_avatar.SetShowSphere(Patches::SHOW_BOTH);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_SPHERE)))
						curr_avatar.SetShowSphere(Patches::SHOW_FRONT);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_SPHERE)))
						curr_avatar.SetShowSphere(Patches::SHOW_BACK);
					else
						curr_avatar.SetShowSphere(Patches::DONT_SHOW);

					// set guild display

					if ((Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_HOUSE))) &&
						(Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_HOUSE))))
						curr_avatar.SetShowGuild(Patches::SHOW_BOTH);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_HOUSE)))
						curr_avatar.SetShowGuild(Patches::SHOW_FRONT);
					else if (Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_HOUSE)))
						curr_avatar.SetShowGuild(Patches::SHOW_BACK);
					else
						curr_avatar.SetShowGuild(Patches::DONT_SHOW);

					// hack: guild id pulled from player options
					int selected = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_HOUSE_ID));



					curr_avatar.SetGuildID( ListBox_GetItemData(GetDlgItem(hDlg, IDC_HOUSE_ID), selected));

					if (curr_avatar.ShowGuild() == Patches::DONT_SHOW)
					{
						curr_avatar.SetGuildRank(0);
						curr_avatar.SetGuildID(Guild::NO_GUILD);
					}
					else
						curr_avatar.SetGuildRank(player->GuildRank(curr_avatar.GuildID()));

					/*
				// disable guild select box if guild patch is not showing
					if (Button_GetCheck(GetDlgItem(hDlg, IDC_FRONT_HOUSE)) ||
					Button_GetCheck(GetDlgItem(hDlg, IDC_BACK_HOUSE)))
				{
					ShowWindow(GetDlgItem(hDlg, IDC_HOUSE_ID), SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg, IDC_STATIC_HOUSE_ID), SW_SHOWNORMAL);
				}
					//EnableWindow(GetDlgItem(hDlg, IDC_HOUSE_ID), TRUE);
				else
				{
					ShowWindow(GetDlgItem(hDlg, IDC_HOUSE_ID), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDC_STATIC_HOUSE_ID), SW_HIDE);
				}
					//EnableWindow(GetDlgItem(hDlg, IDC_HOUSE_ID), FALSE);
					*/

				player->SetAvatar(curr_avatar, false);
			}

			switch (LOWORD(wParam))
			{
				case IDC_OK:
					if (player->flags & ACTOR_TRANSFORMED)
					{ // don't allow avatar customization with nightmare form
						PostMessage(hwnd_avatar, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
						break;
					}

					if (old_avatar == curr_avatar)
						curr_avatar = old_avatar;
					else // it's done this way because != is not defined on LmAvatar
						player->SetAvatar(curr_avatar, true);
					if (test_avatar)
						test_avatar->SetAvatar(player->Avatar());
					DestroyWindow(hDlg);

					options.avatar = player->Avatar();
					SaveInGameRegistryOptionValues();
					return TRUE;

				case IDC_RESET:
					if (player->flags & ACTOR_TRANSFORMED)
					{ // don't allow avatar customization with nightmare form
						SendMessage(hwnd_avatar, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
						break;
					}
					curr_avatar = old_avatar;
					player->SetAvatar(curr_avatar, false);
					SetAvatarValues(hDlg);

					return TRUE;

				case IDC_DESCRIBE:
					CreateTextDialog(IDS_AVATAR_DESCRIP_PROMPT,gs->AvatarDescrip(),AvatarDescripCallback,Lyra::MAX_AVATARDESC);
					return TRUE;

				case IDC_CANCEL:
					if (!(player->flags & ACTOR_TRANSFORMED))
						player->SetAvatar(old_avatar, false);
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}
/*
// called when the user presses O.K in the Avatar Describe dialog box
void PmareAvatarDescripCallback(void *new_descrip)
{
	gs->SetAvatarDescrip((TCHAR*)new_descrip);

	if (gs && gs->LoggedIntoGame())
		gs->SendAvatarDescrip();
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////
// Avatar Customization Dialog


BOOL CALLBACK MonsterAvatarDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int i;
	static LmAvatar old_avatar, curr_avatar;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			avatardlg = false;
			hwnd_avatar = NULL;
			break;

		case WM_INITDIALOG:
		{
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			avatardlg = true;
			hwnd_avatar = hDlg;

			cp->SetMode(AVATAR_TAB,false, false);			

			old_avatar = curr_avatar = player->Avatar();

#ifdef PMARE
			for (i=0; i<NUM_MONSTER_BREEDS; i++)
			{
				TCHAR buf[64];
				LoadString(hInstance, monster_breeds[i].type, buf, sizeof(buf));
				ListBox_AddString(GetDlgItem(hDlg, IDC_MONSTER_BREED), buf);
			}
			
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_MONSTER_BREED), 0); // Default to first selection for now
			ShowWindow(GetDlgItem(hDlg, IDC_DESCRIBE), SW_HIDE);		
			ShowWindow(GetDlgItem(hDlg, IDC_MONSTER_COLOR_1), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_MONSTER_COLOR_2), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_MONSTER_EYES), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_MONSTER_REGION0), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_MONSTER_REGION1), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_MONSTER_REGION2), SW_HIDE);
#else
			for (i=0; i<NUM_MONSTER_COLORS; i++){
				ListBox_AddString(GetDlgItem(hDlg, IDC_MONSTER_REGION0), MonsterColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_MONSTER_REGION1), MonsterColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_MONSTER_REGION2), MonsterColorName(i));
			}
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_MONSTER_REGION0), player->Avatar().Color0());
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_MONSTER_REGION1), player->Avatar().Color1());
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_MONSTER_REGION2), player->Avatar().Color2());
			ShowWindow(GetDlgItem(hDlg, IDC_MONSTER_BREED), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_PMARE_TYPE), SW_HIDE);
#endif

			return TRUE;
		}


		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return (LRESULT) 0;
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			}
			break;

		case WM_COMMAND:
			if (((HIWORD(wParam) == LBN_SELCHANGE) || (HIWORD(wParam) == BN_CLICKED)) &&
				!(player->flags & ACTOR_TRANSFORMED))
			{
					
					// Only set description to match color for PMares
#ifdef PMARE
					int chosen_breed = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_MONSTER_BREED));
					LoadString(hInstance,monster_breeds[chosen_breed].descrip, message, sizeof(message)); 
					gs->SetAvatarDescrip(message);
					if (gs && gs->LoggedIntoGame())
						gs->SendAvatarDescrip();
					
					curr_avatar.SetColor0(monster_breeds[chosen_breed].color1);
					curr_avatar.SetColor1(monster_breeds[chosen_breed].color2);

					// set eyes
					curr_avatar.SetColor2(6);
					curr_avatar.SetColor3(7);
#else
					curr_avatar.SetColor0(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_MONSTER_REGION0)));
					curr_avatar.SetColor1(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_MONSTER_REGION1)));
					// set eyes to same color
					curr_avatar.SetColor2(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_MONSTER_REGION2)));
					curr_avatar.SetColor3(ListBox_GetCurSel(GetDlgItem(hDlg, IDC_MONSTER_REGION2)));

#endif

					player->SetAvatar(curr_avatar, false);
			}
			switch (LOWORD(wParam))
			{
				// Jared 6-03-2000
				case IDC_DESCRIBE:
					CreateTextDialog(IDS_AVATAR_DESCRIP_PROMPT,gs->AvatarDescrip(),AvatarDescripCallback,Lyra::MAX_AVATARDESC);
					return TRUE;

				case IDC_OK:
					if (old_avatar == curr_avatar)
						curr_avatar = old_avatar;
					else // it's done this way because != is not defined on LmAvatar
						player->SetAvatar(curr_avatar, true);
					if (test_avatar)
						test_avatar->SetAvatar(player->Avatar());
					DestroyWindow(hDlg);

					options.avatar = player->Avatar();
					SaveInGameRegistryOptionValues();
					return TRUE;

				case IDC_CANCEL:
					if (!(player->flags & ACTOR_TRANSFORMED))
						player->SetAvatar(old_avatar, false);
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// Fatal Error Dialog - modal!

// expects message to hold the error message to display

BOOL CALLBACK FatalErrorDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_INITDIALOG:
			ShowCursor(TRUE);
			if (cDD)
				SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			else
				SetWindowPos(hDlg, TopMost(), 0, 0, 0, 0, SWP_NOSIZE);
			Static_SetText(GetDlgItem(hDlg, IDC_FATAL_ERROR_TEXT), message);
			return TRUE;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			}
			break;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					EndDialog(hDlg, TRUE);
					return TRUE;

				default:
					break;
			}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Nonfatal Error Dialog

// This dialog is unique in that it carries no static state, so
// many of them may be instantiated at one time

// expects message to hold the error message to display

BOOL CALLBACK NonfatalErrorDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_INITDIALOG:
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			Static_SetText(GetDlgItem(hDlg, IDC_NONFATAL_ERROR_TEXT), message);
			SetFocus(hDlg);
			nonfataldlg++;
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		// this one is triggered on keydown so it is not inadvertantly dismissed
		case WM_KEYDOWN:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
					nonfataldlg--;
					DestroyWindow(hDlg);
					return TRUE;

				default:
					break;
			}
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// Warning Yes/No Dialog

// expects message to hold the error message to display

BOOL CALLBACK WarningYesNoDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static dlg_callback_t callback;
	static int response;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			warningyesnodlg = false;
			break;

		case WM_INITDIALOG:
			warningyesnodlg = true;
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			Static_SetText(GetDlgItem(hDlg, IDC_WARNING_TEXT), message);
			callback = NULL;
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_SET_CALLBACK: // called by object waiting for callback
			callback = (dlg_callback_t)lParam;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_WARNINGNO, 0);
#endif
			return TRUE;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_WARNINGNO, 0);
					return (LRESULT) 0;

				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_WARNINGYES, 0);
					return (LRESULT) 0;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_WARNINGYES:
					response = 1;
					if (callback)
						callback(&response);
					DestroyWindow(hDlg);
					return TRUE;

				case IDC_WARNINGNO:
					response = 0;
					if (callback)
						callback(&response);
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Player Mare Confirmation box

// expects message to hold the error message to display

BOOL CALLBACK PMareDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_INITDIALOG:
			{
			TCHAR text[2048];
			SetWindowPos(hDlg, HWND_TOPMOST, 15, 10, 0,0, SWP_NOSIZE);
			TCHAR disp1[1024]; TCHAR disp2[1024]; TCHAR disp3[1024]; 
			LoadString (hInstance, IDS_AGREEMENT_TEXT5, disp2, sizeof(disp2));
			LoadString (hInstance, IDS_AGREEMENT_TEXT6, disp3, sizeof(disp3));
			LoadString (hInstance, IDS_AGREEMENT_TEXT7, message, sizeof(message));
			float price = (float)(options.pmare_price);
			price = price/100;
			_stprintf(disp1, message, price);
			_stprintf(text, _T("%s%s%s"), disp1, disp2, disp3);
			Static_SetText(GetDlgItem(hDlg, IDC_PMARE_TEXT), text);
			return TRUE;
			}

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return (LRESULT) 0;

				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
				{
					TCHAR buffer[10];
					GetWindowText( GetDlgItem(hDlg, IDC_MAX_PMARE_MINUTES), buffer, sizeof(buffer));
					options.pmare_minutes_online = _ttoi(buffer);
//					if ((options.pmare_minutes_online > 0) &&
//						(options.pmare_minutes_online < 15))
//						options.pmare_minutes_online = 15;
					if (options.pmare_minutes_online == 0)
						options.pmare_minutes_online = SHRT_MAX;
					//sprintf(message, "%d", max_minutes);
					//MessageBox(NULL, message, "", MB_OK);
					EndDialog(hDlg, TRUE);
					return TRUE;
				}

				case IDC_CANCEL:
					EndDialog(hDlg, FALSE);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// GM Teleport

// allows gm's to teleport to anywhere

BOOL CALLBACK GMTeleportDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static float last_x = 0.0f;
	static float last_y = 0.0f;
	static int last_level = 0;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			gmteleportdlg = false;
			break;

		case WM_INITDIALOG:
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			gmteleportdlg = true;
			/*_stprintf(message, "%d", (int)last_x);
			Edit_SetText(GetDlgItem(hDlg, IDC_GM_TEL_X), message);
			_stprintf(message, "%d", (int)last_y);
			Edit_SetText(GetDlgItem(hDlg, IDC_GM_TEL_Y), message);
			_stprintf(message, "%d", last_level);
			Edit_SetText(GetDlgItem(hDlg, IDC_GM_TEL_LEVEL), message);*/
			_stprintf(message, _T("%d;%d;%d"),(int)last_x,(int)last_y,last_level);
			Edit_SetText(GetDlgItem(hDlg, IDC_GM_TELEPORT), message);
			return TRUE;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
				{
					/*
					GetWindowText( GetDlgItem(hDlg, IDC_GM_TEL_X), message, sizeof(message));
					float x = (float)_ttoi(message);
					GetWindowText( GetDlgItem(hDlg, IDC_GM_TEL_Y), message, sizeof(message));
					float y = (float)_ttoi(message);
					GetWindowText( GetDlgItem(hDlg, IDC_GM_TEL_LEVEL), message, sizeof(message));
					int level_id = _ttoi(message);*/
					GetWindowText( GetDlgItem(hDlg, IDC_GM_TELEPORT), message, sizeof(message));
					float x,y; int level_id;
					if (_stscanf(message,_T("%f;%f;%d"),&x,&y,&level_id) == 3)
					{
						DestroyWindow(hDlg);
						last_x = x; last_y = y; last_level = level_id;
						player->Teleport(x, y, 0, level_id);
						return TRUE;
					}
					break;
				}

				case IDC_CANCEL:
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
	}
	return FALSE;
}



// dummy dlg proc for sizing fonts, etc.
BOOL CALLBACK DummyDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}



///////////////////////////////////////////////////////////////////////////////////////////////
// Grant PPoints


BOOL CALLBACK GrantPPointDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static bool art_callback;
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			grantppointdlg = false;
			SetFocus(cDD->Hwnd_Main());
			break;

		case WM_INITDIALOG:
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			grantppointdlg = true;
			art_callback = false;
			LoadString (hInstance, IDS_GRANT_PPOINT_DLG, disp_message, sizeof(disp_message));
			SetWindowText(GetDlgItem(hDlg, IDC_GRANT_PPOINT), disp_message);

			return TRUE;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_SET_ART_CALLBACK: // called by art waiting for callback
			art_callback = true;
#ifdef AGENT // always reject
			PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
#endif
			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_OK:
				{
					Edit_GetText(GetDlgItem(hDlg, IDC_WHY_PPOINT), message, sizeof(message));

					if (art_callback)
						(arts->*(grantpp_callback))(message);

					DestroyWindow(hDlg);
					return TRUE;
				}
				break;

				case IDC_CANCEL:
					if (art_callback)
						arts->CancelArt();

					DestroyWindow(hDlg);
					return FALSE;

				case IDC_ITEMHELP:
					if (!ppoint_helpdlg)
					{
						CreateLyraDialog(hInstance, IDD_PPOINT_HELP, cDD->Hwnd_Main(), (DLGPROC)PPointHelpDlgProc);

					}
					break;

				default:

					break;
			}
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// Use PPoints


BOOL CALLBACK UsePPointDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
#ifndef AGENT
	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;
	int i=0;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			useppointdlg = false;
			break;

		case WM_INITDIALOG:
			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			useppointdlg = true;
			pp.cursel = -2;
			pp.sub_cursel = -2;
			pp.cost = 0;

			// 0 = permanent stat increase
			// 1 = PMare time
			// 2 = Use Art
			// 3 = XP
			LoadString (hInstance, IDS_PP_STAT, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_HOW_USE_PP), message);
			LoadString (hInstance, IDS_PP_PMARE_TIME, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_HOW_USE_PP), message);
			LoadString (hInstance, IDS_PP_ART, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_HOW_USE_PP), message);
			LoadString (hInstance, IDS_PP_XP, message, sizeof(message));
			ComboBox_AddString(GetDlgItem(hDlg, IDC_HOW_USE_PP), message);
			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_HOW_USE_PP), 0);
			PostMessage(hDlg, WM_COMMAND,	(WPARAM)MAKEWPARAM(IDC_HOW_USE_PP, LBN_SELCHANGE),	(LPARAM)0); // Windows should send LBN_SELCHANGE out in SetCurSel but does not
			for (i=1; i<10; i++) {
				_stprintf(message, _T("%d"), i*10);
				ComboBox_AddString(GetDlgItem(hDlg, IDC_SKILL), message);
			}
			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_SKILL), 0);

			_stprintf(message, "%d", 0);
			Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), message);
			_stprintf(disp_message, "%d", player->PPoints());
			Edit_SetText(GetDlgItem(hDlg, IDC_PP_CURRENT), disp_message);

			//ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_PP_SUBSEL), pp.sub_cursel);

			return TRUE;


		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return TRUE;
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return TRUE;
			}
			break;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;
	


		case WM_COMMAND:
		{		
			if (!useppointdlg)
				break;
			int message_length, i, j;
			if ((HIWORD(wParam) == LBN_SELCHANGE) ||
				(HIWORD(wParam) == LBN_SELCANCEL))
			{
				if (LOWORD(wParam) == IDC_HOW_USE_PP) 
				{
					// change active PP use
					int oldsel = pp.cursel;
					pp.cursel = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_HOW_USE_PP));
					if (oldsel == pp.cursel) // do nothing
						break;
					if (pp.cursel == -1)
						pp.cursel = 0;

					// hide all windows here

					//ShowWindow(GetDlgItem(hDlg, IDC_), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDC_PP_STATIC), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDC_PP_SUBSEL), SW_HIDE);
					pp.cost = 0;
					pp.sub_cursel = -1;
					_stprintf(message, "%d", pp.cost);
					ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_HIDE);
					Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), message);
					ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_SHOWNORMAL);
					ShowWindow(GetDlgItem(hDlg, IDC_SKILL), SW_HIDE);
					ShowWindow(GetDlgItem(hDlg, IDC_SKILL_PP), SW_HIDE);

					while (ComboBox_GetCount(GetDlgItem(hDlg, IDC_PP_SUBSEL)) > 0)    
					{
						ComboBox_DeleteString(GetDlgItem(hDlg, IDC_PP_SUBSEL),0);
					}


					switch (pp.cursel) 
					{
#if 0 // PP Train - disabled for now!
					case GMsg_UsePPoint::BYPASS_TRAIN: // train
					{
						LoadString (hInstance, IDS_BYPASS_TRAIN_SEL, message, sizeof(message));
						Edit_SetText(GetDlgItem(hDlg, IDC_PP_STATIC), message);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_STATIC), SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_SUBSEL), SW_SHOWNORMAL);
						int trainable_arts = 0;
						for (int i=0; i<NUM_ARTS; i++)
						{
							if (arts->PPMultiplier(i) <= 0)
								continue;
							// add arts that can be learned new, to level 1
							if ((0 == player->Skill(i)) && 
								(arts->MinOrbit(i) <= player->Orbit()) && 
								arts->Learnable(i) &&
								(arts->PPMultiplier(i) > 0) &&
								((i != Arts::FORGE_TALISMAN) || (player->Skill(Arts::DREAMSMITH_MARK) > 0)))
							{	
								LoadString (hInstance, IDS_PP_LEARN_ART, message, sizeof(message));
								_stprintf(disp_message, message, arts->Descrip(i), 1);
								ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), disp_message);
								ComboBox_SetItemData(GetDlgItem(hDlg, IDC_PP_SUBSEL), trainable_arts, i);
								trainable_arts++;
							} // add arts that can be plateau'd
							else if ((9 == (player->Skill(i)%10)) &&
								(player->Skill(i) < MaxSkill(i)) &&
								((i != Arts::FORGE_TALISMAN) || (player->Skill(Arts::DREAMSMITH_MARK) > 0)))	
							{
								LoadString (hInstance, IDS_PP_LEARN_ART, message, sizeof(message));
								_stprintf(disp_message, message, arts->Descrip(i), player->Skill(i)+1);
								ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), disp_message);
								ComboBox_SetItemData(GetDlgItem(hDlg, IDC_PP_SUBSEL), trainable_arts, i);
								trainable_arts++;
							}
						}
						break;
					}
#endif // PP Train
					case GMsg_UsePPoint::BUY_PMARE_TIME:
					{
						int curPP = player->PPoints();
						LoadString (hInstance, IDS_PP_PMARE_TIME_SEL, message, sizeof(message));
						Edit_SetText(GetDlgItem(hDlg, IDC_PP_STATIC), message);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_STATIC), SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_SUBSEL), SW_SHOWNORMAL);
						int numCreds = curPP / PPOINTS_PER_PMARE_CREDIT;
						for(int i = 1; i < numCreds+1; i++)
						{
							_stprintf(disp_message, "%d", i);
							ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), disp_message);
						}
						break;
					}
					case GMsg_UsePPoint::GAIN_XP: // sphere
					{

						LmStats stats;
						stats.SetXP(player->XP());
						if (player->Orbit() == Stats::ORBIT_MAX) {
							LoadString (hInstance, IDS_ORBIT_MAX, message, sizeof(message));
							pp.cost = 0;
						}
						else if (stats.ReadyToAdvance(player->Sphere()+1)) 
						{
							LoadString (hInstance, IDS_CANT_GET_XP, message, sizeof(message));
							pp.cost = 0;
						} else {
							int curr_orbit = player->Orbit();
							int curr_xp = player->XP();
							int max_xp = 0;

							if (curr_orbit % 10 == 9) // Next orbit is a new sphere
								max_xp = LmStats::OrbitXPBase(curr_orbit + 1) - 1;  // Only gain enough to max this sphere
							else max_xp = LmStats::OrbitXPBase(curr_orbit + 1) + 1; // Only gain enough to get next orbit

							ShowWindow(GetDlgItem(hDlg, IDC_PP_SUBSEL), SW_SHOWNORMAL);
							LoadString(hInstance, IDS_GAIN_XP, message, sizeof(message));

							int xp_gain = 0;
							int total_xp_gain = 0;
							int prev_xp;
							for (int i = 1; i <= player->PPoints(); i++)
							{	
								xp_gain = LmStats::XPPPCost(curr_xp+total_xp_gain);
								prev_xp = curr_xp + total_xp_gain;
								if (max_xp <= (prev_xp + xp_gain)) {
									_stprintf(disp_message, "%d", (max_xp - curr_xp));
									ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), disp_message);
									break;
								} else {
									total_xp_gain = total_xp_gain + xp_gain;

									_stprintf(disp_message, "%d", total_xp_gain);
									ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), disp_message);
								}								
							}
						}
						
						Edit_SetText(GetDlgItem(hDlg, IDC_PP_STATIC), message);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_STATIC), SW_SHOWNORMAL);
						break;
					}
#if 0 // PP Sphere - disabled for now
					case GMsg_UsePPoint::BYPASS_SPHERE: // sphere
					{
						LmStats stats;
						stats.SetXP(player->XP());
						if (player->Sphere() == Stats::SPHERE_MAX) {
							LoadString (hInstance, IDS_SPHERE_MAX, message, sizeof(message));
							pp.cost = 0;
						}
						else if (!stats.ReadyToAdvance(player->Sphere()+1)) 
						{
							LoadString (hInstance, IDS_NOT_READY_TO_SPHERE, message, sizeof(message));
							pp.cost = 0;
						} 
						else
						{
							LoadString (hInstance, IDS_BYPASS_SPHERE_SEL, disp_message, sizeof(disp_message));
							_stprintf(message, disp_message, player->Sphere()+1);
							pp.cost = LmStats::SphereIncreaseCost(player->Sphere()+1);
						}
						Edit_SetText(GetDlgItem(hDlg, IDC_PP_STATIC), message);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_STATIC), SW_SHOWNORMAL);
						_stprintf(disp_message, "%d", pp.cost);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_HIDE);
						Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), disp_message);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_SHOWNORMAL);
						break;
					}
#endif 
					case GMsg_UsePPoint::USE_ART: // art
					{
						LoadString (hInstance, IDS_USEART_LABEL, message, sizeof(message));
						Edit_SetText(GetDlgItem(hDlg, IDC_PP_STATIC), message);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_STATIC), SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_SUBSEL), SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_STATIC), SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_SUBSEL), SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg, IDC_SKILL), SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg, IDC_SKILL_PP), SW_SHOWNORMAL);
						int evokable_arts = 0;
						for (int i=0; i<NUM_ARTS; i++)
						{
							// add arts that can be learned new, to level 1
							if (arts->PPMultiplier(i) > 0)
							{	
								ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), arts->Descrip(i));
								ComboBox_SetItemData(GetDlgItem(hDlg, IDC_PP_SUBSEL), evokable_arts, i);
								evokable_arts++;
							} 
						}
						break;
					}
					case GMsg_UsePPoint::STAT_INCREASE: // stat
					default: 
					{
						// LoadString (hInstance, IDS_DREAMSOUL, message, sizeof(message));
						// ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), message);
						LoadString (hInstance, IDS_WILLPOWER, message, sizeof(message));
						ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), message);
						LoadString (hInstance, IDS_INSIGHT, message, sizeof(message));
						ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), message);
						LoadString (hInstance, IDS_RESILIENCE, message, sizeof(message));
						ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), message);
						LoadString (hInstance, IDS_LUCIDITY, message, sizeof(message));
						ComboBox_AddString(GetDlgItem(hDlg, IDC_PP_SUBSEL), message);

						LoadString (hInstance, IDS_STAT_LABEL, message, sizeof(message));
						Edit_SetText(GetDlgItem(hDlg, IDC_PP_STATIC), message);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_STATIC), SW_SHOWNORMAL);
						ShowWindow(GetDlgItem(hDlg, IDC_PP_SUBSEL), SW_SHOWNORMAL);
						break;
					}
					}
				}

				if (LOWORD(wParam) == IDC_SKILL)
				{
					pp.sub_cursel = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_PP_SUBSEL));
					if (pp.sub_cursel == -1)
						break;
					// calculate cost of evocation
					pp.art_id = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_PP_SUBSEL), pp.sub_cursel);
					int plat = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_SKILL));
					pp.skill= (plat+1)*10;
					pp.cost = LmStats::EvokePPCost(pp.art_id, pp.skill, arts->MinOrbit(pp.art_id), arts->PPMultiplier(pp.art_id));
					_stprintf(message, "%d", pp.cost);
					ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_HIDE);
					Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), message);
					ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_SHOWNORMAL);
					break;
				}
				
				if (LOWORD(wParam) == IDC_PP_SUBSEL) 
				{
					switch (pp.cursel) 
					{
#if 0
						case GMsg_UsePPoint::BYPASS_TRAIN: // train
						{
							int oldsel = pp.sub_cursel;
							pp.sub_cursel = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_PP_SUBSEL));
							if (pp.sub_cursel == -1)
								break;
							if (oldsel == pp.sub_cursel) // do nothing
								break;
							// calculate cost of stat increase
							pp.art_id = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_PP_SUBSEL), pp.sub_cursel);
							pp.skill = 1;
							if (player->Skill(pp.art_id) > 0)
								pp.skill = player->Skill(pp.art_id) +1;
							pp.cost = LmStats::TrainPPCost(pp.art_id, pp.skill, arts->MinOrbit(pp.art_id), 1/*arts->PPMultiplier(pp.art_id)*/);
							_stprintf(message, "%d", pp.cost);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_HIDE);
							Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), message);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_SHOWNORMAL);
							break;
						}
#endif
						case GMsg_UsePPoint::GAIN_XP: 
						{
							int oldsel = pp.sub_cursel;
							pp.sub_cursel = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_PP_SUBSEL));
							if (pp.sub_cursel == -1)
								break;
							if (oldsel == pp.sub_cursel)
								break;
							pp.cost = (pp.sub_cursel + 1);
							_stprintf(message, "%d", pp.cost);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_HIDE);
							Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), message);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_SHOWNORMAL);
						}
							break;
#if 0
						case GMsg_UsePPoint::BYPASS_SPHERE: // sphere
							break;
#endif
						case GMsg_UsePPoint::BUY_PMARE_TIME:
						{
							int oldsel = pp.sub_cursel;
							pp.sub_cursel = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_PP_SUBSEL));
							if(pp.sub_cursel == -1)
								break;							
							if(oldsel == pp.sub_cursel)
								break;
							pp.cost = PPOINTS_PER_PMARE_CREDIT * (pp.sub_cursel + 1);
							_stprintf(message, "%d", pp.cost);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_HIDE);
							Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), message);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_SHOWNORMAL);
							break;
						}

						case GMsg_UsePPoint::USE_ART: // art
						{
							int oldsel = pp.sub_cursel;
							pp.sub_cursel = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_PP_SUBSEL));
							if (pp.sub_cursel == -1)
								break;
							if (oldsel == pp.sub_cursel) // do nothing
								break;
							// calculate cost of evocation
							pp.art_id = ComboBox_GetItemData(GetDlgItem(hDlg, IDC_PP_SUBSEL), pp.sub_cursel);
							int plat = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_SKILL));
							pp.skill = (plat+1)*10;
							pp.cost = LmStats::EvokePPCost(pp.art_id, pp.skill, arts->MinOrbit(pp.art_id), arts->PPMultiplier(pp.art_id));
							_stprintf(message, "%d", pp.cost);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_HIDE);
							Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), message);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_SHOWNORMAL);
							break;
						}
						case GMsg_UsePPoint::STAT_INCREASE: 
						{ // stat
							int oldsel = pp.sub_cursel;
							pp.sub_cursel = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_PP_SUBSEL));
							if (pp.sub_cursel == -1)
								break;
							if (oldsel == pp.sub_cursel) // do nothing
								break;
							// calculate cost of stat increase
							pp.cost = LmStats::StatIncreaseCost(player->FocusStat(), pp.sub_cursel + 1, player->Orbit(), player->MaxStat(pp.sub_cursel + 1));
							_stprintf(message, "%d", pp.cost);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_HIDE);
							Edit_SetText(GetDlgItem(hDlg, IDC_PP_COST), message);
							ShowWindow(GetDlgItem(hDlg, IDC_PP_COST), SW_SHOWNORMAL);
						}
					}
				}
			}


			switch (LOWORD(wParam))
			{
				case IDC_OK:
				{
					if (pp.cost > player->PPoints()) 
					{
						LoadString (hInstance, IDS_TOO_MUCH_PP, message, sizeof(message));
						CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);

						break;
					} 
					else if (pp.cost > 0) 
					{ // spend pps! pp.cursel is cost; 
						// sanity check use
						if (pp.cursel == GMsg_UsePPoint::STAT_INCREASE) 
						{
							pp.sub_cursel++;
							if (player->MaxStat(pp.sub_cursel) == (Stats::STAT_MAX)) 
							{
								LoadString (hInstance, IDS_USE_STAT_MAX, message, sizeof(message));
								CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
									cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
								break;
							}
						} 
						else if(pp.cursel == GMsg_UsePPoint::BUY_PMARE_TIME)
							pp.sub_cursel++;
#if 0
						else if (pp.cursel == GMsg_UsePPoint::BYPASS_TRAIN) 
						{
						} 
						else if (pp.cursel == GMsg_UsePPoint::BYPASS_SPHERE) 
						{	// do nothing; checks already made above
						} 
#endif
						else if (pp.cursel == GMsg_UsePPoint::USE_ART) 
						{
							int possible = arts->CanUseArt(pp.art_id, true);
							if (!possible) 
							{ // error string always written into message
								CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
									cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
								break;

							}
						}
						else if (pp.cursel == GMsg_UsePPoint::GAIN_XP)
						{
							pp.sub_cursel++;
						}

						gs->UsePPoints(pp.cursel, pp.sub_cursel, pp.art_id, pp.skill);
					} 
					else 
					{ // finished
						pp.in_use = false;
					}
					
					DestroyWindow(hDlg);
					return TRUE;

				}

				case IDC_ITEMHELP:
					if (!ppoint_helpdlg)
						CreateLyraDialog(hInstance, IDD_PPOINT_HELP, cDD->Hwnd_Main(), (DLGPROC)PPointHelpDlgProc);
					break;	

				case IDC_CANCEL:
					pp.in_use = false;
					DestroyWindow(hDlg);
					return FALSE;

				default:
					break;
			}
		}
	}
#endif // NOT AGENT
	return FALSE;
}

  
///////////////////////////////////////////////////////////////////////////////////////////////
// Agent Controller Login Dialog

#ifdef GAMEMASTER

#define SERVER_MAX 20

BOOL CALLBACK AgentLoginDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	TCHAR server[SERVER_MAX];
	TCHAR agent_username[Lyra::PLAYERNAME_MAX];
	TCHAR agent_password[Lyra::PASSWORD_MAX];
	HKEY reg_key = NULL;
	unsigned long result,size;
	DWORD reg_type;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush;

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			agentdlg = false;
			break;

		case WM_INITDIALOG:
			agentdlg = true;
			SetWindowPos(hDlg, HWND_TOP,
				cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg),	0,0, SWP_NOSIZE);

			if (RegCreateKeyEx(HKEY_CURRENT_USER, AGENT_REGISTRY_KEY ,0,
							NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result)
							!= ERROR_SUCCESS)
			{
#ifdef GAMEMASTER
				ErrAndExit(GENERIC_ERR, _T("Cannot access registry for agent controller"), __LINE__,_T(__FILE__));
#endif
				EndDialog( hDlg, FALSE );
				return FALSE;
			}

			if (result == REG_CREATED_NEW_KEY)
			{
				_tcscpy(agent_username, _T(""));
				_tcscpy(agent_password, _T(""));
				_tcscpy(server, _T(""));
			}
			else if (result == REG_OPENED_EXISTING_KEY)
			{
				size = sizeof(agent_username);
				RegQueryValueEx(reg_key, _T("agent_username"), NULL, &reg_type,
					(unsigned char *)agent_username, &size);
				size = sizeof(agent_password);
				RegQueryValueEx(reg_key, _T("agent_password"), NULL, &reg_type,
					(unsigned char *)agent_password, &size);
				size = sizeof(server);
				RegQueryValueEx(reg_key, _T("server"), NULL, &reg_type,
					(unsigned char *)server, &size);
			}
			RegCloseKey(reg_key);

			SetWindowText( GetDlgItem(hDlg, IDC_AGENT_USERNAME), agent_username );
			SetWindowText( GetDlgItem(hDlg, IDC_AGENT_PASSWORD), agent_password );
			SetWindowText( GetDlgItem(hDlg, IDC_AGENT_SERVER),   server );

			SetFocus(hDlg);

			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;

		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_CANCEL, 0);
					return (LRESULT) 0;
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					return (LRESULT) 0;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_OK:
				GetWindowText( GetDlgItem(hDlg, IDC_AGENT_USERNAME), agent_username, sizeof(agent_username));
				agent_username[Lyra::PLAYERNAME_MAX-1] = '\0';
				GetWindowText( GetDlgItem(hDlg, IDC_AGENT_PASSWORD), agent_password, sizeof(agent_password));
				agent_password[Lyra::PASSWORD_MAX-1] = '\0';
				GetWindowText( GetDlgItem(hDlg, IDC_AGENT_SERVER), server, sizeof(server));
				server[SERVER_MAX-1] = '\0';

				RegCreateKeyEx(HKEY_CURRENT_USER, AGENT_REGISTRY_KEY ,0,
								NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);

				RegSetValueEx(reg_key, _T("agent_username"), 0, REG_SZ,
					(unsigned char *)agent_username, sizeof(agent_username));
				RegSetValueEx(reg_key, _T("agent_password"), 0, REG_SZ,
					(unsigned char *)agent_password, sizeof(agent_password));
				RegSetValueEx(reg_key, _T("server"), 0, REG_SZ,
					(unsigned char *)server, sizeof(server));
				RegCloseKey(reg_key);

				as->Login(agent_username, agent_password, server);

				DestroyWindow(hDlg);

				return TRUE;

			case IDC_CANCEL:
				DestroyWindow(hDlg);
				return FALSE;
			}
			break;
	}
	return FALSE;
}

#endif


