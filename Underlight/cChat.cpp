// cChat: The rich edit control that displays chat and status messages.

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include <memory.h>
#include "cDDraw.h"
#include "cDSound.h"
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

//////////////////////////////////////////////////////////////////
// External Global Variables

extern cDDraw *cDD;
extern cDSound *cDS;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cChat *display; // needed for window proc
extern options_t options;
extern mouse_look_t mouse_look;
extern mouse_move_t mouse_move;
extern HFONT display_font[MAX_RESOLUTIONS]; 

//////////////////////////////////////////////////////////////////
// Constants

const int MAX_LINES=60;
const int VISIBLE_LINES[MAX_RESOLUTIONS] = { 11, 11, 13 };
const int MIN_REPEAT_INTERVAL = 1000; // same message no more than once/sec

// position for chat display area - no ads 
const struct window_pos_t chatPos[MAX_RESOLUTIONS] = 
{ { 0, 300, 480, 179 }, { 0, 375, 600, 223 }, { 0, 480, 768, 286 } };
	
// position for chat display area - with banner
//const struct window_pos_t chatPos = {0, 300, 480, 119}; 


// chat scrolling buttons
struct button_t {
	window_pos_t position[MAX_RESOLUTIONS];
	int			 button_id;
	int			 bitmap_id;
};


const button_t chat_buttons[NUM_CHAT_BUTTONS] = 
{
{ {{ 460, 150, 13, 25 }, { 579, 193, 13, 25 }, { 747, 255, 13, 25 } },
		DDOWN, LyraBitmap::CP_DDOWNA }, // double down button
{ {{ 460, 0, 13, 25 }, { 579, 0, 13, 25 }, { 747, 0, 13, 25 } },
		DUP, LyraBitmap::CP_DUPA },   // double up button
{ {{ 460, 135, 13, 15 }, { 579, 178, 13, 15 }, { 747, 240, 13, 15 } },
		DOWN, LyraBitmap::CP_DOWNA },   // down button
{ {{ 460, 25, 13, 15 }, { 579, 25, 13, 15 }, { 747, 25, 13, 15 } },
		UP, LyraBitmap::CP_UPA }     // up button
};


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
cChat::cChat(int speech_color, int message_color, int bg_color) 
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
	SendMessage(hwnd_richedit, WM_PASSPROC, 0, (LPARAM) lpfn_richedit ); 

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

	// set background color

	this->SetBGColor(bg_color);
 
	currMode = PLAYER_NAME;
	last_system_message_time = 0;
	first_message = true;

#ifndef AGENT
 	chatlog = new cOutput(player->Name(), true, false); // always append
#endif

	return;

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

void cChat::DisplaySpeech(const TCHAR *text, TCHAR *name, int speechType, bool is_player)
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

	if (isEmote == false)
		this->SwitchMode(PLAYER_SPEECH);
	
	SendMessage(hwnd_richedit, EM_REPLACESEL, 0, (LPARAM) speech);

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
	}  

	return CallWindowProc( lpfn_wproc, hwnd, message, wParam, lParam);
} 


// Check invariants

#ifdef CHECK_INVARIANTS

void cChat::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif



