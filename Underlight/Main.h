// Header file for Main.cpp

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef MAIN_H
#define MAIN_H

#include "Central.h"

//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// Function Prototypes

int PASCAL AgentWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow);
LRESULT WINAPI WindowProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT WINAPI AgentWindowProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
static void ExitCallback (void* value);


#endif
