// The frame creation routine.

// Copyright Lyra LLC, 1996. All rights reserved. 
#define STRICT

#include "Central.h"
#include <windows.h>
#include <stdio.h>

#include "4dx.h"

#include "cActorList.h"
#include "cChat.h"
#include "cGameServer.h"
#include "cDDraw.h"
#include "cControlPanel.h"
#include "cGoalPosting.h" 
#include "cQuestBuilder.h"
#include "cAgentBox.h"
//#include "cBanner.h"
#include "cOutput.h"
#include "cParty.h"
#include "Utils.h" 
#include "cLevel.h"
#include "Interface.h"
#include "cPlayer.h"
#include "cItem.h"
#include "Move.h"
#include "Dialogs.h"
#include "Resource.h"
#include "Realm.h"
#include "cDSound.h"
#ifdef AGENT
#include "cAI.h"
#endif

#include "Options.h"
#include "cPalettes.h"

/////////////////////////////////////////////////
//External Global Variables

extern cDDraw *cDD; // Direct Draw object
extern cDSound *cDS;
extern cControlPanel *cp ; // Control Panel
extern cGameServer *gs;
extern cArts *arts;
extern cActorList *actors; // List of actors
extern cPlayer *player;  // player object
extern cChat *display;
extern options_t options;
extern bool	ready; // init'd?
extern bool	exiting; // finished?
extern bool framerate; // display frame rate
extern bool lost_server; // true when server connection lost
extern bool showing_map,map_shows_current_level;
extern unsigned long exit_time;
//extern cBanner *banner; 
extern cGoalPosting *goals; // goal posting object
extern cQuestBuilder *quests; // quest builder object
extern cPaletteManager *shader;
extern cAgentBox *agentbox;
extern timing_t *timing;
extern HINSTANCE hInstance;
extern bool acceptrejectdlg;
// extern bool exit_switch_task;
extern DWORD last_keystroke;
extern unsigned long *origcolors;
extern bool IsLyraColors;

//////////////////////////////////////////////////////
const unsigned int	POSITION_UPDATE_INTERVAL = 100; // server position updates
const unsigned int  PLAYER_STATS_UPDATE_INTERVAL = 30000; // other server updates
const unsigned int  PACKET_CHECK_INTERVAL = 120000;
const unsigned int  PACKET_COUNT_INTERVAL = 1000;
const unsigned int  CHECK_DRAG_SCROLL_INTERVAL = 1000;
const unsigned int  CHECK_INV_COUNT_INTERVAL = 500;
bool show_splash = false;
unsigned int show_splash_end_time = 0;
#ifndef GAMEMASTER
const unsigned int  MAX_IDLE_TIME = 900000;
#else
const unsigned int MAX_IDLE_TIME = 1200000;
#endif

#ifdef AGENT
#define MIN_MSECS_PER_FRAME 80
#else
#ifndef PROFILE
	#define MIN_MSECS_PER_FRAME  38
#else
	#define MIN_MSECS_PER_FRAME  17
#endif
#endif


#ifndef AGENT
void RenderMap(unsigned char *viewBuffer, bool show_players_position);
void RenderEffects(unsigned char* viewBuffer);
void RenderView(void)
{
  if (cDD) 
  {
		unsigned char *viewBuffer = cDD->GetSurface(BACK_BUFFER);
    
		if (cp->DragItem() != NO_ITEM) 
		{
			POINT cursor;
			GetCursorPos(&cursor);
			SetPickScreenCo_ords(cursor.x - cDD->XOffset(),cursor.y - cDD->YOffset());
		}
		
		if (!showing_map)
			BuildViewBySector(player,viewBuffer,cDD->Pitch());
		else
			RenderMap(viewBuffer,map_shows_current_level);


		// draw the icon on the viewport - not needed with Win10 changes
//		if (cp->DragItem() != NO_ITEM) 
//			cp->DrawDrag(true, viewBuffer);

		cDD->ReleaseSurface(BACK_BUFFER);

  }		
}

#endif

//#define FRAME_DEBUG
static void BlitView(void)
{
	if (cDD) 
		cDD->BlitOffScreenSurface();	
}


static void AdjustCursor(void)
{
	POINT cursor;
	RECT  view_rect;
	// RECT  cursor_view_rect;

	static bool cursor_visible = TRUE;
	static POINT prev_cursor;

	GetCursorPos(&cursor);
	cDD->ViewRect(&view_rect);
	
	int jump_zone_x = view_rect.right - 24;
	int jump_zone_y = view_rect.bottom - 24;
	
	if (prev_cursor.x <= jump_zone_x && cursor.x > jump_zone_x && cursor.y < view_rect.bottom)
		cursor.x = view_rect.right;
	else if (prev_cursor.x >= view_rect.right && cursor.x < view_rect.right && cursor.y < view_rect.bottom)
	  cursor.x = jump_zone_x;

	if (prev_cursor.y <= jump_zone_y && cursor.y > jump_zone_y && cursor.x < view_rect.right)
		cursor.y = view_rect.bottom;
	else if (prev_cursor.y >= view_rect.bottom && cursor.y <= view_rect.bottom && cursor.x < view_rect.right)
	  cursor.y = jump_zone_y;
	
	prev_cursor = cursor;
  SetCursorPos(cursor.x,cursor.y);
	bool cursor_in_viewport = PtInRect(&view_rect,cursor);

  if (cursor_in_viewport)
	{
		if (cursor_visible)
			ShowCursor(FALSE);
		cursor_visible = FALSE;
	}
	else
	{
		if (!cursor_visible)
			ShowCursor(TRUE);
		cursor_visible = TRUE;
	}
}

// break it out for profiling
void AgentSleep(void) 
{
	Sleep(2000);
}

void __cdecl CreateFrame(void)
{
   double frameRate;
   cActor *actor;
//	AdjustCursor();	

#ifndef AGENT
//#ifndef UL_DEBUG
//#ifndef GAMEMASTER
   HWND focus_hwnd = GetFocus();
   if ((focus_hwnd == NULL) && (IsLyraColors == TRUE) && (origcolors))
	{
		SetSysColors(11, syscolors, origcolors);
		IsLyraColors = FALSE;
	}
   
#endif
//#endif
//#endif

#ifndef AGENT
	#ifndef UL_DEBUG
	// uncomment for no afk in goal posting
	//if (goals->Active())
	//	last_keystroke = LyraTime();
	//else 
	if (LyraTime() - last_keystroke > MAX_IDLE_TIME)
	{
		exiting = true;
		gs->Logout(GMsg_Logout::LOGOUT_NORMAL, true);
		LoadString(hInstance, IDS_IDLE_TIMEOUT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, (MAX_IDLE_TIME / 1000 / 60));
		display->DisplayMessage(message);

	    LyraDialogBox(hInstance, IDD_FATAL_ERROR, NULL, (DLGPROC)FatalErrorDlgProc);
		Exit();
		exit(-1);
	}
	#endif
#endif

   if (!ready || exiting || ((LyraTime() - timing->lastFrame) < MIN_MSECS_PER_FRAME))
   {
	   Sleep(0); // give up control
	   return;
   }

   if (exit_time < LyraTime())
   {
	    exiting = true;
/*		if (exit_switch_task)
		{
			LoadString (hInstance, IDS_SWITCH_TASK, message, sizeof(message));
			LyraDialogBox(hInstance, IDD_FATAL_ERROR, NULL, (DLGPROC)FatalErrorDlgProc);
		} */
		Exit();
		exit(-1);
   }

   if (actors)
		 actors->Purge();
  
	timing->lastFrame = LyraTime();


	if ((timing->lastFrame > timing->lastDragScrollCheck + CHECK_DRAG_SCROLL_INTERVAL) &&
		cp && (cp->DragItem() != NO_ITEM))
	{
		cp->CheckDragScroll();
		timing->lastDragScrollCheck = timing->lastFrame;
	}

	// look for tampering with player record
	player->ValidateChecksums();

	arts->CheckTarget();

	// Check for arts *before* the time based network events
	// so arts can send updates without timing interference
	// check to see if an art was just completed
	if ((arts->Casting()) && (arts->ArtCompletionTime() < LyraTime()))
	{
		// if we're networked and haven't fully logged into a room yet,
		// delay the art completion time until we have
      bool ready_to_evoke = true;
	  if (options.network && gs) {
		  if (!gs->LoggedIntoRoom() || !gs->GotPeerUpdates())
			ready_to_evoke = false;
	  }
	  if (ready_to_evoke)
		arts->ApplyArt();
	}

	// check to see if a selection was made for a waiting art
	if (arts->WaitingForSelection())
		arts->CheckForSelection();
   


	// check on time based network events
	if (options.network && gs)
	{
		if (gs->LoggedIntoLevel())
		{
			if 	(timing->lastFrame > timing->lastPositionUpdate + POSITION_UPDATE_INTERVAL)
			{
				gs->SendPositionUpdate(TRIGGER_TIMER);
				timing->lastPositionUpdate = timing->lastFrame;
			}
			//else 
			///_tprintf("not updating - last frame time = %d, last pos update = %d, interval = %d\n", 
			//		timing->lastFrame, timing->lastPositionUpdate, POSITION_UPDATE_INTERVAL);
#ifdef PMARE
			//_tprintf("now: %d logout: %d\n", LyraTime(), options.pmare_logout_time);
			if (LyraTime() > options.pmare_logout_time)
			{
				// collapse the pmare on its way out
				player->SetCurrStat(Stats::DREAMSOUL, 0, SET_ABSOLUTE, Lyra::ID_UNKNOWN);

				exiting = true;
				gs->Logout(GMsg_Logout::LOGOUT_NORMAL, true);
				LoadString (hInstance, IDS_PMARE_TIMEOUT, message, sizeof(message));
				LyraDialogBox(hInstance, IDD_FATAL_ERROR, NULL, (DLGPROC)FatalErrorDlgProc);
				Exit();
				exit(-1);
			}
#endif
		}
		//else
		///_tprintf("not updating - not logged in\n");

#ifndef AGENT
		if (timing->lastFrame > timing->lastServerUpdate + PLAYER_STATS_UPDATE_INTERVAL)
		{
			gs->UpdateServer();
			timing->lastServerUpdate = timing->lastFrame;
		}

		if (timing->lastFrame > timing->lastPacketCount + PACKET_COUNT_INTERVAL)
		{
			int interval = timing->lastFrame - timing->lastPacketCount;
			if ((interval < 0) || (interval > 5000))
				interval = 0; // something is wrong...
			gs->UpdateExpectedPackets(interval);
			timing->lastPacketCount = timing->lastFrame;
		}

		if (timing->lastFrame > timing->lastPacketCheck + PACKET_CHECK_INTERVAL)
		{
			gs->CheckUDPConnectivity();
			timing->lastPacketCheck = timing->lastFrame;
		}


		if (gs && gs->Party() && gs->Party()->NumRequests() &&
			gs->LoggedIntoLevel() && !acceptrejectdlg)
			gs->Party()->DialogQuery(); // pop up dialog for next request
#endif // agent

    }
//	else
		//_tprintf("not updating - not using networking\n");

	if (player->AttemptingTeleport())
		player->RetryTeleport();

#ifndef AGENT

	if (player->Teleporting())
	{
		const int TELEPORT_TICKS = 50; 
		static int teleport_ticks = TELEPORT_TICKS;
		teleport_ticks += timing->nmsecs;
		if (teleport_ticks >= TELEPORT_TICKS)
		{
			teleport_ticks = 0;
			static float brightness = 0.0;
			const float brightness_rate = 0.05f;
			shader->SetGlobalBrightness(brightness);
			brightness += brightness_rate;
			if (brightness > 1.1) 
			{
				player->SetTeleporting(false);
				brightness = 0.0;
				teleport_ticks = TELEPORT_TICKS; 
			}
		}
	}

#endif


///_tprintf("creating frame at %d\n",timing->lastFrame);

   SetCounter();

#ifdef FRAME_DEBUG
	output->ReInit(); // wipe out the existing debug.out
	LoadString (hInstance, IDS_RENDER_VIEW, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
_tprintf(disp_message);
#endif
		InitCollisionDetection();


#ifndef AGENT
   if ( player->flags & ACTOR_BLINDED)
	   cDD->EraseSurface(BACK_BUFFER); // blind, show black screen
   else 
   {
		// if in the game, but not the level, only render
		// if we're in character creation or the entrance level
	   if (show_splash)
	   {
			cDD->ShowSplashScreen();
			if (show_splash_end_time < LyraTime())
			{
				show_splash = false;
				cDS->StopSound(LyraSound::INTRO);
				cDS->ReleaseBuffer(LyraSound::INTRO);
				cDS->PlaySound(LyraSound::ENTRY);
			}
	   }
		else if (!options.network ||
			((options.welcome_ai || (gs && gs->LoggedIntoGame())) &&
			 ((gs && gs->LoggedIntoLevel()) || (level->ID() == START_LEVEL_ID))))
			RenderView();
		else  if (!lost_server)
			cDD->ShowIntroBitmap();
   }

/*		if (cp->DragItem() != NO_ITEM) // if dragging an item
		{
			unsigned char *viewBuffer = cDD->GetSurface(PRIMARY);
			if (cp->UndrawDrag(viewBuffer))
				cp->DrawDrag(false, viewBuffer);
			cDD->ReleaseSurface(PRIMARY);
		}
*/
// draw the item off the viewport
   if (options.show_effects_hud)
   {
	   cp->ShowEffectsHUD(true);
	   cp->UpdateEffects();
   }
   else
	   cp->ShowEffectsHUD(false);
   
   if (!goals->Active() && !quests->Active())
		BlitView(); // always blit, unless we're in goal posting mode

#ifdef FRAME_DEBUG
	LoadString (hInstance, IDS_BLIT_VIEW, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
_tprintf(disp_message);
#endif
	if(timing->lastFrame > timing->lastInvCountUpdate + CHECK_INV_COUNT_INTERVAL)
	{
		timing->lastInvCountUpdate = timing->lastFrame;
		cp->UpdateInvCount();
	}
	/*
	if (timing->lastFrame > timing->lastEffectRender + 2000)
	{
		timing->lastEffectRender = timing->lastFrame;
		cp->UpdateEffects();
	}
	*/
#endif
	
   timing->frames++;

#ifdef FRAME_DEBUG
	LoadString (hInstance, IDS_ACTOR_UPDATE, disp_message, sizeof(disp_message));
_stprintf(disp_message, actors->NumActors());
	display->DisplayMessage(disp_message);
_tprintf(disp_message);
#endif

#if defined (GAMEMASTER)
   if (agentbox)
	   agentbox->Update();
#endif
  
	if (actors) 
	{
		for (actor=actors->IterateActors(INIT); actor != NO_ACTOR; actor=actors->IterateActors(NEXT))
			if (!(actor->IsPlayer()) && !actor->Update())
				actor->SetTerminate();
		actors->IterateActors(DONE);

		player->Update();

		actors->Purge();
	}

	if (framerate)
	{
		if (timing->frameCount && ((LyraTime() - timing->lastDisplayTime) > 1000))
		{
			frameRate =  (1000.0f*timing->frameCount)/(LyraTime() - timing->lastDisplayTime);
#if  defined (GAMEMASTER)
#ifdef AGENT
			agent_info[AgentIndex()].framerate = (int)frameRate;

#else
			LoadString (hInstance, IDS_FRAME_RATE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, frameRate);
			display->DisplayMessage(message);
		_tprintf(message);

#endif
#endif
			timing->lastDisplayTime = LyraTime();
			timing->frameCount = 0;
		}
		else if (timing->lastDisplayTime == 0)
		{
			timing->lastDisplayTime = LyraTime();
			timing->frameCount = 0;
		} 
		else
			timing->frameCount++;

	}
	else
		timing->lastDisplayTime = 0;

#ifdef AGENT
	if (((cAI*)player)->Alone())
		AgentSleep();
#endif

#ifdef FRAME_DEBUG
	LoadString (hInstance, IDS_FIN_FRAME, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message);
_tprintf(disp_message);
#endif
}

