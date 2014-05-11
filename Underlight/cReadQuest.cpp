// Handles the quest-posting screens

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cQuestBuilder.h"
#include "cPostQuest.h"
#include "cReadQuest.h"
#include "cGoalBook.h"
#include "cDetailQuest.h"
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
extern cQuestBuilder *quests;
extern cPostQuest *postquest;
extern cReadQuest *readquest;
extern cGoalBook *goalbook;
extern cDetailQuest *detailquest;
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

// position for read quest window, relative to main
const struct window_pos_t readQuestPos[MAX_RESOLUTIONS]= 
{ { 0, 300, 480, 180 }, { 0, 375, 600, 225 }, { 0, 480, 768, 288 } };

// position for label for suggested sphere of new quest 
const struct window_pos_t spheretextPos[MAX_RESOLUTIONS]= 
{ { 10, 12, 125, 20 }, { 12, 15, 156, 25 }, { 16, 19, 200, 32 } };

// position for suggested sphere of new quest 
const struct window_pos_t sugspherePos[MAX_RESOLUTIONS]= 
{ { 125, 7, 100, 25 }, { 156, 11, 125, 25 }, { 200, 17, 140, 25 } };

// position for label for suggested sphere of new quest 
const struct window_pos_t stattextPos[MAX_RESOLUTIONS]= 
{ { 260, 12, 110, 20 }, { 325, 15, 137, 25 }, { 416, 19, 176, 32 } };

// position for suggested stat of new quest 
const struct window_pos_t sugstatPos[MAX_RESOLUTIONS]= 
{ { 365, 7, 90, 25 }, { 456, 11, 90, 25 }, { 584, 18, 90, 25 } };

// position for label for suggested sphere of new quest 
const struct window_pos_t creatortextPos[MAX_RESOLUTIONS]= 
{ { 10, 42, 125, 20 }, { 12, 52, 156, 25 }, { 16, 67, 200, 32 } };

// position for suggested sphere of new quest 
const struct window_pos_t creatorPos[MAX_RESOLUTIONS]= 
{ { 125, 37, 100, 25 }, { 156, 49, 125, 25 }, { 200, 65, 140, 25 } };

// position for quest text, relative to read quest window
const struct window_pos_t textPos[MAX_RESOLUTIONS]= 
{ { 10, 70, 450, 60 }, { 12, 87, 562, 75 }, { 16, 112, 720, 96 } };

// position for accept button, relative to read quest window
const struct window_pos_t acceptPos[MAX_RESOLUTIONS]= 
{ { 200, 140, 70, 20 }, { 250, 175, 70, 20 }, { 320, 224, 70, 20 } };

// position for edit button, relative to read quest window
//const struct window_pos_t editPos[MAX_RESOLUTIONS]= 
//{ { 80, 140, 70, 20 }, { 100, 175, 70, 20 }, { 128, 224, 70, 20 } };

// position for complete button, relative to read quest window
const struct window_pos_t completePos[MAX_RESOLUTIONS]= 
{ { 170, 140, 100, 20 }, { 212, 175, 100, 20 }, { 272, 224, 100, 20 } };

// position for delete button, relative to read quest window
const struct window_pos_t deletePos[MAX_RESOLUTIONS]= 
{ { 290, 140, 70, 20 }, { 362, 175, 70, 20 }, { 464, 224, 70, 20 } };

// position for exit button, relative to read quest window
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
{ { 380, 140, 70, 20 }, { 475, 175, 70, 20 }, { 608, 224, 70, 20 } };

// Constructor
cReadQuest::cReadQuest(void) 
{
	WNDCLASS wc;

	active = FALSE;

	quest = NULL;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = ReadQuestWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("ReadQuest");

    RegisterClass( &wc );

    hwnd_readquest = CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("ReadQuest"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		readQuestPos[cDD->Res()].x, readQuestPos[cDD->Res()].y, 		
		readQuestPos[cDD->Res()].width, readQuestPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_readquest, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_spheretext = CreateWindow(_T("static"), _T("Suggested sphere:"),
						WS_CHILD | SS_BITMAP,
						spheretextPos[cDD->Res()].x, spheretextPos[cDD->Res()].y, 
						spheretextPos[cDD->Res()].width, spheretextPos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	hSphere = CreateWindowsBitmap(LyraBitmap::SUG_SPHERE_LABEL);
	SendMessage(hwnd_spheretext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hSphere));

	hwnd_sugsphere = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						sugspherePos[cDD->Res()].x, sugspherePos[cDD->Res()].y, 
						sugspherePos[cDD->Res()].width, sugspherePos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_sugsphere, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_stattext = CreateWindow(_T("static"), _T("Suggested focus:"),
						WS_CHILD | SS_BITMAP,
						stattextPos[cDD->Res()].x, stattextPos[cDD->Res()].y, 
						stattextPos[cDD->Res()].width, stattextPos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	hStat = CreateWindowsBitmap(LyraBitmap::SUG_STAT_LABEL);
	SendMessage(hwnd_stattext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hStat));

	hwnd_sugstat = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						sugstatPos[cDD->Res()].x, sugstatPos[cDD->Res()].y, 
						sugstatPos[cDD->Res()].width, sugstatPos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_sugstat, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_creatortext = CreateWindow(_T("static"), _T("Posted by:"),
						WS_CHILD | SS_BITMAP,
						creatortextPos[cDD->Res()].x, creatortextPos[cDD->Res()].y, 
						creatortextPos[cDD->Res()].width, creatortextPos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	hCreator = CreateWindowsBitmap(LyraBitmap::POSTED_BY_LABEL);
	SendMessage(hwnd_creatortext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hCreator));

	hwnd_creator = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						creatorPos[cDD->Res()].x, creatorPos[cDD->Res()].y, 
						creatorPos[cDD->Res()].width, creatorPos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_creator, WM_SETFONT, WPARAM(quests->Hfont()), 0);


	hwnd_text = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | ES_MULTILINE | ES_WANTRETURN | ES_LEFT | ES_READONLY,
						textPos[cDD->Res()].x, textPos[cDD->Res()].y, 
						textPos[cDD->Res()].width, textPos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_text, WM_SETFONT, WPARAM(quests->Hfont()), 0);
	lpfn_text = SubclassWindow(hwnd_text, ReadQuestTextWProc);
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

	hwnd_accept = CreateWindow(_T("button"), _T("Accept Quest"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						acceptPos[cDD->Res()].x, acceptPos[cDD->Res()].y, 
						acceptPos[cDD->Res()].width, acceptPos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL); 
	hAccept = CreateWindowsBitmap(LyraBitmap::ACCEPT_GOAL_BUTTON);
	SendMessage(hwnd_accept, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hAccept));
	SubclassGoalWindow(hwnd_accept);

	hwnd_complete = CreateWindow(_T("button"), _T("Mark Completed"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						completePos[cDD->Res()].x, completePos[cDD->Res()].y, 
						completePos[cDD->Res()].width, completePos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	hComplete = CreateWindowsBitmap(LyraBitmap::COMPLETE_BUTTON);
	SendMessage(hwnd_complete, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hComplete));
	SubclassGoalWindow(hwnd_complete);

//	hwnd_edit = CreateWindow(_T("button"), _T("Edit Quest"),
//						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
//						editPos[cDD->Res()].x, editPos[cDD->Res()].y, 
//						editPos[cDD->Res()].width, editPos[cDD->Res()].height,
//						hwnd_readquest,
//						NULL, hInstance, NULL);
//	hEdit = CreateWindowsBitmap(LyraBitmap::EDIT_BUTTON);
//	SendMessage(hwnd_edit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hEdit));
//	SubclassGoalWindow(hwnd_edit);

	hwnd_delete = CreateWindow(_T("button"), _T("Delete Quest"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						deletePos[cDD->Res()].x, deletePos[cDD->Res()].y, 
						deletePos[cDD->Res()].width, deletePos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	hDelete = CreateWindowsBitmap(LyraBitmap::REMOVE_GOAL_BUTTON);
	SendMessage(hwnd_delete, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hDelete));
	SubclassGoalWindow(hwnd_delete);
	
	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_readquest,
						NULL, hInstance, NULL);
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	return;

}


// it's ok to call this when active - just replace the current quest text
void cReadQuest::Activate(cGoal *new_quest)
{ 
	active = TRUE; 
	quest = new_quest;
	ShowWindow(hwnd_delete, SW_HIDE);
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

	ShowWindow(hwnd_readquest, SW_SHOWNORMAL); 

	return;
};


void cReadQuest::Deactivate(bool deactive_details)
{ 
	if (!active)
		return;
	active = FALSE; 
	ShowWindow(hwnd_readquest, SW_HIDE); 

	if (detailquest->Active() && deactive_details)
		detailquest->Deactivate();
	if ((goalbook->InGoalBook(quest->ID())) && goalbook->Active())
		SendMessage(goalbook->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goalbook->Hwnd()); 
	else if (quests->Active())
		SendMessage(quests->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) quests->Hwnd());
	else
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	quest = NULL;

	return;
}


// if text is available, display it; otherwise put loading message up
void cReadQuest::SetText()
{
	if (quest->Text())
	{
		// quest-is-in-goalbook feature restriction
		if (goalbook->InGoalBook(quest->ID()))
		{
			ShowWindow(hwnd_accept, SW_HIDE);
		}
		else
		{
			ShowWindow(hwnd_accept, SW_SHOWNORMAL);
		}

		//bool can_edit = false;
		bool can_complete = false;

#ifdef GAMEMASTER
//#ifndef _DEBUG
		can_complete = true; // can_edit = true;
//#endif
#endif
			// quests can be read directly from goalbook, so questposting "quests" 
			// object is not safe to use

		if    (_tcscmp(quest->UpperPosterName(), player->UpperName()) == 0)
			can_complete = true;


		//if (can_edit)
		//{
			//ShowWindow(hwnd_edit, SW_SHOWNORMAL);
			//ShowWindow(hwnd_complete, SW_SHOWNORMAL);
			//ShowWindow(hwnd_delete, SW_SHOWNORMAL);
			//ShowWindow(hwnd_accept, SW_HIDE);
		//}
		//else 
		if (can_complete)
		{
			ShowWindow(hwnd_complete, SW_SHOWNORMAL);
			ShowWindow(hwnd_delete, SW_SHOWNORMAL);
			ShowWindow(hwnd_accept, SW_HIDE);
		} 
		else
		{
			//ShowWindow(hwnd_edit, SW_HIDE);
			ShowWindow(hwnd_complete, SW_HIDE);
			ShowWindow(hwnd_delete, SW_HIDE);
		}

		if (quest->SugSphere() == Guild::SPHERE_ANY)
		{
			LoadString(hInstance, IDS_ANY_SPHERE, message, sizeof(message));
			Edit_SetText(hwnd_sugsphere, message);
		}
		else
		{
			LoadString(hInstance, IDS_SPHERE_I, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, quest->SugSphere());
			Edit_SetText(hwnd_sugsphere, message);
		}

		if (quest->SugStat() == Stats::NO_STAT)
		{
			LoadString(hInstance, IDS_ANY_STAT, message, sizeof(message));
			Edit_SetText(hwnd_sugstat, message);
		}
		else
		{
			TranslateValue(LyraItem::TRANSLATION_STAT, quest->SugStat());
			Edit_SetText(hwnd_sugstat, message);
		}
		Edit_SetText(hwnd_creator, quest->PosterName());
		Edit_SetText(hwnd_text, quest->Text());
	}
	else
	{

		gs->RequestGoalText(quest->ID()); 

		//ShowWindow(hwnd_edit, SW_HIDE);
		ShowWindow(hwnd_accept, SW_HIDE);
		ShowWindow(hwnd_complete, SW_HIDE);
		ShowWindow(hwnd_delete, SW_HIDE);

		Edit_SetText(hwnd_sugstat, _T(""));
		Edit_SetText(hwnd_sugsphere, _T(""));
		Edit_SetText(hwnd_creator, _T(""));
		LoadString(hInstance, IDS_RETRIEVE_QUEST_INFO, message, sizeof(message));
		Edit_SetText(hwnd_text, message);
	}
}


void cReadQuest::AcceptQuest(void)
{
	quests->AcceptQuest(quest);
	this->Deactivate();

	return;
}

void cReadQuest::DeleteQuest(void)
{
#ifndef GAMEMASTER
	if (_tcscmp(quest->UpperPosterName(), player->UpperName()) == 0) 
#endif
	{
		LoadString (hInstance, IDS_DELETEQUEST_WARN, message, sizeof(message));
		quests->QuestWarning(&DeleteQuestCallback);
	}

	return;
}

void DeleteQuestCallback(void *value)
{
	int deletequest = *((int*)value);
#ifndef GAMEMASTER	
	if (_tcscmp(readquest->quest->UpperPosterName(), player->UpperName()) == 0) 
#endif
	{
		if (deletequest == 1)
		{
			gs->DeleteGoal(readquest->quest->ID()); 
			readquest->Deactivate();
		}
		else
		{
			LoadString (hInstance, IDS_DELETEQUEST_ABORTED, message, sizeof(message));
			quests->QuestError();
		}
	}
	
	return;
}

void cReadQuest::CompleteQuest(void)
{
	gs->CompleteGoal(quest->ID()); //*****
	Deactivate();

	return;
}


//void cReadQuest::EditQuest(void)
//{
//	postquest->Activate(quest); //*****
//	Deactivate();
	//return;
//}


void cReadQuest::DeleteAcknowledged(realmid_t questid)
{
	LoadString (hInstance, IDS_EXPIREQUEST_ACK, message, sizeof(message));
	display->DisplayMessage(message);
}


void cReadQuest::DeleteError(void)
{
	LoadString (hInstance, IDS_EXPIREQUEST_ERROR, message, sizeof(message));
	display->DisplayMessage(message);
}


void cReadQuest::CompleteAcknowledged(void)
{
	LoadString (hInstance, IDS_COMPLETEQUEST_ACK, message, sizeof(message));
	display->DisplayMessage(message);
}


void cReadQuest::CompleteError(void)
{
	LoadString (hInstance, IDS_COMPLETEQUEST_ERROR, message, sizeof(message));
	display->DisplayMessage(message);
}


void cReadQuest::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_text, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_text, NULL, TRUE);
}


void cReadQuest::ScrollDown(int count)
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
cReadQuest::~cReadQuest(void)
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
//	if (hEdit!=NULL)
		//DeleteObject(hEdit);
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


// Window procedure for the read quest window
LRESULT WINAPI ReadQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;

	if (HBRUSH brush = SetControlColors(hwnd, message, wParam, lParam, true))
		return (LRESULT)brush; 

	switch(message)
	{
		case WM_PAINT:
			TileBackground(readquest->hwnd_readquest);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == readquest->hwnd_accept)
				readquest->AcceptQuest();
			else if ((HWND)lParam == readquest->hwnd_complete)
				readquest->CompleteQuest();
			else if ((HWND)lParam == readquest->hwnd_delete)
				readquest->DeleteQuest();
			else if ((HWND)lParam == readquest->hwnd_exit)
				readquest->Deactivate();
//			else if ((HWND)lParam == readquest->hwnd_edit)
//				readquest->EditQuest();
			break;

		case WM_KEYUP:
			if ((UINT)(wParam) == VK_ESCAPE)
			{
				readquest->Deactivate();
				return 0L;
			}
			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Subclassed window procedure for the rich edit control
LRESULT WINAPI ReadQuestTextWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
			SendMessage(readquest->hwnd_readquest, message,
				(WPARAM) wParam, (LPARAM) lParam);
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			SendMessage(readquest->hwnd_readquest, WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) readquest->hwnd_readquest);
			return (LRESULT) 0;
		case WM_COMMAND:
			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((HWND)lParam == readquest->hwnd_text_buttons[j])
				{
					if (j == DOWN)
						readquest->ScrollDown(1);
					else
						readquest->ScrollUp(1);
					break;
				}
			}
			SendMessage(readquest->hwnd_readquest, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) readquest->hwnd_readquest);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_TEXT_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == readquest->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, readquest->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == readquest->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, readquest->text_buttons_bitmaps[j][1]); 
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

void cReadQuest::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
