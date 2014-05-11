// Handles the player's goal book

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cGameServer.h"
#include "cGoalPosting.h"
#include "cQuestBuilder.h"
#include "cReadGoal.h"
#include "cReadQuest.h"
#include "cReportGoal.h"
#include "cReviewGoals.h"
#include "cGoalBook.h"
#include "resource.h"
#include "cChat.h"
#include "utils.h"
#include "options.h"
#include "interface.h"
#include "cEffects.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cDDraw *cDD;
extern cEffects *effects;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cGameServer *gs;
extern cChat *display;
extern cGoalPosting *goals;
extern cQuestBuilder *quests;
extern cGoalBook *goalbook;
extern cReadGoal *readgoal;
extern cReadQuest *readquest;
extern cReportGoal *reportgoal;
extern cReviewGoals *reviewgoals;
extern options_t options;

//////////////////////////////////////////////////////////////////
// Constants

// position for goal book window, relative to main
const struct window_pos_t goalBookPos[MAX_RESOLUTIONS]= 
{ { 0, 300, 480, 180 }, { 0, 375, 600, 225 }, { 0, 480, 768, 288 } };

// position for summaries list box
const struct window_pos_t summariesPos[MAX_RESOLUTIONS]= 
{ { 5, 5, 460, 145 }, { 6, 6, 575, 181 }, { 8, 8, 736, 232 } };

// position for reports button, relative to goal book window
const struct window_pos_t reportPos[MAX_RESOLUTIONS]= 
{ { 110, 140, 70, 20 }, { 137, 175, 70, 20 }, { 176, 224, 70, 20 } };

// position for read goal button, relative to goal book window
const struct window_pos_t readPos[MAX_RESOLUTIONS]= 
{ { 200, 140, 70, 20 }, { 250, 175, 70, 20 }, { 320, 224, 70, 20 } };

// position for remove button, relative to goal book window
const struct window_pos_t removePos[MAX_RESOLUTIONS]= 
{ { 290, 140, 70, 20 }, { 362, 175, 70, 20 }, { 464, 224, 70, 20 } };

// position for exit button, relative to goal book window
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
{ { 380, 140, 70, 20 }, { 475, 175, 70, 20 }, { 608, 224, 70, 20 } };

// Constructor
cGoalBook::cGoalBook(void) 
{
	int i;

	WNDCLASS wc;

	active = FALSE;

	num_goals = 0;
	for (i=0; i<Lyra::MAX_ACTIVE_GOALS; i++)
		goal_ids[i] = NO_GOAL;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = GoalBookWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("GoalBook");

    RegisterClass( &wc );

    hwnd_goalbook =  CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("GoalBook"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		goalBookPos[cDD->Res()].x, goalBookPos[cDD->Res()].y, 		
		goalBookPos[cDD->Res()].width, goalBookPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_goalbook, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_summaries = CreateWindow(_T("listbox"), _T("Summaries"),
						WS_CHILD | WS_DLGFRAME | LBS_NOTIFY,
						summariesPos[cDD->Res()].x, summariesPos[cDD->Res()].y, 
						summariesPos[cDD->Res()].width, summariesPos[cDD->Res()].height,
						hwnd_goalbook,
						NULL, hInstance, NULL);
	SendMessage(hwnd_summaries, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_read = CreateWindow(_T("button"), _T("Read Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						readPos[cDD->Res()].x, readPos[cDD->Res()].y, 
						readPos[cDD->Res()].width, readPos[cDD->Res()].height,
						hwnd_goalbook,
						NULL, hInstance, NULL); 
	hReadGoal = CreateWindowsBitmap(LyraBitmap::READ_GOAL_BUTTON);
	SendMessage(hwnd_read, BM_SETIMAGE, WPARAM (IMAGE_BITMAP), LPARAM (hReadGoal));
	SubclassGoalWindow(hwnd_read);

	hwnd_remove = CreateWindow(_T("button"), _T("Remove Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						removePos[cDD->Res()].x, removePos[cDD->Res()].y, 
						removePos[cDD->Res()].width, removePos[cDD->Res()].height,
						hwnd_goalbook,
						NULL, hInstance, NULL);
	hRemove = CreateWindowsBitmap(LyraBitmap::REMOVE_GOAL_BUTTON);
	SendMessage(hwnd_remove, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hRemove));
	SubclassGoalWindow(hwnd_remove);

/*
	hwnd_report = CreateWindow("button", "Show Reports",
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						reportPos[cDD->Res()].x, reportPos[cDD->Res()].y, 
						reportPos[cDD->Res()].width, reportPos[cDD->Res()].height,
						hwnd_goalbook,
						NULL, hInstance, NULL); 
	hReport = CreateWindowsBitmap(LyraBitmap::SHOW_REPORTS_BUTTON);
	SendMessage(hwnd_report, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hReport));
	SubclassGoalWindow(hwnd_report);
*/

	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_goalbook,
						NULL, hInstance, NULL);
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	this->CheckInvariants(__LINE__);
	return;

}


void cGoalBook::Activate(void)
{ 
	if (active)
		return;

	active = TRUE; 

	// Get goalbook headers
	if ((num_goals == 0) && options.network && gs && gs->LoggedIntoGame())
		gs->RequestGoalbook();

	ShowWindow(hwnd_summaries, SW_SHOWNORMAL); 
	ShowWindow(hwnd_read, SW_SHOWNORMAL); 
	if (options.network && gs && gs->LoggedIntoGame())
		ShowWindow(hwnd_remove, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL); 

/*
	if (goals->Active()) 
		ShowWindow(hwnd_report, SW_SHOWNORMAL);
	else
		ShowWindow(hwnd_report, SW_HIDE);
*/

	ListBox_SetCurSel(hwnd_summaries, 0);

	ShowWindow(hwnd_goalbook, SW_SHOWNORMAL); 

	this->CheckInvariants(__LINE__);
	return;
};

void cGoalBook::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_goalbook, SW_HIDE); 

	if (goals->Active())
		SendMessage(goals->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goals->Hwnd());
	else if (quests->Active())
		SendMessage(quests->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) quests->Hwnd());
	else 
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	this->CheckInvariants(__LINE__);
	return;
}

void cGoalBook::ReadGoal(void)
{
	int selected;
	
	selected = ListBox_GetCurSel(hwnd_summaries);
	if (selected == -1)
		return;

	if (active_goals[selected].Rank() == Guild::QUEST) 
		readquest->Activate(&(active_goals[selected]));
	else
		readgoal->Activate(&(active_goals[selected]));

	if (goals->Active())
	{ 
		if ((goals->Guild() == active_goals[selected].GuildID())
			&& (goals->Rank() == active_goals[selected].Rank()))
			reviewgoals->Activate(active_goals[selected].ID());
	}

	return;
}


void cGoalBook::RemoveGoal(realmid_t goal_id)
{
	int i =0;
	for (i=0; i<Lyra::MAX_ACTIVE_GOALS; i++)
		if (goal_ids[i] == goal_id)
			break;

	if (i == Lyra::MAX_ACTIVE_GOALS)
	{
		NONFATAL_ERROR(IDS_UNKNOWN_GOAL);
		return;
	}

	ListBox_DeleteString(hwnd_summaries, i);

	// compact goal book...
	for (int j=i; j<num_goals-1; j++)
	{
		goal_ids[j] = goal_ids[j+1];
		active_goals[j] = active_goals[j+1];
	}
	num_goals--;
	goal_ids[num_goals] = NO_GOAL;
	active_goals[num_goals].ResetText();

	// tell server to punt this goal from player's record
	if (options.network && gs && gs->LoggedIntoGame())
		gs->RemoveFromGoalbook(goal_id);

	this->CheckInvariants(__LINE__);
	return;

}

void cGoalBook::RemoveSelectedGoal(void)
{
	int selected;
	
	selected = ListBox_GetCurSel(hwnd_summaries);
	if (selected == -1)
		return;

	if (active_goals[selected].Rank() == Guild::QUEST) 
		LoadString (hInstance, IDS_REMOVEQUESTBOOK_WARN, message, sizeof(message));
	else
		LoadString (hInstance, IDS_REMOVEGOALBOOK_WARN, message, sizeof(message));

	goals->GuildWarning(&RemoveGoalbookCallback);

}

void RemoveGoalbookCallback(void *value)
{
	int deletegoal = *((int*)value);
	int selected;
	
	selected = ListBox_GetCurSel(goalbook->hwnd_summaries);
	if (selected == -1)
		return;

	if (deletegoal == 1)
	{
		goalbook->RemoveGoal(goalbook->active_goals[selected].ID());
		if (reviewgoals->Active()) 
			reviewgoals->Deactivate();
	}
	else
	{
		LoadString (hInstance, IDS_REMOVEGOALBOOK_ABORTED, message, sizeof(message));
		goals->GuildError();
	}	
	return;
}

#if 0  // deprecated
void cGoalBook::ReviewGoals(void)
{
	int selected = ListBox_GetCurSel(hwnd_summaries);

	if (selected == -1)
		return;
	if ((goals->Active()) && (goals->Guild() == active_goals[selected].GuildID())
		&& (goals->Rank() == active_goals[selected].Rank()))
//		reviewgoals->Activate(&(active_goals[selected]));
		reviewgoals->Activate(active_goals[selected].ID());
	else
	{
		LoadString (hInstance, IDS_GOALBOOK_WRONGBOARD, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, RankGoalName(active_goals[selected].Rank()), 
			GuildName(active_goals[selected].GuildID()), RankName(active_goals[selected].Rank()));
		goals->GuildError();
	}

	return;
}
#endif

BOOL cGoalBook::InGoalBook(realmid_t goal_id)
{
	for (int i=0; i<Lyra::MAX_ACTIVE_GOALS; i++)
		if (goal_ids[i] == goal_id)
			return TRUE;

	return FALSE;
}


BOOL cGoalBook::AddGoal(realmid_t goal_id, cGoal *new_goal)
{
	int i;

	if (num_goals >= Lyra::MAX_ACTIVE_GOALS)
	{
		LoadString (hInstance, IDS_GOALBOOK_FULL, message, sizeof(disp_message));
		goals->GuildError();
		return FALSE;
	}

	if (InGoalBook(goal_id))
		return FALSE;

	for (i=0; i<Lyra::MAX_ACTIVE_GOALS; i++)
		if (goal_ids[i] == NO_GOAL)
		{
			goal_ids[i] = goal_id;
			if (new_goal) // goal passed in, we can copy it
			{	// request text if we don't have it already
				active_goals[i] = *new_goal;
				if ((!new_goal->Text()) && options.network && gs && gs->LoggedIntoGame())
					gs->RequestGoalText(goal_id);
			}
			else // goal not passed it; request it
			{
				LoadString (hInstance, IDS_LOADING, message, sizeof(message));
				active_goals[i].SetInfo(NO_GOAL, Lyra::ID_UNKNOWN, 0, Guild::GOAL_ACTIVE, 0, message);
				if (options.network && gs && gs->LoggedIntoGame())
					gs->RequestGoalbook();
			}
			break;
		}

	if (i == Lyra::MAX_ACTIVE_GOALS)
	{
		GAME_ERROR(IDS_NO_FIND_SLOT);
		return FALSE;
	}

	num_goals++;
	ListBox_AddString(hwnd_summaries, active_goals[i].Summary());
	ListBox_SetCurSel(hwnd_summaries, 0);

	this->CheckInvariants(__LINE__);
	return TRUE;
}

// goal info has arrived
void cGoalBook::NewGoalInfo(realmid_t goal_id, guildid_t guild, 
		int goal_difficulty, const TCHAR *goal_summary) 
{
	BOOL goalfound = false;

	if ((goal_difficulty != Guild::QUEST) && 
		(goal_difficulty > player->GuildRank(guild)))
	{
		if (InGoalBook(goal_id))
			this->RemoveGoal(goal_id);
		else
			gs->RemoveFromGoalbook(goal_id);
		return;
	}

	for (int i=0; i<num_goals; i++)
		if (goal_ids[i] == goal_id)
		{
			active_goals[i].SetInfo(goal_id, guild, goal_difficulty, Guild::GOAL_ACTIVE, 0, goal_summary);
			ListBox_DeleteString(hwnd_summaries, i);
			ListBox_InsertString(hwnd_summaries, i, active_goals[i].Summary());
			goalfound = true;
			break;
		}

	if (!goalfound)
	{
		cGoal *newgoal;
		newgoal = new cGoal();
		newgoal->SetInfo(goal_id, guild, goal_difficulty, Guild::GOAL_ACTIVE, 0, goal_summary);
		this->AddGoal(goal_id, newgoal);
		delete newgoal;
	}

	this->CheckInvariants(__LINE__);
	return;
}

// goal text has arrived
void cGoalBook::NewGoalText(realmid_t goal_id, const TCHAR *poster, 
		int sugsphere, int sugstat, const TCHAR *text)
{
	for (int i=0; i<num_goals; i++)
		if (goal_ids[i] == goal_id)
		{
			active_goals[i].SetText(poster, sugsphere, sugstat, text);
			if (active_goals[i].Rank() == Guild::QUEST) 
			{
				if (readquest->Active() && (readquest->CurrQuestID() == goal_id))
					readquest->SetText();
			} 
			else if (readgoal->Active() && (readgoal->CurrGoalID() == goal_id))
				readgoal->SetText();
			break;
		}
	this->CheckInvariants(__LINE__);
	return;
}


void cGoalBook::AcceptAcknowledged(realmid_t goal_id)
{
	if (readgoal->Active())
		readgoal->Deactivate();
	if (readquest->Active())
		readquest->Deactivate();

	goalbook->AddGoal(goal_id, NULL);

	if (!goalbook->Active())
	{
		if (goals->Active())
		{
			LoadString (hInstance, IDS_GOALBOOK_ACCEPT_ACK, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, RankGoalName(goals->Rank()));
			display->DisplayMessage(message);
		}
		else if (quests->Active())
		{
			LoadString (hInstance, IDS_GOALBOOK_ACCEPT_QUEST_ACK, message, sizeof(message));
			display->DisplayMessage(message);
		}
	}
}


void cGoalBook::AcceptError(void)
{
	if (goalbook->Active())
		goalbook->Deactivate();

	if (readgoal->Active()) 
		readgoal->Deactivate();
	if (readquest->Active())
		readquest->Deactivate();

	if (goals->Active())
	{
		LoadString (hInstance, IDS_GOALBOOK_ACCEPT_ERROR, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, RankGoalName(goals->Rank()), RankGoalName(goals->Rank()));
	}
	else
		LoadString (hInstance, IDS_QUESTBOOK_ACCEPT_ERROR, message, sizeof(message));

	display->DisplayMessage(message);
}


// Destructor
cGoalBook::~cGoalBook(void)
{
	if (hReadGoal!=NULL)
		DeleteObject(hReadGoal);
	if (hRemove!=NULL)
		DeleteObject(hRemove);
//	if (hReport!=NULL)
//		DeleteObject(hReport);
	if (hExit!=NULL)
		DeleteObject(hExit);
}


// Window procedure for the read goal window
LRESULT WINAPI GoalBookWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;

	switch(message)
	{
		case WM_PAINT:
			TileBackground(goalbook->hwnd_goalbook);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == goalbook->hwnd_exit)
				goalbook->Deactivate();
			else if ((HWND)lParam == goalbook->hwnd_read)
				goalbook->ReadGoal();
			else if ((HWND)lParam == goalbook->hwnd_remove)
				goalbook->RemoveSelectedGoal();
//			else if ((HWND)lParam == goalbook->hwnd_report)
//				goalbook->ReviewGoals();
			else if ((HWND)lParam == goalbook->hwnd_summaries)
			{
				switch (HIWORD(wParam))
				{
					case LBN_DBLCLK:
						goalbook->ReadGoal();
						break;
				}
			}
			break;

		case WM_KEYUP:
			switch ((UINT)(wParam)) {
				case VK_ESCAPE:
					goalbook->Deactivate();
					return 0L;
				case VK_RETURN:
					goalbook->ReadGoal();
					return 0L;
				case VK_UP:
					{
						int prev_summary = ListBox_GetCurSel(goalbook->hwnd_summaries)-1;
						if (prev_summary < 0) prev_summary = 0;
						ListBox_SetCurSel(goalbook->hwnd_summaries, prev_summary);
						return 0L;
					}
				case VK_DOWN:
					ListBox_SetCurSel(goalbook->hwnd_summaries, ListBox_GetCurSel(goalbook->hwnd_summaries)+1);
					return 0L;

				default:
					break;
			}
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Check invariants

#ifdef CHECK_INVARIANTS

void cGoalBook::CheckInvariants(int line)
{
	if (ListBox_GetCount(hwnd_summaries) != num_goals)
	{
		GAME_ERROR(IDS_GOAL_COUNT_ERR);
	}

	for (int i=num_goals; i<Lyra::MAX_ACTIVE_GOALS; i++)
		if (goal_ids[i] != NO_GOAL)
		{
			LoadString (hInstance, IDS_GOALBOOK_ERR, message, sizeof(message));
			_stprintf(errbuf,message,i,line);
			GAME_ERROR(errbuf);
		}
	
	return;
}

#endif
