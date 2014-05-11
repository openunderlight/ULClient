// Header file for Keyboard

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "Central.h"

//////////////////////////////////////////////////////////////////
// Constants

struct Keystates {
	enum {
	SHIFT = 0,
	ALT, // aka MENU
	CONTROL,
	LOOK_UP,
	LOOK_DOWN,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	TURN_LEFT,
	TURN_RIGHT,
	TRIP,
	RUN,
	JUMP,
	STRAFE,
	SIDESTEP_LEFT,
	SIDESTEP_RIGHT
	};
};

const int num_keystates = 15;
void __cdecl InitKeyboard(void);
void __cdecl DeInitKeyboard(void);


//////////////////////////////////////////////////////////////////
// Keyboard message handlers

void Realm_OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
void Realm_OnChar(HWND hWnd, UINT ch, int cRepeat);

#endif
