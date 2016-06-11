// Handles posting new quests

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cQuestBuilder.h"
#include "cPostQuest.h"
#include "resource.h"
#include "cChat.h"
#include "cItem.h"
#include "Dialogs.h"
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
extern cQuestBuilder *quests;
extern cPostQuest *postquest;
extern cChat *display;
extern bool itemdlg;
extern bool writescrolldlg;


//////////////////////////////////////////////////////////////////
// Constants

const int VISIBLE_LINES = 4;
const int DOWN = 0;
const int UP = 1;
const int MINIMUM_QUEST_ACCEPTEES = 5;

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

// position for quest posting window, relative to main
const struct window_pos_t postQuestPos[MAX_RESOLUTIONS]= 
{ { 0, 0, 480, 300 }, { 0, 0, 600, 375 }, { 0, 0, 768, 480 } };

// position for title text
const struct window_pos_t titlePos[MAX_RESOLUTIONS]=
{ { 0, 0, 350, 25 }, { 0, 0, 437, 31 }, { 0, 0, 560, 40 } };

// position for house logo
//const struct window_pos_t logoPos[MAX_RESOLUTIONS]=
//{ { 400, 0, 80, 80 }, { 500, 0, 100, 100 }, { 640, 0, 128, 128 } };

// position for label for expirationdays of new quest 
const struct window_pos_t exptextPos[MAX_RESOLUTIONS]= 
{ { 10, 35, 120, 20 }, { 12, 42, 150, 25 }, { 16, 55, 192, 32 } };

// position for expirationdays of new quest 
const struct window_pos_t expirationdaysPos[MAX_RESOLUTIONS]= 
{ { 140, 30, 70, 25 }, { 175, 35, 87, 31 }, { 224, 47, 112, 40 } };

// position for label for maxacceptances of new quest 
const struct window_pos_t maxacctextPos[MAX_RESOLUTIONS]= 
{ { 10, 65, 120, 20 }, { 12, 79, 150, 25 }, { 16, 103, 192, 32 } };

// position for maxacceptances of new quest 
const struct window_pos_t maxacceptedPos[MAX_RESOLUTIONS]= 
{ { 140, 60, 70, 25 }, { 175, 73, 87, 31 }, { 224, 93, 112, 40 } };

// position for label for suggested sphere of new quest 
const struct window_pos_t spheretextPos[MAX_RESOLUTIONS]= 
{ { 238, 35, 125, 20 }, { 297, 42, 156, 25 }, { 380, 55, 200, 32 } };

// position for suggested sphere of new quest 
const struct window_pos_t sugspherePos[MAX_RESOLUTIONS]= 
{ { 355, 30, 100, 275 }, { 443, 35, 125, 343 }, { 568, 47, 160, 440 } };

// position for label for suggested sphere of new quest 
const struct window_pos_t stattextPos[MAX_RESOLUTIONS]= 
{ { 239, 65, 125, 20 }, { 298, 79, 156, 25 }, { 382, 103, 200, 32 } };

// position for suggested stat of new quest 
const struct window_pos_t sugstatPos[MAX_RESOLUTIONS]= 
{ { 355, 60, 100, 150 }, { 443, 73, 125, 187 }, { 568, 95, 160, 240 } };

// position for label for retrieval type of new quest (talisman/codex)
const struct window_pos_t retrievetextPos[MAX_RESOLUTIONS]= 
{ { 238, 95, 125, 20 }, { 297, 116, 156, 25 }, { 380, 151, 200, 32 } };

// position for retrieval type of new quest 
const struct window_pos_t retrievePos[MAX_RESOLUTIONS]= 
{ { 355, 90, 100, 275 }, { 443, 109, 125, 343 }, { 568, 143, 160, 440 } };

// position for label for summary of new quest 
const struct window_pos_t sumtextPos[MAX_RESOLUTIONS]= 
{ { 10, 100, 100, 20 }, { 12, 125, 125, 25 }, { 16, 160, 160, 32 } };

// position for summary of new quest 
const struct window_pos_t summaryPos[MAX_RESOLUTIONS]= 
{ { 10, 120, 450, 25 }, { 12, 150, 562, 31 }, { 16, 192, 720, 40 } };

// position for label for quest text 
const struct window_pos_t texttextPos[MAX_RESOLUTIONS]= 
{ { 10, 150, 100, 20 }, { 12, 187, 125, 25 }, { 16, 240, 160, 32 } };

// position for quest text 
const struct window_pos_t questtextPos[MAX_RESOLUTIONS]= 
{ { 10, 170, 450, 80 }, { 12, 212, 562, 100 }, { 16, 272, 720, 128 } };

// position for label for quest xp text 
const struct window_pos_t quest_xptextPos[MAX_RESOLUTIONS]= 
{ { 20, 260, 70, 20 }, { 25, 325, 70, 20 }, { 32, 416, 70, 20 } };

// position for quest xp text 
const struct window_pos_t quest_xpPos[MAX_RESOLUTIONS]= 
{ { 100, 260, 70, 20 }, { 125, 325, 70, 20 }, { 160, 416, 70, 20 } };

// position for help button, relative to quest posting window
const struct window_pos_t helpPos[MAX_RESOLUTIONS]= 
{ { 200, 260, 70, 20 }, { 249, 325, 70, 20 }, { 320, 416, 70, 20 } };

// position for post button, relative to quest posting window
const struct window_pos_t postPos[MAX_RESOLUTIONS]= 
{ { 290, 260, 70, 20 }, { 362, 325, 70, 20 }, { 464, 416, 70, 20 } };

// position for exit button, relative to quest posting window
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
{ { 380, 260, 70, 20 }, { 475, 325, 70, 20 }, { 608, 416, 70, 20 } };

// Constructor
cPostQuest::cPostQuest(void) 
{
	WNDCLASS wc;

	active = FALSE;
	last_xp_award = 0;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = PostQuestWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("PostQuest");

    RegisterClass( &wc );

    hwnd_postquest =  CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("PostQuest"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		postQuestPos[cDD->Res()].x, postQuestPos[cDD->Res()].y, 		
		postQuestPos[cDD->Res()].width, postQuestPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_postquest, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_title = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						titlePos[cDD->Res()].x, titlePos[cDD->Res()].y, 
						titlePos[cDD->Res()].width, titlePos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);

	//hwnd_logo = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
	//					logoPos[cDD->Res()].x, logoPos[cDD->Res()].y, 
	//					logoPos[cDD->Res()].width, logoPos[cDD->Res()].height,
	//					hwnd_postquest,
	//					NULL, hInstance, NULL);

	hwnd_exptext = CreateWindow(_T("static"), _T("Days to expire:"),
						WS_CHILD | SS_BITMAP,
						exptextPos[cDD->Res()].x, exptextPos[cDD->Res()].y, 
						exptextPos[cDD->Res()].width, exptextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	hExpiration = CreateWindowsBitmap(LyraBitmap::EXPIRE_LABEL);
	SendMessage(hwnd_exptext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExpiration));

	hwnd_expirationdays = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_LEFT,
						expirationdaysPos[cDD->Res()].x, expirationdaysPos[cDD->Res()].y, 
						expirationdaysPos[cDD->Res()].width, expirationdaysPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_expirationdays, WM_SETFONT, WPARAM(quests->Hfont()), 0);
	SubclassGoalWindow(hwnd_expirationdays);

	hwnd_spheretext = CreateWindow(_T("static"), _T("Suggested sphere:"),
						WS_CHILD | SS_BITMAP,
						spheretextPos[cDD->Res()].x, spheretextPos[cDD->Res()].y, 
						spheretextPos[cDD->Res()].width, spheretextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	hSphere = CreateWindowsBitmap(LyraBitmap::SUG_SPHERE_LABEL);
	SendMessage(hwnd_spheretext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSphere));

	hwnd_sugsphere = CreateWindow(_T("combobox"), _T(""),
						WS_CHILD | WS_DLGFRAME | CBS_DROPDOWNLIST | 
							WS_VSCROLL | WS_TABSTOP,
						sugspherePos[cDD->Res()].x, sugspherePos[cDD->Res()].y, 
						sugspherePos[cDD->Res()].width, sugspherePos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_sugsphere, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_maxtext = CreateWindow(_T("static"), _T("Max acceptees:"),
						WS_CHILD | SS_BITMAP,
						maxacctextPos[cDD->Res()].x, maxacctextPos[cDD->Res()].y, 
						maxacctextPos[cDD->Res()].width, maxacctextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	hMaxAccept = CreateWindowsBitmap(LyraBitmap::MAX_ACCEPTEES_LABEL);
	SendMessage(hwnd_maxtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hMaxAccept));

	hwnd_maxaccepted = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_LEFT,
						maxacceptedPos[cDD->Res()].x, maxacceptedPos[cDD->Res()].y, 
						maxacceptedPos[cDD->Res()].width, maxacceptedPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_maxaccepted, WM_SETFONT, WPARAM(quests->Hfont()), 0);
	SubclassGoalWindow(hwnd_maxaccepted);

	hwnd_stattext = CreateWindow(_T("static"), _T("Suggested focus:"), // MDA: stat -> focus
						WS_CHILD | SS_BITMAP,
						stattextPos[cDD->Res()].x, stattextPos[cDD->Res()].y, 
						stattextPos[cDD->Res()].width, stattextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	hStat = CreateWindowsBitmap(LyraBitmap::SUG_STAT_LABEL);
	SendMessage(hwnd_stattext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hStat));

	hwnd_sugstat = CreateWindow(_T("combobox"), _T(""),
						WS_CHILD | WS_DLGFRAME | CBS_DROPDOWNLIST | 
							WS_VSCROLL | WS_TABSTOP,
						sugstatPos[cDD->Res()].x, sugstatPos[cDD->Res()].y, 
						sugstatPos[cDD->Res()].width, sugstatPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_sugstat, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_retrievetext = CreateWindow(_T("static"), _T("Retrieve:"),
						WS_CHILD | SS_BITMAP,
						retrievetextPos[cDD->Res()].x, retrievetextPos[cDD->Res()].y, 
						retrievetextPos[cDD->Res()].width, retrievetextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	hRetrieve = CreateWindowsBitmap(LyraBitmap::RETRIEVE_LABEL);
	SendMessage(hwnd_retrievetext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hRetrieve));

	hwnd_retrieve = CreateWindow(_T("combobox"), _T(""),
						WS_CHILD | WS_DLGFRAME | CBS_DROPDOWNLIST | 
							WS_VSCROLL | WS_TABSTOP,
						retrievePos[cDD->Res()].x, retrievePos[cDD->Res()].y, 
						retrievePos[cDD->Res()].width, retrievePos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_retrieve, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_quest_xptext = CreateWindow(_T("static"), _T("Quest XP:"),
						WS_CHILD | SS_BITMAP,
						quest_xptextPos[cDD->Res()].x, quest_xptextPos[cDD->Res()].y, 
						quest_xptextPos[cDD->Res()].width, quest_xptextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	hQuestXP = CreateWindowsBitmap(LyraBitmap::QUEST_XP_AWARD_LABEL);
	SendMessage(hwnd_quest_xptext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hQuestXP));

	hwnd_quest_xp = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_LEFT,
						quest_xpPos[cDD->Res()].x, quest_xpPos[cDD->Res()].y, 
						quest_xpPos[cDD->Res()].width, quest_xpPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_quest_xp, WM_SETFONT, WPARAM(quests->Hfont()), 0);
	SubclassGoalWindow(hwnd_quest_xp);


	hwnd_sumtext = CreateWindow(_T("static"), _T("Summary"),
						WS_CHILD | SS_BITMAP,
						sumtextPos[cDD->Res()].x, sumtextPos[cDD->Res()].y, 
						sumtextPos[cDD->Res()].width, sumtextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	hSummary = CreateWindowsBitmap(LyraBitmap::SUMMARY_LABEL);
	SendMessage(hwnd_sumtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSummary));

	hwnd_summary = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
						summaryPos[cDD->Res()].x, summaryPos[cDD->Res()].y, 
						summaryPos[cDD->Res()].width, summaryPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL); 
	SendMessage(hwnd_summary, WM_SETFONT, WPARAM(quests->Hfont()), 0);
	SubclassGoalWindow(hwnd_summary);

	hwnd_texttext = CreateWindow(_T("static"), _T("Text"),
						WS_CHILD | SS_BITMAP,
						texttextPos[cDD->Res()].x, texttextPos[cDD->Res()].y, 
						texttextPos[cDD->Res()].width, texttextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL);
	hText = CreateWindowsBitmap(LyraBitmap::MESSAGE_LABEL);
	SendMessage(hwnd_texttext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hText));

	hwnd_questtext = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_MULTILINE | 
						ES_LEFT | ES_AUTOVSCROLL | ES_WANTRETURN,
						questtextPos[cDD->Res()].x, questtextPos[cDD->Res()].y, 
						questtextPos[cDD->Res()].width, questtextPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL); 
	SendMessage(hwnd_questtext, WM_SETFONT, WPARAM(quests->Hfont()), 0);
	lpfn_text = SubclassWindow(hwnd_questtext, PostQuestTextWProc);
	SendMessage(hwnd_questtext, WM_PASSPROC, 0, (LPARAM) lpfn_text ); 

	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
	{
		hwnd_text_buttons[i] = CreateWindowEx(WS_EX_TOPMOST, _T("button"), _T(""),
				WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
				text_buttons[i].position[cDD->Res()].x, text_buttons[i].position[cDD->Res()].y, 
				text_buttons[i].position[cDD->Res()].width, text_buttons[i].position[cDD->Res()].height,
				hwnd_questtext, NULL, hInstance, NULL);
		text_buttons_bitmaps[i][0] = // a button
			CreateWindowsBitmap(text_buttons[i].bitmap_id);
		text_buttons_bitmaps[i][1] = // b button
			CreateWindowsBitmap(text_buttons[i].bitmap_id + 1);
	}


	hwnd_post = CreateWindow(_T("button"), _T("Post"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						postPos[cDD->Res()].x, postPos[cDD->Res()].y, 
						postPos[cDD->Res()].width, postPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL); 
	hPost = CreateWindowsBitmap(LyraBitmap::POST_GOAL_BUTTON);
	SendMessage(hwnd_post, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hPost));
	SubclassGoalWindow(hwnd_post);

	hwnd_help = CreateWindow(_T("button"), _T("Help"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						helpPos[cDD->Res()].x, helpPos[cDD->Res()].y, 
						helpPos[cDD->Res()].width, helpPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL); 
	hHelp = CreateWindowsBitmap(LyraBitmap::HELP_GOAL_BUTTON);
	SendMessage(hwnd_help, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hHelp));
	SubclassGoalWindow(hwnd_help);


	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_postquest,
						NULL, hInstance, NULL); 
	hExit = CreateWindowsBitmap(LyraBitmap::CANCEL);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	return;

}

void cPostQuest::Help(void)
{
	LoadString(hInstance, IDS_POSTQUEST_HELP, message, sizeof(message));
	LoadString(hInstance, IDS_POSTQUEST_HELP2, disp_message, sizeof(disp_message));
	HWND hDlg = CreateLyraDialog(hInstance, IDD_QUEST_HELP,	cDD->Hwnd_Main(), (DLGPROC)QuestHelpDlgProc);

	return;
}

void cPostQuest::Activate(cGoal *new_quest)
{ 
	if (active)
		return;

	_tprintf(_T("activated; hwnd = %d\n"), hwnd_postquest);

	active = TRUE; 

	if (new_quest)
		quest = new_quest;
	else
		quest = NULL;

	int string_counter = 0;

	// Load strings and values for suggested sphere combo

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

	// Load strings and values for suggested stat combo
	LoadString(hInstance, IDS_TALISMAN, temp_message, sizeof(temp_message));
	ComboBox_AddString(hwnd_retrieve, temp_message);
	ComboBox_SetItemData(hwnd_retrieve, 0, Quest::TALISMAN);	
	LoadString(hInstance, IDS_CODEX, temp_message, sizeof(temp_message));
	ComboBox_AddString(hwnd_retrieve, temp_message);
	ComboBox_SetItemData(hwnd_retrieve, 1, Quest::CODEX);	
	ComboBox_SetCurSel(hwnd_retrieve, 0);

	// Set field lengths for text areas
	Edit_LimitText(hwnd_maxaccepted, 3);
	Edit_SetText(hwnd_maxaccepted, _T(""));
	Edit_SetText(hwnd_expirationdays, _T(""));
#ifdef GAMEMASTER
	Edit_LimitText(hwnd_quest_xp, 6);
	Edit_LimitText(hwnd_expirationdays, 3);
#else
	Edit_LimitText(hwnd_expirationdays, 2);
	Edit_LimitText(hwnd_quest_xp, 4);
#endif
	Edit_SetText(hwnd_quest_xp, _T(""));

	Edit_LimitText(hwnd_questtext, MAX_GOAL_LENGTH-1);
	Edit_SetText(hwnd_questtext, _T(""));
	Edit_LimitText(hwnd_summary, GOAL_SUMMARY_LENGTH-Lyra::PLAYERNAME_MAX-3);
	Edit_SetText(hwnd_summary, _T(""));

	// send focus to days to expire window
	SendMessage(hwnd_expirationdays, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_postquest);

	hTitle = CreateWindowsBitmap(LyraBitmap::QUEST_BUILDER);
	SendMessage(hwnd_title, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hTitle);

	//hLogo = CreateWindowsBitmap(LyraBitmap::HOUSE_BITMAPS+quests->Guild()+((quests->Rank()-1)*NUM_GUILDS));
	//SendMessage(hwnd_logo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogo);

	ShowWindow(hwnd_title, SW_SHOWNORMAL);
	ShowWindow(hwnd_maxtext, SW_SHOWNORMAL);
	ShowWindow(hwnd_maxaccepted, SW_SHOWNORMAL);
	ShowWindow(hwnd_exptext, SW_SHOWNORMAL);
	ShowWindow(hwnd_expirationdays, SW_SHOWNORMAL);
	ShowWindow(hwnd_spheretext, SW_SHOWNORMAL);
	ShowWindow(hwnd_sugsphere, SW_SHOWNORMAL); 
	ShowWindow(hwnd_stattext, SW_SHOWNORMAL);
	ShowWindow(hwnd_sugstat, SW_SHOWNORMAL); 
	ShowWindow(hwnd_retrievetext, SW_SHOWNORMAL);
	ShowWindow(hwnd_retrieve, SW_SHOWNORMAL); 
	ShowWindow(hwnd_sumtext, SW_SHOWNORMAL);
	ShowWindow(hwnd_summary, SW_SHOWNORMAL); 
	ShowWindow(hwnd_texttext, SW_SHOWNORMAL);
	ShowWindow(hwnd_questtext, SW_SHOWNORMAL);
	ShowWindow(hwnd_quest_xptext, SW_SHOWNORMAL); 
	ShowWindow(hwnd_quest_xp, SW_SHOWNORMAL); 
	ShowWindow(hwnd_post, SW_SHOWNORMAL); 
	ShowWindow(hwnd_help, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL);
	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

	if (quest)
	{
		this->SetText();
		this->SetDetails();
	}

	ShowWindow(hwnd_postquest, SW_SHOWNORMAL);

	return;
};


void cPostQuest::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_postquest, SW_HIDE); 

	if (hTitle!=NULL)
		DeleteObject(hTitle);
	//if (hLogo!=NULL)
		//DeleteObject(hLogo);

	// clear out the comboboxes
	ComboBox_ResetContent(hwnd_sugsphere);
	ComboBox_ResetContent(hwnd_sugstat);
	ComboBox_ResetContent(hwnd_retrieve);

	if (quests->Active())
		SendMessage(quests->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) quests->Hwnd());
	else 
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	if (quest)
		quest = NULL;

	return;
}


// if text is available, display it; otherwise put loading message up
void cPostQuest::SetText()
{
	if (quest->Text())
	{
		int num_entries = ComboBox_GetCount(hwnd_sugsphere);
		for (int i = 0; i < num_entries; i++)
			if (ComboBox_GetItemData(hwnd_sugsphere, i) == quest->SugSphere())
			{
				ComboBox_SetCurSel(hwnd_sugsphere, i);
				break;
			}

		num_entries = ComboBox_GetCount(hwnd_sugstat);
		for (int i = 0; i < num_entries; i++)
			if (ComboBox_GetItemData(hwnd_sugstat, i) == quest->SugStat())
			{
				ComboBox_SetCurSel(hwnd_sugstat, i);
				break;
			}

		num_entries = ComboBox_GetCount(hwnd_retrieve);
		for (int i = 0; i < num_entries; i++)
			if (ComboBox_GetItemData(hwnd_retrieve, i) == quest->Retrieve())
			{
				ComboBox_SetCurSel(hwnd_retrieve, i);
				break;
			}


		Edit_SetText(hwnd_summary, quest->Summary());
		Edit_SetText(hwnd_questtext, quest->Text());

		// send focus to days to expire window
		SendMessage(hwnd_expirationdays, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_postquest);
	}
	else
	{
		gs->RequestGoalText(quest->ID());
		ComboBox_SetCurSel(hwnd_sugstat, -1);
		ComboBox_SetCurSel(hwnd_sugsphere, -1);
		ComboBox_SetCurSel(hwnd_retrieve, -1);
		Edit_SetText(hwnd_summary, _T(""));

		LoadString(hInstance, IDS_RETRIEVE_QUEST_INFO, message, sizeof(message));
		Edit_SetText(hwnd_questtext, message);

		if (quest->MaxAcceptances() == -1)
		{
			Edit_SetText(hwnd_maxaccepted, _T(""));
			Edit_SetText(hwnd_expirationdays, _T(""));
			Edit_SetText(hwnd_quest_xp, _T(""));
		}
	}
}


void cPostQuest::SetDetails()
{
	if (quest->MaxAcceptances() > -1)
	{
		TCHAR maxaccepted_string[4];
		TCHAR expiration_string[3];
		TCHAR quest_xp_string[5];


		_itot(quest->MaxAcceptances(), maxaccepted_string, 10);
		_itot(quest->ExpirationTime(), expiration_string, 10);
		_itot(quest->QuestXP(), quest_xp_string, 10);

		Edit_SetText(hwnd_maxaccepted, maxaccepted_string);
		Edit_SetText(hwnd_expirationdays, expiration_string);
		Edit_SetText(hwnd_quest_xp, quest_xp_string);

		// send focus to days to expire window
		SendMessage(hwnd_expirationdays, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_postquest);
	}
	else
	{

		gs->RequestGoalDetails(quest->ID());
		Edit_SetText(hwnd_maxaccepted, _T(""));
		Edit_SetText(hwnd_expirationdays, _T(""));
		Edit_SetText(hwnd_quest_xp, _T(""));

		if (!quest->Text())
		{
			ComboBox_SetCurSel(hwnd_sugstat, -1);
			ComboBox_SetCurSel(hwnd_sugsphere, -1);
			ComboBox_SetCurSel(hwnd_retrieve, -1);
			Edit_SetText(hwnd_summary, _T(""));
			LoadString(hInstance, IDS_RETRIEVE_QUEST_INFO, message, sizeof(message));
			Edit_SetText(hwnd_questtext, message);
		}
	}
}


void cPostQuest::PrePost(void)
{  // post new quest!
	TCHAR maxaccepted_string[4];
	TCHAR expiration_string[4];
	TCHAR quest_xp_string[7];
	TCHAR temp_summary[GOAL_SUMMARY_LENGTH];
	TCHAR* stopstring;

	last_xp_award = 0;

	GetWindowText(hwnd_maxaccepted, maxaccepted_string, 4);
	GetWindowText(hwnd_expirationdays, expiration_string, 4);
	GetWindowText(hwnd_quest_xp, quest_xp_string, 7);
	maxaccepted_string[3]=_T('\0');
	expiration_string[3]=_T('\0');
	quest_xp_string[6]=_T('\0');
	maxaccepted = _tcstol (maxaccepted_string, &stopstring, 10);
	expirationdays = _tcstol (expiration_string, &stopstring, 10);
	quest_xp = _tcstol (quest_xp_string, &stopstring, 10);
	sugsphere = ComboBox_GetItemData(hwnd_sugsphere, 
		ComboBox_GetCurSel(hwnd_sugsphere));
	sugstat = ComboBox_GetItemData(hwnd_sugstat, 
		ComboBox_GetCurSel(hwnd_sugstat));
	retrieve = ComboBox_GetCurSel(hwnd_retrieve);


	if ((maxaccepted > Lyra::MAX_ACCEPTS) || (maxaccepted < MINIMUM_QUEST_ACCEPTEES))
	{
		LoadString (hInstance, IDS_GOAL_ACCEPTS_OOR, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, MINIMUM_QUEST_ACCEPTEES, Lyra::MAX_ACCEPTS);
		quests->QuestError();
		SendMessage(hwnd_maxaccepted, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_maxaccepted);
		return;
	}

	if (expirationdays < 1 
#ifndef GAMEMASTER
 || (expirationdays > Lyra::MAX_QUEST_LIFE)
#endif	
		)
	{
		LoadString (hInstance, IDS_GOAL_EXPIRE_OOR, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, 1, Lyra::MAX_QUEST_LIFE);
		quests->QuestError();
		SendMessage(hwnd_expirationdays, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_expirationdays);
		return;
	}

#ifndef GAMEMASTER
	if (quest_xp * maxaccepted > player->QuestXPPool())
	{
		LoadString (hInstance, IDS_NOT_ENOUGH_QUEST_XPPOOL_REMAINING, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, player->QuestXPPool(), quest_xp*maxaccepted);
		quests->QuestError();
		SendMessage(hwnd_quest_xp, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_quest_xp);
		return;
	}
#endif

	if (((sugsphere > Stats::SPHERE_MAX) && 
		 (sugsphere != Guild::SPHERE_ANY)) || 
		(sugsphere < Stats::SPHERE_MIN))
	{
		LoadString (hInstance, IDS_GOAL_SPHERE_OOR, message, sizeof(message));
		quests->QuestError();
		SendMessage(hwnd_sugsphere, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_sugsphere);
		return;
	}

	if (((sugstat > Stats::STAT_MAX) && (sugstat != Stats::NO_STAT))
		|| (sugstat < Stats::STAT_MIN))
	{
		LoadString (hInstance, IDS_GOAL_STAT_OOR, message, sizeof(message));
		quests->QuestError();
		SendMessage(hwnd_sugstat, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_sugstat);
		return;
	}

	Edit_GetText(hwnd_questtext, questtext, MAX_GOAL_LENGTH);
	Edit_GetText(hwnd_summary, temp_summary, GOAL_SUMMARY_LENGTH);

	if    (_tcscmp(temp_summary,_T("")) == 0)
	{
		LoadString (hInstance, IDS_GOAL_NULLSUMMARY, message, sizeof(message));
		quests->QuestError();
		SendMessage(hwnd_summary, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_summary);
		return;
	}

	// prepend to quest summary only if quest is a new one
	if (!quest)
	{
		_stprintf(summary, _T("%s: "), player->Name());
	}
	else
		summary[0]='\0';

	_tcscat(summary, temp_summary);

	summary[GOAL_SUMMARY_LENGTH-1]='\0';
	questtext[MAX_GOAL_LENGTH-1]='\0';

	if    (_tcscmp(questtext,_T("")) == 0)
	{
		LoadString (hInstance, IDS_GOAL_NULLTEXT, message, sizeof(message));
		quests->QuestError();
		SendMessage(hwnd_questtext, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_questtext);
		return;
	}


	if (retrieve == Quest::TALISMAN)
	{
		itemdlg = TRUE;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CREATE_ITEM,  cDD->Hwnd_Main(), (DLGPROC)CreateItemDlgProc);
		SendMessage(hDlg, WM_INIT_ITEMCREATOR, 0, (LPARAM)CreateItem::QUEST_ITEM);
		SendMessage(hDlg, WM_INIT_ITEMFIELDS, 0, (LPARAM)quest);
	} 
	else if (retrieve == Quest::CODEX)
	{
		writescrolldlg = TRUE;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_WRITE_SCROLL,  cDD->Hwnd_Main(), (DLGPROC)WriteScrollDlgProc);
		SendMessage(hDlg, WM_SET_SCROLL_QUESTBUILDER_CALLBACK, 0, 0);
	}
}

void cPostQuest::EndPost(void* item_ptr, int color1, int color2, int graphic)
{  // post new quest!

	quest_item_t questitem;

	if (retrieve == Quest::TALISMAN) 
	{
		cItem* item = ((cItem*)item_ptr);
		ItemtoQuestItem(item, &questitem);
		item->SetStatus(ITEM_DUMMY);
	} 
	else if (retrieve == Quest::CODEX) 
	{
		scroll_t* scroll = ((scroll_t*)item_ptr);
		ScrollToCodexQuest(scroll, &questitem);
	}
	// need special handling for "any" graphic and "any" colors
	if (color1 == ANY_COLOR)
		questitem.color1 = ANY_COLOR;
	if (color2 == ANY_COLOR)
		questitem.color2 = ANY_COLOR;
	if (graphic == LyraBitmap::NONE)
		questitem.graphic = LyraBitmap::NONE;

	if ((color1 == ANY_COLOR) &&
		(color2 == ANY_COLOR) &&
		(graphic == LyraBitmap::NONE) &&
		(questitem.item_type == LyraItem::NO_FUNCTION))
	{

		LoadString (hInstance, IDS_LAME_QUEST, message, sizeof(message));
		quests->QuestError();
		return;
	}

#ifndef GAMEMASTER	
	player->SetQuestXPPool(player->QuestXPPool() - quest_xp * maxaccepted);
	last_xp_award = quest_xp * maxaccepted;
#endif


	if (quest)
		gs->PostGoal(quest->ID(), Guild::QUEST, Guild::NO_GUILD, maxaccepted, expirationdays, sugsphere, 
			sugstat, 0, summary, questtext, questitem.keywords, questitem.graphic,
			questitem.charges, questitem.color1, questitem.color2, questitem.item_type, 
			questitem.field1, questitem.field2, questitem.field3, quest_xp);
	else
		gs->PostGoal(Lyra::ID_UNKNOWN, Guild::QUEST, Guild::NO_GUILD, maxaccepted, expirationdays, sugsphere, 
			sugstat, 0, summary, questtext, questitem.keywords, questitem.graphic,
			questitem.charges, questitem.color1, questitem.color2, questitem.item_type, 
			questitem.field1, questitem.field2, questitem.field3, quest_xp);

	postquest->Deactivate();

}


void cPostQuest::PostAcknowledged(void)
{
	last_xp_award = 0;
	LoadString (hInstance, IDS_QUEST_POSTED, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
	if (quests->Active())
		quests->RefreshSummaries();

}


void cPostQuest::PostError(void)
{
	if (last_xp_award > 0)
	{
		player->SetQuestXPPool(player->QuestXPPool()+last_xp_award);
		last_xp_award = 0;
	}

	LoadString (hInstance, IDS_QUEST_ERROR, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
}


void cPostQuest::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_questtext, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_questtext, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_questtext, NULL, TRUE);
}




void cPostQuest::ScrollDown(int count)
{
	int line_count, first_visible, curr_count = count;
	line_count = SendMessage(hwnd_questtext, EM_GETLINECOUNT, 0, 0);
	if (line_count <= VISIBLE_LINES)
		return; // no scrolling until necessary

	first_visible = SendMessage(hwnd_questtext, EM_GETFIRSTVISIBLELINE, 0, 0); 

	while (curr_count && (line_count - first_visible > VISIBLE_LINES))
	{
		SendMessage(hwnd_questtext, WM_VSCROLL, (WPARAM)SB_LINEDOWN, 0);
		curr_count--;
		first_visible++;
	}
	InvalidateRect(hwnd_questtext, NULL, TRUE);
	return;
}


// Destructor
cPostQuest::~cPostQuest(void)
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


// Window procedure for the read quest window
LRESULT WINAPI PostQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;

	switch(message)
	{
		case WM_PAINT:
			TileBackground(postquest->hwnd_postquest);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == postquest->hwnd_exit)
				postquest->Deactivate();
			if ((HWND)lParam == postquest->hwnd_post) 
				postquest->PrePost();
			if ((HWND)lParam == postquest->hwnd_help)
				postquest->Help();

			break;

		case WM_KEYUP:
			if ((UINT)(wParam) == VK_ESCAPE)
			{
				postquest->Deactivate();
				return 0L;
			}
			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Subclassed window procedure for the rich edit control
LRESULT WINAPI PostQuestTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
				SendMessage(postquest->hwnd_postquest, message,
					(WPARAM) wParam, (LPARAM) lParam);
				SendMessage(postquest->hwnd_postquest, WM_ACTIVATE,
					(WPARAM) WA_CLICKACTIVE, (LPARAM) postquest->hwnd_postquest);
				return (LRESULT) 0;
			}
			InvalidateRect(postquest->hwnd_questtext, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_KEYUP:
			if ((wParam) == VK_RETURN)
				InvalidateRect(postquest->hwnd_questtext, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_KILLFOCUS:
		case WM_SETFOCUS:
			InvalidateRect(postquest->hwnd_questtext, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_MOUSEMOVE:
			{
				int selection = Edit_GetSel(postquest->hwnd_questtext);
				int start = LOWORD(selection);
				int finish = HIWORD(selection);
				if (start != finish)
					InvalidateRect(postquest->hwnd_questtext, &(button_strip[cDD->Res()]), TRUE);
			}
			break;

		case WM_LBUTTONDOWN:
			InvalidateRect(postquest->hwnd_questtext, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_RBUTTONDOWN:
			return 0;

		case WM_COMMAND:
			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((HWND)lParam == postquest->hwnd_text_buttons[j])
				{
					if (j == DOWN)
						postquest->ScrollDown(1);
					else
						postquest->ScrollUp(1);
					break;
				}
			}
			SendMessage(postquest->hwnd_postquest, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) postquest->hwnd_postquest);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == postquest->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, postquest->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == postquest->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, postquest->text_buttons_bitmaps[j][1]); 
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

void cPostQuest::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
