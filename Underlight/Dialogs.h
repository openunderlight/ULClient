// Header file for Dialogs.cpp

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef INCL_DIALOGS
#define INCL_DIALOGS

#include "LyraDefs.h"
#include "LmItem.h"
#include "Central.h"

//////////////////////////////////////////////////////////////////
// Constants

struct CreateItem { // reasons for forging items
	enum {
		GM_ITEM = 0,
		FORGE_ITEM,
		QUEST_ITEM
	};
};

const int ACCEPTREJECT_TIMER  =  WM_USER + DIALOG_MAGIC + 50;

//////////////////////////////////////////////////////////////////
// New Windows Messages

#define WM_PARENT DIALOG_MAGIC + WM_USER + 1
#define WM_STRIP_DOT DIALOG_MAGIC + WM_USER + 2
#define WM_STRIP_RETURN DIALOG_MAGIC + WM_USER + 3
#define WM_ADD_INITIATES DIALOG_MAGIC + WM_USER + 4
#define WM_ADD_KNIGHTS DIALOG_MAGIC + WM_USER + 5
#define WM_ADD_RULERS  DIALOG_MAGIC + WM_USER + 6
#define WM_SET_GUILD_NAME DIALOG_MAGIC + WM_USER + 7
#define WM_SET_KNIGHT_NAME DIALOG_MAGIC + WM_USER + 8
#define WM_SET_KNIGHT_ID DIALOG_MAGIC + WM_USER + 9
#define WM_INIT_ITEMCREATOR DIALOG_MAGIC + WM_USER + 10
#define WM_SET_CALLBACK DIALOG_MAGIC + WM_USER + 11
#define WM_ADD_BUDDY DIALOG_MAGIC + WM_USER + 12
#define WM_SET_ITEM DIALOG_MAGIC + WM_USER + 13
#define WM_SET_ART_CALLBACK DIALOG_MAGIC + WM_USER + 14
#define WM_SET_SCROLL_FORGE_CALLBACK DIALOG_MAGIC + WM_USER + 15
#define WM_SET_AR_NEIGHBOR DIALOG_MAGIC + WM_USER + 16
#define WM_PRIOR_LINE DIALOG_MAGIC + WM_USER + 17
#define WM_NEXT_LINE DIALOG_MAGIC + WM_USER + 18
//#define WM_SET_SCROLL_QUEST_CALLBACK DIALOG_MAGIC + WM_USER + 19
#define WM_INIT_ITEMFIELDS DIALOG_MAGIC + WM_USER + 20
#define WM_SET_SCROLL_QUESTBUILDER_CALLBACK  DIALOG_MAGIC + WM_USER + 21
#define WM_SET_USE_PT DIALOG_MAGIC + WM_USER + 22
#define WM_ADD_DESTINATIONS + WM_USER + 23
#define WM_POWER_TOKEN + WM_USER + 24

//////////////////////////////////////////////////////////////////
// New Types

struct scroll_t {
	int  num_charges;
	bool artifact;
	TCHAR descrip[Lyra::MAX_ITEMDESC];
	TCHAR name[LmItem::NAME_LENGTH];
	int color1;
	int color2;
};

typedef VOID (*REGPROC)(LPTSTR); 
typedef void (*dlg_callback_t)(void *value);

const int MAX_MACROS = 10;
typedef TCHAR macro_t[Lyra::MAX_SPEECHLEN-Lyra::PLAYERNAME_MAX];

const int NUM_MONSTER_BREEDS = 4;
struct monster_breed_t
{
	UINT type; // string table pointers
	UINT descrip;
	int color1, color2;
};


//////////////////////////////////////////////////////////////////
// Function Prototypes

BOOL CALLBACK TalkDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PMareTalkDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI TalkEditWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK HelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CreditsDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MetaDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GMItemHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PlayerItemHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK QuestItemHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK QuestHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CreateItemDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK KeyboardConfigDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChooseGuildDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AvatarDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MonsterAvatarDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam); // Jared
BOOL CALLBACK FatalErrorDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK NonfatalErrorDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AcceptRejectDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PowerTokenDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EnterValueDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LocateAvatarDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK IgnoreListDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK WriteScrollDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AgentLoginDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK WarningYesNoDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PMareDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GMTeleportDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DummyDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChatMacrosDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChatMacrosDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK UsePPointDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GrantPPointDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PPointHelpDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SummonDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChooseDestinationDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);

void CreateTextDialog(UINT IDS_prompt, TCHAR *initial_text, dlg_callback_t on_OK_callback, int size);

// helpers
void AddBuddyCallback(void *value);
void AddEnemyCallback(void *value);
void SetAvatarValues(HWND hDlg);
void ResetKeyboardDefaults(HWND hDlg, int default_config);
void RegistryReadMacro(int macro_number, macro_t macro);
int NumPhrases(TCHAR* text);

HWND TopMost(void);

#endif
