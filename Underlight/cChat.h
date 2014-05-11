// Header file for cChat Class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CHAT_H
#define CHAT_H

#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include "Central.h"
#include "Rmsg_Speech.h"

class cOutput;

//////////////////////////////////////////////////////////////////
// Constants

const int NUM_CHAT_COLORS = 16;
const int NUM_CHAT_BUTTONS = 4;

enum // current paragraph format
{
    PLAYER_SPEECH, PLAYER_NAME, SYSTEM_MESSAGE, EMOTE
};


//////////////////////////////////////////////////////////////////
// Friends & Helpers

TCHAR* ChatColorName(int id);

LRESULT WINAPI RichEditWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


//////////////////////////////////////////////////////////////////
// Class Definition


class cChat
{
public: 

private:
	HWND hwnd_richedit; // handle to rich edit control
	HWND hwnd_chat_buttons[NUM_CHAT_BUTTONS]; // scrolling buttons
	WNDPROC	lpfn_richedit; // pointer to window procedure
	HBITMAP chat_buttons_bitmaps[NUM_CHAT_BUTTONS][2];
	PARAFORMAT chatPF; // paragraph format
	CHARFORMAT speechCF, nameCF, systemCF, emoteCF; // character formats for other player's speech, names, and system messages
	cOutput *chatlog; // for logging chat
	int currMode;		     // current mode - player speech, system, etc.
	bool first_message; // has a message been displayed yet?
	TCHAR last_system_message[DEFAULT_MESSAGE_SIZE];
	DWORD last_system_message_time;
	int old_line_count, new_line_count, offset; // for display
	COLORREF BGColor;

public:
    cChat(int speech_color = 0, int message_color = 0, int bg_color = 10);
    ~cChat();
	void SetSpeechFormat(int color); 
	void SetMessageFormat(int color); 
	
	void SetBGColor(int color); 
	inline COLORREF GetBGColor(void){ return BGColor; };

	void DisplayMessage(const TCHAR *text, bool sound = true); // display in chat area
	void DisplaySpeech(const TCHAR *text, TCHAR *name, int speechType, bool is_player = false);
	void PreDisplay(void);
	void PostDisplay(void);
	void Show(void);
	void ScrollUp(int count);
	void ScrollDown(int count);
	inline HWND Hwnd(void) { return hwnd_richedit;};


private:

	void SwitchMode(int mode);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cChat(const cChat& x);
	cChat& operator=(const cChat& x);

#ifdef CHECK_INVARIANTS
		void CheckInvariants(int line, TCHAR *file);
#else
		inline void CheckInvariants(int line, TCHAR *file) {};
#endif



	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI RichEditWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

};

#endif