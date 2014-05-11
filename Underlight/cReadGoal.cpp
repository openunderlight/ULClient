// Handles the goal-posting screens

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cGoalPosting.h"
#include "cPostGoal.h"
#include "cReadGoal.h"
#include "cGoalBook.h"
#include "cReportGoal.h"
#include "cDetailGoal.h"
#include "cReviewGoals.h"
#include "cChat.h"
#include "utils.h"
#include "Options.h"
#include "resource.h"
#include "interface.h"
#include "cEffects.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern cEffects *effects;
extern cDDraw *cDD;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cChat *display;
extern cGoalPosting *goals;
extern cPostGoal *postgoal;
extern cReadGoal *readgoal;
extern cGoalBook *goalbook;
extern cReportGoal *reportgoal;
extern cReviewGoals *reviewgoals;
extern cDetailGoal *detailgoal;
extern options_t options;

//////////////////////////////////////////////////////////////////
// Constants

const int VISIBLE_LINES = 3;
const int DOWN = 0;
const int UP = 1;

// text scrolling buttons
struct button_t {
	window_pos_t position[MAX_RESOLUTIONS];
	int			 button_id;
	int			 bitmap_id;
};

const button_t text_buttons[NUM_TEXT_BUTTONS] = 
{ {
	{ { 432, 39, 13, 15 }, { 543, 54, 13, 15 }, { 701, 74, 13, 15 } },
		DOWN, LyraBitmap::CP_DOWNA  },   // down button
	{ { { 432, 0, 13, 15 }, { 543, 0, 13, 15 }, { 701, 0, 13, 15 } },
		UP, LyraBitmap::CP_UPA  } };   // up button

// position for read goal window, relative to main
const struct window_pos_t readGoalPos[MAX_RESOLUTIONS]= 
{ { 0, 300, 480, 180 }, { 0, 375, 600, 225 }, { 0, 480, 768, 288 } };

// position for label for suggested sphere of new goal 
const struct window_pos_t spheretextPos[MAX_RESOLUTIONS]= 
{ { 10, 12, 125, 20 }, { 12, 15, 156, 25 }, { 16, 19, 200, 32 } };

// position for suggested sphere of new goal 
const struct window_pos_t sugspherePos[MAX_RESOLUTIONS]= 
{ { 125, 7, 100, 25 }, { 156, 11, 125, 25 }, { 200, 17, 140, 25 } };

// position for label for suggested sphere of new goal 
const struct window_pos_t stattextPos[MAX_RESOLUTIONS]= 
{ { 260, 12, 110, 20 }, { 325, 15, 137, 25 }, { 416, 19, 176, 32 } };

// position for suggested stat of new goal 
const struct window_pos_t sugstatPos[MAX_RESOLUTIONS]= 
{ { 365, 7, 90, 25 }, { 456, 11, 90, 25 }, { 584, 18, 90, 25 } };

// position for label for suggested sphere of new goal 
const struct window_pos_t creatortextPos[MAX_RESOLUTIONS]= 
{ { 10, 42, 125, 20 }, { 12, 52, 156, 25 }, { 16, 67, 200, 32 } };

// position for suggested sphere of new goal 
const struct window_pos_t creatorPos[MAX_RESOLUTIONS]= 
{ { 125, 37, 100, 25 }, { 156, 49, 125, 25 }, { 200, 65, 140, 25 } };

// position for goal text, relative to read goal window
const struct window_pos_t textPos[MAX_RESOLUTIONS]= 
{ { 10, 70, 450, 60 }, { 12, 87, 562, 75 }, { 16, 112, 720, 96 } };

// position for accept button, relative to read goal window
const struct window_pos_t acceptPos[MAX_RESOLUTIONS]= 
//{ { 200, 140, 70, 20 }, { 250, 175, 87, 25 }, { 320, 224, 112, 32 } };
{ { 200, 140, 70, 20 }, { 250, 175, 70, 20 }, { 320, 224, 70, 20 } };

// position for edit button, relative to read goal window
const struct window_pos_t editPos[MAX_RESOLUTIONS]= 
//{ { 80, 140, 70, 20 }, { 100, 175, 87, 25 }, { 128, 224, 112, 32 } };
{ { 80, 140, 70, 20 }, { 100, 175, 70, 20 }, { 128, 224, 70, 20 } };

