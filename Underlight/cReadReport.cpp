// Handles the report reading screen

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cGoalPosting.h"
#include "cReadReport.h"
#include "cReportGoal.h"
#include "cReviewGoals.h"
#include "cGoalBook.h"
#include "cChat.h"
#include "Options.h"
#include "utils.h"
#include "interface.h"
#include "cEffects.h"
#include "resource.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern cEffects *effects;
extern cDDraw *cDD;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cChat *display;
extern cGoalPosting *goals;
extern cReadReport *readreport;
extern cReportGoal *reportgoal;
extern cReviewGoals *reviewgoals;
extern cGoalBook *goalbook;
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
{  { 
	{ { 432, 39, 13, 15 }, {543, 54, 13, 15 }, { 701, 74, 13, 15 } },
		DOWN, LyraBitmap::CP_DOWNA  },   // down button
	{ { {432, 0, 13, 15 }, {543, 0, 13, 15 }, { 701, 0, 13, 15 } },
		UP, LyraBitmap::CP_UPA  }    // up button
};

// position for read report window, relative to main
const struct window_pos_t readReportPos[MAX_RESOLUTIONS]= 
{ { 0, 300, 480, 180 }, { 0, 375, 600, 225 }, { 0, 480, 768, 288 } };

// position for label for recipient of report 
const struct window_pos_t recipienttextPos[MAX_RESOLUTIONS]= 
{ { 10, 12, 125, 20 }, { 12, 15, 156, 25 }, { 16, 19, 200, 32 } };

// position for recipient of report 
const struct window_pos_t recipientPos[MAX_RESOLUTIONS]= 
{ { 120, 7, 100, 25 }, { 150, 8, 125, 31 }, { 192, 11, 160, 40 } };

// position for label for creator of report 
const struct window_pos_t creatortextPos[MAX_RESOLUTIONS]= 
{ { 12, 42, 125, 20 }, { 15, 52, 156, 25 }, { 19, 67, 200, 32 } };

// position for creator of report 
const struct window_pos_t creatorPos[MAX_RESOLUTIONS]= 
{ { 120, 37, 100, 25 }, { 150, 46, 125, 31 }, { 192, 59, 160, 40 } };

// position for label for awarded xp of report 
const struct window_pos_t awardtextPos[MAX_RESOLUTIONS]= 
{ { 260, 12, 120, 20 }, { 325, 15, 150, 25 }, { 416, 19, 192, 32 } };

// position for awarded xp of report 
const struct window_pos_t awardPos[MAX_RESOLUTIONS]= 
{ { 365, 7, 90, 25 }, { 456, 8, 112, 31 }, { 584, 11, 144, 40 } };

// position for report text, relative to read report window
const struct window_pos_t textPos[MAX_RESOLUTIONS]= 
{ { 10, 70, 450, 60 }, { 12, 87, 562, 75 }, { 16, 112, 720, 96 } };

// position for delete button, relative to read goal window
const struct window_pos_t deletePos[MAX_RESOLUTIONS]= 
//{ { 200, 140, 70, 20 }, { 250, 175, 87, 25 }, { 320, 224, 112, 32 } };
{ { 200, 140, 70, 20 }, { 250, 175, 70, 20 }, { 320, 224, 70, 20 } };


// position for reply button, relative to read report window
const struct window_pos_t replyPos[MAX_RESOLUTIONS]= 
//{ { 290, 140, 70, 20 }, { 362, 175, 87, 25 }, { 464, 224, 112, 32 } };
{ { 290, 140, 70, 20 }, { 362, 175, 70, 20 }, { 464, 224, 70, 20 } };

// position for exit button, relative to read report window
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
//{ { 380, 140, 70, 20 }, { 475, 175, 87, 25 }, { 608, 224, 112, 32 } };
{ { 380, 140, 70, 20 }, { 475, 175, 70, 20 }, { 608, 224, 70, 20 } };

// Constructor
cReadReport::cReadReport(void) 
{
	WNDCLASS wc;

	active = FALSE;

	report = NULL;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = ReadReportWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("ReadReport");

    RegisterClass( &wc );

    hwnd_readreport = CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("ReadReport"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		readReportPos[cDD->Res()].x, readReportPos[cDD->Res()].y, 		
		readReportPos[cDD->Res()].width, readReportPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_readreport, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_recipienttext = CreateWindow(_T("static"), _T("Report To:"),
						WS_CHILD | SS_BITMAP,
						recipienttextPos[cDD->Res()].x, recipienttextPos[cDD->Res()].y, 
						recipienttextPos[cDD->Res()].width, recipienttextPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	hRecipient = CreateWindowsBitmap(LyraBitmap::REPORT_TO_LABEL);
	SendMessage(hwnd_recipienttext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hRecipient));

	hwnd_recipient = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						recipientPos[cDD->Res()].x, recipientPos[cDD->Res()].y, 
						recipientPos[cDD->Res()].width, recipientPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	SendMessage(hwnd_recipient, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_awardtext = CreateWindow(_T("static"), _T("Awarded XP:"),
						WS_CHILD | SS_BITMAP,
						awardtextPos[cDD->Res()].x, awardtextPos[cDD->Res()].y, 
						awardtextPos[cDD->Res()].width, awardtextPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	hAward = CreateWindowsBitmap(LyraBitmap::XP_AWARD_LABEL);
	SendMessage(hwnd_awardtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hAward));

	hwnd_award = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						awardPos[cDD->Res()].x, awardPos[cDD->Res()].y, 
						awardPos[cDD->Res()].width, awardPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	SendMessage(hwnd_award, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_creatortext = CreateWindow(_T("static"), _T("Report From:"),
						WS_CHILD | SS_BITMAP,
						creatortextPos[cDD->Res()].x, creatortextPos[cDD->Res()].y, 
						creatortextPos[cDD->Res()].width, creatortextPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	hCreator = CreateWindowsBitmap(LyraBitmap::REPORT_FROM_LABEL);
	SendMessage(hwnd_creatortext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hCreator));

	hwnd_creator = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						creatorPos[cDD->Res()].x, creatorPos[cDD->Res()].y, 
						creatorPos[cDD->Res()].width, creatorPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	SendMessage(hwnd_creator, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_text = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | ES_WANTRETURN | ES_MULTILINE | ES_LEFT | ES_READONLY,
						textPos[cDD->Res()].x, textPos[cDD->Res()].y, 
						textPos[cDD->Res()].width, textPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	SendMessage(hwnd_text, WM_SETFONT, WPARAM(goals->Hfont()), 0);
	lpfn_text = SubclassWindow(hwnd_text, ReadReportTextWProc);
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

	hwnd_delete = CreateWindow(_T("button"), _T("Delete Goal"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						deletePos[cDD->Res()].x, deletePos[cDD->Res()].y, 
						deletePos[cDD->Res()].width, deletePos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	hDelete = CreateWindowsBitmap(LyraBitmap::REMOVE_GOAL_BUTTON);
	SendMessage(hwnd_delete, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hDelete));
	SubclassGoalWindow(hwnd_delete);

	hwnd_reply = CreateWindow(_T("button"), _T("Reply"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						replyPos[cDD->Res()].x, replyPos[cDD->Res()].y, 
						replyPos[cDD->Res()].width, replyPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	hReply = CreateWindowsBitmap(LyraBitmap::REPLY_REPORT_BUTTON);
	SendMessage(hwnd_reply, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hReply));
	SubclassGoalWindow(hwnd_reply);

	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_readreport,
						NULL, hInstance, NULL);
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	return;

}

// it's ok to call this when active - just replace the current report text
void cReadReport::Activate(cReport *new_report)
{ 
	active = TRUE; 
	report = new_report;
	
	ShowWindow(hwnd_delete, SW_HIDE);
	ShowWindow(hwnd_reply, SW_HIDE); 

	ShowWindow(hwnd_creatortext, SW_SHOWNORMAL);
	ShowWindow(hwnd_creator, SW_SHOWNORMAL);
	ShowWindow(hwnd_recipienttext, SW_SHOWNORMAL);
	ShowWindow(hwnd_recipient, SW_SHOWNORMAL);
	ShowWindow(hwnd_text, SW_SHOWNORMAL); 
	ShowWindow(hwnd_exit, SW_SHOWNORMAL); 
	for (int i=0; i<NUM_TEXT_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

//	if (!report->Goal()->Text())
//		gs->RequestGoalText(report->Goal()->ID());

	this->SetText();

	ShowWindow(hwnd_readreport, SW_SHOWNORMAL); 

	return;
};

void cReadReport::Deactivate(void)
{ 
	active = FALSE; 
	report = NULL;

	ShowWindow(hwnd_readreport, SW_HIDE); 
	
	if (reviewgoals->Active())
		SendMessage(reviewgoals->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) reviewgoals->Hwnd());
	else if (goals->Active())
		SendMessage(goals->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goals->Hwnd());
	else
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	return;
}

// if text is available, display it; otherwise put loading message up
void cReadReport::SetText()
{
	if (report->Text())
	{
		Edit_SetText(hwnd_creator, report->ReporterName());
		Edit_SetText(hwnd_recipient, report->Recipient());
		Edit_SetText(hwnd_text, report->Text());

		if (goalbook->InGoalBook(report->GoalID()))
			ShowWindow(hwnd_reply, SW_HIDE);
		else
			ShowWindow(hwnd_reply, SW_SHOWNORMAL);
		
		if    (_tcscmp(report->UpperRecipient(), player->UpperName()) == 0)
		{
			ShowWindow(hwnd_delete, SW_SHOWNORMAL);
		}

		else
		{
			if    (_tcscmp(report->UpperReporterName(), player->UpperName()) == 0)
				ShowWindow(hwnd_delete, SW_SHOWNORMAL);
			else
				ShowWindow(hwnd_delete, SW_HIDE);

//			if (report->Goal()->GuardiansManage() < 0)
//				gs->RequestGoalGuardianFlag(report->Goal()->ID());
		}

		if (report->AwardXP() == 0)
		{
			ShowWindow(hwnd_award, SW_HIDE);
			ShowWindow(hwnd_awardtext, SW_HIDE);
		}
		else
		{
			ShowWindow(hwnd_award, SW_SHOWNORMAL);
			ShowWindow(hwnd_awardtext, SW_SHOWNORMAL);
		_stprintf(message, _T("%i"), report->AwardXP());
			Edit_SetText(hwnd_award, message);
		}
	}
	else
	{
		gs->RequestReportText(report->ID());

		ShowWindow(hwnd_delete, SW_HIDE);
		ShowWindow(hwnd_reply, SW_HIDE);
		ShowWindow(hwnd_award, SW_HIDE);
		ShowWindow(hwnd_awardtext, SW_HIDE);
		
		Edit_SetText(hwnd_creator, _T(""));
		Edit_SetText(hwnd_recipient, _T(""));
		LoadString(hInstance, IDS_RETRIEVE_REPORT_INFO, message, sizeof(message));
		Edit_SetText(hwnd_text, message);
		//Edit_SetText(hwnd_text, _T("Retrieving report information..."));
	}
}


void cReadReport::DeleteReport(void)
{
	if (   (_tcscmp(report->UpperRecipient(), player->UpperName()) == 0) ||
		   (_tcscmp(report->UpperReporterName(), player->UpperName()) == 0))
	{
		LoadString (hInstance, IDS_DELETEREPORT_WARN, message, sizeof(message));
		goals->GuildWarning(&DeleteReportCallback);
	}

	return;
}


void DeleteReportCallback(void *value)
{
	int deletereport = *((int*)value);
	
	if (   (_tcscmp(readreport->report->UpperRecipient(), player->UpperName()) == 0) ||
		   (_tcscmp(readreport->report->UpperReporterName(), player->UpperName()) == 0))
	{
		if (deletereport == 1)
		{
			gs->DeleteReport(readreport->report->ID());
			readreport->Deactivate();
		}
		else
		{
			LoadString (hInstance, IDS_DELETEREPORT_ABORTED, message, sizeof(message));
			goals->GuildError();
		}
	}
	
	return;
}


void cReadReport::DeleteAcknowledged(realmid_t reportid)
{
	LoadString (hInstance, IDS_DELETEREPORT_ACK, message, sizeof(message));
	display->DisplayMessage(message);
}


void cReadReport::DeleteError(void)
{
	LoadString (hInstance, IDS_DELETEREPORT_ERROR, message, sizeof(message));
	display->DisplayMessage(message);
}


void cReadReport::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_text, NULL, TRUE);
}


void cReadReport::ScrollDown(int count)
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
cReadReport::~cReadReport(void)
{
	if (hCreator!=NULL)
		DeleteObject(hCreator);
	if (hRecipient!=NULL)
		DeleteObject(hRecipient);
	if (hAward!=NULL)
		DeleteObject(hAward);
	if (hReply!=NULL)
		DeleteObject(hReply);
	if (hDelete!=NULL)
		DeleteObject(hDelete);
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


// Window procedure for the read report window
LRESULT WINAPI ReadReportWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;

	if (HBRUSH brush = SetControlColors(hwnd, message, wParam, lParam, true))
		return (LRESULT)brush; 

	switch(message)
	{
		case WM_PAINT:
			TileBackground(readreport->hwnd_readreport);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == readreport->hwnd_reply)
				reportgoal->Activate(readreport->Report());
			else if ((HWND)lParam == readreport->hwnd_exit)
				readreport->Deactivate();
			else if ((HWND)lParam == readreport->hwnd_delete)
				readreport->DeleteReport();
			break;

		case WM_KEYUP:
			if ((UINT)(wParam) == VK_ESCAPE)
			{
				readreport->Deactivate();
				return 0L;
			}
			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Subclassed window procedure for the rich edit control
LRESULT WINAPI ReadReportTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
			SendMessage(readreport->hwnd_readreport, message,
				(WPARAM) wParam, (LPARAM) lParam);
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			SendMessage(readreport->hwnd_readreport, WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) readreport->hwnd_readreport);
			return (LRESULT) 0;
		case WM_COMMAND:
			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((HWND)lParam == readreport->hwnd_text_buttons[j])
				{
					if (j == DOWN)
						readreport->ScrollDown(1);
					else
						readreport->ScrollUp(1);
					break;

				}
			}
			SendMessage(readreport->hwnd_readreport, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) readreport->hwnd_readreport);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == readreport->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, readreport->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == readreport->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, readreport->text_buttons_bitmaps[j][1]); 
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

void cReadReport::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
