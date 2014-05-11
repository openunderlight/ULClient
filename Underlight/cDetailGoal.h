// Header file for cDetailGoal class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CDETAILGOAL_H
#define CDETAILGOAL_H

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

LRESULT WINAPI DetailGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//LRESULT WINAPI AccepteeWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////
// Class Definition
class cDetailGoal
{

public: 

private:
	HWND hwnd_detailgoal; // handle to window for reading goals
	HWND hwnd_title;
	HWND hwnd_logo;
	HWND hwnd_loadingtext;
	HWND hwnd_maxaccepttext;
	HWND hwnd_maxaccept;
	HWND hwnd_expirationtext;
	HWND hwnd_expiration;
	HWND hwnd_yesvotetext;
	HWND hwnd_yesvote;
	HWND hwnd_novotetext;
	HWND hwnd_novote;
	HWND hwnd_guard;
	HWND hwnd_voteexpirationtext;
	HWND hwnd_voteexpiration;
	HWND hwnd_statusflagtext;
	HWND hwnd_statusflag;
	HWND hwnd_numaccepteetext;
	HWND hwnd_numacceptee;
	HWND hwnd_accepteestext;
	HWND hwnd_acceptees;

	HWND hwnd_yesbutton; // handle to vote yes button
	HWND hwnd_nobutton; // handle to vote no button
	HWND hwnd_exit; // handle to exit

	HWND hwnd_text_buttons[NUM_GOAL_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_GOAL_BUTTONS][2];
//	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hTitle;
	HBITMAP hLogo;
	HBITMAP hLoading;
	HBITMAP hMaxAccept;
	HBITMAP hExpiration;
	HBITMAP hYesVotes;
	HBITMAP hNoVotes;
	HBITMAP hGuard;
	HBITMAP hVoteExpiration;
	HBITMAP hStatus;
	HBITMAP hNumAcceptee;
	HBITMAP hAcceptee;
	HBITMAP hYesButton;
	HBITMAP hNoButton;
	HBITMAP hExit;
	HBITMAP hAbstain;

	BOOL active; // whether or not we're reading a goal

	cGoal *goal; // currently displayed goal
	
public:
    cDetailGoal(void);
    ~cDetailGoal(void);

	void Activate(cGoal *new_goal);
	void Deactivate(void);
	void SetDetails(void); // called when details arrive for a goal

	void Vote(int vote);

	void VoteAcknowledged(realmid_t goalid);
	void VoteError(void);
	void ScrollUp(int count);
	void ScrollDown(int count);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline realmid_t CurrGoalID(void) { if (goal) return goal->ID(); else return NO_GOAL; };

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cDetailGoal(const cDetailGoal& x);
	cDetailGoal& operator=(const cDetailGoal& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI DetailGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//	friend LRESULT WINAPI AccepteeWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif