// Header file for cDetailQuest class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CDETAILQUEST_H
#define CDETAILQUEST_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LyraDefs.h"
#include "cQuestBuilder.h"

class cItem;

//////////////////////////////////////////////////////////////////
// Constants

const int NUM_QUEST_FIELDS = 3;

//////////////////////////////////////////////////////////////////
// New Windows Messages

//////////////////////////////////////////////////////////////////
// Helpers

LRESULT WINAPI DetailQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//LRESULT WINAPI AccepteeWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////
// Class Definition
class cDetailQuest
{

public: 

private:
	HWND hwnd_detailquest; // handle to window for reading quests
	HWND hwnd_title;
	HWND hwnd_loadingtext;
	HWND hwnd_maxaccepttext;
	HWND hwnd_maxaccept;
	HWND hwnd_expirationtext;
	HWND hwnd_expiration;
	HWND hwnd_statusflagtext;
	HWND hwnd_statusflag;
	HWND hwnd_numaccepteetext;
	HWND hwnd_numacceptee;
	HWND hwnd_accepteestext;
	HWND hwnd_acceptees;
	HWND hwnd_questxptext;
	HWND hwnd_questxp;
	HWND hwnd_graphictext;
	HWND hwnd_graphic;
	HWND hwnd_graphicicon;
	HWND hwnd_chargestext;
	HWND hwnd_charges;
	HWND hwnd_colorstext;
	HWND hwnd_color1;
	HWND hwnd_color2;
	HWND hwnd_itemtypetext;
	HWND hwnd_itemtype;
	HWND hwnd_fieldtext[NUM_QUEST_FIELDS];
	HWND hwnd_field[NUM_QUEST_FIELDS];
	HWND hwnd_keywordstext;
	HWND hwnd_keywords;

	HWND hwnd_exit; // handle to exit

	HWND hwnd_text_buttons[NUM_QUEST_BUTTONS]; // scrolling buttons
	HBITMAP text_buttons_bitmaps[NUM_QUEST_BUTTONS][2];
//	WNDPROC	lpfn_text; // pointer to window procedure

	HBITMAP hTitle;
	HBITMAP hLoading;
	HBITMAP hMaxAccept;
	HBITMAP hExpiration;
	HBITMAP hStatus;
	HBITMAP hNumAcceptee;
	HBITMAP hAcceptee;
	HBITMAP hQuestXP;
	HBITMAP hField[NUM_QUEST_FIELDS];
	HBITMAP hGraphicIcon;
	HBITMAP hGraphic;
	HBITMAP hCharges;
	HBITMAP hColors;
	HBITMAP hItemType;
	HBITMAP hKeywords;
	//HBITMAP	hVelocity;
	//HBITMAP	hEffect;
	//HBITMAP hDamage;

	HBITMAP hExit;

	unsigned char icon[ICON_WIDTH*ICON_HEIGHT*BYTES_PER_PIXEL];
	cItem *dummy_item; // dummy for graphic display
	cItem *quest_item; // item in inventory that satisfied the quest

	BOOL active; // whether or not we're reading a quest
	bool completing; // whether we clicked Complete to come here
	int qtype; // type of quest - Talisman/Codex

	cGoal *quest; // currently displayed quest
	
public:
    cDetailQuest(void);
    ~cDetailQuest(void);

	void Activate(cGoal *new_quest, bool completing_quest = false);
	void Deactivate(void);
	void SetDetails(void); // called when details arrive for a quest
	void ShowKeywords(void); // for post of codex quests only

	void ScrollUp(int count);
	void ScrollDown(int count);

	void QuestCompleted(lyra_id_t quest_id);
	void CompleteQuestError(void);

	// Selectors
	inline BOOL Active(void) { return active; };
	inline realmid_t CurrQuestID(void) { if (quest) return quest->ID(); else return NO_GOAL; };

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cDetailQuest(const cDetailQuest& x);
	cDetailQuest& operator=(const cDetailQuest& x);

	// The Window Proc for this control must be a friend...
	friend LRESULT WINAPI DetailQuestWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//	friend LRESULT WINAPI AccepteeWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif