// Interface: utility functions for making the interface pretty

// Copyright Lyra LLC, 1997. All rights reserved.

#define STRICT
#define OEMRESOURCE	// to get defines for windows cursor id's

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "Utils.h"
#include "Resource.h"
#include "cGameServer.h"
#include "Options.h"
#include "SharedConstants.h"
#include "cPlayer.h"
#include "cDDraw.h"
#include "cDSound.h"
#include "Realm.h"
#include "cChat.h"
#include "Dialogs.h"
#include "cOutput.h"
#include "cKeymap.h"
#include "Mouse.h"
#include "cArts.h"
#include "Interface.h"
#include "cEffects.h"

/////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern HINSTANCE hInstance;
extern cPlayer *player;
extern cDDraw *cDD;
extern cDSound *cDS;
extern cArts *arts;
extern cEffects *effects;
extern bool ready;
extern options_t options;
extern cKeymap *keymap;
extern cChat *display;
extern HFONT display_font[MAX_RESOLUTIONS];
extern float scale_x;
extern float scale_y;
const unsigned long lyra_colors[9] = {BLUE, LTBLUE, DKBLUE, LTBLUE, ORANGE, BLUE,
	ORANGE, BLACK, ORANGE};

//////////////////////////////////////////////////////////////////
// Constant Structures

// Pointers  to functions for subclassing
static WNDPROC lpfnPushButtonProc;
static WNDPROC lpfnStateButtonProc; // radio,check buttons
static WNDPROC lpfnStaticTextProc;
static WNDPROC lpfnListBoxProc;
static WNDPROC lpfnComboBoxProc;
static WNDPROC lpfnTrackBarProc;
static WNDPROC lpfnEditProc;
static WNDPROC lpfnGenericProc;
static HBRUSH	blueBrush = NULL;
static HBRUSH	blackBrush = NULL;

// pointer to handles for check and radio button state indicator bitmaps
// i.e the checkbox and round radio button. These are created once on start up
// and deleted at shutdown.
static HBITMAP *hRadioButtons = NULL;
static HBITMAP *hCheckButtons = NULL;
static HBITMAP hBackground = NULL;
static HBITMAP hGoldBackground = NULL;
static HBITMAP hListBoxArrow	= NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Lyra Dialog Manager

// Blits bitmap to dc at the co-ord in region
//void BlitBitmap(HDC dc, HBITMAP bitmap, RECT *region, const window_pos_t& srcregion, int mask)
void BlitBitmap(HDC dc, HBITMAP bitmap, RECT *region, int stretch, int mask)
{
	HGDIOBJ old_object;
	HDC bitmap_dc;

	bitmap_dc = CreateCompatibleDC(dc);
	old_object = SelectObject(bitmap_dc, bitmap);
	//RECT src;

	//PrepareSrcRect(&src, region, stretch);


	BitBlt(dc, region->left, region->top, (region->right - region->left),
			(region->bottom - region->top), bitmap_dc, 0, 0, mask);
	//StretchBlt(dc, region->left, region->top, (region->right - region->left),
	//(region->bottom - region->top), bitmap_dc, 
	//src.left, src.top, (src.right - src.left), (src.bottom - src.top), mask);
	//0, 0, (src.right - src.left), (src.bottom - src.top), mask);

	SelectObject(bitmap_dc, old_object);
	DeleteDC(bitmap_dc);
	return;
}


void TransparentBlitBitmap(HDC dc, int bitmap_id, RECT *region, int stretch, int mask)
{
	HBITMAP hBitmap;
	if (effects->EffectWidth(bitmap_id) == 0 )
		return ;

	effects->LoadEffectBitmaps(bitmap_id, 1);

	hBitmap = effects->CreateBitmap(bitmap_id);
	BlitBitmap(dc, hBitmap, region, stretch, mask);

	DeleteObject(hBitmap);
	effects->FreeEffectBitmaps(bitmap_id);

}

// Creates a windows bitmap from an effects bitmap with given bitmap ID
HBITMAP CreateWindowsBitmap(int bitmap_id)
{
	HBITMAP hBitmap;

	if (effects->EffectWidth(bitmap_id) == 0 )
		return NULL;

	effects->LoadEffectBitmaps(bitmap_id, 2);

	hBitmap = effects->CreateBitmap(bitmap_id);

	effects->FreeEffectBitmaps(bitmap_id);
	return hBitmap;
}

// Creates a windows bitmap, and a grayed version of that,from a game bitmap with given bitmap ID
// Bitmap handles are returned in hBitmaps array, normal first, grayed second
void CreateWindowsBitmaps(int bitmap_id, HBITMAP hBitmaps[] )
{
	if (effects->EffectWidth(bitmap_id ) != 0 )
	{
		effects->LoadEffectBitmaps(bitmap_id, 3);

		int size = effects->EffectWidth(bitmap_id)* effects->EffectHeight(bitmap_id);
		PIXEL *src = (PIXEL *)effects->EffectBitmap(bitmap_id)->address;

		// Normal Version
		hBitmaps[0] = CreateBitmap(effects->EffectWidth(bitmap_id), effects->EffectHeight(bitmap_id), 1,
														BITS_PER_PIXEL,src );

		// Grayed Version
		PIXEL *buffer = new PIXEL[size];

		// Go thru bitmap and create new 'grayed' bitmap
		for (int i = 0; i < size; i++)
		{
			//extract colors
			int red =	(src[i] & 0xF800) >> 11;
			int green = (src[i] & 0x03E0) >> 5;
			int blue =	(src[i] & 0x001F);

			// average the colors and set each color to this
			PIXEL average = ((red+blue+green)/3) & 0x1F;
			buffer[i] = (average << 11) | (average<< 6) | average;
		}

		hBitmaps[1] = CreateBitmap(effects->EffectWidth(bitmap_id), effects->EffectHeight(bitmap_id), 1,
														BITS_PER_PIXEL,buffer );

		delete [] buffer;

		effects->FreeEffectBitmaps(bitmap_id);
	}
	else
	{
		hBitmaps[0] = NULL;
		hBitmaps[1] = NULL;
	}
}

// Create Windows bitmaps for a dialog control from effects bitmaps
// If the control has more than one state e.g a radio button, then is
// assumed there are two effects , one for each state, and that the second effect ID is
// one more than the first.
const int NUM_CONTROL_BITMAPS = 4;
HBITMAP *CreateControlBitmaps( int effect_id, int num_states)
{
	HBITMAP *bitmaps = new HBITMAP [NUM_CONTROL_BITMAPS];
	memset(bitmaps,0, sizeof HBITMAP * NUM_CONTROL_BITMAPS);
	for (int i = 0; i < num_states; i++)
		CreateWindowsBitmaps(effect_id+i, &bitmaps[i*2]);

	// if no bitmaps were actually created, return NULL
	bool success = false;
	for (int i = 0; i < num_states; i++)
		if (bitmaps[i] != NULL)
			success = true;

	if (!success)
	{
		delete bitmaps;
		bitmaps = NULL;
	}

	return bitmaps;
}

// Deletes bitmaps created above
void DeleteControlBitmaps(HBITMAP *bitmaps)
{
	if (!bitmaps)
		return;
	for (int i = 0; (i < NUM_CONTROL_BITMAPS); i++)
		if (bitmaps[i])
			DeleteObject(bitmaps[i]);
	delete bitmaps;
}

//  Resizes a Button Window (Should have been in Windows)
void ResizeButton(HWND hWnd, int new_width, int new_height)
{
	RECT r;
	// determine size of button borders
	int border_x = GetSystemMetrics(SM_CXBORDER) + GetSystemMetrics(SM_CXEDGE);
	int border_y = GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYEDGE);
	GetWindowRect(hWnd,&r);
	MapWindowPoints(NULL,GetParent(hWnd),LPPOINT(&r),2);
	MoveWindow(hWnd, r.left, r.top, new_width + border_x, new_height + border_y,0);
}

//  Resizes a Label Window (Should have been in Windows)
void ResizeLabel(HWND hWnd, int new_width, int new_height)
{
	RECT r;
	GetWindowRect(hWnd,&r);
	MapWindowPoints(NULL,GetParent(hWnd),LPPOINT(&r),2);
	MoveWindow(hWnd, r.left, r.top, new_width, new_height, 0);
}



// Callback for  push buttons
BOOL CALLBACK PushButtonProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// Extract pointer to handle array of button text
	HBITMAP *hButtonText = (HBITMAP *)GetWindowLong(hWnd,GWL_USERDATA);

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

		case WM_ENABLE:
		{
			if ((BOOL)wParam) // If window is being enabled
				SendMessage(hWnd, BM_SETIMAGE, WPARAM (IMAGE_BITMAP), LPARAM (hButtonText[0]));
			else
				SendMessage(hWnd, BM_SETIMAGE, WPARAM (IMAGE_BITMAP), LPARAM (hButtonText[1]));
		}
		break;

		case WM_CAPTURECHANGED:
			SendMessage(GetParent(hWnd),WM_CAPTURECHANGED, wParam, lParam);
			// Allows parent to set focus to another window
		break;

		case WM_DESTROY:
			DeleteControlBitmaps(hButtonText);
		break;

	}
	return CallWindowProc(lpfnPushButtonProc,hWnd, Message, wParam, lParam) ;
}


// Callback for radio and check boxes
BOOL CALLBACK StateButtonProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// Extract pointer to handle array of button text
	HBITMAP *hButtonText = (HBITMAP *)GetWindowLong(hWnd,GWL_USERDATA);

	switch (Message)
	{
		case WM_DESTROY:
		{
			DeleteControlBitmaps(hButtonText);
			break;
		}

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
				case BS_AUTORADIOBUTTON:	hButtons = hRadioButtons;	break;
				case BS_AUTOCHECKBOX:			hButtons = hCheckButtons;	break;
			}

			if (!IsWindowEnabled(hWnd)) // If window is disabled
					hButtons+=1; // Use grayed out bitmaps

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
			BlitBitmap(dc, hButtons[(Button_GetCheck(hWnd)) ? 0 : 2], &button_rect, NOSTRETCH);

			// Blit button text
			if (hButtonText)
					BlitBitmap(dc, hButtonText[IsWindowEnabled(hWnd)?0:1], &text_rect, NOSTRETCH);

			EndPaint(hWnd,&Paint);
			return 0;
		}
		else
			break;
		 case BM_SETCHECK:
		case BM_SETSTATE:  // State of button has changed
			CallWindowProc(lpfnStateButtonProc,hWnd, Message, wParam, lParam);
			// Windows paints its own stuff during above call so we MUST repaint
			InvalidateRect(hWnd,NULL,0);
			UpdateWindow(hWnd);	// Repaint
		return 0;

		case WM_CAPTURECHANGED: // Tell parent we no logner have the focuse
			SendMessage(GetParent(hWnd),WM_CAPTURECHANGED, wParam, lParam);
		}
	return CallWindowProc(lpfnStateButtonProc,hWnd, Message, wParam, lParam) ;
}

