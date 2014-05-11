// Handles the quest detail screen

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include <winsock2.h>
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cDetailQuest.h"
#include "cReadQuest.h"
#include "cGoalBook.h"
#include "utils.h"
#include "cChat.h"
#include "cItem.h"
#include "Dialogs.h"
#include "resource.h"
#include "cEffects.h"
#include "interface.h"
#include "cActorList.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern cDDraw *cDD;
extern cGoalBook *goalbook;
extern cEffects *effects;
extern cChat *display;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cQuestBuilder *quests;
extern cDetailQuest *detailquest;
extern cReadQuest *readquest;
extern cActorList *actors;

//////////////////////////////////////////////////////////////////
// Constants

const int VISIBLE_LINES = 9;
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
{ {{ 434, 198, 13, 25 }, { 540, 237, 13, 25 }, { 704, 263, 13, 25 } },
			DDOWN, LyraBitmap::CP_DDOWNA },   // ddown button
{ {{ 434, 88, 13, 25 }, { 540, 127, 13, 25 }, { 704, 156, 13, 25 } },
			DUP, LyraBitmap::CP_DUPA },   // dup button
{ {{ 434, 183, 13, 15 }, { 540, 224, 13, 15 }, { 704, 248, 13, 15 } },
			DOWN, LyraBitmap::CP_DOWNA },     // down button
{ {{ 434, 113, 13, 15 }, { 540, 152, 13, 15 }, { 704, 181, 13, 15 } },
			UP, LyraBitmap::CP_UPA }   // up button
};

const RECT button_strip[MAX_RESOLUTIONS] = 
{ { 100, 0, 125, 165 }, { 125, 0, 156, 206 }, { 160, 0, 200, 264 } };

// position for read quest details window, relative to main
const struct window_pos_t DetailQuestPos[MAX_RESOLUTIONS]= 
{ { 0, 0, 480, 300 }, { 0, 0, 600, 375 }, { 0, 0, 768, 480 } };

// position for title text
const struct window_pos_t titlePos[MAX_RESOLUTIONS]=
{ { 0, 0, 350, 25 }, { 0, 0, 437, 31 }, { 0, 0, 560, 25 } };

// position for loading message
const struct window_pos_t loadingtextPos[MAX_RESOLUTIONS]= 
{ { 20, 30, 300, 20 }, { 25, 50, 375, 25 }, { 32, 64, 480, 32 } };

// position for status flags
const struct window_pos_t statusflagtextPos[MAX_RESOLUTIONS]= 
{ { 15, 35, 125, 20 }, { 18, 36, 156, 25 }, { 24, 47, 200, 32 } };

const struct window_pos_t statusflagPos[MAX_RESOLUTIONS]= 
{ { 135, 30, 130, 25 }, { 168, 30, 162, 25 }, { 216, 39, 208, 25 } };

// position for label for xp award of new quest
const struct window_pos_t questxptextPos[MAX_RESOLUTIONS]= 
{ { 285, 35, 70, 20 }, { 356, 36, 87, 25 }, { 456, 47, 112, 32 } };

// position for xp award of new quest
const struct window_pos_t questxpPos[MAX_RESOLUTIONS]= 
{ { 375, 30, 75, 25 }, { 468, 30, 93, 25 }, { 600, 39, 120, 25 } };

// position for expiration time
const struct window_pos_t expirationtextPos[MAX_RESOLUTIONS]= 
{ { 15, 65, 125, 20 }, { 18, 73, 156, 25 }, { 24, 95, 200, 32 } };

const struct window_pos_t expirationPos[MAX_RESOLUTIONS]= 
{ { 195, 60, 70, 25 }, { 243, 67, 87, 25 }, { 312, 87, 112, 25 } };

// position for max acceptances
const struct window_pos_t maxaccepttextPos[MAX_RESOLUTIONS]= 
{ { 15, 95, 125, 20 }, { 18, 111, 156, 25 }, { 24, 143, 200, 32 } };

const struct window_pos_t maxacceptPos[MAX_RESOLUTIONS]= 
{ { 195, 90, 70, 25 }, { 243, 105, 87, 25 }, { 312, 135, 112, 25 } };

// position for number of acceptees
const struct window_pos_t numaccepteetextPos[MAX_RESOLUTIONS]= 
{ { 15, 125, 125, 20 }, { 18, 148, 156, 25 }, { 24, 191, 200, 32 } };

const struct window_pos_t numaccepteePos[MAX_RESOLUTIONS]= 
{ { 195, 120, 70, 25 }, { 243, 142, 87, 25 }, { 312, 183, 112, 25 } };

// position for graphic 
const struct window_pos_t graphictextPos[MAX_RESOLUTIONS]= 
{ { 15, 155, 50, 20 }, { 18, 186, 62, 25 }, { 24, 239, 80, 32 } };

const struct window_pos_t graphicPos[MAX_RESOLUTIONS]= 
{ { 75, 150, 65, 25 }, { 93, 180, 81, 25 }, { 120, 236, 104, 25 } };

// position for actual graphic icon
const struct window_pos_t graphicIconPos[MAX_RESOLUTIONS]= 
{ { 145, 155, 16, 16 }, { 181, 186, 16, 16 }, { 232, 239, 16, 16 } };

// position for charges
const struct window_pos_t chargestextPos[MAX_RESOLUTIONS]= 
{ { 165, 155, 50, 20 }, { 206, 186, 62, 25 }, { 264, 239, 80, 32 } };

const struct window_pos_t chargesPos[MAX_RESOLUTIONS]= 
{ { 230, 150, 35, 25 }, { 287, 180, 43, 25 }, { 368, 236, 56, 25 } };

// position for colors 
const struct window_pos_t colorstextPos[MAX_RESOLUTIONS]= 
{ { 15, 185, 50, 20 }, { 18, 223, 62, 25 }, { 24, 287, 80, 32 } };

const struct window_pos_t color1Pos[MAX_RESOLUTIONS]= 
{ { 75, 180, 80, 25 }, { 93, 217, 100, 25 }, { 120, 284, 128, 25 } };

const struct window_pos_t color2Pos[MAX_RESOLUTIONS]= 
{ { 185, 180, 80, 25 }, { 231, 217, 100, 25 }, { 296, 284, 128, 25 } };

// position for item function
const struct window_pos_t itemtypetextPos[MAX_RESOLUTIONS]= 
{ { 15, 210, 125, 20 }, { 18, 255, 156, 25 }, { 24, 327, 200, 32 } };

const struct window_pos_t itemtypePos[MAX_RESOLUTIONS]= 
{ { 105, 210, 160, 25 }, { 131, 255, 200, 25 }, { 168, 327, 256, 25 } };


const struct window_pos_t fieldtextPos[NUM_QUEST_FIELDS][MAX_RESOLUTIONS]= 
{
{ { 0, 240, 80, 20 }, { 0, 300, 100, 20 }, { 0, 384, 128, 20 } },
{ { 0, 267, 80, 20 }, { 0, 333, 100, 20 }, { 0, 427, 128, 20 } },
{ { 185, 240, 85, 20 }, { 231, 300, 106, 20 }, { 296, 384, 136, 20 } }
};

const struct window_pos_t fieldPos[NUM_QUEST_FIELDS][MAX_RESOLUTIONS]= 
{
{ { 95, 240, 85, 25 }, { 118, 300, 106, 25 }, { 152, 384, 136, 25 } },
{ { 95, 267, 85, 25 }, { 118, 333, 106, 25 }, { 152, 427, 136, 25 } },
{ { 275, 240, 85, 25 }, { 343, 300, 106, 25 }, { 440, 384, 136, 25 } }
};

// position for keywords
const struct window_pos_t keywordstextPos[MAX_RESOLUTIONS]= 
{ { 15, 250, 80, 20 }, { 18, 322, 100, 25 }, { 24, 415, 128, 32 } };

const struct window_pos_t keywordsPos[MAX_RESOLUTIONS]= 
{ { 75, 245, 290, 25 }, { 93, 317, 360, 25 }, { 120, 410, 360, 25 } };

// position for acceptees list box
const struct window_pos_t accepteestextPos[MAX_RESOLUTIONS]= 
{ { 285, 65, 165, 20 }, { 400, 93, 156, 25 }, { 512, 120, 200, 32 } };

const struct window_pos_t accepteesPos[MAX_RESOLUTIONS]= 
{ { 285, 85, 165, 164 }, { 400, 124, 156, 164 }, { 512, 152, 208, 164 } };

// position for exit button
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
{ { 380, 250, 70, 20 }, { 475, 322, 70, 20 }, { 608, 415, 70, 20 } };

// Constructor
cDetailQuest::cDetailQuest(void) 
{
	WNDCLASS wc;

	active = FALSE;

	quest = NULL;
	dummy_item = NO_ITEM;
	completing = false;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = DetailQuestWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("DetailQuest");

    RegisterClass( &wc );

    hwnd_detailquest = CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("DetailQuest"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		DetailQuestPos[cDD->Res()].x, DetailQuestPos[cDD->Res()].y, 		
		DetailQuestPos[cDD->Res()].width, DetailQuestPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_detailquest, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_title = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						titlePos[cDD->Res()].x, titlePos[cDD->Res()].y, 
						titlePos[cDD->Res()].width, titlePos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);

	hwnd_loadingtext = CreateWindow(_T("static"), _T("Retrieving quest information..."),
						WS_CHILD | SS_BITMAP ,
						loadingtextPos[cDD->Res()].x, loadingtextPos[cDD->Res()].y, 
						loadingtextPos[cDD->Res()].width, loadingtextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hLoading = CreateWindowsBitmap(LyraBitmap::RET_INFO_LABEL);
	SendMessage(hwnd_loadingtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hLoading));

	hwnd_maxaccepttext = CreateWindow(_T("static"), _T("Max. Acceptees:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						maxaccepttextPos[cDD->Res()].x, maxaccepttextPos[cDD->Res()].y, 
						maxaccepttextPos[cDD->Res()].width, maxaccepttextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hMaxAccept = CreateWindowsBitmap(LyraBitmap::MAX_ACCEPTEES_LABEL);
	SendMessage(hwnd_maxaccepttext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hMaxAccept));

	hwnd_maxaccept = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						maxacceptPos[cDD->Res()].x, maxacceptPos[cDD->Res()].y, 
						maxacceptPos[cDD->Res()].width, maxacceptPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_maxaccept, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_expirationtext = CreateWindow(_T("static"), _T("Days to expiration:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						expirationtextPos[cDD->Res()].x, expirationtextPos[cDD->Res()].y, 
						expirationtextPos[cDD->Res()].width, expirationtextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hExpiration = CreateWindowsBitmap(LyraBitmap::EXPIRE_LABEL);
	SendMessage(hwnd_expirationtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExpiration));


	hwnd_expiration = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						expirationPos[cDD->Res()].x, expirationPos[cDD->Res()].y, 
						expirationPos[cDD->Res()].width, expirationPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_expiration, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_statusflagtext = CreateWindow(_T("static"), _T("Status flags:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						statusflagtextPos[cDD->Res()].x, statusflagtextPos[cDD->Res()].y, 
						statusflagtextPos[cDD->Res()].width, statusflagtextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hStatus = CreateWindowsBitmap(LyraBitmap::STATUS_LABEL);
	SendMessage(hwnd_statusflagtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hStatus));

	hwnd_statusflag = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						statusflagPos[cDD->Res()].x, statusflagPos[cDD->Res()].y, 
						statusflagPos[cDD->Res()].width, statusflagPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_statusflag, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_questxptext = CreateWindow(_T("static"), _T("XP Award:"),
						WS_CHILD | SS_BITMAP,
						questxptextPos[cDD->Res()].x, questxptextPos[cDD->Res()].y, 
						questxptextPos[cDD->Res()].width, questxptextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hQuestXP = CreateWindowsBitmap(LyraBitmap::QUEST_XP_AWARD_LABEL);
	SendMessage(hwnd_questxptext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hQuestXP));

	hwnd_questxp = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						questxpPos[cDD->Res()].x, questxpPos[cDD->Res()].y, 
						questxpPos[cDD->Res()].width, questxpPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_questxp, WM_SETFONT, WPARAM(quests->Hfont()), 0);


	hwnd_numaccepteetext = CreateWindow(_T("static"), _T("Number acceptees:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						numaccepteetextPos[cDD->Res()].x, numaccepteetextPos[cDD->Res()].y, 
						numaccepteetextPos[cDD->Res()].width, numaccepteetextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hNumAcceptee = CreateWindowsBitmap(LyraBitmap::NUM_ACCEPTEES_LABEL);
	SendMessage(hwnd_numaccepteetext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hNumAcceptee));

	hwnd_numacceptee = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						numaccepteePos[cDD->Res()].x, numaccepteePos[cDD->Res()].y, 
						numaccepteePos[cDD->Res()].width, numaccepteePos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_numacceptee, WM_SETFONT, WPARAM(quests->Hfont()), 0);


	hwnd_graphictext = CreateWindow(_T("static"), _T("Graphic:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						graphictextPos[cDD->Res()].x, graphictextPos[cDD->Res()].y, 
						graphictextPos[cDD->Res()].width, graphictextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hGraphic = CreateWindowsBitmap(LyraBitmap::GRAPHIC_LABEL);
	SendMessage(hwnd_graphictext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hGraphic));

	hwnd_graphic = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						graphicPos[cDD->Res()].x, graphicPos[cDD->Res()].y, 
						graphicPos[cDD->Res()].width, graphicPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_graphic, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_graphicicon = CreateWindow(_T("static"), _T("Graphic:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						graphicIconPos[cDD->Res()].x, graphicIconPos[cDD->Res()].y, 
						graphicIconPos[cDD->Res()].width, graphicIconPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);

	hwnd_chargestext = CreateWindow(_T("static"), _T("Number of charges:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						chargestextPos[cDD->Res()].x, chargestextPos[cDD->Res()].y, 
						chargestextPos[cDD->Res()].width, chargestextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hCharges = CreateWindowsBitmap(LyraBitmap::CHARGES_LABEL);
	SendMessage(hwnd_chargestext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hCharges));

	hwnd_charges = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						chargesPos[cDD->Res()].x, chargesPos[cDD->Res()].y, 
						chargesPos[cDD->Res()].width, chargesPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_charges, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_colorstext = CreateWindow(_T("static"), _T("Colors:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						colorstextPos[cDD->Res()].x, colorstextPos[cDD->Res()].y, 
						colorstextPos[cDD->Res()].width, colorstextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hColors = CreateWindowsBitmap(LyraBitmap::COLORS_LABEL);
	SendMessage(hwnd_colorstext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hColors));

	hwnd_color1 = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						color1Pos[cDD->Res()].x, color1Pos[cDD->Res()].y, 
						color1Pos[cDD->Res()].width, color1Pos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_color1, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	hwnd_color2 = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						color2Pos[cDD->Res()].x, color2Pos[cDD->Res()].y, 
						color2Pos[cDD->Res()].width, color2Pos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_color2, WM_SETFONT, WPARAM(quests->Hfont()), 0);


	hwnd_itemtypetext = CreateWindow(_T("static"), _T("Item function:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						itemtypetextPos[cDD->Res()].x, itemtypetextPos[cDD->Res()].y, 
						itemtypetextPos[cDD->Res()].width, itemtypetextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hItemType = CreateWindowsBitmap(LyraBitmap::FUNCTION_LABEL);
	SendMessage(hwnd_itemtypetext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hItemType));

	hwnd_itemtype = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						itemtypePos[cDD->Res()].x, itemtypePos[cDD->Res()].y, 
						itemtypePos[cDD->Res()].width, itemtypePos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_itemtype, WM_SETFONT, WPARAM(quests->Hfont()), 0);

	for (int i=0; i<NUM_QUEST_FIELDS; i++) 
	{
		hwnd_fieldtext[i] = CreateWindow(_T("static"), _T("Quest Item Field"),
							WS_CHILD | SS_RIGHT, // | SS_BITMAP,
							fieldtextPos[i][cDD->Res()].x, fieldtextPos[i][cDD->Res()].y, 
							fieldtextPos[i][cDD->Res()].width, fieldtextPos[i][cDD->Res()].height,
							hwnd_detailquest,
							NULL, hInstance, NULL);
		//hfield[i] = CreateWindowsBitmap(LyraBitmap::field[i]_LABEL);
		//SendMessage(hwnd_fieldtext[i], STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hfield[i]));


		hwnd_field[i] = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						fieldPos[i][cDD->Res()].x, fieldPos[i][cDD->Res()].y, 
						fieldPos[i][cDD->Res()].width, fieldPos[i][cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
		SendMessage(hwnd_field[i], WM_SETFONT, WPARAM(quests->Hfont()), 0);
	}

	hwnd_keywordstext = CreateWindow(_T("static"), _T("Keywords"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						keywordstextPos[cDD->Res()].x, keywordstextPos[cDD->Res()].y, 
						keywordstextPos[cDD->Res()].width, keywordstextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hKeywords = CreateWindowsBitmap(LyraBitmap::KEYWORDS_LABEL);
	SendMessage(hwnd_keywordstext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hKeywords));

	hwnd_keywords = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						keywordsPos[cDD->Res()].x, keywordsPos[cDD->Res()].y, 
						keywordsPos[cDD->Res()].width, keywordsPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_keywords, WM_SETFONT, WPARAM(quests->Hfont()), 0);


	hwnd_accepteestext = CreateWindow(_T("static"), _T("Acceptees:"),
						WS_CHILD | SS_BITMAP,
						accepteestextPos[cDD->Res()].x, accepteestextPos[cDD->Res()].y, 
						accepteestextPos[cDD->Res()].width, accepteestextPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	hAcceptee = CreateWindowsBitmap(LyraBitmap::ACCEPTEES_LABEL);
	SendMessage(hwnd_keywordstext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hKeywords));

	hwnd_acceptees = CreateWindow(_T("listbox"), _T("Acceptees"),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED, //| WS_VSCROLL,
						accepteesPos[cDD->Res()].x, accepteesPos[cDD->Res()].y, 
						accepteesPos[cDD->Res()].width, accepteesPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL);
	SendMessage(hwnd_acceptees, WM_SETFONT, WPARAM(quests->Hfont()), 0);
//	lpfn_text = SubclassWindow(hwnd_acceptees, AccepteeWProc);
//	SendMessage(hwnd_acceptees, WM_PASSPROC, 0, (LPARAM) lpfn_text ); 

	// NOTE -- these scroll buttons are different, they are subclassed to 
	// the detailquest window.  Otherwise they would scroll in the listbox...
	for (int i=0; i<NUM_QUEST_BUTTONS; i++)
	{
		hwnd_text_buttons[i] = CreateWindow(_T("button"), _T(""),
				WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
				text_buttons[i].position[cDD->Res()].x, text_buttons[i].position[cDD->Res()].y, 
				text_buttons[i].position[cDD->Res()].width, text_buttons[i].position[cDD->Res()].height,
				hwnd_detailquest, NULL, hInstance, NULL);
		text_buttons_bitmaps[i][0] = // a button
			CreateWindowsBitmap(text_buttons[i].bitmap_id);
		text_buttons_bitmaps[i][1] = // b button
			CreateWindowsBitmap(text_buttons[i].bitmap_id + 1);
	}

	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_detailquest,
						NULL, hInstance, NULL); 
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	return;

}


void cDetailQuest::Activate(cGoal *new_quest, bool completing_quest)
{ 
	active = TRUE; 
	quest = new_quest;
	completing = completing_quest;

	hTitle = CreateWindowsBitmap(LyraBitmap::QUEST_BUILDER);
	SendMessage(hwnd_title, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hTitle);

	ShowWindow(hwnd_title, SW_SHOWNORMAL);
	ShowWindow(hwnd_loadingtext, SW_SHOWNORMAL);
	ShowWindow(hwnd_maxaccepttext, SW_HIDE);
	ShowWindow(hwnd_maxaccept, SW_HIDE);
	ShowWindow(hwnd_expirationtext, SW_HIDE);
	ShowWindow(hwnd_expiration, SW_HIDE);
	ShowWindow(hwnd_statusflagtext, SW_HIDE);
	ShowWindow(hwnd_statusflag, SW_HIDE);
	ShowWindow(hwnd_numaccepteetext, SW_HIDE);
	ShowWindow(hwnd_numacceptee, SW_HIDE);
	ShowWindow(hwnd_keywordstext, SW_HIDE);
	ShowWindow(hwnd_keywords, SW_HIDE);
	ShowWindow(hwnd_chargestext, SW_HIDE);
	ShowWindow(hwnd_charges, SW_HIDE);
	ShowWindow(hwnd_graphictext, SW_HIDE);
	ShowWindow(hwnd_graphic, SW_HIDE);
	ShowWindow(hwnd_graphicicon, SW_HIDE);
	ShowWindow(hwnd_colorstext, SW_HIDE);
	ShowWindow(hwnd_color1, SW_HIDE);
	ShowWindow(hwnd_color2, SW_HIDE);
	ShowWindow(hwnd_itemtypetext, SW_HIDE);
	ShowWindow(hwnd_itemtype, SW_HIDE);
	for (int i=0; i<NUM_QUEST_FIELDS; i++)
	{
		ShowWindow(hwnd_fieldtext[i], SW_HIDE);
		ShowWindow(hwnd_field[i], SW_HIDE);
	}
	ShowWindow(hwnd_accepteestext, SW_HIDE);
	ShowWindow(hwnd_acceptees, SW_HIDE);
	ShowWindow(hwnd_questxptext, SW_SHOWNORMAL);
	ShowWindow(hwnd_questxp, SW_SHOWNORMAL);
	ShowWindow(hwnd_exit, SW_SHOWNORMAL);
	for (int i=0; i<NUM_QUEST_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);


	if (quest->HasDetails())
		this->SetDetails();
	else
		gs->RequestGoalDetails(quest->ID()); 

	ShowWindow(hwnd_detailquest, SW_SHOWNORMAL); 

	return;
};

void cDetailQuest::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_detailquest, SW_HIDE); 

	quest = NULL;
	ListBox_ResetContent(hwnd_acceptees);

	if (readquest->Active())
		readquest->Deactivate();

	if (hTitle!=NULL)
		DeleteObject(hTitle);

	if (dummy_item != NO_ITEM)
		dummy_item->SetMarkedForDeath(true);
		
	if (quests->Active())
		SendMessage(quests->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) quests->Hwnd());
	else 
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	return;
}

void cDetailQuest::QuestCompleted(lyra_id_t quest_id)
{

	LoadString(hInstance, IDS_QUEST_COMPLETED, message, sizeof(message));
	CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,  
		cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
	goalbook->RemoveGoal(quest_id);
	this->Deactivate();
	//if (quest_item)
	//	quest_item->Destroy();
}

void cDetailQuest::CompleteQuestError(void)
{
	LoadString(hInstance, IDS_QUEST_COMPLETE_ERROR, message, sizeof(message));
	quests->QuestError();
	return;
}


// if text is available, display it; otherwise put loading message up
void cDetailQuest::SetDetails()
{

	for (int i=0; i<NUM_QUEST_FIELDS; i++)
	{	// we always clear out the fields
		Edit_SetText(hwnd_field[i], _T(""));
		Edit_SetText(hwnd_fieldtext[i], _T(""));
	}

	if (quest->Charges() > 0)
		qtype = Quest::TALISMAN;
	else
		qtype = Quest::CODEX;
	

	if (quest->MaxAcceptances() > -1) // check if data has been loaded
	{
		// if we've accepted this quest, check to see if we've already satisfied the quest
		if (goalbook->InGoalBook(quest->ID())) 
		{
			// for codex quests, we need to ask the server for verification
			// for talisman quests, we can determine it from inventory
			if (completing && (qtype == Quest::CODEX))
			{
				gs->IsCodexQuestCompleted(quest->ID());
			}
			else if (qtype == Quest::TALISMAN)
			{
				bool completed = false;
				cItem* item = NULL;
				for (item = actors->IterateItems(INIT); item != NO_ITEM; item = actors->IterateItems(NEXT))
					if (item->Status() == ITEM_OWNED)
					{
						if (DoesTalismanSatisfyQuest(item, quest))
						{
							completed = true;
							break;
						}
					}
				actors->IterateItems(DONE);

				if (completed && item)
				{ // Quest completed successfully - verify with server!
					quest_item = item;
					gs->CompleteQuest(quest->ID());
				} 
				else if (completing) 
				{ // player trying to complete quest, but still unfinished
					completing = false;
					readquest->Deactivate(false);
					CompleteQuestError();
					readquest->Activate(quest);
				}
			}
		}


		SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
		ShowWindow(hwnd_loadingtext, SW_HIDE);
		ShowWindow(hwnd_maxaccepttext, SW_SHOWNORMAL);
		ShowWindow(hwnd_maxaccept, SW_SHOWNORMAL);
		ShowWindow(hwnd_expirationtext, SW_SHOWNORMAL);
		ShowWindow(hwnd_expiration, SW_SHOWNORMAL);
		ShowWindow(hwnd_statusflagtext, SW_SHOWNORMAL);
		ShowWindow(hwnd_statusflag, SW_SHOWNORMAL);
		ShowWindow(hwnd_numaccepteetext, SW_SHOWNORMAL);
		ShowWindow(hwnd_numacceptee, SW_SHOWNORMAL);

		int status = SW_HIDE;
		if (qtype == Quest::TALISMAN)
			status = SW_SHOWNORMAL;

		for (int i=0; i<NUM_QUEST_FIELDS; i++)
		{
			ShowWindow(hwnd_fieldtext[i], SW_HIDE);
			ShowWindow(hwnd_field[i], SW_HIDE);
		}
		
		// show some windows for talisman quests but not codex quests
		ShowWindow(hwnd_graphictext, status);
		ShowWindow(hwnd_graphic, status);
		ShowWindow(hwnd_graphicicon, status);
		ShowWindow(hwnd_colorstext, status);
		ShowWindow(hwnd_color1, status);
		ShowWindow(hwnd_color2, status);
		ShowWindow(hwnd_chargestext, status);
		ShowWindow(hwnd_charges, status);
		ShowWindow(hwnd_itemtypetext, status);
		ShowWindow(hwnd_itemtype, status);

		// show some windows for codex quests but not codex quests
		status = SW_HIDE;

		// only show keywords to the poster for codex quests
		ShowWindow(hwnd_keywordstext, SW_HIDE);
		ShowWindow(hwnd_keywords, SW_HIDE);

		if (qtype == Quest::CODEX)
			status = SW_SHOWNORMAL;

		ShowWindow(hwnd_accepteestext, SW_SHOWNORMAL);
		ShowWindow(hwnd_acceptees, SW_SHOWNORMAL);
		_stprintf(message, _T("%u"), quest->MaxAcceptances());
		Edit_SetText(hwnd_maxaccept, message);
		_stprintf(message, _T("%u"), quest->ExpirationTime());
		Edit_SetText(hwnd_expiration, message);
		_stprintf(message, _T("%u"), quest->QuestXP());
		Edit_SetText(hwnd_questxp, message);
		
		if (qtype == Quest::TALISMAN)
		{
			_stprintf(message, _T("%u"), quest->Charges());
			Edit_SetText(hwnd_charges, message);
			if (quest->Graphic() == LyraBitmap::NONE)
			 	LoadString (hInstance, IDS_ANY_GRAPHIC, message, sizeof(message));
			else
				_stprintf(message, TalismanName(quest->Graphic()));
			Edit_SetText(hwnd_graphic, message);
			Edit_SetText(hwnd_color1, ColorName(quest->Color1()));
			Edit_SetText(hwnd_color2, ColorName(quest->Color2()));

			if (quest->ItemType() == LyraItem::NO_FUNCTION)
			{
			 	LoadString (hInstance, IDS_ANY_FUNCTION, message, sizeof(message));
				Edit_SetText(hwnd_itemtype, message);
			}
			else
				Edit_SetText(hwnd_itemtype, LyraItem::FunctionName(quest->ItemType()));
			int fields[NUM_QUEST_FIELDS];
			// put values into an array to make it easier to deal with below in the loop
			fields[0] = quest->Field1();
			fields[1] = quest->Field2();
			fields[2] = quest->Field3();
			
			int index = 0;
			for (int i=0; i<LyraItem::FunctionEntries(quest->ItemType()); i++)
			{
				if (LyraItem::EntryTranslation(quest->ItemType(), i) != 0)
				{
					SetWindowText(hwnd_fieldtext[index], LyraItem::EntryName(quest->ItemType(), i));
					ShowWindow(hwnd_fieldtext[index], SW_SHOWNORMAL);
					ShowWindow(hwnd_field[index], SW_SHOWNORMAL);
					TranslateValue(LyraItem::EntryTranslation(quest->ItemType(), i), fields[index]);
					Edit_SetText(hwnd_field[index], message);
					index++;
				}
			}
			
			LmItem info;
			LmItemHdr header;
			header.Init(0, 0);
			header.SetGraphic(quest->Graphic());
			int color1 = quest->Color1();
			int color2 = quest->Color2();
			// give dummy item color 0 if it can be any color
			if (color1 == ANY_COLOR)
				color1 = 0;
			if (color2 == ANY_COLOR)
				color2 = 0;
			header.SetColor1(color1); header.SetColor2(color2);
			
			LoadString(hInstance, IDS_DUMMY_ITEM, temp_message, sizeof(temp_message));
			info.Init(header, temp_message, 0, 0, 0);
			info.SetCharges(255);
			
			dummy_item = CreateItem(0.0, 0.0, 0, info, 0, false, 999999999, _T(""), ITEM_DUMMY);

			if (quest->Graphic() != LyraBitmap::NONE) 
			{
				RenderActor(dummy_item,(PIXEL *)icon,ICON_WIDTH,ICON_HEIGHT);
				hGraphicIcon = CreateBitmap(ICON_WIDTH, ICON_HEIGHT, 1, BITS_PER_PIXEL, icon);
				SendMessage(hwnd_graphicicon, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hGraphicIcon));			
			}
		}
		else if (qtype == Quest::CODEX) 
		{
			Edit_SetText(hwnd_keywords, quest->Keywords());
		}
		
		switch (quest->StatusFlags()) {
			case Guild::GOAL_ACTIVE:
				LoadString(hInstance, IDS_ACTIVE, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			case Guild::GOAL_COMPLETED: 
				LoadString(hInstance, IDS_COMPLETED, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			case Guild::GOAL_MAX_ACCEPTED: 
				LoadString(hInstance, IDS_MAX_ACCEPTED, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			case Guild::GOAL_TIME_EXPIRED: 
				LoadString(hInstance, IDS_TIME_EXPIRED, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			default: 
				_stprintf(message, _T("0"), quest->StatusFlags());
				Edit_SetText(hwnd_statusflag, message);
				break;
		}

		_stprintf(message, _T("%u"), quest->NumAcceptees()-quest->NumCompletees());
		Edit_SetText(hwnd_numacceptee, message);

		ListBox_ResetContent(hwnd_acceptees);	

		// first put up names of normal acceptees
		int i=0;
		for (i = 0; i < (quest->NumAcceptees() - quest->NumCompletees()); i++)
			ListBox_AddString(hwnd_acceptees, quest->Acceptee(i));

		// now put up names of completees with a *
		// first put up names of normal acceptees
		TCHAR completee[Lyra::PLAYERNAME_MAX+1];

		for (i = (quest->NumAcceptees() - quest->NumCompletees()); 
				i < quest->NumAcceptees(); i++)
		{
			_stprintf(completee, _T("*%s"), quest->Acceptee(i));
			ListBox_AddString(hwnd_acceptees, completee);
		}

	}
	else
	{
		ShowWindow(hwnd_loadingtext, SW_SHOWNORMAL);
		ShowWindow(hwnd_maxaccepttext, SW_HIDE);
		ShowWindow(hwnd_maxaccept, SW_HIDE);
		ShowWindow(hwnd_expirationtext, SW_HIDE);
		ShowWindow(hwnd_expiration, SW_HIDE);
		ShowWindow(hwnd_statusflagtext, SW_HIDE);
		ShowWindow(hwnd_statusflag, SW_HIDE);
		ShowWindow(hwnd_numaccepteetext, SW_HIDE);
		ShowWindow(hwnd_numacceptee, SW_HIDE);
		ShowWindow(hwnd_keywordstext, SW_HIDE);
		ShowWindow(hwnd_keywords, SW_HIDE);
		ShowWindow(hwnd_accepteestext, SW_HIDE);
		ShowWindow(hwnd_acceptees, SW_HIDE);
		ShowWindow(hwnd_graphictext, SW_HIDE);
		ShowWindow(hwnd_graphic, SW_HIDE);
		ShowWindow(hwnd_graphicicon, SW_HIDE);
		ShowWindow(hwnd_colorstext, SW_HIDE);
		ShowWindow(hwnd_color1, SW_HIDE);
		ShowWindow(hwnd_color2, SW_HIDE);
		ShowWindow(hwnd_chargestext, SW_HIDE);
		ShowWindow(hwnd_charges, SW_HIDE);
		ShowWindow(hwnd_itemtypetext, SW_HIDE);
		ShowWindow(hwnd_itemtype, SW_HIDE);
		for (int i=0; i<NUM_QUEST_FIELDS; i++)
		{
			ShowWindow(hwnd_fieldtext[i], SW_HIDE);
			ShowWindow(hwnd_field[i], SW_HIDE);
		}
		Edit_SetText(hwnd_maxaccept, _T(""));
		Edit_SetText(hwnd_expiration, _T(""));
		Edit_SetText(hwnd_statusflag, _T(""));
		Edit_SetText(hwnd_numacceptee, _T(""));
		Edit_SetText(hwnd_questxp, _T(""));
		Edit_SetText(hwnd_graphic, _T(""));
		Edit_SetText(hwnd_charges, _T(""));
		Edit_SetText(hwnd_color1, _T(""));
		Edit_SetText(hwnd_color2, _T(""));
		Edit_SetText(hwnd_itemtype, _T(""));

		SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));

		ListBox_ResetContent(hwnd_acceptees);
	}
}

void cDetailQuest::ShowKeywords(void)
{
	ShowWindow(hwnd_keywordstext, SW_SHOWNORMAL);
	ShowWindow(hwnd_keywords, SW_SHOWNORMAL);
}

void cDetailQuest::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_acceptees, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_acceptees, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_acceptees, &(button_strip[cDD->Res()]), TRUE);
	for (int j=0; j<NUM_QUEST_BUTTONS; j++)
		InvalidateRect(hwnd_text_buttons[j], NULL, TRUE);
	return;
}


void cDetailQuest::ScrollDown(int count)
{
	int line_count, first_visible, curr_count = count;
	line_count = ListBox_GetCount(hwnd_acceptees);
	if (line_count <= VISIBLE_LINES)
		return; // no scrolling until necessary

	first_visible = SendMessage(hwnd_acceptees, LB_GETTOPINDEX, 0, 0); 

	while (curr_count && (line_count - first_visible > VISIBLE_LINES))
	{
		SendMessage(hwnd_acceptees, WM_VSCROLL, (WPARAM)SB_LINEDOWN, 0);
		curr_count--;
		first_visible++;
	}
	InvalidateRect(hwnd_acceptees, &(button_strip[cDD->Res()]), TRUE);
	for (int j=0; j<NUM_QUEST_BUTTONS; j++)
		InvalidateRect(hwnd_text_buttons[j], NULL, TRUE);
	return;
}


// Destructor
cDetailQuest::~cDetailQuest(void)
{
	if (hLoading!=NULL)
		DeleteObject(hLoading);
	if (hMaxAccept!=NULL)
		DeleteObject(hMaxAccept);
	if (hExpiration!=NULL)
		DeleteObject(hExpiration);
	if (hStatus!=NULL)
		DeleteObject(hStatus);
	if (hQuestXP!=NULL)
		DeleteObject(hQuestXP);
	if (hNumAcceptee!=NULL)
		DeleteObject(hNumAcceptee);
	if (hGraphic!=NULL)
		DeleteObject(hGraphic);
	if (hGraphicIcon!=NULL)
		DeleteObject(hGraphicIcon);
	if (hCharges!=NULL)
		DeleteObject(hCharges);
	if (hColors!=NULL)
		DeleteObject(hColors);
	if (hItemType!=NULL)
		DeleteObject(hItemType);
	//if (hVelocity!=NULL)
	//	DeleteObject(hVelocity);
	//if (hDamage!=NULL)
	//	DeleteObject(hDamage);
	//if (hItemType!=NULL)
		//DeleteObject(hEffect);

	if (hAcceptee!=NULL)
		DeleteObject(hAcceptee);
	if (hExit!=NULL)
		DeleteObject(hExit);
	for (int i=0; i<NUM_QUEST_BUTTONS; i++)
	{
		if (text_buttons_bitmaps[i][0])
			DeleteObject(text_buttons_bitmaps[i][0]);
		if (text_buttons_bitmaps[i][1])
			DeleteObject(text_buttons_bitmaps[i][1]);
	}
	return;
}


// Window procedure for the read quest window
LRESULT WINAPI DetailQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	int style;
	int j;
    LPDRAWITEMSTRUCT lpdis; 
	HDC dc;


	if (HBRUSH brush = SetControlColors(hwnd, message, wParam, lParam, true))
		return (LRESULT)brush; 

	switch(message)
	{
		case WM_PAINT:
				TileBackground(detailquest->hwnd_detailquest);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))	
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == detailquest->hwnd_exit)
				detailquest->Deactivate();
			else
			{
			for (j=0; j<NUM_QUEST_BUTTONS; j++)
			{
				if ((HWND)lParam == detailquest->hwnd_text_buttons[j])
				{
					int scroll_amount = 1;
					if ((j == DDOWN) || (j == DUP))
						scroll_amount = 5;
					if ((j == DDOWN) || (j == DOWN))
						detailquest->ScrollDown(scroll_amount);
					else
						detailquest->ScrollUp(scroll_amount);
					break;
				}
			}
			SendMessage(detailquest->hwnd_detailquest, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) detailquest->hwnd_detailquest);
			}

			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_QUEST_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == detailquest->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, detailquest->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == detailquest->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, detailquest->text_buttons_bitmaps[j][1]); 
			}

			//RECT src;  
			//PrepareSrcRect(&src, &lpdis->rcItem, NOSTRETCH);
			//StretchBlt(lpdis->hDC,lpdis->rcItem.left, lpdis->rcItem.top,  
              //  lpdis->rcItem.right - lpdis->rcItem.left, 
               /// lpdis->rcItem.bottom - lpdis->rcItem.top, 
              //  dc, 0, 0, 
			//	(src.right - src.left), (src.bottom - src.top),				
			//	SRCCOPY);
		
            BitBlt(lpdis->hDC,lpdis->rcItem.left, lpdis->rcItem.top,  
					lpdis->rcItem.right - lpdis->rcItem.left, 
					lpdis->rcItem.bottom - lpdis->rcItem.top, 
					dc, 0, 0, SRCCOPY); 
            DeleteDC(dc); 
			return TRUE; 

		case WM_KEYUP:
			if ((UINT)(wParam) == VK_ESCAPE)
			{
				detailquest->Deactivate();
				return 0L;
			}
			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 


// Check invariants

#ifdef CHECK_INVARIANTS

void cDetailQuest::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
