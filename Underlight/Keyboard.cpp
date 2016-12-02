// keyboard handlers

// Copyright Lyra LLC, 1997. All rights reserved.

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "cDDraw.h"
#include "cDSound.h"
#include "cAgentBox.h"
#include "cActorList.h"
#include "cArts.h"
#include "cControlPanel.h"
#include "cChat.h"
#include "cItem.h"
#include "cLevel.h"
#include "cNeighbor.h"
#include "cOrnament.h"
#include "cPlayer.h"
#include "Interface.h"
#include "cGameServer.h"
#include "cGoalPosting.h"
#include "cGoalBook.h"
#include "cParty.h"
#include "Dialogs.h"
#include "Mouse.h"
#include "Move.h"
#include "cQuestBuilder.h"
#include "Options.h"
#include "Realm.h"
#include "Resource.h"
#include "Keyboard.h"
#include "cKeymap.h"
#include "SharedConstants.h"
//#include "RogerWilco.h"
#include "utils.h"
#include "LmXPTable.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cActorList *actors;
extern cDDraw *cDD;
extern cDSound *cDS;
extern cPlayer *player;
extern cControlPanel *cp;
extern cGameServer *gs;
extern cLevel *level;
extern cArts *arts;
extern cChat *display;
extern cAgentBox *agentbox;
extern cKeymap *keymap;
extern cGoalBook *goalbook;
extern cGoalPosting *goals;
extern cQuestBuilder *quests;
//extern HWND hEntryWin;

extern options_t options;
extern unsigned char keyboard[num_keystates];
extern mouse_move_t mouse_move;
extern mouse_look_t mouse_look;
extern bool mouselooking;
extern bool show_map;
extern int MAX_LV_ITEMS;

extern bool leveleditor;
extern bool framerate;
extern bool talkdlg;
extern bool pmare_talkdlg;
extern bool metadlg;
extern bool itemdlg;
extern bool itemhelpdlg;
extern bool ignorelistdlg;
extern bool ready;
extern bool exiting;
extern bool agentdlg;
extern bool gmteleportdlg;
extern bool avatardlg;

extern cNeighbor *test_avatar;
extern cOrnament *test_object;
// HACK
extern int num_find_sector_calls[15];
extern bool showing_map;

static bool numlock_on_at_startup = false;
extern xp_entry lyra_xp_table[];
//////////////////// Constants ////////////////////////
//#ifdef UL_DEBUG
	//const int MAX_GM_EFFECTS=99;	// max effects at once time
//#else
	const int MAX_GM_EFFECTS=2;		// max effects at once time
//#endif


static void SetNumLock(bool on)
{
  BYTE states[256];
	GetKeyboardState(states);
	states[VK_NUMLOCK] = on;
	SetKeyboardState(states);
}

void __cdecl InitKeyboard(void)
{
 if (GetKeyState(VK_NUMLOCK) != 0)
 {
	 numlock_on_at_startup = true;
	 SetNumLock(false);
 }
}
void __cdecl DeInitKeyboard(void)
{
	if (numlock_on_at_startup)
		SetNumLock(true);
}

typedef void (*YESNOCALLBACK)(void *value);
static void LiftCallback(void *value);
static void DropCallback(void *value);
static void JunkCallback(void *value);
static void LiftItems(void *value);
static void DoOnConfirm(UINT uID, YESNOCALLBACK lpDialogFunc);
static void DoOnConfirm(TCHAR* pMsg, YESNOCALLBACK lpDialogFunc);

//////////////////////////////////////////////////////////////////
// Message Handlers for Keyboard Messages

static HWND hDlg;

// returns true if it handles a debugging key
bool HandleLyraDebugKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	cNeighbor *n;

	switch (vk) {
/*	case 'A':
// Disabled as per Martin's request - Jared 7/14/00
		{
			unsigned int current_form = player->Avatar().AvatarType();
			unsigned int next_form = current_form + 1;
			LmAvatar temp_avatar;
			temp_avatar = player->Avatar();
			
			if (!(player->flags & ACTOR_SOULSPHERE))
			{
				LoadString (hInstance, IDS_FORM_CHANGE, message, sizeof(message));
				display->DisplayMessage(message, false);
				return true;
			}
			else
			{
				if (next_form > Avatars::MAX_AVATAR_TYPE)
					next_form = Avatars::MIN_AVATAR_TYPE;
				
				LoadString (hInstance, IDS_FORM_UPDATE, message, sizeof(message));
			_stprintf(disp_message, message, (TCHAR *) NightmareName(current_form), (TCHAR *) NightmareName(next_form));
				display->DisplayMessage(disp_message, false);
				
				temp_avatar.SetAvatarType(next_form);
				player->SetAvatar(temp_avatar, true);
			}
		}
		return true;
*/		
	case 'O': // neighbor debug output
	_stprintf(errbuf, _T("Neighbor debug output information in room %d:"),player->Room());
		INFO(errbuf);
	_stprintf(errbuf, _T("Player at %d,%d,%d"),(int)player->x,(int)player->y,(int)player->z);
		INFO(errbuf);
		for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
		{
		_stprintf(errbuf, _T("Name: %s ID: %d Loc: %d,%d,%d,%d Phys: %d Avatar: %d"),
				n->Name(),n->ID(),(int)n->x,(int)n->y,(int)n->z,
				n->sector,(int)n->physht,n->Avatar());
			INFO(errbuf);
		}
		actors->IterateNeighbors(DONE);
		return true;
		
	case 'Q':
		for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
		{
			if (n->IsLocal())
			{
				LoadString (hInstance, IDS_NAME_LOCAL, message, sizeof(message));
			_stprintf(message, n->Name());
			}
			else if (n->IsOutsider())
			{
				LoadString (hInstance, IDS_NAME_OUTSIDER, message, sizeof(message));
			_stprintf(message, n->Name());
			}
			else
			{
				LoadString (hInstance, IDS_NAME_UNKNOWN, message, sizeof(message));
			_stprintf(message, n->Name());
			}
			display->DisplayMessage(message, false);
		}
		actors->IterateNeighbors(DONE);
		return true;
	case 'M':
		MemoryCheck(_T("user request"));
		display->DisplayMessage (message, false);
		return true;
	case 'X':
		{
			static int sphere = 0;
			static int originalXP = player->XP();
			int base = lyra_xp_table[sphere*10].xp_base;
			
			if (sphere < 0)
				player->SetXP(originalXP, true);
			else
				player->SetXP(base + (base/100), true);
			
			sphere++;
			if (sphere>9)
				sphere = -1;
		}
		gs->UpdateServer();
		return true;
		
	case 'Y':
		framerate = !framerate;
		return true;
		
	case 'Z':
		extern int opt_on;
		opt_on = !opt_on;
		LoadString (hInstance, IDS_NAME_LOCAL, message, sizeof(message));
//	_stprintf(message, "Optimization %s\n", opt_on ? "On" : "Off");
	_stprintf(disp_message, message, opt_on ? "On" : "Off");
		display->DisplayMessage(disp_message, false);
		return true;
	case 'U':	// update
		{
			
			for (int i=0; i<NUM_PLAYER_STATS; i++)
			{ // ALWAYS set max first
				player->SetMaxStat(i, 99, SERVER_UPDATE_ID);
				player->SetCurrStat(i, 1, SET_ABSOLUTE,  player->ID());
				player->SetCurrStat(i, 99, SET_ABSOLUTE,  player->ID());
			}
			for (int i=0; i<NUM_GUILDS; i++)
			{
				player->SetGuildRank(i, 3);
				player->SetGuildXPPool(i, 10000000);
			}
			player->SetQuestXPPool(1000000);
			
			for (int i=0; i<NUM_ARTS; i++)
			{
				player->SetSkill(i, 1, SET_ABSOLUTE, player->ID(), false);
				player->SetSkill(i, 99, SET_ABSOLUTE, player->ID(), false);
			}
			
			gs->UpdateServer();
		}

	case VK_F11: // create item programatically
		{
/*
			RMsg_EnterRoom enter_msg;
			int num_dudes = 1;
			enter_msg.Init(num_dudes);
			for (int z=1000; z<1000+num_dudes; z++)
			{
				TCHAR buf[64];
				_stprintf(buf, _T("Dude%d"), z);
				RmRemotePlayer rm;
				LmPeerUpdate pu;
				lyra_peer_update_t p;
				p.realtime_id = z;
				p.x = player->x + rand()%500;
				p.y = player->y + rand()%500;
				pu.Init(p);
				rm.Init(pu, player->Avatar(), buf, 0, 0);
				enter_msg.SetPlayer(z, rm);
				cNeighbor *n = new cNeighbor(enter_msg.Player(z));
			}
*/

			// create the token
			LmItem info;
			LmItemHdr header;
			cItem *item;

			lyra_item_gratitude_t gratitude = {LyraItem::GRATITUDE_FUNCTION, 0, 0, 0, 0, 0};
			
			header.Init(0, 0);
			header.SetFlags(LyraItem::FLAG_HASDESCRIPTION);
			header.SetGraphic(LyraBitmap::TALISMAN7);
			header.SetColor1(15); header.SetColor2(7);
			int gratnum = LyraItem::GRATITUDE_FUNCTION;
			header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::GRATITUDE_FUNCTION), 0, 0));
			_stprintf(message, _T("%s"), _T("NEW Gratitude"));
			_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
			disp_message[LmItem::NAME_LENGTH-1] = '\0';
			
			info.Init(header, disp_message, 0, 0, 0);
			
			gratitude.maturity_date = 513;
			info.SetStateField(0, &gratitude, sizeof(gratitude));
			//gratitude.set_creatorid(3123456789);
			gratitude.set_creatorid(34561);
			gratitude.target = 0;
			
			info.SetStateField(0, &gratitude, sizeof(gratitude));
			info.SetCharges(254);
			int ttl = 120000;

			TCHAR descrip[512] = _T("This is my essence string");
			item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl, descrip);

//			cGoal* dummy = new cGoal();
//			dummy->SetDetails(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
//				_T("essence"), NULL);
//			bool success = DoesCodexSatisfyQuest(item, dummy);
//			delete dummy;

			return true;

			/*
			lyra_item_train_support_t support = {LyraItem::SUPPORT_TRAIN_FUNCTION, 0, 0, 0, 0, 0, 0};
			
			header.Init(0, 0);
			header.SetFlags(yraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
			header.SetGraphic(LyraBitmap::GUILD_ASCENSION_TOKEN);
			header.SetColor1(0); header.SetColor2(0);
			header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_TRAIN_FUNCTION), 0, 0));
			
			support.art_id = 82;
			support.art_level = 69;
			support.set_creator_id(1000 + rand()%100);
			support.set_target_id(2141);
			
			_stprintf(message, "%s-%s", arts->Descrip(0), "Xenus token");
			_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
			disp_message[LmItem::NAME_LENGTH-1] = '\0';
			
			info.Init(header, disp_message, 0, 0, 0);
			info.SetStateField(0, &support, sizeof(support));
			info.SetCharges(1);
			int ttl = 120000;
			item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
			LoadString (hInstance, IDS_SUPPORT_TRAIN_TOKEN, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, "Xenus", "JP");
			display->DisplayMessage (message);
			return true;
			
			// create the token
			LmItem info;
			LmItemHdr header;
			cItem *item;
			lyra_item_train_support_t support = {LyraItem::SUPPORT_TRAIN_FUNCTION, 0, 0, 0, 0, 0, 0};
			
			header.Init(0, 0);
			header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
			header.SetGraphic(LyraBitmap::GUILD_ASCENSION_TOKEN);
			header.SetColor1(0); header.SetColor2(0);
			header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_TRAIN_FUNCTION), 0, 0));
			
			support.art_id = 255;
			support.art_level = 69;
			support.set_creator_id(1000 + rand()%100);
			support.set_target_id(319);
			
		_stprintf(message, "%s-%s", arts->Descrip(0), "Xenus token");
			_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
			disp_message[LmItem::NAME_LENGTH-1] = '\0';
			
			info.Init(header, disp_message, 0, 0, 0);
			info.SetStateField(0, &support, sizeof(support));
			info.SetCharges(1);
			int ttl = 120;
			item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
			LoadString (hInstance, IDS_SUPPORT_TRAIN_TOKEN, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, "Xenus", "JP");
			display->DisplayMessage (message);
			return true;
			*/

		}
	case VK_F12:
		{
			lyra_item_meta_essence_nexus_t nexus = { LyraItem::META_ESSENCE_NEXUS_FUNCTION, 0, 0, 0, 200, 200 };
			LmItem info;
			LmItemHdr header;
			cItem *item;
			header.Init(0, 0);
			header.SetFlags(LyraItem::FLAG_HASDESCRIPTION | LyraItem::FLAG_SENDSTATE);
			header.SetGraphic(LyraBitmap::META_ESSENCE);
			header.SetColor1(15); header.SetColor2(15);
			header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::META_ESSENCE_NEXUS_FUNCTION), 0, 0));
			_stprintf(message, _T("%s"), _T("Essence Box"));
			_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH - 1);
			disp_message[LmItem::NAME_LENGTH - 1] = '\0';
			info.Init(header, disp_message, 0, 0, 0);
			info.SetStateField(0, &nexus, sizeof(nexus));
			info.SetCharges(1);
			int ttl = 120000;

			TCHAR descrip[512] = _T("This is my essence string");
			item = CreateItem(player->x, player->y, player->angle, info, 0, false, GMsg_PutItem::DEFAULT_TTL, descrip);
			return true;
		}
  
	default:
		return false;
	}	
}

// returns true if it handles an offline debugging key
bool HandleDebugOfflineKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{	// offline functions only
	int frame = test_avatar->currframe;
	int angle = test_avatar->angle;
	int color;
	switch (vk)
	{
	case 'U':
		if (leveleditor)
		{
			level->Load(level->ID());
			player->Teleport( player->StartX(), player->StartY(), player->StartAngle());
			return true;
		};
		return false;
		
	case 'Z':
	case 'z':
		frame++;
		if (frame >= test_avatar->frames)
			frame = 0;
		test_avatar->currframe = frame;
		//test_avatar->SetPose(WALKING, true);
		return true;
	case 'X':
	case 'x':
		frame--;
		if (frame <= 0)
			frame = test_avatar->frames - 1;
		test_avatar->currframe = frame;
		//test_avatar->SetPose(WALKING, true);
		return true;
	case '1':
		angle = FixAngle(angle + 170);
		test_avatar->angle = angle;
		return true;
	case '2':
		angle = FixAngle(angle - 170);
		test_avatar->angle = angle;
		return true;
	case '3':
		frame++;
		if (frame >= test_avatar->frames)
			frame = 0;
		test_avatar->currframe = frame;
		//test_avatar->destx = 306;
		//test_avatar->desty = -650;
		return true;
	case '4':
		frame--;
		if (frame <= 0)
			frame = test_avatar->frames - 1;
		test_avatar->currframe = frame;
		
		//test_avatar->destx = 320;
		//test_avatar->desty = 500;
		return true;
	case '5':
		color = test_object->Color(0);
		color++;
		if (color == NUM_ACTOR_COLORS)
			color = 0;
		test_object->SetColor(0, color);
		return true;
	case '6':
		color = test_object->Color(1);
		color++;
		if (color == NUM_ACTOR_COLORS)
			color = 0;
		test_object->SetColor(1, color);
		return true;
	case '7':
		{
			LoadString (hInstance, IDS_NO_SERVER, message, sizeof(message));
			hDlg = CreateLyraDialog(hInstance, IDD_CHOOSE_GUILD,
				cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		}
		return true;
	case '8':
		InvalidateRect(GetDlgItem(hDlg, IDC_FATAL_ERROR_TEXT), NULL, FALSE);
		return true;
	default:
		return false;
	}	
}


bool HandlePlayerMetaKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	switch (vk)
	{
#ifndef PMARE
	case '0':  // chat macros alt '0' - alt '9'
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		if (player->CanUseChatMacros() && !talkdlg)
			// && ((options.network && gs && gs->LoggedIntoGame()) || options.welcome_ai))
		{
			talkdlg = TRUE;
			HWND hDlg = CreateLyraDialog(hInstance, IDD_TALK,
				cDD->Hwnd_Main(), (DLGPROC)TalkDlgProc);		 // open talk dialog
			SendMessage(GetDlgItem(hDlg,IDC_SPEECH),WM_SYSKEYUP,vk,0); // tell it which macro key was used
			return true;
		}
#endif
	default:
		int found_key = -1;
		// Search for a mapped function for the key pressed
		found_key = keymap->FindMapping(vk);
	
		if (found_key != MAPPING_NOT_FOUND)
			switch (found_key)
			{
			case LyraKeyboard::SCROLL_UP:
				display->ScrollUp(5);
				return true;
			case LyraKeyboard::SELECT_PREV:
				display->ScrollUp(1);
				return true;
			case LyraKeyboard::SELECT_NEXT:
				display->ScrollDown(1);
				return true;
			case LyraKeyboard::SCROLL_DOWN:
				display->ScrollDown(5);
				return true;
			default:
				return false;
			}
		else
			return false;
	}
}

#ifdef GAMEMASTER
bool HandleGMFullMetaKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	int guild_id = -1;
	int goal_board = 1; // default to initiate; shift to go to knight boards
	if (keyboard[Keystates::SHIFT])
		goal_board = 2;
	
	if (keyboard[Keystates::CONTROL]) 
	{
		switch (vk)
		{
			//alt F1,F3-F5,F7-F10 access initiate & knight goal boards for gm's
		case VK_F10: guild_id++; 
		case VK_F9: guild_id++; 
		case VK_F8: guild_id++; 
		case VK_F7: guild_id++;
			// Alt-F6 is not sent by Windows
		case VK_F5: guild_id++; 
		case VK_F4: guild_id++;
		case VK_F3: guild_id++; 
			// Alt-F2 is not sent by Windows
		case VK_F1: guild_id++; 
			goals->Activate(guild_id, goal_board); 
			return true;
		default:
			return false;
		}
	}
	
	
	switch (vk)
	{		 
#ifndef RPGM
	case 'W':
	{

		if (!gs || (!gs->LoggedIntoLevel())) {
			LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
			return true;
		}

		if (0 != actors->NumNeighbors())
		{
			LoadString (hInstance, IDS_CANT_HIDE, message, sizeof(message));
			display->DisplayMessage(message, false);
			return true;
		}
		LmAvatar tempavatar = player->Avatar();
		if (tempavatar.Hidden())
		{
			tempavatar.SetHidden(0);
			player->SetAvatar(tempavatar, true);
			player->RemoveTimedEffect(LyraEffect::PLAYER_INVISIBLE);
			player->RemoveTimedEffect(LyraEffect::PLAYER_MIND_BLANKED);
			LoadString (hInstance, IDS_HIDDEN_TOGGLE, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, "Off");
		}
		else
		{
			tempavatar.SetHidden(1);
			player->SetAvatar(tempavatar, true);
			player->SetTimedEffect(LyraEffect::PLAYER_INVISIBLE, INT_MAX, player->ID());
			player->SetTimedEffect(LyraEffect::PLAYER_MIND_BLANKED, INT_MAX, player->ID());
			LoadString (hInstance, IDS_HIDDEN_TOGGLE, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, "On");
		}
		display->DisplayMessage (message, false);

		return true;
	}
#endif

	case 'M': 
		ShowWindow(cDD->Hwnd_Main(), SW_SHOWMINIMIZED);
		return true;

	case 'T': // gm teleport
		if (!gmteleportdlg)
		{
			gmteleportdlg = true;
			HWND hDlg = CreateLyraDialog(hInstance, IDD_GM_TELEPORT,
				cDD->Hwnd_Main(), (DLGPROC)GMTeleportDlgProc);
		}
		return true;
				
	case 'O':
		cp->DumpInventory();
		return true;
		
	case 'L': // Lift all dropped objects into inventory
		LoadString(hInstance, IDS_LIFT, message, sizeof(message));
		DoOnConfirm(message, LiftCallback);
		return true;
		
	case 'F': // drop all dropped objects into inventory
		LoadString(hInstance, IDS_DROP, message, sizeof(message));
		DoOnConfirm(message, DropCallback);
		return true;
		
	case 'J': // JUNK (delete) all in inventory
		LoadString(hInstance, IDS_DEL, message, sizeof(message));
		DoOnConfirm(message, JunkCallback);
		return true;

	case 'P':
		gs->PingServer();
		return true;
		
	case 'D':
		player->SetCurrStat(Stats::DREAMSOUL, 0, SET_ABSOLUTE, player->ID());
		return true;

	//case 'H':
	//	options.ignore_whispers = !options.ignore_whispers;
	//	if (options.ignore_whispers)
	//	{
	//		LoadString (hInstance, IDS_IGNORE_WHISP, disp_message, sizeof(disp_message));
	//		display->DisplayMessage (disp_message, false);
	//	}
	//	else 
	//	{
	//		LoadString (hInstance, IDS_LISTEN_WHISP, disp_message, sizeof(disp_message));
	//		display->DisplayMessage (disp_message, false);
	//	}
	//	return true;
		
	case 'B':
//#ifndef UL_DEBUG
//		if (player->Avatar().AvatarType() < Avatars::MIN_NIGHTMARE_TYPE)
//		{
//			display->DisplayMessage ("You must be a nighmare to babble", false);
//			return true;
//		}
//#endif
		options.babble_filter = !options.babble_filter;
		if (options.babble_filter)
		{
			LoadString (hInstance, IDS_BABBLE_ON, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message, false);
		}
		else
		{
			LoadString (hInstance, IDS_BABBLE_OFF, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message, false);
		}
		return true;
		
	case 'I':
		// no invulnerability for dark mares
		if (player->Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
		{
#ifdef UL_DEBUG
			LoadString (hInstance, IDS_DM_INVUL_SUGGESTION, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message, false);
#else
			LoadString (hInstance, IDS_NO_DM_INVUL, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message, false);
			return true;
#endif
		}
		
		if (options.invulnerable)
		{
			options.invulnerable = false;
			LoadString (hInstance, IDS_INVULNERABLE_TOGGLE_OFF, message, sizeof(message));
		}
		else
		{
			options.invulnerable = true;
			LoadString (hInstance, IDS_INVULNERABLE_TOGGLE_ON, message, sizeof(message));
		}
		display->DisplayMessage (message, false);
		return true;

	case 'C':
		LoadString (hInstance, IDS_COORDS, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message,(int)player->x,(int)player->y,(int)player->z,(int)player->Room(), level->ID(), (int)player->sector,(int)player->angle);
		display->DisplayMessage(message, false);
		return true;

		
	case 'R':
		// no restore for dm's, except from soul sphere
		if ((player->Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) &&
			!(player->flags & ACTOR_SOULSPHERE))
		{
#ifdef UL_DEBUG
			LoadString (hInstance, IDS_DM_RESTORE_SUGGESTION, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message, false);
#else
			LoadString (hInstance, IDS_DM_NO_RESTORE, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message, false);
			return true;
#endif
		}
		
		if (player->flags & ACTOR_SOULSPHERE)
		{
			options.num_gm_effects = 0;
			player->ReformAvatar();
		}
		
		player->SetCurrStat(Stats::DREAMSOUL, player->MaxStat(Stats::DREAMSOUL), SET_ABSOLUTE, player->ID());
		player->SetCurrStat(Stats::WILLPOWER, player->MaxStat(Stats::WILLPOWER), SET_ABSOLUTE, player->ID());
		player->SetCurrStat(Stats::INSIGHT, player->MaxStat(Stats::INSIGHT), SET_ABSOLUTE, player->ID());
		player->SetCurrStat(Stats::RESILIENCE, player->MaxStat(Stats::RESILIENCE), SET_ABSOLUTE, player->ID());
		player->SetCurrStat(Stats::LUCIDITY, player->MaxStat(Stats::LUCIDITY), SET_ABSOLUTE, player->ID());
		gs->UpdateServer();
		return true;
		
	case 'Y': // HYPNOTIC WEAVE
	case 'X': // TERROR
		//case 'Z': // EARTHQUAKE
	case 'K': // DARKNESS
		{
			unsigned int my_RMSG, my_ART;
			options.num_gm_effects = 0;
			
			// First check to see that a DM has enough points to cast this
			
			if (player->flags & ACTOR_FREE_ACTION)
				options.num_gm_effects++;
			if (player->flags & ACTOR_DETECT_INVIS)
				options.num_gm_effects++;
			if (player->flags & ACTOR_PROT_FEAR)
				options.num_gm_effects++;
			
			if ((player->Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) && (options.num_gm_effects>MAX_GM_EFFECTS))
			{
				LoadString (hInstance, IDS_NO_PROTECTION, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message, false);
				return true;
			}
			
			switch (vk) 
			{
			case 'Y':
				my_RMSG = RMsg_PlayerMsg::HYPNOTIC_WEAVE;
				my_ART  = Arts::HYPNOTIC_WEAVE;
				LoadString (hInstance, IDS_HYPNOTIC, message, sizeof(message));
				break;
			case 'X': 
				my_RMSG = RMsg_PlayerMsg::TERROR;
				my_ART  = Arts::TERROR;
				LoadString (hInstance, IDS_RUN_TERROR, message, sizeof(message));
				break;
			case 'K': 
				my_RMSG = RMsg_PlayerMsg::DARKNESS;
				my_ART  = Arts::DARKNESS;
				LoadString (hInstance, IDS_ROOM_DARKNESS, message, sizeof(message));
				break;
			}
			if (gs)
				gs->SendPlayerMessage(0, my_RMSG, player->Skill(my_ART), 0);
			
			display->DisplayMessage(message, false);
			return true;
		}
		
	case 'A':	// PROTECTION
	case 'V':	// VISION
	case 'E':	// RESIST FEAR
	case 'G':	// FREE ACTION
	case 'Z':   // REGENERATION
		{
			unsigned int ACTOR_CHECK, ACTOR_EFFECT;

			options.num_gm_effects = 0;

			// Lets perform one last check. When an art is abjured, we 
			// don't take into account DM's, so lets just do the check now.
						
			if (player->flags & ACTOR_FREE_ACTION)
				options.num_gm_effects++;
			if (player->flags & ACTOR_DETECT_INVIS)
				options.num_gm_effects++;
			if (player->flags & ACTOR_PROT_FEAR)
				options.num_gm_effects++;
			if (player->flags & ACTOR_PROT_CURSE)
				options.num_gm_effects++;
			if (player->flags & ACTOR_REGENERATING)
				options.num_gm_effects++;


			switch (vk) 
			{
			case 'A':
				ACTOR_CHECK = ACTOR_PROT_CURSE;
				ACTOR_EFFECT = LyraEffect::PLAYER_PROT_CURSE;
				break;
			case 'V': 
				ACTOR_CHECK = ACTOR_DETECT_INVIS;
				ACTOR_EFFECT = LyraEffect::PLAYER_DETECT_INVISIBLE;
				break;
			case 'E': 
				ACTOR_CHECK = ACTOR_PROT_FEAR;
				ACTOR_EFFECT = LyraEffect::PLAYER_PROT_FEAR;
				break;
			case 'G': 
				ACTOR_CHECK = ACTOR_FREE_ACTION;
				ACTOR_EFFECT = LyraEffect::PLAYER_PROT_PARALYSIS;
				break;
			case 'Z':
				ACTOR_CHECK = ACTOR_REGENERATING;
				ACTOR_EFFECT = LyraEffect::PLAYER_REGENERATING;
				break;
			}
			
			// Check to see if the player already has the art evoked. If so
			// Toggle it off.
			
			if (player->flags & ACTOR_CHECK)
			{
				player->RemoveTimedEffect(ACTOR_EFFECT);
				options.num_gm_effects--;
				return true;
			} 
			
			if ((player->Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) && (options.num_gm_effects >= MAX_GM_EFFECTS)) 
			{
				LoadString (hInstance, IDS_GM_ART_LIMIT, message, sizeof(message));
			_stprintf(disp_message, message, MAX_GM_EFFECTS);
				display->DisplayMessage(disp_message, false);
			} else 
			{
				player->SetTimedEffect(ACTOR_EFFECT, 10000000, player->ID());	
				options.num_gm_effects++;
			}
		}
		return true;
		
	default:
		return false;
	}
}

bool HandleGMSpecialKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	switch (vk) 
	{
	case VK_F2:
		if ((!itemdlg) && (options.network))
		{
			itemdlg = TRUE;
			HWND hDlg = CreateLyraDialog(hInstance, IDD_CREATE_ITEM, cDD->Hwnd_Main(), (DLGPROC)CreateItemDlgProc);
			SendMessage(hDlg, WM_INIT_ITEMCREATOR, 0, (LPARAM)CreateItem::GM_ITEM);
		}
		return true;
	case VK_F3:
		if (options.network)
		{
			cItem *selected_item = cp->SelectedItem();
			// don't attempt to duplicate if no item has been selected or if it's not a valid item 			
			if ((selected_item == NO_ACTOR) || !(actors->ValidItem(selected_item)))
				return false;

			gs->DuplicateItem(selected_item);
			return true;
		}
		return false;
	case VK_F4:
		agentbox->Show();
		return true;
	case VK_F6:
		player->Teleport(6958, 7522, 979, 29);
		return true;
	case VK_F7:
		player->Teleport(-3344, -7568, 0, 22);
		return true;
	case VK_F8:
		player->Teleport(-525, 1990, 0, 44);
		return true;

	case VK_F9:
	{
		// use this to fix checksums
		for (int q=0;q<NUM_ARTS;q++)
			arts->CanUseArt(q, true);
		return true;
	}

		
	default:
		return false;
	}
}


bool HandleGMMetaKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	switch (vk)
	{
	case 'U': // Go to Unknown / Horron's Lair
		if (player->Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
			player->Teleport (23312,8064,0, 35);	// Horron's Lair
		else
			player->Teleport (-7839,12457,0,43);	// Unknown
		return true;
		
		
	case 'Q': // NB: no 'q'! vk is a KEY not a character. small 'q' and 'Q' are the same.
		if (options.autorun)
		{
			options.autorun = false;
			LoadString (hInstance, IDS_RUN_TOGGLE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, "Off");
		}
		else
		{
			options.autorun = true;
			LoadString (hInstance, IDS_RUN_TOGGLE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, "On");
		}
		display->DisplayMessage (message, false);
		return true;
		
	case 'H': // Raw EMOTE
		if (!talkdlg && ((gs && gs->LoggedIntoGame()) || options.welcome_ai))
		{
			talkdlg = TRUE;
			HWND hDlg = CreateLyraDialog(hInstance, IDD_TALK,
				cDD->Hwnd_Main(), (DLGPROC)TalkDlgProc);
			Button_SetCheck(GetDlgItem(hDlg, IDC_RAW_EMOTE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_TALK), 0);
		}
		return true;

	case 'S': // MONSTER ROAR
		if ((player->Avatar().AvatarType() == Avatars::AGOKNIGHT)) {
			LoadString (hInstance, IDS_AGOKNIGHT_ROAR, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message, false);
		}
		Scream(player->Avatar().AvatarType(), player, true);
		return true;


	default:
		return false;
	}
}

#endif


void Realm_OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	hWnd = HWND(hWnd);
	int found_key = -1;
	int i;
	cNeighbor *n;

	CancelExit();

	//_stprintf(message, "got key: %d fdown: %d\n", vk, fDown);	
	//display->DisplayMessage (message, false);

	//
	// First set/check state of helper keys; these are SHIFT, CONTROL, ALT
	// Also perform any actions required by key-up,key-down pairs, like the Roger
	// Wilco RECORD function
	//

	if (fDown)	// key down
		switch (vk) 
		{
			case VK_SHIFT:keyboard[Keystates::SHIFT]	= 1; break;
			case VK_CONTROL: keyboard[Keystates::CONTROL] = 1; break;
			case VK_MENU: keyboard[Keystates::ALT] = 1; break;
			case VK_NUMLOCK: SetNumLock(false);
			// RogerWilco code below
			//case VK_F12:Record(true); 
				break;

		}
	else		// on key up
		switch (vk)
		{
			case VK_SHIFT: keyboard[Keystates::SHIFT] = 0; break;
			case VK_CONTROL: keyboard[Keystates::CONTROL] = 0; break;
			case VK_MENU: keyboard[Keystates::ALT] = 0; break;
			// RogerWilco code below
			//case VK_F12:Record(false);
				break;

		}


#ifdef UL_DEBUG

	//
	//   Debug functions are activated only from the debug state mode

	if (!fDown && (vk == VK_TAB))
	{
		options.debug_state_mode = !options.debug_state_mode;
		LoadString (hInstance, IDS_DEBUG_STATE, message, sizeof(message));
	_stprintf(disp_message, message, options.debug_state_mode ? "Enabled" : "Disabled");
		display->DisplayMessage (disp_message, false);
		return;
	}

	// if we are sent a debug key, handle it here, and ignore the key-up
	if (!fDown &&
		(keyboard[Keystates::ALT] == 1) && 
		options.debug_state_mode &&
		HandleLyraDebugKey(hWnd, vk, fDown, cRepeat, flags))
		return;

	if (options.debug_state_mode &&
		(!options.network) &&
		HandleDebugOfflineKey(hWnd, vk, fDown, cRepeat, flags))
		return;
#endif

	// now check for player meta keys

	if ((keyboard[Keystates::ALT] == 1) && 
		!fDown &&
		HandlePlayerMetaKey(hWnd, vk, fDown, cRepeat, flags))
		return;

	// now check for gm meta keys
#ifdef GAMEMASTER_FULL
	
	if ((keyboard[Keystates::ALT] == 1) && 
		!fDown &&
		HandleGMFullMetaKey(hWnd, vk, fDown, cRepeat, flags))
		return;
	
	if (!fDown && HandleGMSpecialKey(hWnd, vk, fDown, cRepeat, flags))
		return;
	
#endif
	
#ifdef GAMEMASTER
	
	if (!fDown && (keyboard[Keystates::ALT] == 1) && 
		HandleGMMetaKey(hWnd, vk, fDown, cRepeat, flags))
		return;
	
#endif
	
	if (!fDown) 
	switch (vk)
	{
	case VK_RETURN:
		// if we're waiting for a selection, make selection
		if (arts && arts->WaitingForSelection())
		{
			switch (cp->Mode())
			{
			case INVENTORY_TAB:
				if (cp->SelectedItem() != NO_ITEM)
					cp->SetSelectionMade(true);
				break;
			case NEIGHBORS_TAB:
				if (cp->SelectedNeighbor() != NO_ACTOR)
					cp->SetSelectionMade(true);
				break;
			case ARTS_TAB:
				if (cp->SelectedArt() != Arts::NONE)
					cp->SetSelectionMade(true);
				break;
			default:
				break;
			}
		}
		// return focus to main window
		SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
			(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
		break;
	case VK_ESCAPE:
		if (showing_map)
			showing_map = false;
		else if (arts && (arts->Waiting() || arts->Casting()))
			arts->CancelArt();
		else
			SendMessage(cp->Hwnd_CP(), WM_COMMAND, 0, (LPARAM)cp->Hwnd_Meta());
		break;
				case VK_F1:
					SendMessage(cp->Hwnd_CP(), WM_COMMAND, 0, (LPARAM)cp->Hwnd_Meta());
					break;
					
				default:
					break;
	}
	
	
	// Search for a mapped function for the key pressed
	found_key = keymap->FindMapping(vk);
	
	if (found_key != MAPPING_NOT_FOUND)
	{
		
		if (!mouselooking && ((vk == VK_LBUTTON) || (vk == VK_RBUTTON)))
			return; // stop if function is mapped to mouse, but we aren't in mouse look mode
		
		if (fDown)
			switch (found_key)
		{
	case LyraKeyboard::LOOK_UP: keyboard[Keystates::LOOK_UP] = 1; break;
	case LyraKeyboard::LOOK_DOWN: keyboard[Keystates::LOOK_DOWN] = 1; break;
	case LyraKeyboard::MOVE_FORWARD: keyboard[Keystates::MOVE_FORWARD] = 1; break;
	case LyraKeyboard::MOVE_BACKWARD: keyboard[Keystates::MOVE_BACKWARD] = 1; break;
	case LyraKeyboard::TURN_LEFT: keyboard[Keystates::TURN_LEFT] = 1; break;
	case LyraKeyboard::TURN_RIGHT: keyboard[Keystates::TURN_RIGHT] = 1; break;
	case LyraKeyboard::TRIP: keyboard[Keystates::TRIP] = 1; break;
	case LyraKeyboard::RUN: keyboard[Keystates::RUN] = 1; break;
	case LyraKeyboard::JUMP: keyboard[Keystates::JUMP] = 1; break;
	case LyraKeyboard::STRAFE: keyboard[Keystates::STRAFE] = 1; break;
	case LyraKeyboard::SIDESTEP_LEFT: keyboard[Keystates::SIDESTEP_LEFT] = 1; break;
	case LyraKeyboard::SIDESTEP_RIGHT: keyboard[Keystates::SIDESTEP_RIGHT] = 1; break;
	case LyraKeyboard::MOUSE_LOOK: mouselooking = !options.mouselook; break;
	case LyraKeyboard::WAVE: player->SetWaving(true);	break;
		
		}
		else // on key up
			switch (found_key)
		{
	case LyraKeyboard::LOOK_UP: keyboard[Keystates::LOOK_UP] = 0; break;
	case LyraKeyboard::LOOK_DOWN: keyboard[Keystates::LOOK_DOWN] = 0; break;
	case LyraKeyboard::MOVE_FORWARD: keyboard[Keystates::MOVE_FORWARD] = 0; break;
	case LyraKeyboard::MOVE_BACKWARD: keyboard[Keystates::MOVE_BACKWARD] = 0; break;
	case LyraKeyboard::TURN_LEFT: keyboard[Keystates::TURN_LEFT] = 0; break;
	case LyraKeyboard::TURN_RIGHT: keyboard[Keystates::TURN_RIGHT] = 0; break;
	case LyraKeyboard::TRIP: keyboard[Keystates::TRIP] = 0; break;
	case LyraKeyboard::RUN: keyboard[Keystates::RUN] = 0; break;
	case LyraKeyboard::JUMP: keyboard[Keystates::JUMP] = 0; break;
	case LyraKeyboard::STRAFE: keyboard[Keystates::STRAFE] = 0; break;
	case LyraKeyboard::SIDESTEP_LEFT: keyboard[Keystates::SIDESTEP_LEFT] = 0; break;
	case LyraKeyboard::SIDESTEP_RIGHT: keyboard[Keystates::SIDESTEP_RIGHT] = 0; break;
	case LyraKeyboard::WAVE: player->SetWaving(false); break;
	case LyraKeyboard::USE:
		if ((player->CurrentAvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) && !(player->GetAccountType() == LmAvatar::ACCT_PMARE))
		{
			player->NightmareAttack();
			return;
		}
		SendMessage(cp->Hwnd_CP(), WM_COMMAND, 0, (LPARAM)cp->Hwnd_Use());
		break;
	case LyraKeyboard::INSPECT:
		if ((cp->SelectedItem() != NO_ITEM) && (actors->ValidItem(cp->SelectedItem()) &&
			(cp->SelectedItem()->Status() == ITEM_OWNED))){
			cp->SelectedItem()->RightClick();
		}
		else{
			LoadString (hInstance, IDS_ITEM_SELECT_INSPECT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			cp->SetSelectedItem(NO_ITEM);
		}
		break;
	case LyraKeyboard::DESELECT:
		cp->DeselectSelected();	
		break;
	case LyraKeyboard::TOGGLE_AUTORUN: 
		options.autorun = !options.autorun;
		if (options.autorun)
			LoadString (hInstance, IDS_RUN_TOGGLE_ON, message, sizeof(message));
		else
			LoadString (hInstance, IDS_RUN_TOGGLE_OFF, message, sizeof(message));

		display->DisplayMessage (message, false);
		SaveInGameRegistryOptionValues();
		break;
	case LyraKeyboard::TOGGLE_ADULT_FILTER: 
		options.adult_filter = !options.adult_filter;
		if (options.adult_filter)
			LoadString (hInstance, IDS_ADULT_FILTER_TOGGLE_ON, message, sizeof(message));
		else
			LoadString (hInstance, IDS_ADULT_FILTER_TOGGLE_OFF, message, sizeof(message));
		display->DisplayMessage (message, false);
		SaveInGameRegistryOptionValues();
		break;

	case LyraKeyboard::TIME_ONLINE: 
	{	
		int minutes, seconds;
		TimeOnline(&minutes, &seconds);
		LoadString (hInstance, IDS_TIME_ONLINE, message, sizeof(message));
		_stprintf(disp_message, message, minutes, seconds);
		display->DisplayMessage(disp_message, false);
		break;
	}

	case LyraKeyboard::SHOW_DST_TIME: 
	{	
		//player->Teleport(-3300, 1700, 0, 39);

		int item_count = 0;
		int total_count = 0;
		for (cItem* item = actors->IterateItems(INIT); item != NO_ITEM; item = actors->IterateItems(NEXT))
		{
			if (item->Status() == ITEM_OWNED)
				item_count++;
			total_count++;
		}
		actors->IterateItems(DONE);
		int jjj = 9999;

		SYSTEMTIME dsttime;
		GetDSTTime(&dsttime);
		LoadString (hInstance, IDS_SHOW_DST, message, sizeof(message));
		if (dsttime.wHour < 12)
			LoadString (hInstance, IDS_AM, temp_message, sizeof(temp_message));
		else {
			dsttime.wHour-=12;
			LoadString (hInstance, IDS_PM, temp_message, sizeof(temp_message));
		}
		_stprintf(disp_message, message, dsttime.wHour, dsttime.wMinute, temp_message, dsttime.wDay, dsttime.wMonth);
		display->DisplayMessage(disp_message, false);
		break;
	}

	case LyraKeyboard::TOGGLE_NAMES:
#ifdef PMARE // no nametags for pmares
		LoadString (hInstance, IDS_PMARE_NONAMES, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
#else
		options.nametags = !options.nametags;
		if (options.nametags)
			LoadString (hInstance, IDS_NAMES_ON, disp_message, sizeof(disp_message));
		else
			LoadString (hInstance, IDS_NAMES_OFF, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message, false);
		SaveInGameRegistryOptionValues();
#endif
		break;
	case LyraKeyboard::SET_ITEM_EFFECT_0:
		if (cp->SelectedItem() != NO_ITEM)
		{
			cp->SelectedItem()->SetSelectedFunction(0);
			LoadString (hInstance, IDS_FUNCTION_SELECTED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, 1, cp->SelectedItem()->Name());
			display->DisplayMessage (message, false);
		}
		break;
	case LyraKeyboard::SET_ITEM_EFFECT_1:
		if (cp->SelectedItem() != NO_ITEM)
		{
			cp->SelectedItem()->SetSelectedFunction(1);
			LoadString (hInstance, IDS_FUNCTION_SELECTED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, 2, cp->SelectedItem()->Name());
			display->DisplayMessage (message, false);
		}
		break;
	case LyraKeyboard::SET_ITEM_EFFECT_2:
		if (cp->SelectedItem() != NO_ITEM)
		{
			cp->SelectedItem()->SetSelectedFunction(2);
			LoadString (hInstance, IDS_FUNCTION_SELECTED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, 3, cp->SelectedItem()->Name());
			display->DisplayMessage (message, false);
		}
		break;
	case LyraKeyboard::DROP_ITEM:
		SendMessage(cp->Hwnd_CP(), WM_COMMAND, 0, (LPARAM)cp->Hwnd_Drop());
		break;

	case LyraKeyboard::SHOW_PARTY:
	{
		for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
		{
			if (gs && gs->Party() && gs->Party()->IsInParty(n->ID()))
			{
			    if(player->IsChannelling() && player->ChannelTarget() == n->ID())
			        LoadString(hInstance, IDS_SHOW_IN_PARTY_CHANNEL, disp_message, sizeof(disp_message));
                else			        
				    LoadString (hInstance, IDS_SHOW_IN_PARTY, disp_message, sizeof(disp_message));
				    
				_stprintf(message, disp_message, n->Name());
				display->DisplayMessage (message, false);
			}
		}
		n = actors->IterateNeighbors(DONE);
	}
		break;

	
	case LyraKeyboard::EMOTE:

	//	player->Teleport (6958, 7522, 979, 46);	// Cup Arena

	//	player->Teleport (6426,2534,0, 46);	// DocA


		if (player->flags & ACTOR_SOULSPHERE)
		{
			LoadString (hInstance, IDS_SOULSPHERE_NO_EMOTE, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
			return;
		}
#ifdef PMARE
		//if (!pmare_talkdlg && ((options.network && gs && gs->LoggedIntoGame()) || options.welcome_ai))
		if (!pmare_talkdlg)
		{
			pmare_talkdlg = TRUE;
			HWND hDlg = CreateLyraDialog(hInstance, IDD_PMARE_TALK,
				cDD->Hwnd_Main(), (DLGPROC)PMareTalkDlgProc);
		}
#else
		//if (!talkdlg && ((options.network && gs && gs->LoggedIntoGame()) || options.welcome_ai))
		if (!talkdlg)
		{
			talkdlg = TRUE;
			HWND hDlg = CreateLyraDialog(hInstance, IDD_TALK,
				cDD->Hwnd_Main(), (DLGPROC)TalkDlgProc);
			Button_SetCheck(GetDlgItem(hDlg, IDC_EMOTE), 1);
			Button_SetCheck(GetDlgItem(hDlg, IDC_TALK), 0);
		}
#endif
		break;
	case LyraKeyboard::OPEN_GOAL_BOOK:
#ifdef PMARE
		LoadString (hInstance, IDS_PMARE_GOALBOOK, message, sizeof(message));
		display->DisplayMessage(message);
		break;

#endif
		if (options.welcome_ai)
		{
			LoadString (hInstance, IDS_COMPLETE_TRAINING, message, sizeof(message));
			display->DisplayMessage(message);
			break;
		}
		if (goalbook) // open goal book
			goalbook->Activate();
		break;
	case LyraKeyboard::LEAVE_PARTY:
		if (options.network && gs && gs->Party())
			gs->Party()->LeaveParty();
		break;
	case LyraKeyboard::SELECT_NEXT:
		cp->SelectNew(SELECT_NEXT_LISTITEM);
		break;
	case LyraKeyboard::SELECT_PREV:
		cp->SelectNew(SELECT_PREV_LISTITEM);
		break;
	case LyraKeyboard::SHOW_NEXT:
		switch (cp->Mode())
		{
		case INVENTORY_TAB:
			cp->ShowNextItem(1);
			break;
		case NEIGHBORS_TAB:
			cp->ShowNextNeighbor(1);
			break;
		case ARTS_TAB:
			cp->ShowNextArt(1);
			break;
		default:
			break;
		}
		break;
	case LyraKeyboard::SHOW_PREV:
		switch (cp->Mode())
		{
			case INVENTORY_TAB:
				cp->ShowPrevItem(1);
				break;
			case NEIGHBORS_TAB:
				cp->ShowPrevNeighbor(1);
				break;
			case ARTS_TAB:
				cp->ShowPrevArt(1);
				break;
			default:
				break;
			}
			break;
	case LyraKeyboard::RESET_EYELEVEL:
		player->ResetEyeHeight();
		break;
	case LyraKeyboard::TOGGLE_SOUND:
		cDS->ToggleSound();
		break;

	case LyraKeyboard::TALK:
#ifdef PMARE
	//	if (!pmare_talkdlg && ((options.network && gs && gs->LoggedIntoGame()) || options.welcome_ai))
		if (!pmare_talkdlg)
		{
			pmare_talkdlg = TRUE;
			HWND hDlg = CreateLyraDialog(hInstance, IDD_PMARE_TALK,
				cDD->Hwnd_Main(), (DLGPROC)PMareTalkDlgProc);
		}
#else
		//if (!talkdlg && ((options.network && gs && gs->LoggedIntoGame()) || options.welcome_ai))
		if (!talkdlg)
		{
			talkdlg = TRUE;
			HWND hDlg = CreateLyraDialog(hInstance, IDD_TALK,
				cDD->Hwnd_Main(), (DLGPROC)TalkDlgProc);
		}
#endif
		break;
	case LyraKeyboard::WHO_NEARBY:
		i = 0;
		for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
			//if (!(n->IsMonster()) && n->Render())
		{
			if (i == 0)
				LoadString (hInstance, IDS_NEIGHBORS_LIST, message, sizeof(message));
			if (n->Avatar().Hidden())
				continue;
			i++;
		_tcscat(message, n->Name());
		_tcscat(message, _T("  "));
		}
		if (i == 0)
			LoadString (hInstance, IDS_NEIGHBORS_NONE, message, sizeof(message));
		
	_tcscat(message,_T("\n"));
		display->DisplayMessage(message, false);
		actors->IterateNeighbors(DONE);
		break;
	case LyraKeyboard::ACTIVE_EFFECTS:
		player->DisplayTimedEffects();
		break;
	case LyraKeyboard::SHOW_XP:
		if (options.welcome_ai)
		{
			LoadString (hInstance, IDS_COMPLETE_TRAINING, message, sizeof(message));
			display->DisplayMessage(message);
			break;
		}
		//LoadString (hInstance, IDS_PLAYER_SHOWXP, disp_message, sizeof(disp_message));
		//_stprintf(message,disp_message,player->XP());
		LoadString (hInstance, IDS_SHOWXP_PPOINT, disp_message, sizeof(disp_message));
		_stprintf(message,disp_message,player->XP(), player->PPoints(), player->PPPool());
		display->DisplayMessage(message, false);
		break;
	case LyraKeyboard::SCROLL_UP:
		switch (cp->Mode())
		{
		case INVENTORY_TAB:
			cp->ShowPrevItem(MAX_LV_ITEMS-1);
			break;
		case NEIGHBORS_TAB:
			cp->ShowPrevNeighbor(MAX_LV_ITEMS-1);
			break;
		case ARTS_TAB:
			cp->ShowPrevArt(MAX_LV_ITEMS-1);
			break;
		default:
			break;
		}
		break;
		case LyraKeyboard::SCROLL_DOWN:
	switch (cp->Mode())
			{
			case INVENTORY_TAB:
				cp->ShowNextItem(MAX_LV_ITEMS-1);
				break;
			case NEIGHBORS_TAB:
				cp->ShowNextNeighbor(MAX_LV_ITEMS-1);
				break;
			case ARTS_TAB:
				cp->ShowNextArt(MAX_LV_ITEMS-1);
				break;
			default:
				break;
			}
			break;
	case LyraKeyboard::PICK_ARTS:
		cp->SetMode(ARTS_TAB, false, true);
		break;
	case LyraKeyboard::PICK_NEIGHBORS:
		cp->SetMode(NEIGHBORS_TAB, false, true);
		break;
	case LyraKeyboard::PICK_INVENTORY:
		cp->SetMode(INVENTORY_TAB, false, true);
		break;
	case LyraKeyboard::SELECT_FROM_LIST:
		// cp selection has been made
		if (((cp->Mode() == INVENTORY_TAB) && (cp->SelectedItem() != NO_ITEM)) ||
			((cp->Mode() == NEIGHBORS_TAB) && (cp->SelectedNeighbor() != NO_ACTOR)) |
			((cp->Mode() == ARTS_TAB) && (cp->SelectedArt() != Arts::NONE)))
			cp->SetSelectionMade(true);
		break;
	case LyraKeyboard::AVATAR_CUSTOMIZATION:
		if (player->flags & ACTOR_SOULSPHERE)
		{
			LoadString (hInstance, IDS_NO_SS_AVATAR_CHANGE, message, sizeof(message));
			display->DisplayMessage(message);
		}
		else if (options.welcome_ai)
		{
			LoadString (hInstance, IDS_COMPLETE_TRAINING, message, sizeof(message));
			display->DisplayMessage(message);
		}
		//else if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY))
		//{
		//LoadString (hInstance, IDS_AVATAR_CHANGE_SANCTUARY, message, sizeof(message));
		//display->DisplayMessage(message);
		//}
		else if (!avatardlg && gs && gs->LoggedIntoGame() &&
			!(player->flags & ACTOR_TRANSFORMED))
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
	case LyraKeyboard::ART:
		cp->SetUsing(true);
		arts->BeginArt(keymap->FindArt(vk));
		cp->SetUsing(false);
		break;
	case LyraKeyboard::MOUSE_LOOK:
		mouselooking = options.mouselook;
		SaveInGameRegistryOptionValues();
		break;
	case LyraKeyboard::SHOW_RANKS:
		{
			if (options.welcome_ai)
			{
				LoadString (hInstance, IDS_COMPLETE_TRAINING, message, sizeof(message));
				display->DisplayMessage(message);
				break;
			}
			
			TCHAR house_descrip[40] = _T("\0");
			TCHAR rank_descrip[320] = _T("\0");
			int ranks = 0;
							
			for (int i = 0; i < NUM_GUILDS; i++)
			{
				if (player->GuildRank(i) >= Guild::INITIATE)
				{
				_stprintf(house_descrip, _T("%s: %s\n"), GuildName(i), RankName(player->GuildRank(i)));
				_tcscat(rank_descrip, house_descrip);
					ranks++;
				}
			}
			
			if (ranks == 0)
			{
				LoadString (hInstance, IDS_NO_RANKS, message, sizeof(message));
				display->DisplayMessage(message);
			}
			else
			{
				TCHAR ranks_message[DEFAULT_MESSAGE_SIZE];
				LoadString (hInstance, IDS_RANK_START, ranks_message, sizeof(ranks_message));
			_tcscat(ranks_message, rank_descrip);
				display->DisplayMessage(ranks_message);
			}
			
		}
		break;
	case LyraKeyboard::SHOW_FOCUS:
		{
			if (options.welcome_ai)
			{
				LoadString (hInstance, IDS_COMPLETE_TRAINING, message, sizeof(message));
				display->DisplayMessage(message);
				break;
			}
			
			LoadString (hInstance, IDS_SHOW_FOCUS, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, player->StatName(player->FocusStat()));
			
			display->DisplayMessage(message);
		}
		break;
	case LyraKeyboard::SHOW_SHIELD:
		{
			if (options.welcome_ai)
			{
				LoadString (hInstance, IDS_COMPLETE_TRAINING, message, sizeof(message));
				display->DisplayMessage(message);
				break;
			}
							
			if (player->ActiveShieldValid())
			{
				LoadString (hInstance, IDS_SHOW_SHIELD, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, player->ActiveShield()->Name());
				display->DisplayMessage(message);
			}
			else
			{
				LoadString (hInstance, IDS_SHOW_SHIELD_NONE, message, sizeof(disp_message));
				display->DisplayMessage(message);
			}
			
		}
		break;
	case LyraKeyboard::IGNORE_LIST:
		if (!ignorelistdlg)
		{
			ignorelistdlg = TRUE;
			CreateLyraDialog(hInstance, IDD_IGNORE_LIST,
				cDD->Hwnd_Main(), (DLGPROC)IgnoreListDlgProc);
		}
		break;
		
	case LyraKeyboard::SHOW_LEARNABLE_ARTS:
		{
#ifdef PMARE
			LoadString (hInstance, IDS_PMARE_LEARNABLE, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
		break;
#endif
			bool has_learnable = false;
			bool can_plateau = false;
			for (int i=0; i<NUM_ARTS; i++)
			{
				if (arts->Learnable(i) && arts->DisplayLearnable(i))
				{
					has_learnable = true;
					break;
				}
				if (arts->CanPlateauArt(i)>0)
				{
					can_plateau = true;
					break;
				}
			}
			if ((has_learnable) || (can_plateau))
			{
				int plat_skill = 0;
				LoadString (hInstance, IDS_LEARNABLE_ARTS, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message);
					
				for (int i=0; i<NUM_ARTS; i++)
				{
					if ((arts->Learnable(i)) && (arts->DisplayLearnable(i)))
					{
						display->DisplayMessage(arts->Descrip(i));
					}

					plat_skill = arts->CanPlateauArt(i);
					if (plat_skill>0)
					{
						int plat = plat_skill/10;
						switch (plat){
							case 1:	_stprintf(message, "first");
								break;
							case 2:	_stprintf(message, "second");
								break;
							case 3:	_stprintf(message, "third");
								break;
							case 4:	_stprintf(message, "fourth");
								break;
							case 5:	_stprintf(message, "fifth");
								break;
							case 6:	_stprintf(message, "sixth");
								break;
							case 7:	_stprintf(message, "seventh");
								break;
							case 8:	_stprintf(message, "eighth");
								break;
							case 9:	_stprintf(message, "ninth");
								break;
							default: _stprintf(message, "next higher");
								break;
							}

						_stprintf(disp_message,"%s to the %s plateau",arts->Descrip(i),message);
						display->DisplayMessage(disp_message);
					}
				}
			}
			else
			{
				LoadString (hInstance, IDS_NO_LEARNABLE_ARTS, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message);
			}
								
		}
		break;

//	case LyraKeyboard::BREAK_TELEPATHY:
		//LeaveChannel();
		//break;
		}
		
	}
	
	return;
}


void Realm_OnChar(HWND hWnd, UINT ch, int cRepeat)
{
	// needed for support of talk dialogs
	return;
}

static void DoOnConfirm(UINT uID, DLGPROC lpDialogFunc)
{
	LoadString (hInstance, uID, message, sizeof(message));
	HWND hDlg;
	hDlg = CreateLyraDialog(hInstance, IDD_WARNING_YESNO,
				cDD->Hwnd_Main(), (DLGPROC)WarningYesNoDlgProc);
	SendMessage(hDlg, WM_SET_CALLBACK, 0,	(LPARAM)lpDialogFunc);

	return;
}

static void DoOnConfirm(TCHAR* pMsg, YESNOCALLBACK lpDialogFunc)
{
	_tcscpy(message, pMsg);
	HWND hDlg;
	hDlg = CreateLyraDialog(hInstance, IDD_WARNING_YESNO,
				cDD->Hwnd_Main(), (DLGPROC)WarningYesNoDlgProc);
	SendMessage(hDlg, WM_SET_CALLBACK, 0,	(LPARAM)lpDialogFunc);

	return;
}

static void LiftCallback(void *value)
{
	int Affirmative = *((int*)value);
	if (Affirmative)
		DoOnConfirm(_T("Lift NoPickup Items too?"), LiftItems);
}

static void LiftItems(void* value)
{
	int Affirmative = *((int*)value);
	for (cItem *item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
	{
		if (cp->InventoryFull())
		{ // carrying too much stuff
			LoadString(hInstance, IDS_ITEM_TOOMUCH, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
			break;
		}

		if (item == NO_ITEM || (item->Status() == ITEM_OWNED) ||
			(item->ItemFunction(0) == LyraItem::WARD_FUNCTION) ||
			(!Affirmative && item->NoPickup()))
			continue;

		item->Get();
	}
	actors->IterateItems(DONE);
}

static void DropCallback(void *value)
{
	int Affirmative = *((int*)value);
	if (Affirmative)
	{
		for (cItem *item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		{
			if (item == NO_ITEM)
				continue;

			if (item->Status() == ITEM_OWNED)
				item->Drop(player->x, player->y, player->angle);
		}
		actors->IterateItems(DONE);
	}
	return;
}

static void JunkCallback(void *value)
{
	int Affirmative = *((int*)value);
	if (Affirmative)
	{
		for (cItem *item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		{
			if (item == NO_ITEM)
				continue;

			if (item->Status() == ITEM_OWNED)
				item->Destroy();
		}
		actors->IterateItems(DONE);
	}
	return;
}