// Callback for static text
BOOL CALLBACK StaticTextProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// Extract pointer to handle array of text
	HBITMAP *hBitmaps = (HBITMAP *)GetWindowLong(hWnd,GWL_USERDATA);
	switch (Message)
	{
		case WM_DESTROY:
		{
			DeleteControlBitmaps(hBitmaps);
			break;
		}

		case WM_SETFOCUS:   // Ensures that the focus is never on a button
		case WM_KILLFOCUS:  // and that the annoying focus rectangle is not displayed
			return 0;

		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR: // pass on keyboard messages to parent
			PostMessage(GetParent(hWnd), Message, wParam, lParam);
			return 0;

		case WM_PAINT:
			if (hBitmaps)
			{
				PAINTSTRUCT Paint;
				HDC dc= BeginPaint(hWnd,&Paint);
				RECT r;
				GetClientRect(hWnd, &r);
				if (hBitmaps)
					BlitBitmap(dc, hBitmaps[IsWindowEnabled(hWnd)?0:1], &r, NOSTRETCH);
				EndPaint(hWnd,&Paint);
				return 0;
			}; // pass on to normal handler if no bitmaps
			break;
	}
	return CallWindowProc(lpfnStaticTextProc,hWnd, Message, wParam, lParam) ;
}

// returns true if we're dealing with a "special" list box that doesn't
// get normal processing below
bool IsSpecialListBox(HWND hWnd)
{
	if ((GetDlgCtrlID(hWnd) == IDC_KEY_EFFECT_COMBO) ||
		(GetDlgCtrlID(hWnd) == IDC_GUILDS) ||
		(GetDlgCtrlID(hWnd) == IDC_IGNORE_LIST) ||
		(GetDlgCtrlID(hWnd) == IDC_WATCH_LIST))
		return true;
	else
		return false;
}

void DrawListBoxArrow(HWND hWnd, HDC dc, RECT r) //###
{
	HDC bitmap_dc = CreateCompatibleDC(dc);
	HGDIOBJ old_object = SelectObject(bitmap_dc, hListBoxArrow);
	BitBlt(dc, r.left, 0, effects->EffectWidth(LyraBitmap::LISTBOX_ARROWS),
			effects->EffectHeight(LyraBitmap::LISTBOX_ARROWS), bitmap_dc,
			0, 0, SRCCOPY);

	SelectObject(bitmap_dc, old_object);
	DeleteDC(bitmap_dc);
}


// Callback for list boxes
BOOL CALLBACK ListBoxProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_KEYUP:
		case WM_CHAR: // pass on keyboard messages to parent
			PostMessage(GetParent(hWnd), Message, wParam, lParam);
			break;
		case WM_KEYDOWN:
			{
				PostMessage(GetParent(hWnd), Message, wParam, lParam);

				int old_sel = ListBox_GetCurSel(hWnd);
				int sel = old_sel;

				if (wParam == VK_UP && sel == 0)
					sel = ListBox_GetCount(hWnd) - 1;
				else if (wParam == VK_DOWN && sel == (ListBox_GetCount(hWnd) - 1))
					sel = 0;
				if (old_sel != sel)
				{
					ListBox_SetCurSel(hWnd, sel);
					InvalidateRect(hWnd, NULL, TRUE); //..since Windows does its own painting in SetCurSel...

					PostMessage(GetParent(hWnd), WM_COMMAND,
						(WPARAM)MAKEWPARAM(GetDlgCtrlID(hWnd), LBN_SELCHANGE),
						(LPARAM)hWnd); // Windows should send LBN_SELCHANGE out in SetCurSel but does not
					return 0;
				}
			}
			break;

		case LB_SETCURSEL:
			if (IsSpecialListBox(hWnd))
				break;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case WM_MOUSEMOVE:
			if (IsSpecialListBox(hWnd))
				break;
			return 0;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			if (IsSpecialListBox(hWnd))
				break;
			InvalidateRect(hWnd, NULL, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;

		case WM_LBUTTONDOWN:
		{
			if (IsSpecialListBox(hWnd))
				break;
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);
			RECT r;
			GetWindowRect(hWnd, &r);
			// if within the up/down arrow...
			if (x >= (r.right - r.left - effects->EffectWidth(LyraBitmap::LISTBOX_ARROWS)))
			{	  //generate corrosponding keyboard message
				if (y < (effects->EffectHeight(LyraBitmap::LISTBOX_ARROWS)/2))
					SendMessage(hWnd,WM_KEYDOWN, VK_UP,0);  // in up arrow
				else
					SendMessage(hWnd,WM_KEYDOWN, VK_DOWN,0); // in down arrow
			}
			break;
		}

		case WM_PAINT:
		{ // important - do NOT custom paint the keyboard config or
		  // select guild listboxes - they are different!
			if (IsSpecialListBox(hWnd))
				break;

			TCHAR buffer[DEFAULT_MESSAGE_SIZE];

			HFONT  pOldFont;
			PAINTSTRUCT Paint;
			HDC dc= BeginPaint(hWnd,&Paint);

			SetTextColor(dc, ORANGE);
			SetBkMode(dc, OPAQUE);
			SetBkColor(dc, DKBLUE);
			int curr_selection = ListBox_GetCurSel(hWnd);
			if (curr_selection != -1)
			{
				TEXTMETRIC tm;
				RECT text_region;
				pOldFont = (HFONT)SelectObject(dc, display_font[0]);
				GetTextMetrics(dc, &tm);
				GetWindowRect(hWnd, &text_region);
				ListBox_GetText(hWnd, curr_selection, buffer);
				int clip_width = text_region.right - text_region.left;
// EITHER
/* 			int counter = 0;
				int text_width = 0;
				int char_width;
				while (buffer[counter] != '\0')
				{
					GetCharWidth(dc, buffer[counter], buffer[counter], &char_width);
					text_width += char_width;
					if (text_width + tm.tmAveCharWidth > clip_width)
					{
						buffer[counter] = '\0';
						break;
					}
					counter++;
				}
*/
// OR
				buffer[(int)((clip_width-(4*tm.tmAveCharWidth)) / tm.tmAveCharWidth)]='\0';

				TextOut(dc, 0, 0, buffer, _tcslen(buffer));
				SelectObject(dc, pOldFont);
			}

			RECT r;
			GetClientRect(hWnd, &r);
			r.left = r.right - effects->EffectWidth(LyraBitmap::LISTBOX_ARROWS);
			DrawListBoxArrow(hWnd, dc, r);

			// ensure the focus rect is painted consistently
			GetClientRect(hWnd, &r);
			r.top = 1;
			r.bottom = effects->EffectHeight(LyraBitmap::LISTBOX_ARROWS) - 1;

			if (GetFocus() == hWnd)
				DrawFocusRect(dc, &r);
			EndPaint(hWnd,&Paint);
			return 0;
		}; // pass on to normal handler if no bitmaps
		break;
	}

	return CallWindowProc(lpfnListBoxProc,hWnd, Message, wParam, lParam) ;
}


// Callback for combo boxes
BOOL CALLBACK ComboBoxProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR: // pass on keyboard messages to parent
			PostMessage(GetParent(hWnd), Message, wParam, lParam);
			break;
	}

	return CallWindowProc(lpfnComboBoxProc, hWnd, Message, wParam, lParam) ;
}

// Callback for trackbars
BOOL CALLBACK TrackBarProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR: // pass on keyboard messages to parent
			PostMessage(GetParent(hWnd), Message, wParam, lParam);
			break;
	}

	return CallWindowProc(lpfnTrackBarProc, hWnd, Message, wParam, lParam) ;
}

// Callback for all other controls
BOOL CALLBACK EditProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
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

	return CallWindowProc(lpfnEditProc, hWnd, Message, wParam, lParam) ;
}

// Callback for all other controls
BOOL CALLBACK GenericProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_CHAR: // pass on keyboard messages to parent
			PostMessage(GetParent(hWnd), Message, wParam, lParam);
			break;
	}

	return CallWindowProc(lpfnGenericProc, hWnd, Message, wParam, lParam) ;
}


void ResizeDlg(HWND hDlg)
{
	int w,h;
	RECT rect;

	if ((scale_x == 0.0f) || (!ready))
		return;

	// now resize appropriately, based on fonts
	GetWindowRect(hDlg, &rect);
	w = (int)(((float)(rect.right - rect.left))*scale_x);
	h = (int)(((float)(rect.bottom - rect.top))*scale_y);
	MoveWindow(hDlg, rect.left, rect.top, w, h, TRUE);

	// and resize all child windows appropriately
	EnumChildWindows(hDlg, EnumChildProcSize, NULL);
}

// callback to set size of child windows
BOOL CALLBACK EnumChildProcSize( HWND hChild, LPARAM lParam )
{
	int w,h,x,y;
	RECT rect1, rect2;

	// now resize appropriately, based on fonts
	GetWindowRect(GetParent(hChild), &rect1);
	GetWindowRect(hChild, &rect2);
	x = (int)((float)rect2.left*scale_x) - rect1.left;
	y = (int)((float)rect2.top*scale_y) - rect1.top;
	w = (int)(((float)(rect2.right - rect2.left))*scale_x);
	h = (int)(((float)(rect2.bottom - rect2.top))*scale_y);
	MoveWindow(hChild, x, y, w, h, TRUE);

	return TRUE;
}


// Callback to enumerate through all controls of a dialog and set up
//  apropriate callbacks and bitmaps
BOOL CALLBACK EnumChildProcSetup( HWND hChild, LPARAM lParam )
{
	TCHAR class_name[30];
	int effect_id = GetDlgCtrlID(hChild);

	SendMessage(hChild, WM_SETFONT, WPARAM(display_font[0]), 0);

	GetClassName(hChild, class_name, sizeof class_name);

	if(_tcscmp(class_name, _T("Edit"))	 &&
		 _tcscmp(class_name, _T("ComboBox"))&&
		_tcscmp(class_name, _T("ListBox"))
		) // These controls won't have their TABSTOP style unset, so they can receive the focus
			// All others cannot receive focus
	{
		int style = GetWindowLong(hChild, GWL_STYLE);
		style &= ~WS_TABSTOP;
		SetWindowLong(hChild, GWL_STYLE, style);
	}


	if    (_tcscmp(class_name, _T("Static")) == 0)  // if window is a static text control
	{ // bitmaps will be null if this is drawn as text
		HBITMAP *hBitmaps = CreateControlBitmaps(effect_id,1);
		SetWindowLong(hChild, GWL_USERDATA, LONG(hBitmaps));

		if (hBitmaps)
		{
			BITMAP bm;
			GetObject(hBitmaps[0],sizeof BITMAP, &bm);
			ResizeButton(hChild,bm.bmWidth,bm.bmHeight);
		}
		lpfnStaticTextProc = SubclassWindow(hChild, StaticTextProc);
		return TRUE;
	}

	//if    (_tcscmp(class_name, "ComboBox") == 0)
	//{	// if window is a combo box
	// lpfnComboBoxProc = SubclassWindow(hChild, ComboBoxProc);
	// return TRUE;
	//}

	if    (_tcscmp(class_name, _T("ListBox")) == 0)
	{
		lpfnListBoxProc = SubclassWindow(hChild, ListBoxProc);
		return TRUE;
	}

	if    (_tcscmp(class_name, _T("ComboBox")) == 0)
	{	// if window is a list box
		lpfnComboBoxProc = SubclassWindow(hChild, ComboBoxProc);
		return TRUE;
	}


	if    (_tcscmp(class_name, _T("msctls_trackbar32")) == 0)
	{ // track bar
		lpfnTrackBarProc = SubclassWindow(hChild, TrackBarProc);
		return TRUE;
	}

	if (_tcscmp(class_name, _T("Edit")) == 0)
	{ // edit
		lpfnEditProc = SubclassWindow(hChild, EditProc);
		return TRUE;
	}


	if (_tcscmp(class_name, _T("Button")))  // if window is not a button type
	{	// use generic proc
		lpfnGenericProc = SubclassWindow(hChild, GenericProc);
		return TRUE;
	}

  int style =GetWindowStyle(hChild);
	switch (style & 0x000F)   // styles are any integer from 0..16
	{
		// window data for radio & checkboxes is the bitmap handle
		case BS_AUTORADIOBUTTON:
		case BS_AUTOCHECKBOX:
			{
				HBITMAP *hBitmaps = CreateControlBitmaps(effect_id,1);
				if (hBitmaps)
				{

					SetWindowLong(hChild,GWL_USERDATA ,LONG(hBitmaps));
					//BITMAP bm;
					//GetObject(hBitmaps[0], sizeof BITMAP, &bm);
					//ResizeButton(hChild,bm.bmWidth,bm.bmHeight);
				}
				lpfnStateButtonProc = SubclassWindow(hChild, StateButtonProc);
			}
		break;

		case BS_PUSHBUTTON:
		case BS_DEFPUSHBUTTON:
			{
				switch (effect_id)
				{
					case IDC_OK:	  effect_id = LyraBitmap::OK;   break;
					case IDC_CANCEL:	effect_id = LyraBitmap::CANCEL;		break;
				} // Use standard IDS for common buttons

				// window data for buttons is the bitmap handle
				HBITMAP *hBitmaps = CreateControlBitmaps(effect_id,1);
				if (hBitmaps)
				{
					int style = GetWindowLong(hChild, GWL_STYLE);
					style |=  BS_BITMAP;
					SetWindowLong(hChild, GWL_STYLE, style);

					SetWindowLong(hChild, GWL_USERDATA, LONG(hBitmaps));
					SendMessage(hChild, BM_SETIMAGE,WPARAM (IMAGE_BITMAP), LPARAM (hBitmaps[0]));
					BITMAP bm;
					GetObject(hBitmaps[0], sizeof BITMAP, &bm);
					ResizeButton(hChild,bm.bmWidth,bm.bmHeight);
					ResizeButton(hChild,bm.bmWidth,bm.bmHeight);
				}
				lpfnPushButtonProc = SubclassWindow(hChild, PushButtonProc);
			}
		break;
	}
	return TRUE; // Continue enumeration
}


// Callback for master dialog proc. Sets up child controls,paints background,etc
BOOL CALLBACK LyraDialogProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	DLGPROC windowProc;

	switch(Message)
	{
		case WM_INITDIALOG:
			SetWindowLong(hDlg, GWL_USERDATA, lParam);
			SendMessage(hDlg, WM_SETFONT, WPARAM(display_font[0]), 0);
			ResizeDlg(hDlg);
			EnumChildWindows(hDlg, EnumChildProcSetup, NULL);
			break;
	}

	windowProc = DLGPROC(GetWindowLong(hDlg, GWL_USERDATA));

	if (windowProc)
		return windowProc(hDlg, Message, wParam, lParam);
	else
		return 0;
}

static HWND hCurrentDlg;

// modeless version of dlg box proc to call
HWND __cdecl CreateLyraDialog( HINSTANCE hInstance, int dialog, HWND hWndParent,
											 DLGPROC lpDialogFunc)

{
	DLGPROC lpDialogProc = lpDialogFunc;

	hCurrentDlg = CreateDialogParam(hInstance,MAKEINTRESOURCE(dialog),hWndParent,LyraDialogProc, LPARAM(lpDialogFunc));
	cDS->PlaySound(LyraSound::MESSAGE_ALERT);

	return hCurrentDlg;
}


bool IsLyraDialogMessage(MSG *msg)
{
	if (msg->message == WM_KEYDOWN  && msg->wParam == VK_TAB) // tabs are used to move in dialog
			return IsDialogMessage(hCurrentDlg, msg); // Seems that Windows checks if hCurrent is valid,returns false if not
	else
		 return false;
}


// modal version of dlg box proc to call
int __cdecl LyraDialogBox( HINSTANCE hInstance, int dialog, HWND hWndParent, DLGPROC lpDialogFunc)

{
	DLGPROC lpDialogProc = lpDialogFunc;

	if (cDS)
		cDS->PlaySound(LyraSound::MESSAGE_ALERT);

	if (!ready) // use plain dialogs for startup error messages to avoid weird color combinations
		return DialogBox (hInstance, MAKEINTRESOURCE(dialog), hWndParent, lpDialogFunc);
	else
		return DialogBoxParam(hInstance, MAKEINTRESOURCE(dialog), hWndParent, LyraDialogProc, LPARAM(lpDialogFunc));
}




// Creates cursor from bitmap in effects.rlm: first creates windows bitmap,  creates windows
// bitmap mask from this, creates windows cursor from these, and sets the system cursor to this.
static void SetupLyraCursor(void)
{
/* int bitmap_id = LyraBitmap::CURSOR;

	if (effects->EffectWidth(bitmap_id ) == 0 )
		return;

	effects->LoadEffectBitmaps(bitmap_id);

	int width = effects->EffectWidth(bitmap_id);
	int height= effects->EffectHeight(bitmap_id);

	HBITMAP hCursorBitmap = CreateBitmap(width, height, 1, BITS_PER_PIXEL,
																				effects->EffectBitmap(bitmap_id)->address);

	// Create cursor mask bitmap from cursor.
	int size  = width*height/8; // its a 1 bit per pixel bitmap
	UCHAR *maskbits = new UCHAR  [size];
	UCHAR *dest = maskbits;
	PIXEL *src	= (PIXEL*)effects->EffectBitmap(bitmap_id)->address;

	memset(maskbits,0,size);
	for (int i = 0; i < size; i++)
	{
		int j = 8;
	  while (j--)
			if (!*src++)			// if the source pixel is black
				*dest |=  1 << j;  // set the mask bit on (which makes black transparent)
		 *dest++ ;
	}
	HBITMAP hCursorMask = CreateBitmap(width, height, 1, 1, maskbits);

	delete [] maskbits;
	effects->FreeEffectBitmaps(bitmap_id);

	// Create Windows cursor
	ICONINFO ii;
	ii.fIcon  = false ;	// cursor (false) or icon (true)
	ii.xHotspot  = 0;
	ii.yHotspot  = 0;
	ii.hbmMask	 = hCursorMask;
	ii.hbmColor  = hCursorBitmap;
	HCURSOR hCursor = CreateIconIndirect(&ii);

  DeleteObject(hCursorMask);	// CreateIconIdirect makes copies of these so
	DeleteObject(hCursorBitmap); // they can be deleted here
*/
	// Set system cursor to Lyra cursor
 HCURSOR hCursor = LoadCursor(NULL,IDC_ARROW);
	SetCursor(hCursor);
	SetSystemCursor(hCursor,OCR_NORMAL );
}

// Restores the system cursor by accessing the registry for the previous (software)cursor's filename
static void RestoreSystemCursor(void)
{
	HKEY reg_key = NULL;
	DWORD reg_type;
	TCHAR cursor_file[DEFAULT_MESSAGE_SIZE] = _T("");
	DWORD size = sizeof cursor_file;


	RegOpenKeyEx(HKEY_CURRENT_USER, _T("Control Panel\\Cursors"),0,KEY_ALL_ACCESS ,&reg_key);

	if (reg_key) 
	{	// Get name of file of the regular arrow cursor
		RegQueryValueEx(reg_key,_T("Arrow"),NULL,&reg_type, (LPBYTE)cursor_file, &size);

		// Load cursor and set system cursor to it
		HCURSOR hCursor;
		if (_tcslen(cursor_file))
			hCursor = LoadCursorFromFile(cursor_file);// software cursor
		else
			hCursor = LoadCursor(NULL,IDC_ARROW); // hardware cursor (or registry calls failed)
		SetSystemCursor(hCursor, OCR_NORMAL);
		RegCloseKey(reg_key);
	}

}

void __cdecl InitLyraDialogController(void)
{
	hRadioButtons = CreateControlBitmaps(LyraBitmap::BULLET,2);
	hCheckButtons = CreateControlBitmaps(LyraBitmap::CHECK_BUTTON, 2);
	hBackground   = CreateWindowsBitmap(LyraBitmap::DLG_BACKGROUND);
	hGoldBackground = CreateWindowsBitmap(LyraBitmap::DLG_BACKGROUND2);
	hListBoxArrow	= CreateWindowsBitmap(LyraBitmap::LISTBOX_ARROWS);
	blueBrush	  = CreateSolidBrush(DKBLUE);
	blackBrush	  = CreateSolidBrush(BLACK);
	SetupLyraCursor();
}

void __cdecl DeInitLyraDialogController(void)
{
	RestoreSystemCursor();

	if (hRadioButtons)
		DeleteControlBitmaps(hRadioButtons);
	if (hCheckButtons)
		DeleteControlBitmaps(hCheckButtons);
	if (hBackground)
		DeleteObject(hBackground);
	if (hGoldBackground)
		DeleteObject(hGoldBackground);
	if (hListBoxArrow)
		DeleteObject(hListBoxArrow);
	if (blueBrush)
		DeleteObject(blueBrush);
	if (blackBrush)
		DeleteObject(blackBrush);
}

bool TileBackground(HWND hWnd, int which_bitmap)
{
	RECT r;
	BITMAP bm;
	GetClientRect(hWnd,&r);
	int bytes;

	if (!ready) // no coloration before game starts
		return false;

	if (which_bitmap == 0)
		bytes = GetObject(hBackground,sizeof(BITMAP),&bm);
	else
		bytes = GetObject(hGoldBackground,sizeof(BITMAP),&bm);

	if (!bytes || (&bm == NULL))
		return false;

	PAINTSTRUCT Paint;
	HDC dc= BeginPaint(hWnd,&Paint);
	SetTextColor(dc, ORANGE);
	int repeat_x = (r.right/bm.bmWidth) + 1;
	int repeat_y = (r.bottom/bm.bmHeight) + 1;

	// Tile background bitmap
	for (int x= 0; x < repeat_x; x++)
		for (int y= 0; y < repeat_y; y++)
		{
			r.left = x*bm.bmWidth;
			r.top  = y*bm.bmHeight;
			r.right = r.left + bm.bmWidth;
			r.bottom = r.top + bm.bmHeight;
			if (which_bitmap == 0)
				BlitBitmap(dc,hBackground,&r, NOSTRETCH);
			else
				BlitBitmap(dc,hGoldBackground,&r, NOSTRETCH);
		}
	EndPaint(hWnd,&Paint);
	return true;
}

// sets standard colors for controls; returns true if the message is handled
HBRUSH SetControlColors(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam, bool goalposting)
{
	if (!ready) // no coloration before game starts
		return NULL;

	TCHAR class_name[30];

	switch(Message)
	{
		case WM_CTLCOLORSTATIC: // set color of Static controls
		{
			GetClassName((HWND)lParam, class_name, sizeof class_name);
			if    (_tcscmp(class_name, _T("msctls_trackbar32")) == 0)
			{
				HDC dc = (HDC)wParam;
				SetTextColor(dc, ORANGE);
				SetBkMode(dc, OPAQUE);
				SetBkColor(dc, BLACK);
				return blackBrush;
			}
			else
			{
				HDC dc = (HDC)wParam;
				SetTextColor(dc, ORANGE);
				if (goalposting)
				{
					SetBkMode(dc, OPAQUE);
					SetBkColor(dc, BLACK);
					return blackBrush;
				}
				else
				{
					SetBkMode(dc, TRANSPARENT);
					return ((HBRUSH)GetStockObject(NULL_BRUSH));
				}
			}
		}


		case WM_CTLCOLOREDIT: // set color of edit controls
		case WM_CTLCOLORLISTBOX: // set color of listbox controls
		case WM_CTLCOLORBTN: // set color of button controls
		{
			HDC dc = (HDC)wParam;
			SetTextColor(dc, ORANGE);
			SetBkMode(dc, OPAQUE);
			if (goalposting)
			{
				SetBkColor(dc, BLACK);
				return blackBrush;
			}
			else
			{
				SetBkColor(dc, DKBLUE);
				return blueBrush;
			}
		}
		default:
			return NULL;
	}
}

		/*
			// code for custom painting combo boxes

			SIZE		size;
			PAINTSTRUCT Paint;
			HDC dc= BeginPaint(hWnd,&Paint);
			RECT		rc, rcTab, rcFocus;

			// draw border

			GetClientRect( hWnd, &rc );
			Draw3DRect( dc, &rc, lyra_colors[COLOR_HIGHLIGHT], lyra_colors[COLOR_BTNSHADOW] );
			DeflateRect( &rc, 2, 2 );

			// draw tab

			rcTab = rc;
			rcTab.left = rc.right = rcTab.right - GetSystemMetrics( SM_CXHTHUMB );
			RECT rcTab2 = rcTab;

			bool m_dropDown = SendMessage(hWnd, CB_GETDROPPEDSTATE, 0, 0);

			if( m_dropDown )
			{
				Draw3DRect( dc, &rcTab, lyra_colors[COLOR_HIGHLIGHT], lyra_colors[COLOR_BTNSHADOW] );

				rcTab.top++;
				rcTab.left++;
				// last param was m_crThumbDn
				Draw3DRect( dc, &rcTab, lyra_colors[COLOR_BTNSHADOW], lyra_colors[COLOR_3DLIGHT]);

				DeflateRect( &rcTab, 1, 1 );
				// last param was m_crThumbDn
				HBRUSH thumbDnBrush = CreateSolidBrush(lyra_colors[COLOR_3DLIGHT]);
				FillRect( dc, &rcTab, thumbDnBrush);
				DeleteObject(thumbDnBrush);
			} else
			{
				Draw3DRect( dc, &rcTab, lyra_colors[COLOR_HIGHLIGHT], lyra_colors[COLOR_BTNSHADOW] );
				DeflateRect( &rcTab, 2, 2 );

				rcTab.right++;
				rcTab.bottom++;
				// last param was m_crThumbUp
				HBRUSH thumbUpBrush = CreateSolidBrush(lyra_colors[COLOR_3DLIGHT]);
				FillRect( dc, &rcTab, thumbUpBrush );
				DeleteObject(thumbUpBrush);
			}

			rcTab = rcTab2;


			// draw arrow

			HPEN darkpen = CreatePen( PS_SOLID, 1, lyra_colors[COLOR_BTNSHADOW] );
			HPEN lightpen = CreatePen( PS_SOLID, 1, lyra_colors[COLOR_HIGHLIGHT] );

			POINT topleft = {rcTab.left + ( (rcTab.right - rcTab.left) / 3 ) + m_dropDown, rcTab.top + ( (rcTab.bottom - rcTab.top) / 3 ) + m_dropDown };
			POINT topright = { rcTab.right - ( (rcTab.right - rcTab.left) / 3 ) + m_dropDown, rcTab.top + ( (rcTab.bottom - rcTab.top) / 3 ) + m_dropDown };
			POINT bottom = { rcTab.left + ( (rcTab.right - rcTab.left) / 2 ) + m_dropDown, rcTab.bottom - ( (rcTab.bottom - rcTab.top) / 3 ) + m_dropDown };

			HPEN OldPen = (HPEN)SelectObject(dc, lightpen);
			MoveToEx( dc, topright.x, topright.y, NULL );
			LineTo( dc, bottom.x, bottom.y );

			SelectObject(dc, darkpen );
			LineTo( dc, topleft.x, topleft.y );
			LineTo( dc, topright.x, topright.y );

			SelectObject(dc,	OldPen );

			// draw background

			rc.bottom++; // BOGON - ALIGNMENT FIX

			HBRUSH bgbrush = CreateSolidBrush(DKBLUE);
			FrameRect( dc, &rc, bgbrush );
			DeflateRect( &rc, 1, 1 );

			HBRUSH focbrush = CreateSolidBrush(lyra_colors[COLOR_3DLIGHT]);

			if( GetFocus() == hWnd ) FillRect( dc, &rc, focbrush );
				else FillRect( dc, &rc, bgbrush ) ;

			DeleteObject(bgbrush);
			DeleteObject(focbrush);

			// draw text

			TCHAR text[64];
			HFONT  pOldFont;

			//pOldFont = dc.SelectObject( GetFont() );

			SetTextColor(dc, lyra_colors[COLOR_WINDOWTEXT]);

			SetBkMode( dc, TRANSPARENT );

			_tcscpy(text, "W");
			GetTextExtentPoint(dc, text, 1, &size);
			rc.left += size.cx;

			GetWindowText( hWnd, text, sizeof(text) );
			DrawText( dc, text, _tcslen(text), &rc, DT_LEFT|DT_SINGLELINE|DT_VCENTER );
			//dc.SelectObject( pOldFont );
			EndPaint(hWnd,&Paint);
			return 0;
		*/



/*
// following are functions ripped off from MFC to ease porting of
// the combo box drawing code

void Draw3DRect(HDC dc, RECT *lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	Draw3DRect(dc, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
		lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}

void Draw3DRect(HDC dc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	FillSolidRect(dc, x, y, cx - 1, 1, clrTopLeft);
	FillSolidRect(dc, x, y, 1, cy - 1, clrTopLeft);
	FillSolidRect(dc, x + cx, y, -1, cy, clrBottomRight);
	FillSolidRect(dc, x, y + cy, cx, -1, clrBottomRight);
}

void FillSolidRect(HDC dc, int x, int y, int cx, int cy, COLORREF clr)
{

	SetBkColor(dc, clr);
	RECT rect = {x, y, x + cx, y + cy};
	ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}


void DeflateRect(RECT *rect, int x, int y)
{
	rect->left += x;
	rect->top += y;
	rect->right -= x;
	rect->bottom -= y;
}

*/
