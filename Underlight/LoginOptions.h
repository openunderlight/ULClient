// Header file for Login Options

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef LOGINOPTIONS_H
#define LOGINOPTIONS_H

#include "LyraDefs.h"
#include "Central.h"

//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// Structs


//////////////////////////////////////////////////////////////////
// Options Dialog Box Procedures

BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LaunchOptionsDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);

void __cdecl SaveOutOfGameRegistryOptionValues(void);


#endif
