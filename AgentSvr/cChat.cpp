// cChat: The rich edit control that displays chat and status messages.

// This file is NOT shared with the main project!

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "cDDraw.h"
#include "cChat.h"

extern cDDraw *cDD;



/////////////////////////////////////////////////////////////////
// Class Defintion

cChat::cChat(int speech_color, int message_color, int bg_color)
{	// avoid illegal window handle references
	hwnd_richedit = cDD->Hwnd_Main();
}

void cChat::SwitchMode(int mode) {}

void cChat::DisplayMessage(const char *message, bool sound) {}

void cChat::DisplaySpeech(const char *message, char *name, int speechType, bool is_player) {}


void cChat::SetSpeechFormat(int index) {}

char* ChatColorName(int value) { return NULL;};

void cChat::SetMessageFormat(int index) {}

void cChat::SetBGColor(int index) {}

cChat::~cChat(void) {}


#ifdef DEBUG

void cChat::Debug_CheckInvariants(int caller) {}

#endif



