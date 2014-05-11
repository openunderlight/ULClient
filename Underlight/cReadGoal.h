// Header file for cReadGoal class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CREADGOAL_H
#define CREADGOAL_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LyraDefs.h"
#include "cGoalPosting.h"

//////////////////////////////////////////////////////////////////
// Constants


//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI ReadGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI ReadGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DeleteGoalCallback(void *value);

//////////////////////////////////////////////////////////////////
// Class Definition
class cReadGoal
{

public: 

private:
	HWND hwnd_readgoal; // handle to window for reading goals
	HWND hwnd_spheretext;
	HWND hwnd_sugsphere;
	HWND hwnd_stattext;
	HWND hwnd_sugstat;
	HWND hwnd_creatortext;
	HWND hwnd_creator;
	HWND hwnd_text; // handle to goal text window
	HWND hwnd_accept; // handle to accept goal button
	HWND hwnd_complete; // handle to mark completed button
	HWND hwnd_edit; // handle to edit goal button
	HWND hwnd_delete; // handle to delete goal button
	HWND hwnd_report; // handle to report goal button
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
	HBITMAP hReport;
	HBITMAP hExit;

	BOOL active; // whether or not we're reading a goal

	cGoal *goal; // currently displayed goal
	
public:
    cReadGoal(void);
    ~cReadGoal(void);

	void Activate(cGoal *new_goal);
	void Deactivate(void);
	void AcceptGoal(void);
	void DeleteGoal(void);
	void ReportGoal(void);
	void CompleteGoal(void);
	void EditGoal(void);
	void SetText(void); // called when text arrives for a goal

	void DeleteAcknowledged(realmid_t goalid);
	void DeleteError(void);
	void CompleteAcknowledged(void);
	void CompleteError(void);
	void ScrollUp(int count);
	void ScrollDown(int count);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline realmid_t CurrGoalID(void) { if (goal) return goal->ID(); else return NO_GOAL; };

	// Friends
	friend void DeleteGoalCallback(void *value);

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cReadGoal(const cReadGoal& x);
	cReadGoal& operator=(const cReadGoal& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI ReadGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT WINAPI ReadGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif