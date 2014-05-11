// Header file for cPostGoal class 

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CPOSTGOAL_H
#define CPOSTGOAL_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LyraDefs.h"
#include "cGoalPosting.h" // for goal class

//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// Enumerations

//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI PostGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI PostGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////
// Class Definition
class cPostGoal
{

public: 

private:
	int guild;
	int rank;

	HWND hwnd_postgoal; // handle to window for posting new goals
	HWND hwnd_title;
	HWND hwnd_logo;
	HWND hwnd_maxtext;  
	HWND hwnd_maxaccepted;
	HWND hwnd_exptext;
	HWND hwnd_expirationdays;
	HWND hwnd_spheretext;
	HWND hwnd_sugsphere;
	HWND hwnd_stattext;
	HWND hwnd_sugstat;
	HWND hwnd_texttext; // example of naming conventions gone bad
	HWND hwnd_goaltext; // handle to text for new goal
	HWND hwnd_sumtext;
	HWND hwnd_summary; // handle to goal summary
	HWND hwnd_guard; // handle to guardian checkbox
	HWND hwnd_post; // handle to button for post new goal
	HWND hwnd_exit; // handle to exit

	HWND hwnd_text_buttons[NUM_TEXT_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_TEXT_BUTTONS][2];
	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hTitle;
	HBITMAP hLogo;
	HBITMAP hMaxAccept;
	HBITMAP hExpiration;
	HBITMAP hSphere;
	HBITMAP hStat;
	HBITMAP hText;
	HBITMAP hSummary;
	HBITMAP hGuard;
	HBITMAP hPost;
	HBITMAP hExit;

	BOOL active; // whether or not we're reporting a goal

	cGoal *goal; // currently displayed goal
	
public:
    cPostGoal(void);
    ~cPostGoal(void);

	void Activate(int trip_guild, int trip_rank, cGoal *new_goal);
	void Deactivate(void);
	void SetText(void); // called when text arrives for a goal
	void SetDetails(void);
	void Post(void);
	void PostAcknowledged(void);
	void PostError(void);
	void ScrollUp(int count);
	void ScrollDown(int count);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline int Rank(void) { return rank; };
	inline int Guild(void) { return guild; };
	inline realmid_t CurrGoalID(void) { if (goal) return goal->ID(); else return NO_GOAL; };
    inline HWND Hwnd(void) { return hwnd_postgoal; };

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cPostGoal(const cPostGoal& x);
	cPostGoal& operator=(const cPostGoal& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI PostGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT WINAPI PostGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif