// Handles subclassing procedures common to both quests and goals

// Copyright Lyra LLC, 2001. All rights reserved. 


#define STRICT
#include "Central.h"
#include <windows.h>
#include "cGoalPosting.h"
#include "cQuestBuilder.h"
#include "cItem.h"
#include "Dialogs.h"
#include "Interface.h"


extern HBITMAP *hGoalCheckButtons;
extern WNDPROC lpfnGoalPushButtonProc;
extern WNDPROC lpfnGoalStateButtonProc;
extern WNDPROC lpfnGoalEditProc;
extern WNDPROC lpfnGoalComboBoxProc;


// NOTE: deprecated due to use of small buttons for all resolutions
void BlitGoalBitmap(HWND hWnd) 
{
	RECT region;
	GetUpdateRect(hWnd, &region, FALSE);
	if ((region.left == 0) && (region.right == 0))
		return; // no update needed

	// find which bitmap to use
	HBITMAP hBitmap = (HBITMAP)SendMessage(hWnd, BM_GETIMAGE, (WPARAM)IMAGE_BITMAP, 0);
	HGDIOBJ old_object;
	HDC dc;
	HDC bitmap_dc;
	PAINTSTRUCT paint;
				
	// do the painting routine
	dc = BeginPaint(hWnd, &paint);
	bitmap_dc = CreateCompatibleDC(dc);
	old_object = SelectObject(bitmap_dc, hBitmap);
		
	//RECT src;  
	//PrepareSrcRect(&src, &region, STRETCH);
	BitBlt(dc, region.left, region.top, (region.right - region.left), 
		(region.bottom - region.top), bitmap_dc, 
		region.left, region.top, 
	//	(src.right - src.left), (src.bottom - src.top),
		SRCCOPY);	
				
	SelectObject(bitmap_dc, old_object);
	DeleteDC(bitmap_dc);
	EndPaint(hWnd, &paint);
			
	return;
}

// Callback for push buttons
BOOL CALLBACK GoalPushButtonProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
		return 0;   

		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR: // pass on keyboard messages to parent
			PostMessage(GetParent(hWnd), Message, wParam, lParam);
			break;

		//case WM_PAINT:
		//	printf("Got paint message!\n");
		//	BlitGoalBitmap(hWnd); 
		//	return TRUE;

		case WM_CAPTURECHANGED:  
			SendMessage(GetParent(hWnd),WM_CAPTURECHANGED, wParam, lParam);
			// Allows parent to set focus to another window
		break;

	}	
	return CallWindowProc(lpfnGoalPushButtonProc, hWnd, Message, wParam, lParam) ;
}


// Callback for edit controls
BOOL CALLBACK GoalEditProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

//	_tprintf(_T("hWnd: %d Parent: %d Message: %d Loword: %d\n"), hWnd, GetParent(hWnd), Message, LOWORD(wParam));

	switch (Message)
	{
		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(GetParent(hWnd), Message, wParam, lParam);
				break;
			}
		case WM_RBUTTONDOWN:
			return 0;

		break;
	}

	return CallWindowProc(lpfnGoalEditProc, hWnd, Message, wParam, lParam) ;
}


// Callback for combo boxes
BOOL CALLBACK GoalComboBoxProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_RBUTTONDOWN:
			return 0;
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR: // pass on keyboard messages to parent
			PostMessage(GetParent(hWnd), Message, wParam, lParam);
			break;
	}

	return CallWindowProc(lpfnGoalComboBoxProc, hWnd, Message, wParam, lParam) ;
}


// Callback for radio and check boxes
BOOL CALLBACK GoalStateButtonProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// Extract pointer to handle array of button text 	
	HBITMAP hButtonText = (HBITMAP)GetWindowLong(hWnd,GWL_USERDATA);

	switch (Message)
	{
		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			return 0;

		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR: // pass on keyboard messages to parent
			PostMessage(GetParent(hWnd), Message, wParam, lParam);
			return 0;

		case WM_PAINT:
		if (hButtonText)
		{	
			HBITMAP *hButtons;
				
			// Determine which bitmaps to used based on button's style
			int style =GetWindowStyle(hWnd);
			switch (style & 0x000F)   // styles are any integer from 0..16 
			{
				case BS_AUTOCHECKBOX:			hButtons = hGoalCheckButtons;	break;
			}

			PAINTSTRUCT Paint; 
			HDC dc= BeginPaint(hWnd,&Paint);

			RECT button_rect, text_rect; 
			GetClientRect(hWnd, &button_rect);
			GetClientRect(hWnd, &text_rect);

			//Determine size of state indicator bitmap
 			int size;
			BITMAP bm;
			GetObject(hButtons[0],sizeof BITMAP, &bm);
			size = bm.bmWidth;

			// Determine where button and text go depending on button style
			if (style & BS_LEFTTEXT)
				button_rect.left = button_rect.right - size;
			else
				text_rect.left += size;
			
			// Blit button depending on check state
			BlitBitmap(dc, hButtons[(Button_GetCheck(hWnd)) ? 0 : 2] ,&button_rect, NOSTRETCH);
			
			// Blit button text
			if (hButtonText)
					BlitBitmap(dc, hButtonText, &text_rect, NOSTRETCH);
			
			EndPaint(hWnd,&Paint);
			return 0;
		}
		else
			break;
	    case BM_SETCHECK:
		case BM_SETSTATE:  // State of button has changed
			CallWindowProc(lpfnGoalStateButtonProc,hWnd, Message, wParam, lParam);
			// Windows paints its own stuff during above call so we MUST repaint
			InvalidateRect(hWnd,NULL,0);
			UpdateWindow(hWnd);  // Repaint 
		return 0;

		case WM_CAPTURECHANGED: // Tell parent we no logner have the focuse
			SendMessage(GetParent(hWnd),WM_CAPTURECHANGED, wParam, lParam);
		}	
	return CallWindowProc(lpfnGoalStateButtonProc,hWnd, Message, wParam, lParam) ;
}


void SubclassGoalWindow(HWND Hwnd)
{
	TCHAR class_name[30];

	GetClassName(Hwnd, class_name, sizeof class_name);

	if (_tcscmp(class_name, _T("Edit")) == 0) 
	{
		lpfnGoalEditProc = SubclassWindow(Hwnd, GoalEditProc);
		return;
	}
	else if (_tcscmp(class_name, _T("ComboBox")) == 0)
	{
		lpfnGoalComboBoxProc = SubclassWindow(Hwnd, GoalComboBoxProc);
		return;
	}

	int	style =GetWindowStyle(Hwnd);
	switch (style & 0x000F)   // styles are any integer from 0..16 
	{
		// window data for radio & checkboxes is the bitmap handle
		case BS_AUTORADIOBUTTON:
		case BS_AUTOCHECKBOX:
			lpfnGoalStateButtonProc = SubclassWindow(Hwnd, GoalStateButtonProc);
		break;

		case BS_PUSHBUTTON:
		case BS_DEFPUSHBUTTON:
			lpfnGoalPushButtonProc = SubclassWindow(Hwnd, GoalPushButtonProc);
		break;

		default:
			break;
	}
}

// convert a scroll_t structure to a quest item
void ScrollToCodexQuest(scroll_t* scroll, quest_item_t* questitem)
{
	memset(questitem, 0, sizeof(quest_item_t));
	_tcscpy(questitem->keywords, scroll->descrip);
}

// convert a cItem* to a quest item
void ItemtoQuestItem(cItem* item, quest_item_t* questitem)
{
	const void* state;
	state = item->Lmitem().StateField(0);
	memset(questitem, 0, sizeof(quest_item_t));
	// we need to retrieve item descrips from the server,
	// so we just blank it out here
	_tcscpy(questitem->keywords, _T(""));
	questitem->charges = item->Lmitem().Charges();
	questitem->color1 = item->Lmitem().Color1();
	questitem->color2 = item->Lmitem().Color2();
	questitem->graphic = item->BitmapID();
	questitem->item_type = item->ItemFunction(0);

	switch (item->ItemFunction(0))
	{
		case LyraItem::CHANGE_STAT_FUNCTION:
		{
			lyra_item_change_stat_t changestat;
			memcpy(&changestat, state, sizeof(changestat));
			questitem->field1 = changestat.stat;
			questitem->field2 = changestat.modifier;
			break;
		}
		case LyraItem::MISSILE_FUNCTION:
		{
			lyra_item_missile_t missile;
			memcpy(&missile, state, sizeof(missile));
			questitem->field1 = missile.velocity;
			questitem->field2 = missile.effect;
			questitem->field3 = missile.damage;
			break;
		}
		case LyraItem::EFFECT_PLAYER_FUNCTION:
		{
			lyra_item_effect_player_t effectplayer;
			memcpy(&effectplayer, state, sizeof(effectplayer));
			questitem->field1 = effectplayer.effect;
			questitem->field2 = effectplayer.duration;
			break;
		}

		case LyraItem::ARMOR_FUNCTION:
		{
			lyra_item_armor_t armor;
			memcpy(&armor, state, sizeof(armor));
			questitem->field1 = armor.curr_durability;
			questitem->field2 = armor.max_durability;
			questitem->field3 = armor.absorption;
			break;
		}
	}
	return;
}



bool DoesTalismanSatisfyQuest(cItem* item, cGoal* quest)
{
	quest_item_t candidate;

	ItemtoQuestItem(item, &candidate);

	if (candidate.charges >= quest->Charges())
		if ((candidate.color1 == quest->Color1()) ||
		    (quest->Color1() == ANY_COLOR)) 
			if ((candidate.color2 == quest->Color2()) ||
				(quest->Color2() == ANY_COLOR))
					if ((candidate.graphic == quest->Graphic()) ||
						(quest->Graphic() == LyraBitmap::NONE)) // ANY_GRAPHIC
					{
						if (quest->ItemType() == LyraItem::NO_FUNCTION)
							return true;
						else if (candidate.field1 == quest->Field1())
							if (candidate.field2 == quest->Field2())
								if (candidate.field3 == quest->Field3())
									if (candidate.item_type == quest->ItemType())
										return true;
					}
	return false;
}