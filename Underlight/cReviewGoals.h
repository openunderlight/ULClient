// Header file for cReviewGoals class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CREVIEWGOALS_H
#define CREVIEWGOALS_H

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

LRESULT WINAPI ReviewGoalsWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI ReportSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////
// Class Definition
class cReviewGoals
{

public: 

private:
	HWND hwnd_reviewgoals; // handle to window for reporting goals
	HWND hwnd_title; // handle to title text
	HWND hwnd_logo; // handle to house logo
	HWND hwnd_summariestext; // handle to summary label
	HWND hwnd_summaries; // handle to text area where reports are displayed
	HWND hwnd_exit; // handle to exit
	HWND hwnd_read; // handle to read button
	HWND hwnd_refresh; // handle to refresh summaries button

	HWND hwnd_text_buttons[NUM_GOAL_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_GOAL_BUTTONS][2];
	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hTitle;
	HBITMAP hLogo;
	HBITMAP hSummaries;
	HBITMAP hExit;
	HBITMAP hReadReport;
	HBITMAP hRefresh;

	BOOL active; // whether or not we're reporting a goal

	int session_id;
	realmid_t last_report_seen;

//	cGoal *goal;
	realmid_t goal_id;
	cReport* (curr_reports[MAX_SIMUL_GOALS]); // currently active reports
	int next_report; // index of next report summary to fill in
	int page_num;
	
public:
    cReviewGoals(void);
    ~cReviewGoals(void);

//	void Activate(cGoal *new_goal);
	void Activate(realmid_t goal);
	void Deactivate(void);
	inline unsigned int LastReportSeen(void) { return last_report_seen; }

	void ReadReport(void);
	// summary info has arrived for a new report
	void NewReportInfo(int sessionid, realmid_t report_id, realmid_t goal_id, const TCHAR *report_summary); 
	// goal text has arrived
	void NewReportText(realmid_t report_id, int awardxp, int flags, const TCHAR *creator, 
		const TCHAR* recipient, const TCHAR *text);
	void NextPage(void);
	void PrevPage(void);
	void NextLine(void);
	void PrevLine(void);
	void ReportNotFound(realmid_t report_id);
	void RefreshSummaries(void);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline HWND Hwnd(void) { return hwnd_reviewgoals; };

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cReviewGoals(const cReviewGoals& x);
	cReviewGoals& operator=(const cReviewGoals& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI ReviewGoalsWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT WINAPI ReportSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif