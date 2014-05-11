// Header file for cReadQuest class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CREADQUEST_H
#define CREADQUEST_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LyraDefs.h"
#include "cQuestBuilder.h"

//////////////////////////////////////////////////////////////////
// Constants


//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI ReadQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI ReadQuestTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DeleteQuestCallback(void *value);

//////////////////////////////////////////////////////////////////
// Class Definition
class cReadQuest
{

public: 

private:
	HWND hwnd_readquest; // handle to window for reading quests
	HWND hwnd_spheretext;
	HWND hwnd_sugsphere;
	HWND hwnd_stattext;
	HWND hwnd_sugstat;
	HWND hwnd_creatortext;
	HWND hwnd_creator;
	HWND hwnd_text; // handle to quest text window
	HWND hwnd_accept; // handle to accept quest button
	HWND hwnd_complete; // handle to mark completed button
	HWND hwnd_edit; // handle to edit quest button
	HWND hwnd_delete; // handle to delete quest button
	HWND hwnd_exit; // handle to exit

	HWND hwnd_text_buttons[NUM_TEXT_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_TEXT_BUTTONS][2];
	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hSphere;
	HBITMAP hStat;
	HBITMAP hCreator;
	HBITMAP hAccept;
	HBITMAP hComplete;
	HBITMAP hEdit;
	HBITMAP hDelete;
	HBITMAP hExit;

	BOOL active; // whether or not we're reading a quest

	cGoal *quest; // currently displayed quest
	
public:
    cReadQuest(void);
    ~cReadQuest(void);

	void Activate(cGoal *new_quest);
	void Deactivate(bool deactive_details = true);
	void AcceptQuest(void);
	void DeleteQuest(void);
	void CompleteQuest(void);
	void EditQuest(void);
	void SetText(void); // called when text arrives for a quest

	void DeleteAcknowledged(realmid_t questid);
	void DeleteError(void);
	void CompleteAcknowledged(void);
	void CompleteError(void);
	void ScrollUp(int count);
	void ScrollDown(int count);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline realmid_t CurrQuestID(void) { if (quest) return quest->ID(); else return NO_GOAL; };

	// Friends
	friend void DeleteQuestCallback(void *value);

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cReadQuest(const cReadQuest& x);
	cReadQuest& operator=(const cReadQuest& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI ReadQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT WINAPI ReadQuestTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif