// Mouse: mouse handling routines

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "4dx.h"
#include "cActorList.h"
#include "cDDraw.h"
#include "cEffects.h"
#include "cControlPanel.h"
#include "Utils.h" 
#include "Options.h"
#include "cPlayer.h"
#include "cChat.h"
#include "cOutput.h"
#include "cKeymap.h"
#include "Realm.h"
#include "Resource.h"
#include "Mouse.h"
#include "keyboard.h"

#include "cLevel.h"
#include "Move.h"
#include <string>
#include <Mouse/MouseClass.h>

/////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cDDraw *cDD; 
extern cChat *display;
extern cActorList *actors;
extern cPlayer *player;
extern cControlPanel *cp;
extern options_t options;
extern cKeymap *keymap; 
extern mouse_move_t mouse_move; 
extern mouse_look_t mouse_look; 
extern bool mouselooking;
extern cEffects *effects; 
extern unsigned char keyboard[num_keystates]; 

extern bool showing_map;
extern bool map_shows_current_level;

extern cChat *display; // For mousewheel scrolling


/////////////////////////////////////////////////
// Local Global Variables


// Mouse Handlers

void  Realm_OnLButtonDown( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	CancelExit();

	// If in mouse mode, the mouse button may be mapped...
	if (mouselooking && (keymap->FindMapping(VK_LBUTTON) != MAPPING_NOT_FOUND))
	{
		PostMessage(cp->Hwnd_CP(), WM_KEYDOWN, VK_LBUTTON, 0);
		return;
	}
	
	if (hWnd == cDD->Hwnd_Main())
	{
		if (GetFocus() != cDD->Hwnd_Main())
		{
			SetActiveWindow(cDD->Hwnd_Main());
			SetFocus(cDD->Hwnd_Main());
		}


		bool click_used = false;
	  cActor *chosen_actor = ActorAtScreenPoint(x,y);
		if (chosen_actor != NO_ACTOR) 
			click_used = chosen_actor->LeftClick();
		if (!click_used)
			StartMouseMove(x,y);
	}
}

void RenderView();

static bool CheckPortalClicking(int x, int y)
{
#ifndef AGENT
	SetPickScreenCo_ords(x,y);
	RenderView();
	linedef *l = GetPickLinedef();

	if (l && l->TripFlags & (TRIP_TELEPORT|TRIP_LEVELCHANGE))
	{
		if (l->TripFlags & TRIP_TELEPORT)
		{
			int sector = FindSector((float)l->trip1, (float)l->trip2 ,DEAD_SECTOR, true);
			if (sector != DEAD_SECTOR)
			{
				LoadString (hInstance, IDS_TELEPORTAL, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, level->RoomName(level->Sectors[sector]->room));
				display->DisplayMessage (message);
				return true;
			}
		}
		else
		{
			LoadString (hInstance, IDS_LEVELPORTAL, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, level->Name(l->trip4 & 0xFF) );
			display->DisplayMessage (message);
			return true;
		}
	}
#endif
	return false;

}
		
