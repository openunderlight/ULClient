// Handles the goal reporting screen

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cGoalPosting.h"
#include "cGoalBook.h"
#include "cReportGoal.h"
#include "cReadGoal.h"
#include "cReadReport.h"
#include "cChat.h"
#include "Options.h"
#include "resource.h"
#include "utils.h"
#include "interface.h"
#include "cEffects.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cDDraw *cDD;
extern cEffects *effects;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cGameServer *gs;
extern cGoalPosting *goals;
extern cGoalBook *goalbook;
extern cReadGoal *readgoal;
extern cReportGoal *reportgoal;
extern cChat *display;
extern cReadReport *readreport;
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
{
{ 
	{ { 432, 39, 13, 15 }, { 543, 54, 13, 15 }, { 701, 74, 13, 15 } },
			DOWN, LyraBitmap::CP_DOWNA  },   // down button
	{ 
	{ { 432, 0, 13, 15 }, { 543, 0, 13, 15 }, { 701, 0, 13, 15 } },
	UP, LyraBitmap::CP_UPA  }     // up button 
 };

const RECT button_strip[MAX_RESOLUTIONS] = 
{ { 430, 0, 450, 60 }, { 537, 0, 562, 75 }, { 688, 0, 720, 96 } };

// position for report goal window, relative to main
const struct window_pos_t reportGoalPos[MAX_RESOLUTIONS]= 
{ { 0, 120, 480, 180 }, { 0, 150, 600, 225 }, { 0, 192, 768, 288 } };


// position for label for report summary
const struct window_pos_t sumlabelPos[MAX_RESOLUTIONS]= 
{ { 10, 0, 80, 20 }, { 12, 0, 100, 25 }, { 16, 0, 128, 32 } };

// position for report summary
const struct window_pos_t summaryPos[MAX_RESOLUTIONS]= 
{ { 10, 20, 450, 100 }, { 12, 25, 562, 125 }, { 16, 32, 720, 160 } };


// position for label for report text
const struct window_pos_t textlabelPos[MAX_RESOLUTIONS]= 
{ { 10, 50, 80, 20 }, { 12, 62, 100, 25 }, { 16, 80, 128, 32 } };


// position for report text
const struct window_pos_t textPos[MAX_RESOLUTIONS]= 
{ { 10, 70, 450, 60 }, { 12, 87, 562, 75 }, { 16, 112, 720, 96 } };


// position for label for award xp editbox
const struct window_pos_t xplabelPos[MAX_RESOLUTIONS]= 
{ { 10, 143, 100, 20 }, { 12, 178, 125, 25 }, { 16, 228, 160, 32 } };


// position for award xp editbox
const struct window_pos_t awardxpPos[MAX_RESOLUTIONS]= 
{ { 120, 138, 100, 25 }, { 150, 172, 125, 31 }, { 192, 220, 160, 40 } };

// position for post button, relative to goal report window
const struct window_pos_t donePos[MAX_RESOLUTIONS]= 
//{ { 290, 140, 70, 20 }, { 362, 175, 87, 25 }, { 464, 224, 112, 32 } };
{ { 290, 140, 70, 20 }, { 362, 175, 70, 20 }, { 464, 224, 70, 20 } };

// position for exit button, relative to report goal window
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
//{ { 380, 140, 70, 20 }, { 475, 175, 87, 25 }, { 608, 224, 112, 32 } };
{ { 380, 140, 70, 20 }, { 475, 175, 70, 20 }, { 608, 224, 70, 20 } };

BOOL CALLBACK FindEdit( HWND hChild, LPARAM lParam )
{
	TCHAR class_name[30];

	GetClassName(hChild, class_name, sizeof class_name);

	if    (_tcscmp(class_name, _T("Edit")) == 0) 
		SubclassGoalWindow(hChild);

	return true;
}


// Constructor
cReportGoal::cReportGoal(void) 
{
	WNDCLASS wc;

	last_xp_award = 0;

	active = FALSE;
	goal = NULL;
	report = NULL;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = ReportGoalWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("ReportGoal");

    RegisterClass( &wc );

    hwnd_reportgoal =  CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("ReportGoal"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		reportGoalPos[cDD->Res()].x, reportGoalPos[cDD->Res()].y, 		
		reportGoalPos[cDD->Res()].width, reportGoalPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_reportgoal, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_sumlabel = CreateWindow(_T("static"), _T("Summary"), WS_CHILD | SS_BITMAP,
						sumlabelPos[cDD->Res()].x, sumlabelPos[cDD->Res()].y, 
						sumlabelPos[cDD->Res()].width, sumlabelPos[cDD->Res()].height,
						hwnd_reportgoal,
						NULL, hInstance, NULL);
	hSummary = CreateWindowsBitmap(LyraBitmap::SUMMARY_LABEL);
	SendMessage(hwnd_sumlabel, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSummary));

	hwnd_summary = CreateWindow(_T("combobox"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | CBS_AUTOHSCROLL | CBS_DROPDOWN,
						summaryPos[cDD->Res()].x, summaryPos[cDD->Res()].y, 
						summaryPos[cDD->Res()].width, summaryPos[cDD->Res()].height,
						hwnd_reportgoal,
						NULL, hInstance, NULL); 
	SendMessage(hwnd_summary, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	EnumChildWindows(hwnd_summary, FindEdit, NULL);

	hwnd_textlabel = CreateWindow(_T("static"), _T("Text"), WS_CHILD | SS_BITMAP,
						textlabelPos[cDD->Res()].x, textlabelPos[cDD->Res()].y, 
						textlabelPos[cDD->Res()].width, textlabelPos[cDD->Res()].height,
						hwnd_reportgoal,
						NULL, hInstance, NULL);
	hText = CreateWindowsBitmap(LyraBitmap::MESSAGE_LABEL);
	SendMessage(hwnd_textlabel, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hText));

	hwnd_text = CreateWindow(_T("edit"), _T(""),
				 		WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_MULTILINE | 
						ES_LEFT | ES_AUTOVSCROLL | ES_WANTRETURN,
						textPos[cDD->Res()].x, textPos[cDD->Res()].y, 
						textPos[cDD->Res()].width, textPos[cDD->Res()].height,
						hwnd_reportgoal,
						NULL, hInstance, NULL); 
	SendMessage(hwnd_text, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	lpfn_text = SubclassWindow(hwnd_text, ReportGoalTextWProc);
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

	hwnd_xplabel = CreateWindow(_T("static"), _T("XP Award:"), WS_CHILD | SS_BITMAP,
						xplabelPos[cDD->Res()].x, xplabelPos[cDD->Res()].y, 
						xplabelPos[cDD->Res()].width, xplabelPos[cDD->Res()].height,
						hwnd_reportgoal,
						NULL, hInstance, NULL);
	hAward = CreateWindowsBitmap(LyraBitmap::XP_AWARD_LABEL);
	SendMessage(hwnd_xplabel, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hAward));

	hwnd_awardxp = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_TABSTOP | ES_LEFT,
						awardxpPos[cDD->Res()].x, awardxpPos[cDD->Res()].y, 
						awardxpPos[cDD->Res()].width, awardxpPos[cDD->Res()].height,
						hwnd_reportgoal,
						NULL, hInstance, NULL); 
	SendMessage(hwnd_awardxp, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	SubclassGoalWindow(hwnd_awardxp);

	hwnd_done = CreateWindow(_T("button"), _T("Post"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						donePos[cDD->Res()].x, donePos[cDD->Res()].y, 
						donePos[cDD->Res()].width, donePos[cDD->Res()].height,
						hwnd_reportgoal,
						NULL, hInstance, NULL);
	hPost = CreateWindowsBitmap(LyraBitmap::POST_GOAL_BUTTON);
	SendMessage(hwnd_done, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hPost));
	SubclassGoalWindow(hwnd_done);

	hwnd_exit = CreateWindow(_T("button"), _T("Cancel"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_reportgoal,
						NULL, hInstance, NULL);
	hExit = CreateWindowsBitmap(LyraBitmap::CANCEL);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	LoadString (hInstance, IDS_REPORT_DEFAULT_COMPLETED, disp_message, sizeof(message));
_stprintf(message, disp_message, RankGoalName(goals->Rank()));
	ComboBox_AddString(hwnd_summary, message);
	LoadString (hInstance, IDS_REPORT_DEFAULT_FAILED, disp_message, sizeof(message));
_stprintf(message, disp_message, RankGoalName(goals->Rank()));
	ComboBox_AddString(hwnd_summary, message);
	LoadString (hInstance, IDS_REPORT_DEFAULT_CONTINUE, disp_message, sizeof(message));
_stprintf(message, disp_message, RankGoalName(goals->Rank()));
	ComboBox_AddString(hwnd_summary, message);

	return;

}


void cReportGoal::Activate(cGoal *new_goal)
{ 
	if (active)
		return;

	active = TRUE; 
	goal = new_goal;
	Edit_LimitText(hwnd_text, MAX_REPORT_LENGTH-1);
	Edit_SetText(hwnd_text, _T(""));
	Edit_LimitText(hwnd_summary, GOAL_SUMMARY_LENGTH-1);
	Edit_SetText(hwnd_summary, _T(""));

	// send focus to summary window
	SendMessage(hwnd_summary, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_reportgoal);

	ShowWindow(hwnd_awardxp, SW_HIDE);
	ShowWindow(hwnd_xplabel, SW_HIDE);
	ShowWindow(hwnd_summary, SW_SHOWNORMAL);
	ShowWindow(hwnd_sumlabel, SW_SHOWNORMAL);
	ShowWindow(hwnd_text, SW_SHOWNORMAL); 
	ShowWindow(hwnd_textlabel, SW_SHOWNORMAL);
	ShowWindow(hwnd_done, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL); 
	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

	ShowWindow(hwnd_reportgoal, SW_SHOWNORMAL); 

	return;
};


void cReportGoal::Activate(cReport *new_report)
{ 
	if (active)
		return;

	active = TRUE; 
	report = new_report;
	goal = NULL;
	Edit_LimitText(hwnd_text, MAX_REPORT_LENGTH-1);
	Edit_SetText(hwnd_text, _T(""));
	ComboBox_LimitText(hwnd_summary, GOAL_SUMMARY_LENGTH-1);
	ComboBox_SetCurSel(hwnd_summary, -1);
	Edit_LimitText(hwnd_awardxp, 10);
	Edit_SetText(hwnd_awardxp, _T(""));

	// send focus to summary window
	SendMessage(hwnd_summary, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_reportgoal);

	ShowWindow(hwnd_sumlabel, SW_SHOWNORMAL);
	ShowWindow(hwnd_summary, SW_SHOWNORMAL);
	ShowWindow(hwnd_textlabel, SW_SHOWNORMAL);
	ShowWindow(hwnd_text, SW_SHOWNORMAL); 
	ShowWindow(hwnd_done, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL); 
	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

	bool xp_award = false;

//	if    (_tcscmp(goals->UpperPosterName(), player->UpperName()) == 0)
//		xp_award = true;
	if (player->GuildRank(goals->Guild()) == Guild::RULER) 
		xp_award = true;
	if ((player->GuildRank(goals->Guild()) == Guild::KNIGHT) && 
		(goals->Rank() == Guild::INITIATE))
		xp_award = true;
	if    (_tcscmp(player->UpperName(), report->UpperReporterName()) == 0)
		xp_award = false;

	//if (goalbook->InGoalBook(report->GoalID()))
		//xp_award = false;
	
	if (xp_award)
	{
		ShowWindow(hwnd_xplabel, SW_SHOWNORMAL);
		ShowWindow(hwnd_awardxp, SW_SHOWNORMAL);
	}
	else
	{
		ShowWindow(hwnd_xplabel, SW_HIDE);
		ShowWindow(hwnd_awardxp, SW_HIDE);
	}

	ShowWindow(hwnd_reportgoal, SW_SHOWNORMAL);

	return;
};

// called when user is finished with goal report
void cReportGoal::Report(void)
{
	if (!goals->Active())
		return;

	TCHAR report_text[MAX_REPORT_LENGTH];
	TCHAR temp_summary[GOAL_SUMMARY_LENGTH];
	TCHAR summary[Lyra::PLAYERNAME_MAX + 2 + GOAL_SUMMARY_LENGTH];
	TCHAR award_string[7];
	int awardxp;
	TCHAR* stopstring;

	ComboBox_GetText(hwnd_summary, temp_summary, GOAL_SUMMARY_LENGTH);
	Edit_GetText(hwnd_text, report_text, MAX_REPORT_LENGTH);

	if    (_tcscmp(temp_summary,_T("")) == 0)
	{
		LoadString (hInstance, IDS_GOAL_NULLSUMMARY, message, sizeof(message));
		goals->GuildError();
		SendMessage(hwnd_reportgoal, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_summary);
		return;
	}

	// prepend user name
_stprintf(summary, _T("%s: "), player->Name());
_tcscat(summary, temp_summary);

	summary[GOAL_SUMMARY_LENGTH-1]=_T('\0');
	report_text[MAX_REPORT_LENGTH-1]=_T('\0');

	if    (_tcscmp(report_text,_T("")) == 0)
	{
		LoadString (hInstance, IDS_GOAL_NULLTEXT, message, sizeof(message));
		goals->GuildError();
		SendMessage(hwnd_reportgoal, WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_text);
		return;
	}

	awardxp = 0;
	last_xp_award = 0;

	if (//   (_tcscmp(goal->UpperPosterName(), player->UpperName()) == 0) ||
		((player->GuildRank(goals->Guild()) > goals->Rank()) &&
//		 (goal->GuardiansManage() > 0) &&
		 ((report) && !(goalbook->InGoalBook(report->GoalID()))) ||
		 ((goal) && !(goalbook->InGoalBook(goal->ID())))))
	{ // handling for manager reports (may have xp award)
		Edit_GetText(hwnd_awardxp, award_string, 6);
		award_string[6] = _T('\0');
		awardxp = _tcstol (award_string, &stopstring, 10);
		last_xp_award = awardxp;
		int curr_xp_pool = player->GuildXPPool(goals->Guild());
		if (awardxp > curr_xp_pool)
		{
			LoadString (hInstance, IDS_REPORT_TOOMUCHXP, disp_message, sizeof(message));
		_stprintf(message, disp_message, player->GuildXPPool(goals->Guild()));
			goals->GuildError();
			SendMessage(hwnd_reportgoal, WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_awardxp);
			return;
		}
		else
			player->SetGuildXPPool(goals->Guild(), curr_xp_pool-awardxp);

		// now post the manager's report
		if ((report) &&    (_tcscmp(report->UpperReporterName(), player->UpperName()) != 0))
			gs->PostReport(report->GoalID(), awardxp, report->ReporterName(), summary, report_text);
		else
		{
			LoadString (hInstance, IDS_REPORTGOAL_BADRECIPIENT, message, sizeof(message));
			goals->GuildError();
			return;
		}
	}
	else 
	{ // post a non-manager report
		if (report)
			gs->PostReport(report->GoalID(), 0, report->ReporterName(), summary, report_text);
		else
			gs->PostReport(goal->ID(), 0, goal->PosterName(), summary, report_text);
	}

	reportgoal->Deactivate();

}


void cReportGoal::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_text, NULL, TRUE);
}


void cReportGoal::ScrollDown(int count)
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


void cReportGoal::Deactivate(void)
{ 
	if (readgoal->Active())
		readgoal->Deactivate();
	if (readreport->Active())
		readreport->Deactivate();
	if (goalbook->Active())
		goalbook->Deactivate();

	active = FALSE; 
	ShowWindow(hwnd_reportgoal, SW_HIDE); 

	goal = NULL;
	report = NULL;
	if (goals->Active())
		SendMessage(goals->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goals->Hwnd());
	else 
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	return;
}


void cReportGoal::ReportAcknowledged(void)
{
	last_xp_award = 0;
	LoadString (hInstance, IDS_REPORT_POSTED, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
}


void cReportGoal::ReportError(void)
{
	if (last_xp_award > 0)
	{
		player->SetGuildXPPool(goals->Guild(), player->GuildXPPool(goals->Guild())+last_xp_award);
		last_xp_award = 0;
	}

	LoadString (hInstance, IDS_REPORT_ERROR, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
}


// Destructor
cReportGoal::~cReportGoal(void)
{
	if (hAward!=NULL)
		DeleteObject(hAward);
	if (hSummary!=NULL)
		DeleteObject(hSummary);
	if (hText!=NULL)
		DeleteObject(hText);
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
LRESULT WINAPI ReportGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;

	switch(message)
	{
		case WM_PAINT:
			TileBackground(reportgoal->hwnd_reportgoal);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == reportgoal->hwnd_exit)
				reportgoal->Deactivate();
			else if ((HWND)lParam == reportgoal->hwnd_done)
				reportgoal->Report();
			break;

		case WM_KEYUP:
			if ((UINT)(wParam) == VK_ESCAPE)
			{
				reportgoal->Deactivate();
				return 0L;
			}
			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Subclassed window procedure for the rich edit control
LRESULT WINAPI ReportGoalTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
				SendMessage(reportgoal->hwnd_reportgoal, message,
					(WPARAM) wParam, (LPARAM) lParam);
				SendMessage(reportgoal->hwnd_reportgoal, WM_ACTIVATE,
					(WPARAM) WA_CLICKACTIVE, (LPARAM) reportgoal->hwnd_reportgoal);
				return (LRESULT) 0;
			}
			InvalidateRect(reportgoal->hwnd_text, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_KEYUP:
			if ((wParam) == VK_RETURN)
				InvalidateRect(reportgoal->hwnd_text, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_MOUSEMOVE:
			{
				int selection = Edit_GetSel(reportgoal->hwnd_text);
				int start = LOWORD(selection);
				int finish = HIWORD(selection);
				if (start != finish)
					InvalidateRect(reportgoal->hwnd_text, &(button_strip[cDD->Res()]), TRUE);
			}
			break;

		case WM_KILLFOCUS:
		case WM_SETFOCUS:
			InvalidateRect(reportgoal->hwnd_text, &(button_strip[cDD->Res()]), TRUE);
			break;

		case WM_LBUTTONDOWN:
			InvalidateRect(reportgoal->hwnd_text, &(button_strip[cDD->Res()]), TRUE);
			break;
		
		case WM_RBUTTONDOWN:
			return 0;

		case WM_COMMAND:
			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((HWND)lParam == reportgoal->hwnd_text_buttons[j])
				{
					if (j == DOWN)
						reportgoal->ScrollDown(1);
					else
						reportgoal->ScrollUp(1);
					break;
				}
			}
			SendMessage(reportgoal->hwnd_reportgoal, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) reportgoal->hwnd_reportgoal);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
            dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == reportgoal->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, reportgoal->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == reportgoal->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, reportgoal->text_buttons_bitmaps[j][1]); 
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

void cReportGoal::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
