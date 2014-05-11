// Header file for cQuestBuilder class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CQuestBuilder_H
#define CQuestBuilder_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LyraDefs.h"
#include "cGameServer.h"
#include "cGoalPosting.h"

//////////////////////////////////////////////////////////////////
// Constants

const int NUM_QUEST_BUTTONS = 4;
struct scroll_t;

const TCHAR QUEST_FONT_NAME[16]=_T("Arial");
typedef void (*quest_warning_callback_t)(void *value);


//////////////////////////////////////////////////////////////////
// Enumerations/Structures

struct quest_item_t
{
	int graphic;
	int color1;
	int color2;
	int charges;
	int item_type;
	int field1;
	int field2;
	int field3;
	TCHAR keywords[Lyra::QUEST_KEYWORDS_LENGTH];
};


//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI QuestBuilderWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI QuestSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK CompareQuests(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
void ItemtoQuestItem(cItem* item, quest_item_t* questitem);
void ScrollToCodexQuest(scroll_t* scroll, quest_item_t* questitem);
bool DoesTalismanSatisfyQuest(cItem* item, cGoal* quest);

//////////////////////////////////////////////////////////////////
// Class Definition

///////////////////////////////////////////////////////////////////////
// cQuestBuilder Class

class cQuestBuilder
{

public: 

private:
	short rank;

	HWND hwnd_questbuilder; // handle to control panel background
	HWND hwnd_title; // handle to title text
	HWND hwnd_summariestext; // handle to summaries label
	HWND hwnd_summaries; // handle to listview for summaries
	HWND hwnd_accept; // handle to accept quest button
	HWND hwnd_details; // handle to get details button
	HWND hwnd_read; // handle to read quest button
	HWND hwnd_goalbook; // handle to view quest book button
	HWND hwnd_exit; // handle to exit
	HWND hwnd_post; // handle to post new quest
	HWND hwnd_help; // handle to help
	HWND hwnd_complete; // handle to complete accepted quests
	HWND hwnd_refresh; // handle to refresh summaries button

	HWND hwnd_text_buttons[NUM_QUEST_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_QUEST_BUTTONS][2];
	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hTitle; // handles to windows bitmaps used in the window
	HBITMAP hSummaries;
	HBITMAP hAcceptQuest;
	HBITMAP hShowDetails;
	HBITMAP hReadQuest;
	HBITMAP hGoalBook;
	HBITMAP hExit;
	HBITMAP hPostQuest;
	HBITMAP hHelpQuest;
	HBITMAP hCompleteQuest;
	HBITMAP hRefresh;
	HBITMAP hPostOff;
	HBITMAP hDetailsOff;

	HFONT questFont; // font handle

	BOOL active; // whether or not we're in quest posting mode
	int session_id; // id for current quest session
	lyra_id_t last_quest_seen;

	int next_quest; // index of next quest summary to fill in
	int num_new_quests; // # quests retrieved on last message to server
	cGoal* (curr_quests[MAX_SIMUL_GOALS]); // currently active quests

	
public:
	int page_num; // current page number of quests
    cQuestBuilder(void);
    ~cQuestBuilder(void);

	void Activate();
	void Deactivate(void);
	void DBUnavailable(void);

	void ReadQuest(void);
	void ReadQuestDetails(bool completing);
	void Post(void);
	void AcceptQuest(cGoal *new_quest);
	void DisplayGoalBook(void);
	void NextPage(void);
	void PrevPage(void);
	void NextLine(void);
	void PrevLine(void);
	void RefreshSummaries(void);
	void Help(void);
	// summary info has arrived for a new quest
	void NewQuestInfo(int sessionid, realmid_t quest_id, short status, short playeroption, const TCHAR *quest_summary); 
	// quest text has arrived
	void NewQuestText(realmid_t quest_id, const TCHAR *poster, 
		int sugsphere, int sugstat, const TCHAR *text);
	// quest details have arrived
	void NewQuestDetails(realmid_t quest_id, int quest_rank, int maxacceptances, 
		int expirationtime, int numacceptees, int graphic, int charges, 
		int color1, int color2, int item_type, int field1, int field2,
		int field3,	int quest_xp,  int num_completees,
		const TCHAR* keywords, pname_t *acceptees);
	// the selected quest was not found in the database
	void QuestNotFound(realmid_t quest_id);
	

	// Selectors
	inline BOOL Active(void) { return active; };
	inline HWND Hwnd(void) { return hwnd_questbuilder; };
	inline HFONT Hfont(void) { return questFont; };

	// Mutators
 
	// other stuff
	void QuestError(void);
	void QuestWarning(quest_warning_callback_t callbackproc);

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cQuestBuilder(const cQuestBuilder& x);
	cQuestBuilder& operator=(const cQuestBuilder& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI QuestBuilderWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT WINAPI QuestSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif


};



#endif