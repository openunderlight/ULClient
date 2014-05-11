// Header file for cReportGoal class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CREPORT_H
#define CREPORT_H

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

LRESULT WINAPI ReportGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI ReportGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////
// Class Definition
class cReportGoal
{

public: 

private:
	HWND hwnd_reportgoal; // handle to window for reporting goals
	HWND hwnd_xplabel;
	HWND hwnd_awardxp;
	HWND hwnd_sumlabel;
	HWND hwnd_summary;
	HWND hwnd_textlabel;
	HWND hwnd_text; // text of report
	HWND hwnd_done; // handle to report button
	HWND hwnd_exit; // handle to exit

	HWND hwnd_text_buttons[NUM_TEXT_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_TEXT_BUTTONS][2];
	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hAward;
	HBITMAP hSummary;
	HBITMAP hText;
	HBITMAP hPost;
	HBITMAP hExit;

	BOOL active; // whether or not we're reporting a goal

	cGoal *goal; // goal being reported -- OR --
	cReport *report; // report being responded

	int last_xp_award;
	
public:
    cReportGoal(void);
    ~cReportGoal(void);

	void Activate(cGoal *new_goal);
	void Activate(cReport *new_report);
	void Deactivate(void);
	void Report(void);
	void ReportAcknowledged(void);
	void ReportError(void);
	void ScrollUp(int count);
	void ScrollDown(int count);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline HWND Hwnd(void) { return hwnd_reportgoal; };

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cReportGoal(const cReportGoal& x);
	cReportGoal& operator=(const cReportGoal& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI ReportGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT WINAPI ReportGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif