// Header file for cPostQuest class 

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CPOSTQUEST_H
#define CPOSTQUEST_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LyraDefs.h"
#include "cQuestBuilder.h" // for quest class

//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// Enumerations

//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI PostQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI PostQuestTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////
// Class Definition
class cPostQuest
{

public: 

private:
	int rank;

	HWND hwnd_postquest; // handle to window for posting new quests
	HWND hwnd_title;
	HWND hwnd_maxtext;  
	HWND hwnd_maxaccepted;
	HWND hwnd_exptext;
	HWND hwnd_expirationdays;
	HWND hwnd_spheretext;
	HWND hwnd_sugsphere;
	HWND hwnd_stattext;
	HWND hwnd_sugstat;
	HWND hwnd_retrievetext;
	HWND hwnd_retrieve;
	HWND hwnd_texttext; // example of naming conventions gone bad
	HWND hwnd_questtext; // handle to text for new quest
	HWND hwnd_sumtext;
	HWND hwnd_summary; // handle to quest summary
	HWND hwnd_quest_xptext;
	HWND hwnd_quest_xp;
	HWND hwnd_help;
	HWND hwnd_post; // handle to button for post new quest
	HWND hwnd_exit; // handle to exit

	HWND hwnd_text_buttons[NUM_TEXT_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_TEXT_BUTTONS][2];
	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hTitle;
	//HBITMAP hLogo;
	HBITMAP hMaxAccept;
	HBITMAP hExpiration;
	HBITMAP hSphere;
	HBITMAP hStat;
	HBITMAP hRetrieve;
	HBITMAP hHelp;
	HBITMAP hQuestXP;
	HBITMAP hText;
	HBITMAP hSummary;
	HBITMAP hPost;
	HBITMAP hExit;

	BOOL active; // whether or not we're reporting a quest

	int last_xp_award;
	int maxaccepted;
	int expirationdays;
	int quest_xp;
	int sugsphere;
	int sugstat;
	int retrieve;

	TCHAR questtext[MAX_GOAL_LENGTH];
	TCHAR summary[Lyra::PLAYERNAME_MAX + 9 + GOAL_SUMMARY_LENGTH];

	cGoal *quest; // currently displayed quest
	
public:
    cPostQuest(void);
    ~cPostQuest(void);

	void Activate(cGoal *new_quest);
	void Deactivate(void);
	void SetText(void); // called when text arrives for a quest
	void SetDetails(void);
	void PrePost();
	void EndPost(void* item_ptr, int color1, int color2, int graphic);
	void Help(void);
	void PostAcknowledged(void);
	void PostError(void);
	void ScrollUp(int count);
	void ScrollDown(int count);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline int Rank(void) { return rank; };
	inline realmid_t CurrQuestID(void) { if (quest) return quest->ID(); else return NO_GOAL; };
    inline HWND Hwnd(void) { return hwnd_postquest; };

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cPostQuest(const cPostQuest& x);
	cPostQuest& operator=(const cPostQuest& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI PostQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT WINAPI PostQuestTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif