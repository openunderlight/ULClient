// cControlPanel: The control panel class.

// Copyright Lyra LLC, 1996-7. All rights reserved. 

#define STRICT

#include "Central.h"
#include <stdio.h>
#include <windows.h>
#include <Commctrl.h>
#include <limits.h>
#include "cDDraw.h"
#include "cDSound.h"
#include "cChat.h"
#include "cActorList.h"
#include "cGameServer.h"
#include "cPlayer.h"
#include "cEffects.h"
#include "Dialogs.h"
#include "Options.h"
#include "Resource.h"
#include "cLevel.h"
#include "cParty.h"
#include "cArts.h"
#include "cControlPanel.h"
#include "Interface.h"
//#include "cBanner.h"
#include "Realm.h"
#include "Mouse.h"

////////////////////////////////////////////////////////////////
// External Global Variables

extern cDDraw *cDD;
extern cDSound *cDS;
extern cActorList *actors;
extern cPlayer *player;
extern HINSTANCE hInstance;
extern cControlPanel *cp; // needed for window proc
extern cGameServer *gs; 
extern cChat *display;
extern cEffects *effects;
extern cArts *arts;
extern bool ready; // ready to render...
extern bool exiting;
extern bool metadlg;
extern bool talkdlg;
extern bool avatardlg;
extern ppoint_t pp; // personality points use tracker


extern options_t options;
//extern cBanner *banner; 
extern mouse_look_t mouse_look;
extern mouse_move_t mouse_move;
extern HFONT display_font[MAX_RESOLUTIONS]; 
extern HFONT bold_font[MAX_RESOLUTIONS]; 

extern int MAX_LV_ITEMS;

//////////////////////////////////////////////////////////////////
// Constants

const int BULLET_HEIGHT[MAX_RESOLUTIONS] = { 16, 20, 24};
const int BULLET_WIDTH[MAX_RESOLUTIONS] = { 16, 20, 24};

const int AVATAR_DISPLAY_HEIGHT[MAX_RESOLUTIONS]= { 200, 250, 320};
const int AVATAR_DISPLAY_WIDTH[MAX_RESOLUTIONS]= { 120, 150, 192};
//const int AVATAR_DISPLAY_HEIGHT[MAX_RESOLUTIONS]= { 200, 200, 200};
//const int AVATAR_DISPLAY_WIDTH[MAX_RESOLUTIONS]= { 120, 120, 120};

const int DEFAULT_ILIST_SIZE= 128;
const int STAT_LENGTH = 16;
const int MAX_UNUSED_ICONS = 16;
const int DRAG_REGION = 16;

const unsigned int RED = 0x00000DD;
const unsigned int YELLOW = 0x0000DDDD;
const unsigned int GREEN = 0x0000FF00;

// to prepare for high res:
// 1) modify right most bracket to put space between number and bracket
//		(search/replace }; with  };)
// 2) change window_pos_t XPos to window_pos_t XPos[MAX_RESOLUTIONS]
// 3) change all references to Pos. to Pos[cDD->Res()].
// 3) transfer to grzzt, run converter, transfer back

// position for the control panel, relative to the main window
const struct window_pos_t cpPos[MAX_RESOLUTIONS] = 
{ { 480, 0, 160, 480 }, { 600, 0, 200, 600 }, { 768, 0, 256, 768 } };


// position for avatar
const struct window_pos_t avatarPos[MAX_RESOLUTIONS] = 
{	{ 500, 90, 120, 200 }, 
	{ 625, 112, 150, 250 }, 
	{ 800, 144, 192, 320 } };

// position for turn avatar left button
const struct window_pos_t leftPos[MAX_RESOLUTIONS] = 
{ { 12, 5, 68, 55 }, { 15, 6, 85, 69 }, { 19, 8, 109, 88 } };

// position for turn avatar right button
const struct window_pos_t rightPos[MAX_RESOLUTIONS] = 
{ { 80, 5, 68, 55 }, { 100, 6, 85, 69 }, { 128, 8, 109, 88 } };

// position for tab control, relative to main window
const struct window_pos_t tabPos[MAX_RESOLUTIONS] = 
{ { 480, 0, 160, 38 }, { 600, 0, 200, 47 }, { 768, 0, 256, 60 } };

// position for inventory counter, relative to tab control
const struct window_pos_t invcountPos[MAX_RESOLUTIONS] =
{ { 60, 20, 40, 12}, { 75, 30, 50, 16}, {96, 40, 64, 20 } };

// position for main control panel bitmap, relative to main window
const struct window_pos_t mainPos[MAX_RESOLUTIONS] = 
{ { 480, 38, 160, 442 }, { 600, 47, 200, 553 }, { 768, 60, 256, 708 } };

// position of listviews, relative to cp bitmap
const struct window_pos_t listviewPos[MAX_RESOLUTIONS] = 
{ { 10, 0, 138, 255 }, { 12, 0, 172, 320 }, { 16, 8, 220, 402 } };

// position for use button, relative to bottom
const struct window_pos_t usePos[MAX_RESOLUTIONS] = 
{ { 12, 258, 39, 26 }, { 15, 323, 48, 32 }, { 19, 413, 62, 41 } };

// position for give button, relative to bottom
const struct window_pos_t givePos[MAX_RESOLUTIONS] = 
{ { 51, 258, 50, 26 }, { 63, 323, 48, 32 }, { 81, 413, 80, 41 } };

// position for drop button, relative to bottom
const struct window_pos_t dropPos[MAX_RESOLUTIONS] = 
{ { 101, 258, 51, 26 }, { 126, 323, 63, 32 }, { 161, 413, 81, 41 } };

// position for use button, relative to bottom
const struct window_pos_t useppPos[MAX_RESOLUTIONS] = 
{ { 12, 394, 39, 26 }, { 15, 492, 48, 32 }, { 19, 630, 62, 41 } };


// position for use button, relative to bottom
const struct window_pos_t grantppPos[MAX_RESOLUTIONS] = 
{ { 101, 394, 51, 26 }, { 126, 492, 63, 32 }, { 161, 630, 81, 41 } };


// position for meta button, relative to bottom
const struct window_pos_t metaPos[MAX_RESOLUTIONS] = 
{ { 58, 394, 41, 41 }, { 72, 492, 51, 51 }, { 92, 630, 65, 65 } };


struct button_t {
	window_pos_t position[MAX_RESOLUTIONS];
	int			 button_id;
	int			 bitmap_id;
};

const button_t listview_buttons[NUM_LV_BUTTONS] = 
{
{ {{ 127, 231, 13, 25 }, { 160, 295, 13, 25 }, { 208, 376, 13, 25 } },
		DDOWN, LyraBitmap::CP_DDOWNA }, // double down button
{ {{ 127, 0, 13, 25 }, { 160, 0, 13, 25 }, { 208, 0, 13, 25 } },
		DUP, LyraBitmap::CP_DUPA },   // double up button
{ {{ 127, 216, 13, 15 }, { 160, 282, 13, 15 }, { 208, 361, 13, 15 } },
		DOWN, LyraBitmap::CP_DOWNA },   // down button
{ {{ 127, 25, 13, 15 }, { 160, 25, 13, 15 }, { 208, 25, 13, 15 } },
		UP, LyraBitmap::CP_UPA }     // up button
};


struct stats_t {
	window_pos_t position[MAX_RESOLUTIONS];
	RECT		 rect[MAX_RESOLUTIONS];
};


struct stats_t cp_stats[NUM_PLAYER_STATS] = 
{
	{ { { 97, 378, 40, 16}, { 121, 473, 50, 20 }, { 155, 605, 64, 25 } },
	{ { 15, 377, 160, 393 }, { 19, 472, 200, 491 }, { 25, 603, 256, 628 } },
	}, //ds

	{ { { 97, 315, 40, 16}, { 121, 395, 50, 20 }, { 155, 505, 64, 25 } },
	{ { 15, 313, 160, 329 }, { 19, 392, 200, 411 }, { 25, 503, 256, 526 } },
	},  // wil

	{ { { 97, 331, 40, 16}, { 121, 415, 50, 20 }, { 155, 530, 64, 25 } },
	{ { 15, 329, 160, 345 }, { 19, 412, 200, 431 }, { 25, 528, 256, 552 } },
	},  // insight

	{ { { 97, 347, 40, 16}, { 121, 435, 50, 20 }, { 155, 555, 64, 25 } },
	{ { 15, 345, 160, 361 }, { 19, 432, 200, 451 }, { 25, 553, 256, 577 } },
	}, // res

	{ {	{ 97, 363, 40, 16}, { 121, 454, 50, 20 }, { 155, 580, 64, 25 } },
	{ { 15, 361, 160, 377 }, { 19, 452, 200, 471 }, { 25, 578, 256, 603 } },
	} //lucid
};

// positions of static controls of player's stats

// position for orbit static control
const struct window_pos_t orbitPos[MAX_RESOLUTIONS] = 
{ { 97, 297, 40, 16 }, { 121, 374, 50, 20 }, { 155, 477, 64, 25 } };

/////////////////////////////////////////////////////////////////
// Class Defintion


// Constructor
cControlPanel::cControlPanel(void) 
{
   int i;
   num_items = num_neighbors = num_arts = 0;
   drag_item = selected_item =  NO_ITEM;
   num_icons = num_unused_icons = 0;
   curr_avatar_view = player->views/2;
   selected_neighbor = NO_ACTOR;
   selected_art = Arts::NONE;
   image_list = NULL;
   if (options.welcome_ai)
	   tab_mode = INVENTORY_TAB; // to help newlies
   else 
	   tab_mode = NEIGHBORS_TAB; // default 
   captured = last_select_by_click = giving = useing = false;
   first_paint = true;
   setCounter = false;
   WNDCLASS wc;
   WNDPROC	lpfn_inventory, lpfn_neighbors, lpfn_arts;


      // Create Image List
    image_list = ImageList_Create(ICON_WIDTH, ICON_HEIGHT,  	
					ILC_COLORDDB | ILC_MASK, DEFAULT_ILIST_SIZE, 0); 
	if (image_list == NULL)
	{	
		GAME_ERROR(IDS_NO_CREATE_IMG_LIST);
		return;
	}

	// Now create the decorative windows to display UI bitmaps
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpfnWndProc   = ControlPanelWProc;
    wc.lpszClassName = _T("CP_Tab");
    RegisterClass( &wc );

	// Create the tab control

	// window for tab control
    hwnd_tab = CreateWindow(_T("CP_Tab"), _T(""), WS_POPUP | WS_CHILD, 
		tabPos[cDD->Res()].x, tabPos[cDD->Res()].y, 		
		tabPos[cDD->Res()].width, tabPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL); 

	// bitmaps for tab control
	for (i = 0; i < NUM_TABS; i++)
		cp_tab_bitmap[i] = CreateWindowsBitmap(LyraBitmap::CP_TAB1 + i*MAX_RESOLUTIONS + cDD->Res());

	// create main control panel bitmap
    wc.lpszClassName = _T("CP_Window");
    RegisterClass( &wc );

    hwnd_cp = CreateWindow(_T("CP_Window"), _T(""), WS_POPUP | WS_CHILD, 
		mainPos[cDD->Res()].x, mainPos[cDD->Res()].y, mainPos[cDD->Res()].width, mainPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );

    cp_window_bitmap = CreateWindowsBitmap(LyraBitmap::CP_WINDOW + cDD->Res());

	for (i=0; i<NUM_LISTVIEWS; i++)
	{
		hwnd_listviews[i] = CreateWindowEx(0, WC_LISTVIEW, _T("Underlight Listview"),  WS_CHILD | WS_VISIBLE |
						 LVS_SMALLICON | LVS_NOLABELWRAP | LVS_ALIGNTOP | LVS_NOSCROLL |
						 LVS_SINGLESEL | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS, 
						 listviewPos[cDD->Res()].x, listviewPos[cDD->Res()].y, 
						 listviewPos[cDD->Res()].width, listviewPos[cDD->Res()].height,
						 hwnd_cp, NULL, hInstance, NULL); 
		ListView_SetBkColor(hwnd_listviews[i], BLACK);
		ListView_SetTextBkColor(hwnd_listviews[i], BLACK);
		ListView_SetTextColor(hwnd_listviews[i], ORANGE);
		ListView_SetImageList(hwnd_listviews[i], image_list, LVSIL_SMALL);
	}

	// subclass to hook into our cp window proc
	lpfn_inventory = SubclassWindow(hwnd_listviews[INVENTORY_TAB], ::ControlPanelWProc);
	SendMessage(hwnd_listviews[INVENTORY_TAB], WM_PASS_INV_PROC, 0, (LPARAM) lpfn_inventory);
	SendMessage(hwnd_listviews[INVENTORY_TAB], WM_PASS_INV_HWND, 0, (LPARAM) hwnd_listviews[INVENTORY_TAB]);
	lpfn_neighbors = SubclassWindow(hwnd_listviews[NEIGHBORS_TAB], ::ControlPanelWProc);
	SendMessage(hwnd_listviews[NEIGHBORS_TAB], WM_PASS_WHO_PROC, 0, (LPARAM) lpfn_neighbors);
	SendMessage(hwnd_listviews[NEIGHBORS_TAB], WM_PASS_WHO_HWND, 0, (LPARAM) hwnd_listviews[NEIGHBORS_TAB]);
	lpfn_arts = SubclassWindow(hwnd_listviews[ARTS_TAB], ::ControlPanelWProc);
	SendMessage(hwnd_listviews[ARTS_TAB], WM_PASS_ARTS_PROC, 0, (LPARAM) lpfn_arts);
	SendMessage(hwnd_listviews[ARTS_TAB], WM_PASS_ARTS_HWND, 0, (LPARAM) hwnd_listviews[ARTS_TAB]);

	// create main control panel bitmap
    wc.lpszClassName = _T("CP_Avatar");
    RegisterClass( &wc );

	// window for avatar 
    hwnd_avatar = CreateWindow(_T("CP_Avatar"), _T(""), WS_POPUP | WS_CHILD, 
		avatarPos[cDD->Res()].x, avatarPos[cDD->Res()].y, 		
		avatarPos[cDD->Res()].width, avatarPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL); 

	cp_avatar_bitmap = NULL;

	// create selected stat bullet

    cp_bullet_bitmap = 	CreateWindowsBitmap(LyraBitmap::CP_BULLET + cDD->Res());
	
	// create buttons for turning avatar left/right
	hwnd_left = CreateWindow(_T("button"), _T(""),
						WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
						leftPos[cDD->Res()].x, leftPos[cDD->Res()].y, 
						leftPos[cDD->Res()].width, leftPos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

    left_bitmap[0] = CreateWindowsBitmap(LyraBitmap::CP_LEFTA + cDD->Res());
    left_bitmap[1] = CreateWindowsBitmap(LyraBitmap::CP_LEFTB + cDD->Res());
		

	hwnd_right = CreateWindow(_T("button"), _T(""),
						WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
						rightPos[cDD->Res()].x, rightPos[cDD->Res()].y, 
						rightPos[cDD->Res()].width, rightPos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

    right_bitmap[0] = CreateWindowsBitmap( LyraBitmap::CP_RIGHTA + cDD->Res());
    right_bitmap[1] = CreateWindowsBitmap( LyraBitmap::CP_RIGHTB + cDD->Res());

	// create use item button
	hwnd_use = CreateWindow(_T("button"), _T(""),
						WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
						usePos[cDD->Res()].x, usePos[cDD->Res()].y, 
						usePos[cDD->Res()].width, usePos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

    use_bitmap[0] = CreateWindowsBitmap(LyraBitmap::CP_USEA + cDD->Res());
    use_bitmap[1] = CreateWindowsBitmap(LyraBitmap::CP_USEB + cDD->Res());
	

	// create drop item button
	hwnd_drop = CreateWindow(_T("button"), _T(""),
						WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
						dropPos[cDD->Res()].x, dropPos[cDD->Res()].y, 
						dropPos[cDD->Res()].width, dropPos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

    drop_bitmap[0] = CreateWindowsBitmap(LyraBitmap::CP_DROPA + cDD->Res());
    drop_bitmap[1] = CreateWindowsBitmap(LyraBitmap::CP_DROPB + cDD->Res());
		
	// create meta button 
	hwnd_meta = CreateWindow(_T("button"), _T(""),
						WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
						metaPos[cDD->Res()].x, metaPos[cDD->Res()].y, 
						metaPos[cDD->Res()].width, metaPos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

    meta_bitmap[0] = CreateWindowsBitmap(LyraBitmap::CP_METAA + cDD->Res());
    meta_bitmap[1] = CreateWindowsBitmap(LyraBitmap::CP_METAB + cDD->Res());

	// create use item button
	hwnd_usepp = CreateWindow(_T("button"), _T(""),
						WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
						useppPos[cDD->Res()].x, useppPos[cDD->Res()].y, 
						useppPos[cDD->Res()].width, useppPos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

    usepp_bitmap[0] = CreateWindowsBitmap(LyraBitmap::CP_USEPPA + cDD->Res());
    usepp_bitmap[1] = CreateWindowsBitmap(LyraBitmap::CP_USEPPB + cDD->Res());

	// create use item button
	hwnd_grantpp = CreateWindow(_T("button"), _T(""),
						WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
						grantppPos[cDD->Res()].x, grantppPos[cDD->Res()].y, 
						grantppPos[cDD->Res()].width, grantppPos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

    grantpp_bitmap[0] = CreateWindowsBitmap(LyraBitmap::CP_GRANTPPA + cDD->Res());
    grantpp_bitmap[1] = CreateWindowsBitmap(LyraBitmap::CP_GRANTPPB + cDD->Res());


	// create meta button 
	hwnd_give = CreateWindow(_T("button"), _T(""),
						WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
						givePos[cDD->Res()].x, givePos[cDD->Res()].y, 
						givePos[cDD->Res()].width, givePos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

    give_bitmap[0] = CreateWindowsBitmap(LyraBitmap::CP_GIVEA + cDD->Res());
    give_bitmap[1] = CreateWindowsBitmap(LyraBitmap::CP_GIVEB + cDD->Res());

	// create arts scrolling buttons 
	for (i=0; i<NUM_LISTVIEWS; i++)
	{
		for (int j=0; j<NUM_LV_BUTTONS; j++)
		{
			hwnd_listview_buttons[i][j] = CreateWindow(_T("button"), _T(""),
					WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW,
					listview_buttons[j].position[cDD->Res()].x, listview_buttons[j].position[cDD->Res()].y, 
					listview_buttons[j].position[cDD->Res()].width, listview_buttons[j].position[cDD->Res()].height,
					hwnd_listviews[i], 	NULL, hInstance, NULL);

			listview_buttons_bitmaps[i][j][0] = // a button
				CreateWindowsBitmap(listview_buttons[j].bitmap_id);

			listview_buttons_bitmaps[i][j][1] = // b button
				CreateWindowsBitmap(listview_buttons[j].bitmap_id + 1);
		}
	}

	// create static control for inventory count

	hwnd_invcounter = CreateWindowEx(WS_EX_TRANSPARENT,
		_T("static"), NULL, WS_CHILD | SS_CENTER,
		invcountPos[cDD->Res()].x, invcountPos[cDD->Res()].y,
		invcountPos[cDD->Res()].width, invcountPos[cDD->Res()].height,
		hwnd_tab,
		NULL, hInstance, NULL);
	SendMessage(hwnd_invcounter, WM_SETFONT, WPARAM(display_font[cDD->Res()]), 0);
	


    // create orbit static control
	hwnd_orbit = CreateWindowEx(WS_EX_TRANSPARENT,
						_T("static"), NULL,
						WS_CHILD ,
						orbitPos[cDD->Res()].x, orbitPos[cDD->Res()].y, 
						orbitPos[cDD->Res()].width, orbitPos[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);

	// create static controls for stats
	for (i=0; i<NUM_PLAYER_STATS; i++)
	{
		hwnd_stats[i] = CreateWindowEx(WS_EX_TRANSPARENT,
						_T("static"), NULL,	WS_CHILD ,
						cp_stats[i].position[cDD->Res()].x, cp_stats[i].position[cDD->Res()].y, 
						cp_stats[i].position[cDD->Res()].width, cp_stats[i].position[cDD->Res()].height,
						hwnd_cp,
						NULL, hInstance, NULL);
		SendMessage(hwnd_stats[i], WM_SETFONT, WPARAM(bold_font[cDD->Res()]), 0);
	}

	SendMessage(hwnd_orbit, WM_SETFONT, WPARAM(bold_font[cDD->Res()]), 0);
	SendMessage(hwnd_cp, WM_SETFONT, WPARAM(display_font[cDD->Res()]), 0);
	for (i=0; i<NUM_LISTVIEWS; i++)
		SendMessage(hwnd_listviews[i], WM_SETFONT, WPARAM(display_font[cDD->Res()]), 0);

	// set up icons for arts
	for (i=0; i<NUM_PLAYER_STATS; i++)
	{
		effects->LoadEffectBitmaps(LyraBitmap::ART_ICONS + i, 5);
		this->AddIcon(effects->EffectBitmap(LyraBitmap::ART_ICONS + i)->address);
		effects->FreeEffectBitmaps(LyraBitmap::ART_ICONS + i);
	}

	return;

}

void cControlPanel::AddAvatar(void)
{
	unsigned char* avatar_bits = new unsigned char [AVATAR_DISPLAY_WIDTH[cDD->Res()]*AVATAR_DISPLAY_HEIGHT[cDD->Res()]*BYTES_PER_PIXEL];

	if (cp_avatar_bitmap)
		DeleteObject(cp_avatar_bitmap);

	RenderActor(player,(PIXEL *)avatar_bits, AVATAR_DISPLAY_WIDTH[cDD->Res()],AVATAR_DISPLAY_HEIGHT[cDD->Res()]);

    cp_avatar_bitmap = effects->Create16bppBitmapFromBits(avatar_bits, AVATAR_DISPLAY_WIDTH[cDD->Res()], AVATAR_DISPLAY_HEIGHT[cDD->Res()]);
		//CreateBitmap(AVATAR_DISPLAY_WIDTH[cDD->Res()], AVATAR_DISPLAY_HEIGHT[cDD->Res()], 
						//	1, BITS_PER_PIXEL, avatar_bits);

	InvalidateRect(hwnd_avatar, NULL, FALSE);

	try
	{ delete [] avatar_bits; }
	catch (char *str)
	{ }


    return;
}

// change avatar view by delta & range check appropriately
void cControlPanel::TurnAvatar(int delta)
{
	curr_avatar_view += delta;

	if (curr_avatar_view < 0)
		curr_avatar_view += player->views;
	else if (curr_avatar_view >= player->views)
		curr_avatar_view = 0;

	cDS->PlaySound(LyraSound::MENU_CLICK);

	this->AddAvatar();
	return;
}

// We want to handle the painting of all our bitmaps, but not the
// other components; this returns TRUE if we handled the paint of
// a window ourselves, or FALSE if not. 
BOOL cControlPanel::HandlePaint(HWND hwnd)
{
	RECT region;

	GetUpdateRect(hwnd, &region, FALSE);

	if ((region.left == 0) && (region.right == 0))
		return TRUE; // no update needed

	// now put up the appropriate bitmap
	if (hwnd == hwnd_tab)
	{
		this->BlitBitmap(hwnd, cp_tab_bitmap[tab_mode], &region);	
		return TRUE;
	} 
	else if (hwnd == hwnd_cp)
	{
		this->BlitBitmap(hwnd, cp_window_bitmap, &region);	
		return TRUE;
	}
	else if (hwnd == hwnd_avatar)
	{
		if (cp_avatar_bitmap)
			this->BlitBitmap(hwnd, cp_avatar_bitmap, &region);	
		return TRUE;
	}
	

	return FALSE;
}


void cControlPanel::BlitBitmap(HWND hwnd, HBITMAP bitmap, RECT *region)
{
	HGDIOBJ old_object;
	HDC dc;
	HDC bitmap_dc;
	PAINTSTRUCT paint;
	RECT *rect;

	dc = BeginPaint(hwnd, &paint);
	bitmap_dc = CreateCompatibleDC(dc);
	old_object = SelectObject(bitmap_dc, bitmap);

	BitBlt(dc, region->left, region->top, (region->right - region->left), 
		(region->bottom - region->top), bitmap_dc, 
		region->left, region->top, SRCCOPY);	


	if (hwnd == hwnd_cp)
	{ 	// put in the bullet if we're blitting the main portion
		SelectObject(bitmap_dc, cp_bullet_bitmap);
		rect = this->GetSelectedStatRect();

		BitBlt(dc, rect->left, rect->top, 
			BULLET_WIDTH[cDD->Res()], BULLET_HEIGHT[cDD->Res()],
			bitmap_dc, 0, 0, SRCCOPY);	


//		StretchBlt(dc, rect->left, rect->top, 
//			BULLET_WIDTH[cDD->Res()], BULLET_HEIGHT[cDD->Res()],
//			bitmap_dc, 0, 0, BULLET_WIDTH[0], BULLET_HEIGHT[0],	SRCCOPY);	
	}

	SelectObject(bitmap_dc, old_object);
	DeleteDC(bitmap_dc);
	EndPaint(hwnd, &paint);
 
	// this is a hack to align high-res drawing properly; otherwise
	// on redraws the imprecise calculations offset the stat labels a bit

	if (first_paint) 
	{
		for (int i=0; i< NUM_PLAYER_STATS; i++) 
			InvalidateRect(cp->hwnd_cp, &(cp_stats[i].rect[cDD->Res()]), FALSE);
		first_paint = false;
	}

	return;
}

int cControlPanel::Mode(void)
{
	return tab_mode;
}

// art cap is true when called by an art to set up for a click capture
void cControlPanel::SetMode(int new_mode, bool art_capture, bool force_redraw)				
{
	if ((tab_mode == new_mode) && (!force_redraw))
		return; // avoid unnecessary redraws
	else if ((new_mode == AVATAR_TAB) && (0 == cp_avatar_bitmap))
	{ // no avatar yet - must be in training
			LoadString (hInstance, IDS_NO_AVATAR, disp_message, sizeof(disp_message));
 			display->DisplayMessage(disp_message);
			return;
	}

	// cancel art if waiting for selection & switching away from the tab 
	if (!art_capture && arts->WaitingForSelection())
		arts->CancelArt();

	tab_mode = new_mode; 

	// turn scrolling buttons on/off

	for (int i=0; i<NUM_LISTVIEWS; i++)
	{
		if (i == new_mode)
			ShowWindow(hwnd_listviews[i], SW_SHOWNORMAL);
		else
			ShowWindow(hwnd_listviews[i], SW_HIDE);
		for (int j=0; j<NUM_LV_BUTTONS; j++)
			if (i == new_mode)
				ShowWindow(hwnd_listview_buttons[i][j], SW_SHOWNORMAL);
			else
				ShowWindow(hwnd_listview_buttons[i][j], SW_HIDE);
	}

	ShowWindow(hwnd_usepp, SW_SHOWNORMAL);
	ShowWindow(hwnd_grantpp, SW_SHOWNORMAL);

	//ShowWindow(hwnd_usepp, SW_HIDE);
	//ShowWindow(hwnd_grantpp, SW_HIDE);

	if (new_mode == AVATAR_TAB)
	{
		ShowWindow(hwnd_use, SW_HIDE);
		ShowWindow(hwnd_drop, SW_HIDE);
		ShowWindow(hwnd_give, SW_HIDE);
		ShowWindow(hwnd_invcounter, SW_HIDE);
		ShowWindow(hwnd_left, SW_SHOWNORMAL);
		ShowWindow(hwnd_right, SW_SHOWNORMAL);
		ShowWindow(hwnd_avatar, SW_SHOWNORMAL);
	} 
	else if (new_mode == INVENTORY_TAB)
	{
		ShowWindow(hwnd_use, SW_SHOWNORMAL);
		ShowWindow(hwnd_drop, SW_SHOWNORMAL);
		ShowWindow(hwnd_give, SW_SHOWNORMAL);
		ShowWindow(hwnd_invcounter, SW_SHOWNORMAL);
		ShowWindow(hwnd_left, SW_HIDE);
		ShowWindow(hwnd_right, SW_HIDE);
		ShowWindow(hwnd_avatar, SW_HIDE);
	}
	else if (new_mode == NEIGHBORS_TAB)
	{
		ShowWindow(hwnd_use, SW_HIDE);
		ShowWindow(hwnd_drop, SW_HIDE);
		ShowWindow(hwnd_give, SW_HIDE);
		ShowWindow(hwnd_invcounter, SW_HIDE);
		ShowWindow(hwnd_left, SW_HIDE);
		ShowWindow(hwnd_right, SW_HIDE);
		ShowWindow(hwnd_avatar, SW_HIDE);
	}
	else if (new_mode == ARTS_TAB)
	{
		ShowWindow(hwnd_use, SW_SHOWNORMAL);
		ShowWindow(hwnd_drop, SW_HIDE);
		ShowWindow(hwnd_give, SW_HIDE);
		ShowWindow(hwnd_invcounter, SW_HIDE);
		ShowWindow(hwnd_left, SW_HIDE);
		ShowWindow(hwnd_right, SW_HIDE);
		ShowWindow(hwnd_avatar, SW_HIDE);
	}

	InvalidateRect(cp->hwnd_tab, NULL, TRUE);

	return;
}

	
void cControlPanel::DumpInventory(void)
{
	cItem *item;

	LoadString (hInstance, IDS_DUMP_INVENTORY, disp_message, sizeof(disp_message));
 	display->DisplayMessage(disp_message);
	for (item = actors->IterateItems(INIT); item != NO_ITEM; item = actors->IterateItems(NEXT))
		if (item->Status() ==ITEM_OWNED)
		{		
			LoadString (hInstance, IDS_ITEM_INFO, disp_message, sizeof(disp_message));
		_stprintf(message,disp_message,item->Name(),LookUpItemIndex(item),item->SortIndex());
			display->DisplayMessage(message);
		}
	actors->IterateItems(DONE);
}
	

int cControlPanel::AddToListView(HWND listview, int new_index, 
					 int image_index, TCHAR *descrip, long data)
{
	int index;
	LV_ITEM stats;

    stats.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
    stats.iItem = new_index;
    stats.iSubItem = 0; 
    stats.pszText = descrip; // item text
    stats.iImage = image_index; // index of the list view item's icon 
    stats.lParam = data; // use item ptr as 32 bit user data

	index = ListView_InsertItem(listview, &stats);	
	return index;
}

int cControlPanel::AddItem(cItem* item)
{
	if (num_items == MAX_LV_ITEMS)
	{ // determine if we should replace an existing listview entry
		for (int i=0; i<num_items-1; i++)
			if ((((CompareItems((long)LookUpItem(i), (long)item, 0) == -1) &&
				 (CompareItems((long)LookUpItem(i+1), (long)item, 0) == 1))) ||
				(this->GetPrevItem(this->GetFirstItem()) == item))
			{
				DeleteItem(this->GetLastItem(), SHOW_NONE);
				break;
			}
		if (num_items == MAX_LV_ITEMS)
			return -1;
	}

	//_tprintf("adding item %s, si = %d\n",item->Name(), item->SortIndex());
	int index = this->AddToListView(hwnd_listviews[INVENTORY_TAB], num_items, this->AddIcon(item->IconBits()),
		item->Name(), (long)item);
	num_items++;
	if (item == selected_item) // readded selected item
		this->SetListViewSelected(hwnd_listviews[INVENTORY_TAB], -1, this->LookUpItemIndex(selected_item));
	SendMessage(hwnd_listviews[INVENTORY_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareItems);

	if (gs->LoggedIntoGame())
		player->SetItemNeedFlagsOrSortingUpdate(true);

	//ListView_RedrawItems(hwnd_listviews[INVENTORY_TAB], 0, num_items); 
	//this->DumpInventory();
	return index;
}


// returns the neighbor with the lowest sorting value (1st in list)
int cControlPanel::AddNeighbor(cNeighbor* n)
{
	// if the neighbor is hidden (GM super invis), don't display
	if (n->Avatar().Hidden())
		return -1;
	if (num_neighbors == MAX_LV_ITEMS)
	{ // determine if we should replace an existing listview entry
		for (int i=0; i<num_neighbors-1; i++)
			if ((((CompareNeighbors((long)LookUpNeighbor(i), (long)n, 0) == -1) &&
				 (CompareNeighbors((long)LookUpNeighbor(i+1), (long)n, 0) == 1))) ||
				(this->GetPrevNeighbor(this->GetFirstNeighbor()) == n))
			{
				DeleteNeighbor(this->GetLastNeighbor(), SHOW_NONE);
				break;
			}
		if (num_neighbors == MAX_LV_ITEMS)
			return -1;
	}
// Jared 6-05-00
// PMares cannot see the names of dreamers, but they can see the names of other mares
#ifdef PMARE
	int index;
	if ((n->GetAccountType() != LmAvatar::ACCT_DREAMER) || (n->GetAccountType() != LmAvatar::ACCT_ADMIN)) // Display nightmare names as usual
		index = this->AddToListView(hwnd_listviews[NEIGHBORS_TAB], num_neighbors, this->AddIcon(n->IconBits()),
			n->Name(), (long)n);
	else
		if (n->IsMale())
			index = this->AddToListView(hwnd_listviews[NEIGHBORS_TAB], num_neighbors, this->AddIcon(n->IconBits()),
				"Male Dreamer", (long)n);
		else if (n->IsFemale())
			index = this->AddToListView(hwnd_listviews[NEIGHBORS_TAB], num_neighbors, this->AddIcon(n->IconBits()),
				"Female Dreamer", (long)n);
		else // Players in nightmare form are neither male nor female
			index = this->AddToListView(hwnd_listviews[NEIGHBORS_TAB], num_neighbors, this->AddIcon(n->IconBits()),
				"Dreamer", (long)n);
#else
	int index = this->AddToListView(hwnd_listviews[NEIGHBORS_TAB], num_neighbors, this->AddIcon(n->IconBits()),
		n->Name(), (long)n);
#endif
	num_neighbors++;
	if (n == selected_neighbor) // readded selected neighbor
		this->SetListViewSelected(hwnd_listviews[NEIGHBORS_TAB], -1, this->LookUpNeighborIndex(selected_neighbor));
	SendMessage(hwnd_listviews[NEIGHBORS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareNeighbors);
	//ListView_RedrawItems(hwnd_listviews[NEIGHBORS_TAB], 0, num_neighbors); 
	return index;
}


int cControlPanel::AddArt(realmid_t art)
{	// text is used to display art & skill level
	if ((art < 0) || (art >= NUM_ARTS))
		return -1;

	if (num_arts == MAX_LV_ITEMS)
	{ // determine if we should replace an existing listview entry
		for (int i=0; i<num_arts-1; i++)
			if ((((CompareArts((long)LookUpArt(i), (long)art, 0) == -1) &&
				 (CompareArts((long)LookUpArt(i+1), (long)art, 0) == 1))) ||
				(this->GetPrevArt(this->GetFirstArt()) == art))
			{
				DeleteArt(this->GetLastArt(), SHOW_NONE);
				break;
			}
		if (num_arts == MAX_LV_ITEMS)
			return -1;
	}

	this->FillInArtString(art, message);
	int icon = arts->Stat(art);
	if (icon == Stats::NO_STAT)
		icon = Stats::DREAMSOUL;
	int index = this->AddToListView(hwnd_listviews[ARTS_TAB], num_arts, icon, message, (long)art);	
	num_arts++;
	if (art == selected_art) // readded selected art
		this->SetListViewSelected(hwnd_listviews[ARTS_TAB], -1, this->LookUpArtIndex(art));
	
	SendMessage(hwnd_listviews[ARTS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareArts);
	return index;
}


// returns the listview index of the given item, or -1 if
// it can not be found
int cControlPanel::LookUpItemIndex(cItem *item)
{
	LV_FINDINFO stats;

    stats.flags = LVFI_PARAM; stats.lParam = (LPARAM)item;

	return ListView_FindItem(hwnd_listviews[INVENTORY_TAB], -1, &stats);
}

cItem* cControlPanel::GetFirstItem(void)
{
	cItem *item, *first = NO_ITEM;
	for (int i=0; i<num_items; i++)
	{	
		item = this->LookUpItem(i);
		if ((first == NO_ITEM) || (CompareItems((long)item, (long)first, 0) == -1))
			first = item;
	}
	return first;
}

cItem* cControlPanel::GetLastItem(void)
{
	cItem *item, *last = NO_ITEM;
	for (int i=0; i<num_items; i++)
	{	
		item = this->LookUpItem(i);
		if ((last == NO_ITEM) || (CompareItems((long)item, (long)last, 0) == 1))
			last = item;
	}
	return last;
}

cNeighbor* cControlPanel::GetFirstNeighbor(void)
{
	cNeighbor *n, *first = NO_ACTOR;
	for (int i=0; i<num_neighbors; i++)
	{	
		n = this->LookUpNeighbor(i);
		if ((first == NO_ACTOR) || (CompareNeighbors((long)n, (long)first, 0) == -1))
			first = n;
	}
	return first;
}

cNeighbor* cControlPanel::GetLastNeighbor(void)
{
	cNeighbor *n, *last = NO_ACTOR;
	for (int i=0; i<num_neighbors; i++)
	{	
		n = this->LookUpNeighbor(i);
		if ((last == NO_ACTOR) || (CompareNeighbors((long)n, (long)last, 0) == 1))
			last = n;
	}
	return last;
}

lyra_id_t cControlPanel::GetFirstArt(void)
{
	lyra_id_t art, first = Arts::NONE;
	for (int i=0; i<num_arts; i++)
	{	
		art = this->LookUpArt(i);
		if ((first == Arts::NONE) || (CompareArts((long)art, (long)first, 0) == -1))
			first = art;
	}
	return first;
}

lyra_id_t cControlPanel::GetLastArt(void)
{
	lyra_id_t art, last = Arts::NONE;
	for (int i=0; i<num_arts; i++)
	{	
		art = this->LookUpArt(i);
		if ((last == Arts::NONE) || (CompareArts((long)art, (long)last, 0) == 1))
			last = art;
	}
	return last;
}


// if lv_ok is true, allow return of items currently in the listview
cItem* cControlPanel::GetNextItem(cItem *last, bool lv_ok)
{
	cItem *item, *next = NO_ITEM;

	// next item must be owned, not in the listview, and be sorted after the current last item
	for (item = actors->IterateItems(INIT); item != NO_ITEM; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (lv_ok || this->LookUpItemIndex(item) == -1) && 
			(CompareItems((long)item, (long)last, 0) == 1))
		{
			if ((next == NO_ITEM) ||
				(CompareItems((long)item, (long)next, 0) == -1))
				next = item;
		}
	actors->IterateItems(DONE);
	return next;
}

// finds the next item in inventory to add to the listview
void cControlPanel::AddNextItem(void)
{ 
	if ((num_items == 0) || (num_items == MAX_LV_ITEMS))
		return;

	cItem *next = this->GetNextItem(this->GetLastItem());
	if (next != NO_ITEM)
	{
		this->AddItem(next);
		SendMessage(hwnd_listviews[INVENTORY_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareItems);
	}
}

// find previous item
cItem* cControlPanel::GetPrevItem(cItem *first, bool lv_ok)
{
	cItem *item, *prev = NO_ITEM;
	// prev item must be owned, not in the listview, and be sorted after the current first item
	for (item = actors->IterateItems(INIT); item != NO_ITEM; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (lv_ok || this->LookUpItemIndex(item) == -1) && 
			(CompareItems((long)item, (long)first, 0) == -1))
		{
			if ((prev == NO_ITEM) ||
				(CompareItems((long)item, (long)prev, 0) == 1))
				prev = item;
		}
	actors->IterateItems(DONE);
	return prev;
}


void cControlPanel::AddPrevItem(void)
{ 
	if ((num_items == 0) || (num_items == MAX_LV_ITEMS))
		return;

	cItem *prev = this->GetPrevItem(this->GetFirstItem());
	if (prev != NO_ITEM)
	{
		this->AddItem(prev);
		SendMessage(hwnd_listviews[INVENTORY_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareItems);
	}
}

cNeighbor* cControlPanel::GetNextNeighbor(cNeighbor *last, bool lv_ok)
{
	cNeighbor *n, *next = NO_NEIGHBOR;

	// next neighbor must be not in the listview and be sorted after the current last neighbor
	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
	{
		if ((n->Avatar().Hidden()) && (player->ID() != n->ID()))
			continue;
		if ((lv_ok || this->LookUpNeighborIndex(n) == -1) && (CompareNeighbors((long)n, (long)last, 0) == 1))
		{
			if ((next == NO_NEIGHBOR) ||
				(CompareNeighbors((long)n, (long)next, 0) == -1))
				next = n;
		}
	}
	actors->IterateNeighbors(DONE);
	return next;
}


void cControlPanel::AddNextNeighbor(void) 
{
	if ((num_neighbors == 0) || (num_neighbors == MAX_LV_ITEMS))
		return;

	cNeighbor *next = this->GetNextNeighbor(this->GetLastNeighbor());
	if (next != NO_NEIGHBOR)
	{
		this->AddNeighbor(next);
		SendMessage(hwnd_listviews[NEIGHBORS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareNeighbors);
	}
}

cNeighbor* cControlPanel::GetPrevNeighbor(cNeighbor *first, bool lv_ok)
{
	cNeighbor *n, *prev = NO_NEIGHBOR;

	// prev neighbor must be not in the listview and be sorted after the current first neighbor
	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
	{
		if ((n->Avatar().Hidden()) && (player->ID() != n->ID()))
			continue;
		if ((lv_ok || this->LookUpNeighborIndex(n) == -1) && (CompareNeighbors((long)n, (long)first, 0) == -1))
		{
			if ((prev == NO_NEIGHBOR) ||
				(CompareNeighbors((long)n, (long)prev, 0) == 1))
				prev = n;
		}
	}
	actors->IterateNeighbors(DONE);
	return prev;
}


void cControlPanel::AddPrevNeighbor(void) 
{
	if ((num_neighbors == 0) || (num_neighbors == MAX_LV_ITEMS))
		return;

	cNeighbor *prev = this->GetPrevNeighbor(this->GetFirstNeighbor());
	if (prev != NO_NEIGHBOR)
	{
		this->AddNeighbor(prev);
		SendMessage(hwnd_listviews[NEIGHBORS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareNeighbors);
	}

}

lyra_id_t cControlPanel::GetNextArt(lyra_id_t last, bool lv_ok)
{
	lyra_id_t next = Arts::NONE;

	// next art must have skill, not be in the listview, and be sorted after the current last art
	for (int i=0; i<NUM_ARTS; i++)
		if (player->Skill(i) && (lv_ok || this->LookUpArtIndex(i) == -1) && 
			(CompareArts((long)i, (long)last, 0) == 1))
		{
			if ((next == Arts::NONE) ||
				(CompareArts((long)i, (long)next, 0) == -1))
				next = i;
		}
	return next;
}

void cControlPanel::AddNextArt(void) 
{
	if ((num_arts == 0) || (num_arts == MAX_LV_ITEMS))
		return;

	lyra_id_t next = this->GetNextArt(this->GetLastArt());
	if (next != Arts::NONE)
	{
		this->AddArt(next);
		SendMessage(hwnd_listviews[ARTS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareArts);
	}
}

lyra_id_t cControlPanel::GetPrevArt(lyra_id_t first, bool lv_ok)
{
	lyra_id_t prev = Arts::NONE;

	// prev art must have skill, not be in the listview, and be sorted after the current first art
	for (int i=0; i<NUM_ARTS; i++)
		if (player->Skill(i) && (lv_ok || this->LookUpArtIndex(i) == -1) && 
			(CompareArts((long)i, (long)first, 0) == -1))
		{
			if ((prev == Arts::NONE) ||
				(CompareArts((long)i, (long)prev, 0) == 1))
				prev = i;
		}
	return prev;
}


void cControlPanel::AddPrevArt(void) 
{
	if ((num_arts == 0) || (num_arts == MAX_LV_ITEMS))
		return;

	lyra_id_t prev = this->GetPrevArt(this->GetFirstArt());
	if (prev != Arts::NONE)
	{
		this->AddArt(prev);
		SendMessage(hwnd_listviews[ARTS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareArts);
	}
}



void cControlPanel::ShowPrevItem(int count) 
{
	if (num_items < MAX_LV_ITEMS)
		return;
	for (int i=0; i<count; i++)
	{
		if (this->GetPrevItem(this->GetFirstItem()) == NO_ACTOR)
			break;
		this->DeleteItem(GetLastItem(), SHOW_PREV);
	}
}

void cControlPanel::ShowNextItem(int count) 
{
	if (num_items < MAX_LV_ITEMS)
		return;
	for (int i=0; i<count; i++)
	{
		if (this->GetNextItem(this->GetLastItem()) == NO_ACTOR)
			break;
		this->DeleteItem(this->GetFirstItem(), SHOW_NEXT);
	}
}


void cControlPanel::ShowNextNeighbor(int count) 
{
	if (num_neighbors < MAX_LV_ITEMS)
		return;
	for (int i=0; i<count; i++)
	{
		if (this->GetNextNeighbor(this->GetLastNeighbor()) == NO_ACTOR)
			break;
		this->DeleteNeighbor(GetFirstNeighbor(), SHOW_NEXT);
	}
}

void cControlPanel::ShowPrevNeighbor(int count) 
{
	if (num_neighbors < MAX_LV_ITEMS)
		return;
	for (int i=0; i<count; i++)
	{
		if (this->GetPrevNeighbor(this->GetFirstNeighbor()) == NO_ACTOR)
			break;
		this->DeleteNeighbor(GetLastNeighbor(), SHOW_PREV);
	}
}


void cControlPanel::ShowNextArt(int count) 
{
	if (num_arts < MAX_LV_ITEMS)
		return;
	for (int i=0; i<count; i++)
	{
		if (this->GetNextArt(this->GetLastArt()) == Arts::NONE)
			break;
		this->DeleteArt(GetFirstArt(), SHOW_NEXT);
	}
}

void cControlPanel::ShowPrevArt(int count) 
{
	if (num_arts < MAX_LV_ITEMS)
		return;
	for (int i=0; i<count; i++)
	{
		if (this->GetPrevArt(this->GetFirstArt()) == Arts::NONE)
			break;
		this->DeleteArt(GetLastArt(), SHOW_PREV);
	}
}

// returns the listview index of the given neighbor, or -1 if
// it can not be found
int cControlPanel::LookUpNeighborIndex(cNeighbor *n)
{
	LV_FINDINFO stats;

    stats.flags = LVFI_PARAM; stats.lParam = (LPARAM)n;

	return ListView_FindItem(hwnd_listviews[NEIGHBORS_TAB], -1, &stats);

}

// returns the listview index of the given art, or -1 if
// it can not be found
int cControlPanel::LookUpArtIndex(realmid_t art)
{
	LV_FINDINFO stats;

    stats.flags = LVFI_PARAM; stats.lParam = (LPARAM)art;

	return ListView_FindItem(hwnd_listviews[ARTS_TAB], -1, &stats);

}


// returns the item associated with a given listview index
cItem* cControlPanel::LookUpItem(int index)
{
	LV_ITEM stats;
	stats.mask = LVIF_PARAM; stats.iItem = index; stats.iSubItem = 0;
	ListView_GetItem(hwnd_listviews[INVENTORY_TAB], &stats);
	return (cItem*)stats.lParam;
}

// returns the neighbor associated with a given listview index
cNeighbor* cControlPanel::LookUpNeighbor(int index)
{
	LV_ITEM stats;
	stats.mask = LVIF_PARAM; stats.iItem = index; stats.iSubItem = 0;
	ListView_GetItem(hwnd_listviews[NEIGHBORS_TAB], &stats);
	return (cNeighbor*)stats.lParam;
}

// returns the art associated with a given listview index
realmid_t cControlPanel::LookUpArt(int index)
{
	LV_ITEM stats;
	stats.mask = LVIF_PARAM; stats.iItem = index; stats.iSubItem = 0;
	ListView_GetItem(hwnd_listviews[ARTS_TAB], &stats);
	return (realmid_t)stats.lParam;
}



void cControlPanel::DeleteItem(cItem* item, int type)
{ 	// look up the item based on the pointer & delet
	LV_ITEM stats;

	if (LookUpItemIndex(item) != -1)
	{  // delete and rearrange array of items
		player->SetItemNeedFlagsOrSortingUpdate(true);
		// remove image from image list
		num_items--;
		//_tprintf("deleting item %s, si = %d\n",item->Name(), item->SortIndex());
		if (type == SHOW_NEXT)
		{
			this->AddNextItem();
			if (num_items < MAX_LV_ITEMS)
				this->AddPrevItem(); // could be the last one that got dropped
		}
		else if (type == SHOW_PREV)
		{
			this->AddPrevItem();
			if (num_items < MAX_LV_ITEMS)
				this->AddNextItem();
		} // warning - index can change when items are added/deleted!
		stats.mask = LVIF_IMAGE; stats.iItem = LookUpItemIndex(item); stats.iSubItem = 0;
		ListView_GetItem(hwnd_listviews[INVENTORY_TAB], &stats);
		ListView_DeleteItem(hwnd_listviews[INVENTORY_TAB], stats.iItem);
		this->RemoveIcon(stats.iImage);
		ListView_Arrange(hwnd_listviews[INVENTORY_TAB], LVA_ALIGNTOP);
		SendMessage(hwnd_listviews[INVENTORY_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareItems);
		//ListView_RedrawItems(hwnd_listviews[INVENTORY_TAB], 0, num_items); 
		//this->DumpInventory();
	}
}


void cControlPanel::DeleteNeighbor(cNeighbor *n, int type)
{ 	// look up the neighbor based on the pointer & delete
	LV_ITEM stats;

	if (LookUpNeighborIndex(n) != -1)
	{  // delete and rearrange array 
		// remove image from image list
		num_neighbors--;
		if (type == SHOW_NEXT)
		{
			this->AddNextNeighbor();
			if (num_neighbors < MAX_LV_ITEMS)
				this->AddPrevNeighbor();
		}
		else if (type == SHOW_PREV)
		{
			this->AddPrevNeighbor();
			if (num_neighbors < MAX_LV_ITEMS)
				this->AddNextNeighbor();
		}
		stats.mask = LVIF_IMAGE; stats.iItem = LookUpNeighborIndex(n); stats.iSubItem = 0;
		ListView_GetItem(hwnd_listviews[NEIGHBORS_TAB], &stats);
		ListView_DeleteItem(hwnd_listviews[NEIGHBORS_TAB], stats.iItem);
		this->RemoveIcon(stats.iImage);
		ListView_Arrange(hwnd_listviews[NEIGHBORS_TAB], LVA_ALIGNTOP);
		SendMessage(hwnd_listviews[NEIGHBORS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareNeighbors);
		//ListView_RedrawItems(hwnd_listviews[NEIGHBORS_TAB], 0, num_neighbors); 
	}
}

void cControlPanel::DeleteArt(realmid_t art, int type)
{ 	// look up the item based on the pointer & delete
	if (LookUpArtIndex(art) != -1)
	{  // delete and rearrange array of items
		num_arts--;
		if (type == SHOW_NEXT)
		{
			this->AddNextArt();
			if (num_arts < MAX_LV_ITEMS)
				this->AddPrevArt();
		}
		else if (type == SHOW_PREV)
		{
			this->AddPrevArt();
			if (num_arts < MAX_LV_ITEMS)
				this->AddNextArt();
		}
		ListView_DeleteItem(hwnd_listviews[ARTS_TAB], LookUpArtIndex(art));
		ListView_Arrange(hwnd_listviews[ARTS_TAB], LVA_ALIGNTOP);
		SendMessage(hwnd_listviews[ARTS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareArts);
		//ListView_RedrawItems(hwnd_listviews[ARTS_TAB], 0, num_arts); 
	}
}

// returns true if we're maxed out on items; false otherwise
bool cControlPanel::InventoryFull(void)
{
	cItem *item;
	int curr_items = 0;

	// add items being "gotten" to total inventory load
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_GETTING) || (item->Status() == ITEM_CREATING) ||
			(item->Status() == ITEM_OWNED))
			curr_items++;
	actors->IterateItems(DONE);

	if (curr_items == Lyra::INVENTORY_MAX)
		return true;
	else
		return false;
}

// returns true if we have items; false otherwise
bool cControlPanel::InventoryEmpty(void)
{
	cItem *item;
	int curr_items = 0;

	// add items being "gotten" to total inventory load
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_GETTING) || (item->Status() == ITEM_CREATING) ||
			(item->Status() == ITEM_OWNED))
		{
			curr_items++;
			break;
		}
	actors->IterateItems(DONE);

	if (curr_items == 0)
		return true;
	else
		return false;
}

// redraws the icon used for a neighbor, due to avatar change
void cControlPanel::ReplaceNeighborIcon(cNeighbor* n)
{
	LV_ITEM stats;
	int index = LookUpNeighborIndex(n);

	if (index != -1)
	{ // change out image used for icon
		stats.mask = LVIF_IMAGE; stats.iItem = index; stats.iSubItem = 0;
		ListView_GetItem(hwnd_listviews[NEIGHBORS_TAB], &stats);
		this->RemoveIcon(stats.iImage);
		stats.iImage = this->AddIcon(n->IconBits());
		ListView_SetItem(hwnd_listviews[NEIGHBORS_TAB], &stats);
		if (tab_mode == NEIGHBORS_TAB)
			ListView_RedrawItems(hwnd_listviews[NEIGHBORS_TAB], index, index); 
	}
	return;
}


// returns the # of agents nearby
int cControlPanel::NumMonsters(void)
{
	cNeighbor *n;
	int num_monsters = 0;

	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
		if (n->IsMonster())
			num_monsters++;
	actors->IterateNeighbors(DONE);

	return num_monsters;
}

// return color to use for stats
unsigned int cControlPanel::LookUpStatColor(int stat)
{
	if (player->MaxStat(stat) == 0)
		return ORANGE; // avoid dbz error on init

	float percent = (((float)player->CurrStat(stat))/((float)player->MaxStat(stat)));

	if (percent < 0.34f)
		return RED;
	else if (percent < 0.67f)
		return YELLOW;
	else if (percent < 0.99f)
		return GREEN;
	else
		return ORANGE;
}

// used to deselect the current selection in the listview
void cControlPanel::DeselectSelected(void)
{
	if (tab_mode == INVENTORY_TAB)
		if (this->SelectedItem() == NO_ITEM) {
			LoadString (hInstance, IDS_ITEM_DESELECT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
		else
			this->SetSelectedItem(NO_ITEM);
	else if (tab_mode == NEIGHBORS_TAB)
		if (this->SelectedNeighbor() == NO_ACTOR) {
			LoadString (hInstance, IDS_NEIGHBOR_DESELECT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
		else
			this->SetSelectedNeighbor(NO_ACTOR);
	else if (tab_mode == ARTS_TAB)
		if (this->SelectedArt() == Arts::NONE) {
			LoadString (hInstance, IDS_ART_DESELECT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
		else
			this->SetSelectedArt(Arts::NONE);
	return;
}

// used to select the "next" or "prev" selection in the active listview
void cControlPanel::SelectNew(bool direction)
{
	cItem *item;
	cNeighbor *n;
	lyra_id_t art;

	last_select_by_click = false;

	if (tab_mode == INVENTORY_TAB)  
	{
		if (num_items == 0) 
		{
			LoadString (hInstance, IDS_ITEM_NONE, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			return;
		}

		// figure out index of new selected item
		if ((selected_item == NO_ITEM) && (direction == SELECT_NEXT_LISTITEM))
			item = this->GetFirstItem();
		else if ((selected_item == NO_ITEM) && (direction == SELECT_PREV_LISTITEM))
			item = this->GetLastItem();
		else if ((direction == SELECT_NEXT_LISTITEM) &&  
			     (this->GetNextItem(selected_item, true) != NO_ITEM))
			item = this->GetNextItem(selected_item, true);
		else if ((direction == SELECT_PREV_LISTITEM) &&  
			     (this->GetPrevItem(selected_item, true) != NO_ITEM))
			item = this->GetPrevItem(selected_item, true);
		else
			item = selected_item;

		// scroll for new selected item, if needed
		if ((LookUpItemIndex(selected_item) != -1) && (LookUpItemIndex(item) == -1))
		{
			if (CompareItems((long)item, (long)selected_item, 0) == 1)
				this->ShowNextItem(1);
			else
				this->ShowPrevItem(1);
		}

		cp->SetSelectedItem(item);
		SendMessage(hwnd_listviews[INVENTORY_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareItems);

	}
	else if (tab_mode == NEIGHBORS_TAB)
	{
		if (num_neighbors == 0) 
		{
			LoadString (hInstance, IDS_NEIGHBOR_NONE, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			return;
		}
		// figure out index of new selected item
		if ((selected_neighbor == NO_ACTOR) && (direction == SELECT_NEXT_LISTITEM))
			n = this->GetFirstNeighbor();
		else if ((selected_neighbor == NO_ACTOR) && (direction == SELECT_PREV_LISTITEM))
			n = this->GetLastNeighbor();
		else if ((direction == SELECT_NEXT_LISTITEM) &&  
			     (this->GetNextNeighbor(selected_neighbor, true) != NO_ACTOR))
			n = this->GetNextNeighbor(selected_neighbor, true);
		else if ((direction == SELECT_PREV_LISTITEM) &&  
			     (this->GetPrevNeighbor(selected_neighbor, true) != NO_ACTOR))
			n = this->GetPrevNeighbor(selected_neighbor, true);
		else
			n = selected_neighbor;

		// scroll for new selected neighbor, if needed
		if ((LookUpNeighborIndex(selected_neighbor) != -1) && (LookUpNeighborIndex(n) == -1))
		{
			if (CompareNeighbors((long)n, (long)selected_neighbor, 0) == 1)
				this->ShowNextNeighbor(1);
			else
				this->ShowPrevNeighbor(1);
		}

		cp->SetSelectedNeighbor(n);
		SendMessage(hwnd_listviews[NEIGHBORS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareNeighbors);
	}
	else if (tab_mode == ARTS_TAB)
	{
		if (num_arts == 0) 
		{
			LoadString (hInstance, IDS_ART_NONE, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			return;
		}
		// figure out index of new selected item
		if ((selected_art == Arts::NONE) && (direction == SELECT_NEXT_LISTITEM))
			art = this->GetFirstArt();
		else if ((selected_art == Arts::NONE) && (direction == SELECT_PREV_LISTITEM))
			art = this->GetLastArt();
		else if ((direction == SELECT_NEXT_LISTITEM) &&  
			     (this->GetNextArt(selected_art, true) != Arts::NONE))
			art = this->GetNextArt(selected_art, true);
		else if ((direction == SELECT_PREV_LISTITEM) &&  
			     (this->GetPrevArt(selected_art, true) != Arts::NONE))
			art = this->GetPrevArt(selected_art, true);
		else
			art = selected_art;

		// scroll for new selected neighbor, if needed
		if ((LookUpArtIndex(selected_art) != -1) && (LookUpArtIndex(art) == -1))
		{
			if (CompareArts((long)art, (long)selected_art, 0) == 1)
				this->ShowNextArt(1);
			else
				this->ShowPrevArt(1);
		}

		cp->SetSelectedArt(art);
		SendMessage(hwnd_listviews[ARTS_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareArts);
	}
	return;
}

void cControlPanel::SetListViewSelected(HWND listview, int curr_index, int new_index)
{
	LV_ITEM stats;

	stats.mask = LVIF_STATE;
	stats.stateMask = LVIS_SELECTED | LVIS_DROPHILITED;
	stats.iSubItem = 0;

	// unselect current
	stats.state = 0;
	stats.iItem = curr_index;
	if (curr_index != -1)
		SendMessage(listview, LVM_SETITEMSTATE, (WPARAM) curr_index, (LPARAM)&stats);
	
	stats.state = LVIS_SELECTED;
	stats.iItem = new_index;
	if (new_index != -1)
		SendMessage(listview, LVM_SETITEMSTATE, (WPARAM) new_index, (LPARAM)&stats);
}

cItem*  cControlPanel::SelectedItem(void) 
{
	if (!(actors->ValidItem(selected_item)) || (selected_item->Status() != ITEM_OWNED))
		selected_item = NO_ITEM;
	return selected_item;
}

// note - we have to be careful with the order things are done here;
// selected must be set to the new value BEFORE calling setlistviewselected
// to avoid an infinite loop, but we have to look up the indexes
// of the items before we set selected.
void cControlPanel::SetSelectedItem(cItem *item) 
{ 
	if (selected_item == item)
		return; // avoid endless loop of WM_NOTIFY and SetSelected calls
	int i = LookUpItemIndex(selected_item);
	int j = LookUpItemIndex(item);
	selected_item = item;
	this->SetListViewSelected(hwnd_listviews[INVENTORY_TAB], i, j);
	return;
}

cNeighbor* cControlPanel::SelectedNeighbor(void) 
{
	if (!(actors->ValidNeighbor(selected_neighbor)))
		selected_neighbor = NO_ACTOR;
	return selected_neighbor;
}

void cControlPanel::SetSelectedNeighbor(cNeighbor *n) 
{ 
	if (selected_neighbor == n)
		return; // avoid endless loop of WM_NOTIFY and SetSelected calls
	int i = LookUpNeighborIndex(selected_neighbor);
	int j = LookUpNeighborIndex(n);
	selected_neighbor = n;
	this->SetListViewSelected(hwnd_listviews[NEIGHBORS_TAB], i, j);
}


lyra_id_t cControlPanel::SelectedArt(void) 
{
	if (!(player->Skill(selected_art)))
		selected_art = Arts::NONE;
	return selected_art;
}

void cControlPanel::SetSelectedArt(realmid_t art) 
{ 
	if (selected_art == art)
		return; // avoid endless loop of WM_NOTIFY and SetSelected calls
	int i = LookUpArtIndex(selected_art);
	int j = LookUpArtIndex(art);
	selected_art = art;
	this->SetListViewSelected(hwnd_listviews[ARTS_TAB], i, j);
}


void cControlPanel::Show(void)
{ 
	ShowWindow(hwnd_tab, SW_SHOWNORMAL );
	ShowWindow(hwnd_cp, SW_SHOWNORMAL ); 
	//ShowWindow(hwnd_left, SW_SHOWNORMAL ); 
	//ShowWindow(hwnd_right, SW_SHOWNORMAL ); 
	ShowWindow(hwnd_meta, SW_SHOWNORMAL ); 
	ShowWindow(hwnd_orbit, SW_SHOWNORMAL );

	int curr_mode = tab_mode;
	tab_mode = NO_TAB; // force setmode to run through
	this->SetMode(curr_mode, false, true);

	for (int i=0; i<NUM_PLAYER_STATS; i++)
		ShowWindow(hwnd_stats[i], SW_SHOWNORMAL );

}


 // set up initial listing of arts
void cControlPanel::SetupArts(void)
{
	lyra_id_t first = Arts::NONE;
	for (int i=0; i<NUM_ARTS; i++)
		if (player->Skill(i) &&
			((first == Arts::NONE) ||
			 (CompareArts(i, first, 0) == -1)))
			 first = i;
	while (num_arts)
		this->DeleteArt(this->GetFirstArt(), SHOW_NONE);
	this->AddArt(first);
	for (int i=0; i<MAX_LV_ITEMS - 1; i++)
		this->AddNextArt();

}




void cControlPanel::FillInArtString(lyra_id_t art, TCHAR *buffer)
{
_stprintf(buffer, _T("%s %d/%d"),arts->Descrip(art),player->Skill(art),arts->Drain(art));
}


// update display of a player's skill by adjusting art icon
// if skill is > 0 and it's not on the list, add it
// if skill is = 0 and it's on the list, delete it
void cControlPanel::UpdateArt(lyra_id_t art)
{
	int i;
	LV_ITEM stats;

	if ((art < 0) || (art >= NUM_ARTS))
		return;

	i = this->LookUpArtIndex(art);

	if ((i == -1) && (player->Skill(art) > 0))
		this->AddArt(art);	// add to list
	else if ((i != -1) && (player->Skill(art) == 0))
		this->DeleteArt(art);  // remove from list
	else if ((i != -1) && (player->Skill(art) > 0))
	{	// update
		stats.iSubItem = 0; 
		//stats.mask = LVIF_IMAGE;
		stats.mask = LVIF_TEXT;
		stats.iItem = LookUpArtIndex(art);
		this->FillInArtString(art, message);
		stats.pszText = message;
		//stats.iImage = player->Skill(art);
		ListView_SetItem(hwnd_listviews[ARTS_TAB], &stats);
	}

	return;
}

// update display of inventory counter
void cControlPanel::UpdateInvCount(void)
{
	if(!setCounter)
		return;

	setCounter = false;
	TCHAR new_value[STAT_LENGTH];
	TCHAR old_value[STAT_LENGTH];
	RECT region;
	cItem *item = NO_ITEM;

	_stprintf(new_value, _T("%02d/%02d\0"), num_items, Lyra::INVENTORY_MAX);
	GetWindowText(hwnd_invcounter, old_value, STAT_LENGTH);
	if (_tcscmp(new_value, old_value))
	{
		SetWindowText(hwnd_invcounter, new_value);
		region.left = invcountPos[cDD->Res()].x;
		region.top = invcountPos[cDD->Res()].y;
		region.right = invcountPos[cDD->Res()].x + invcountPos[cDD->Res()].width;
		region.bottom = invcountPos[cDD->Res()].y + invcountPos[cDD->Res()].height;
		InvalidateRect(hwnd_tab, &region, FALSE);
	}

	return;
}

// update display of stats; only blit new value if necessary
void cControlPanel::UpdateStats(void)
{
	TCHAR new_value[STAT_LENGTH];
	TCHAR old_value[STAT_LENGTH];
	RECT region;

	for (int i=0; i<NUM_PLAYER_STATS; i++)
	{
	_stprintf(new_value,_T("%02d/%02d\0"),player->CurrStat(i),player->MaxStat(i));
		GetWindowText(hwnd_stats[i], old_value, STAT_LENGTH);
		if    (_tcscmp(new_value,old_value))
		{
			SetWindowText(hwnd_stats[i], new_value);
			region.left = cp_stats[i].position[cDD->Res()].x; region.top = cp_stats[i].position[cDD->Res()].y;
			region.right = cp_stats[i].position[cDD->Res()].x + cp_stats[i].position[cDD->Res()].width; 
			region.bottom = cp_stats[i].position[cDD->Res()].y + cp_stats[i].position[cDD->Res()].height;
			InvalidateRect(hwnd_cp, &region, FALSE);
		}

	}

_stprintf(new_value,_T("%d\0"),player->Orbit());
	GetWindowText(hwnd_orbit, old_value, STAT_LENGTH);
	if    (_tcscmp(new_value,old_value))
	{
		SetWindowText(hwnd_orbit, new_value);
		region.left = orbitPos[cDD->Res()].x; region.top = orbitPos[cDD->Res()].y;
		region.right = orbitPos[cDD->Res()].x + orbitPos[cDD->Res()].width; 
		region.bottom = orbitPos[cDD->Res()].y + orbitPos[cDD->Res()].height;
		InvalidateRect(hwnd_cp, &region, FALSE);
	}

	return;
}

// finds the entry closest to the current cursor position
// for the listview specified by the parameter; returns -1 if
// no item is found
int cControlPanel::FindClosestEntry(HWND hwnd_listview)
{
	POINT cursor_pos; 
	int closest = -1;
	int closest_diff = INT_MAX;
	RECT rect, item_rect;

	GetCursorPos(&cursor_pos);
	GetWindowRect(hwnd_listview, &rect);

	cursor_pos.y -= rect.top; // set cursor to be relative to window
	
	for (int i=0; i<ListView_GetItemCount(hwnd_listview); i++)
	{
		ListView_GetItemRect(hwnd_listview, i, &item_rect, LVIR_BOUNDS);
		if ((abs(cursor_pos.y - (item_rect.top + ICON_HEIGHT/2))) < closest_diff)
		{
			closest_diff = abs(cursor_pos.y - (item_rect.top + ICON_HEIGHT/2));
			closest = i;	
		}
	}

	return closest;
}
	

// xoffset and yoffset are the hotspot offsets used for drawing
// the item relative to the cursor position
void cControlPanel::StartDrag(cItem *item, int xoffset, int yoffset)
{
	hotspot_pos.x = xoffset;
	hotspot_pos.y = yoffset;

	player->PerformedAction();

	if (hotspot_pos.x >= ICON_WIDTH)
		hotspot_pos.x = ICON_WIDTH -1;
	if (hotspot_pos.y >= ICON_HEIGHT)
		hotspot_pos.y = ICON_HEIGHT -1;

	drag_item = item;

	// munched_bits_valid = false;

	HBITMAP bmp = effects->Create16bppBitmapFromBits(item->IconBits(), ICON_WIDTH, ICON_HEIGHT);

	HCURSOR hc = NULL;
	hc = BitmapToCursor(bmp, hc);
	SetCursor(hc);
//	ShowCursor(FALSE);
	//this->DrawDrag(false);
	DeleteObject(bmp);
	SetCapture(hwnd_cp);
}

void cControlPanel::EndDrag(void)
{
	HCURSOR hCursor = LoadCursor(NULL,IDC_ARROW);
	SetCursor(hCursor);
	ReleaseCapture();
	captured = false;

	POINT cursor_pos;
	RECT view_rect, inv_rect, tab_rect, item_rect;
	float itemx,itemy;
	int closest, closest_diff, sort_index;

	GetCursorPos(&cursor_pos);

/*	if (!this->UndrawDrag()) // in case we couldn't undo the last draw...
	{
		InvalidateRect(cDD->Hwnd_Main(), NULL, TRUE);
		if (display)
			InvalidateRect(display->Hwnd(), NULL, TRUE);
//		if (banner)
//			InvalidateRect(banner->Hwnd(), NULL, TRUE);
		InvalidateRect(hwnd_cp, NULL, TRUE);
		InvalidateRect(hwnd_tab, NULL, TRUE);
		InvalidateRect(hwnd_listviews[INVENTORY_TAB], NULL, TRUE);
		InvalidateRect(hwnd_listviews[NEIGHBORS_TAB], NULL, TRUE);
		InvalidateRect(hwnd_listviews[ARTS_TAB], NULL, TRUE);

	}
	ReleaseCapture();
	captured = false;
*/
	cDD->ViewRect(&view_rect);
	GetWindowRect(hwnd_listviews[INVENTORY_TAB], &inv_rect);
	GetWindowRect(hwnd_tab, &tab_rect);


	// if owned & released over viewport...
	if (PtInRect(&view_rect, cursor_pos))
	{ // drop the item if owned
		GetPickWorldCo_ords(itemx,itemy);
		if (drag_item->Status() == ITEM_OWNED)
			drag_item->Drop(itemx, itemy, player->angle);
		else  // place if unowned
			drag_item->PlaceActor(itemx,itemy,0,player->angle, SET_XHEIGHT, true);
	} 
	else if ((PtInRect(&inv_rect, cursor_pos)) || (PtInRect(&tab_rect, cursor_pos)))
	{// if unowned & released over inventory, get the item
		if ((drag_item->Status() == ITEM_UNOWNED))
			drag_item->Get();
		else
		{	// renumber sort indexes for items; first, find the two items that it is inbetween
			player->SetItemNeedFlagsOrSortingUpdate(true);
			closest = 0;  
			sort_index = drag_item->SortIndex();
			closest_diff = INT_MAX;
			cursor_pos.y -= inv_rect.top; // set cursor to be relative to window
			int i;
			for (i=0; i<num_items; i++)
			{
				ListView_GetItemRect(hwnd_listviews[INVENTORY_TAB], i, &item_rect, LVIR_BOUNDS);
				if ((cursor_pos.y >= item_rect.top) && ((cursor_pos.y - item_rect.top) < closest_diff))
				{
					closest_diff = cursor_pos.y - item_rect.top;
					closest = i;
					sort_index = this->LookUpItem(i)->SortIndex();
				}
			}
			//_stprintf(message, "closest is %d\n", closest);
			//display->DisplayMessage (message);
			if (num_items)
			{ // sort_index = sort index of item to place this after
			  // set drag item to sort index+1, those after it to sort index +2
				drag_item->SetSortIndex(sort_index+1);
				// if at top half of item, put in front; if at bottom, after
				if (closest_diff <= ICON_HEIGHT/2)
					i = closest;
				else
					i = closest+1;
				for (cItem* item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
					if ((item->Status() == ITEM_OWNED)  && (item != drag_item) && (item->SortIndex() > sort_index))
						item->SetSortIndex(item->SortIndex() + 2);
				actors->IterateItems(DONE);

			}
			SendMessage(hwnd_listviews[INVENTORY_TAB], LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareItems);
		}
		InvalidateRect(hwnd_listviews[this->Mode()], NULL, TRUE);
	}

	drag_item = NO_ITEM;

//	ShowCursor(TRUE);

}

// scroll inventory list up/down if holding the drag item at
// the bottom/top of the listview
void cControlPanel::CheckDragScroll(void)
{
	RECT lv_rect;
	POINT cursor_pos;

	if (drag_item == NO_ITEM)
		return;

	GetWindowRect(hwnd_listviews[INVENTORY_TAB], &lv_rect);
	GetCursorPos(&cursor_pos);

	if ((cursor_pos.x >= lv_rect.left) &&
		(cursor_pos.x <= lv_rect.right))
	{
		if (((cursor_pos.y - ICON_HEIGHT/2) < lv_rect.top) &&
			(cursor_pos.y - ICON_HEIGHT/2 + DRAG_REGION > lv_rect.top))
			this->ShowPrevItem(1);
		else if (((cursor_pos.y + ICON_HEIGHT/2) > lv_rect.bottom) &&
			(cursor_pos.y + ICON_HEIGHT/2 - DRAG_REGION < lv_rect.bottom))
			this->ShowNextItem(1);
	}
}

// If viewport false, it is a GDI only draw; if viewport is true, 
// it is a viewport only call. If buffer is non-NULL, use it, otherwise
// get the buffer from cDD. Returns false if buffer can't be
// captured; returns true otherwise
bool cControlPanel::DrawDrag(bool viewport, unsigned char *buffer)
{
/* None of the old code here is needed due to the update to Windows 10 -- JAH 5/2/16
	unsigned char *dst = buffer;
	unsigned char *source = drag_item->IconBits();
	int	i,j,p,xoff,yoff;
	POINT cursor;
	POINT current;
	RECT viewrect;
	bool create_buffer = (buffer == NULL);

	GetCursorPos(&cursor);
	// set buffer to spot of current draw
	xoff = (cursor.x - hotspot_pos.x);
	yoff = (cursor.y - hotspot_pos.y);

	// if viewport only and drawing entire on GDI, do nothing
	if (viewport && ((xoff >= cDD->ViewX()) || (yoff >= cDD->ViewY())))
		return true; 

	if (create_buffer)
	{ // capture the surface, if needed
		if (viewport)
			dst = cDD->GetSurface(BACK_BUFFER);
		else
			dst = cDD->GetSurface(PRIMARY);
		if (!dst)
			return false; // don't do anything if we can't get the surface
	}

	p = cDD->Pitch();

	cDD->ViewRect(&viewrect); // set up viewport rectangle
	
	dst += xoff*BYTES_PER_PIXEL + yoff*p;

	for (i=0; i<ICON_HEIGHT; i++)
	{
		for (j=0; j<ICON_WIDTH; j++)
		{	// don't go outside the viewport if we want the viewport only
			current.x = xoff + j;
			current.y = yoff + i;
			if ((viewport && (PtInRect(&viewrect, current)))  // in viewport
				|| (!viewport && ((xoff + j >= cDD->ViewX()) || (yoff + i >= cDD->ViewY())))) // not in viewport
			{	// don't bother saving the bits if its viewport only or off the screen
				if ((yoff >=0) && (xoff >= 0))
				{
					if (!viewport)
						memcpy(&(munched_bits[(i*ICON_HEIGHT+j)*BYTES_PER_PIXEL]),dst,BYTES_PER_PIXEL);
					if (*source || (*(source+1)))
						memcpy(dst, source, BYTES_PER_PIXEL);
				}
			}
			source+=BYTES_PER_PIXEL;
			dst+=BYTES_PER_PIXEL;
		}
		dst += (p - ICON_WIDTH*BYTES_PER_PIXEL);
	}

	if (create_buffer)
	{
		if (viewport)
			cDD->ReleaseSurface(BACK_BUFFER);
		else
			cDD->ReleaseSurface(PRIMARY);
	}
	
	if (!viewport)
	{
		last_drag.x = cursor.x;
		last_drag.y = cursor.y;
		munched_bits_valid = true;
	} */
	return true;
}

// use the stored image to put back what the drag image was on top of
// note that we always use the primary buffer to do this, since we
// really only care about GDI updates; the viewport stuff is constantly
// redrawn so we don't undraw over the viewport
bool cControlPanel::UndrawDrag(unsigned char *buffer)
{
/* // This was mostly deprecated in favor of massively less code required for drawing items that were
// 'on the cursor' due to Windows 10 update -- JAH 5/2/16
	unsigned char *dst = buffer;
	unsigned char *source = (unsigned char*)munched_bits;
	int	i,j,p,xoff,yoff;
	bool create_buffer = (buffer == NULL);

	if (!munched_bits_valid) // nothing to redraw
		return true;
		
	if (create_buffer)
	{
		dst = cDD->GetSurface(PRIMARY);
		if (!dst)
			return false; // don't do anything if we can't read the old bits
	}

	p = cDD->Pitch();

	// set buffer to spot of current draw
	xoff = (last_drag.x - hotspot_pos.x);
	yoff = (last_drag.y - hotspot_pos.y);

	dst += xoff*BYTES_PER_PIXEL + yoff*p;

	for (i=0; i<ICON_HEIGHT; i++)
	{
		for (j=0; j<ICON_WIDTH; j++)
		{	// only draw on GDI
			if ((xoff + j >= cDD->ViewX()) || (yoff + i >= cDD->ViewY()))
				if ((xoff >=0) && (yoff >=0))
					memcpy(dst, source, BYTES_PER_PIXEL);
			source+=BYTES_PER_PIXEL;
			dst+=BYTES_PER_PIXEL;
		}
		dst += (p - ICON_WIDTH*BYTES_PER_PIXEL);
	}
	
	if (create_buffer)
		cDD->ReleaseSurface(PRIMARY);

	munched_bits_valid = false;
*/
	if (drag_item == NO_ITEM)
		return false;
	return true;
}

int cControlPanel::AddIcon(unsigned char* icon_bits)
{
	HBITMAP bitmap;
	int image_index=0;

	if (num_unused_icons == (MAX_UNUSED_ICONS - 1))
		this->CompactImageList();

	bitmap = effects->Create16bppBitmapFromBits(icon_bits, ICON_WIDTH, ICON_HEIGHT);
	image_index = ImageList_AddMasked(image_list, bitmap, RGB(0,0,0));

	if (image_index == -1)
	{	
		GAME_ERROR(IDS_NO_ADD_BMP_IMG_LIST);
		return -1;
	}

	DeleteObject(bitmap);

	num_icons++;

	return image_index;
}


// deleting icons is a bit weird - because we want indexes to remain
// correct after deletion, and because windows shifts down the icon
// bits after a delete to fill in the empty space, we must go 
// occasionally delete all the icons and reinsert the live ones
// note that the lowest-numbered icons are for arts, which don't change,
// so we skip over those. This method rebuilds the image list, 
// removing unused entries
void cControlPanel::CompactImageList()
{
	LV_ITEM stats;
	stats.iSubItem = 0; 
	stats.mask = LVIF_IMAGE;
	cNeighbor *n;
	cItem *item;

	while (num_icons > NUM_PLAYER_STATS)
	{
		ImageList_Remove(image_list, num_icons - 1);
		num_icons--;
	}

	num_unused_icons = 0; 

	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if (item->Status() == ITEM_OWNED)
		{
			stats.iItem = LookUpItemIndex(item);
			if (stats.iItem != -1)
			{
				stats.iImage = this->AddIcon(item->IconBits());
				ListView_SetItem(hwnd_listviews[INVENTORY_TAB], &stats);
			}
		}
	actors->IterateItems(DONE);

	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
	{
		if ((n->Avatar().Hidden()) && (player->ID() != n->ID()))
			continue;
		stats.iItem = LookUpNeighborIndex(n);
		if (stats.iItem != -1)
		{
			stats.iImage = this->AddIcon(n->IconBits());
			ListView_SetItem(hwnd_listviews[NEIGHBORS_TAB], &stats);
		}
	}
	actors->IterateNeighbors(DONE);

	ListView_RedrawItems(hwnd_listviews[tab_mode], 0, MAX_LV_ITEMS);
	UpdateWindow(hwnd_listviews[tab_mode]);		
}


void cControlPanel::RemoveIcon(int index)
{
	num_unused_icons++;
	if ((num_unused_icons == MAX_UNUSED_ICONS) && !exiting)
		this->CompactImageList();
	return;
}

// return rect pointer for selected stat

RECT* cControlPanel::GetSelectedStatRect()
{
	return &(cp_stats[player->SelectedStat()].rect[cDD->Res()]);
}

// Destructor
cControlPanel::~cControlPanel(void)
{
	int i = 0;
	for (i=0; i<NUM_TABS; i++)
		if (NULL != cp_tab_bitmap[i])
			DeleteObject(cp_tab_bitmap[i]);

	for (i=0; i<NUM_LISTVIEWS; i++)
		for (int j=0; j<NUM_LV_BUTTONS; j++)
			for (int k=0; k<2; k++)
				if (NULL != listview_buttons_bitmaps[i][j][k])
					DeleteObject(listview_buttons_bitmaps[i][j][k]);

		
	for (i=0; i<2; i++)
	{
		if (left_bitmap[i])
			DeleteObject(left_bitmap[i]);
		if (right_bitmap[i])
			DeleteObject(right_bitmap[i]);
		if (use_bitmap[i])
			DeleteObject(use_bitmap[i]);
		if (drop_bitmap[i])
			DeleteObject(drop_bitmap[i]);
		if (meta_bitmap[i])
			DeleteObject(meta_bitmap[i]);
		if (give_bitmap[i])
			DeleteObject(give_bitmap[i]);
	}
    if (cp_window_bitmap)
		DeleteObject(cp_window_bitmap);
    if (cp_avatar_bitmap)
		DeleteObject(cp_avatar_bitmap);
	if (cp_bullet_bitmap)
		DeleteObject(cp_bullet_bitmap);

	if (image_list)
		ImageList_Destroy(image_list);

	return;
}

#ifdef CHECK_INVARIANTS
void cControlPanel::CheckInvariants(int line, TCHAR *file)
{

}
#endif


// Subclassed window procedure for the control panel window.

LRESULT WINAPI ControlPanelWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	static WNDPROC lpfn_inventory = NULL;
	static WNDPROC lpfn_neighbors = NULL;
	static WNDPROC lpfn_arts = NULL;
	static HWND hwnd_listviews[NUM_LISTVIEWS] = {NULL, NULL, NULL};
	RECT inv_rect, icon_rect;
	POINT cursor_pos;
	POINT view_pos;
	LPNMHDR notify;
	NM_LISTVIEW *stats;
	LV_ITEM new_selected;
    LPDRAWITEMSTRUCT lpdis; 
    HDC dc; 
	int x,i;

	switch(message)
	{   // the lpfn's and hwnd's need to be passed in so we can use
		// them even before the cp is fully constructed
		case WM_MOUSEWHEEL: // pass off to be handled elsewhere so people can scroll even when an item is selected in the listview
			Realm_OnMouseWheelScroll(hwnd, LOWORD(lParam), HIWORD(lParam), (short)HIWORD(wParam));
			break;	
		case WM_DESTROY:
			break;
		case WM_PASS_INV_PROC:
			lpfn_inventory = (WNDPROC) lParam;
			return (LRESULT) 0;
		case WM_PASS_WHO_PROC:
			lpfn_neighbors = (WNDPROC) lParam;
			return (LRESULT) 0;
		case WM_PASS_ARTS_PROC:
			lpfn_arts = (WNDPROC) lParam;
			return (LRESULT) 0;
		case WM_PASS_INV_HWND:
			hwnd_listviews[INVENTORY_TAB] = (HWND) lParam;
			return (LRESULT) 0;
		case WM_PASS_WHO_HWND:
			hwnd_listviews[NEIGHBORS_TAB] = (HWND) lParam;
			return (LRESULT) 0;
		case WM_PASS_ARTS_HWND:
			hwnd_listviews[ARTS_TAB] = (HWND) lParam;
			return (LRESULT) 0;
		case WM_CTLCOLORSTATIC: // set color of Static controls
			dc = (HDC)wParam;
			SetBkMode(dc, TRANSPARENT);
			for (i=0; i<NUM_PLAYER_STATS; i++)
				if (cp->hwnd_stats[i] == (HWND)lParam)
				{ // color stat according to player condition
					SetTextColor(dc, cp->LookUpStatColor(i));
					return ((LRESULT)(GetStockObject(NULL_BRUSH))); 
				}
			SetTextColor(dc, ORANGE); // default
			return ((LRESULT)(GetStockObject(NULL_BRUSH))); 
		case WM_NOTIFY:

			// *important*: don't pass on notify messages to original
			// window proc, as they may come after the window is destroyed
			notify = (LPNMHDR) lParam;
			switch (notify->code)
			{
				case LVN_ITEMCHANGED:
					stats = (NM_LISTVIEW*) lParam;
					if ((stats->uChanged & LVIF_STATE) && !exiting)
					{
						if ((stats->uNewState & LVIS_SELECTED) && !(stats->uOldState & LVIS_SELECTED))
						{  // look up item pointer and select it
							new_selected.mask = LVIF_PARAM;
							new_selected.iItem = stats->iItem;
							if (cp->last_select_by_click)
								cp->SetSelectionMade(true);
							ListView_GetItem(notify->hwndFrom, &new_selected);
							if (notify->hwndFrom == hwnd_listviews[INVENTORY_TAB])
								cp->SetSelectedItem((cItem*)new_selected.lParam);
							else if (notify->hwndFrom == hwnd_listviews[NEIGHBORS_TAB])
								cp->SetSelectedNeighbor((cNeighbor*)new_selected.lParam);
							else if (notify->hwndFrom == hwnd_listviews[ARTS_TAB])
								cp->SetSelectedArt((int)new_selected.lParam);
						}
					}
					return (LRESULT)0;
				case LVN_BEGINDRAG:
					if (cp->Mode() == INVENTORY_TAB)
					{
						stats = (NM_LISTVIEW*) lParam;
						ListView_GetItemRect(hwnd_listviews[INVENTORY_TAB], stats->iItem, &icon_rect, LVIR_BOUNDS); 
						ListView_GetOrigin(hwnd_listviews[INVENTORY_TAB], &view_pos); 
						GetWindowRect(hwnd_listviews[INVENTORY_TAB], &inv_rect);
						GetCursorPos(&cursor_pos);
						cp->StartDrag(cp->LookUpItem(stats->iItem),	
						cursor_pos.x - inv_rect.left - GetSystemMetrics(SM_CXDLGFRAME) - (icon_rect.left - view_pos.x), 
						cursor_pos.y - inv_rect.top - GetSystemMetrics(SM_CYDLGFRAME) - (icon_rect.top - view_pos.y));
					}
					return (LRESULT)0;
				default:
					return (LRESULT)0;
			}
			return (LRESULT) 0;
		 case WM_DRAWITEM: 
            lpdis = (LPDRAWITEMSTRUCT) lParam; 
            dc = CreateCompatibleDC(lpdis->hDC); 

			if ((lpdis->hwndItem == cp->hwnd_use) && (lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->use_bitmap[0]); 
			else if ((lpdis->hwndItem == cp->hwnd_use) && !(lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->use_bitmap[1]); 
			else if ((lpdis->hwndItem == cp->hwnd_drop) && (lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->drop_bitmap[0]); 
			else if ((lpdis->hwndItem == cp->hwnd_drop) && !(lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->drop_bitmap[1]); 
			else if ((lpdis->hwndItem == cp->hwnd_meta) && (lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->meta_bitmap[0]); 
			else if ((lpdis->hwndItem == cp->hwnd_meta) && !(lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->meta_bitmap[1]); 
			else if ((lpdis->hwndItem == cp->hwnd_left) && (lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->left_bitmap[0]); 
			else if ((lpdis->hwndItem == cp->hwnd_left) && !(lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->left_bitmap[1]); 
			else if ((lpdis->hwndItem == cp->hwnd_right) && (lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->right_bitmap[0]); 
			else if ((lpdis->hwndItem == cp->hwnd_right) && !(lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->right_bitmap[1]); 
			else if ((lpdis->hwndItem == cp->hwnd_give) && (lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->give_bitmap[0]); 
			else if ((lpdis->hwndItem == cp->hwnd_give) && !(lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->give_bitmap[1]); 
			else if ((lpdis->hwndItem == cp->hwnd_usepp) && (lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->usepp_bitmap[0]); 
			else if ((lpdis->hwndItem == cp->hwnd_usepp) && !(lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->usepp_bitmap[1]); 
			else if ((lpdis->hwndItem == cp->hwnd_grantpp) && (lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->grantpp_bitmap[0]); 
			else if ((lpdis->hwndItem == cp->hwnd_grantpp) && !(lpdis->itemState & ODS_SELECTED)) 
                SelectObject(dc, cp->grantpp_bitmap[1]); 

			else // loop through listview button bitmaps
			{
				for (int i=0; i<NUM_LISTVIEWS; i++)
					for (int j=0; j<NUM_LV_BUTTONS; j++)
					{
						if ((lpdis->hwndItem == cp->hwnd_listview_buttons[i][j]) && (lpdis->itemState & ODS_SELECTED)) 
						    SelectObject(dc, cp->listview_buttons_bitmaps[i][j][0]); 
						else if ((lpdis->hwndItem == cp->hwnd_listview_buttons[i][j]) && !(lpdis->itemState & ODS_SELECTED)) 
						    SelectObject(dc, cp->listview_buttons_bitmaps[i][j][1]); 
					}
			}
		
			BitBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top,  
					lpdis->rcItem.right - lpdis->rcItem.left, 
					lpdis->rcItem.bottom - lpdis->rcItem.top, 
					dc, 0, 0, SRCCOPY); 

			DeleteDC(dc);
            return TRUE; 
		case WM_COMMAND:
			if ((HWND)lParam == cp->hwnd_use) 
			{ // pmares can use some arts
#ifdef PMARE	
				if (cp->Mode() != ARTS_TAB)
				{
					player->NightmareAttack(); 
					break;
				}
#endif
				if (cp->Mode() == ARTS_TAB)
				{
					if ((cp->SelectedArt() != Arts::NONE) && (player->Skill(cp->SelectedArt())))
					{
						cp->SetUsing(true);
						arts->BeginArt(cp->SelectedArt());
						cp->SetUsing(false);
					}
					else 
					{
						LoadString (hInstance, IDS_ART_SELECT, disp_message, sizeof(disp_message));
						display->DisplayMessage (disp_message);
						cp->SetSelectedArt(Arts::NONE);
					}
				}
				else
				{
					if ((cp->SelectedItem() != NO_ITEM) && (actors->ValidItem(cp->SelectedItem()) &&
						(cp->SelectedItem()->Status() == ITEM_OWNED)))
							cp->SelectedItem()->Use();
					else 
					{
						LoadString (hInstance, IDS_ITEM_SELECT_USE, disp_message, sizeof(disp_message));
						display->DisplayMessage (disp_message);
						cp->SetSelectedItem(NO_ITEM);
					}
				}
				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			}
			else if (((HWND)lParam == cp->hwnd_drop) && (cp->Mode() == INVENTORY_TAB))
			{
				if (cp->SelectedItem() != NO_ITEM)
				{
					if (player->flags & ACTOR_PARALYZED)
					{
						LoadString (hInstance, IDS_NODROP, disp_message, sizeof(disp_message));
						display->DisplayMessage (disp_message);
					} 
					else
					{
						cp->SelectedItem()->Drop(player->x, player->y, player->angle);
					}
				}
				else
				{
					LoadString (hInstance, IDS_ITEM_SELECT_DROP, disp_message, sizeof(disp_message));
					display->DisplayMessage (disp_message);
				}
				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());			
			}
			else if (((HWND)lParam == cp->hwnd_give) && (cp->Mode() == INVENTORY_TAB))
			{
				if (cp->SelectedItem() == NO_ITEM)
				{
					LoadString (hInstance, IDS_ITEM_SELECT_GIVE, disp_message, sizeof(disp_message));
					display->DisplayMessage (disp_message);
				}
				else
				{
					cp->SetGiving(true);
					arts->BeginArt(Arts::GIVE);
					cp->SetGiving(false);
				}
				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());			
			}
			else if ((HWND)lParam == cp->hwnd_meta)
			{
				if (!metadlg)
				{
					metadlg = TRUE;
					CreateLyraDialog(hInstance, IDD_META, 
						cDD->Hwnd_Main(), (DLGPROC)MetaDlgProc);
				}
			}
			else if ((HWND)lParam == cp->hwnd_usepp)
			{
				if (gs && !gs->LoggedIntoLevel())
				{
					LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					return false;
				}

				bool canusepp = true;
#if (defined PMARE) || defined (AGENT)
				canusepp = false;
#endif
				if (!canusepp) 
				{
					LoadString (hInstance, IDS_NO_PP_USE, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
				} else if (!pp.in_use)
				{
					pp.in_use = true;
					CreateLyraDialog(hInstance, IDD_USE_PPOINT, 
						cDD->Hwnd_Main(), (DLGPROC)UsePPointDlgProc);
				}

			}
			else if ((HWND)lParam == cp->hwnd_grantpp)
			{
				bool canusepp = true;
#if (defined PMARE) || defined (AGENT)
				canusepp = false;
#endif
				if (canusepp) {
					arts->BeginArt(Arts::GRANT_PPOINT);
					SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
				} else {
					LoadString (hInstance, IDS_NO_PP_USE, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
				}
			}
			else if ((HWND)lParam == cp->hwnd_left)
			{
				cp->TurnAvatar(+1);
				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			}
			else if ((HWND)lParam == cp->hwnd_right)
			{
				cp->TurnAvatar(-1);
				SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			}
			else
			{
				for (int j=0; j<NUM_LV_BUTTONS; j++)
				{
					if ((HWND)lParam == cp->hwnd_listview_buttons[cp->Mode()][j])
					{
						int scroll_amount = 1;
						if ((j == DDOWN) || (j == DUP))
							scroll_amount = MAX_LV_ITEMS - 1;
					    switch (cp->Mode())
						{
							case INVENTORY_TAB:
								if ((j == DDOWN) || (j == DOWN))
									cp->ShowNextItem(scroll_amount);
								else
									cp->ShowPrevItem(scroll_amount);
								break;
							case NEIGHBORS_TAB:
								if ((j == DDOWN) || (j == DOWN))
									cp->ShowNextNeighbor(scroll_amount);
								else
									cp->ShowPrevNeighbor(scroll_amount);
								break;
							case ARTS_TAB:
								if ((j == DDOWN) || (j == DOWN))
									cp->ShowNextArt(scroll_amount);
								else
									cp->ShowPrevArt(scroll_amount);
								break;
						}
						SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
					}
				}
			}
			break;
		case WM_MOUSEMOVE:
			if (player->flags & (ACTOR_DRUNK | ACTOR_SCARED | ACTOR_PARALYZED))
				return (LRESULT) 0;
			if (mouse_look.looking || mouse_move.moving)
			{
				StopMouseMove();
				StopMouseLook();
			}
			break;
		case WM_LBUTTONDBLCLK:
			if ((hwnd == cp->hwnd_listviews[INVENTORY_TAB]) || (hwnd == cp->hwnd_listviews[ARTS_TAB]))
				SendMessage(cp->Hwnd_CP(), WM_COMMAND, 0,	(LPARAM)cp->Hwnd_Use());
#ifndef PMARE
			else if ((hwnd == cp->hwnd_listviews[NEIGHBORS_TAB]) && !talkdlg && (cp->SelectedNeighbor() != NO_ACTOR))
			{	
				// no whispers in thresh
//#ifndef GAMEMASTER
//				if (level->ID() == 20)
				//{
					//LoadString (hInstance, IDS_NO_WHISPER_THRESH, disp_message, sizeof(disp_message));
					//display->DisplayMessage(disp_message);
				//}
				//else
//#endif
				{
					talkdlg = TRUE;
					HWND hDlg = CreateLyraDialog(hInstance, IDD_TALK, 
							 cDD->Hwnd_Main(), (DLGPROC)TalkDlgProc);
					Button_SetCheck(GetDlgItem(hDlg, IDC_WHISPER), 1);
					Button_SetCheck(GetDlgItem(hDlg, IDC_TALK), 0);
					return (LRESULT)0;
				}
			}
#endif
			else if ((hwnd == cp->hwnd_avatar) && !avatardlg && 
					 gs && gs->LoggedIntoGame() && !(player->flags & ACTOR_TRANSFORMED))
			{
					avatardlg = TRUE;
					if ((player->GetAccountType() == LmAvatar::ACCT_PMARE) || (player->GetAccountType() == LmAvatar::ACCT_DARKMARE))
						CreateLyraDialog(hInstance, IDD_MONSTER_AVATAR, 
							 cDD->Hwnd_Main(), (DLGPROC)MonsterAvatarDlgProc);
					else
						CreateLyraDialog(hInstance, IDD_AVATAR, 
							 cDD->Hwnd_Main(), (DLGPROC)AvatarDlgProc);
			}
			break;

		case WM_LBUTTONDOWN:
			if ((hwnd == cp->hwnd_avatar) && !avatardlg && 
					 gs && gs->LoggedIntoGame() && !(player->flags & ACTOR_TRANSFORMED))
			{
					avatardlg = TRUE;
					if ((player->GetAccountType() == LmAvatar::ACCT_PMARE) || (player->GetAccountType() == LmAvatar::ACCT_DARKMARE))
						CreateLyraDialog(hInstance, IDD_MONSTER_AVATAR, 
							 cDD->Hwnd_Main(), (DLGPROC)MonsterAvatarDlgProc);
					else
						CreateLyraDialog(hInstance, IDD_AVATAR, 
							 cDD->Hwnd_Main(), (DLGPROC)AvatarDlgProc);
			} 
			else if (hwnd == cp->hwnd_tab)
			{ // determine which tab was clicked on
				x = (int)(short)LOWORD(lParam);
				for (i=0; i<NUM_TABS; i++)
				{
					if (x < ((i+1)*(tabPos[cDD->Res()].width)/NUM_TABS))
					{
						cp->SetMode(i, false, true);
						break;
					}
				}
				cDS->PlaySound(LyraSound::MENU_CLICK);
			} 
#if 0			
			// PARALYSIS ONLY
			if (player->flags & (/*ACTOR_DRUNK | ACTOR_SCARED |*/ ACTOR_PARALYZED))
				return (LRESULT) 0;
#endif
			if ((hwnd == cp->hwnd_listviews[INVENTORY_TAB]) || (hwnd == cp->hwnd_listviews[NEIGHBORS_TAB]) || (hwnd == cp->hwnd_listviews[ARTS_TAB]))
				cp->last_select_by_click = true;

			if (hwnd == cp->hwnd_cp)
			{  // could be changing the selected stat
				cursor_pos.x = (int)(short)LOWORD(lParam);
				cursor_pos.y = (int)(short)HIWORD(lParam);
				//printf("Clicked at %d, %d\n", cursor_pos.x, cursor_pos.y);
				for (i=0; i<NUM_PLAYER_STATS; i++)
					if (PtInRect(&(cp_stats[i].rect[cDD->Res()]),cursor_pos))
					{
						InvalidateRect(cp->hwnd_cp, cp->GetSelectedStatRect(), FALSE);
						player->SetSelectedStat(i);
						InvalidateRect(cp->hwnd_cp, &(cp_stats[i].rect[cDD->Res()]), FALSE);
						cDS->PlaySound(LyraSound::MENU_CLICK);
					}
			}
			SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			break;
	

		case WM_LBUTTONUP:
			if (cp->DragItem() != NO_ITEM)
			{
				int x; int y;
				x = LOWORD(lParam); y = HIWORD(lParam);
				cp->EndDrag();
			}
			//SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			break;
		case WM_RBUTTONUP:
			if ((hwnd == cp->hwnd_listviews[INVENTORY_TAB]) && (cp->Mode() == INVENTORY_TAB))
			{ // identify item clicked on
				i = cp->FindClosestEntry(hwnd_listviews[INVENTORY_TAB]);
				if (i != -1)
					cp->LookUpItem(i)->RightClick();
			} else if ((hwnd == cp->hwnd_listviews[NEIGHBORS_TAB]) && (cp->Mode() == NEIGHBORS_TAB))
			{
				cNeighbor *n = cp->SelectedNeighbor();
#if defined PMARE // pmares only see descriptions of other mares
				if (n && ((n->GetAccountType() != LmAvatar::ACCT_DREAMER) && (n->GetAccountType() != LmAvatar::ACCT_ADMIN)))
					gs->GetAvatarDescrip(n->ID());
#else
				if (n)
				  gs->GetAvatarDescrip(n->ID());
#endif
			}

		case WM_RBUTTONDOWN:
			//PARALYZE ONLY
#if 0
			if (player->flags & (/* ACTOR_DRUNK | ACTOR_SCARED | */ACTOR_PARALYZED))
				return (LRESULT) 0;
#endif
			SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			break;
		case WM_KEYDOWN: // send the character + the focus back to the main window
		case WM_KEYUP:
		case WM_CHAR: 
			SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			SendMessage(cDD->Hwnd_Main(), message,
				(WPARAM) wParam, (LPARAM) lParam);
			return (LRESULT) 0;
		case WM_PAINT:
			if (cp->HandlePaint(hwnd))
				return 0L; 
		case WM_WINDOWPOSCHANGED:
			if (cp)
			{
				RECT cpr;
				GetWindowRect(cp->Hwnd_CP(), &cpr);
				if (cpr.top != mainPos[cDD->Res()].y) {
					display->DisplayMessage("Moving CP to proper alignment\n", false);
					MoveWindow(cp->Hwnd_CP(), mainPos[cDD->Res()].x, mainPos[cDD->Res()].y, mainPos[cDD->Res()].width, mainPos[cDD->Res()].height, true);
				}
			}
		break;
	}  

	if (hwnd == hwnd_listviews[INVENTORY_TAB])
		return CallWindowProc( lpfn_inventory, hwnd, message, wParam, lParam);
	else if (hwnd == hwnd_listviews[NEIGHBORS_TAB])
		return CallWindowProc( lpfn_neighbors, hwnd, message, wParam, lParam);
	else if (hwnd == hwnd_listviews[ARTS_TAB])
		return CallWindowProc( lpfn_arts, hwnd, message, wParam, lParam);
	else 
		return DefWindowProc(hwnd, message, wParam, lParam);
} 


/////////////////////////////////////////////////////////////////
// Helper Functions

// callback used to compare items for sorting in the listview
int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	cItem *item1 = (cItem*)lParam1;
	cItem *item2 = (cItem*)lParam2;
	int index1 = item1->SortIndex();
	int index2 = item2->SortIndex();

	if (index1 < index2)
		return -1;
	else if (index2 < index1)
		return 1;
	else 
		return    (_tcscmp(item1->Name(), item2->Name()));
}

// callback used to compare items for sorting in the listview
int CALLBACK CompareNeighbors(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{

	lyra_id_t id1, id2;
	int et1, et2;
	bool party1, party2;
	
	et1 = ((cNeighbor*)lParam1)->EnterTime();
	et2 = ((cNeighbor*)lParam2)->EnterTime();

	id1 = ((cNeighbor*)lParam1)->ID();
	id2 = ((cNeighbor*)lParam2)->ID();

	if (gs && gs->LoggedIntoGame() && gs->LoggedIntoLevel())
	{
		party1 = gs->Party()->IsInParty(id1);
		party2 = gs->Party()->IsInParty(id2);
	} 
	else
	{
		party1 = party2 = false;
	}

	/*
	old way - compare ID's, enter time 
	// put the player on top, for arts selection
	if (id1 == player->ID())
		return -1;
	else if (id2 == player->ID())
		return 1;
	else if (et1 < et2)
		return -1;
	else if (et2 < et1)
		return 1;
	else 
		return 0; */

	// put the player on top, for arts selection
	// put party members after player
	// put new arrivals at the bottom
	// sort everyone else alphabetically

	if (id1 == player->ID())
		return -1;
	else if (id2 == player->ID())
		return 1;
	else if (party1 && !party2)
		return -1;
	else if (!party1 && party2)
		return 1;
	else if (et1 < et2)
		return -1;
	else if (et2 < et1)
		return 1;

	TCHAR buf1[64];
	_tcscpy(buf1, ((cNeighbor*)lParam1)->Name());
	TCHAR buf2[64];
	_tcscpy(buf2, ((cNeighbor*)lParam2)->Name());

	return (_tcscmp(buf1, buf2));

}
// callback used to compare arts for sorting in the listview
int CALLBACK CompareArts(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TCHAR buf1[64];
	_tcscpy(buf1, arts->Descrip(lParam1));
	TCHAR buf2[64];
	_tcscpy(buf2, arts->Descrip(lParam2));
	return (_tcscmp(buf1, buf2));
}

// Check invariants

#ifdef DEBUG

void cControlPanel::Debug_CheckInvariants(int caller)
{
	return;
}

#endif


