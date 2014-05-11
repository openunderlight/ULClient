// Handles the main goal-posting screen

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cReadGoal.h"
#include "cChat.h"
#include "cReportGoal.h"
#include "cPostGoal.h"
#include "cReviewGoals.h"
#include "cGoalBook.h"
#include "cGoalPosting.h"
#include "cReadReport.h"
#include "Interface.h"
#include "cOutput.h"
#include "resource.h"
#include "cDetailGoal.h"
#include "dialogs.h"
#include "utils.h"
#include "cEffects.h"
#include "realm.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern cEffects *effects;
extern cDDraw *cDD;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cChat *display;
extern cGoalPosting *goals;
extern cReadGoal *readgoal;
extern cReportGoal *reportgoal;
extern cPostGoal *postgoal;
extern cGoalBook *goalbook;
extern cReviewGoals *reviewgoals;
extern cReadReport *readreport;
extern cDetailGoal *detailgoal;

//////////////////////////////////////////////////////////////////
// Constants

extern HBITMAP *hGoalCheckButtons;
extern WNDPROC lpfnGoalPushButtonProc;
extern WNDPROC lpfnGoalStateButtonProc;
extern WNDPROC lpfnGoalEditProc;
extern WNDPROC lpfnGoalComboBoxProc;

const int VISIBLE_LINES = 10;
const int DDOWN = 0;
const int DUP = 1;
const int DOWN = 2;
const int UP = 3;

// text scrolling buttons
struct button_t {
	window_pos_t position[MAX_RESOLUTIONS];
	int			 button_id;
	int			 bitmap_id;
};

const button_t text_buttons[NUM_GOAL_BUTTONS] = 
{
{ {{ 442, 125, 13, 25 }, { 556, 125, 13, 25 }, { 717, 125, 13, 25 } },
			DDOWN, LyraBitmap::CP_DDOWNA },   // ddown button
{ {{ 442, 0, 13, 25 }, { 556, 0, 13, 25 }, { 717, 0, 13, 25 } },
			DUP, LyraBitmap::CP_DUPA },   // dup button
{ {{ 442, 110, 13, 15 }, { 556, 110, 13, 15 }, { 717, 110, 13, 15 } },
			DOWN, LyraBitmap::CP_DOWNA },     // down button
{ {{ 442, 25, 13, 15 }, { 556, 25, 13, 15 }, { 717, 25, 13, 15 } },
			UP, LyraBitmap::CP_UPA }   // up button
};

const RECT button_strip[MAX_RESOLUTIONS] = 
{ { 100, 0, 125, 165 }, { 125, 0, 156, 206 }, { 160, 0, 200, 264 } };

// position for goal posting menu
const struct window_pos_t goalPostingPos[MAX_RESOLUTIONS]= 
{ { 0, 0, 480, 300 }, { 0, 0, 600, 375 }, { 0, 0, 768, 480 } };

// position for title text
const struct window_pos_t titlePos[MAX_RESOLUTIONS]=
{ { 0, 0, 350, 25 }, { 0, 0, 437, 31 }, { 0, 0, 560, 40 } };

// position for house logo
const struct window_pos_t logoPos[MAX_RESOLUTIONS]=
{ { 400, 0, 80, 80 }, { 500, 0, 100, 100 }, { 640, 0, 128, 128 } };

// position for summaries label
const struct window_pos_t summariestextPos[MAX_RESOLUTIONS]= 
{ { 15, 45, 100, 20 }, { 18, 56, 125, 25 }, { 24, 72, 160, 32 } };

// position for summaries list box
const struct window_pos_t summariesPos[MAX_RESOLUTIONS]= 
//{ { 5, 65, 460, 170 }, { 6, 81, 575, 212 }, { 8, 104, 736, 272 } };
{ { 5, 65, 460, 170 }, { 6, 101, 575, 170 }, { 8, 154, 736, 170 } };

// position for postgoal button
const struct window_pos_t postPos[MAX_RESOLUTIONS]= 
//{ { 110, 230, 70, 20 }, { 137, 287, 87, 25 }, { 176, 368, 112, 32 } };
{ { 110, 230, 70, 20 }, { 137, 287, 70, 20 }, { 176, 368, 70, 20 } };

// position for details button
const struct window_pos_t detailsPos[MAX_RESOLUTIONS]= 
//{ { 110, 260, 70, 20 }, { 137, 325, 87, 25 }, { 176, 416, 112, 32 } };
{ { 110, 260, 70, 20 }, { 137, 325, 70, 20 }, { 176, 416, 70, 20 } };

// position for review button
const struct window_pos_t reviewPos[MAX_RESOLUTIONS]= 
//{ { 200, 230, 70, 20 }, { 250, 287, 87, 25 }, { 320, 368, 112, 32 } };
{ { 200, 230, 70, 20 }, { 250, 287, 70, 20 }, { 320, 368, 70, 20 } };

// position for goal book button
const struct window_pos_t goalbookPos[MAX_RESOLUTIONS]= 
//{ { 200, 260, 70, 20 }, { 250, 325, 87, 25 }, { 320, 416, 112, 32 } };
{ { 200, 260, 70, 20 }, { 250, 325, 70, 20 }, { 320, 416, 70, 20 } };

// position for read goal button
const struct window_pos_t readPos[MAX_RESOLUTIONS]= 
//{ { 290, 230, 70, 20 }, { 362, 287, 87, 25 }, { 464, 368, 112, 32 } };
{ { 290, 230, 70, 20 }, { 362, 287, 70, 20 }, { 464, 368, 70, 20 } };

// position for accept goal button
const struct window_pos_t acceptPos[MAX_RESOLUTIONS]= 
//{ { 290, 260, 70, 20 }, { 362, 325, 87, 25 }, { 464, 416, 112, 32 } };
{ { 290, 260, 70, 20 }, { 362, 325, 70, 20 }, { 464, 416, 70, 20 } };

// position for refresh button
const struct window_pos_t refreshPos[MAX_RESOLUTIONS]= 
//{ { 380, 230, 70, 20 }, { 475, 287, 87, 25 }, { 608, 368, 112, 32 } };
{ { 380, 230, 70, 20 }, { 475, 287, 70, 20 }, { 608, 368, 70, 20 } };

// position for exit button
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
//{ { 380, 260, 70, 20 }, { 475, 325, 87, 25 }, { 608, 416, 112, 32 } };
{ { 380, 260, 70, 20 }, { 475, 325, 70, 20 }, { 608, 416, 70, 20 } };


/////////////////////////////////////////////////////////////////
// Class Definitions

/////////////////////////////////////////////////////////////////
// Goal Posting Class
cGoalPosting::cGoalPosting(void) 
{
#ifndef AGENT
	hGoalCheckButtons = CreateControlBitmaps(LyraBitmap::CHECK_BUTTON, 2);
#endif
	WNDCLASS wc;

	last_goal_seen = Lyra::ID_UNKNOWN;
	next_goal = 0;
	page_num = 0;
	m_rhCount = 0;
 	num_new_goals = 0;
	active = FALSE;
	guild = Guild::NO_GUILD;
	rank = Guild::NO_RANK;
	for (int i=0; i<MAX_SIMUL_GOALS; i++)
		curr_goals[i] = NULL;

	// Create Fonts
	goalFont = NULL;

	LOGFONT logFont;

	memset(&logFont, 0, sizeof(LOGFONT));

	logFont.lfHeight = 15;
	logFont.lfWeight = 500;
	logFont.lfEscapement = 0;
	logFont.lfItalic = 0;
	logFont.lfUnderline = 0;
	logFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	logFont.lfClipPrecision = CLIP_STROKE_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	_tcscpy(logFont.lfFaceName, GOAL_FONT_NAME);

	// set the control to use this font
	goalFont = CreateFontIndirect(&logFont); 


    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = GoalPostingWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("GoalPosting");

    RegisterClass( &wc );

    hwnd_goalposting = CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("GoalPosting"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		goalPostingPos[cDD->Res()].x, goalPostingPos[cDD->Res()].y, 		
		goalPostingPos[cDD->Res()].width, goalPostingPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_goalposting, WM_SETFONT, WPARAM(goalFont), 0);

	hwnd_title = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						titlePos[cDD->Res()].x, titlePos[cDD->Res()].y, 
						titlePos[cDD->Res()].width, titlePos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL);

	hwnd_logo = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						logoPos[cDD->Res()].x, logoPos[cDD->Res()].y, 
						logoPos[cDD->Res()].width, logoPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL);

	hwnd_summariestext = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						summariestextPos[cDD->Res()].x, summariestextPos[cDD->Res()].y, 
						summariestextPos[cDD->Res()].width, summariestextPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL);
	hSummaries = CreateWindowsBitmap(LyraBitmap::SUMMARIES_LABEL);
	SendMessage(hwnd_summariestext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSummaries));

	hwnd_summaries = CreateWindow(_T("listbox"), _T("Summaries"),
						WS_CHILD | WS_DLGFRAME | LBS_NOTIFY,
						summariesPos[cDD->Res()].x, summariesPos[cDD->Res()].y, 
						summariesPos[cDD->Res()].width, summariesPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL);
	SendMessage(hwnd_summaries, WM_SETFONT, WPARAM(goalFont), 0);
	lpfn_text = SubclassWindow(hwnd_summaries, GoalSummariesWProc);
	SendMessage(hwnd_summaries, WM_PASSPROC, 0, (LPARAM) lpfn_text ); 

	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
	{
		hwnd_text_buttons[i] = CreateWindow(_T("button"), _T(""),
				WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
				text_buttons[i].position[cDD->Res()].x, text_buttons[i].position[cDD->Res()].y, 
				text_buttons[i].position[cDD->Res()].width, text_buttons[i].position[cDD->Res()].height,
				hwnd_summaries, NULL, hInstance, NULL);
		SubclassGoalWindow(hwnd_text_buttons[i]);

		text_buttons_bitmaps[i][0] = // a button
			CreateWindowsBitmap(text_buttons[i].bitmap_id);
		text_buttons_bitmaps[i][1] = // b button
			CreateWindowsBitmap(text_buttons[i].bitmap_id + 1);
	}

	hwnd_goalbook = CreateWindow(_T("Button"), _T("Goal Book"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						goalbookPos[cDD->Res()].x, goalbookPos[cDD->Res()].y, 
						goalbookPos[cDD->Res()].width, goalbookPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL); 
	hGoalBook = CreateWindowsBitmap(LyraBitmap::GOAL_BOOK_BUTTON);
	SendMessage(hwnd_goalbook, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hGoalBook));
	SubclassGoalWindow(hwnd_goalbook);

	hwnd_read = CreateWindow(_T("Button"), _T("Read Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						readPos[cDD->Res()].x, readPos[cDD->Res()].y, 
						readPos[cDD->Res()].width, readPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL); 
	hReadGoal = CreateWindowsBitmap(LyraBitmap::READ_GOAL_BUTTON);
	SendMessage(hwnd_read, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hReadGoal));
	SubclassGoalWindow(hwnd_read);


	hwnd_accept = CreateWindow(_T("Button"), _T("Accept Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						acceptPos[cDD->Res()].x, acceptPos[cDD->Res()].y, 
						acceptPos[cDD->Res()].width, acceptPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL); 
	hAcceptGoal = CreateWindowsBitmap(LyraBitmap::ACCEPT_GOAL_BUTTON);
	SendMessage(hwnd_accept, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hAcceptGoal));
	SubclassGoalWindow(hwnd_accept);


	hwnd_exit = CreateWindow(_T("Button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL); 
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	hwnd_post = CreateWindow(_T("Button"), _T("Post Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						postPos[cDD->Res()].x, postPos[cDD->Res()].y, 
						postPos[cDD->Res()].width, postPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL); 
	hPostGoal = CreateWindowsBitmap(LyraBitmap::POST_GOAL_BUTTON);
	hPostOff = CreateWindowsBitmap(LyraBitmap::POST_GOAL_OFF);
	SendMessage(hwnd_post, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hPostGoal));
	SubclassGoalWindow(hwnd_post);

	hwnd_review = CreateWindow(_T("Button"), _T("Review Reports"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						reviewPos[cDD->Res()].x, reviewPos[cDD->Res()].y, 
						reviewPos[cDD->Res()].width, reviewPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL); 
	hReviewReports = CreateWindowsBitmap(LyraBitmap::SHOW_REPORTS_BUTTON);
	SendMessage(hwnd_review, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hReviewReports));
	SubclassGoalWindow(hwnd_review);

	hwnd_refresh = CreateWindow(_T("Button"), _T("Refresh"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						refreshPos[cDD->Res()].x, refreshPos[cDD->Res()].y, 
						refreshPos[cDD->Res()].width, refreshPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL); 
	hRefresh = CreateWindowsBitmap(LyraBitmap::REFRESH_LIST_BUTTON);
	SendMessage(hwnd_refresh, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hRefresh));
	SubclassGoalWindow(hwnd_refresh);

	hwnd_details = CreateWindow(_T("Button"), _T("Show Details"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						detailsPos[cDD->Res()].x, detailsPos[cDD->Res()].y, 
						detailsPos[cDD->Res()].width, detailsPos[cDD->Res()].height,
						hwnd_goalposting,
						NULL, hInstance, NULL);
	hShowDetails = CreateWindowsBitmap(LyraBitmap::SHOW_DETAILS_BUTTON);
	hDetailsOff = CreateWindowsBitmap(LyraBitmap::SHOW_DETAILS_OFF);
	SendMessage(hwnd_details, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hShowDetails));
	SubclassGoalWindow(hwnd_details);

	return;

}

void cGoalPosting::Activate(int trip_guild, int trip_rank)
{ 
    guild = trip_guild;
	rank = trip_rank;

	// Get goalbook headers
	if (goalbook->NumGoals() == 0)
		gs->RequestGoalbook();

	if (player->GuildRank(guild) == Guild::NO_RANK)
	{ // not member of guild
	   	LoadString (hInstance, IDS_GUILD_NOTMEMBER, message, sizeof(message));
	_stprintf(disp_message, message, GuildName(guild));
		display->DisplayMessage (disp_message);
		return;
	}

	if (rank > abs(player->GuildRank(guild)))
	{ // member of guild, but lacks rank
	   	LoadString (hInstance, IDS_GUILD_LOWRANK, message, sizeof(message));
	_stprintf(disp_message, message, GuildName(guild));
		display->DisplayMessage (disp_message);
		return;
	}

	if ((active) || !gs->LoggedIntoLevel())
		return;

	active = TRUE;
	page_num = 0;
	last_goal_seen = Lyra::ID_UNKNOWN;

//RRR 01/19/00 - Re-enabled so you log into the message board and not stand outside.
	gs->LevelLogout(RMsg_Logout::GOALBOOK); 

	if (player->GuildRank(guild) > rank) 
	{ // tell higher ranks how much xp pool is left
		LoadString (hInstance, IDS_XPPOOL_REMAINING, message, sizeof(message));
	_stprintf(disp_message, message, player->GuildXPPool(guild), GuildName(guild));
		display->DisplayMessage (disp_message);
	}

	if (readgoal->Active())
		readgoal->Deactivate();
	if (goalbook->Active())
		goalbook->Deactivate();
	if (postgoal->Active())
		postgoal->Deactivate();
	if (reportgoal->Active())
		reportgoal->Deactivate();
	if (reviewgoals->Active())
		reviewgoals->Deactivate();
	if (readreport->Active())
		readreport->Deactivate();
	if (detailgoal->Active())
		detailgoal->Deactivate();

	// use session id so we can identify stale messages 
	session_id = rand();

	// erase the viewport and show this window
	cDD->EraseSurface(BACK_BUFFER);
	cDD->BlitOffScreenSurface();

	hTitle = CreateWindowsBitmap(LyraBitmap::HOUSE_GOALBOARD_TITLES+guild+((rank-1)*NUM_GUILDS));
	SendMessage(hwnd_title, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hTitle);

	hLogo = CreateWindowsBitmap(LyraBitmap::HOUSE_BITMAPS+guild+((rank-1)*NUM_GUILDS));
	SendMessage(hwnd_logo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogo);

	ShowWindow(hwnd_title, SW_SHOWNORMAL); 
	ShowWindow(hwnd_logo, SW_SHOWNORMAL);
	ShowWindow(hwnd_summariestext, SW_SHOWNORMAL);
	ShowWindow(hwnd_summaries, SW_SHOWNORMAL); 
	ShowWindow(hwnd_goalbook, SW_SHOWNORMAL); 
	ShowWindow(hwnd_read, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL); 
	ShowWindow(hwnd_refresh, SW_SHOWNORMAL);


	if (player->GuildRank(guild) >= Guild::KNIGHT)
		ShowWindow(hwnd_review, SW_SHOWNORMAL); 
	else
		ShowWindow(hwnd_review, SW_HIDE); 

	// Show Accept Goal Button if player is same or higher rank as this goal board
	if (rank <= abs(player->GuildRank(guild)))
		ShowWindow(hwnd_accept, SW_SHOWNORMAL);
	else
		ShowWindow(hwnd_accept, SW_HIDE);


	// Show PostGoal and Details buttons if player is high enough rank to use them
	if (rank < abs(player->GuildRank(guild)))
	{
		SendMessage(hwnd_post, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hPostGoal));
		SendMessage(hwnd_details, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hShowDetails));
		ShowWindow(hwnd_post, SW_SHOWNORMAL);
		ShowWindow(hwnd_details, SW_SHOWNORMAL);
	}
	else if (rank == 2)
	{
		SendMessage(hwnd_details, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hDetailsOff));
		SendMessage(hwnd_post, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hPostOff));
		ShowWindow(hwnd_post, SW_SHOWNORMAL);
		ShowWindow(hwnd_details, SW_SHOWNORMAL);
	}
	else 
	{
		ShowWindow(hwnd_post, SW_HIDE);
		ShowWindow(hwnd_details, SW_HIDE);
	}

	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

	gs->RequestGoalHeaders(session_id, guild, rank, last_goal_seen);

	ShowWindow(hwnd_goalposting, SW_SHOWNORMAL);

