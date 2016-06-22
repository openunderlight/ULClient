// Header file for cControlPanel Class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include <Commctrl.h>
#include "4dx.h"
#include "SharedConstants.h"
#include "LyraDefs.h"
#include "Effects.h"

//////////////////////////////////////////////////////////////////
// Constants

const bool SELECT_NEXT_LISTITEM = true;
const bool SELECT_PREV_LISTITEM = false;

const int NUM_TABS  = 4;
const int NUM_LISTVIEWS = 3;
const int NUM_LV_BUTTONS = 4;

enum cp_tab_type {
  NO_TAB = -1,
  INVENTORY_TAB = 0,   
  NEIGHBORS_TAB = 1,    
  ARTS_TAB = 2,   
  AVATAR_TAB = 3
};

enum delete_type {
	SHOW_NEXT = 1,
	SHOW_PREV = 2,
	SHOW_NONE = 3
};

enum scroll_buttons {
	DDOWN = 0,
	DUP,
	DOWN,
	UP
};


//////////////////////////////////////////////////////////////////
// New Windows Messages

#define WM_PASS_INV_PROC WM_USER + CONTROL_PANEL_MAGIC + 2
#define WM_PASS_WHO_PROC WM_USER + CONTROL_PANEL_MAGIC + 3
#define WM_PASS_ARTS_PROC WM_USER + CONTROL_PANEL_MAGIC + 4
#define WM_PASS_INV_HWND WM_USER + CONTROL_PANEL_MAGIC + 7
#define WM_PASS_WHO_HWND WM_USER + CONTROL_PANEL_MAGIC + 8
#define WM_PASS_ARTS_HWND WM_USER + CONTROL_PANEL_MAGIC + 9

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI ControlPanelWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
int CALLBACK CompareNeighbors(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
int CALLBACK CompareArts(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 

class cItem;
class cNeighbor;
struct window_pos_t;

//////////////////////////////////////////////////////////////////
// Class Definition

class cControlPanel
{

public: 

private:
	HWND hwnd_cp;
	HWND hwnd_left,hwnd_right; // avatar turning buttons
	HWND hwnd_tab; // handle to tab bitmap
	HWND hwnd_listviews[NUM_LISTVIEWS];
	HWND hwnd_avatar; // avatar window

	HWND hwnd_use;  
	HWND hwnd_drop; 
	HWND hwnd_meta; 
	HWND hwnd_give;
	HWND hwnd_invcounter;

	HWND hwnd_usepp;
	HWND hwnd_grantpp;

	HWND hwnd_stats[NUM_PLAYER_STATS];
	HWND hwnd_orbit;
	HWND hwnd_listview_buttons[NUM_LISTVIEWS][NUM_LV_BUTTONS];
	bool captured;	// true when the mouse has been captured
	bool giving; // true when player has clicked on give button
	bool useing; // true when player has clicked on use button	
				 // intentionally misspelled since using is a reserved word
	bool last_select_by_click; // true if last selection was made by a click
	int curr_avatar_view; // view of current avatar

	unsigned char munched_bits[ICON_WIDTH*ICON_HEIGHT*BYTES_PER_PIXEL];
//	BOOL munched_bits_valid; // if this has been successfully filled in
	cItem *selected_item; // pointer to selected item
	cNeighbor *selected_neighbor; // pointer to selected neighbor
	lyra_id_t selected_art; // id of selected art
	int tab_mode; // which tab is currently selected
	int num_items; // # of items currently carried
	int num_neighbors; 
	int num_arts;
	int num_icons, num_unused_icons; // # of icons & unused icons in listview
	cItem *drag_item; // NO_ITEM if no dragging occurring, otherwise = pointer to dragged item
	POINT last_drag; // position of last drag image drawing
	POINT hotspot_pos;
	bool first_paint;

	// tracks whether the user has recently made a selection on the items,
	// arts, or neighbors tab; used by cArts for target selection
	bool selection_made;

	HIMAGELIST			 image_list;

	// handles for UI graphics
	HBITMAP	cp_window_bitmap;
	HBITMAP	cp_tab_bitmap[NUM_TABS];
	HBITMAP cp_bullet_bitmap;
	HBITMAP cp_avatar_bitmap;

	// handles for bitmap buttons - two for each (selected/unselected)
	HBITMAP left_bitmap[2];
	HBITMAP right_bitmap[2];
	HBITMAP use_bitmap[2];
	HBITMAP drop_bitmap[2];
	HBITMAP meta_bitmap[2];
	HBITMAP give_bitmap[2];
	HBITMAP usepp_bitmap[2];
	HBITMAP grantpp_bitmap[2];

	// handles for listview buttons
	HBITMAP listview_buttons_bitmaps[NUM_LISTVIEWS][NUM_LV_BUTTONS][2];
	
public:
    cControlPanel(void);
    ~cControlPanel(void);

	// use the cp_window bitmap as the target for outside messages
	inline HWND Hwnd_CP(void) { return hwnd_cp;};
	inline HWND Hwnd_Avatar(void) { return hwnd_avatar;};
	inline HWND Hwnd_Left(void) { return hwnd_left;};
	inline HWND Hwnd_Right(void) { return hwnd_right;};
	inline HWND Hwnd_Use(void) { return hwnd_use;};
	inline HWND Hwnd_Meta(void) { return hwnd_meta;};
	inline HWND Hwnd_Drop(void) { return hwnd_drop;};
	inline HWND Hwnd_Give(void) { return hwnd_give;};
	inline HWND Hwnd_UsePP(void) { return hwnd_usepp;};
	inline HWND Hwnd_GrantPP(void) { return hwnd_grantpp;};
	inline cItem* DragItem(void) { return drag_item;};
	inline bool SelectionMade(void) { return selection_made; };
	inline void SetSelectionMade(bool value) { selection_made = value; };
	inline bool Giving(void) { return giving; };
	inline bool Using(void) { return useing; };
	inline void SetGiving(bool value) { giving = value; };
	inline void SetUsing(bool value) { useing = value; };

	void AddAvatar(void);
	inline int CurrAvatarView(void) { return curr_avatar_view;};
	void Show(void);
	void SelectNew(bool direction);
	void DeselectSelected(void);
	int AddIcon(unsigned char* icon_bits); // returns new index
	unsigned int LookUpStatColor(int stat); // return color to use for stats

	// add/remove items from control panel
	int AddItem(cItem* item);
	void DeleteItem(cItem* item, int type = SHOW_NEXT);
	cItem* SelectedItem(void);
	void SetSelectedItem(cItem *item);
	bool InventoryFull(void);
	bool InventoryEmpty(void);
	cItem* GetFirstItem(void);
	cItem* GetLastItem(void);
	cItem* GetNextItem(cItem *last, bool lv_ok = false);
	cItem* GetPrevItem(cItem *first, bool lv_ok = false);

	// add/remove neighbors from control panel
	int AddNeighbor(cNeighbor* n);
	void DeleteNeighbor(cNeighbor* n, int type = SHOW_NEXT);
	void ReplaceNeighborIcon(cNeighbor* n);
	int	NumMonsters(void);
	cNeighbor* SelectedNeighbor(void);
	void SetSelectedNeighbor(cNeighbor *n);
	cNeighbor* GetFirstNeighbor(void);
	cNeighbor* GetLastNeighbor(void);
	cNeighbor* GetNextNeighbor(cNeighbor *last, bool lv_ok = false);
	cNeighbor* GetPrevNeighbor(cNeighbor *first, bool lv_ok = false);

	// add/remove arts from control panel
	int AddArt(lyra_id_t art);
	void DeleteArt(lyra_id_t art, int type = SHOW_NEXT);
	void SetupArts(void); // set up initial listing of arts
	lyra_id_t SelectedArt(void);
	void SetSelectedArt(lyra_id_t art);
	lyra_id_t GetFirstArt(void);
	lyra_id_t GetLastArt(void);
	lyra_id_t GetNextArt(lyra_id_t last, bool lv_ok = false);
	lyra_id_t GetPrevArt(lyra_id_t first, bool lv_ok = false);

	// manipulate control panel displays
	void ShowNextItem(int count);
	void ShowPrevItem(int count);
	void ShowNextNeighbor(int count);
	void ShowPrevNeighbor(int count);
	void ShowNextArt(int count);
	void ShowPrevArt(int count);


	// update displays
	void UpdateStats(void);
	void UpdateArt(lyra_id_t art);
	void UpdateInvCount(void);
	void FillInArtString(lyra_id_t art, TCHAR *buffer);


	void SetMode(int new_mode, bool art_capture = false, bool force_redraw = false);
	int  Mode(void);

	inline bool MouseCaptured(void) { return captured;};

	// drag methods
	bool UndrawDrag(unsigned char *buffer = NULL);
	bool DrawDrag(bool viewport, unsigned char *buffer = NULL);
	void CheckDragScroll(void);
	void StartDrag(cItem *item, int xoffset, int yoffset);
	void EndDrag(void);

	void DumpInventory(void); // diagnostics

private:
	BOOL HandlePaint(HWND hwnd); 
	void BlitBitmap(HWND hwnd, HBITMAP bitmap, RECT *region);
	int  AddToListView(HWND listview, int new_index,  int image_index, 
					   TCHAR *descrip, long data);
	void SetListViewSelected(HWND listview, int curr_index, int new_index);
	int LookUpItemIndex(cItem *item);
	cItem* LookUpItem(int index);
	int LookUpNeighborIndex(cNeighbor *n);
	cNeighbor* LookUpNeighbor(int index);
	int LookUpArtIndex(lyra_id_t art);
	lyra_id_t LookUpArt(int index);
	int FindClosestEntry(HWND hwnd_listview);

	void AddNextItem(void);
	void AddPrevItem(void);
	void AddNextNeighbor(void);
	void AddPrevNeighbor(void);
	void AddNextArt(void);
	void AddPrevArt(void);

	void TurnAvatar(int delta); // change view by delta

	void CompactImageList();
	void RemoveIcon(int index); 
	RECT* GetSelectedStatRect(void); // return rect for selected stat

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cControlPanel(const cControlPanel& x);
	cControlPanel& operator=(const cControlPanel& x);

	// The window procs for this control must be friends...
	friend LRESULT WINAPI ControlPanelWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
	friend int CALLBACK CompareNeighbors(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 
	friend int CALLBACK CompareArts(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif


};

#endif