// Header file for cGoalBook class 

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CGOALBOOK_H
#define CGOALBOOK_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LyraDefs.h"
#include "cGoalPosting.h" // for goal class

//////////////////////////////////////////////////////////////////
// Enumerations

//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI GoalBookWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void RemoveGoalbookCallback(void *value);

//////////////////////////////////////////////////////////////////
// Class Definition
class cGoalBook
{

public: 

private:
	HWND hwnd_goalbook; // handle to window for goal book
	HWND hwnd_summaries; // handle to goal summaries window
	HWND hwnd_read; // handle to read goal button
	HWND hwnd_remove; // handle to remove goal button
//	HWND hwnd_report; // handle to review reports button
	HWND hwnd_exit; // handle to exit

	HBITMAP hReadGoal;
	HBITMAP hRemove;
//	HBITMAP hReport;
	HBITMAP hExit;

	BOOL active; // whether or not the goal book is active

	int num_goals; // number of goals currently in goal book
	realmid_t goal_ids[Lyra::MAX_ACTIVE_GOALS];
	cGoal active_goals[Lyra::MAX_ACTIVE_GOALS];
	
public:
    cGoalBook(void);
    ~cGoalBook(void);

	void Activate(void);
	void Deactivate(void);
	void ReadGoal(void);
	// summary info has arrived for goal in our book
	void NewGoalInfo(realmid_t goal_id, guildid_t guild, 
		int goal_difficulty, const TCHAR *goal_summary); 
	// goal text has arrived
	void NewGoalText(realmid_t goal_id, const TCHAR* poster, 
		int sugsphere, int sugstat, const TCHAR *text);

	BOOL InGoalBook(realmid_t goal_id);

	BOOL AddGoal(realmid_t goal_id, cGoal *new_goal = NULL);
	void RemoveGoal(realmid_t goal_id);
	void RemoveSelectedGoal(void);
//	void ReviewGoals(void);
	void AcceptAcknowledged(realmid_t goal_id);
	void AcceptError(void);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline int NumGoals(void) { return num_goals; };
	inline HWND Hwnd(void) { return hwnd_goalbook; };

	// Friends
	friend void RemoveGoalbookCallback(void *value);

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cGoalBook(const cGoalBook& x);
	cGoalBook& operator=(const cGoalBook& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI GoalBookWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif