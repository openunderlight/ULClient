// Handles reviewing goal reports

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cGoalPosting.h"
#include "cReviewGoals.h"
#include "cReadReport.h"
#include "cReadGoal.h"
#include "cGoalBook.h"
#include "cChat.h"
#include "resource.h"
#include "utils.h"
#include "interface.h"
#include "cEffects.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern cEffects *effects;
extern cDDraw *cDD;
extern HINSTANCE hInstance;
extern cChat *display;
extern cPlayer *player;
extern cGoalPosting *goals;
extern cReviewGoals *reviewgoals;
extern cReadReport *readreport;
extern cGoalBook *goalbook;
extern cReadGoal *readgoal;

//////////////////////////////////////////////////////////////////
// Constants

const int NO_NEW_REPORTS = -1;
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
	DDOWN, LyraBitmap::CP_DDOWNA  },   // ddown button
{ {{ 442, 0, 13, 25 }, { 556, 0, 13, 25 }, { 717, 0, 13, 25 } },
			DUP, LyraBitmap::CP_DUPA  },   // dup button
{ {{ 442, 110, 13, 15 }, { 556, 110, 13, 15 }, { 717, 110, 13, 15 } },
	DOWN, LyraBitmap::CP_DOWNA  },     // down button
{ {{ 442, 25, 13, 15 }, { 556, 25, 13, 15 }, { 717, 25, 13, 15 } },
	UP, LyraBitmap::CP_UPA  }   // up button
 };

// position for goal review window, relative to main
const struct window_pos_t reviewGoalsPos[MAX_RESOLUTIONS]= 
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

// position for goal report summaries
const struct window_pos_t reportsPos[MAX_RESOLUTIONS]= 
//{ { 5, 65, 460, 170 }, { 6, 81, 575, 212 }, { 8, 104, 736, 272 } };
{ { 5, 65, 460, 170 }, { 6, 128, 575, 170 }, { 8, 154, 736, 170 } };

// position for read report button
const struct window_pos_t readPos[MAX_RESOLUTIONS]= 
//{ { 290, 230, 70, 20 }, { 362, 287, 87, 25 }, { 464, 368, 112, 32 } };
{ { 290, 230, 70, 20 }, { 362, 287, 70, 20 }, { 464, 368, 70, 20 } };


// position for refresh button
const struct window_pos_t refreshPos[MAX_RESOLUTIONS]= 
//{ { 380, 230, 70, 20 }, { 475, 287, 87, 25 }, { 608, 368, 112, 32 } };
{ { 380, 230, 70, 20 }, { 475, 287, 70, 20 }, { 608, 368, 70, 20 } };

// position for exit button, relative to goal reviewing window
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
//{ { 380, 260, 70, 20 }, { 475, 325, 87, 25 }, { 608, 416, 112, 32 } };
{ { 380, 260, 70, 20 }, { 475, 325, 70, 20 }, { 608, 416, 70, 20 } };


/////////////////////////////////////////////////////////////////
// Class Definition

// Constructor
cReviewGoals::cReviewGoals(void) 
{
	WNDCLASS wc;

	active = FALSE;
	next_report = 0;
	page_num = 0;
	last_report_seen = Lyra::ID_UNKNOWN;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = ReviewGoalsWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("ReviewGoals");

    RegisterClass( &wc );

    hwnd_reviewgoals =  CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("ReviewGoals"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		reviewGoalsPos[cDD->Res()].x, reviewGoalsPos[cDD->Res()].y, 		
		reviewGoalsPos[cDD->Res()].width, reviewGoalsPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_reviewgoals, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_title = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						titlePos[cDD->Res()].x, titlePos[cDD->Res()].y, 
						titlePos[cDD->Res()].width, titlePos[cDD->Res()].height,
						hwnd_reviewgoals,
						NULL, hInstance, NULL);

	hwnd_logo = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						logoPos[cDD->Res()].x, logoPos[cDD->Res()].y, 
						logoPos[cDD->Res()].width, logoPos[cDD->Res()].height,
						hwnd_reviewgoals,
						NULL, hInstance, NULL);

	hwnd_summariestext = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						summariestextPos[cDD->Res()].x, summariestextPos[cDD->Res()].y, 
						summariestextPos[cDD->Res()].width, summariestextPos[cDD->Res()].height,
						hwnd_reviewgoals,
						NULL, hInstance, NULL);
	hSummaries = CreateWindowsBitmap(LyraBitmap::REPORTS_LABEL);
	SendMessage(hwnd_summariestext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSummaries));

	hwnd_summaries = CreateWindow(_T("listbox"), _T(""),
						WS_CHILD | WS_DLGFRAME | LBS_NOTIFY,
						reportsPos[cDD->Res()].x, reportsPos[cDD->Res()].y, 
						reportsPos[cDD->Res()].width, reportsPos[cDD->Res()].height,
						hwnd_reviewgoals,
						NULL, hInstance, NULL); 
	SendMessage(hwnd_summaries, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	lpfn_text = SubclassWindow(hwnd_summaries, ReportSummariesWProc);
	SendMessage(hwnd_summaries, WM_PASSPROC, 0, (LPARAM) lpfn_text ); 

	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
	{
		hwnd_text_buttons[i] = CreateWindow(_T("button"), _T(""),
				WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
				text_buttons[i].position[cDD->Res()].x, text_buttons[i].position[cDD->Res()].y, 
				text_buttons[i].position[cDD->Res()].width, text_buttons[i].position[cDD->Res()].height,
				hwnd_summaries, NULL, hInstance, NULL);
		text_buttons_bitmaps[i][0] = // a button
			CreateWindowsBitmap(text_buttons[i].bitmap_id);
		text_buttons_bitmaps[i][1] = // b button
			CreateWindowsBitmap(text_buttons[i].bitmap_id + 1);
	}


	hwnd_read = CreateWindow(_T("button"), _T("Read Report"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						readPos[cDD->Res()].x, readPos[cDD->Res()].y, 
						readPos[cDD->Res()].width, readPos[cDD->Res()].height,
						hwnd_reviewgoals,
						NULL, hInstance, NULL); 
	hReadReport = CreateWindowsBitmap(LyraBitmap::READ_GOAL_BUTTON);
	SendMessage(hwnd_read, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hReadReport));
	SubclassGoalWindow(hwnd_read);

	hwnd_refresh = CreateWindow(_T("button"), _T("Refresh"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						refreshPos[cDD->Res()].x, refreshPos[cDD->Res()].y, 
						refreshPos[cDD->Res()].width, refreshPos[cDD->Res()].height,
						hwnd_reviewgoals,
						NULL, hInstance, NULL); 
	hRefresh = CreateWindowsBitmap(LyraBitmap::REFRESH_LIST_BUTTON);
	SendMessage(hwnd_refresh, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hRefresh));
	SubclassGoalWindow(hwnd_refresh);
	
	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_reviewgoals,
						NULL, hInstance, NULL);
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	return;

}


//void cReviewGoals::Activate(cGoal *new_goal)
void cReviewGoals::Activate(realmid_t goal)
{ 
	if (active || !(goals->Active()))
		return;

	if (readreport->Active())
		readreport->Deactivate();

	active = TRUE; 
//	goal = new_goal;
	goal_id = goal;
	next_report = 0;
	page_num = 0;
	last_report_seen = Lyra::ID_UNKNOWN;
	ListBox_ResetContent(hwnd_summaries);

	hTitle = CreateWindowsBitmap(LyraBitmap::HOUSE_GOALBOARD_TITLES+goals->Guild()+((goals->Rank()-1)*NUM_GUILDS));
	SendMessage(hwnd_title, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hTitle);

	hLogo = CreateWindowsBitmap(LyraBitmap::HOUSE_BITMAPS+goals->Guild()+((goals->Rank()-1)*NUM_GUILDS));
	SendMessage(hwnd_logo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogo);

	ShowWindow(hwnd_title, SW_SHOWNORMAL);
	ShowWindow(hwnd_logo, SW_SHOWNORMAL);
	ShowWindow(hwnd_summariestext, SW_SHOWNORMAL);
	ShowWindow(hwnd_summaries, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL); 
	ShowWindow(hwnd_read, SW_SHOWNORMAL);
	ShowWindow(hwnd_refresh, SW_SHOWNORMAL);

	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

	// use session id so we can identify stale messages 
	session_id = rand();

	gs->RequestReportHeaders(goal_id, goals->Guild(), goals->Rank(), session_id, last_report_seen);

	if (goal_id == NO_GOAL)
	{
		LoadString(hInstance, IDS_NO_NEW_REPORTS, message, sizeof(message));
		ListBox_AddString(hwnd_summaries, message);
		ListBox_SetItemData(hwnd_summaries, 0, NO_NEW_REPORTS);
	}

	ListBox_SetCurSel(hwnd_summaries, 0);

	ShowWindow(hwnd_reviewgoals, SW_SHOWNORMAL); 

	return;
};

void cReviewGoals::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_reviewgoals, SW_HIDE); 

	goal_id = Lyra::ID_UNKNOWN;

	if (hTitle!=NULL)
		DeleteObject(hTitle);
	if (hLogo!=NULL)
		DeleteObject(hLogo);

	if (readgoal->Active())
		readgoal->Deactivate();
	if (readreport->Active())
		readreport->Deactivate();

	for (int i=0; i<next_report; i++)
	{
		ListBox_DeleteString(hwnd_summaries, 0);
		delete curr_reports[i];
	}

	next_report = 0;
	page_num = 0;
	last_report_seen = Lyra::ID_UNKNOWN;

	if (goals->Active())
		SendMessage(goals->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goals->Hwnd());
	else 
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	return;
}


// summary info has arrived for a new report
void cReviewGoals::NewReportInfo(int sessionid, realmid_t report_id, realmid_t goal_id, const TCHAR *report_summary)
{	
	int num_reports_shown;

	if ((!active) || (this->session_id != sessionid))
		return;

	if ((ListBox_GetCount(hwnd_summaries) == 1) 
		&& (ListBox_GetItemData(hwnd_summaries, 0) == NO_NEW_REPORTS))
		ListBox_ResetContent(hwnd_summaries);

	// check to see if this report is already displayed
	for (int i = 0; i < next_report; i++)
		if (report_id == curr_reports[i]->ID())
		{ // report is already in the array
			ListBox_AddString(hwnd_summaries, report_summary);
			num_reports_shown = ListBox_GetCount(hwnd_summaries);
			ListBox_SetItemData(hwnd_summaries, num_reports_shown-1, report_id);
			return;
		}

	// make sure we don't overrun allocated memory
	if (next_report == MAX_SIMUL_GOALS)
		return;

	curr_reports[next_report] = new cReport();
	curr_reports[next_report]->SetInfo(goal_id, report_id, report_summary);
	next_report++;

	ListBox_AddString(hwnd_summaries, report_summary);
	num_reports_shown = ListBox_GetCount(hwnd_summaries);
	ListBox_SetItemData(hwnd_summaries, num_reports_shown-1, report_id);

	if (num_reports_shown == VISIBLE_LINES)
		last_report_seen = report_id;

	if (ListBox_GetCurSel(hwnd_summaries) == -1)
		ListBox_SetCurSel(hwnd_summaries,0);
}

// report text has arrived
void cReviewGoals::NewReportText(realmid_t report_id, int awardxp, int flags, const TCHAR *creator, 
		const TCHAR* recipient, const TCHAR *text)
{	
	if (!active)
		return;

	for (int i=0; i<next_report; i++)
		if (curr_reports[i]->ID() == report_id)
		{ // found a match, set text
			curr_reports[i]->SetText(awardxp, flags, creator, recipient, text);
			if (readreport->Active() && (readreport->CurrReportID() == report_id))
				readreport->SetText();
			return;
		}
	LoadString(hInstance, IDS_UNKNOWN_REPORT, message, sizeof(message));
	NONFATAL_ERROR(message);
	return;
}


// User requests next page of reports
void cReviewGoals::NextPage(void)
{
	// don't bother if there isn't a full page already
	if (ListBox_GetCount(hwnd_summaries) < 10)
		return;

	ListBox_ResetContent(hwnd_summaries);

	page_num++;

	if (next_report < (page_num+1)*10)
		gs->RequestReportHeaders(goal_id, goals->Guild(), goals->Rank(), session_id, last_report_seen);
	else
	{
		for (int i = 0; i < 10; i++)
		{
			ListBox_AddString(hwnd_summaries, curr_reports[(page_num*10)+i]->Summary());
			ListBox_SetItemData(hwnd_summaries, i, curr_reports[(page_num*10)+i]->ID());
		}

		ListBox_SetCurSel(hwnd_summaries, 0);
	}
}


void cReviewGoals::NextLine(void)
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


void cReviewGoals::PrevLine(void)
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


// User requests prev page of reports
void cReviewGoals::PrevPage(void)
{
	if (page_num > 0)
		page_num--;
	else
		return;

	ListBox_ResetContent(hwnd_summaries);

	for (int i = 0; i < 10; i++)
	{
		ListBox_AddString(hwnd_summaries, curr_reports[(page_num*10)+i]->Summary());
		ListBox_SetItemData(hwnd_summaries, i, curr_reports[(page_num*10)+i]->ID());
	}

	ListBox_SetCurSel(hwnd_summaries, VISIBLE_LINES-1);
}



void cReviewGoals::ReadReport(void)
{
	int selected = ListBox_GetCurSel(hwnd_summaries);
	if ((selected == -1) || (ListBox_GetItemData(hwnd_summaries, selected) == NO_NEW_REPORTS))
		return;

	int searchreport = ListBox_GetItemData(hwnd_summaries, selected);

	for (int i=0; i<next_report; i++)
		if (curr_reports[i]->ID() == searchreport)
		{ // found the report, activate readreport
			readreport->Activate(curr_reports[i]);
			return;
		}

	return;
}


void cReviewGoals::RefreshSummaries(void)
{
	ListBox_ResetContent(hwnd_summaries);

	for (int i=0; i<next_report; i++)
	{
		ListBox_DeleteString(hwnd_summaries, 0);
		delete curr_reports[i];
	}

	next_report = 0;
	page_num = 0;
	last_report_seen = 0;
	session_id++;

	gs->RequestReportHeaders(goal_id, goals->Guild(), goals->Rank(), session_id, last_report_seen);

	return;
}


void cReviewGoals::ReportNotFound(realmid_t report_id)
{
	if (readreport->Active())
		readreport->Deactivate();
	if (goalbook->Active())
		goalbook->Deactivate();

	LoadString (hInstance, IDS_REPORT_NOTFOUND, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
}



// Destructor
cReviewGoals::~cReviewGoals(void)
{
	if (hSummaries!=NULL)
		DeleteObject(hSummaries);
	if (hReadReport!=NULL)
		DeleteObject(hReadReport);
	if (hExit!=NULL)
		DeleteObject(hExit);
	if (hRefresh!=NULL)
		DeleteObject(hRefresh);
	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
	{
		if (text_buttons_bitmaps[i][0])
			DeleteObject(text_buttons_bitmaps[i][0]);
		if (text_buttons_bitmaps[i][1])
			DeleteObject(text_buttons_bitmaps[i][1]);
	}
}


// Subclassed window procedure for the rich edit control
LRESULT WINAPI ReportSummariesWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
			SendMessage(reviewgoals->hwnd_reviewgoals, message,
				(WPARAM) wParam, (LPARAM) lParam);
			return 0L;

		case WM_KEYUP:
			switch ((UINT)(wParam)) {
				case VK_ESCAPE:
					if (readreport->Active())
						readreport->Deactivate();
					else
						reviewgoals->Deactivate();
					return 0L;
				case VK_RETURN:
					reviewgoals->ReadReport();
					return 0L;
				case VK_UP:
					reviewgoals->PrevLine();
					return 0L;
				case VK_DOWN:
					reviewgoals->NextLine();
					return 0L;
				default:
					break;
			}
			break;

		case WM_LBUTTONDOWN:
		case LB_SETCURSEL:
			{
				for (int i=0; i<NUM_GOAL_BUTTONS; i++)
					InvalidateRect(reviewgoals->hwnd_text_buttons[i], NULL, false);
			}
			break;

		case WM_COMMAND:
			for (j=0; j<NUM_GOAL_BUTTONS; j++)
			{
				if ((HWND)lParam == reviewgoals->hwnd_text_buttons[j])
				{
					if (j == DDOWN)
						reviewgoals->NextPage();
					else if (j == DUP)
						reviewgoals->PrevPage();
					else if (j == DOWN)
						reviewgoals->NextLine();
					else 
						reviewgoals->PrevLine();
					break;
				}
			}
			SendMessage(reviewgoals->hwnd_reviewgoals, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) reviewgoals->hwnd_reviewgoals);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 

			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_GOAL_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == reviewgoals->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, reviewgoals->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == reviewgoals->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, reviewgoals->text_buttons_bitmaps[j][1]); 
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


// Window procedure for the read goal window
LRESULT WINAPI ReviewGoalsWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;

	switch(message)
	{
		case WM_PAINT:
			if (goals->Rank() < 2)
				TileBackground(reviewgoals->hwnd_reviewgoals);
			else
				TileBackground(reviewgoals->hwnd_reviewgoals, GOLD_BACKGROUND);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == reviewgoals->hwnd_exit)
				reviewgoals->Deactivate();
			else if ((HWND)lParam == reviewgoals->hwnd_read)
				reviewgoals->ReadReport();
			else if ((HWND)lParam == reviewgoals->hwnd_refresh)
				reviewgoals->RefreshSummaries();
			else if ((HWND)lParam == reviewgoals->hwnd_summaries)
			{
				switch (HIWORD(wParam))
				{
					case LBN_DBLCLK:
						reviewgoals->ReadReport();
						for (int i=0; i<NUM_GOAL_BUTTONS; i++)
							InvalidateRect(reviewgoals->hwnd_text_buttons[i], NULL, false);
						break;
				}
			}
			break;

		case WM_KEYUP:
			switch ((UINT)(wParam)) {
				case VK_ESCAPE:
					reviewgoals->Deactivate();
					return 0L;
				case VK_RETURN:
					reviewgoals->ReadReport();
					return 0L;
				case VK_UP:
					reviewgoals->PrevLine();
					return 0L;
				case VK_DOWN:
					reviewgoals->NextLine();
					return 0L;

				default:
					break;
			}
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Check invariants

#ifdef CHECK_INVARIANTS

void cReviewGoals::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