// position for complete button, relative to read goal window
const struct window_pos_t completePos[MAX_RESOLUTIONS]= 
//{ { 170, 140, 100, 20 }, { 212, 175, 125, 25 }, { 272, 224, 160, 32 } };
{ { 170, 140, 100, 20 }, { 212, 175, 100, 20 }, { 272, 224, 100, 20 } };

#ifdef GAMEMASTER // Don't overlap in GM builds!
const struct window_pos_t deletePos[MAX_RESOLUTIONS]=
// position for delete button, relative to read goal window
//{ { 290, 140, 70, 20 }, { 362, 175, 87, 25 }, { 464, 224, 112, 32 } };
{ { 290, 160, 70, 20 }, { 362, 195, 70, 20 }, { 464, 244, 70, 20 } };
#else
const struct window_pos_t deletePos[MAX_RESOLUTIONS]= 
// position for delete button, relative to read goal window
//{ { 290, 140, 70, 20 }, { 362, 175, 87, 25 }, { 464, 224, 112, 32 } };
{ { 290, 140, 70, 20 }, { 362, 175, 70, 20 }, { 464, 224, 70, 20 } };
#endif

// position for report button, relative to read goal window
const struct window_pos_t reportPos[MAX_RESOLUTIONS]= 
//{ { 290, 140, 70, 20 }, { 362, 175, 87, 25 }, { 464, 224, 112, 32 } };
{ { 290, 140, 70, 20 }, { 362, 175, 70, 20 }, { 464, 224, 70, 20 } };

// position for exit button, relative to read goal window
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
//{ { 380, 140, 70, 20 }, { 475, 175, 87, 25 }, { 608, 224, 112, 32 } };
{ { 380, 140, 70, 20 }, { 475, 175, 70, 20 }, { 608, 224, 70, 20 } };

// Constructor
cReadGoal::cReadGoal(void) 
{
	WNDCLASS wc;

	active = FALSE;

	goal = NULL;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = ReadGoalWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("ReadGoal");

    RegisterClass( &wc );

    hwnd_readgoal = CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("ReadGoal"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		readGoalPos[cDD->Res()].x, readGoalPos[cDD->Res()].y, 		
		readGoalPos[cDD->Res()].width, readGoalPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_readgoal, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_spheretext = CreateWindow(_T("static"), _T("Suggested sphere:"),
						WS_CHILD | SS_BITMAP,
						spheretextPos[cDD->Res()].x, spheretextPos[cDD->Res()].y, 
						spheretextPos[cDD->Res()].width, spheretextPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	hSphere = CreateWindowsBitmap(LyraBitmap::SUG_SPHERE_LABEL);
	SendMessage(hwnd_spheretext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSphere));

	hwnd_sugsphere = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						sugspherePos[cDD->Res()].x, sugspherePos[cDD->Res()].y, 
						sugspherePos[cDD->Res()].width, sugspherePos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_sugsphere, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_stattext = CreateWindow(_T("static"), _T("Suggested stat:"),
						WS_CHILD | SS_BITMAP,
						stattextPos[cDD->Res()].x, stattextPos[cDD->Res()].y, 
						stattextPos[cDD->Res()].width, stattextPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	hStat = CreateWindowsBitmap(LyraBitmap::SUG_STAT_LABEL);
	SendMessage(hwnd_stattext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hStat));

	hwnd_sugstat = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						sugstatPos[cDD->Res()].x, sugstatPos[cDD->Res()].y, 
						sugstatPos[cDD->Res()].width, sugstatPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_sugstat, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_creatortext = CreateWindow(_T("static"), _T("Posted by:"),
						WS_CHILD | SS_BITMAP,
						creatortextPos[cDD->Res()].x, creatortextPos[cDD->Res()].y, 
						creatortextPos[cDD->Res()].width, creatortextPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	hCreator = CreateWindowsBitmap(LyraBitmap::POSTED_BY_LABEL);
	SendMessage(hwnd_creatortext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hCreator));

	hwnd_creator = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						creatorPos[cDD->Res()].x, creatorPos[cDD->Res()].y, 
						creatorPos[cDD->Res()].width, creatorPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_creator, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_text = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | ES_MULTILINE | ES_WANTRETURN | ES_LEFT | ES_READONLY,
						textPos[cDD->Res()].x, textPos[cDD->Res()].y, 
						textPos[cDD->Res()].width, textPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_text, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	lpfn_text = SubclassWindow(hwnd_text, ReadGoalTextWProc);
	SendMessage(hwnd_text, WM_PASSPROC, 0, (LPARAM) lpfn_text ); 

	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
	{
		hwnd_text_buttons[i] = CreateWindow(_T("button"), _T(""),
				WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
				text_buttons[i].position[cDD->Res()].x, text_buttons[i].position[cDD->Res()].y, 
				text_buttons[i].position[cDD->Res()].width, text_buttons[i].position[cDD->Res()].height,
				hwnd_text, NULL, hInstance, NULL);
		text_buttons_bitmaps[i][0] = // a button
			CreateWindowsBitmap(text_buttons[i].bitmap_id);
		text_buttons_bitmaps[i][1] = // b button
			CreateWindowsBitmap(text_buttons[i].bitmap_id + 1);
	}

	hwnd_accept = CreateWindow(_T("button"), _T("Accept Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						acceptPos[cDD->Res()].x, acceptPos[cDD->Res()].y, 
						acceptPos[cDD->Res()].width, acceptPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL); 
	hAccept = CreateWindowsBitmap(LyraBitmap::ACCEPT_GOAL_BUTTON);
	SendMessage(hwnd_accept, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hAccept));
	SubclassGoalWindow(hwnd_accept);

	hwnd_complete = CreateWindow(_T("button"), _T("Mark Completed"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						completePos[cDD->Res()].x, completePos[cDD->Res()].y, 
						completePos[cDD->Res()].width, completePos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	hComplete = CreateWindowsBitmap(LyraBitmap::COMPLETE_BUTTON);
	SendMessage(hwnd_complete, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hComplete));
	SubclassGoalWindow(hwnd_complete);

	hwnd_edit = CreateWindow(_T("button"), _T("Edit Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						editPos[cDD->Res()].x, editPos[cDD->Res()].y, 
						editPos[cDD->Res()].width, editPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	hEdit = CreateWindowsBitmap(LyraBitmap::EDIT_BUTTON);
	SendMessage(hwnd_edit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hEdit));
	SubclassGoalWindow(hwnd_edit);

	hwnd_delete = CreateWindow(_T("button"), _T("Delete Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						deletePos[cDD->Res()].x, deletePos[cDD->Res()].y, 
						deletePos[cDD->Res()].width, deletePos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	hDelete = CreateWindowsBitmap(LyraBitmap::REMOVE_GOAL_BUTTON);
	SendMessage(hwnd_delete, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hDelete));
	SubclassGoalWindow(hwnd_delete);
	
	hwnd_report = CreateWindow(_T("button"), _T("Post Report"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						reportPos[cDD->Res()].x, reportPos[cDD->Res()].y, 
						reportPos[cDD->Res()].width, reportPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	hReport = CreateWindowsBitmap(LyraBitmap::REPORT_BUTTON);
	SendMessage(hwnd_report, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hReport));
	SubclassGoalWindow(hwnd_report);

	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_readgoal,
						NULL, hInstance, NULL);
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	return;

}


// it's ok to call this when active - just replace the current goal text
void cReadGoal::Activate(cGoal *new_goal)
{ 
	if (reportgoal->Active())
		reportgoal->Deactivate();

	active = TRUE; 
	goal = new_goal;
	ShowWindow(hwnd_delete, SW_HIDE);
	ShowWindow(hwnd_report, SW_HIDE);
	ShowWindow(hwnd_complete, SW_HIDE);
	ShowWindow(hwnd_spheretext, SW_SHOWNORMAL);
	ShowWindow(hwnd_sugsphere, SW_SHOWNORMAL);
	ShowWindow(hwnd_stattext, SW_SHOWNORMAL);
	ShowWindow(hwnd_sugstat, SW_SHOWNORMAL);
	ShowWindow(hwnd_creatortext, SW_SHOWNORMAL);
	ShowWindow(hwnd_creator, SW_SHOWNORMAL);
	ShowWindow(hwnd_text, SW_SHOWNORMAL); 
	ShowWindow(hwnd_accept, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL); 
	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);


	this->SetText();

	ShowWindow(hwnd_readgoal, SW_SHOWNORMAL); 

	return;
};


void cReadGoal::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_readgoal, SW_HIDE); 

	if (detailgoal->Active())
		detailgoal->Deactivate();
	if (reviewgoals->Active())
		reviewgoals->Deactivate();
	if (reportgoal->Active())
		reportgoal->Deactivate();

	if ((goalbook->InGoalBook(goal->ID())) && goalbook->Active())
		SendMessage(goalbook->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goalbook->Hwnd()); 
	else if (goals->Active())
		SendMessage(goals->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goals->Hwnd());
	else
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	goal = NULL;

	return;
}


// if text is available, display it; otherwise put loading message up
void cReadGoal::SetText()
{
	if (goal->Text())
	{
		// goal-is-in-goalbook feature restriction
		if (goalbook->InGoalBook(goal->ID()))
		{
			ShowWindow(hwnd_accept, SW_HIDE);
			ShowWindow(hwnd_report, SW_SHOWNORMAL);
		}
		else
		{
			ShowWindow(hwnd_accept, SW_SHOWNORMAL);
			ShowWindow(hwnd_report, SW_HIDE);
		}

		bool can_edit = false;
		bool can_complete = false;

#ifdef GAMEMASTER
#ifndef _DEBUG
		can_edit = can_complete = true;
#endif
#endif
			// goals can be read directly from goalbook, so goalposting "goals" 
			// object is not safe to use

		if (   (_tcscmp(goal->UpperPosterName(), player->UpperName()) == 0) &&
			(goal->Rank() == Guild::INITIATE))
			can_edit = can_complete = true;

		if    (_tcscmp(goal->UpperPosterName(), player->UpperName()) == 0)
			can_complete = true;

		//if ((goal->GuardiansManage() && 
			//(goal->Rank() == Guild::INITIATE) && 
			//(player->GuildRank(goal->GuildID()) > Guild::INITIATE)))
			//can_edit = true;

		if (can_edit)
		{
			ShowWindow(hwnd_edit, SW_SHOWNORMAL);
			ShowWindow(hwnd_complete, SW_SHOWNORMAL);
			ShowWindow(hwnd_delete, SW_SHOWNORMAL);
			ShowWindow(hwnd_accept, SW_HIDE);
		}
		else if (can_complete)
		{
			ShowWindow(hwnd_complete, SW_SHOWNORMAL);
			ShowWindow(hwnd_delete, SW_SHOWNORMAL);
			ShowWindow(hwnd_accept, SW_HIDE);
		} 
		else
		{
			ShowWindow(hwnd_edit, SW_HIDE);
			ShowWindow(hwnd_complete, SW_HIDE);
			ShowWindow(hwnd_delete, SW_HIDE);
		}

		if (goal->SugSphere() == Guild::SPHERE_ANY)
		{
			LoadString(hInstance, IDS_ANY_SPHERE, message, sizeof(message));
			Edit_SetText(hwnd_sugsphere, message);
		}
		else if (goal->SugSphere() == Guild::RULER_ISSUE)
		{
			LoadString(hInstance, IDS_RULER_ISSUE, message, sizeof(message));
			Edit_SetText(hwnd_sugsphere, message);
			ShowWindow(hwnd_accept, SW_HIDE);
		}
		else
		{
		LoadString(hInstance, IDS_SPHERE_I, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, goal->SugSphere());
			Edit_SetText(hwnd_sugsphere, message);
		}

		if (goal->SugStat() == Stats::NO_STAT)
		{
			LoadString(hInstance, IDS_ANY_STAT, message, sizeof(message));
			Edit_SetText(hwnd_sugstat, message);
		}
		else
		{
			TranslateValue(LyraItem::TRANSLATION_STAT, goal->SugStat());
			Edit_SetText(hwnd_sugstat, message);
		}
		Edit_SetText(hwnd_creator, goal->PosterName());
		Edit_SetText(hwnd_text, goal->Text());
	}
	else
	{
		gs->RequestGoalText(goal->ID());

		ShowWindow(hwnd_edit, SW_HIDE);
		ShowWindow(hwnd_accept, SW_HIDE);
		ShowWindow(hwnd_complete, SW_HIDE);
		ShowWindow(hwnd_delete, SW_HIDE);
		ShowWindow(hwnd_report, SW_HIDE);

		Edit_SetText(hwnd_sugstat, _T(""));
		Edit_SetText(hwnd_sugsphere, _T(""));
		Edit_SetText(hwnd_creator, _T(""));


		LoadString(hInstance, IDS_RETRIEVE_GOAL_INFO, message, sizeof(message));
		Edit_SetText(hwnd_text, message);
	}
}


void cReadGoal::AcceptGoal(void)
{
	goals->AcceptGoal(goal);
	this->Deactivate();

	return;
}

void cReadGoal::DeleteGoal(void)
{
#ifndef GAMEMASTER
	if (   (_tcscmp(goal->UpperPosterName(), player->UpperName()) == 0) 
		&& (goal->Rank() < abs(player->GuildRank(goal->GuildID()))))
#endif
	{
		LoadString (hInstance, IDS_DELETEGOAL_WARN, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, RankGoalName(goal->Rank()), RankGoalName(goal->Rank()), RankGoalName(goal->Rank()));
		goals->GuildWarning(&DeleteGoalCallback);
	}

	return;
}

void DeleteGoalCallback(void *value)
{
	int deletegoal = *((int*)value);

#ifndef GAMEMASTER  
	if (   (_tcscmp(readgoal->goal->UpperPosterName(), player->UpperName()) == 0) 
		&& (readgoal->goal->Rank() < abs(player->GuildRank(readgoal->goal->GuildID()))))
#endif
	{
		if (deletegoal == 1)
		{
			gs->DeleteGoal(readgoal->goal->ID());
			readgoal->Deactivate();
		}
		else
		{
			LoadString (hInstance, IDS_DELETEGOAL_ABORTED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, RankGoalName(goals->Rank()));
			goals->GuildError();
		}
	}
	
	return;
}

void cReadGoal::ReportGoal(void)
{
	BOOL igb = goalbook->InGoalBook(goal->ID());
	BOOL act = goals->Active();
	BOOL ig = (goals->Guild() == goal->GuildID());
	BOOL ir = (goals->Rank() == goal->Rank());		 
	if (act && igb && ig && ir)
		reportgoal->Activate(goal);
	else
	{
		LoadString (hInstance, IDS_GOALBOOK_WRONGBOARD, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, RankGoalName(goal->Rank()), GuildName(goal->GuildID()), 
			RankName(goal->Rank()));
		goals->GuildError();
	}

	return;
}


void cReadGoal::CompleteGoal(void)
{
	gs->CompleteGoal(goal->ID());
	Deactivate();

	return;
}


void cReadGoal::EditGoal(void)
{
	postgoal->Activate(goals->Guild(), goals->Rank(), goal);
	Deactivate();

	return;
}


void cReadGoal::DeleteAcknowledged(realmid_t goalid)
{
	LoadString (hInstance, IDS_EXPIREGOAL_ACK, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, RankGoalName(goals->Rank()));
	display->DisplayMessage(message);
}


void cReadGoal::DeleteError(void)
{
	LoadString (hInstance, IDS_EXPIREGOAL_ERROR, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, RankGoalName(goals->Rank()));
	display->DisplayMessage(message);
}


void cReadGoal::CompleteAcknowledged(void)
{
	LoadString (hInstance, IDS_COMPLETEGOAL_ACK, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, RankGoalName(goals->Rank()));
	display->DisplayMessage(message);
}


void cReadGoal::CompleteError(void)
{
	LoadString (hInstance, IDS_COMPLETEGOAL_ERROR, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, RankGoalName(goals->Rank()));
	display->DisplayMessage(message);
}


void cReadGoal::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_text, NULL, TRUE);
}


void cReadGoal::ScrollDown(int count)
{
	int line_count, first_visible, curr_count = count;
	line_count = SendMessage(hwnd_text, EM_GETLINECOUNT, 0, 0);
	if (line_count <= VISIBLE_LINES)
		return; // no scrolling until necessary

	first_visible = SendMessage(hwnd_text, EM_GETFIRSTVISIBLELINE, 0, 0); 

	while (curr_count && (line_count - first_visible > VISIBLE_LINES))
	{
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_LINEDOWN, 0);
		curr_count--;
		first_visible++;
	}

	InvalidateRect(hwnd_text, NULL, TRUE);

	return;
}


// Destructor
cReadGoal::~cReadGoal(void)
{
	if (hSphere!=NULL)
		DeleteObject(hSphere);
	if (hStat!=NULL)
		DeleteObject(hStat);
	if (hCreator!=NULL)
		DeleteObject(hCreator);
	if (hAccept!=NULL)
		DeleteObject(hAccept);
	if (hComplete!=NULL)
		DeleteObject(hComplete);
	if (hEdit!=NULL)
		DeleteObject(hEdit);
	if (hDelete!=NULL)
		DeleteObject(hDelete);
	if (hReport!=NULL)
		DeleteObject(hReport);
	if (hExit!=NULL)
		DeleteObject(hExit);
	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
	{
		if (text_buttons_bitmaps[i][0])
			DeleteObject(text_buttons_bitmaps[i][0]);
		if (text_buttons_bitmaps[i][1])
			DeleteObject(text_buttons_bitmaps[i][1]);
	}

	return;
}


// Window procedure for the read goal window
LRESULT WINAPI ReadGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;

	if (HBRUSH brush = SetControlColors(hwnd, message, wParam, lParam, true))
		return (LRESULT)brush; 

	switch(message)
	{
		case WM_PAINT:
			TileBackground(readgoal->hwnd_readgoal);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == readgoal->hwnd_accept)
				readgoal->AcceptGoal();
			else if ((HWND)lParam == readgoal->hwnd_complete)
				readgoal->CompleteGoal();
			else if ((HWND)lParam == readgoal->hwnd_delete)
				readgoal->DeleteGoal();
			else if ((HWND)lParam == readgoal->hwnd_report)
				readgoal->ReportGoal();
			else if ((HWND)lParam == readgoal->hwnd_exit)
				readgoal->Deactivate();
			else if ((HWND)lParam == readgoal->hwnd_edit)
				readgoal->EditGoal();
			break;

		case WM_KEYUP:
			if ((UINT)(wParam) == VK_ESCAPE)
			{
				readgoal->Deactivate();
				return 0L;
			}
			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Subclassed window procedure for the rich edit control
LRESULT WINAPI ReadGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	static WNDPROC lpfn_wproc;
    LPDRAWITEMSTRUCT lpdis; 
	int j;
	HDC dc;

	switch(message)
	{
		case WM_PASSPROC:
			lpfn_wproc = (WNDPROC) lParam;
			return (LRESULT) 0;
		case WM_KEYDOWN: // send the key to the main window
			SendMessage(readgoal->hwnd_readgoal, message,
				(WPARAM) wParam, (LPARAM) lParam);
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			SendMessage(readgoal->hwnd_readgoal, WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) readgoal->hwnd_readgoal);
			return (LRESULT) 0;
		case WM_COMMAND:
			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((HWND)lParam == readgoal->hwnd_text_buttons[j])
				{
					if (j == DOWN)
						readgoal->ScrollDown(1);
					else
						readgoal->ScrollUp(1);
					break;
				}
			}
			SendMessage(readgoal->hwnd_readgoal, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) readgoal->hwnd_readgoal);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == readgoal->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, readgoal->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == readgoal->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, readgoal->text_buttons_bitmaps[j][1]); 
			}
			
            BitBlt(lpdis->hDC,lpdis->rcItem.left, lpdis->rcItem.top,  
                lpdis->rcItem.right - lpdis->rcItem.left, 
                lpdis->rcItem.bottom - lpdis->rcItem.top, 
                dc, 0, 0, SRCCOPY); 
            DeleteDC(dc); 
			return TRUE; 


	    case WM_SETCURSOR:
			return 0;
	}  

	return CallWindowProc( lpfn_wproc, hwnd, message, wParam, lParam);
} 


// Check invariants

#ifdef CHECK_INVARIANTS

void cReadGoal::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
