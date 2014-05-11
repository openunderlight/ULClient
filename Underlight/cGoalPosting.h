// Header file for cGoalPosting class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CGOALPOSTING_H
#define CGOALPOSTING_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LyraDefs.h"
#include "cGameServer.h"


//////////////////////////////////////////////////////////////////
// Constants

const int NO_GOAL = 0;
const int NO_SESSION = 0;
const int GOLD_BACKGROUND = 1;

const int NUM_TEXT_BUTTONS = 2;
const int NUM_GOAL_BUTTONS = 4;

const int GOAL_GUARDIAN_MANAGE_FLAG = 0x01;
const int GOAL_UNUSED_1				= 0x02;
const int GOAL_UNUSED_2				= 0x04;
const int GOAL_UNUSED_3				= 0x08;
const int GOAL_UNUSED_4				= 0x10;
const int GOAL_UNUSED_5				= 0x20;

const TCHAR GOAL_FONT_NAME[16]=_T("Arial");
typedef void (*goal_warning_callback_t)(void *value);


//////////////////////////////////////////////////////////////////
// Enumerations

//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI GoalPostingWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI GoalSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK CompareGoals(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
void BlitGoalBitmap(HWND hWnd);
BOOL CALLBACK GoalPushButtonProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GoalStateButtonProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GoalEditProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GoalComboBoxProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void SubclassGoalWindow(HWND Hwnd);

//////////////////////////////////////////////////////////////////
// Class Definition

// goal constants
const int MAX_READABLE = 999;
const int GOAL_SUMMARY_LENGTH = 64;
const int QUEST_KEYWORDS_LENGTH = 64;
const int MAX_GOAL_LENGTH = 2048;
const int MAX_REPORT_LENGTH  = 1024;
const int MAX_SIMUL_GOALS = 512;
typedef unsigned int guildid_t;



///////////////////////////////////////////////////////////////////////
// cGoal Class

class cGoal
{
public:

private:
	realmid_t id; 
	TCHAR poster_name[Lyra::PLAYERNAME_MAX];
	TCHAR upper_poster_name[Lyra::PLAYERNAME_MAX];
	guildid_t guild_id;
	int rank;
	int sug_sphere;
	int sug_stat;
	short playeroption;
	int max_acceptances;
	int expiration_time;
	int yes_votes;
	int no_votes;
	int vote_expiration;
	int statusflags;
	int otherflags;
	int num_acceptees;
	int num_completees;
	pname_t *acceptees;
	TCHAR summary[GOAL_SUMMARY_LENGTH];
	int length;
	TCHAR *text;
	bool voted; // have we voted on this goal?
	bool has_details;
	// all-guardians-manage goals need duplicate flag
	int all_guardians_manage;

	// below are the class member for supporting quests
	int charges;
	int graphic;
	int color1;
	int color2;
	int item_type;
	int field1;
	int field2;
	int field3;

	int retrieve; // type of item to retrieve; Quest::TALISMAN or Quest::CODEX
	int quest_xp;
	TCHAR keywords[QUEST_KEYWORDS_LENGTH];

public:

    cGoal(void);
	void SetInfo(realmid_t goal_id, guildid_t guild, 
		int goal_difficulty, short status, short playeroption, const TCHAR *goal_summary);
	void SetText(const TCHAR* post_name, int sugsphere, int sugstat, const TCHAR *goal_text);
    void SetDetails(int goal_rank, int maxacceptances, int expirationtime, 
		int yesvotes, int novotes, int voteexpiration, int goal_status,
		int goal_flags, int numacceptees, int quest_graphic, int quest_charges, 
		int quest_color1, int quest_color2, int quest_item_type, int quest_field1, 
		int quest_field2, int quest_field3,	int quest_xp_, int numcompletees,
		const TCHAR* keywords, 	pname_t *goal_acceptees); 
	void SetGuardiansManageFlag(int manageflag);
	~cGoal(void);

	// Selectors
	inline realmid_t ID(void) { return id; };
	inline TCHAR* PosterName(void) { return poster_name; };
	inline TCHAR* UpperPosterName(void) { return upper_poster_name; };
	inline guildid_t GuildID(void) { return guild_id; };
	inline int Rank(void) { return rank; };
	inline short PlayerOption(void) { return playeroption; };
	inline int MaxAcceptances(void) { return max_acceptances; };
	inline int ExpirationTime(void) { return expiration_time; };
	inline int YesVotes(void) { return yes_votes; };
	inline int NoVotes(void) { return no_votes; };
	inline int VoteExpiration(void) { return vote_expiration; };
	inline int StatusFlags(void) { return statusflags; };
	inline int Flags(void) { return otherflags; };
	inline int GuardiansManage(void) { return all_guardians_manage; };
	inline int NumAcceptees(void) { return num_acceptees; };
	inline int NumCompletees(void) { return num_completees; };
	inline TCHAR* Acceptee(int index) { return acceptees[index]; };
	inline int SugSphere(void) { return sug_sphere; };
	inline int SugStat(void) { return sug_stat; };
	inline TCHAR* Summary(void) { return summary; };
	inline int Length(void) { return length; };
	inline TCHAR* Text(void) { return text; };
	inline bool Voted(void) { return voted; };
	inline void SetVoted(bool value) { voted = value; };
	inline bool HasDetails (void) { return has_details; };

	// Quest selectors
	inline int Graphic(void) { return graphic; };
	inline int Charges(void) { return charges; };
	inline int Color1(void) { return color1; };
	inline int Color2(void) { return color2; };
	inline int ItemType(void) { return item_type; };
	inline int Field1(void) { return field1; };
	inline int Field2(void) { return field2; };
	inline int Field3(void) { return field3; };
	inline int QuestXP(void) { return quest_xp; };
	inline int Retrieve(void) { return retrieve; };
	inline TCHAR* Keywords(void) { return keywords; };

	// Mutators
	inline void ResetText(void) { if (text) delete [] text; text = NULL; };

	// the assignment operator
	cGoal& operator=(const cGoal& x);

private:
	// copy constructor is private and undefined -> errors if used
	cGoal(const cGoal& x);
};

///////////////////////////////////////////////////////////////////////
// cReport Class

class cReport
{
public:

private:
//	cGoal* goal;
	realmid_t report_id;
	realmid_t goal_id;
	int awardxp;
	int flags;
	TCHAR reporter_name[Lyra::PLAYERNAME_MAX];
	TCHAR upper_reporter_name[Lyra::PLAYERNAME_MAX];
	TCHAR recipient[Lyra::PLAYERNAME_MAX];
	TCHAR upper_recipient[Lyra::PLAYERNAME_MAX];
	TCHAR summary[GOAL_SUMMARY_LENGTH];
	int length;
	TCHAR *text;

public:

    cReport(void);
//	void SetInfo(cGoal* goal, realmid_t report, const TCHAR* report_summary);
	void SetInfo(realmid_t goal, realmid_t report, const TCHAR* report_summary);
	void SetText(int award, int flags, const TCHAR *post_name, 
		const TCHAR* recip_name, const TCHAR *report_text);
    ~cReport(void);

	// Selectors
//	inline cGoal* Goal(void) { return goal; };
	inline realmid_t GoalID(void) { return goal_id; };
	inline realmid_t ID(void) { return report_id; };
	inline TCHAR* ReporterName(void) { return reporter_name; };
	inline TCHAR* UpperReporterName(void) { return upper_reporter_name; };
	inline TCHAR* Recipient(void) { return recipient; };
	inline TCHAR* UpperRecipient(void) { return upper_recipient; };
	inline TCHAR* Summary(void) { return summary; };
	inline int Length(void) { return length; };
	inline TCHAR* Text(void) { return text; };
	inline int AwardXP(void) { return awardxp; };

	// the assignment operator
	cReport& operator=(const cReport& x);

private:
	// copy constructor private and undefined -> errors if used
	cReport(const cReport& x);
	
};

///////////////////////////////////////////////////////////////////////
// cGoalPosting Class

class cGoalPosting
{

public: 

private:
	short guild;
	short rank;

	HWND hwnd_goalposting; // handle to control panel background
	HWND hwnd_title; // handle to title text
	HWND hwnd_logo; // handle to house logo
	HWND hwnd_summariestext; // handle to summaries label
	HWND hwnd_summaries; // handle to listview for summaries
	HWND hwnd_accept; // handle to accept goal button
	HWND hwnd_details; // handle to get details button
	HWND hwnd_read; // handle to read goal button
	HWND hwnd_goalbook; // handle to view goal book button
	HWND hwnd_exit; // handle to exit
	HWND hwnd_post; // handle to post new goal
	HWND hwnd_review; // handle to review goal reports
	HWND hwnd_refresh; // handle to refresh summaries button

	HWND hwnd_text_buttons[NUM_GOAL_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_GOAL_BUTTONS][2];
	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hTitle; // handles to windows bitmaps used in the window
	HBITMAP hLogo;
	HBITMAP hSummaries;
	HBITMAP hAcceptGoal;
	HBITMAP hShowDetails;
	HBITMAP hReadGoal;
	HBITMAP hGoalBook;
	HBITMAP hExit;
	HBITMAP hPostGoal;
	HBITMAP hReviewReports;
	HBITMAP hRefresh;
	HBITMAP hPostOff;
	HBITMAP hDetailsOff;

	HFONT goalFont; // font handle

	BOOL active; // whether or not we're in goal posting mode
	int session_id; // id for current goal session
	realmid_t last_goal_seen;

	int next_goal; // index of next goal summary to fill in
	int num_new_goals; // # goals retrieved on last message to server
	cGoal* (curr_goals[MAX_SIMUL_GOALS]); // currently active goals

	
public:
	void AddReportHeader(lyra_id_t goalid);
	int page_num; // current page number of goals
	int m_rhCount;
	void InitReportHeaders();
	lyra_id_t m_reportHeaders[MAX_READABLE];
    cGoalPosting(void);
    ~cGoalPosting(void);

	void Activate(int trip_guild, int trip_rank);
	void Deactivate(void);
	void DBUnavailable(void);

	void ReadGoal(void);
	void ReadGoalDetails(void);
	void Post(void);
	void AcceptGoal(cGoal *new_goal);
	void DisplayGoalBook(void);
	void NextPage(void);
	void PrevPage(void);
	void NextLine(void);
	void PrevLine(void);
	void ShowReports(void);
	void RefreshSummaries(void);
	// summary info has arrived for a new goal
	void NewGoalInfo(int sessionid, realmid_t goal_id, short status, short playeroption, const TCHAR *goal_summary); 
	// goal text has arrived
	void NewGoalText(realmid_t goal_id, const TCHAR *poster, 
		int sugsphere, int sugstat, const TCHAR *text);
	// goal details have arrived
	void NewGoalDetails(realmid_t goal_id, int goal_rank, int maxacceptances, 
		int expirationtime, int yesvotes, int novotes, int voteexpiration,
		int status, int flags, int numacceptees, pname_t *acceptees);
	void SetGuardianFlag(realmid_t goal_id);
	// the selected goal was not found in the database
	void GoalNotFound(realmid_t goal_id);
	

	// Selectors
	inline BOOL Active(void) { return active; };
	inline HWND Hwnd(void) { return hwnd_goalposting; };
	inline int Guild(void) { return guild; };
	inline int Rank(void) { return rank; };
	inline HFONT Hfont(void) { return goalFont; };

	// Mutators
 
	
	// other stuff
	void GuildError(void);
	void GuildWarning(goal_warning_callback_t callbackproc);

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cGoalPosting(const cGoalPosting& x);
	cGoalPosting& operator=(const cGoalPosting& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI GoalPostingWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT WINAPI GoalSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif


};



#endif