//	cGoal* null_goal = new cGoal;
	reviewgoals->Activate(NO_GOAL);
//	delete null_goal;
	
	return;
};

void cGoalPosting::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_goalposting, SW_HIDE); 

	if (hTitle!=NULL)
		DeleteObject(hTitle);
	if (hLogo!=NULL)
		DeleteObject(hLogo);

	if (readgoal->Active())
		readgoal->Deactivate();
	if (goalbook->Active())
		goalbook->Deactivate();
	if (postgoal->Active())
		postgoal->Deactivate();
	if (reportgoal->Active())
		reportgoal->Deactivate();
	if (reviewgoals->Active())
		reviewgoals->Deactivate();
	if (readreport->Active())
		readreport->Deactivate();
	if (detailgoal->Active())
		detailgoal->Deactivate();

	for (int i=0; i<next_goal; i++)
	{
		ListBox_DeleteString(hwnd_summaries, 0);
		delete curr_goals[i];
	}

	next_goal = 0;
	page_num = 0;
	last_goal_seen = Lyra::ID_UNKNOWN;

//RRR 01/19/00 - Re-enabled so you log into the message board and not stand outside.
	gs->LevelLogin();	

	return;
}


void cGoalPosting::DBUnavailable(void)
{
	LoadString (hInstance, IDS_DB_UNAVAILABLE, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);

	this->Deactivate();
}


// summary info has arrived for a new goal
void cGoalPosting::NewGoalInfo(int sessionid, realmid_t goal_id, short status, short playeroption, const TCHAR *goal_summary)
{	
	int num_goals_shown;

	// make sure the goalposting screen is active and is the requestor
	if ((!active) || (this->session_id != sessionid))
		return;

	for (int i = 0; i < next_goal; i++)
		if (goal_id == curr_goals[i]->ID())
		{ // goal is already in the array
			ListBox_AddString(hwnd_summaries, goal_summary);
			num_goals_shown = ListBox_GetCount(hwnd_summaries);
			ListBox_SetItemData(hwnd_summaries, num_goals_shown-1, goal_id);
			return;
		}

	// make sure we don't overrun allocated memory
	if (next_goal == MAX_SIMUL_GOALS)
		return;

	// this must really be _new_ goal info
	curr_goals[next_goal] = new cGoal();
	curr_goals[next_goal]->SetInfo(goal_id, guild, rank, status, 
		playeroption, goal_summary);
	next_goal++;
	num_new_goals++;

	if (player->GuildRank(guild) > rank)
	{
		switch (status)
		{
			// commented out stuff is for possible future use
			case Guild::GOAL_COMPLETED:
			{
	   			LoadString (hInstance, IDS_GUILD_GOALCOMPLETED, message, sizeof(message));
			_stprintf(disp_message, message, goal_summary);
				break;
			}
			case Guild::GOAL_MAX_ACCEPTED:
			{
	   			LoadString (hInstance, IDS_GUILD_GOALMAXACCEPTED, message, sizeof(message));
			_stprintf(disp_message, message, goal_summary);
				break;
			}
			case Guild::GOAL_TIME_EXPIRED:
			{
	   			LoadString (hInstance, IDS_GUILD_GOALTIMEEXPIRED, message, sizeof(message));
			_stprintf(disp_message, message, goal_summary);
				break;
			}
			case Guild::GOAL_PENDING_VOTE:
			{
				if (playeroption)
		   			LoadString (hInstance, IDS_GUILD_GOALVOTED, message, sizeof(message));
				else
					LoadString (hInstance, IDS_GUILD_GOALPENDINGVOTE, message, sizeof(message));
			_stprintf(disp_message, message, goal_summary);
				break;
			}
			case Guild::GOAL_VOTED_DOWN:
			{
		   		LoadString (hInstance, IDS_GUILD_GOALVOTEDDOWN, message, sizeof(message));
			_stprintf(disp_message, message, goal_summary);
				break;
			}
			case Guild::GOAL_RULER_VOTE:
			{
				if (playeroption)
		   			LoadString (hInstance, IDS_GUILD_GOALVOTED, message, sizeof(message));
				else
					LoadString (hInstance, IDS_GUILD_GOALPENDINGVOTE, message, sizeof(message));
			_stprintf(disp_message, message, goal_summary);
				break;
			}
			case Guild::GOAL_RULER_PASSED:
			{
		   		LoadString (hInstance, IDS_GUILD_GOALRULERPASSED, message, sizeof(message));
			_stprintf(disp_message, message, goal_summary);
				break;
			}
			case Guild::GOAL_RULER_FAILED:
			{
		   		LoadString (hInstance, IDS_GUILD_GOALRULERFAILED, message, sizeof(message));
			_stprintf(disp_message, message, goal_summary);
				break;
			}
			default:
			{
			_stprintf(disp_message, goal_summary);
				break;
			}
		}
	}
	else
	_stprintf(disp_message, goal_summary);

	ListBox_AddString(hwnd_summaries, disp_message);
	num_goals_shown = ListBox_GetCount(hwnd_summaries);
	ListBox_SetItemData(hwnd_summaries, num_goals_shown-1, goal_id);

	if (num_goals_shown == VISIBLE_LINES)
		last_goal_seen = goal_id;

	if (ListBox_GetCurSel(hwnd_summaries) == -1)
		ListBox_SetCurSel(hwnd_summaries,0);
}


// goal text has arrived
void cGoalPosting::NewGoalText(realmid_t goal_id, const TCHAR *poster,
		int sugsphere, int sugstat, const TCHAR *text)
{	
	

	if (!active)
		return;

	for (int i=0; i<next_goal; i++)
	{
		if (curr_goals[i]->ID() == goal_id)
		{ // found a match, set text
			curr_goals[i]->SetText(poster, sugsphere, sugstat, text);
			if ((readgoal->Active()) && (readgoal->CurrGoalID() == goal_id))
				readgoal->SetText();
			if (postgoal->Active() && (postgoal->CurrGoalID() == goal_id))
				postgoal->SetText();
			return;
		}
	}
	NONFATAL_ERROR(IDS_GOAL_ERR1);
	return;
}

// goal details have arrived
void cGoalPosting::NewGoalDetails(realmid_t goal_id, int goal_rank, int maxacceptances, 
		int expirationtime, int yesvotes, int novotes, int voteexpiration,
		int status, int flags, int numacceptees, pname_t *acceptees)
{
	if (!active)
		return;

	for (int i=0; i<next_goal; i++)
	{
		if (curr_goals[i]->ID() == goal_id)
		{ // found a match, set details
			curr_goals[i]->SetDetails(goal_rank, maxacceptances, expirationtime, yesvotes,
				novotes, voteexpiration, status, flags, numacceptees, 
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _T(""), acceptees);
			if (detailgoal->Active() && (detailgoal->CurrGoalID() == goal_id))
				detailgoal->SetDetails();
			if (postgoal->Active() && (postgoal->CurrGoalID() == goal_id))
				postgoal->SetDetails();
			return;
		}
	}
	NONFATAL_ERROR(IDS_GOAL_ERR2);
	return;
}


// goal details have arrived
void cGoalPosting::SetGuardianFlag(realmid_t goal_id)
{
	if (!active)
		return;

	for (int i=0; i<next_goal; i++)
	{
		if (curr_goals[i]->ID() == goal_id)
		{ // found a match, set details
			curr_goals[i]->SetGuardiansManageFlag(1);

			if (readreport->Active())
				readreport->SetText();

			return;
		}
	}
	NONFATAL_ERROR(IDS_GOAL_ERR3);
	return;
}


// User requests next page of goals
void cGoalPosting::NextPage(void)
{
	// don't bother if there isn't a full page already
	if (ListBox_GetCount(hwnd_summaries) < 10)
		return;

	if (next_goal < (page_num+2)*10)
	{
		// if we are on the last page, and not enough new goals to fill a 
		// new page were last gotten, return
		if (next_goal <= ((page_num+1)*10))
		{	
			if (num_new_goals < 10)
				return;
		}
		
		ListBox_ResetContent(hwnd_summaries);
		page_num++;
		num_new_goals = 0;
		gs->RequestGoalHeaders(session_id, guild, rank, last_goal_seen);
	}
	else
	{
		ListBox_ResetContent(hwnd_summaries);
		page_num++;

		for (int i = 0; i < 10; i++)
		{
			ListBox_AddString(hwnd_summaries, curr_goals[(page_num*10)+i]->Summary());
			ListBox_SetItemData(hwnd_summaries, i, curr_goals[(page_num*10)+i]->ID());
		}

		ListBox_SetCurSel(hwnd_summaries, 0);
	}
}


void cGoalPosting::NextLine(void)
{
	int lines_displayed = ListBox_GetCount(hwnd_summaries);
	int current_line = ListBox_GetCurSel(hwnd_summaries);

	if ((current_line == LB_ERR) && (lines_displayed > 0))
		ListBox_SetCurSel(hwnd_summaries, 0);
	else 
	{
		current_line++;
		if (current_line >= VISIBLE_LINES)
			NextPage();
		else if (current_line < lines_displayed)
		{
			ListBox_SetCurSel(hwnd_summaries, current_line);
			for (int i=0; i<NUM_GOAL_BUTTONS; i++)
				InvalidateRect(hwnd_text_buttons[i], NULL, false);
		}
	}
}


void cGoalPosting::PrevLine(void)
{
	int lines_displayed = ListBox_GetCount(hwnd_summaries);
	int current_line = ListBox_GetCurSel(hwnd_summaries);

	if ((current_line == LB_ERR) && (lines_displayed > 0))
		ListBox_SetCurSel(hwnd_summaries, 0);
	else 
	{
		current_line--;
		if (current_line < 0)
			PrevPage();
		else
		{
			ListBox_SetCurSel(hwnd_summaries, current_line);
			for (int i=0; i<NUM_GOAL_BUTTONS; i++)
				InvalidateRect(hwnd_text_buttons[i], NULL, false);
		}
	}
}


// User requests prev page of goals
void cGoalPosting::PrevPage(void)
{
	if (page_num > 0)
		page_num--;
	else
		return;
	
	ListBox_ResetContent(hwnd_summaries);
	
	for (int i = 0; i < 10; i++)
	{
		ListBox_AddString(hwnd_summaries, curr_goals[(page_num*10)+i]->Summary());
		ListBox_SetItemData(hwnd_summaries, i, curr_goals[(page_num*10)+i]->ID());
	}
	
	ListBox_SetCurSel(hwnd_summaries, VISIBLE_LINES-1);
}


void cGoalPosting::ReadGoal(void)
{	
	int selected = ListBox_GetCurSel(hwnd_summaries);
	if (selected == -1)
		return;
	
	int searchgoal = ListBox_GetItemData(hwnd_summaries, selected);
	
	for (int i=0; i<next_goal; i++)
	{
		if (curr_goals[i]->ID() == searchgoal)
		{ // found the goal, activate readgoal
			readgoal->Activate(curr_goals[i]);
			return;
		}
	}
	NONFATAL_ERROR(IDS_GOAL_ERR4);
	return;
}

void cGoalPosting::ReadGoalDetails(void)
{	
	
	if (rank >= (abs(player->GuildRank(guild))))
	{ // member of guild, but lacks rank for this function
		LoadString (hInstance, IDS_GUILD_LOWRANK, message, sizeof(disp_message));
	_stprintf(disp_message, message, GuildName(guild));
		display->DisplayMessage (disp_message);
		return;
	}
	
	int selected = ListBox_GetCurSel(hwnd_summaries);
	if (selected == -1)
		return;
	
	int searchgoal = ListBox_GetItemData(hwnd_summaries, selected);
	
	for (int i=0; i<next_goal; i++)
	{
		if (curr_goals[i]->ID() == searchgoal)
		{ // found the goal, activate readgoal
			
			bool can_read_reports = false;
			for (int j=0; j<MAX_READABLE; j++)
				if (m_reportHeaders[j] == curr_goals[i]->ID())
					can_read_reports = true;
				
				if (!can_read_reports)
				{
					LoadString (hInstance, IDS_NO_DETAIL_VIEW, message, sizeof(message));
					display->DisplayMessage(message);
				}
				else
				{
					detailgoal->Activate(curr_goals[i]);
					readgoal->Activate(curr_goals[i]);
				}
				return;
		}
	}
	NONFATAL_ERROR(IDS_GOAL_ERR5);
	return;
}

void cGoalPosting::Post(void)
{	
	if (rank >= (abs(player->GuildRank(guild))))
	{ // member of guild, but lacks rank for this function
		LoadString (hInstance, IDS_GUILD_LOWRANK, message, sizeof(disp_message));
	_stprintf(disp_message, message, GuildName(guild));
		display->DisplayMessage (disp_message);
		return;
	}
	
	postgoal->Activate(goals->guild, goals->rank, NULL);
	
	return;
}


void cGoalPosting::AcceptGoal(cGoal *new_goal)
{
	if (goalbook->NumGoals() >= Lyra::MAX_ACTIVE_GOALS)
	{
		LoadString (hInstance, IDS_GOALBOOK_FULL, message, sizeof(disp_message));
		goals->GuildError();
	}
	else if (goalbook->InGoalBook(new_goal->ID()))
	{
		LoadString (hInstance, IDS_GOALBOOK_ALREADY_HAVE, disp_message, sizeof(message));
		_stprintf(message, disp_message, RankGoalName(goals->Rank()));
		goals->GuildError();
	}
	else
		gs->AcceptGoal(new_goal->ID());
	
	return;
}



void cGoalPosting::RefreshSummaries(void)
{
	ListBox_ResetContent(hwnd_summaries);
	
	for (int i=0; i<next_goal; i++)
	{
		ListBox_DeleteString(hwnd_summaries, 0);
		delete curr_goals[i];
	}
	
	next_goal = 0;
	page_num = 0;
	last_goal_seen = 0;
	session_id++;
	
	gs->RequestGoalHeaders(session_id, guild, rank, last_goal_seen);
	
	return;
}


void cGoalPosting::DisplayGoalBook(void)
{
	goalbook->Activate();
	return;
}


void cGoalPosting::ShowReports(void)
{	
	int selected = ListBox_GetCurSel(hwnd_summaries);
	if (selected == -1)
		return;
	
	int searchgoal = ListBox_GetItemData(hwnd_summaries, selected);
	
	for (int i=0; i<next_goal; i++)
		if (curr_goals[i]->ID() == searchgoal)
		{ // found the goal, activate readgoal
			bool can_read_reports = false;
			for (int j=0; j<MAX_READABLE; j++)
				if (m_reportHeaders[j] == curr_goals[i]->ID())
					can_read_reports = true;
				
				if (can_read_reports)
				{
					reviewgoals->Activate(curr_goals[i]->ID());
					readgoal->Activate(curr_goals[i]);
				}
				else
			{
		   	LoadString (hInstance, IDS_NO_REPORT_VIEW, message, sizeof(message));
			display->DisplayMessage(message);
			}
			return;
		}

	NONFATAL_ERROR(IDS_GOAL_ERR6);
	return;
}


void cGoalPosting::GoalNotFound(realmid_t goal_id)
{
	if (readgoal->Active())
		readgoal->Deactivate();
	if (detailgoal->Active())
		detailgoal->Deactivate();
	if (reviewgoals->Active())
		reviewgoals->Deactivate();
	if (goalbook->Active())
		goalbook->Deactivate();
	if (goalbook->InGoalBook(goal_id))
		goalbook->RemoveGoal(goal_id);

	LoadString (hInstance, IDS_GOAL_NOTFOUND, disp_message, sizeof(disp_message));
	if (goals->Active())
	_stprintf(message, disp_message, RankGoalName(goals->Rank()));
	else
	_stprintf(message, disp_message, RankGoalName(2));
	display->DisplayMessage(message);
}


// the error string should be loaded into message before calling here
void cGoalPosting::GuildError(void)
{
	CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR, 
				cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
	return;
}

void cGoalPosting::GuildWarning(goal_warning_callback_t callbackproc)
{
	HWND hDlg;
	hDlg = CreateLyraDialog(hInstance, IDD_WARNING_YESNO, 
				cDD->Hwnd_Main(), (DLGPROC)WarningYesNoDlgProc);
	SendMessage(hDlg, WM_SET_CALLBACK, 0,  (LPARAM)callbackproc);
	return;
}


// Destructor
cGoalPosting::~cGoalPosting(void)
{
	if (hGoalCheckButtons!=NULL)
		DeleteControlBitmaps(hGoalCheckButtons);
	if (goalFont!=NULL)
		DeleteObject(goalFont);
	if (hSummaries!=NULL)
		DeleteObject(hSummaries);
	if (hAcceptGoal!=NULL)
		DeleteObject(hAcceptGoal);
	if (hShowDetails!=NULL)
		DeleteObject(hShowDetails);
	if (hReadGoal!=NULL)
		DeleteObject(hReadGoal);
	if (hGoalBook!=NULL)
		DeleteObject(hGoalBook);
	if (hExit!=NULL)
		DeleteObject(hExit);
	if (hPostGoal!=NULL)
		DeleteObject(hPostGoal);
	if (hReviewReports!=NULL)
		DeleteObject(hReviewReports);
	if (hRefresh!=NULL)
		DeleteObject(hRefresh);
	if (hPostOff!=NULL)
		DeleteObject(hPostOff);
	if (hDetailsOff!=NULL)
		DeleteObject(hDetailsOff);
	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
	{
		if (text_buttons_bitmaps[i][0])
			DeleteObject(text_buttons_bitmaps[i][0]);
		if (text_buttons_bitmaps[i][1])
			DeleteObject(text_buttons_bitmaps[i][1]);
	}
	return;
}


// Subclassed window procedure for the rich edit control
LRESULT WINAPI GoalSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
			SendMessage(goals->hwnd_goalposting, message,
				(WPARAM) wParam, (LPARAM) lParam);
			return 0L;

		case WM_KEYUP:
			switch ((UINT)(wParam)) {
				case VK_ESCAPE:
					if (readgoal->Active())
						readgoal->Deactivate();
					else
						goals->Deactivate();
					return 0L;
				case VK_RETURN:
					goals->ReadGoal();
					return 0L;
				case VK_UP:
					goals->PrevLine();
					return 0L;
				case VK_DOWN:
					goals->NextLine();
					return 0L;
				default:
					break;
			}
			break;

		case WM_LBUTTONDOWN:
		case LB_SETCURSEL:
			{
				for (int i=0; i<NUM_GOAL_BUTTONS; i++)
					InvalidateRect(goals->hwnd_text_buttons[i], NULL, false);
			}
			break;

		case WM_COMMAND:
			for (j=0; j<NUM_GOAL_BUTTONS; j++)
			{
				if ((HWND)lParam == goals->hwnd_text_buttons[j])
				{
					if (j == DDOWN)
						goals->NextPage();
					else if (j == DUP)
						goals->PrevPage();
					else if (j == DOWN)
						goals->NextLine();
					else 
						goals->PrevLine();
					break;
				}
			}
			SendMessage(goals->hwnd_goalposting, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) goals->hwnd_goalposting);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 

			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_GOAL_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == goals->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, goals->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == goals->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, goals->text_buttons_bitmaps[j][1]); 
			}
		
			//RECT src;  
			//PrepareSrcRect(&src, &lpdis->rcItem, NOSTRETCH);

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


// Window procedure for the goal posting window
LRESULT WINAPI GoalPostingWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int selected_goal = -1;
	int style;

	switch(message)
	{
		case WM_PAINT:
			if (goals->rank < 2)
				TileBackground(goals->hwnd_goalposting);
			else
				TileBackground(goals->hwnd_goalposting, GOLD_BACKGROUND);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == goals->hwnd_read)
				goals->ReadGoal();
			else if ((HWND)lParam == goals->hwnd_goalbook)
				goals->DisplayGoalBook();
			else if ((HWND)lParam == goals->hwnd_accept)
			{

				selected_goal = ListBox_GetCurSel(goals->hwnd_summaries);
				if (selected_goal != -1)
					goals->AcceptGoal(goals->curr_goals[goals->page_num*10 + selected_goal]);
			}

			else if ((HWND)lParam == goals->hwnd_refresh)
				goals->RefreshSummaries();
			else if ((HWND)lParam == goals->hwnd_exit)
				goals->Deactivate();
			else if ((HWND)lParam == goals->hwnd_post)
				goals->Post();
			else if ((HWND)lParam == goals->hwnd_details)
				goals->ReadGoalDetails();
			else if ((HWND)lParam == goals->hwnd_review)
				goals->ShowReports();
			else if ((HWND)lParam == goals->hwnd_summaries)
			{
				switch (HIWORD(wParam))
				{
					case LBN_DBLCLK:
						goals->ReadGoal();
						for (int i=0; i<NUM_GOAL_BUTTONS; i++)
							InvalidateRect(goals->hwnd_text_buttons[i], NULL, false);
						break;
				}
			}
			break;

		case WM_KEYUP:
			switch ((UINT)(wParam)) {
				case VK_ESCAPE:
					goals->Deactivate();
					return 0L;
				case VK_RETURN:
					goals->ReadGoal();
					return 0L;
				case VK_UP:
					goals->PrevLine();
					return 0L;
				case VK_DOWN:
					goals->NextLine();
					return 0L;
				default:
					break;
			}

			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Check invariants

#ifdef CHECK_INVARIANTS

void cGoalPosting::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif

///////////////////////////////////////////////////////////////////////
// Goal Class

// Constants

// Constructor
cGoal::cGoal(void) 
{
	id = Lyra::ID_UNKNOWN;
	guild_id = Guild::NO_GUILD;
	poster_name[0]='\0';
	upper_poster_name[0]='\0';
	rank = Guild::NO_RANK;
	sug_sphere = 99; // -1, -2 used as flags
	sug_stat = Stats::NO_STAT;
	max_acceptances = -1;
	expiration_time = 0;
	yes_votes = 0;
	no_votes = 0;
	vote_expiration = 0;
	statusflags = 0;
	otherflags = 0;
	num_acceptees = 0;
	acceptees = NULL;
	voted = has_details = false;
	length = 0;
	text = NULL;
	
	all_guardians_manage = -1;

	return;
}

// Assignment operator
cGoal& cGoal::operator=(const cGoal& x)
{
	if (text) 
	{
		delete [] text;
		text = NULL;
	}
	length = 0;
	if (acceptees)
	{
		delete [] acceptees;
		acceptees = NULL;
	}

	this->SetInfo(x.id, x.guild_id, x.rank, x.statusflags, x.playeroption, x.summary);
	this->SetText(x.poster_name, x.sug_sphere, x.sug_stat, x.text);
	this->SetDetails(x.rank, x.max_acceptances, x.expiration_time, 
					 x.yes_votes, x.no_votes, x.vote_expiration,
					 x.statusflags, x.otherflags, x.num_acceptees,  x.num_completees, 
					 0, 0, 0, 0, 0, 0, 0, 0, 0, _T(""), x.acceptees);
	this->SetGuardiansManageFlag(x.all_guardians_manage);

	return (cGoal&)(*this);
}


// set header info - all except for text
void cGoal::SetInfo(realmid_t goal_id, guildid_t guild,
			 int goal_rank, short status_flags, short player_option, 
			 const TCHAR *goal_summary)
{
	id = goal_id;
	guild_id = guild;
	rank = goal_rank;
	statusflags = status_flags;
	playeroption = player_option;

	_tcscpy(summary, goal_summary);

	return;
}


// Set the text for the goal
void cGoal::SetText(const TCHAR *post_name, int sugsphere, int sugstat, const TCHAR *goal_text)
{
	sug_sphere = sugsphere;
	sug_stat = sugstat;
    _tcscpy(poster_name, post_name);
	_tcscpy(upper_poster_name, post_name);
	_tcsupr(upper_poster_name);

	if (text)
	{
		NONFATAL_ERROR(IDS_GOAL_ERR8);
		delete [] text;
		text = NULL;
	}

	if (goal_text)
	{
		length = _tcslen(goal_text);
		text = new TCHAR[length+1];
		_tcscpy(text, goal_text);
	}
	else
	{
		length = 0;
		text = NULL;
	}

	return;
}

// Set the details for the goal
void cGoal::SetDetails(int goal_rank, int maxacceptances, int expirationtime, 
					   int yesvotes, int novotes, int voteexpiration, int goal_status,
					   int goal_flags, int numacceptees, int quest_graphic, int quest_charges, 
					   int quest_color1, int quest_color2, int quest_item_type, int quest_field1, int quest_field2,
					   int quest_field3, int quest_xp_, int numcompletees,
					   const TCHAR* quest_keywords,
					   pname_t *goal_acceptees)
{
	rank = goal_rank;
	max_acceptances = maxacceptances;
	expiration_time = expirationtime;
	yes_votes = yesvotes;
	no_votes = novotes;
	vote_expiration = voteexpiration;
	statusflags = goal_status;
	num_acceptees = numacceptees;
	num_completees = numcompletees;
	otherflags = goal_flags;
	graphic = quest_graphic;
	charges = quest_charges;
	color1 = quest_color1;
	color2 = quest_color2;
	item_type = quest_item_type;
	field1 = quest_field1;
	field2 = quest_field2;
	field3 = quest_field3;
	quest_xp = quest_xp_;
	_tcscpy(keywords, quest_keywords);
	has_details = true;

	if (acceptees)
	{
		NONFATAL_ERROR(IDS_GOAL_ERR9);
		delete [] acceptees;
		acceptees = NULL;
	}

	if (goal_acceptees)
	{
		if (statusflags < Guild::GOAL_PENDING_VOTE)
		{
			acceptees = new pname_t[num_acceptees];
			for (int i = 0; i < num_acceptees; i++)
				_tcscpy(acceptees[i], goal_acceptees[i]);
		}
		else
		{
			acceptees = new pname_t[(yes_votes + no_votes)];
			for (int i = 0; i < (yes_votes + no_votes); i++)
				_tcscpy(acceptees[i], goal_acceptees[i]);
		}
	}
	else
		acceptees = NULL;

	return;
}


void cGoal::SetGuardiansManageFlag(int manageflag)
{
	all_guardians_manage = manageflag;
	if (manageflag == 1)
		otherflags = otherflags | GOAL_GUARDIAN_MANAGE_FLAG;
}


// Destructor
cGoal::~cGoal(void)
{
	if (text)
		delete [] text;
	text = NULL;
	if (acceptees)
		delete [] acceptees;
	acceptees = NULL;
	return;
}

///////////////////////////////////////////////////////////////////////
// Report Class

// Constants

// Constructor
cReport::cReport(void)
{
//	goal = NULL;
	goal_id = Lyra::ID_UNKNOWN;
	report_id = Lyra::ID_UNKNOWN;
	reporter_name[0]='\0';
	upper_reporter_name[0]='\0';
	recipient[0]='\0';
	upper_recipient[0]='\0';
	awardxp = 0;
	length = 0;
	text = NULL;

	return;
}


// set header info - all except for text
void cReport::SetInfo(realmid_t goal, realmid_t report, const TCHAR *report_summary)
{
	goal_id = goal;
	report_id = report;

	_tcscpy(summary, report_summary);

	return;
}


// Set the text for the goal
void cReport::SetText(int award, int reportflags, const TCHAR *post_name, const TCHAR* recip_name, const TCHAR *report_text)
{
	awardxp = award;
	flags = reportflags;
    _tcscpy(reporter_name, post_name);
	_tcscpy(upper_reporter_name, post_name);
	_tcsupr(upper_reporter_name);
	_tcscpy(recipient, recip_name);
	_tcscpy(upper_recipient, recip_name);
	_tcsupr(upper_recipient);

	if (text)
	{
		NONFATAL_ERROR(IDS_GOAL_ERR10);
		delete [] text;
		text = NULL;
	}

	if (report_text)
	{
		length = _tcslen(report_text);
		text = new TCHAR[length+1];
		_tcscpy(text, report_text);
	}
	else
	{
		length = 0;
		text = NULL;
	}

	return;

}


// Assignment operator
cReport& cReport::operator=(const cReport& x)
{
	if (text) 
	{
		delete [] text;
		text = NULL;
	}
	length = 0;

	this->SetInfo(x.goal_id, x.report_id, (TCHAR*)x.summary);
	this->SetText(x.awardxp, x.flags, (TCHAR*)x.reporter_name, 
		(TCHAR*)x.recipient, x.text);

	return (cReport&)(*this);
}


// Destructor
cReport::~cReport(void)
{
	if (text)
		delete [] text;
	return;
}


void cGoalPosting::InitReportHeaders()
{
	for (int i=0; i<MAX_READABLE; i++)
		m_reportHeaders[i] = 0;
	m_rhCount = 0;
}

void cGoalPosting::AddReportHeader(lyra_id_t goalid)
{
	for (int i=0; i<m_rhCount; i++) // don't record duplicates
		if (m_reportHeaders[i] == goalid)
			return;

	m_reportHeaders[m_rhCount] = goalid;
	m_rhCount++;
}
