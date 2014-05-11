// Handles the main quest-posting screen

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cReadQuest.h"
#include "cChat.h"
#include "cPostQuest.h"
#include "cGoalBook.h"
#include "cQuestBuilder.h"
#include "Interface.h"
#include "cOutput.h"
#include "resource.h"
#include "cDetailQuest.h"
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
extern cQuestBuilder *quests;
extern cReadQuest *readquest;
extern cPostQuest *postquest;
extern cGoalBook *goalbook;
extern cDetailQuest *detailquest;

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

const button_t text_buttons[NUM_QUEST_BUTTONS] = 
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

// position for quest posting menu
const struct window_pos_t questBuilderPos[MAX_RESOLUTIONS]= 
{ { 0, 0, 480, 300 }, { 0, 0, 600, 375 }, { 0, 0, 768, 480 } };

// position for title text
const struct window_pos_t titlePos[MAX_RESOLUTIONS]=
{ { 0, 0, 350, 25 }, { 0, 0, 437, 31 }, { 0, 0, 560, 40 } };

// position for summaries label
const struct window_pos_t summariestextPos[MAX_RESOLUTIONS]= 
{ { 15, 45, 100, 20 }, { 18, 56, 125, 25 }, { 24, 72, 160, 32 } };

// position for summaries list box
const struct window_pos_t summariesPos[MAX_RESOLUTIONS]= 
{ { 5, 65, 460, 170 }, { 6, 101, 575, 170 }, { 8, 154, 736, 170 } };

// position for postquest button
const struct window_pos_t helpPos[MAX_RESOLUTIONS]= 
{ { 20, 230, 70, 20 }, { 25, 287, 70, 20 }, { 32, 368, 70, 20 } };

// position for postquest button
const struct window_pos_t postPos[MAX_RESOLUTIONS]= 
{ { 110, 230, 70, 20 }, { 137, 287, 70, 20 }, { 176, 368, 70, 20 } };

// position for complete quest button
const struct window_pos_t completePos[MAX_RESOLUTIONS]= 
{ { 200, 230, 70, 20 }, { 250, 287, 70, 20 }, { 320, 368, 70, 20 } };

// position for details button
const struct window_pos_t detailsPos[MAX_RESOLUTIONS]= 
{ { 110, 260, 70, 20 }, { 137, 325, 70, 20 }, { 176, 416, 70, 20 } };

// position for goal book button
const struct window_pos_t goalbookPos[MAX_RESOLUTIONS]= 
{ { 200, 260, 70, 20 }, { 250, 325, 70, 20 }, { 320, 416, 70, 20 } };

// position for read quest button
const struct window_pos_t readPos[MAX_RESOLUTIONS]= 
{ { 290, 230, 70, 20 }, { 362, 287, 70, 20 }, { 464, 368, 70, 20 } };

// position for accept quest button
const struct window_pos_t acceptPos[MAX_RESOLUTIONS]= 
{ { 290, 260, 70, 20 }, { 362, 325, 70, 20 }, { 464, 416, 70, 20 } };

// position for refresh button
const struct window_pos_t refreshPos[MAX_RESOLUTIONS]= 
{ { 380, 230, 70, 20 }, { 475, 287, 70, 20 }, { 608, 368, 70, 20 } };

// position for exit button
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
{ { 380, 260, 70, 20 }, { 475, 325, 70, 20 }, { 608, 416, 70, 20 } };


/////////////////////////////////////////////////////////////////
// Class Definitions

/////////////////////////////////////////////////////////////////
// Quest Posting Class
cQuestBuilder::cQuestBuilder(void) 
{
	WNDCLASS wc;

	last_quest_seen = Lyra::ID_UNKNOWN;
	next_quest = 0;
	page_num = 0;
 	num_new_quests = 0;
	active = FALSE;
	rank = Guild::NO_RANK;
	for (int i=0; i<MAX_SIMUL_GOALS; i++)
		curr_quests[i] = NULL;

	// Create Fonts
	questFont = NULL;

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
	_tcscpy(logFont.lfFaceName, QUEST_FONT_NAME);

	// set the control to use this font
	questFont = CreateFontIndirect(&logFont); 


    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = QuestBuilderWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("QuestBuilder");

    RegisterClass( &wc );

    hwnd_questbuilder = CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("QuestBuilder"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		questBuilderPos[cDD->Res()].x, questBuilderPos[cDD->Res()].y, 		
		questBuilderPos[cDD->Res()].width, questBuilderPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_questbuilder, WM_SETFONT, WPARAM(questFont), 0);

	hwnd_title = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						titlePos[cDD->Res()].x, titlePos[cDD->Res()].y, 
						titlePos[cDD->Res()].width, titlePos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL);

	hwnd_summariestext = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						summariestextPos[cDD->Res()].x, summariestextPos[cDD->Res()].y, 
						summariestextPos[cDD->Res()].width, summariestextPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL);
	hSummaries = CreateWindowsBitmap(LyraBitmap::SUMMARIES_LABEL);
	SendMessage(hwnd_summariestext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSummaries));

	hwnd_summaries = CreateWindow(_T("listbox"), _T("Summaries"),
						WS_CHILD | WS_DLGFRAME | LBS_NOTIFY,
						summariesPos[cDD->Res()].x, summariesPos[cDD->Res()].y, 
						summariesPos[cDD->Res()].width, summariesPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL);
	SendMessage(hwnd_summaries, WM_SETFONT, WPARAM(questFont), 0);
	lpfn_text = SubclassWindow(hwnd_summaries, QuestSummariesWProc);
	SendMessage(hwnd_summaries, WM_PASSPROC, 0, (LPARAM) lpfn_text ); 

	for (int i=0; i<NUM_QUEST_BUTTONS; i++)
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

	hwnd_goalbook = CreateWindow(_T("Button"), _T("Quest Book"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						goalbookPos[cDD->Res()].x, goalbookPos[cDD->Res()].y, 
						goalbookPos[cDD->Res()].width, goalbookPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL); 
	hGoalBook = CreateWindowsBitmap(LyraBitmap::GOAL_BOOK_BUTTON);
	SendMessage(hwnd_goalbook, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hGoalBook));
	SubclassGoalWindow(hwnd_goalbook);

	hwnd_read = CreateWindow(_T("Button"), _T("Read Quest"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						readPos[cDD->Res()].x, readPos[cDD->Res()].y, 
						readPos[cDD->Res()].width, readPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL); 
	hReadQuest = CreateWindowsBitmap(LyraBitmap::READ_GOAL_BUTTON);
	SendMessage(hwnd_read, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hReadQuest));
	SubclassGoalWindow(hwnd_read);


	hwnd_accept = CreateWindow(_T("Button"), _T("Accept Quest"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						acceptPos[cDD->Res()].x, acceptPos[cDD->Res()].y, 
						acceptPos[cDD->Res()].width, acceptPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL); 
	hAcceptQuest = CreateWindowsBitmap(LyraBitmap::ACCEPT_GOAL_BUTTON);
	SendMessage(hwnd_accept, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hAcceptQuest));
	SubclassGoalWindow(hwnd_accept);


	hwnd_exit = CreateWindow(_T("Button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL); 
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	hwnd_help = CreateWindow(_T("Button"), _T("Help"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						helpPos[cDD->Res()].x, helpPos[cDD->Res()].y, 
						helpPos[cDD->Res()].width, helpPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL); 
	hHelpQuest = CreateWindowsBitmap(LyraBitmap::HELP_GOAL_BUTTON);
	SendMessage(hwnd_help, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hHelpQuest));
	SubclassGoalWindow(hwnd_help);

	hwnd_post = CreateWindow(_T("Button"), _T("Post Quest"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						postPos[cDD->Res()].x, postPos[cDD->Res()].y, 
						postPos[cDD->Res()].width, postPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL); 
	hPostQuest = CreateWindowsBitmap(LyraBitmap::POST_GOAL_BUTTON);
	hPostOff = CreateWindowsBitmap(LyraBitmap::POST_GOAL_OFF);
	SendMessage(hwnd_post, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hPostQuest));
	SubclassGoalWindow(hwnd_post);

	hwnd_complete = CreateWindow(_T("Button"), _T("Complete Quest"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						completePos[cDD->Res()].x, completePos[cDD->Res()].y, 
						completePos[cDD->Res()].width, completePos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL); 
	hCompleteQuest = CreateWindowsBitmap(LyraBitmap::COMPLETE_LABEL);
	SendMessage(hwnd_complete, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hCompleteQuest));
	SubclassGoalWindow(hwnd_complete);

	hwnd_refresh = CreateWindow(_T("Button"), _T("Refresh"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						refreshPos[cDD->Res()].x, refreshPos[cDD->Res()].y, 
						refreshPos[cDD->Res()].width, refreshPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL); 
	hRefresh = CreateWindowsBitmap(LyraBitmap::REFRESH_LIST_BUTTON);
	SendMessage(hwnd_refresh, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hRefresh));
	SubclassGoalWindow(hwnd_refresh);

	hwnd_details = CreateWindow(_T("Button"), _T("Show Details"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						detailsPos[cDD->Res()].x, detailsPos[cDD->Res()].y, 
						detailsPos[cDD->Res()].width, detailsPos[cDD->Res()].height,
						hwnd_questbuilder,
						NULL, hInstance, NULL);
	hShowDetails = CreateWindowsBitmap(LyraBitmap::SHOW_DETAILS_BUTTON);
	hDetailsOff = CreateWindowsBitmap(LyraBitmap::SHOW_DETAILS_OFF);
	SendMessage(hwnd_details, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hShowDetails));
	SubclassGoalWindow(hwnd_details);

	return;

}

void cQuestBuilder::Activate()
{ 

	// Get goalbook headers
	if (goalbook->NumGoals() == 0)
		gs->RequestGoalbook();

	if ((active) || !gs->LoggedIntoLevel())
		return;

	active = TRUE;
	page_num = 0;
	last_quest_seen = Lyra::ID_UNKNOWN;

	gs->LevelLogout(RMsg_Logout::GOALBOOK); 

	if ((player->Skill(Arts::TRAIN) > 0) ||
		player->IsRuler(Guild::NO_GUILD) ||
		player->IsKnight(Guild::NO_GUILD))
	{ // tell higher ranks how much xp pool is left
		LoadString (hInstance, IDS_QUEST_XPPOOL_REMAINING, message, sizeof(message));
		_stprintf(disp_message, message, player->QuestXPPool());
		display->DisplayMessage (disp_message);
	}

	if (readquest->Active())
		readquest->Deactivate();

	if (goalbook->Active())
		goalbook->Deactivate();
	if (postquest->Active())
		postquest->Deactivate();
	if (detailquest->Active())
		detailquest->Deactivate();

	// use session id so we can identify stale messages 
	session_id = rand();

	// erase the viewport and show this window
	cDD->EraseSurface(BACK_BUFFER);
	cDD->BlitOffScreenSurface();

	hTitle = CreateWindowsBitmap(LyraBitmap::QUEST_BUILDER);
	SendMessage(hwnd_title, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hTitle);

	ShowWindow(hwnd_title, SW_SHOWNORMAL); 
	ShowWindow(hwnd_summariestext, SW_SHOWNORMAL);
	ShowWindow(hwnd_summaries, SW_SHOWNORMAL); 
	ShowWindow(hwnd_goalbook, SW_SHOWNORMAL); 
	ShowWindow(hwnd_read, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL); 
	ShowWindow(hwnd_help, SW_SHOWNORMAL); 
	ShowWindow(hwnd_refresh, SW_SHOWNORMAL);
	SendMessage(hwnd_details, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hShowDetails));
	ShowWindow(hwnd_details, SW_SHOWNORMAL);
	ShowWindow(hwnd_accept, SW_SHOWNORMAL);
	ShowWindow(hwnd_complete, SW_SHOWNORMAL);

	// Show PostQuest and Details buttons if player is high enough rank to use them
	if ((player->Skill(Arts::TRAIN) == 0) &&
		(!player->IsRuler(Guild::NO_GUILD)) &&
		(!player->IsKnight(Guild::NO_GUILD)))
	{
		ShowWindow(hwnd_post, SW_HIDE);

	}
	else 
	{
		SendMessage(hwnd_post, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hPostQuest));
		ShowWindow(hwnd_post, SW_SHOWNORMAL);
	}
	for (int i=0; i<NUM_QUEST_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

	gs->RequestGoalHeaders(session_id, Guild::NO_GUILD, Guild::QUEST, last_quest_seen);

	ShowWindow(hwnd_questbuilder, SW_SHOWNORMAL);
	
	return;
};

void cQuestBuilder::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_questbuilder, SW_HIDE); 

	if (hTitle!=NULL)
		DeleteObject(hTitle);

	if (readquest->Active())
		readquest->Deactivate();
	if (goalbook->Active())
		goalbook->Deactivate();
	if (postquest->Active())
		postquest->Deactivate();
	if (detailquest->Active())
		detailquest->Deactivate();

	for (int i=0; i<next_quest; i++)
	{
		ListBox_DeleteString(hwnd_summaries, 0);
		delete curr_quests[i];
	}

	next_quest = 0;
	page_num = 0;
	last_quest_seen = Lyra::ID_UNKNOWN;

	gs->LevelLogin();	

	return;
}


void cQuestBuilder::DBUnavailable(void)
{
	LoadString (hInstance, IDS_DB_UNAVAILABLE, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);

	this->Deactivate();
}


// summary info has arrived for a new quest
void cQuestBuilder::NewQuestInfo(int sessionid, realmid_t quest_id, short status, short playeroption, const TCHAR *quest_summary)
{	
	int num_quests_shown;

	// make sure the QuestBuilder screen is active and is the requestor
	if ((!active) || (this->session_id != sessionid))
		return;

	for (int i = 0; i < next_quest; i++)
		if (quest_id == curr_quests[i]->ID())
		{ // quest is already in the array
			ListBox_AddString(hwnd_summaries, quest_summary);
			num_quests_shown = ListBox_GetCount(hwnd_summaries);
			ListBox_SetItemData(hwnd_summaries, num_quests_shown-1, quest_id);
			return;
		}

	// make sure we don't overrun allocated memory
	if (next_quest == MAX_SIMUL_GOALS)
		return;

	// this must really be _new_ quest info
	curr_quests[next_quest] = new cGoal();
	curr_quests[next_quest]->SetInfo(quest_id, Guild::QUEST, Guild::NO_GUILD, status, 
		playeroption, quest_summary); //******
	next_quest++;
	num_new_quests++;

	if (1) /// *******player->GuildRank(guild) > rank)
	{
		switch (status)
		{
			// commented out stuff is for possible future use
			case Guild::GOAL_COMPLETED:
			{
	   			LoadString (hInstance, IDS_GUILD_GOALCOMPLETED, message, sizeof(message));
				_stprintf(disp_message, message, quest_summary);
				break;
			}
			case Guild::GOAL_MAX_ACCEPTED:
			{
	   			LoadString (hInstance, IDS_GUILD_GOALMAXACCEPTED, message, sizeof(message));
				_stprintf(disp_message, message, quest_summary);
				break;
			}
			case Guild::GOAL_TIME_EXPIRED:
			{
	   			LoadString (hInstance, IDS_GUILD_GOALTIMEEXPIRED, message, sizeof(message));
				_stprintf(disp_message, message, quest_summary);
				break;
			}
			default:
			{
				_stprintf(disp_message, quest_summary);
				break;
			}
		}
	}
	else
	_stprintf(disp_message, quest_summary);

	ListBox_AddString(hwnd_summaries, disp_message);
	num_quests_shown = ListBox_GetCount(hwnd_summaries);
	ListBox_SetItemData(hwnd_summaries, num_quests_shown-1, quest_id);

	if (num_quests_shown == VISIBLE_LINES)
		last_quest_seen = quest_id;

	if (ListBox_GetCurSel(hwnd_summaries) == -1)
		ListBox_SetCurSel(hwnd_summaries,0);
}



// quest text has arrived
void cQuestBuilder::NewQuestText(realmid_t quest_id, const TCHAR *poster,
		int sugsphere, int sugstat, const TCHAR *text)
{	
	if (!active)
		return;

	for (int i=0; i<next_quest; i++)
	{
		if (curr_quests[i]->ID() == quest_id)
		{ // found a match, set text
			curr_quests[i]->SetText(poster, sugsphere, sugstat, text);
			if ((readquest->Active()) && (readquest->CurrQuestID() == quest_id))
				readquest->SetText();
			// 0 charges means a codex quest
			if ((curr_quests[i]->Charges() == 0) &&
				(detailquest->Active()) &&
				(0 == _tcscmp(player->UpperName(), curr_quests[i]->UpperPosterName())))
				detailquest->ShowKeywords();
			if (postquest->Active() && (postquest->CurrQuestID() == quest_id))
				postquest->SetText();
			return;
		}
	}

	LoadString(hInstance, IDS_UNKNOWN_QUEST, message, sizeof(message));
	NONFATAL_ERROR(message);
	return;
}


// quest details have arrived
void cQuestBuilder::NewQuestDetails(realmid_t quest_id, int quest_rank, int maxacceptances, 
		int expirationtime, int numacceptees, int graphic, int charges, 
		int color1, int color2, int item_type, int field1, int field2,
		int field3,	int quest_xp, int numcompletees, 
		const TCHAR* keywords, pname_t *acceptees)
{
	if (!active)
		return;

	for (int i=0; i<next_quest; i++)
	{
		if (curr_quests[i]->ID() == quest_id)
		{ // found a match, set details 
			curr_quests[i]->SetDetails(quest_rank, maxacceptances, expirationtime, 0,
				0, 0, 0, 0, numacceptees, graphic, charges, 
					   color1, color2, item_type, field1, field2,
					   field3, quest_xp, numcompletees, keywords, acceptees);
			if (detailquest->Active() && (detailquest->CurrQuestID() == quest_id))
				detailquest->SetDetails();
			if (postquest->Active() && (postquest->CurrQuestID() == quest_id))
				postquest->SetDetails();
			return;
		}
	}
	LoadString(hInstance, IDS_UNKNOWN_QUEST, message, sizeof(message));
	NONFATAL_ERROR(message);
	return;
}



// User requests next page of quests
void cQuestBuilder::NextPage(void)
{
	// don't bother if there isn't a full page already
	if (ListBox_GetCount(hwnd_summaries) < 10)
		return;

	if (next_quest < (page_num+2)*10)
	{
		// if we are on the last page, and not enough new quests to fill a 
		// new page were last gotten, return
		if (next_quest <= ((page_num+1)*10))
		{	
			if (num_new_quests < 10)
				return;
		}
		
		ListBox_ResetContent(hwnd_summaries);
		page_num++;
		num_new_quests = 0;
		gs->RequestGoalHeaders(session_id, Guild::NO_GUILD, Guild::QUEST, last_quest_seen);
	}
	else
	{
		ListBox_ResetContent(hwnd_summaries);
		page_num++;

		for (int i = 0; i < 10; i++)
		{
			ListBox_AddString(hwnd_summaries, curr_quests[(page_num*10)+i]->Summary());
			ListBox_SetItemData(hwnd_summaries, i, curr_quests[(page_num*10)+i]->ID());
		}

		ListBox_SetCurSel(hwnd_summaries, 0);
	}
}


void cQuestBuilder::NextLine(void)
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
			for (int i=0; i<NUM_QUEST_BUTTONS; i++)
				InvalidateRect(hwnd_text_buttons[i], NULL, false);
		}
	}
}


void cQuestBuilder::PrevLine(void)
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
			for (int i=0; i<NUM_QUEST_BUTTONS; i++)
				InvalidateRect(hwnd_text_buttons[i], NULL, false);
		}
	}
}


// User requests prev page of quests
void cQuestBuilder::PrevPage(void)
{
	if (page_num > 0)
		page_num--;
	else
		return;
	
	ListBox_ResetContent(hwnd_summaries);
	
	for (int i = 0; i < 10; i++)
	{
		ListBox_AddString(hwnd_summaries, curr_quests[(page_num*10)+i]->Summary());
		ListBox_SetItemData(hwnd_summaries, i, curr_quests[(page_num*10)+i]->ID());
	}
	
	ListBox_SetCurSel(hwnd_summaries, VISIBLE_LINES-1);
}


void cQuestBuilder::ReadQuest(void)
{	
	int selected = ListBox_GetCurSel(hwnd_summaries);
	if (selected == -1)
		return;
	
	int searchquest = ListBox_GetItemData(hwnd_summaries, selected);
	
	for (int i=0; i<next_quest; i++)
	{
		if (curr_quests[i]->ID() == searchquest)
		{ // found the quest, activate readquest
			readquest->Activate(curr_quests[i]);
			return;
		}
	}
	LoadString(hInstance, IDS_UNKNOWN_QUEST_READ, message, sizeof(message));
	NONFATAL_ERROR(message);
	return;
}


// completing = true if we're getting the details because the player
// is attempting to complete the quest
void cQuestBuilder::ReadQuestDetails(bool completing)
{	
	int selected = ListBox_GetCurSel(hwnd_summaries);
	if (selected == -1)
		return;
	
	int searchquest = ListBox_GetItemData(hwnd_summaries, selected);
	
	for (int i=0; i<next_quest; i++)
	{
		if (curr_quests[i]->ID() == searchquest)
		{ // found the quest, activate readquest
			detailquest->Activate(curr_quests[i], completing);
			readquest->Activate(curr_quests[i]);	
			return;
		}
	}
	LoadString(hInstance, IDS_UNKNOWN_QUEST_READ, message, sizeof(message));
	NONFATAL_ERROR(message);
	return;
}


void cQuestBuilder::Post(void)
{	
	if ((player->Skill(Arts::TRAIN) == 0) &&
		!player->IsRuler(Guild::NO_GUILD) &&
		!player->IsKnight(Guild::NO_GUILD))
	{ // member of guild, but lacks rank for this function
		LoadString (hInstance, IDS_QUEST_LOWRANK, message, sizeof(message));
		display->DisplayMessage (message);
		return;
	}
	
	postquest->Activate(NULL);
	
	return;
}


void cQuestBuilder::AcceptQuest(cGoal *new_quest)
{
	if (goalbook->NumGoals() >= Lyra::MAX_ACTIVE_GOALS)
	{
		LoadString (hInstance, IDS_GOALBOOK_FULL, message, sizeof(disp_message));
		quests->QuestError();
	}
	else if (goalbook->InGoalBook(new_quest->ID()))
	{
		LoadString (hInstance, IDS_GOALBOOK_ALREADY_HAVE_QUEST, message, sizeof(message));
		quests->QuestError();
	}
	else 
		gs->AcceptGoal(new_quest->ID());
	
	return;
}



