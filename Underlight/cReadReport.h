// Header file for cReadReport class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CREADREPORT_H
#define CREADREPORT_H

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

LRESULT WINAPI ReadReportWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI ReadReportTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DeleteReportCallback(void *value);

//////////////////////////////////////////////////////////////////
// Class Definition
class cReadReport
{

public: 

private:
	HWND hwnd_readreport; // handle to window for reading reports
	HWND hwnd_creatortext;
	HWND hwnd_creator;
	HWND hwnd_awardtext;
	HWND hwnd_award;
	HWND hwnd_recipienttext;
	HWND hwnd_recipient;
	HWND hwnd_text; // handle to report text window
	HWND hwnd_reply; // handle to reply button
	HWND hwnd_delete; // handle to delete goal button
	HWND hwnd_exit; // handle to exit

	HWND hwnd_text_buttons[NUM_TEXT_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_TEXT_BUTTONS][2];
	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hCreator;
	HBITMAP hRecipient;
	HBITMAP hAward;
	HBITMAP hDelete;
	HBITMAP hReply;
	HBITMAP hExit;

	BOOL active; // whether or not we're reading a report

	cReport *report; // currently displayed report
	
public:
    cReadReport(void);
    ~cReadReport(void);

	void Activate(cReport *new_report);
	void Deactivate(void);
	void DeleteReport(void);
	void SetText(void); // called when text arrives for a report

	void DeleteAcknowledged(realmid_t reportid);
	void DeleteError(void);
	void ScrollUp(int count);
	void ScrollDown(int count);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline cReport* Report(void) { return report; };
	inline realmid_t CurrReportID(void) { if (report) return report->ID(); else return NO_GOAL; };

	// Friends
	friend void DeleteReportCallback(void *value);
	friend LRESULT WINAPI ReadReportTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cReadReport(const cReadReport& x);
	cReadReport& operator=(const cReadReport& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI ReadReportWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif