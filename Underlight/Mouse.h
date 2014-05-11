// Header file for mouse.cpp

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef MOUSE_H
#define MOUSE_H

#include "Central.h"

//////////////////////////////////////////////////////////////////
// New Window Messages

//////////////////////////////////////////////////////////////////
// Constants

struct mouse_move_t
{
	bool	moving;
	bool	forward;
	bool	backward;
	bool	left;
	bool	right;
	bool	shift;
	float	xratio;
};

struct mouse_look_t
{
	bool	looking;
	bool	up;
	bool	down;
	bool	left;
	bool	right;
	bool	shift;
	float	xratio;
	float	yratio;
};

struct forced_move_t
{
	bool forward;
	bool backward;
	bool left;
	bool right;
	bool strafe;
};

//////////////////////////////////////////////////////////////////
// Function Prototypes

void Realm_OnLButtonDown( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags); 
void Realm_OnMButtonDown( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags); 
void Realm_OnRButtonDown( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags); 
void Realm_OnKillFocus(HWND hWnd, HWND hWndNewFocus);
void Realm_OnMouseMove(HWND hWnd, WORD x, WORD y, WORD fwKeys);
void Realm_OnLButtonUp(HWND hWnd, WORD x, WORD y, WORD fwKeys);
void Realm_OnMButtonUp(HWND hWnd, WORD x, WORD y, WORD fwKeys);
void Realm_OnRButtonUp(HWND hWnd, WORD x, WORD y, WORD fwKeys );

// local functions
void StartMouseMove(int x);
void StartMouseMove(int x, int y);
void StopMouseMove(void);
void StartMouseLook(int x, int y);
void StopMouseLook(void);

#endif
