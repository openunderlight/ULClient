// Handles posting new goals

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cGoalPosting.h"
#include "cPostGoal.h"
#include "resource.h"
#include "cChat.h"
#include "lmitemdefs.h"
#include "utils.h"
#include "interface.h"
#include "cEffects.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cDDraw *cDD;
extern cEffects *effects;
extern cGameServer *gs;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cGoalPosting *goals;
extern cPostGoal *postgoal;
extern cChat *display;

//////////////////////////////////////////////////////////////////
// Constants

const int VISIBLE_LINES = 4;
const int DOWN = 0;
const int UP = 1;

// text scrolling buttons
struct button_t {
	window_pos_t position[MAX_RESOLUTIONS];
	int			 button_id;
	int			 bitmap_id;
};

const button_t text_buttons[NUM_TEXT_BUTTONS] = 
{  { { { 432, 59, 13, 15 }, { 543, 78, 13, 15 }, { 699, 105, 13, 15 } }, 
		DOWN, LyraBitmap::CP_DOWNA }, 
{ { { 432, 0, 13, 15 }, { 543, 0, 13, 15 }, { 699, 0, 13, 15 } }, 
		UP, LyraBitmap::CP_UPA } 
};     // up button

const RECT button_strip[MAX_RESOLUTIONS] =
{ { 430, 0, 450, 80 }, { 537, 0, 562, 100 }, { 688, 0, 720, 128 } };

// position for goal posting window, relative to main
const struct window_pos_t postGoalPos[MAX_RESOLUTIONS]= 
{ { 0, 0, 480, 300 }, { 0, 0, 600, 375 }, { 0, 0, 768, 480 } };

// position for title text
const struct window_pos_t titlePos[MAX_RESOLUTIONS]=
{ { 0, 0, 350, 25 }, { 0, 0, 437, 31 }, { 0, 0, 560, 40 } };

// position for house logo
const struct window_pos_t logoPos[MAX_RESOLUTIONS]=
{ { 400, 0, 80, 80 }, { 500, 0, 100, 100 }, { 640, 0, 128, 128 } };

// position for label for expirationdays of new goal 
const struct window_pos_t exptextPos[MAX_RESOLUTIONS]= 
{ { 10, 40, 120, 20 }, { 12, 50, 150, 25 }, { 16, 64, 192, 32 } };

// position for expirationdays of new goal 
const struct window_pos_t expirationdaysPos[MAX_RESOLUTIONS]= 
{ { 140, 35, 70, 25 }, { 175, 43, 87, 31 }, { 224, 56, 112, 40 } };

// position for label for maxacceptances of new goal 
const struct window_pos_t maxacctextPos[MAX_RESOLUTIONS]= 
{ { 10, 70, 120, 20 }, { 12, 87, 150, 25 }, { 16, 112, 192, 32 } };

// position for maxacceptances of new goal 
const struct window_pos_t maxacceptedPos[MAX_RESOLUTIONS]= 
{ { 140, 65, 70, 25 }, { 175, 81, 87, 31 }, { 224, 104, 112, 40 } };

// position for label for suggested sphere of new goal 
const struct window_pos_t spheretextPos[MAX_RESOLUTIONS]= 
{ { 238, 40, 125, 20 }, { 297, 50, 156, 25 }, { 380, 64, 200, 32 } };

// position for suggested sphere of new goal 
const struct window_pos_t sugspherePos[MAX_RESOLUTIONS]= 
{ { 355, 35, 100, 275 }, { 443, 43, 125, 343 }, { 568, 56, 160, 440 } };

// position for label for suggested sphere of new goal 
const struct window_pos_t stattextPos[MAX_RESOLUTIONS]= 
{ { 239, 70, 125, 20 }, { 298, 87, 156, 25 }, { 382, 112, 200, 32 } };

// position for suggested stat of new goal 
const struct window_pos_t sugstatPos[MAX_RESOLUTIONS]= 
{ { 355, 65, 100, 150 }, { 443, 81, 125, 187 }, { 568, 104, 160, 240 } };

// position for label for summary of new goal 
const struct window_pos_t sumtextPos[MAX_RESOLUTIONS]= 
{ { 10, 100, 100, 20 }, { 12, 125, 125, 25 }, { 16, 160, 160, 32 } };

// position for summary of new goal 
const struct window_pos_t summaryPos[MAX_RESOLUTIONS]= 
{ { 10, 120, 450, 25 }, { 12, 150, 562, 31 }, { 16, 192, 720, 40 } };

// position for label for goal text 
const struct window_pos_t texttextPos[MAX_RESOLUTIONS]= 
{ { 10, 150, 100, 20 }, { 12, 187, 125, 25 }, { 16, 240, 160, 32 } };

// position for goal text 
const struct window_pos_t goaltextPos[MAX_RESOLUTIONS]= 
{ { 10, 170, 450, 80 }, { 12, 212, 562, 100 }, { 16, 272, 720, 128 } };

// position for guardian checkbox, relative to goal posting window
const struct window_pos_t guardPos[MAX_RESOLUTIONS]= 
//{ { 10, 260, 170, 14 }, { 12, 325, 220, 20 }, { 16, 416, 280, 25 } };
{ { 10, 260, 170, 14 }, { 12, 325, 170, 14 }, { 16, 416, 170, 14 } };

// position for post button, relative to goal posting window
const struct window_pos_t postPos[MAX_RESOLUTIONS]= 
//{ { 290, 260, 70, 20 }, { 362, 325, 87, 25 }, { 464, 416, 112, 32 } };
{ { 290, 260, 70, 20 }, { 362, 325, 70, 20 }, { 464, 416, 70, 20 } };

// position for exit button, relative to goal posting window
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
//{ { 380, 260, 70, 20 }, { 475, 325, 87, 25 }, { 608, 416, 112, 32 } };
{ { 380, 260, 70, 20 }, { 475, 325, 70, 20 }, { 608, 416, 70, 20 } };

// Constructor
cPostGoal::cPostGoal(void) 
{
	WNDCLASS wc;

	active = FALSE;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = PostGoalWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("PostGoal");

    RegisterClass( &wc );

    hwnd_postgoal =  CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("PostGoal"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		postGoalPos[cDD->Res()].x, postGoalPos[cDD->Res()].y, 		
		postGoalPos[cDD->Res()].width, postGoalPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_postgoal, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_title = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						titlePos[cDD->Res()].x, titlePos[cDD->Res()].y, 
						titlePos[cDD->Res()].width, titlePos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);

	hwnd_logo = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						logoPos[cDD->Res()].x, logoPos[cDD->Res()].y, 
						logoPos[cDD->Res()].width, logoPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);

	hwnd_exptext = CreateWindow(_T("static"), _T("Days to expire:"),
						WS_CHILD | SS_BITMAP,
						exptextPos[cDD->Res()].x, exptextPos[cDD->Res()].y, 
						exptextPos[cDD->Res()].width, exptextPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	hExpiration = CreateWindowsBitmap(LyraBitmap::EXPIRE_LABEL);
	SendMessage(hwnd_exptext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExpiration));

	hwnd_expirationdays = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_LEFT,
						expirationdaysPos[cDD->Res()].x, expirationdaysPos[cDD->Res()].y, 
						expirationdaysPos[cDD->Res()].width, expirationdaysPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_expirationdays, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	SubclassGoalWindow(hwnd_expirationdays);

	hwnd_spheretext = CreateWindow(_T("static"), _T("Suggested sphere:"),
						WS_CHILD | SS_BITMAP,
						spheretextPos[cDD->Res()].x, spheretextPos[cDD->Res()].y, 
						spheretextPos[cDD->Res()].width, spheretextPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	hSphere = CreateWindowsBitmap(LyraBitmap::SUG_SPHERE_LABEL);
	SendMessage(hwnd_spheretext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSphere));

	hwnd_sugsphere = CreateWindow(_T("combobox"), _T(""),
						WS_CHILD | WS_DLGFRAME | CBS_DROPDOWNLIST | 
							WS_VSCROLL | WS_TABSTOP,
						sugspherePos[cDD->Res()].x, sugspherePos[cDD->Res()].y, 
						sugspherePos[cDD->Res()].width, sugspherePos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_sugsphere, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_maxtext = CreateWindow(_T("static"), _T("Max acceptees:"),
						WS_CHILD | SS_BITMAP,
						maxacctextPos[cDD->Res()].x, maxacctextPos[cDD->Res()].y, 
						maxacctextPos[cDD->Res()].width, maxacctextPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	hMaxAccept = CreateWindowsBitmap(LyraBitmap::MAX_ACCEPTEES_LABEL);
	SendMessage(hwnd_maxtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hMaxAccept));

	hwnd_maxaccepted = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_LEFT,
						maxacceptedPos[cDD->Res()].x, maxacceptedPos[cDD->Res()].y, 
						maxacceptedPos[cDD->Res()].width, maxacceptedPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_maxaccepted, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	SubclassGoalWindow(hwnd_maxaccepted);

	hwnd_stattext = CreateWindow(_T("static"), _T("Suggested focus:"),
						WS_CHILD | SS_BITMAP,
						stattextPos[cDD->Res()].x, stattextPos[cDD->Res()].y, 
						stattextPos[cDD->Res()].width, stattextPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	hStat = CreateWindowsBitmap(LyraBitmap::SUG_STAT_LABEL);
	SendMessage(hwnd_stattext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hStat));

	hwnd_sugstat = CreateWindow(_T("combobox"), _T(""),
						WS_CHILD | WS_DLGFRAME | CBS_DROPDOWNLIST | 
							WS_VSCROLL | WS_TABSTOP,
						sugstatPos[cDD->Res()].x, sugstatPos[cDD->Res()].y, 
						sugstatPos[cDD->Res()].width, sugstatPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_sugstat, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_sumtext = CreateWindow(_T("static"), _T("Summary"),
						WS_CHILD | SS_BITMAP,
						sumtextPos[cDD->Res()].x, sumtextPos[cDD->Res()].y, 
						sumtextPos[cDD->Res()].width, sumtextPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	hSummary = CreateWindowsBitmap(LyraBitmap::SUMMARY_LABEL);
	SendMessage(hwnd_sumtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSummary));

	hwnd_summary = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
						summaryPos[cDD->Res()].x, summaryPos[cDD->Res()].y, 
						summaryPos[cDD->Res()].width, summaryPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL); 
	SendMessage(hwnd_summary, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	SubclassGoalWindow(hwnd_summary);

	hwnd_texttext = CreateWindow(_T("static"), _T("Text"),
						WS_CHILD | SS_BITMAP,
						texttextPos[cDD->Res()].x, texttextPos[cDD->Res()].y, 
						texttextPos[cDD->Res()].width, texttextPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL);
	hText = CreateWindowsBitmap(LyraBitmap::MESSAGE_LABEL);
	SendMessage(hwnd_texttext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hText));

	hwnd_goaltext = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_MULTILINE | 
						ES_LEFT | ES_AUTOVSCROLL | ES_WANTRETURN,
						goaltextPos[cDD->Res()].x, goaltextPos[cDD->Res()].y, 
						goaltextPos[cDD->Res()].width, goaltextPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL); 
	SendMessage(hwnd_goaltext, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	lpfn_text = SubclassWindow(hwnd_goaltext, PostGoalTextWProc);
	SendMessage(hwnd_goaltext, WM_PASSPROC, 0, (LPARAM) lpfn_text ); 

	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
	{
		hwnd_text_buttons[i] = CreateWindowEx(WS_EX_TOPMOST, _T("button"), _T(""),
				WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
				text_buttons[i].position[cDD->Res()].x, text_buttons[i].position[cDD->Res()].y, 
				text_buttons[i].position[cDD->Res()].width, text_buttons[i].position[cDD->Res()].height,
				hwnd_goaltext, NULL, hInstance, NULL);
		text_buttons_bitmaps[i][0] = // a button
			CreateWindowsBitmap(text_buttons[i].bitmap_id);
		text_buttons_bitmaps[i][1] = // b button
			CreateWindowsBitmap(text_buttons[i].bitmap_id + 1);
	}

	hwnd_guard = CreateWindow(_T("button"), _T("All guardians manage"),
						WS_CHILD | BS_AUTOCHECKBOX | BS_LEFTTEXT,
						guardPos[cDD->Res()].x, guardPos[cDD->Res()].y, 
						guardPos[cDD->Res()].width, guardPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL); 
	hGuard = CreateWindowsBitmap(LyraBitmap::GUARDIAN_LABEL);
	SetWindowLong(hwnd_guard, GWL_USERDATA, LONG(hGuard));
	SubclassGoalWindow(hwnd_guard);

	hwnd_post = CreateWindow(_T("button"), _T("Post"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						postPos[cDD->Res()].x, postPos[cDD->Res()].y, 
						postPos[cDD->Res()].width, postPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL); 
	hPost = CreateWindowsBitmap(LyraBitmap::POST_GOAL_BUTTON);
	SendMessage(hwnd_post, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hPost));
	SubclassGoalWindow(hwnd_post);

	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_postgoal,
						NULL, hInstance, NULL); 
	hExit = CreateWindowsBitmap(LyraBitmap::CANCEL);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	return;

}


void cPostGoal::Activate(int trip_guild, int trip_rank, cGoal *new_goal)
{ 
	if (active)
		return;

	active = TRUE; 

	guild = trip_guild;
	rank = trip_rank;
	if (new_goal)
		goal = new_goal;
	else
		goal = NULL;

	int string_counter = 0;
	// Load strings and values for suggested sphere combo
	if ((player->GuildRank(guild) >= Guild::RULER) 
		&& (rank == Guild::KNIGHT))
	{

		LoadString(hInstance, IDS_RULER_ISSUE, temp_message, sizeof(temp_message));
		ComboBox_AddString(hwnd_sugsphere, temp_message);
		ComboBox_SetItemData(hwnd_sugsphere, string_counter, Guild::RULER_ISSUE);
		string_counter++;
	}

	LoadString(hInstance, IDS_ANY_SPHERE, temp_message, sizeof(temp_message));
	ComboBox_AddString(hwnd_sugsphere, temp_message);
	ComboBox_SetItemData(hwnd_sugsphere, string_counter, Guild::SPHERE_ANY);
	string_counter++;
	
	for (int i = Stats::SPHERE_MIN; i < ((Stats::SPHERE_MAX - Stats::SPHERE_MIN)+1); i++)
	{
		LoadString(hInstance, IDS_SPHERE_I, temp_message, sizeof(temp_message));
		_stprintf(message, temp_message, i);
		ComboBox_AddString(hwnd_sugsphere, message);
		ComboBox_SetItemData(hwnd_sugsphere, string_counter, i);
		string_counter++;
	}
	ComboBox_SetCurSel(hwnd_sugsphere, 0);


	// Load strings and values for suggested stat combo
	LoadString(hInstance, IDS_ANY_STAT, temp_message, sizeof(temp_message));
	ComboBox_AddString(hwnd_sugstat, temp_message);
	ComboBox_SetItemData(hwnd_sugstat, 0, Stats::NO_STAT);	

	for (int i = 1; i < NUM_PLAYER_STATS; i++)
	{
		TranslateValue(LyraItem::TRANSLATION_STAT,i);
		ComboBox_AddString(hwnd_sugstat, message);
		ComboBox_SetItemData(hwnd_sugstat, i, i);
	}			
	ComboBox_SetCurSel(hwnd_sugstat, 0);

	// Set field lengths for text areas
	Edit_LimitText(hwnd_maxaccepted, 3);
	Edit_SetText(hwnd_maxaccepted, _T(""));
	Edit_LimitText(hwnd_expirationdays, 2);
	Edit_SetText(hwnd_expirationdays, _T(""));

	Edit_LimitText(hwnd_goaltext, MAX_GOAL_LENGTH-1);
	Edit_SetText(hwnd_goaltext, _T(""));
	Edit_LimitText(hwnd_summary, GOAL_SUMMARY_LENGTH-Lyra::PLAYERNAME_MAX-3);
	Edit_SetText(hwnd_summary, _T(""));

	Button_SetCheck(hwnd_guard, 1);
	
	// send focus to days to expire window
	SendMessage(hwnd_expirationdays, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_postgoal);

	hTitle = CreateWindowsBitmap(LyraBitmap::HOUSE_GOALBOARD_TITLES+goals->Guild()+((goals->Rank()-1)*NUM_GUILDS));
	SendMessage(hwnd_title, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hTitle);

	hLogo = CreateWindowsBitmap(LyraBitmap::HOUSE_BITMAPS+goals->Guild()+((goals->Rank()-1)*NUM_GUILDS));
	SendMessage(hwnd_logo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogo);

	ShowWindow(hwnd_title, SW_SHOWNORMAL);
	ShowWindow(hwnd_maxtext, SW_SHOWNORMAL);
	ShowWindow(hwnd_maxaccepted, SW_SHOWNORMAL);
	ShowWindow(hwnd_exptext, SW_SHOWNORMAL);
	ShowWindow(hwnd_expirationdays, SW_SHOWNORMAL);
	ShowWindow(hwnd_spheretext, SW_SHOWNORMAL);
	ShowWindow(hwnd_sugsphere, SW_SHOWNORMAL); 
	ShowWindow(hwnd_stattext, SW_SHOWNORMAL);
	ShowWindow(hwnd_sugstat, SW_SHOWNORMAL); 
	ShowWindow(hwnd_sumtext, SW_SHOWNORMAL);
	ShowWindow(hwnd_summary, SW_SHOWNORMAL); 
	ShowWindow(hwnd_texttext, SW_SHOWNORMAL);
	ShowWindow(hwnd_goaltext, SW_SHOWNORMAL);
	ShowWindow(hwnd_post, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL);
	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

	if (rank == Guild::INITIATE)
		ShowWindow(hwnd_guard, SW_SHOWNORMAL);
	else
		ShowWindow(hwnd_guard, SW_HIDE);

	if (goal)
	{
		this->SetText();
		this->SetDetails();
	}

	ShowWindow(hwnd_postgoal, SW_SHOWNORMAL);

	return;
};


void cPostGoal::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_postgoal, SW_HIDE); 

	if (hTitle!=NULL)
		DeleteObject(hTitle);
	if (hLogo!=NULL)
		DeleteObject(hLogo);

	// clear out the comboboxes
	ComboBox_ResetContent(hwnd_sugsphere);
	ComboBox_ResetContent(hwnd_sugstat);
	Button_SetCheck(hwnd_guard, 0);

	if (goals->Active())
		SendMessage(goals->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goals->Hwnd());
	else 
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	if (goal)
		goal = NULL;

	return;
}


// if text is available, display it; otherwise put loading message up
void cPostGoal::SetText()
{
	if (goal->Text())
	{
		int num_entries = ComboBox_GetCount(hwnd_sugsphere);
		for (int i = 0; i < num_entries; i++)
			if (ComboBox_GetItemData(hwnd_sugsphere, i) == goal->SugSphere())
			{
				ComboBox_SetCurSel(hwnd_sugsphere, i);
				break;
			}

		num_entries = ComboBox_GetCount(hwnd_sugstat);
		for (int i = 0; i < num_entries; i++)
			if (ComboBox_GetItemData(hwnd_sugstat, i) == goal->SugStat())
			{
				ComboBox_SetCurSel(hwnd_sugstat, i);
				break;
			}

		Edit_SetText(hwnd_summary, goal->Summary());
		Edit_SetText(hwnd_goaltext, goal->Text());

		// send focus to days to expire window
		SendMessage(hwnd_expirationdays, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_postgoal);
	}
	else
	{
		gs->RequestGoalText(goal->ID());
		ComboBox_SetCurSel(hwnd_sugstat, -1);
		ComboBox_SetCurSel(hwnd_sugsphere, -1);
		Edit_SetText(hwnd_summary, _T(""));

		LoadString(hInstance, IDS_RETRIEVE_GOAL_INFO, message, sizeof(message));
		Edit_SetText(hwnd_goaltext, message);

		if (goal->MaxAcceptances() == -1)
		{
			Edit_SetText(hwnd_maxaccepted, _T(""));
			Edit_SetText(hwnd_expirationdays, _T(""));
		}
	}
}


void cPostGoal::SetDetails()
{
	if (goal->MaxAcceptances() > -1)
	{
		TCHAR maxaccepted_string[4];
		TCHAR expiration_string[3];

		_itot(goal->MaxAcceptances(), maxaccepted_string, 10);
		_itot(goal->ExpirationTime(), expiration_string, 10);

		Edit_SetText(hwnd_maxaccepted, maxaccepted_string);
		Edit_SetText(hwnd_expirationdays, expiration_string);

		Button_SetCheck(hwnd_guard, (goal->Flags() & GOAL_GUARDIAN_MANAGE_FLAG));

		// send focus to days to expire window
		SendMessage(hwnd_expirationdays, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_postgoal);
	}
	else
	{
		gs->RequestGoalDetails(goal->ID());
		Edit_SetText(hwnd_maxaccepted, _T(""));
		Edit_SetText(hwnd_expirationdays, _T(""));

		Button_SetCheck(hwnd_guard, 0);

		if (!goal->Text())
		{
			ComboBox_SetCurSel(hwnd_sugstat, -1);
			ComboBox_SetCurSel(hwnd_sugsphere, -1);
			Edit_SetText(hwnd_summary, _T(""));
			LoadString(hInstance, IDS_RETRIEVE_GOAL_INFO, message, sizeof(message));
			Edit_SetText(hwnd_goaltext, message);
		}
	}
}


void cPostGoal::Post(void)
{  // post new goal!
	TCHAR maxaccepted_string[4];
	TCHAR expiration_string[3];
	TCHAR goaltext[MAX_GOAL_LENGTH];
	TCHAR temp_summary[GOAL_SUMMARY_LENGTH];
	TCHAR summary[Lyra::PLAYERNAME_MAX + 9 + GOAL_SUMMARY_LENGTH];
	TCHAR* stopstring;

	GetWindowText(hwnd_maxaccepted, maxaccepted_string, 4);
	GetWindowText(hwnd_expirationdays, expiration_string, 3);
	maxaccepted_string[3]=_T('\0');
	expiration_string[2]=_T('\0');
	int maxaccepted = _tcstol (maxaccepted_string, &stopstring, 10);
	int expirationdays = _tcstol (expiration_string, &stopstring, 10);
	int sugsphere = ComboBox_GetItemData(hwnd_sugsphere, 
		ComboBox_GetCurSel(hwnd_sugsphere));
	int sugstat = ComboBox_GetItemData(hwnd_sugstat, 
		ComboBox_GetCurSel(hwnd_sugstat));

	int guardian = Button_GetCheck(hwnd_guard);

	if ((maxaccepted > Lyra::MAX_ACCEPTS) || (maxaccepted < 1))
	{
		LoadString (hInstance, IDS_GOAL_ACCEPTS_OOR, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, 1, Lyra::MAX_ACCEPTS);
		goals->GuildError();
		SendMessage(hwnd_maxaccepted, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_maxaccepted);
		return;
	}

	if ((expirationdays > Lyra::MAX_GOAL_LIFE) || (expirationdays < 1))
	{
		LoadString (hInstance, IDS_GOAL_EXPIRE_OOR, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, 1, Lyra::MAX_GOAL_LIFE);
		goals->GuildError();
		SendMessage(hwnd_expirationdays, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_expirationdays);
		return;
	}

	if (((sugsphere > Stats::SPHERE_MAX) && 
		 (sugsphere != Guild::SPHERE_ANY) &&
		 (sugsphere != Guild::RULER_ISSUE)) || 
		(sugsphere < Stats::SPHERE_MIN))
	{
		LoadString (hInstance, IDS_GOAL_SPHERE_OOR, message, sizeof(message));
		goals->GuildError();
		SendMessage(hwnd_sugsphere, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_sugsphere);
		return;
	}

	if (((sugstat > Stats::STAT_MAX) && (sugstat != Stats::NO_STAT))
		|| (sugstat < Stats::STAT_MIN))
	{
		LoadString (hInstance, IDS_GOAL_STAT_OOR, message, sizeof(message));
		goals->GuildError();
		SendMessage(hwnd_sugstat, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_sugstat);
		return;
	}

	Edit_GetText(hwnd_goaltext, goaltext, MAX_GOAL_LENGTH);
	Edit_GetText(hwnd_summary, temp_summary, GOAL_SUMMARY_LENGTH);

	if    (_tcscmp(temp_summary,_T("")) == 0)
	{
		LoadString (hInstance, IDS_GOAL_NULLSUMMARY, message, sizeof(message));
		goals->GuildError();
		SendMessage(hwnd_summary, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_summary);
		return;
	}

	// prepend to goal summary only if goal is a new one
	if (!goal)
	{
		// prepend user name, possibly "Issue" tag
		if (sugsphere == Stats::SPHERE_MIN - 2) // Ruler Issue
		{
		LoadString(hInstance, IDS_ISSUE, message, sizeof(message));
		_stprintf(summary, message, player->Name());
		}
		else 
		_stprintf(summary, _T("%s: "), player->Name());
	}
	else
		summary[0]='\0';

_tcscat(summary, temp_summary);

	summary[GOAL_SUMMARY_LENGTH-1]='\0';
	goaltext[MAX_GOAL_LENGTH-1]='\0';

	if    (_tcscmp(goaltext,_T("")) == 0)
	{
		LoadString (hInstance, IDS_GOAL_NULLTEXT, message, sizeof(message));
		goals->GuildError();
		SendMessage(hwnd_goaltext, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_goaltext);
		return;
	}

	// *** Goal post goes here

	if (goal)
		gs->PostGoal(goal->ID(), rank, guild, maxaccepted, expirationdays, sugsphere, 
			sugstat, guardian, summary, goaltext, _T(""), 0, 0, 0, 0, 0, 0, 0, 0, 0);
	else
		gs->PostGoal(Lyra::ID_UNKNOWN, rank, guild, maxaccepted, expirationdays, sugsphere, 
			sugstat, guardian, summary, goaltext, _T(""), 0, 0, 0, 0, 0, 0, 0, 0, 0);
			
	postgoal->Deactivate();
}


void cPostGoal::PostAcknowledged(void)
{
	if (postgoal->Rank() == Guild::INITIATE)
		LoadString (hInstance, IDS_MISSION_POSTED, disp_message, sizeof(disp_message));
	else if (postgoal->Rank() == Guild::KNIGHT)
		LoadString (hInstance, IDS_GOAL_POSTED, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
	if (goals->Active())
		goals->RefreshSummaries();
}


void cPostGoal::PostError(void)
{
	LoadString (hInstance, IDS_GOAL_ERROR, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, RankGoalName(postgoal->Rank()));
	display->DisplayMessage(message);
}


void cPostGoal::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_goaltext, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_goaltext, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_goaltext, NULL, TRUE);
}


void cPostGoal::ScrollDown(int count)
{
	int line_count, first_visible, curr_count = count;
	line_count = SendMessage(hwnd_goaltext, EM_GETLINECOUNT, 0, 0);
	if (line_count <= VISIBLE_LINES)
		return; // no scrolling until necessary

	first_visible = SendMessage(hwnd_goaltext, EM_GETFIRSTVISIBLELINE, 0, 0); 

	while (curr_count && (line_count - first_visible > VISIBLE_LINES))
	{
		SendMessage(hwnd_goaltext, WM_VSCROLL, (WPARAM)SB_LINEDOWN, 0);
		curr_count--;
		first_visible++;
	}
	InvalidateRect(hwnd_goaltext, NULL, TRUE);
	return;
}


// Destructor
cPostGoal::~cPostGoal(void)
{
	if (hMaxAccept!=NULL)
		DeleteObject(hMaxAccept);
	if (hExpiration!=NULL)
		DeleteObject(hExpiration);
	if (hSphere!=NULL)
		DeleteObject(hSphere);
	if (hStat!=NULL)
		DeleteObject(hStat);
	if (hText!=NULL)
		DeleteObject(hText);
	if (hSummary!=NULL)
		DeleteObject(hSummary);
	if (hGuard != NULL)
		DeleteObject(hGuard);
	if (hPost!=NULL)
		DeleteObject(hPost);
	if (hExit!=NULL)
		DeleteObject(hExit);
	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
	{
		if (text_buttons_bitmaps[i][0])
			DeleteObject(text_buttons_bitmaps[i][0]);
		if (text_buttons_bitmaps[i][1])
			DeleteObject(text_buttons_bitmaps[i][1]);
	}
}


// Window procedure for the read goal window
LRESULT WINAPI PostGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;

	switch(message)
	{
		case WM_PAINT:
			if (goals->Rank() < 2)
				TileBackground(postgoal->hwnd_postgoal);
			else
				TileBackground(postgoal->hwnd_postgoal, GOLD_BACKGROUND);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == postgoal->hwnd_exit)
				postgoal->Deactivate();
			if ((HWND)lParam == postgoal->hwnd_post)
				postgoal->Post();
			break;

		case WM_KEYUP:
			if ((UINT)(wParam) == VK_ESCAPE)
			{
				postgoal->Deactivate();
				return 0L;
			}
			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Subclassed window procedure for the rich edit control
LRESULT WINAPI PostGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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

		case WM_KEYDOWN:
			if (wParam == VK_TAB)
			{
				SendMessage(postgoal->hwnd_postgoal, message,
					(WPARAM) wParam, (LPARAM) lParam);
				SendMessage(postgoal->hwnd_postgoal, WM_ACTIVATE,
					(WPARAM) WA_CLICKACTIVE, (LPARAM) postgoal->hwnd_postgoal);
				return (LRESULT) 0;
			}
			InvalidateRect(postgoal->hwnd_goaltext, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_KEYUP:
			if ((wParam) == VK_RETURN)
				InvalidateRect(postgoal->hwnd_goaltext, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_KILLFOCUS:
		case WM_SETFOCUS:
			InvalidateRect(postgoal->hwnd_goaltext, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_MOUSEMOVE:
			{
				int selection = Edit_GetSel(postgoal->hwnd_goaltext);
				int start = LOWORD(selection);
				int finish = HIWORD(selection);
				if (start != finish)
					InvalidateRect(postgoal->hwnd_goaltext, &(button_strip[cDD->Res()]), TRUE);
			}
			break;

		case WM_LBUTTONDOWN:
			InvalidateRect(postgoal->hwnd_goaltext, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_RBUTTONDOWN:
			return 0;

		case WM_COMMAND:
			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((HWND)lParam == postgoal->hwnd_text_buttons[j])
				{
					if (j == DOWN)
						postgoal->ScrollDown(1);
					else
						postgoal->ScrollUp(1);
					break;
				}
			}
			SendMessage(postgoal->hwnd_postgoal, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) postgoal->hwnd_postgoal);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == postgoal->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, postgoal->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == postgoal->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, postgoal->text_buttons_bitmaps[j][1]); 
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

void cPostGoal::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