void  Realm_OnRButtonDown( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	CancelExit();

	// If in mouse mode, the mouse button may be mapped...
	if (mouselooking && (keymap->FindMapping(VK_RBUTTON) != MAPPING_NOT_FOUND))
	{
		PostMessage(cp->Hwnd_CP(), WM_KEYDOWN, VK_RBUTTON, 0);
		return;
	}

	if (hWnd == cDD->Hwnd_Main())
	{
		if (showing_map)
		{
		   if (map_shows_current_level)
           {	
				int r = FindRoomOnMap(x,y);
				if (r)
				{
					LoadString (hInstance, IDS_MAP_AREA, disp_message, sizeof(disp_message));
					_stprintf(message,disp_message, level->RoomName(r));
					display->DisplayMessage(message);
				}
		   }
		   else
           {
				LoadString (hInstance, IDS_CANNOT_TELL_MAP_AREA, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message);
		   }
 		}
		else
		{

			if (player->flags & ACTOR_BLINDED)
			{
				LoadString (hInstance, IDS_BLINDED_NO_RCLICK, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
				return;
			}
			bool click_used = false;
			cActor *chosen_actor = ActorAtScreenPoint(x,y);
			if (chosen_actor != NO_ACTOR) 
				click_used = chosen_actor->RightClick();
			if (!click_used)
				click_used = CheckPortalClicking(x,y);
			if (!click_used)
				StartMouseLook(x,y);
		}
	}
}


void  Realm_OnMButtonDown( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	PostMessage(cp->Hwnd_CP(), WM_KEYDOWN, VK_MBUTTON, 0);
	return;
}


void  Realm_OnKillFocus(HWND hWnd, HWND hWndNewFocus)
{ 

// clear out the keyboard buffer so we don't keep moving
   memset(keyboard, 0, num_keystates);

}

void  Realm_OnLButtonUp(HWND hWnd, WORD x, WORD y, WORD fwKeys )
{ 

	// If in mouse mode, the mouse button may be mapped...
	if (mouselooking && (keymap->FindMapping(VK_LBUTTON) != MAPPING_NOT_FOUND))
	{
		PostMessage(cp->Hwnd_CP(), WM_KEYUP, VK_LBUTTON, 0);
		return;
	}
	StopMouseMove();
}


void  Realm_OnMButtonUp(HWND hWnd, WORD x, WORD y, WORD fwKeys )
{ 
	PostMessage(cp->Hwnd_CP(), WM_KEYUP, VK_MBUTTON, 0);
	return;
}

void Realm_OnMouseWheelScroll(HWND hWnd, int x, int y, int direction)
{
	POINT pt;
	pt.x = x; pt.y = y;
	RECT rect;
	if (!cp)
		return;

	GetWindowRect(cp->Hwnd_CP(), &rect);
	
	if (PtInRect(&rect, pt))
	{

		if (direction > 0) // Scrolled up
		{

			switch (cp->Mode())
			{
			case INVENTORY_TAB:
				
				cp->ShowPrevItem(2);
				break;
			case NEIGHBORS_TAB:
				cp->ShowPrevNeighbor(2);
				break;
			case ARTS_TAB:
				cp->ShowPrevArt(2);
				break;
			default:
				break;
			}
	
		}

		else // Scrolled down
		{
			switch (cp->Mode())
			{
			case INVENTORY_TAB:
				cp->ShowNextItem(2);
				break;
			case NEIGHBORS_TAB:
				cp->ShowNextNeighbor(2);
				break;
			case ARTS_TAB:
				cp->ShowNextArt(2);
				break;
			default:
				break;
			}

		}

	} // End if PtInRect
	else
	{
		GetWindowRect(display->Hwnd(), &rect);
		if (PtInRect(&rect, pt)) // Scroll the chat window
		{

			if (direction > 0) // scrolled up
				display->ScrollUp(5);
			else
				display->ScrollDown(5);

		}
	}
}

void  Realm_OnRButtonUp( HWND hWnd,WORD x, WORD y, WORD fwKeys )
{

	// If in mouse mode, the mouse button may be mapped...
	if (mouselooking && (keymap->FindMapping(VK_RBUTTON) != MAPPING_NOT_FOUND))
	{
		PostMessage(cp->Hwnd_CP(), WM_KEYUP, VK_RBUTTON, 0);
		return;
	}

/*	cActor *chosenActor;

	hWnd = HWND(hWnd);
	if (hWnd == cDD->Hwnd_Main())
	{
		chosenActor = aBox->FindBox(x,y);
		if ((actors->ValidActor(chosenActor)) && (chosenActor == r_clicked))
			chosenActor->RightClick();
	}
*/
	//player->ResetEyeHeight();
	StopMouseLook();
}


static MouseClass mouse;
void  Realm_OnMouseMove(HWND hWnd, WORD x, WORD y, WORD fwKeys)
{
	
	//mouse.OnMouseMove(x, y);

	if (mouselooking)
	{
		StopMouseLook();
		StartMouseLook(x,y);
		return;
	}
	else if (fwKeys & MK_LBUTTON) 
	{
	 if (mouse_move.moving)
		{
			StopMouseMove();
			StartMouseMove(x,y);
		}
	} 
	
	if ((fwKeys & MK_RBUTTON) && mouse_look.looking)
	{
		StopMouseLook();
		StartMouseLook(x,y);
		return;
	}
}


static void StartMouseMove(int x, int y)
{	
	mouse_move.moving = true;

	if (x >= cDD->ViewX() || y >= cDD->ViewY())  
		 return;
	
	int  y_ratio = (y*5)/cDD->ViewY();	
	switch (y_ratio)
	{
  		case 0: mouse_move.forward = mouse_move.shift = true; break;
		case 1: mouse_move.forward = true; break;
		case 3: mouse_move.backward = true; break;
		case 4: mouse_move.backward = mouse_move.shift = true; break;
	}
	mouse_move.xratio = 1.0f;
	int  x_ratio = (x*17)/cDD->ViewX();	
	switch (x_ratio)
	{
  	case 0: mouse_move.left = mouse_move.shift = true; break;
    case 1: mouse_move.left = true; break;
	case 2: mouse_move.left = true; mouse_move.xratio = 0.65f; break;
	case 3: mouse_move.left = true; mouse_move.xratio = 0.4f; break;
	case 4: mouse_move.left = true; mouse_move.xratio = 0.25f; break;
	case 5: mouse_move.left = true; mouse_move.xratio = 0.1f; break;

	case 11: mouse_move.right = true; mouse_move.xratio = 0.1f; break;
	case 12: mouse_move.right = true; mouse_move.xratio = 0.25f; break;
	case 13: mouse_move.right = true; mouse_move.xratio = 0.4f; break;
    case 14: mouse_move.right = true; mouse_move.xratio = 0.65f; break;
	case 15: mouse_move.right = true; break;
	case 16: mouse_move.right = mouse_move.shift = true; break;
	}
	return;
}

void StopMouseMove(void)
{
	mouse_move.forward = mouse_move.backward = mouse_move.left = 
		mouse_move.right = mouse_move.shift = mouse_move.moving = false;
	mouse_move.xratio = 1.0f;
}

void StopMouseLook(void)
{
	mouse_look.up = mouse_look.down = mouse_look.left = 
		mouse_look.right = mouse_look.shift = mouse_look.looking = false;
	mouse_look.xratio = 1.0f;
	mouse_look.yratio = 1.0f;

	mouse_look.looking = false;
}

static void StartMouseLook(int x, int y)
{
	mouse_look.looking = true;


	/* using rawinput relative cords, via mouseclass/mouseevent.
	mouse_look.yratio = 1.0f;
	int  y_ratio = (mouse.GetPosY()*11)/cDD->ViewY();	
	switch (y_ratio)
	{
		case 0: mouse_look.up = true; break;
		case 1: mouse_look.up = true; mouse_look.yratio = 0.5f; break;
		case 2: mouse_look.up = true; mouse_look.yratio = 0.2f; break;

		case 8: mouse_look.down = true; mouse_look.yratio = 0.2f; break;
		case 9: mouse_look.down = true; mouse_look.yratio = 0.5f; break;
		case 10: mouse_look.down = true; break;

	} 

	if ((mouse_look.up || mouse_look.down) && options.invertmouse)
	{
		mouse_look.up = !mouse_look.up;
		mouse_look.down = !mouse_look.down;
	}
	
	mouse_look.xratio = 1.0f;
	int  x_ratio = (mouse.GetPosX()*17)/cDD->ViewX();	

	switch (x_ratio)
	{
  	case 0: mouse_look.left = mouse_look.shift = true; break;
    case 1: mouse_look.left = true; break;
	case 2: mouse_look.left = true; mouse_look.xratio = 0.65f; break;
	case 3: mouse_look.left = true; mouse_look.xratio = 0.4f; break;
	case 4: mouse_look.left = true; mouse_look.xratio = 0.25f; break;
	case 5: mouse_look.left = true; mouse_look.xratio = 0.1f; break;

	case 11: mouse_look.right = true; mouse_look.xratio = 0.1f; break;
	case 12: mouse_look.right = true; mouse_look.xratio = 0.25f; break;
	case 13: mouse_look.right = true; mouse_look.xratio = 0.4f; break;
	
	case 15: mouse_look.right = true; break;
	case 16: mouse_look.right = mouse_look.shift = true; break;
	}*/
	return;
}