void cQuestBuilder::RefreshSummaries(void)
{
	ListBox_ResetContent(hwnd_summaries);
	
	for (int i=0; i<next_quest; i++)
	{
		ListBox_DeleteString(hwnd_summaries, 0);
		delete curr_quests[i];
	}
	
	next_quest = 0;
	page_num = 0;
	last_quest_seen = 0;
	session_id++;

	gs->RequestGoalHeaders(session_id, Guild::NO_GUILD, Guild::QUEST, last_quest_seen);
	
	return;
}


void cQuestBuilder::DisplayGoalBook(void)
{
	goalbook->Activate();
	return;
}



void cQuestBuilder::QuestNotFound(realmid_t quest_id)
{
	if (readquest->Active())
		readquest->Deactivate();
	if (detailquest->Active())
		detailquest->Deactivate();
	if (goalbook->Active())
		goalbook->Deactivate();
	if (goalbook->InGoalBook(quest_id))
		goalbook->RemoveGoal(quest_id);

	LoadString (hInstance, IDS_QUEST_NOTFOUND, message, sizeof(message));
	display->DisplayMessage(message);
}


// the error string should be loaded into message before calling here
void cQuestBuilder::QuestError(void)
{
	CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR, 
				cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
	return;
}

void cQuestBuilder::QuestWarning(quest_warning_callback_t callbackproc)
{
	HWND hDlg;
	hDlg = CreateLyraDialog(hInstance, IDD_WARNING_YESNO, 
				cDD->Hwnd_Main(), (DLGPROC)WarningYesNoDlgProc);
	SendMessage(hDlg, WM_SET_CALLBACK, 0,  (LPARAM)callbackproc);
	return;
}

void cQuestBuilder::Help(void)
{
	LoadString(hInstance, IDS_QUEST_HELP, message, sizeof(message));
	LoadString(hInstance, IDS_QUEST_HELP2, disp_message, sizeof(disp_message));
	HWND hDlg = CreateLyraDialog(hInstance, IDD_QUEST_HELP,	cDD->Hwnd_Main(), (DLGPROC)QuestHelpDlgProc);

	return;
}



// Destructor
cQuestBuilder::~cQuestBuilder(void)
{
	if (questFont!=NULL)
		DeleteObject(questFont);
	if (hSummaries!=NULL)
		DeleteObject(hSummaries);
	if (hAcceptQuest!=NULL)
		DeleteObject(hAcceptQuest);
	if (hShowDetails!=NULL)
		DeleteObject(hShowDetails);
	if (hReadQuest!=NULL)
		DeleteObject(hReadQuest);
	if (hGoalBook!=NULL)
		DeleteObject(hGoalBook);
	if (hExit!=NULL)
		DeleteObject(hExit);
	if (hPostQuest!=NULL)
		DeleteObject(hPostQuest);
	if (hHelpQuest!=NULL)
		DeleteObject(hHelpQuest);
	if (hCompleteQuest!=NULL)
		DeleteObject(hCompleteQuest);
	if (hRefresh!=NULL)
		DeleteObject(hRefresh);
	if (hPostOff!=NULL)
		DeleteObject(hPostOff);
	if (hDetailsOff!=NULL)
		DeleteObject(hDetailsOff);
	for (int i=0; i<NUM_QUEST_BUTTONS; i++)
	{
		if (text_buttons_bitmaps[i][0])
			DeleteObject(text_buttons_bitmaps[i][0]);
		if (text_buttons_bitmaps[i][1])
			DeleteObject(text_buttons_bitmaps[i][1]);
	}
	return;
}


// Subclassed window procedure for the rich edit control
LRESULT WINAPI QuestSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
			SendMessage(quests->hwnd_questbuilder, message,
				(WPARAM) wParam, (LPARAM) lParam);
			return 0L;

		case WM_KEYUP:
			switch ((UINT)(wParam)) {
				case VK_ESCAPE:
					if (readquest->Active())
						readquest->Deactivate();
					else
						quests->Deactivate();
					return 0L;
				case VK_RETURN:
					quests->ReadQuest();
					return 0L;
				case VK_UP:
					quests->PrevLine();
					return 0L;
				case VK_DOWN:
					quests->NextLine();
					return 0L;
				default:
					break;
			}
			break;

		case WM_LBUTTONDOWN:
		case LB_SETCURSEL:
			{
				for (int i=0; i<NUM_QUEST_BUTTONS; i++)
					InvalidateRect(quests->hwnd_text_buttons[i], NULL, false);
			}
			break;

		case WM_COMMAND:
			for (j=0; j<NUM_QUEST_BUTTONS; j++)
			{
				if ((HWND)lParam == quests->hwnd_text_buttons[j])
				{
					if (j == DDOWN)
						quests->NextPage();
					else if (j == DUP)
						quests->PrevPage();
					else if (j == DOWN)
						quests->NextLine();
					else 
						quests->PrevLine();
					break;
				}
			}
			SendMessage(quests->hwnd_questbuilder, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) quests->hwnd_questbuilder);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 

			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_QUEST_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == quests->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, quests->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == quests->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, quests->text_buttons_bitmaps[j][1]); 
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


// Window procedure for the quest posting window
LRESULT WINAPI QuestBuilderWProc ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	int selected_quest = -1;
	int style;

	switch(msg)
	{
		case WM_PAINT:
			if (quests->rank < 2)
				TileBackground(quests->hwnd_questbuilder);
			else
				TileBackground(quests->hwnd_questbuilder, GOLD_BACKGROUND);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == quests->hwnd_read)
				quests->ReadQuest();
			else if ((HWND)lParam == quests->hwnd_goalbook)
				quests->DisplayGoalBook();
			else if ((HWND)lParam == quests->hwnd_accept)
			{

				selected_quest = ListBox_GetCurSel(quests->hwnd_summaries);
				if (selected_quest != -1)
					quests->AcceptQuest(quests->curr_quests[quests->page_num*10 + selected_quest]);
			}

			else if ((HWND)lParam == quests->hwnd_refresh)
				quests->RefreshSummaries();
			else if ((HWND)lParam == quests->hwnd_exit)
				quests->Deactivate();
			else if ((HWND)lParam == quests->hwnd_post)
				quests->Post();	
			else if ((HWND)lParam == quests->hwnd_help)
				quests->Help();				
			else if ((HWND)lParam == quests->hwnd_details)
				quests->ReadQuestDetails(false);
			else if ((HWND)lParam == quests->hwnd_complete)
			{
				selected_quest = ListBox_GetCurSel(quests->hwnd_summaries);
				if (selected_quest != -1)
				{
					lyra_id_t quest_id = quests->curr_quests[quests->page_num*10 + selected_quest]->ID();
					if (!goalbook->InGoalBook(quest_id)) 
					{
						LoadString(hInstance, IDS_QUEST_NO_GOALBOOK, message, sizeof(message));
						quests->QuestError();
					}
					else
						quests->ReadQuestDetails(true);
					break;
				}
			}
			else if ((HWND)lParam == quests->hwnd_summaries)
			{
				switch (HIWORD(wParam))
				{
					case LBN_DBLCLK:
						quests->ReadQuest();
						for (int i=0; i<NUM_QUEST_BUTTONS; i++)
							InvalidateRect(quests->hwnd_text_buttons[i], NULL, false);
						break;
				}
			}
			break;

		case WM_KEYUP:
			switch ((UINT)(wParam)) {
				case VK_ESCAPE:
					quests->Deactivate();
					return 0L;
				case VK_RETURN:
					quests->ReadQuest();
					return 0L;
				case VK_UP:
					quests->PrevLine();
					return 0L;
				case VK_DOWN:
					quests->NextLine();
					return 0L;
				default:
					break;
			}

			break;
	}  

	return DefWindowProc(hwnd, msg, wParam, lParam);
} 


// Check invariants

#ifdef CHECK_INVARIANTS

void cQuestBuilder::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif

