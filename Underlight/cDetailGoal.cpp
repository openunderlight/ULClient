// Handles the goal detail screen

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windowsx.h>
#include "cDDraw.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cGoalPosting.h"
#include "cDetailGoal.h"
#include "cReadGoal.h"
#include "utils.h"
#include "cChat.h"
#include "resource.h"
#include "cEffects.h"
#include "interface.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern cDDraw *cDD;
extern cEffects *effects;
extern cChat *display;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cGoalPosting *goals;
extern cDetailGoal *detailgoal;
extern cReadGoal *readgoal;

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

const button_t text_buttons[NUM_GOAL_BUTTONS] = 
{ 
{ {{ 434, 208, 13, 25 }, { 540, 237, 13, 25 }, { 704, 263, 13, 25 } },
			DDOWN, LyraBitmap::CP_DDOWNA },   // ddown button
{ {{ 434, 98, 13, 25 }, { 540, 127, 13, 25 }, { 704, 156, 13, 25 } },
			DUP, LyraBitmap::CP_DUPA },   // dup button
{ {{ 434, 193, 13, 15 }, { 540, 224, 13, 15 }, { 704, 248, 13, 15 } },
			DOWN, LyraBitmap::CP_DOWNA },     // down button
{ {{ 434, 123, 13, 15 }, { 540, 152, 13, 15 }, { 704, 181, 13, 15 } },
			UP, LyraBitmap::CP_UPA }   // up button
};

const RECT button_strip[MAX_RESOLUTIONS] = 
{ { 100, 0, 125, 165 }, { 125, 0, 156, 206 }, { 160, 0, 200, 264 } };

// position for read goal details window, relative to main
const struct window_pos_t DetailGoalPos[MAX_RESOLUTIONS]= 
{ {0, 0, 480, 300 }, { 0, 0, 600, 375 }, { 0, 0, 768, 480 } };

// position for title text
const struct window_pos_t titlePos[MAX_RESOLUTIONS]=
{ { 0, 0, 350, 25 }, { 0, 0, 437, 31 }, { 0, 0, 560, 40 } };

// position for house logo
const struct window_pos_t logoPos[MAX_RESOLUTIONS]=
{ { 400, 0, 80, 80 }, { 500, 0, 100, 100 }, { 640, 0, 128, 128 } };

// position for loading message
const struct window_pos_t loadingtextPos[MAX_RESOLUTIONS]= 
{ { 20, 40, 300, 20 }, { 25, 50, 375, 25 }, { 32, 64, 480, 32 } };

// position for status flags
const struct window_pos_t statusflagtextPos[MAX_RESOLUTIONS]= 
{ { 15, 45, 125, 20 }, { 18, 56, 156, 25 }, { 24, 72, 200, 32 } };

const struct window_pos_t statusflagPos[MAX_RESOLUTIONS]= 
{ { 135, 40, 130, 25 }, { 168, 53, 130, 25 }, { 216, 70, 130, 25 } };

// position for expiration time
const struct window_pos_t expirationtextPos[MAX_RESOLUTIONS]= 
{ { 15, 75, 125, 20 }, { 18, 93, 156, 25 }, { 24, 120, 200, 32 } };


const struct window_pos_t expirationPos[MAX_RESOLUTIONS]= 
{ { 195, 70, 70, 25 }, { 228, 90, 70, 25 }, { 276, 118, 70, 25 } };

// position for max acceptances
const struct window_pos_t maxaccepttextPos[MAX_RESOLUTIONS]= 
{ { 15, 105, 125, 20 }, { 18, 131, 156, 25 }, { 24, 168, 200, 32 } };

const struct window_pos_t maxacceptPos[MAX_RESOLUTIONS]= 
{ { 195, 100, 70, 25 }, { 228, 128, 70, 25 }, { 276, 166, 70, 25 } };

// position for number of acceptees
const struct window_pos_t numaccepteetextPos[MAX_RESOLUTIONS]= 
{ { 15, 135, 125, 20 }, { 18, 168, 156, 25 }, { 24, 216, 200, 32 } };


const struct window_pos_t numaccepteePos[MAX_RESOLUTIONS]= 
{ { 195, 130, 70, 25 }, { 228, 165, 70, 25 }, { 276, 214, 70, 25 } };


// position for guardian checkbox, relative to goal posting window
const struct window_pos_t guardPos[MAX_RESOLUTIONS]= 
//{ { 15, 165, 195, 14 }, { 18, 206, 243, 17 }, { 24, 264, 312, 22 } };
{ { 15, 165, 195, 14 }, { 18, 206, 195, 14 }, { 24, 264, 195, 14 } };

// position for vote expiration time
const struct window_pos_t voteexpirationtextPos[MAX_RESOLUTIONS]= 
{ { 15, 165, 125, 20 }, { 18, 206, 156, 25 }, { 24, 264, 200, 32 } };

const struct window_pos_t voteexpirationPos[MAX_RESOLUTIONS]= 
{ { 195, 160, 70, 25 }, { 243, 200, 87, 31 }, { 312, 256, 112, 40 } };

// position for yes votes
const struct window_pos_t yesvotetextPos[MAX_RESOLUTIONS]= 
{ { 15, 195, 125, 20 }, { 18, 243, 156, 25 }, { 24, 312, 200, 32 } };

const struct window_pos_t yesvotePos[MAX_RESOLUTIONS]= 
{ { 195, 190, 70, 25 }, { 243, 237, 87, 31 }, { 312, 304, 112, 40 } };

// position for no votes
const struct window_pos_t novotetextPos[MAX_RESOLUTIONS]= 
{ { 15, 225, 125, 20 }, { 18, 281, 156, 25 }, { 24, 360, 200, 32 } };

const struct window_pos_t novotePos[MAX_RESOLUTIONS]= 
{ { 195, 220, 70, 25 }, { 243, 275, 87, 31 }, { 312, 352, 112, 40 } };

// position for acceptees list box
const struct window_pos_t accepteestextPos[MAX_RESOLUTIONS]= 
{ { 320, 75, 125, 20 }, { 400, 93, 156, 25 }, { 512, 120, 200, 32 } };

const struct window_pos_t accepteesPos[MAX_RESOLUTIONS]= 
{ { 285, 95, 165, 164 }, { 400, 124, 156, 164 }, { 512, 152, 208, 164 } };

// position for vote yes button
const struct window_pos_t yesbuttonPos[MAX_RESOLUTIONS]= 
//{ { 200, 260, 70, 20 }, { 250, 325, 87, 25 }, { 320, 416, 112, 32 } };
{ { 200, 260, 70, 20 }, { 250, 325, 70, 20 }, { 320, 416, 70, 20 } };

// position for vote no button
const struct window_pos_t nobuttonPos[MAX_RESOLUTIONS]= 
//{ { 290, 260, 70, 20 }, { 362, 325, 70, 20 }, { 464, 416, 112, 32 } };
{ { 290, 260, 70, 20 }, { 362, 325, 70, 20 }, { 464, 416, 70, 20 } };

// position for exit button
const struct window_pos_t exitPos[MAX_RESOLUTIONS]= 
//{ { 380, 260, 70, 20 }, { 475, 325, 70, 20 }, { 608, 416, 112, 32 } };
{ { 380, 260, 70, 20 }, { 475, 325, 70, 20 }, { 608, 416, 70, 20 } };

// Constructor
cDetailGoal::cDetailGoal(void) 
{
	WNDCLASS wc;

	active = FALSE;

	goal = NULL;

    // set up and register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc   = DetailGoalWProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = _T("DetailGoal");

    RegisterClass( &wc );

    hwnd_detailgoal = CreateWindowEx(
 	    WS_EX_CLIENTEDGE,
		_T("DetailGoal"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME, 
		DetailGoalPos[cDD->Res()].x, DetailGoalPos[cDD->Res()].y, 		
		DetailGoalPos[cDD->Res()].width, DetailGoalPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );
	SendMessage(hwnd_detailgoal, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_title = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						titlePos[cDD->Res()].x, titlePos[cDD->Res()].y, 
						titlePos[cDD->Res()].width, titlePos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);

	hwnd_logo = CreateWindow(_T("static"), _T(""), WS_CHILD | SS_BITMAP,
						logoPos[cDD->Res()].x, logoPos[cDD->Res()].y, 
						logoPos[cDD->Res()].width, logoPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);

	hwnd_loadingtext = CreateWindow(_T("static"), _T("Retrieving goal information..."),
						WS_CHILD | SS_BITMAP ,
						loadingtextPos[cDD->Res()].x, loadingtextPos[cDD->Res()].y, 
						loadingtextPos[cDD->Res()].width, loadingtextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hLoading = CreateWindowsBitmap(LyraBitmap::RET_INFO_LABEL);
	SendMessage(hwnd_loadingtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hLoading));

	hwnd_maxaccepttext = CreateWindow(_T("static"), _T("Max. Acceptees:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						maxaccepttextPos[cDD->Res()].x, maxaccepttextPos[cDD->Res()].y, 
						maxaccepttextPos[cDD->Res()].width, maxaccepttextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hMaxAccept = CreateWindowsBitmap(LyraBitmap::MAX_ACCEPTEES_LABEL);
	SendMessage(hwnd_maxaccepttext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hMaxAccept));

	hwnd_maxaccept = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						maxacceptPos[cDD->Res()].x, maxacceptPos[cDD->Res()].y, 
						maxacceptPos[cDD->Res()].width, maxacceptPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_maxaccept, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_expirationtext = CreateWindow(_T("static"), _T("Days to expiration:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						expirationtextPos[cDD->Res()].x, expirationtextPos[cDD->Res()].y, 
						expirationtextPos[cDD->Res()].width, expirationtextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hExpiration = CreateWindowsBitmap(LyraBitmap::EXPIRE_LABEL);
	SendMessage(hwnd_expirationtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExpiration));


	hwnd_expiration = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						expirationPos[cDD->Res()].x, expirationPos[cDD->Res()].y, 
						expirationPos[cDD->Res()].width, expirationPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_expiration, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_guard = CreateWindow(_T("button"), _T("All guardians manage"),
						WS_CHILD | WS_DISABLED | BS_AUTOCHECKBOX | BS_LEFTTEXT,
						guardPos[cDD->Res()].x, guardPos[cDD->Res()].y, 
						guardPos[cDD->Res()].width, guardPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL); 
	hGuard = CreateWindowsBitmap(LyraBitmap::GUARDIAN_LABEL);
	SetWindowLong(hwnd_guard, GWL_USERDATA, LONG(hGuard));
	SubclassGoalWindow(hwnd_guard);

	hwnd_voteexpirationtext = CreateWindow(_T("static"), _T("Vote days left:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						voteexpirationtextPos[cDD->Res()].x, voteexpirationtextPos[cDD->Res()].y, 
						voteexpirationtextPos[cDD->Res()].width, voteexpirationtextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hVoteExpiration = CreateWindowsBitmap(LyraBitmap::VOTE_EXPIRE_LABEL);
	SendMessage(hwnd_voteexpirationtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hVoteExpiration));

	hwnd_voteexpiration = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						voteexpirationPos[cDD->Res()].x, voteexpirationPos[cDD->Res()].y, 
						voteexpirationPos[cDD->Res()].width, voteexpirationPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_voteexpiration, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_yesvotetext = CreateWindow(_T("static"), _T("Votes for:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						yesvotetextPos[cDD->Res()].x, yesvotetextPos[cDD->Res()].y, 
						yesvotetextPos[cDD->Res()].width, yesvotetextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hYesVotes = CreateWindowsBitmap(LyraBitmap::YES_VOTES_LABEL);
	SendMessage(hwnd_yesvotetext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hYesVotes));

	hwnd_yesvote = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						yesvotePos[cDD->Res()].x, yesvotePos[cDD->Res()].y, 
						yesvotePos[cDD->Res()].width, yesvotePos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_yesvote, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_novotetext = CreateWindow(_T("static"), _T("Votes against:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						novotetextPos[cDD->Res()].x, novotetextPos[cDD->Res()].y, 
						novotetextPos[cDD->Res()].width, novotetextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hNoVotes = CreateWindowsBitmap(LyraBitmap::NO_VOTES_LABEL);
	SendMessage(hwnd_novotetext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hNoVotes));

	hwnd_novote = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						novotePos[cDD->Res()].x, novotePos[cDD->Res()].y, 
						novotePos[cDD->Res()].width, novotePos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_novote, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_statusflagtext = CreateWindow(_T("static"), _T("Status flags:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						statusflagtextPos[cDD->Res()].x, statusflagtextPos[cDD->Res()].y, 
						statusflagtextPos[cDD->Res()].width, statusflagtextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hStatus = CreateWindowsBitmap(LyraBitmap::STATUS_LABEL);
	SendMessage(hwnd_statusflagtext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hStatus));

	hwnd_statusflag = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						statusflagPos[cDD->Res()].x, statusflagPos[cDD->Res()].y, 
						statusflagPos[cDD->Res()].width, statusflagPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_statusflag, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_numaccepteetext = CreateWindow(_T("static"), _T("Number acceptees:"),
						WS_CHILD | SS_RIGHT | SS_BITMAP,
						numaccepteetextPos[cDD->Res()].x, numaccepteetextPos[cDD->Res()].y, 
						numaccepteetextPos[cDD->Res()].width, numaccepteetextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hNumAcceptee = CreateWindowsBitmap(LyraBitmap::NUM_ACCEPTEES_LABEL);
	SendMessage(hwnd_numaccepteetext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hNumAcceptee));

	hwnd_numacceptee = CreateWindow(_T("edit"), _T(""),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED | ES_LEFT | ES_READONLY,
						numaccepteePos[cDD->Res()].x, numaccepteePos[cDD->Res()].y, 
						numaccepteePos[cDD->Res()].width, numaccepteePos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_numacceptee, WM_SETFONT, WPARAM(goals->Hfont()), 0);

	hwnd_accepteestext = CreateWindow(_T("static"), _T("Acceptees"),
						WS_CHILD | SS_BITMAP,
						accepteestextPos[cDD->Res()].x, accepteestextPos[cDD->Res()].y, 
						accepteestextPos[cDD->Res()].width, accepteestextPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	hAcceptee = CreateWindowsBitmap(LyraBitmap::ACCEPTEES_LABEL);
	SendMessage(hwnd_accepteestext, STM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hAcceptee));

	hwnd_acceptees = CreateWindow(_T("listbox"), _T("Acceptees"),
						WS_CHILD | WS_DLGFRAME | WS_DISABLED, //| WS_VSCROLL,
						accepteesPos[cDD->Res()].x, accepteesPos[cDD->Res()].y, 
						accepteesPos[cDD->Res()].width, accepteesPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL);
	SendMessage(hwnd_acceptees, WM_SETFONT, WPARAM(goals->Hfont()), 0);
//	lpfn_text = SubclassWindow(hwnd_acceptees, AccepteeWProc);
//	SendMessage(hwnd_acceptees, WM_PASSPROC, 0, (LPARAM) lpfn_text ); 

	// NOTE -- these scroll buttons are different, they are subclassed to 
	// the detailgoal window.  Otherwise they would scroll in the listbox...
	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
	{
		hwnd_text_buttons[i] = CreateWindow(_T("button"), _T(""),
				WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
				text_buttons[i].position[cDD->Res()].x, text_buttons[i].position[cDD->Res()].y, 
				text_buttons[i].position[cDD->Res()].width, text_buttons[i].position[cDD->Res()].height,
				hwnd_detailgoal, NULL, hInstance, NULL);
		text_buttons_bitmaps[i][0] = // a button
			CreateWindowsBitmap(text_buttons[i].bitmap_id);
		text_buttons_bitmaps[i][1] = // b button
			CreateWindowsBitmap(text_buttons[i].bitmap_id + 1);
	}

	hwnd_yesbutton = CreateWindow(_T("button"), _T("Vote Yes"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						yesbuttonPos[cDD->Res()].x, yesbuttonPos[cDD->Res()].y, 
						yesbuttonPos[cDD->Res()].width, yesbuttonPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL); 
	hYesButton = CreateWindowsBitmap(LyraBitmap::VOTE_YES_BUTTON);
	SendMessage(hwnd_yesbutton, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hYesButton));
	SubclassGoalWindow(hwnd_yesbutton);

	hwnd_nobutton = CreateWindow(_T("button"), _T("Vote No"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						nobuttonPos[cDD->Res()].x, nobuttonPos[cDD->Res()].y, 
						nobuttonPos[cDD->Res()].width, nobuttonPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL); 
	hNoButton = CreateWindowsBitmap(LyraBitmap::VOTE_NO_BUTTON);
	SendMessage(hwnd_nobutton, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hNoButton));
	SubclassGoalWindow(hwnd_nobutton);

	hwnd_exit = CreateWindow(_T("button"), _T("Exit"),
						WS_CHILD | BS_PUSHBUTTON | BS_BITMAP,
						exitPos[cDD->Res()].x, exitPos[cDD->Res()].y, 
						exitPos[cDD->Res()].width, exitPos[cDD->Res()].height,
						hwnd_detailgoal,
						NULL, hInstance, NULL); 
	hExit = CreateWindowsBitmap(LyraBitmap::EXIT);
	SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
	SubclassGoalWindow(hwnd_exit);

	hAbstain = CreateWindowsBitmap(LyraBitmap::ABSTAIN_BUTTON);

	return;

}


void cDetailGoal::Activate(cGoal *new_goal)
{ 
	active = TRUE; 
	goal = new_goal;

	gs->RequestGoalDetails(goal->ID());

	hTitle = CreateWindowsBitmap(LyraBitmap::HOUSE_GOALBOARD_TITLES+goals->Guild()+((goals->Rank()-1)*NUM_GUILDS));
	SendMessage(hwnd_title, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hTitle);

	hLogo = CreateWindowsBitmap(LyraBitmap::HOUSE_BITMAPS+goals->Guild()+((goals->Rank()-1)*NUM_GUILDS));
	SendMessage(hwnd_logo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogo);

	ShowWindow(hwnd_title, SW_SHOWNORMAL);
	ShowWindow(hwnd_logo, SW_SHOWNORMAL);
	ShowWindow(hwnd_loadingtext, SW_SHOWNORMAL);
	ShowWindow(hwnd_maxaccepttext, SW_HIDE);
	ShowWindow(hwnd_maxaccept, SW_HIDE);
	ShowWindow(hwnd_expirationtext, SW_HIDE);
	ShowWindow(hwnd_expiration, SW_HIDE);
	ShowWindow(hwnd_voteexpirationtext, SW_HIDE);
	ShowWindow(hwnd_voteexpiration, SW_HIDE);
	ShowWindow(hwnd_yesvotetext, SW_HIDE);
	ShowWindow(hwnd_yesvote, SW_HIDE);
	ShowWindow(hwnd_novotetext, SW_HIDE);
	ShowWindow(hwnd_novote, SW_HIDE);
	ShowWindow(hwnd_statusflagtext, SW_HIDE);
	ShowWindow(hwnd_statusflag, SW_HIDE);
	ShowWindow(hwnd_numaccepteetext, SW_HIDE);
	ShowWindow(hwnd_numacceptee, SW_HIDE);
	ShowWindow(hwnd_accepteestext, SW_HIDE);
	ShowWindow(hwnd_acceptees, SW_HIDE);
	ShowWindow(hwnd_exit, SW_SHOWNORMAL);
	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
		ShowWindow(hwnd_text_buttons[i], SW_SHOWNORMAL);

	ShowWindow(hwnd_guard, SW_HIDE);

	this->SetDetails();

	ShowWindow(hwnd_detailgoal, SW_SHOWNORMAL); 

	return;
};

void cDetailGoal::Deactivate(void)
{ 
	active = FALSE; 
	ShowWindow(hwnd_detailgoal, SW_HIDE); 

	goal = NULL;
	ListBox_ResetContent(hwnd_acceptees);

	if (readgoal->Active())
		readgoal->Deactivate();

	if (hTitle!=NULL)
		DeleteObject(hTitle);
	if (hLogo!=NULL)
		DeleteObject(hLogo);

	ShowWindow(hwnd_yesbutton, SW_HIDE);
	ShowWindow(hwnd_nobutton, SW_HIDE);

	if (goals->Active())
		SendMessage(goals->Hwnd(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) goals->Hwnd());
	else 
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	return;
}


// if text is available, display it; otherwise put loading message up
void cDetailGoal::SetDetails()
{
	if (goal->MaxAcceptances() > -1) // check if data has been loaded
	{
		if (goal->Rank() > Guild::INITIATE)
		{
			ShowWindow(hwnd_guard, SW_HIDE);
			ShowWindow(hwnd_voteexpirationtext, SW_SHOWNORMAL);
			ShowWindow(hwnd_voteexpiration, SW_SHOWNORMAL);
			ShowWindow(hwnd_yesvotetext, SW_SHOWNORMAL);
			ShowWindow(hwnd_yesvote, SW_SHOWNORMAL);
			ShowWindow(hwnd_novotetext, SW_SHOWNORMAL);
			ShowWindow(hwnd_novote, SW_SHOWNORMAL);
			if (((goal->StatusFlags() == Guild::GOAL_PENDING_VOTE) ||
				(goal->StatusFlags() == Guild::GOAL_RULER_VOTE)) &&
				!(goal->PlayerOption()) && !(goal->Voted()))
			{
				ShowWindow(hwnd_yesbutton, SW_SHOWNORMAL);
				ShowWindow(hwnd_nobutton, SW_SHOWNORMAL);
				SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hAbstain));
			}
			else
			{
				ShowWindow(hwnd_yesbutton, SW_HIDE);
				ShowWindow(hwnd_nobutton, SW_HIDE);
				SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));
			}

		_stprintf(message, _T("%i"), goal->VoteExpiration());
			Edit_SetText(hwnd_voteexpiration, message);
		_stprintf(message, _T("%i"), goal->YesVotes());
			Edit_SetText(hwnd_yesvote, message);
		_stprintf(message, _T("%i"), goal->NoVotes());
			Edit_SetText(hwnd_novote, message);

			Button_SetCheck(hwnd_guard, 0);
		}
		else
		{
			SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));

			ShowWindow(hwnd_yesbutton, SW_HIDE);
			ShowWindow(hwnd_nobutton, SW_HIDE);
			ShowWindow(hwnd_voteexpirationtext, SW_HIDE);
			ShowWindow(hwnd_voteexpiration, SW_HIDE);
			ShowWindow(hwnd_yesvotetext, SW_HIDE);
			ShowWindow(hwnd_yesvote, SW_HIDE);
			ShowWindow(hwnd_novotetext, SW_HIDE);
			ShowWindow(hwnd_novote, SW_HIDE);
			Edit_SetText(hwnd_voteexpiration, _T(""));	
			Edit_SetText(hwnd_yesvote, _T(""));
			Edit_SetText(hwnd_novote, _T(""));
			SetWindowText(hwnd_exit, _T("Exit"));

			ShowWindow(hwnd_guard, SW_SHOWNORMAL);
			Button_SetCheck(hwnd_guard, (goal->Flags() & GOAL_GUARDIAN_MANAGE_FLAG));
		}

		ShowWindow(hwnd_loadingtext, SW_HIDE);
		ShowWindow(hwnd_maxaccepttext, SW_SHOWNORMAL);
		ShowWindow(hwnd_maxaccept, SW_SHOWNORMAL);
		ShowWindow(hwnd_expirationtext, SW_SHOWNORMAL);
		ShowWindow(hwnd_expiration, SW_SHOWNORMAL);
		ShowWindow(hwnd_statusflagtext, SW_SHOWNORMAL);
		ShowWindow(hwnd_statusflag, SW_SHOWNORMAL);
		ShowWindow(hwnd_numaccepteetext, SW_SHOWNORMAL);
		ShowWindow(hwnd_numacceptee, SW_SHOWNORMAL);
		ShowWindow(hwnd_accepteestext, SW_SHOWNORMAL);
		ShowWindow(hwnd_acceptees, SW_SHOWNORMAL);

	_stprintf(message, _T("%i"), goal->MaxAcceptances());
		Edit_SetText(hwnd_maxaccept, message);
	_stprintf(message, _T("%i"), goal->ExpirationTime());
		Edit_SetText(hwnd_expiration, message);

		switch (goal->StatusFlags()) {
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
			case Guild::GOAL_PENDING_VOTE: 
				LoadString(hInstance, IDS_VOTE_PENDING, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			case Guild::GOAL_VOTED_DOWN: 
				LoadString(hInstance, IDS_VOTED_DOWN, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			case Guild::GOAL_RULER_VOTE: 
				LoadString(hInstance, IDS_RULE_VOTE_PENDING, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			case Guild::GOAL_RULER_PASSED: 
				LoadString(hInstance, IDS_RULE_PASSED, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			case Guild::GOAL_RULER_FAILED: 
				LoadString(hInstance, IDS_RULE_FAILED, message, sizeof(message));
				Edit_SetText(hwnd_statusflag, message); break;
			default: 
			_stprintf(message, _T("%i"), goal->StatusFlags());
				Edit_SetText(hwnd_statusflag, message);
				break;
		}

	_stprintf(message, _T("%i"), goal->NumAcceptees());
		Edit_SetText(hwnd_numacceptee, message);

		ListBox_ResetContent(hwnd_acceptees);
		if (goal->StatusFlags() < Guild::GOAL_PENDING_VOTE)
			for (int i = 0; i < goal->NumAcceptees(); i++)
				ListBox_AddString(hwnd_acceptees, goal->Acceptee(i));
		else
			for (int i = 0; i < (goal->YesVotes() + goal->NoVotes()); i++)
				ListBox_AddString(hwnd_acceptees, goal->Acceptee(i));
	
	}
	else
	{
		ShowWindow(hwnd_loadingtext, SW_SHOWNORMAL);
		ShowWindow(hwnd_maxaccepttext, SW_HIDE);
		ShowWindow(hwnd_maxaccept, SW_HIDE);
		ShowWindow(hwnd_expirationtext, SW_HIDE);
		ShowWindow(hwnd_expiration, SW_HIDE);
		ShowWindow(hwnd_guard, SW_HIDE);
		ShowWindow(hwnd_voteexpirationtext, SW_HIDE);
		ShowWindow(hwnd_voteexpiration, SW_HIDE);
		ShowWindow(hwnd_yesvotetext, SW_HIDE);
		ShowWindow(hwnd_yesvote, SW_HIDE);
		ShowWindow(hwnd_novotetext, SW_HIDE);
		ShowWindow(hwnd_novote, SW_HIDE);
		ShowWindow(hwnd_statusflagtext, SW_HIDE);
		ShowWindow(hwnd_statusflag, SW_HIDE);
		ShowWindow(hwnd_numaccepteetext, SW_HIDE);
		ShowWindow(hwnd_numacceptee, SW_HIDE);
		ShowWindow(hwnd_accepteestext, SW_HIDE);
		ShowWindow(hwnd_acceptees, SW_HIDE);

		Edit_SetText(hwnd_maxaccept, _T(""));
		Edit_SetText(hwnd_expiration, _T(""));
		Edit_SetText(hwnd_statusflag, _T(""));
		Edit_SetText(hwnd_numacceptee, _T(""));
		Edit_SetText(hwnd_voteexpiration, _T(""));	
		Edit_SetText(hwnd_yesvote, _T(""));
		Edit_SetText(hwnd_novote, _T(""));

		SendMessage(hwnd_exit, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hExit));

		Button_SetCheck(hwnd_guard, 0);

		ListBox_ResetContent(hwnd_acceptees);
	}
}


void cDetailGoal::Vote(int vote)
{
	if ((goal->Rank() > Guild::INITIATE) &&
		((goal->StatusFlags() == Guild::GOAL_PENDING_VOTE) || 
		(goal->StatusFlags() == Guild::GOAL_RULER_VOTE)) &&
		(player->GuildRank(goal->GuildID()) > Guild::KNIGHT))
		gs->VoteGoal(goal->ID(), vote);

	goal->SetVoted(true);

	this->Deactivate();

	return;
}


void cDetailGoal::VoteAcknowledged(realmid_t goalid)
{
	LoadString (hInstance, IDS_VOTE_ACK, message, sizeof(disp_message));
	display->DisplayMessage(message);

	return;
}


void cDetailGoal::VoteError(void)
{
	LoadString (hInstance, IDS_VOTE_ERROR, message, sizeof(disp_message));
	display->DisplayMessage(message);

	return;
}


void cDetailGoal::ScrollUp(int count)
{
	if (count == 1)
		SendMessage(hwnd_acceptees, WM_VSCROLL, (WPARAM)SB_LINEUP, 0);
	else
		SendMessage(hwnd_acceptees, WM_VSCROLL, (WPARAM)SB_PAGEUP, 0);
	InvalidateRect(hwnd_acceptees, &(button_strip[cDD->Res()]), TRUE);
	for (int j=0; j<NUM_GOAL_BUTTONS; j++)
		InvalidateRect(hwnd_text_buttons[j], NULL, TRUE);
	return;
}


void cDetailGoal::ScrollDown(int count)
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
	for (int j=0; j<NUM_GOAL_BUTTONS; j++)
		InvalidateRect(hwnd_text_buttons[j], NULL, TRUE);
	return;
}


// Destructor
cDetailGoal::~cDetailGoal(void)
{
	if (hLoading!=NULL)
		DeleteObject(hLoading);
	if (hMaxAccept!=NULL)
		DeleteObject(hMaxAccept);
	if (hExpiration!=NULL)
		DeleteObject(hExpiration);
	if (hYesVotes!=NULL)
		DeleteObject(hYesVotes);
	if (hNoVotes!=NULL)
		DeleteObject(hNoVotes);
	if (hGuard!=NULL)
		DeleteObject(hGuard);
	if (hVoteExpiration!=NULL)
		DeleteObject(hVoteExpiration);
	if (hStatus!=NULL)
		DeleteObject(hStatus);
	if (hNumAcceptee!=NULL)
		DeleteObject(hNumAcceptee);
	if (hAcceptee!=NULL)
		DeleteObject(hAcceptee);
	if (hYesButton!=NULL)
		DeleteObject(hYesButton);
	if (hNoButton!=NULL)
		DeleteObject(hNoButton);
	if (hExit!=NULL)
		DeleteObject(hExit);
	if (hAbstain!=NULL)
		DeleteObject(hAbstain);
	for (int i=0; i<NUM_GOAL_BUTTONS; i++)
	{
		if (text_buttons_bitmaps[i][0])
			DeleteObject(text_buttons_bitmaps[i][0]);
		if (text_buttons_bitmaps[i][1])
			DeleteObject(text_buttons_bitmaps[i][1]);
	}
	return;
}


// Window procedure for the read goal window
LRESULT WINAPI DetailGoalWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
			if (goals->Rank() < 2)
				TileBackground(detailgoal->hwnd_detailgoal);
			else
				TileBackground(detailgoal->hwnd_detailgoal, GOLD_BACKGROUND);
			break;

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			style =GetWindowStyle(HWND(wParam));
			if (((style & 0x000F) == BS_DEFPUSHBUTTON) ||
				((style & 0x000F) == BS_PUSHBUTTON))	
				return 0; 
			break;

		case WM_COMMAND:
			if ((HWND)lParam == detailgoal->hwnd_exit)
				detailgoal->Deactivate();
			else if ((HWND)lParam == detailgoal->hwnd_yesbutton)
				detailgoal->Vote(Guild::YES_VOTE);
			else if ((HWND)lParam == detailgoal->hwnd_nobutton)
				detailgoal->Vote(Guild::NO_VOTE);
			else
			{
			for (j=0; j<NUM_GOAL_BUTTONS; j++)
			{
				if ((HWND)lParam == detailgoal->hwnd_text_buttons[j])
				{
					int scroll_amount = 1;
					if ((j == DDOWN) || (j == DUP))
						scroll_amount = 5;
					if ((j == DDOWN) || (j == DOWN))
						detailgoal->ScrollDown(scroll_amount);
					else
						detailgoal->ScrollUp(scroll_amount);
					break;
				}
			}
			SendMessage(detailgoal->hwnd_detailgoal, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) detailgoal->hwnd_detailgoal);
			}

			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_GOAL_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == detailgoal->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, detailgoal->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == detailgoal->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, detailgoal->text_buttons_bitmaps[j][1]); 
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
				detailgoal->Deactivate();
				return 0L;
			}
			break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 

// Replaced by subclassing scroll buttons to main window, kept just in case
// Subclassed window procedure for the rich edit control
/*LRESULT WINAPI AccepteeWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
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
			SendMessage(detailgoal->hwnd_detailgoal, message,
				(WPARAM) wParam, (LPARAM) lParam);
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			SendMessage(detailgoal->hwnd_detailgoal, WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) detailgoal->hwnd_detailgoal);
			return (LRESULT) 0;
		case WM_COMMAND:
			for (j=0; j<NUM_GOAL_BUTTONS; j++)
			{
				if ((HWND)lParam == detailgoal->hwnd_text_buttons[j])
				{
					int scroll_amount = 1;
					if ((j == DDOWN) || (j == DUP))
						scroll_amount = 5;
					if ((j == DDOWN) || (j == DOWN))
						detailgoal->ScrollDown(scroll_amount);
					else
						detailgoal->ScrollUp(scroll_amount);
					break;
				}
			}
			SendMessage(detailgoal->hwnd_detailgoal, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) detailgoal->hwnd_detailgoal);
			break;

		case WM_DRAWITEM: 
			lpdis = (LPDRAWITEMSTRUCT) lParam; 
			dc = CreateCompatibleDC(lpdis->hDC); 

			for (j=0; j<NUM_GOAL_BUTTONS; j++)
			{
				if ((lpdis->hwndItem == detailgoal->hwnd_text_buttons[j]) && (lpdis->itemState & ODS_SELECTED)) 
					SelectObject(dc, detailgoal->text_buttons_bitmaps[j][0]); 
				else if ((lpdis->hwndItem == detailgoal->hwnd_text_buttons[j]) && !(lpdis->itemState & ODS_SELECTED)) 
				    SelectObject(dc, detailgoal->text_buttons_bitmaps[j][1]); 
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
*/

// Check invariants

#ifdef CHECK_INVARIANTS

void cDetailGoal::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
