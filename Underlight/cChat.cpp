// cChat: The rich edit control that displays chat and status messages.

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include <memory.h>
#include "cActorList.h"
#include "cDDraw.h"
#include "cDSound.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cChat.h"
#include "Utils.h"
#include "cOutput.h"
#include "cControlPanel.h"
#include "Options.h"
#include "resource.h"
#include "Interface.h"
#include "Mouse.h"
#include "Realm.h"
#include "Dialogs.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern cActorList* actors;
extern cDDraw *cDD;
extern cGameServer* gs;
extern cControlPanel* cp;
extern cChat* display;
extern cDSound *cDS;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cChat *display; // needed for window proc
extern options_t options;
extern mouse_look_t mouse_look;
extern mouse_move_t mouse_move;
extern HFONT display_font[MAX_RESOLUTIONS]; 
extern bool talkdlg;

//////////////////////////////////////////////////////////////////
// Constants

const int MAX_LINES=60;
#ifndef PMARE
const int VISIBLE_LINES[MAX_RESOLUTIONS] = { 9, 9, 10 };
#else
const int VISIBLE_LINES[MAX_RESOLUTIONS] = { 11, 11, 13 };
#endif

const int MIN_REPEAT_INTERVAL = 1000; // same message no more than once/sec

// position for chat display area - no ads 
#ifndef PMARE
const struct window_pos_t chatPos[MAX_RESOLUTIONS] = 
{ { 0, 300, 480, 155 }, { 0, 375, 600, 194 }, { 0, 480, 768, 253 } };

const struct window_pos_t entryPos[MAX_RESOLUTIONS] =
{ { 0, 455, 480, 25 }, { 0, 570, 600, 35 }, { 0, 733, 768, 40 } };
#else
const struct window_pos_t chatPos[MAX_RESOLUTIONS] =
{ { 0, 300, 480, 180 },{ 0, 375, 600, 225 },{ 0, 480, 768, 288 } };
#endif

// chat scrolling buttons
struct button_t {
	window_pos_t position[MAX_RESOLUTIONS];
	int			 button_id;
	int			 bitmap_id;
};

#ifndef PMARE
const button_t chat_buttons[NUM_CHAT_BUTTONS] = 
{
{ {{ 460, 150 - 25, 13, 25 }, { 579, 193 - 30, 13, 25 }, { 747, 255 - 35, 13, 25 } },
		DDOWN, LyraBitmap::CP_DDOWNA }, // double down button
{ {{ 460, 0, 13, 25 }, { 579, 0, 13, 25 }, { 747, 0, 13, 25 } },
		DUP, LyraBitmap::CP_DUPA },   // double up button
{ {{ 460, 135 - 25, 13, 15 }, { 579, 178 - 30, 13, 15 }, { 747, 240 - 35, 13, 15 } },
		DOWN, LyraBitmap::CP_DOWNA },   // down button
{ {{ 460, 25, 13, 15 }, { 579, 25, 13, 15 }, { 747, 25, 13, 15 } },
		UP, LyraBitmap::CP_UPA }     // up button
};
#else
const button_t chat_buttons[NUM_CHAT_BUTTONS] =
{
{ { { 460, 150, 13, 25 },{ 579, 193, 13, 25 },{ 747, 255, 13, 25 } },
	DDOWN, LyraBitmap::CP_DDOWNA }, // double down button
{ { { 460, 0, 13, 25 },{ 579, 0, 13, 25 },{ 747, 0, 13, 25 } },
DUP, LyraBitmap::CP_DUPA },   // double up button
{ { { 460, 135, 13, 15 },{ 579, 178, 13, 15 },{ 747, 240, 13, 15 } },
DOWN, LyraBitmap::CP_DOWNA },   // down button
{ { { 460, 25, 13, 15 },{ 579, 25, 13, 15 },{ 747, 25, 13, 15 } },
UP, LyraBitmap::CP_UPA }     // up button
};

#endif

struct chat_colors_t {
	UINT name;
	int red;
	int green;
	int blue;
};

chat_colors_t chat_colors[NUM_CHAT_COLORS] = {
	{IDS_BLACK, 0x00, 0x00, 0x00 },
	{IDS_WHITE, 0xFF, 0xFF, 0xFF },
	{IDS_LIGHT_BLUE, 0x66, 0x99, 0xCC  },
	{IDS_LIGHT_ORANGE, 0xFF, 0xCC, 0x66 },
	{IDS_ORANGE, 0xFF, 0x99, 0x33  },
	{IDS_DEEP_BLUE, 0x00, 0x66, 0xFF  },
	{IDS_LIGHT_PURPLE, 0xCC, 0x99, 0xFF },
	{IDS_DEEP_PURPLE, 0xCC, 0x00, 0xFF  },
	{IDS_GREEN, 0x00, 0xCC, 0x00  },
	{IDS_DARK_GREEN, 0x00, 0x66, 0x00  },
	{IDS_DARK_ORANGE, 0xCC, 0x33, 0x00  },
	{IDS_DARK_BLUE, 0x00, 0x00, 0x99  },
	{IDS_RED, 0xFF, 0x00, 0x00  },
	{IDS_DARK_RED, 0x99, 0x00, 0x00 },
	{IDS_DARK_GRAY, 0x33, 0x33, 0x33  },
	{IDS_GRAY, 0x99, 0x99, 0x99  }
};

typedef bool (cChat::*speech_callback_t)(TCHAR *value);

struct slash_command_t {
	const TCHAR* command;
	speech_callback_t handler;
};

const int NUM_SLASH_COMMANDS = 13;

slash_command_t slash_commands[NUM_SLASH_COMMANDS] = {
	{ "whisper", &cChat::doWhisper },
	{ "say", &cChat::doTalk },
	{ "me", &cChat::doEmote },
	{ "raw", &cChat::doRaw },
	{ "rp", &cChat::doRPReport },
	{ "cheat", &cChat::doCheatReport },
	{ "bug", &cChat::doBugReport },
	{ "graw", &cChat::doGlobalRaw },
	{ "gtalk", &cChat::doGlobalTalk },
	{ "shout", &cChat::doShout },
	{ "help", &cChat::doHelp },
	{"ping", &cChat::doPing },
	{"macro", &cChat::doMacro },
};

/////////////////////////////////////////////////////////////////
// Helpers
TCHAR* ChatColorName(int id)
{
	LoadString(hInstance, IDS_BLACK+id, temp_message, sizeof(temp_message));
	return temp_message;
}


/////////////////////////////////////////////////////////////////
// Class Defintion


// Constructor
cChat::cChat(int speech_color, int message_color, int whisper_color, int bg_color) 
{
	// The richedit can't be read-only or we won't be able to delete
	// from it. However, the window proc will immediately return the
	// the focus to the main window for any attempted edits. 
	//hwnd_richedit = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_TOPMOST, "RICHEDIT20A", "",
	hwnd_richedit = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_TOPMOST, _T("RICHEDIT"), _T(""),
		WS_CHILD |  ES_MULTILINE | //ES_AUTOVSCROLL  | WS_VSCROLL | 
		WS_BORDER | ES_WANTRETURN,
		chatPos[cDD->Res()].x, chatPos[cDD->Res()].y, 
		chatPos[cDD->Res()].width, chatPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL); 
	
	lpfn_richedit = SubclassWindow(hwnd_richedit, RichEditWProc);
	isTabbing = false;
	autocompleteNeedsQuoting = false;
	SendMessage(hwnd_richedit, WM_PASSPROC, 0, (LPARAM)lpfn_richedit);

	for (int i=0; i<NUM_CHAT_BUTTONS; i++)
	{
		hwnd_chat_buttons[i] = CreateWindow(_T("button"), _T(""),
				WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
				chat_buttons[i].position[cDD->Res()].x, chat_buttons[i].position[cDD->Res()].y, 
				chat_buttons[i].position[cDD->Res()].width, chat_buttons[i].position[cDD->Res()].height,
				hwnd_richedit, NULL, hInstance, NULL);
		chat_buttons_bitmaps[i][0] = // a button
			CreateWindowsBitmap(chat_buttons[i].bitmap_id);
		chat_buttons_bitmaps[i][1] = // b button
			CreateWindowsBitmap(chat_buttons[i].bitmap_id + 1);
	}

 
	// Set up paragraph format for player speech.

	chatPF.cbSize = sizeof(chatPF);
	chatPF.dwMask = PFM_ALIGNMENT | PFM_OFFSET | PFM_OFFSETINDENT | PFM_RIGHTINDENT;
	chatPF.wAlignment = PFA_LEFT;
	chatPF.dxRightIndent = 175;
	chatPF.dxStartIndent = 0;
	chatPF.dxOffset = 200; 

	SendMessage(hwnd_richedit, EM_SETPARAFORMAT, 0, (LPARAM)&chatPF);


	SendMessage(hwnd_richedit, WM_SETFONT, WPARAM(display_font[cDD->Res()]), 0);

	// Initialize character format structures

	// player chat, player name, and message character formats
	this->SetSpeechFormat(speech_color);
	this->SetMessageFormat(message_color);
	this->SetWhisperFormat(whisper_color);
	// set background color

	this->SetBGColor(bg_color);
 
	currMode = PLAYER_NAME;
	last_system_message_time = 0;
	first_message = true;

#ifndef PMARE
	hwnd_textentry = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_TOPMOST, "RICHEDIT", "",
		WS_CHILD | ES_AUTOHSCROLL | WS_BORDER | ES_WANTRETURN,
		entryPos[cDD->Res()].x, entryPos[cDD->Res()].y,
		entryPos[cDD->Res()].width, entryPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL
		);
	const TCHAR CHAT_FONT_NAME[16] = _T("Arial");
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));

	logFont.lfHeight = 18;
	logFont.lfWidth = 0;
	logFont.lfWeight = 750;
	logFont.lfEscapement = 0;
	logFont.lfItalic = 0;
	logFont.lfUnderline = 0;
	logFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	logFont.lfClipPrecision = CLIP_STROKE_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	_tcscpy(logFont.lfFaceName, CHAT_FONT_NAME);

	// set the control to use this font
	entryfont = CreateFontIndirect(&logFont);
	lpfn_entry = SubclassWindow(hwnd_textentry, EntryWProc);
	SendMessage(hwnd_textentry, WM_PASSPROC, 0, (LPARAM)lpfn_entry);
	SendMessage(hwnd_textentry, WM_SETFONT, WPARAM(entryfont), 0);
	ShowWindow(hwnd_textentry, SW_SHOWNORMAL);
	BGColor = RGB(0x00, 0x33, 0x66);
	FGColor = RGB(chat_colors[4].red, chat_colors[4].green, chat_colors[4].blue);
	SendMessage(hwnd_textentry, EM_SETBKGNDCOLOR, (WPARAM)FALSE, (LPARAM)BGColor);
	CHARFORMAT2 cf;
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = FGColor; //The text color
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwEffects = { 0 };
	SendMessageA(hwnd_textentry, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
#endif

#ifndef AGENT
 	chatlog = new cOutput(player->Name(), true, false); // always append
#endif

	return;
}

bool cChat::doHelp(TCHAR* help)
{
	display->DisplayMessage("/bug - Submits a bug report. Example: /bug <report>");
	display->DisplayMessage("/cheat - Submits a cheat report. Example: /cheat <report>");
	display->DisplayMessage("/macro - Opens the macro dialog.");
	display->DisplayMessage("/me - Sends an emote. Example: /me smiles");
	display->DisplayMessage("/rp - Sends a roleplaying report. Example: /rp <report>");
	display->DisplayMessage("/shout - SHOUTS to the entire area. Example: /shout Can anyone hear me?!");
	display->DisplayMessage("/whisper <Target> - whispers to the target. Example: /whisper Joe Hi, Joe!");
	display->DisplayMessage("\tNote: For dreamers with spaces in their names, you must surround their names\r\twith quotes, i.e. /whisper \"Joe Bloggs\" Hi Joe!");
	display->DisplayMessage("\tYou can type any number of letters of your target's name and hit the TAB key\r\tfor autocompletion.");
	display->DisplayMessage("/say: Talks to the entire area. Alternatively, you can simply type and hit enter.");
	display->DisplayMessage("/ping: Sends a ping message to the server.");
	return true;
}

bool cChat::doMacro(TCHAR* message)
{
	if (!talkdlg) {
		// open the macro dialog
		CreateLyraDialog(hInstance, IDD_CHAT_MACROS, cDD->Hwnd_Main(), (DLGPROC)ChatMacrosDlgProc);
		return true;
	}

	return false;
}

bool cChat::doPing(TCHAR* unused)
{
	gs->PingServer();
	return true;
}

bool cChat::doWhisper(TCHAR* whisper)
{
	TCHAR target[Lyra::PLAYERNAME_MAX] = { 0 };
	char endChar = ' ';
	if (*whisper == '\"')
	{
		whisper++;
		endChar = '\"';
	}

	int i = 0;
	bool bad = false;
	while (*whisper) 
	{
		if (i >= (Lyra::PLAYERNAME_MAX - 1)) 
		{
			bad = true;
			break;
		}

		if (*whisper == endChar)
		{
			whisper++;
			break;
		}

		target[i++] = *whisper++;
	}

	if (bad)
		return false;
	target[i] = 0;
	while (*whisper == ' ')
		whisper++;
	cNeighbor* n;
	bool whispered = false;
	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
	{
		if ((n->Avatar().Hidden()) && (player->ID() != n->ID()))
			continue;
		if (strnicmp(target, n->Name(), strlen(target)) == 0)
		{
			if (n->CanWhisper())
			{
				gs->Talk(whisper, RMsg_Speech::WHISPER, n->ID(), true, true, false);
				whispered = true;
			}
			else
			{
				LoadString(hInstance, IDS_NEIGHBOR_TOO_FAR, message, sizeof(message));
				display->DisplayMessage(message);
				//SetActiveWindow(cDD->Hwnd_Main());
				//SetFocus(cDD->Hwnd_Main());
				//CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
				//	cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
			}
			break;
		}
	}
	actors->IterateNeighbors(DONE);
	if (!whispered)
		return false;
#ifndef GAMEMASTER // GM's don't sent out whisper emote
	int show_emote = rand() % 10;
	if (0 == show_emote)
	{
		LoadString(hInstance, IDS_WHISPER_EMOTE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
		gs->Talk(message, RMsg_Speech::WHISPER_EMOTE, n->ID(), false);
	}
#endif // gamemaster.
	return true;
}

bool cChat::HandleReturn(TCHAR* sentence)
{
	size_t message_length = strlen(sentence);
	if (!message_length)
		return false;
	// strip off extra returns at the end of a message
	message_length = strlen(sentence);
	for (size_t i = message_length - 2; i > 0; i--)
	{
		if (sentence[i] == VK_RETURN)
			sentence[i] = '\0';
		else
			break;
	}

	while (*sentence == ' ')
		sentence++;
	if (*sentence == '/')
	{
		sentence++;
		for (int i = 0; i < NUM_SLASH_COMMANDS; i++)
		{
			slash_command_t sc = slash_commands[i];
			if (strnicmp(sc.command, sentence, strlen(sc.command)) == 0)
			{
				speech_callback_t callback = sc.handler;
				sentence += strlen(sc.command);
				while (*sentence == ' ')
					sentence++;
				return (this->*callback)(sentence);
			}
		}
	}
	else {
		return doTalk(sentence);
	}
}

bool cChat::doTalk(TCHAR* talk)
{
	if (!(*talk))
		return false;
	if (player->Avatar().Hidden())
		return false;
	gs->Talk(talk, RMsg_Speech::SPEECH, Lyra::ID_UNKNOWN);
	return true;
}

bool cChat::doRPReport(TCHAR* rp)
{
	if (!(*rp))
		return false;
	gs->Talk(rp, RMsg_Speech::RP, Lyra::ID_UNKNOWN, true);
	return true;
}

bool cChat::doCheatReport(TCHAR* cheat)
{
	if (!(*cheat))
		return false;
	gs->Talk(cheat, RMsg_Speech::REPORT_CHEAT, Lyra::ID_UNKNOWN, true);
	return true;
}

bool cChat::doBugReport(TCHAR* bug)
{
	if (!(*bug))
		return false;
	static char winVer[50];
	strcat(message, bug);
	getWindowsVersion(winVer);
	strcat(message, "\n");
	strcat(message, winVer);
	gs->Talk(message, RMsg_Speech::REPORT_BUG, Lyra::ID_UNKNOWN, true);
	return true;
}

bool cChat::doEmote(TCHAR* emote)
{
	if (!(*emote))
		return false;
	if (player->Avatar().Hidden())
		return false;
	// make sure player cannot emote as soulsphere by entering talk dlg and selecting emote
	if ((player->flags & ACTOR_SOULSPHERE))
	{
		LoadString(hInstance, IDS_SOULSPHERE_NO_EMOTE, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		return false;
	}
	gs->Talk(emote, RMsg_Speech::EMOTE, Lyra::ID_UNKNOWN);
	return true;
}

bool cChat::doRaw(TCHAR* raw)
{
#ifdef GAMEMASTER
	if (!(*raw))
		return false;
	gs->Talk(raw, RMsg_Speech::RAW_EMOTE, Lyra::ID_UNKNOWN);
	return true;
#else
	return false;
#endif
}

bool cChat::doShout(TCHAR* shout)
{
	if (!(*shout))
		return false;
	if (player->Avatar().Hidden())
		return false;
	gs->Talk(shout, RMsg_Speech::SHOUT, Lyra::ID_UNKNOWN);
	return true;
}

bool cChat::doGlobalRaw(TCHAR* graw)
{
#ifdef GAMEMASTER
	if (!(*graw))
		return false;
	gs->Talk(graw, RMsg_Speech::RAW_EMOTE, Lyra::ID_UNKNOWN, false, true, true);
	return true;
#else
	return false;
#endif
}

bool cChat::doGlobalTalk(TCHAR* gtalk)
{
#ifdef GAMEMASTER
	if (!(*gtalk))
		return false;
	gs->Talk(gtalk, RMsg_Speech::SPEECH, Lyra::ID_UNKNOWN, false, true, true);
	return true;
#else
	return false;
#endif
}

void cChat::Show(void) 
{ 
	ShowWindow( hwnd_richedit, SW_SHOWNORMAL );
	for (int j=0; j<NUM_LV_BUTTONS; j++)
			ShowWindow(hwnd_chat_buttons[j], SW_SHOWNORMAL);
			//ShowWindow(hwnd_chat_buttons[j], SW_HIDE);
	InvalidateRect(hwnd_richedit, NULL, TRUE);
};

void cChat::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_richedit, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_richedit, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_richedit, NULL, TRUE);
}

void cChat::ScrollDown(int count)
{
	int line_count, first_visible, curr_count = count;
	line_count = SendMessage(hwnd_richedit, EM_GETLINECOUNT, 0, 0);
	if (line_count <= VISIBLE_LINES[cDD->Res()])
		return; // no scrolling until necessary

	first_visible = SendMessage(hwnd_richedit, EM_GETFIRSTVISIBLELINE, 0, 0); 

	while (curr_count && (line_count - first_visible > VISIBLE_LINES[cDD->Res()]))
	{
		SendMessage(hwnd_richedit, WM_VSCROLL, (WPARAM)SB_LINEDOWN, 0);
		curr_count--;
		first_visible++;
	}
	InvalidateRect(hwnd_richedit, NULL, TRUE);
	return;
}

void cChat::SetSpeechFormat(int color)
{
	if ((color < 0) || (color >= NUM_CHAT_COLORS))
		color = 0;

	memset(&speechCF, 0, sizeof(speechCF));
	speechCF.cbSize = sizeof(speechCF);
	speechCF.dwMask = CFM_FACE | CFM_ITALIC | CFM_BOLD | CFM_COLOR;
	speechCF.dwEffects = 0;  
	speechCF.crTextColor = (RGB(chat_colors[color].red, chat_colors[color].green, chat_colors[color].blue));
_stprintf(speechCF.szFaceName, FONT_NAME);
	SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&speechCF);

	memset(&emoteCF, 0, sizeof(emoteCF));
	emoteCF.cbSize = sizeof(emoteCF);
	emoteCF.dwMask = CFM_FACE | CFM_ITALIC | CFM_BOLD | CFM_COLOR;
	emoteCF.dwEffects = CFM_ITALIC;  
	emoteCF.crTextColor = (RGB(chat_colors[color].red, chat_colors[color].green, chat_colors[color].blue));
_stprintf(emoteCF.szFaceName, FONT_NAME);
	SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&emoteCF);

	memset(&nameCF, 0, sizeof(nameCF));
	nameCF.cbSize = sizeof(nameCF);
	nameCF.dwMask = CFM_ITALIC | CFM_BOLD | CFM_FACE | CFM_COLOR;
	nameCF.dwEffects = CFE_BOLD; 
	nameCF.crTextColor = (RGB(chat_colors[color].red, chat_colors[color].green, chat_colors[color].blue));
_stprintf(nameCF.szFaceName, FONT_NAME);
	SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&nameCF);
}

void cChat::SetWhisperFormat(int color)
{
	if ((color < 0) || (color >= NUM_CHAT_COLORS))
		color = 0;

	memset(&whisperCF, 0, sizeof(whisperCF));
	whisperCF.cbSize = sizeof(whisperCF);
	whisperCF.dwMask = CFM_FACE | CFM_ITALIC | CFM_BOLD | CFM_COLOR;
	whisperCF.dwEffects = 0;
	whisperCF.crTextColor = (RGB(chat_colors[color].red, chat_colors[color].green, chat_colors[color].blue));
	_stprintf(whisperCF.szFaceName, FONT_NAME);
	SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&whisperCF);
}

void cChat::SetMessageFormat(int color)
{
	if ((color < 0) || (color >= NUM_CHAT_COLORS))
		color = 0;

	memset(&systemCF, 0, sizeof(systemCF));
	systemCF.cbSize = sizeof(systemCF);
	systemCF.dwMask = CFM_ITALIC | CFM_FACE | CFM_BOLD | CFM_COLOR; 
	systemCF.dwEffects = CFE_BOLD;  
	systemCF.crTextColor = (RGB(chat_colors[color].red, chat_colors[color].green, chat_colors[color].blue));
_stprintf(systemCF.szFaceName, FONT_NAME);

	SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&systemCF);
}

void cChat::SetBGColor(int color)
{
	if ((color < 0) || (color >= NUM_CHAT_COLORS))
		color = 0;

	BGColor =	RGB(chat_colors[color].red, chat_colors[color].green, chat_colors[color].blue);
	SendMessage(hwnd_richedit, EM_SETBKGNDCOLOR, (WPARAM) FALSE,	(LPARAM) BGColor); 
}


// Switch to the appropriate paragraph format and character format for
// the desired mode. 

void cChat::SwitchMode(int mode)
{
	switch (mode)
	{
		case PLAYER_SPEECH:
			SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&speechCF);
			currMode = PLAYER_SPEECH;		
			break;

		case PLAYER_NAME:
			SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&nameCF);
			currMode = PLAYER_NAME;
			break;

		case SYSTEM_MESSAGE:
			SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&systemCF);
			currMode = SYSTEM_MESSAGE; 
			break;

		case EMOTE:
			SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&emoteCF);
			currMode = EMOTE; 
			break;

		case WHISPER:
			SendMessage(hwnd_richedit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&whisperCF);
			currMode = WHISPER;
			break;
	}

	currMode = mode;
	return;
}

// called for setup by displaymessage & displayspeech
void cChat::PreDisplay(void)
{
	old_line_count = SendMessage(hwnd_richedit, EM_GETLINECOUNT, 0, 0);

	// offset marks the end of the edit; insert the message
	offset = SendMessage(hwnd_richedit, EM_LINEINDEX, old_line_count - 1, 0) +
		SendMessage(hwnd_richedit, EM_LINELENGTH, old_line_count, 0) + 2;
//		SendMessage(hwnd_richedit, EM_LINELENGTH, old_line_count, 0) + 1;
	SendMessage(hwnd_richedit, EM_SETSEL, (WPARAM) offset, (LPARAM) offset);

	// first print the newline
	if (!first_message)
	{
		SendMessage(hwnd_richedit, EM_SETSEL, (WPARAM) -1, (LPARAM) -1 );
		SendMessage(hwnd_richedit, EM_REPLACESEL, 0, (LPARAM) "\n");
	}
	first_message = false;

	SendMessage(hwnd_richedit, EM_SETPARAFORMAT, 0, (LPARAM)&chatPF);

	return;
}

// called after new text is added to clean up old lines & scroll appropriately
void cChat::PostDisplay(void)
{
	new_line_count = SendMessage(hwnd_richedit, EM_GETLINECOUNT, 0, 0);
	// Now check to see if we need to delete messages.
	while (SendMessage(hwnd_richedit, EM_GETLINECOUNT, 0, 0) > MAX_LINES) 
	{	// delete line 0
		offset = SendMessage(hwnd_richedit, EM_LINEINDEX, 1, 0);
		//offset = SendMessage(hwnd_richedit, EM_LINEINDEX, 1, 0) + 1;
		SendMessage(hwnd_richedit, EM_SETSEL, (WPARAM) 0, (LPARAM) offset);
		SendMessage(hwnd_richedit, WM_CLEAR, 0, 0);
	}

	if (options.extra_scroll)
		new_line_count=new_line_count+2;

	// scroll down if lines were added
	if (new_line_count > VISIBLE_LINES[cDD->Res()])
		for (int i=old_line_count; i<new_line_count; i++)
			SendMessage(hwnd_richedit, WM_VSCROLL, (WPARAM)SB_LINEDOWN, 0);
			
	InvalidateRect(hwnd_richedit, NULL, TRUE);
	return;
}

// Displays a new system message on the screen
void cChat::DisplayMessage(const TCHAR *text, bool sound)
{
	TCHAR speech[DEFAULT_MESSAGE_SIZE]; 

	static int count = 0;

	count++;
	if (count == 3)
	{ 
		count++;
		count--;
	}

	if (((LyraTime() - last_system_message_time) < MIN_REPEAT_INTERVAL) &&
		(_tcslen(last_system_message) > 2) &&
		(_tcscmp(message, last_system_message) == 0))
		return;

	if (sound)
		cDS->PlaySound(LyraSound::MESSAGE);

	this->PreDisplay(); // set line counts, offset correctly

	// now rip the last newline out of the message
	//_tcsnccpy(speech, text, sizeof(speech));
	_tcscpy(speech, text);
	int len = _tcslen(speech);
	for (int i=_tcslen(speech)-1; i >= 0; i--)
		if (speech[i] == '\n')
		{
			speech[i] = '\0';
			break;
		}

	this->SwitchMode(SYSTEM_MESSAGE);
	SendMessage(hwnd_richedit, EM_REPLACESEL, 0, (LPARAM) speech);
	_tcscpy(last_system_message, speech);
	last_system_message_time = LyraTime();

#ifndef AGENT
	if (options.log_chat)
	{
		chatlog->Write(speech);
		chatlog->Write(_T("\n")); // add cr
	}

	this->PostDisplay();
#endif

	return;
}

void cChat::DisplaySpeech(const TCHAR *text, TCHAR *name, int speechType, bool is_player, bool isUniversal)
{
	TCHAR speech[DEFAULT_MESSAGE_SIZE]; 

	bool isEmote = ((speechType == RMsg_Speech::EMOTE) || 
					(speechType == RMsg_Speech::RAW_EMOTE) ||					
					(speechType == RMsg_Speech::WHISPER_EMOTE));

	if ( isEmote && (player->flags & ACTOR_BLINDED))
		return;
	if (!isEmote && (player->flags & ACTOR_DEAFENED))
		return;
	

	this->PreDisplay(); // set line counts, offset correctly

	_tcsnccpy(speech, text, sizeof(speech));

	if (isEmote)
		this->SwitchMode(EMOTE);
	else if (speechType == RMsg_Speech::WHISPER)
		this->SwitchMode(WHISPER);
	else
		this->SwitchMode(PLAYER_NAME);

	// fill in other stuff for name in disp_message
	switch (speechType)
	{
		case RMsg_Speech::GLOBALSHOUT: // got global shout
		case RMsg_Speech::SHOUT: // got shout
			if (is_player)
			_stprintf(message, _T("%s: "), name);
			else
			{
				LoadString (hInstance, IDS_SPEAK_SHOUTED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, name);
			}
			break;
		case RMsg_Speech::WHISPER_EMOTE:
		case RMsg_Speech::EMOTE:
			if (speech[0] == '\'')
				_stprintf(message, _T(">%s"), name);
			else
				_stprintf(message, _T(">%s "), name);
			break;
		case RMsg_Speech::RAW_EMOTE:
		_stprintf(message, _T(""));
			break;
		case RMsg_Speech::SPEECH: // got speech
		_stprintf(message, _T("%s: "),name);
			break;
		case RMsg_Speech::WHISPER: // got whisper
			if (is_player)
			_stprintf(message, _T("%s: "), name);
			else
			{
				LoadString (hInstance, IDS_SPEAK_WHISPERED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, name);
			}
			break;
		default:
				_tcscpy(message, name);
			break;
		}
	SendMessage(hwnd_richedit, EM_SETSEL, (WPARAM) -1, (LPARAM) -1 );

	if (speechType != RMsg_Speech::RAW_EMOTE)
	SendMessage(hwnd_richedit, EM_REPLACESEL, 0, (LPARAM) message);
#ifndef AGENT
	if (options.log_chat && speechType != RMsg_Speech::RAW_EMOTE)
		chatlog->Write(message);
#endif

	if (isEmote == false && speechType != RMsg_Speech::WHISPER)
		this->SwitchMode(PLAYER_SPEECH);
	
	SendMessage(hwnd_richedit, EM_REPLACESEL, 0, (LPARAM) speech);

	if (RMsg_Speech::WHISPER == speechType)
	{
		FLASHWINFO flash;
		flash.cbSize = sizeof(flash);
		flash.hwnd = cDD->Hwnd_Main();
		flash.dwFlags = FLASHW_TIMERNOFG;
		flash.dwTimeout = 0;
		flash.uCount = 5;
		FlashWindowEx(&flash);
	}

#ifndef AGENT
	if (options.log_chat)
	{
		chatlog->Write(speech);
		chatlog->Write(_T("\n")); // add cr
	}

	this->PostDisplay(); // clean up extra lines
#endif

	return;
}

typedef char name_t[Lyra::PLAYERNAME_MAX + 2]; // + 2 for quotes.
name_t names[100];
void cChat::OnTabKeypress()
{
	static char sentence[Lyra::MAX_SPEECHLEN - Lyra::PLAYERNAME_MAX];
	static char textToComplete[Lyra::MAX_SPEECHLEN - Lyra::PLAYERNAME_MAX];
	static int autocompleteChoices = 0;
	static int next = -1;
	static int prevBreak;
	static DWORD firstChar, lastChar;
	autocompleteNeedsQuoting = false;
	if (!isTabbing)
	{
		isTabbing = true;
		autocompleteChoices = 0;
		strcpy(textToComplete, "\0");
		strcpy(sentence, "\0");
		next = -1;
	
		GetWindowText(hwnd_textentry, sentence, Lyra::MAX_SPEECHLEN - Lyra::PLAYERNAME_MAX - 2);
		SendMessage(hwnd_textentry, EM_GETSEL, (WPARAM)&firstChar, (LPARAM)&lastChar);
		prevBreak = firstChar;
		while (sentence[prevBreak] != ' ' && prevBreak != 0)
			prevBreak--;
		int prevPrevBreak = prevBreak - 1;
		if (prevPrevBreak < 0)
			prevPrevBreak = 0;
		else {
			while (sentence[prevPrevBreak] != ' ' && prevPrevBreak != 0)
				prevPrevBreak--;
		}

		int nextBreak = prevBreak + 1;
		while (sentence[nextBreak] != ' ' && sentence[nextBreak])
			nextBreak++;

		if (prevBreak != 0)
			prevBreak++;
		strncpy(textToComplete, sentence + prevBreak, (nextBreak - prevBreak) + 1);
		if (prevPrevBreak == 0 && strnicmp("/whisper", sentence, 8) == 0)
			autocompleteNeedsQuoting = true;
		
		cNeighbor* n;
		for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
		{
			if ((n->Avatar().Hidden()) && (player->ID() != n->ID()))
				continue;
			if (strnicmp(textToComplete, n->Name(), strlen(textToComplete)) == 0)
			{				
				if (autocompleteNeedsQuoting && strchr(n->Name(), ' '))
					sprintf(names[autocompleteChoices], "\"%s\"", n->Name());
				else
					strcpy(names[autocompleteChoices], n->Name());
				autocompleteChoices++;
			}
		}
		actors->IterateNeighbors(DONE);
		strcpy(names[autocompleteChoices], textToComplete);
		autocompleteChoices++;
	}
	next++;
	next %= autocompleteChoices;
	SendMessage(hwnd_textentry, EM_GETSEL, (WPARAM)&firstChar, (LPARAM)&lastChar);
	SendMessage(hwnd_textentry, EM_SETSEL, (WPARAM)prevBreak, (LPARAM)firstChar);
	SendMessage(hwnd_textentry, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)names[next]);
}

// Destructor
cChat::~cChat(void)
{
	for (int j=0; j<NUM_CHAT_BUTTONS; j++)
		for (int k=0; k<2; k++)
			if (chat_buttons_bitmaps[j][k])
				DeleteObject(chat_buttons_bitmaps[j][k]);

#ifndef AGENT
	if (chatlog)
		delete chatlog;
#endif

	return;
}


// Subclassed window procedure for the rich edit control
LRESULT WINAPI RichEditWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	static WNDPROC lpfn_wproc;
    LPDRAWITEMSTRUCT lpdis; 
    // HDC dc; 
	int j;

	switch(message)
	{
		case WM_PASSPROC:
			lpfn_wproc = (WNDPROC) lParam;
			return (LRESULT) 0;

		case WM_SETFOCUS:   // Ensures that the focus is never here
		case WM_KILLFOCUS:  
			return 0;

		case WM_KEYUP:
		case WM_KEYDOWN: // send the key to the main window
		case WM_CHAR:
			SendMessage(cDD->Hwnd_Main(), message,
				(WPARAM) wParam, (LPARAM) lParam);
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			return (LRESULT) 0;
		case WM_MOUSEMOVE:
			if (mouse_look.looking || mouse_move.moving)
			{
				StopMouseMove();
				StopMouseLook();
			}
			break;
		case WM_COMMAND:
			for (j=0; j<NUM_CHAT_BUTTONS; j++)
			{
				if ((HWND)lParam == display->hwnd_chat_buttons[j])
				{
					int scroll_amount = 1;
					if ((j == DDOWN) || (j == DUP))
						scroll_amount = 5;
					if ((j == DDOWN) || (j == DOWN))
						display->ScrollDown(scroll_amount);
					else
						display->ScrollUp(scroll_amount);
					break;
				}
			}
			SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			break;
		case WM_DRAWITEM: 
				lpdis = (LPDRAWITEMSTRUCT) lParam; 
		//	dc = CreateCompatibleDC(lpdis->hDC); 
			/* 
			for (j=0; j<NUM_CHAT_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == display->hwnd_chat_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, display->chat_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == display->hwnd_chat_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, display->chat_buttons_bitmaps[j][1]); 
			}
			
			   BitBlt(lpdis->hDC,lpdis->rcItem.left, lpdis->rcItem.top,  
			lpdis->rcItem.right - lpdis->rcItem.left, 
			lpdis->rcItem.bottom - lpdis->rcItem.top, 
			dc, 0, 0, SRCCOPY); */
			for (j=0; j<NUM_CHAT_BUTTONS; j++)
			{
				if (lpdis->hwndItem == display->hwnd_chat_buttons[j])  
					break;
			}
			SetBkColor(lpdis->hDC,display->GetBGColor() );
			TransparentBlitBitmap(lpdis->hDC, chat_buttons[j].bitmap_id + !(lpdis->itemState & ODS_SELECTED),
														&lpdis->rcItem, NOSTRETCH);

			//DeleteDC(dc); 
			return TRUE; 

	    case WM_SETCURSOR:
			return 0;
		case WM_MOUSEWHEEL: {
			Realm_OnMouseWheelScroll(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (short)HIWORD(wParam));
			return (LRESULT)0;
		}
		break;
	}  

	return CallWindowProc( lpfn_wproc, hwnd, message, wParam, lParam);
} 

// Subclassed window procedure for the rich edit control
LRESULT WINAPI EntryWProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static WNDPROC lpfn_wproc;
	static char sentence[Lyra::MAX_SPEECHLEN - Lyra::PLAYERNAME_MAX];
	static HFONT hEditFont;

	if (HBRUSH brush = SetControlColors(hwnd, message, wParam, lParam))
		return (LRESULT)brush;

	switch (message)
	{
	case WM_SETFOCUS:
	{
		CHARRANGE ichCharRange;
		ichCharRange.cpMin = -1;
		ichCharRange.cpMax = -1;
		SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&ichCharRange);
		break;
	}
	case WM_PASSPROC:
	{
		lpfn_wproc = (WNDPROC)lParam;
		return (LRESULT)0;
	}

	case WM_INITDIALOG:
	{
		Edit_LimitText(hwnd, (Lyra::MAX_SPEECHLEN / 2) - Lyra::PLAYERNAME_MAX - 3);
		return TRUE;
	}	//		case WM_KEYDOWN:
	case WM_KEYUP:

		if (wParam == VK_ESCAPE)
			SendMessage(cp->Hwnd_CP(), WM_COMMAND, 0, (LPARAM)cp->Hwnd_Meta());
		break;
	case WM_CHAR:
	{
		if (wParam == VK_TAB)
		{
			display->OnTabKeypress();
			return 0;
		}
		else
			display->isTabbing = false;
		switch (wParam)
		{
			case VK_RETURN:
			{
				GetWindowText(hwnd, sentence, Lyra::MAX_SPEECHLEN - Lyra::PLAYERNAME_MAX - 2);
				if (display->HandleReturn(sentence))
				{
					Edit_SetText(hwnd, _T(""));
				}

				CHARRANGE ichCharRange;
				ichCharRange.cpMin = 0;
				ichCharRange.cpMax = -1;
				SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&ichCharRange);

				//					SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE, 
				//							(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
				return TRUE;
			}
		}
	}
	}
	return CallWindowProc(lpfn_wproc, hwnd, message, wParam, lParam);
}

// Check invariants

#ifdef CHECK_INVARIANTS

void cChat::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif



