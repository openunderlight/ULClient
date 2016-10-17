 // The Player Class

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "Central.h"
#include <Windows.h>
#include <string.h>
#include "cDDraw.h"
#include "cDSound.h"
#include "cChat.h"
#include "cActorList.h"
#include "cControlPanel.h"
#include "cGameServer.h"
#include "cActor.h"
#include "cEffects.h"
#include "cLevel.h"
#include "cItem.h"
#include "cOrnament.h"
#include "cArts.h"
#include "cParty.h"
#include "Realm.h"
#include "Move.h"
#include "4dx.h"
#include "Dialogs.h"
#include "Mouse.h"
#include "Options.h"
#include "cPlayer.h"
#include "cPalettes.h"
#include "Utils.h"
#include "resource.h"
#include "keyboard.h"
#include "cAgentBox.h"
#include "LmStats.h"
#include "LmXPTable.h"
extern xp_entry lyra_xp_table[];


#ifdef AGENT
#include "cAI.h"
#endif

const int PLAYER_WALK_ANIMATE_TICKS  = 84;
const int PLAYER_RUN_ANIMATE_TICKS	 = 56;
const int PLAYER_BLADE_ANIMATE_TICKS = 1;

// # of actor-collision-free moves after a teleport
const unsigned int NUM_FREE_MOVES	 = 5;
const int HEAL_INTERVAL = 5000; // natural healing
const int SECTOR_TAG_INTERVAL = 2000; // apply sector tags every 2 seconds
const int NIGHTMARE_CHECK_INTERVAL = 1000; // for area effect mare stuff
const int POISON_INTERVAL = 4000;
const int BLEED_INTERVAL = 3000;
const int TRAIL_INTERVAL = 5000;
const float MIN_TRAIL_SEPARATION = 300.0f;
const float UPDOWNVEL=12.0f; // speed at which we look/down
const int HORRON_FEAR_DISTANCE = 150000;
const int HORRON_DRAIN_DISTANCE = 50000;
const int THRESHOLD_QUAD = 6;
const int MAGIC_XOR_VAR = 0xf801;

//////////////////////////////////////////////////////////////////
// External Global Variables

extern cDDraw *cDD;
extern cDSound *cDS;
extern cGameServer *gs;
extern cEffects *effects;
extern cActorList *actors;
extern cPlayer *player;
extern cChat *display;
extern cArts *arts;
extern cControlPanel *cp;
extern HINSTANCE hInstance;
extern unsigned char keyboard[num_keystates];
extern options_t options;
extern cLevel *level;
extern mouse_move_t mouse_move;
extern mouse_look_t mouse_look;
extern timing_t *timing;
extern cTimedEffects *timed_effects;
extern avatar_poses_t dreamer_poses[];
extern avatar_poses_t mare_poses[];
extern bool ready;
extern bool acceptrejectdlg;
extern cPaletteManager *shader;
extern bool showing_map;
extern cAgentBox *agentbox;
extern HWND hwnd_acceptreject;
extern ppoint_t pp; // personality points use tracker


/////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
cPlayer::cPlayer(short viewport_height) :
		cActor(0.0f, 0.0f, 0, 0,  PLAYER)
{
	playerID = INVALID_PLAYERID;

	// set vertical tilt members here; rest handled in Init method
	vertical_tilt_origin = viewport_height/2;
	max_vertical_tilt 	= viewport_height ;
	vertical_tilt 			= vertical_tilt_origin;
	vertical_tilt_float	= (float)vertical_tilt_origin;
	collapse_time = 0;
	checksum_incorrect = attempting_teleport = false;

	for (int i=0; i<COLLAPSES_TRACKED; i++) 
	{
		collapses[i].collapser_id = Lyra::ID_UNKNOWN;
		collapses[i].time = 0;
	}
	next_collapse_index = 0;

	return;
}


void cPlayer::InitPlayer(void)
{
	int i;

	orbit = 0;
	this->SetAvatar(options.avatar, false);
	avatar_poses = dreamer_poses;
	if (avatar_poses[STANDING].start_frame < frames)
		currframe = avatar_poses[STANDING].start_frame; // for normal avatars
	start_x = start_y = 0.0f;
	start_angle = start_level = room = 0;
	xp = last_footstep = quest_xp_pool = 0;
	speed = WALK_SPEED;
	reset_speed = turnrate = 0.0f;
	num_items = 0;
	selected_stat = Stats::DREAMSOUL;
	focus_stat = Stats::WILLPOWER;
	injured = teleporting = jumped = waving = old_sanct = false;
	next_heal = next_regeneration = LyraTime() + HEAL_INTERVAL;
	next_sector_tag = LyraTime() + SECTOR_TAG_INTERVAL;
	next_nightmare_check = LyraTime() + NIGHTMARE_CHECK_INTERVAL;
	next_poison = next_bleed = next_trail = 0;
	free_moves = 5;
	channelTarget = 0;
	step_frame = avatar_poses[WALKING].start_frame;
	checksum_incorrect = last_loc_valid = false;
	item_flags_sorting_changed = false;
	//  Initialize curse_strength to zero
	curse_strength = 0; // no curse on new player
	blast_chance = 0; // no chance Ago will return Blast to start
	poison_strength = 0;
	reflect_strength = 0;
 	last_poisoner = last_bleeder = Lyra::ID_UNKNOWN;
	gamesite = GMsg_LoginAck::GAMESITE_LYRA;
	gamesite_id = 0;
	session_id = 0;

	for (i=0; i<NUM_GUILDS; i++)
		guild_ranks[i].rank = Guild::NO_RANK;

	for (i=0; i<NUM_PLAYER_STATS; i++)
	{
		stats[i].current = stats[i].max = Stats::STAT_MIN;
		stats[i].current_needs_update = false;
	}

	LoadString(hInstance, IDS_DREAMSOUL, stats[Stats::DREAMSOUL].name, 16);
	LoadString(hInstance, IDS_WILLPOWER, stats[Stats::WILLPOWER].name, 16);
	LoadString(hInstance, IDS_INSIGHT, stats[Stats::INSIGHT].name, 16);
	LoadString(hInstance, IDS_RESILIENCE, stats[Stats::RESILIENCE].name, 16);
	LoadString(hInstance, IDS_LUCIDITY, stats[Stats::LUCIDITY].name, 16);

	for (i=0; i<NUM_ARTS; i++)
	{
		skills[i].skill = 0;
		skills[i].needs_update = false;
	}

#ifndef AGENT
	playerID = Lyra::ID_UNKNOWN;
#endif// AGENT
	last_party_leader = Lyra::ID_UNKNOWN;
	next_heal = LyraTime() + HEAL_INTERVAL;
	active_shield = NO_ITEM;

	_tcscpy(upper_name, options.username[options.account_index]);
	_tcsupr(upper_name);

	using_blade = false;
	blade_position_index = 0;
	blade_ticks = 0;
	pp_pool = ppoints = granting_pp = 0;

	hit = false;

	// Jared 6-25-00
	// Don't babble pmares. They have preset speach and emotes now
//#ifdef PMARE
//		options.babble_filter = true; 
//#else		
		options.babble_filter = false; 
//#endif

}


TCHAR* cPlayer::Name(void)
{
	return options.username[options.account_index];
}

TCHAR* cPlayer::Password(void)
{
	return options.password[options.account_index];
}

// set start x,y,angle for later use on death
void cPlayer::SetStartPos(float starting_x, float starting_y, int starting_angle, int starting_level)
{
	start_x = starting_x;
	start_y = starting_y;
	start_angle = starting_angle;
	start_level = starting_level;
	eyeheight = (float)(z - (physht*.2));

	return;
}

void cPlayer::ResetEyeHeight(void)
{
 if ( vertical_tilt != vertical_tilt_origin)
 {
	reset_speed = timing->nticks*(float)UPDOWNVEL;
	if (vertical_tilt_float > vertical_tilt_origin )
		reset_speed = -reset_speed;
 }
	return;
}

// Returns the height delta for missiles fired by the player
int cPlayer::HeightDelta(void)
{
	 return (int)((vertical_tilt - vertical_tilt_origin)/42);
}

bool cPlayer::Update(void)
{
	int moveangle = 0;
	int aticks = 0;
	realmid_t lastRoom;
	float old_velocity = velocity;
	float old_x = x;
	float old_y = y;
	float xheight;
	bool move;	// true only when player can be moved by keyboard/mouse
  bool spin = true; // true only when player can spin
	bool animate = false;
	forced_move_t forced_move = {false,false,false,false,false};
	bool was_in_water = this->InWater();

	if (terminate)
		return false;

	this->ModifyHeight();

	this->CheckStatus();

	this->CheckMissile();

	if (UsingBlade())
	{
		blade_ticks += timing->nmsecs;
		if (blade_ticks >  PLAYER_BLADE_ANIMATE_TICKS)
		{
			blade_ticks = 0;
			blade_position_index++;
		}
	 }

//extern int opt_on;
	if ( Hit())
// if (opt_on || Hit())
	{
		static float brightness = 0.3f;
		const float brightness_rate = 0.005f;

		shader->SetGlobalBrightness(brightness);
		brightness += timing->nmsecs *  brightness_rate;
		if (brightness > 1.1)
		{
			shader->SetGlobalBrightness(1.0);
			brightness = 0.3f;
			SetHit(false);
			//opt_on = false;
		}
	}

	xheight = level->Sectors[sector]->FloorHt(x,y)+level->Sectors[sector]->HtOffset+physht;

	if (flags & ACTOR_TRANSFORMED)
		speed = SHAMBLE_SPEED;
	// Jared 2-26-00
	// XOR autorun with keyboard shift so that holding shift with autorun on
	// will set speed to WALK, and vise versa
	else if ((options.autorun + keyboard[Keystates::RUN])%2
			|| mouse_move.shift || mouse_look.shift
			|| (flags & ACTOR_SCARED)) // set speed
		speed = RUN_SPEED;
	else
		speed = WALK_SPEED;

#ifdef UL_DEBUG // super fast run for level building
	if (!options.network)
	{
		if (speed == RUN_SPEED)
			speed = RUN_SPEED*3;
		else
			speed = RUN_SPEED;
	}
#endif

	strafe = NO_STRAFE;

	move = false;

	if (!options.network ||
			((options.welcome_ai || (gs && gs->LoggedIntoGame()))  && 
			  ((gs && gs->LoggedIntoLevel()) || (level->ID() == START_LEVEL_ID))))
		move = true;

	if ((flags & ACTOR_PARALYZED))//  || (cp->DragItem() != NO_ITEM))
		move = false;

  if (flags & ACTOR_SPIN) {
    spin = false;
    forced_move.left = true;
  }

	if (flags & ACTOR_SCARED)
		forced_move.forward = true;

	if (flags & ACTOR_DRUNK)	// force movement
		switch (rand()%6)
		{
			case 0:
				forced_move.strafe = true;
			case 1:
				forced_move.left = true;
				break;
			case 2:
				forced_move.strafe = true;
			case 3:
				forced_move.right = true;
				break;
			case 4:
				forced_move.forward = true;
				break;
			case 5:
				forced_move.backward = true;
				break;
		}

	 turnrate = 0.0f;

	if (forming || dissolving)
		aticks = ANIMATION_TICKS;
	else if ((velocity < -MAXWALK) || (velocity > MAXWALK))
		aticks = PLAYER_RUN_ANIMATE_TICKS;
	else
		aticks = PLAYER_WALK_ANIMATE_TICKS;

	animate_ticks += timing->nmsecs;

	if (animate_ticks > aticks)
	{
		animate_ticks = 0;
		animate = true;
		if (forming || dissolving) // update control panel for death/rebirth sequence
		{
			if (++currframe >=  effects->EffectFrames(LyraBitmap::FORMREFORM_EFFECT))
			{	// make them real again
				forming = dissolving = false;
				currframe = avatar_poses[STANDING].start_frame;
			}
			cp->AddAvatar();
		}
		if ((velocity < MAXWALK) && (velocity > -MAXWALK))
			step_frame = 0;
		if (++step_frame > avatar_poses[WALKING].end_frame)
			step_frame = avatar_poses[WALKING].start_frame;
	}

	if (move)
	{
    if (!spin && keyboard[Keystates::STRAFE]) // disregard
      keyboard[Keystates::STRAFE] = 0;
		if ((keyboard[Keystates::STRAFE] || forced_move.strafe) &&
			(keyboard[Keystates::TURN_RIGHT] || mouse_move.right || keyboard[Keystates::TURN_LEFT]
			|| mouse_move.left || forced_move.right || forced_move.left)) // strafing?
		{
			if (keyboard[Keystates::TURN_RIGHT] || mouse_move.right || forced_move.right)
			{ // handle angles for combined strafe / forwards / backwards move
				if (keyboard[Keystates::MOVE_FORWARD] || mouse_move.forward || forced_move.forward)
					moveangle = FixAngle(angle + Angle_45);
				else if (keyboard[Keystates::MOVE_BACKWARD] || mouse_move.backward || forced_move.backward)
					moveangle = FixAngle(angle - Angle_45);
				else moveangle = FixAngle(angle + Angle_90);
				strafe = STRAFE_RIGHT;
			}

			if (keyboard[Keystates::TURN_LEFT] || mouse_move.left || forced_move.left)
			{ // handle angles for combined strafe / forwards / backwards move
				if (keyboard[Keystates::MOVE_FORWARD] || mouse_move.forward || forced_move.forward)
					moveangle = FixAngle(angle - Angle_45);
				else if (keyboard[Keystates::MOVE_BACKWARD] || mouse_move.backward || forced_move.backward)
					moveangle = FixAngle(angle + Angle_45);
				else moveangle = FixAngle(angle - Angle_90);
				strafe = STRAFE_LEFT;
			}
		}
		else if (keyboard[Keystates::SIDESTEP_LEFT] || keyboard[Keystates::SIDESTEP_RIGHT]) // sidestep keys
		{
//			INFO("SSL");
				// allow turns in combination with sidesteps
			if (keyboard[Keystates::TURN_RIGHT] || mouse_move.right || mouse_look.right || forced_move.right)
			{
				if (mouse_look.looking)
					turnrate+=(timing->nticks*speed*options.turnrate*mouse_look.xratio);
				else if (mouse_move.moving)
					turnrate+=(timing->nticks*speed*options.turnrate*mouse_move.xratio);
				else
					turnrate+=(timing->nticks*speed*options.turnrate);
			}
			else if (keyboard[Keystates::TURN_LEFT] || mouse_move.left || mouse_look.left || forced_move.left)
			{
				if (mouse_look.looking)
					turnrate-=(timing->nticks*speed*options.turnrate*mouse_look.xratio);
					else if (mouse_move.moving)
					turnrate-=(timing->nticks*speed*options.turnrate*mouse_move.xratio);
				else
					turnrate-=(timing->nticks*speed*options.turnrate);
			}

			if (turnrate > (options.turnrate*speed))
				turnrate = options.turnrate*speed;
			else if (turnrate < -(options.turnrate*speed))
				turnrate = -options.turnrate*speed;
			angle = FixAngle(angle+(int)(turnrate));
			moveangle = angle;

			if (keyboard[Keystates::SIDESTEP_RIGHT])
			{ // handle angles for combined strafe / forwards / backwards move
				if (keyboard[Keystates::MOVE_FORWARD] || mouse_move.forward || forced_move.forward)
					moveangle = FixAngle(angle + Angle_45);
				else if (keyboard[Keystates::MOVE_BACKWARD] || mouse_move.backward || forced_move.backward)
					moveangle = FixAngle(moveangle - Angle_45);
				else moveangle = FixAngle(moveangle + Angle_90);
				strafe = STRAFE_RIGHT;
			}

			if (keyboard[Keystates::SIDESTEP_LEFT])
			{ // handle angles for combined strafe / forwards / backwards move
				if (keyboard[Keystates::MOVE_FORWARD] || mouse_move.forward || forced_move.forward)
					moveangle = FixAngle(angle - Angle_45);
				else if (keyboard[Keystates::MOVE_BACKWARD] || mouse_move.backward || forced_move.backward)
					moveangle = FixAngle(moveangle + Angle_45);
				else moveangle = FixAngle(moveangle - Angle_90);
				strafe = STRAFE_LEFT;
			}

		}
		else // plain old turning
		{
      if (!spin)
        keyboard[Keystates::TURN_RIGHT] = 0;

			  if (keyboard[Keystates::TURN_RIGHT] || mouse_move.right || mouse_look.right || forced_move.right)
			  {
				  if (mouse_look.looking)
					  turnrate+=(timing->nticks*speed*options.turnrate*mouse_look.xratio);
					else if (mouse_move.moving)
					  turnrate+=(timing->nticks*speed*options.turnrate*mouse_move.xratio);
				  else
					  turnrate+=(timing->nticks*speed*options.turnrate);
			  }
    
		  	else if (keyboard[Keystates::TURN_LEFT] || mouse_move.left || mouse_look.left || forced_move.left)
			  {
				  if (mouse_look.looking)
					  turnrate-=(timing->nticks*speed*options.turnrate*mouse_look.xratio);
				  else if (mouse_move.moving)
					  turnrate-=(timing->nticks*speed*options.turnrate*mouse_move.xratio);
				  else
					  turnrate-=(timing->nticks*speed*options.turnrate);
			  }

			if (turnrate > (options.turnrate*speed))
				turnrate = options.turnrate*speed;
			else if (turnrate < -(options.turnrate*speed))
				turnrate = -options.turnrate*speed;
			angle = FixAngle(angle+(int)(turnrate));
			moveangle = angle;
		}
	}


	if (move && (keyboard[Keystates::MOVE_FORWARD] || mouse_move.forward || forced_move.forward))
		velocity = MAXWALK;
	else if (move && (keyboard[Keystates::MOVE_BACKWARD] || mouse_move.backward || forced_move.backward))
		velocity = -MAXWALK;
	else if (move && ((keyboard[Keystates::SIDESTEP_RIGHT] || keyboard[Keystates::SIDESTEP_LEFT]) ||
		((keyboard[Keystates::STRAFE] || forced_move.strafe) &&
		(keyboard[Keystates::TURN_RIGHT] || mouse_move.right || forced_move.right
		|| keyboard[Keystates::TURN_LEFT] || mouse_move.left || forced_move.left))))
		velocity = MAXSTRAFE;
	else if ((z > xheight - (0.1*physht)) && (z < xheight + (0.1*physht)))
	{
		if ( velocity > 0)
		{
			velocity -= MAXWALK/6;
			if (velocity < 0)
				velocity = 0.0f;
		}
		else if ( velocity < 0)
		{
			velocity +=MAXWALK/6;

			if (velocity > 0)
				velocity = 0.0f;
		}
	}
	if (velocity)
	{
		lastRoom = room;
		if (strafe != NO_STRAFE)
		{ // make sure deceleration doesn't drag player forward after a strafe
			MoveActor(this,moveangle,velocity*timing->nticks*speed,MOVE_NORMAL);
			velocity = 0.0f;
		}
		else
			MoveActor(this,moveangle,velocity*timing->nticks*speed,MOVE_NORMAL);
		if (free_moves)
			free_moves--;

		///_tprintf("setting room...\n");
		this->SetRoom(old_x, old_y);
		if (options.network && gs->LoggedIntoGame() && (lastRoom == room) && // didn't change room; presend?
			((velocity > 0 && old_velocity == 0)))
			gs->SendPositionUpdate(TRIGGER_MOVE);
		if ((z == xheight) && options.footsteps && animate &&
			((velocity >= MAXWALK) || (velocity <= -MAXWALK)) &&
			!(flags & ACTOR_SOULSPHERE) && !(flags & ACTOR_TRANSFORMED))
		{	// footstep possible
			if (step_frame == 6)
			{
				if (this->InWater())
					cDS->PlaySound(LyraSound::WATER_STEP_1, x, y, false);
				else
					cDS->PlaySound(LyraSound::PLAYER_STEP_1, x, y, false);
				last_footstep = LyraTime();
			}
			else if (step_frame == 2)
			{
				if (this->InWater())
					cDS->PlaySound(LyraSound::WATER_STEP_2, x, y, false);
				else
					cDS->PlaySound(LyraSound::PLAYER_STEP_2, x, y, false);
				last_footstep = LyraTime();
			}
		}
		this->PerformedAction();
	}
	else if (options.network && gs->LoggedIntoGame() && old_velocity)
		// preupdate for slowdown
		gs->SendPositionUpdate(TRIGGER_MOVE);

	if (!was_in_water && this->InWater() && !(flags & ACTOR_SOULSPHERE) &&
		((z > xheight - (.1*physht)) && (z < xheight + (.1*physht))))
			cDS->PlaySound(LyraSound::ENTER_WATER, x, y, false);

	if (keyboard[Keystates::TRIP] && move)
	{
	  keyboard[Keystates::TRIP] = 0;
	  //_tprintf("checking trip...\n");
	  this->PerformedAction();
	  MoveActor(this, angle, MANUAL_TRIP_DISTANCE, MOVE_TRIP);
	}

	if (reset_speed)
	{
		vertical_tilt_float += reset_speed;
		if ( reset_speed < 0  &&  vertical_tilt_float <= (vertical_tilt_origin) ||
				reset_speed > 0  &&	vertical_tilt_float >= (vertical_tilt_origin))
		{
			reset_speed = 0.0f;
			vertical_tilt_float = (float)(vertical_tilt_origin);
		}
	}
	else if (move && (keyboard[Keystates::LOOK_DOWN] || mouse_look.down)) // look down
	{
		if (mouse_look.down && mouse_look.looking)
			vertical_tilt_float-=timing->nticks*(float)UPDOWNVEL*mouse_look.yratio;
		else
			vertical_tilt_float-=timing->nticks*(float)UPDOWNVEL;

		if ( vertical_tilt_float < -physht )
		{
			vertical_tilt_float = (-physht);
		}
	}
	else if (move && (keyboard[Keystates::LOOK_UP] || mouse_look.up)) // || (reset_speed  == -1)) // look up
	{
		if (mouse_look.up && mouse_look.looking)
			vertical_tilt_float+=timing->nticks*(float)UPDOWNVEL*mouse_look.yratio;
		else
			vertical_tilt_float+=timing->nticks*(float)UPDOWNVEL;

		if (vertical_tilt_float > max_vertical_tilt + physht)
		{
			vertical_tilt_float = max_vertical_tilt + physht; // ????
			reset_speed = 0.0f;
		}
	}
	vertical_tilt = (long)vertical_tilt_float;

	if ((z > xheight - (.1*physht)) && (z < xheight + (.1*physht)) &&
		keyboard[Keystates::JUMP] && move)
	{
		if (options.network)
			gs->SetJump();
		keyboard[Keystates::JUMP] = 0;
		vertforce=-40.0f;
		jumped = true;
		if (flags & ACTOR_MEDITATING) // expire meditation on jump
			this->RemoveTimedEffect(LyraEffect::PLAYER_MEDITATING);
		player->PerformedAction();
	}

	return true;
}

void cPlayer::PerformedAction(void)
{
	if (flags & ACTOR_MEDITATING) // expire meditation on move
		this->RemoveTimedEffect(LyraEffect::PLAYER_MEDITATING);
	if (arts->Casting() || arts->Waiting())
		arts->CancelArt();
	waving = false;
#ifndef PMARE // pmares regen like players, continually
#ifndef AGENT
	// Darkmares in sanctuary need to *not* reset their next_heal counter so they'll continue to get thrashed
	// even when they move
	if (!((this->GetAccountType() == LmAvatar::ACCT_DARKMARE) && (level->Rooms[this->Room()].flags & ROOM_SANCTUARY)))
		next_heal = LyraTime() + HEAL_INTERVAL;
#endif
#endif
	showing_map = false;

	return;
}


// set up, or extend, a time-based effect; returns true if
// applied, or false if rejected for whatever reason
bool cPlayer::SetTimedEffect(int effect, DWORD duration, lyra_id_t caster_id)
{
	if (duration <= 0)
		return false;

#ifdef GAMEMASTER
#ifdef AGENT
	if (((this->AvatarType() >= Avatars::AGOKNIGHT) || (this->AvatarType() < Avatars::MIN_NIGHTMARE_TYPE)) &&
		timed_effects->harmful[effect])
		return false;
#else
	if (options.invulnerable && timed_effects->harmful[effect])
		return false;
#endif
#endif

	// only effect on soulsphere is soulevoke
	if ((effect != LyraEffect::PLAYER_SOULEVOKE) && (flags & ACTOR_SOULSPHERE))
	{
		LoadString (hInstance, IDS_NO_SSPHERE, disp_message, sizeof(disp_message));
		if (caster_id == this->ID())
			display->DisplayMessage(disp_message);
		return false;
	}

	switch (effect) {
	case LyraEffect::PLAYER_CURSED:{
		// check to see if protection is in effect
		if (flags & ACTOR_PROT_CURSE)
		{
		LoadString (hInstance, IDS_PLAYER_CURSE_DEFLECT, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		//  Curse and Protection offset and partially cancel
		timed_effects->expires[LyraEffect::PLAYER_PROT_CURSE]-=duration;
		return false;
		}
		// Implementing Curse Effect
		// I also added some debugging checks for this
		// Fixing Curse effect to Balthiir's specs
		// Make sure that this strength computation is in the release
		// I recalculated the strength of curse and made it a buildable effect - Ajax
		int new_strength = (duration/20000)+1;		
		new_strength = new_strength + curse_strength;

		if (new_strength>50) new_strength = 50;
		// Note that these messages are now debug ONLY
#ifdef UL_DEBUG
		LoadString (hInstance, IDS_CURSE_CHANGE, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, new_strength, curse_strength);
		display->DisplayMessage(message, false);
#endif
		if (new_strength>curse_strength) {
			curse_strength = new_strength;
#ifdef UL_DEBUG
			LoadString (hInstance, IDS_CURSE_STRONGER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, curse_strength);
			display->DisplayMessage(message, false);
#endif
		} break;
								   }
	case LyraEffect::PLAYER_PARALYZED:{
#ifndef PMARE // pmares get permanent free action
		if (flags & ACTOR_FREE_ACTION)
#endif
		{
		LoadString (hInstance, IDS_PLAYER_PARALYZE_DEFLECT, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		// Paralyze and Free Action now offset and partially cancel
		timed_effects->expires[LyraEffect::PLAYER_PROT_PARALYSIS]-=duration*3; // new code
		return false;
									  }
		// If actually paralyzed, cancel any current evoke.
									  arts->CancelArt();} break;
	case LyraEffect::PLAYER_DRUNK:{
#ifndef PMARE // pmares get permanent free action
		if (flags & ACTOR_FREE_ACTION)
#endif
		{
		LoadString (hInstance, IDS_PLAYER_STAGGER_DEFLECT, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		// Stagger and Free Action now offset and partially cancel
		timed_effects->expires[LyraEffect::PLAYER_PROT_PARALYSIS]-=duration*3; // new code
		return false;
								  }} break;
	case LyraEffect::PLAYER_FEAR:{
		if (flags & ACTOR_PROT_FEAR)
		{
		LoadString (hInstance, IDS_PLAYER_FEAR_DEFLECT, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		// Fear and Resist Fear now offset and partially cancel
		timed_effects->expires[LyraEffect::PLAYER_PROT_FEAR]-=duration*3;
		return false;
								 }} break;
	case LyraEffect::PLAYER_BLIND:{
#ifndef PMARE // pmares get permanent vision
		if (flags & ACTOR_DETECT_INVIS)
#endif
		{
		LoadString (hInstance, IDS_PLAYER_BLIND_DEFLECT, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		// Blind and Vision now offset and partially cancel
		timed_effects->expires[LyraEffect::PLAYER_DETECT_INVISIBLE]-=duration*3;
		return false;
								  }} break;
//  Players must know how to Recall, Transform, etc. in case talisman causes effect
// Recommend moving this back to cArts and calling it from here
	case LyraEffect::PLAYER_RECALL: {
		this->SetRecall(this->x, this->y, this->angle, level->ID());
		gs->SendPlayerMessage(0, RMsg_PlayerMsg::RECALL, 0, 0);
		LoadString (hInstance, IDS_RECALL, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message, false);
									} break;
	case LyraEffect::PLAYER_TRANSFORMED: {
		LmAvatar new_avatar;
		//new_avatar.Init((player->Skill(Arts::NIGHTMARE_FORM)/20 + 1), 0, 0, 0, 0, 0, Guild::NO_GUILD, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		new_avatar.Init(Avatars::EMPHANT, 0, 0, 0, 0, 0, Guild::NO_GUILD, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		this->SetTransformedAvatar(new_avatar);
										 } break;
	case LyraEffect::PLAYER_RETURN: {
		if (this->flags & ACTOR_RETURN) { // 2nd activation - return
			if (this->Teleport(this->ReturnX(), this->ReturnY(), this->ReturnAngle(), this->ReturnLevel())) {
				this->RemoveTimedEffect(LyraEffect::PLAYER_RETURN);
				return(false);
			} else { // teleport failed - replace dreamsoul
				this->SetCurrStat(arts->Stat(Arts::RETURN), arts->Drain(Arts::RETURN), SET_RELATIVE, playerID);
			}

			return(true);
		}
		else { // 1st activation - mark location
			this->SetReturn(this->x, this->y, this->angle, level->ID());
			gs->SendPlayerMessage(0, RMsg_PlayerMsg::RETURN, 0, 0);
		}
	} break;
	case LyraEffect::PLAYER_REFLECT: {
		if (this->flags & ACTOR_REFLECT) { // 2nd activation - reflect
			this->RemoveTimedEffect(LyraEffect::PLAYER_REFLECT);
			return(true);
		}
		else {
			// 5-50% chance (starts at 5%, increases 5% per plateau)
			int new_strength = duration / 6000;
			if (new_strength>50) new_strength = 50;

			reflect_strength = new_strength;

#ifdef GAMEMASTER
			// give GMs a 10 point boost to reflect strength
			reflect_strength = new_strength + 10;
#endif

#ifdef UL_DEBUG
			_stprintf(message, "Used reflect of strength %d from duration of %d (debug MSG Only)", reflect_strength, duration);
			display->DisplayMessage(message, false);
#endif
		}
	} break;
	case LyraEffect::PLAYER_SOULEVOKE: {
		if (!(player->flags & ACTOR_SOULSPHERE))	
		{
			LoadString (hInstance, IDS_SOULEVOKE_SOULSPHERE_ONLY, disp_message, sizeof(disp_message));
		_stprintf(message,disp_message, "Soulevoke");
			display->DisplayMessage (message, false);
			return(false);
		}
									   } break;
	case LyraEffect::PLAYER_MIND_BLANKED: {
		if (this->flags & ACTOR_MIND_BLANKED)
		{
			this->RemoveTimedEffect(LyraEffect::PLAYER_MIND_BLANKED);
			return(true);
		}
		else
		{
			gs->SendPlayerMessage(0, RMsg_PlayerMsg::MIND_BLANK, 1, 0);
		}
										} break;

	case LyraEffect::PLAYER_PEACE_AURA: {
		if (this->flags & ACTOR_PEACE_AURA) { // 2nd evoke - Peace Aura
			this->RemoveTimedEffect(LyraEffect::PLAYER_PEACE_AURA);
			return(true);
		}
									 } break;

	case LyraEffect::PLAYER_BLEED: {
		if (this->flags & ACTOR_PROT_CURSE) {
			LoadString (hInstance, IDS_PLAYER_BLEED_DEFLECT, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
			timed_effects->expires[LyraEffect::PLAYER_PROT_CURSE]-=duration*3;
			return false;
		}
		if (caster_id != player->ID())

			last_bleeder = caster_id;
								   } break;

	case LyraEffect::PLAYER_POISONED: {
		if ((flags & ACTOR_NO_POISON) || (player->IsPMare()) || (player->GetAccountType() == LmAvatar::ACCT_DARKMARE))
		{
			LoadString (hInstance, IDS_PLAYER_POISON_DEFLECT, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
			return false;
		}

		if (caster_id != player->ID())

			last_poisoner = caster_id;

		int new_strength = (duration/60000) + 1;	

		if (new_strength>10) new_strength = 10;

		if (new_strength>poison_strength)

		poison_strength = new_strength;

		} break;
  case LyraEffect::PLAYER_SPIN:
    {
    break;
	}

	default:
		break; 
		}


	if (flags & timed_effects->actor_flag[effect])
	{	// extend duration of existing effect
		//if (timed_effects->harmful[effect]) // 1/4 duration extension
		//	timed_effects->expires[effect] += (int)(duration/4);
		//else								// 1/2 duration extension
		timed_effects->expires[effect] += (int)(duration/2);
		if (timed_effects->more_descrip[effect])
			display->DisplayMessage(timed_effects->more_descrip[effect] );
		else
		{
			LoadString (hInstance, IDS_DURATION_EXTENDED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, timed_effects->name[effect]);
			display->DisplayMessage(message);
		}
	}
	else // new effect
	{
		timed_effects->expires[effect] = LyraTime() + duration;
		if (timed_effects->start_descrip[effect])
			display->DisplayMessage(timed_effects->start_descrip[effect]);

		flags = flags | timed_effects->actor_flag[effect];
		// Nectar's tweak
		if (effect == LyraEffect::PLAYER_FEAR)
			cDS->PlaySound(LyraSound::SCARE_LOOP,x,y,false);
		else if (effect == LyraEffect::PLAYER_BLIND)
			cDS->PlaySound(LyraSound::BLIND_LOOP,x,y,false);
		else if (effect == LyraEffect::PLAYER_DEAF)
			cDS->PlaySound(LyraSound::DEAFEN_LOOP,x,y,false);

	}
	return true;
}

int cPlayer::TeacherID(void)
{
	if (this->CurrentAvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
		return 0;
	else
		return avatar.Teacher();
}

// returns account type
unsigned int cPlayer::GetAccountType (void)
{
	return avatar.AccountType();
}


void cPlayer::RemoveTimedEffect(int effect)
{

/*
#ifdef AGENT
	if ((effect == LyraEffect::PLAYER_DETECT_INVISIBLE) && (avatar.AvatarType() >= Avatars::SHAMBLIX))
		return; // Shamblix and Horron agents never lose Vision
#endif
*/
	if (!(flags & timed_effects->actor_flag[effect]))
		return; // effect not active

	flags = flags & ~(timed_effects->actor_flag[effect]);
	timed_effects->expires[effect] = 0;
	// in case we are invis AND chamele'd, no re-visible message
	if (((effect == LyraEffect::PLAYER_CHAMELED) && (flags & ACTOR_INVISIBLE)) ||
		((effect == LyraEffect::PLAYER_INVISIBLE) && (flags & ACTOR_CHAMELED)))
		return;

	if (timed_effects->expire_descrip[effect])
		display->DisplayMessage(timed_effects->expire_descrip[effect]);
	// other effects of expiration go here
	if (effect == LyraEffect::PLAYER_TRAIL)
	{	// delete trail markers at expiration of trail
		for (cActor *a=actors->IterateActors(INIT); a != NO_ACTOR; a=actors->IterateActors(NEXT))
			if ((a->Type() == ORNAMENT) && (a->CurrBitmap(0) == LyraBitmap::TRAIL_MARKER))
				a->SetTerminate();
		actors->IterateActors(DONE);
	}
	// I'm not sure why this code is malfunctioning in 215. It works for me.
	if (effect == LyraEffect::PLAYER_TRANSFORMED)
	{	// go back to true form
		this->SetBitmapInfo(avatar.BitmapID());
		if (options.network)
			gs->AvatarChange(avatar, false);
		if (avatar.AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
			avatar_poses = mare_poses;
		else
			avatar_poses = dreamer_poses;
		if (!forming && !dissolving)
			currframe = avatar_poses[STANDING].start_frame;
		cp->AddAvatar();
	}
	if (effect == LyraEffect::PLAYER_FEAR)
		cDS->StopSound(LyraSound::SCARE_LOOP);
	else if (effect == LyraEffect::PLAYER_BLIND)
		cDS->StopSound(LyraSound::BLIND_LOOP);
	else if (effect == LyraEffect::PLAYER_DEAF)
		cDS->StopSound(LyraSound::DEAFEN_LOOP);
	else if (effect == LyraEffect::PLAYER_MIND_BLANKED)
	{
		if (options.network && gs)
		{
			gs->SendPlayerMessage(0, RMsg_PlayerMsg::MIND_BLANK, 0, 0);
		}
	}
	else if (effect == LyraEffect::PLAYER_CURSED)
		curse_strength = 0;

	else if (effect == LyraEffect::PLAYER_POISONED) {
		last_poisoner = Lyra::ID_UNKNOWN;
		poison_strength = 0;
	}
	else if (effect == LyraEffect::PLAYER_BLEED) {
		last_bleeder = Lyra::ID_UNKNOWN;
	}
	else if (effect == LyraEffect::PLAYER_REFLECT)
		reflect_strength = 0;

	return;
};


// check on time based timed_effects
void cPlayer::CheckStatus(void)
{
	DWORD curr_time = LyraTime();
	int i,stat,value,view,dist;

	//_tprintf("now: %d expire: %d\n",LyraTime(), timed_effects->expires[LyraEffect::PLAYER_FEAR]);

	evokingFX.Update();
	evokedFX.Update();

	// check to see if we've healed naturally
	if ((LyraTime() > next_heal) && !(flags & ACTOR_SOULSPHERE))
	{	  // raise selected if under max; else raise lowest
#ifdef AGENT  // true nightmares regen fast
		value = this->AvatarType();
#else 
#ifdef PMARE // regen fast if boggo or ago
		if (this->AvatarType() <= Avatars::AGOKNIGHT)
			value = this->AvatarType();
		else
			value = 1;
#else  // dreamers regen slowly
		if (flags & ACTOR_MEDITATING)
			 value = 2 + (player->Skill(Arts::MEDITATION) / 10);
		else
			 value = 1;
#endif
#endif
//		if ((avatar.AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) &&
		// Note that there is corresponding code in cPlayer::PerformedAction() to make the next_heal counter *not* get
		// reset by movement when dark mares are in sanctuary (pmares and agents never reset the next_heal
		// counter).
		if (((this->GetAccountType() == LmAvatar::ACCT_PMARE) || (this->GetAccountType() == LmAvatar::ACCT_DARKMARE) ||
			(this->GetAccountType() == LmAvatar::ACCT_NIGHTMARE)) &&
			(level->Rooms[this->Room()].flags & ROOM_SANCTUARY))
		{ // all mares get thrashed in sanct
			value = (avatar.AvatarType() -Avatars::MIN_NIGHTMARE_TYPE + 1)*-5;
			this->SetCurrStat(Stats::DREAMSOUL, value, SET_RELATIVE, playerID);
			LoadString (hInstance, IDS_SANCTUARY_HURTS, message, sizeof(message));
			display->DisplayMessage(message);

		}
		
// 	else if (stats[selected_stat].current < stats[selected_stat].max)
		else if (this->CurrStat(selected_stat) < this->MaxStat(selected_stat))
			this->SetCurrStat(selected_stat, value, SET_RELATIVE,  playerID);
		else
		{
			stat = selected_stat;
			for (i=0; i<NUM_PLAYER_STATS; i++)
				if ((stats[i].max - stats[i].current) >
					(stats[stat].max - stats[stat].current))
					stat = i;
			if (stat != selected_stat)
				this->SetCurrStat(stat, value, SET_RELATIVE, playerID);
		}
		next_heal = LyraTime() + HEAL_INTERVAL;
		}

	if ((flags & ACTOR_POISONED) && (LyraTime() > next_poison))
	{	 // sap dreamsoul...
		this->SetCurrStat(Stats::DREAMSOUL, -((rand()%poison_strength)+1), SET_RELATIVE,last_poisoner);
		next_poison = LyraTime() + POISON_INTERVAL;
	}

	if ((flags & ACTOR_BLEED) && (LyraTime() > next_bleed))
	{	 // sap dreamsoul...
		this->SetCurrStat(Stats::DREAMSOUL, -1, SET_RELATIVE,last_bleeder);
		next_bleed = LyraTime() + BLEED_INTERVAL;
	}

	if ((flags & ACTOR_REGENERATING) && (LyraTime() > next_regeneration) )
	{	 // sap one of all stats
			
		this->SetCurrStat(Stats::DREAMSOUL, +1, SET_RELATIVE, playerID);
		next_regeneration = LyraTime() +  HEAL_INTERVAL ;
	}

	if (LyraTime() > next_sector_tag)
	{
		switch (level->Sectors[this->sector]->tag) 
		{
		case SECTOR_WILLPOWER:
			this->SetCurrStat(Stats::WILLPOWER, +1, SET_RELATIVE, playerID);
			break;
		case SECTOR_INSIGHT:
			this->SetCurrStat(Stats::INSIGHT, +1, SET_RELATIVE, playerID);
			break;
		case SECTOR_RESILIENCE:
			this->SetCurrStat(Stats::RESILIENCE, +1, SET_RELATIVE, playerID);
			break;
		case SECTOR_LUCIDITY:
			this->SetCurrStat(Stats::LUCIDITY, +1, SET_RELATIVE, playerID);
			break;
		case SECTOR_DREAMSOUL:
			if (!(flags & ACTOR_SOULSPHERE))
				this->SetCurrStat(Stats::DREAMSOUL, +1, SET_RELATIVE, playerID);
			break;
		case SECTOR_DAMAGE:
			this->SetCurrStat(Stats::DREAMSOUL, -1, SET_RELATIVE, playerID);
			break;
		case SECTOR_CURSE:
		case SECTOR_NO_REGEN:
		default:
			break;
		}
		next_sector_tag = LyraTime() +  SECTOR_TAG_INTERVAL;
	}

	if ((flags & ACTOR_TRAILING) && (LyraTime() > next_trail))
	{ // drop trail marker if no others are close to us
		 bool mark = true;
		for (cActor *a=actors->IterateActors(INIT); a != NO_ACTOR; a=actors->IterateActors(NEXT))
		{
			if ((a->Type() == ORNAMENT) && (a->CurrBitmap(0) == LyraBitmap::TRAIL_MARKER) &&
				(fdist(x,y,a->x,a->y) < MIN_TRAIL_SEPARATION))
			{
				mark = false;
				break;
			}
		}
		actors->IterateActors(DONE);
		if (mark)
		{
				cOrnament* trail_marker = new cOrnament(x, y, 0, angle, ACTOR_NOCOLLIDE, LyraBitmap::TRAIL_MARKER);
			trail_marker->flags = trail_marker->flags | ACTOR_NOCOLLIDE;
		}
		next_trail = LyraTime() + TRAIL_INTERVAL;
	}

	for (i=0; i<NUM_TIMED_EFFECTS; i++)
	{	// do not removed timed effects in mission boards to avoid perma mind blank!
		if (options.network && gs && !gs->LoggedIntoLevel())
			break;
		if ((flags & timed_effects->actor_flag[i]) && (curr_time > timed_effects->expires[i]))
			this->RemoveTimedEffect(i);
	}

	if ((flags & ACTOR_SOULSPHERE) && 
		(level->Rooms[room].flags & ROOM_SANCTUARY) &&
		(player->Avatar().AvatarType() < Avatars::MIN_NIGHTMARE_TYPE))
		this->ReformAvatar();

#if defined PMARE

	if ((flags & ACTOR_SOULSPHERE))
	{	
		Sleep(1000);
		this->Teleport(329,1366,-90,44);
	}

	// PMare Threshold is a sanctuary to pmares
	if ((flags & ACTOR_SOULSPHERE) && (level->ID() == START_LEVEL_ID))
		this->ReformAvatar();
#endif
		
	if ((flags & ACTOR_SOULSPHERE) && 
		(this->CurrStat(Stats::DREAMSOUL) > 0))
		this->ReformAvatar();	
	
#ifdef PMARE
	unsigned int cur_time = LyraTime();
	unsigned int sec_diff = (cur_time - collapse_time)/1000;
	if ((flags & ACTOR_SOULSPHERE) && (sec_diff > 90))
		this->SetCurrStat(Stats::DREAMSOUL, this->MaxStat(Stats::DREAMSOUL), 
						  SET_ABSOLUTE, playerID);
#endif

	if (!(player->Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE))
	{
		if ((LyraTime() > next_nightmare_check) && !(flags & ACTOR_SOULSPHERE) &&
			!(level->Rooms[this->Room()].flags & ROOM_SANCTUARY))
		{
			next_nightmare_check = LyraTime() + NIGHTMARE_CHECK_INTERVAL;

			// check for effects of being too close to monsters
				for (cNeighbor *n=actors->IterateNeighbors(INIT); n != NO_ACTOR; n=actors->IterateNeighbors(NEXT))
					if (n->IsMonster())
					{
						view = FixAngle((angle - n->angle)+32)/(Angle_360/Avatars::VIEWS);
						if (n->Visible() && (view == 3)) // face-on
						{ // gaze effects here
							if ((n->Avatar().AvatarType() == Avatars::SHAMBLIX) &&
								!(flags & ACTOR_CURSED) && !(flags & ACTOR_PROT_CURSE))
								arts->ApplyCurse(1, n->ID());
							if ((n->Avatar().AvatarType() == Avatars::HORRON) &&
								!(flags & ACTOR_PARALYZED) && !(flags & ACTOR_FREE_ACTION))
							{
								arts->ApplyParalyze(Arts::PARALYZE, 2, n->ID());
	// 						arts->ApplyAbjure(1, n->ID());
							}
						}
						dist = (int)((n->x - x)*(n->x - x) + (n->y - y)*(n->y - y));
						if (n->Avatar().AvatarType() == Avatars::HORRON)
						{
							if ((dist < HORRON_FEAR_DISTANCE) &&
								!(flags & ACTOR_SCARED) && !(flags & ACTOR_PROT_FEAR))
								arts->ApplyScare(1, n->ID());
							if (dist < HORRON_DRAIN_DISTANCE)
								this->SetCurrStat(Stats::DREAMSOUL, -1, SET_RELATIVE, n->ID());
						}
					}
			actors->IterateNeighbors(DONE);
		}
	}
	return;
}

int cPlayer::IconBitmap(void)
{
	if (forming)
		return (LyraBitmap::FORMREFORM_EFFECT + currframe);
	else if (dissolving)
		return (LyraBitmap::FORMREFORM_EFFECT + effects->EffectFrames(LyraBitmap::FORMREFORM_EFFECT) - currframe);
	else if (flags & ACTOR_SOULSPHERE)
		return (LyraBitmap::SOULSPHERE_EFFECT);
	else
		return (this->cActor::IconBitmap());
}


// Set current room # to newroom
void cPlayer::SetRoom(float old_x, float old_y)
{
	realmid_t last_room = room;
	bool new_sanct = false;

	if (level && (level->ID() != NO_LEVEL) && ready)
	{
		if ((room < 0) || (room > level->NumRooms()))
			room = last_room = 0;

		room = level->Sectors[sector]->room;

		if (room == 0)
		{
			room = last_room;
			level->Sectors[sector]->room = room;
			LoadString (hInstance, IDS_NOROOM0, temp_message, sizeof(temp_message));
			_stprintf(errbuf, temp_message, sector,level->ID());
			NONFATAL_ERROR(errbuf);
		}

		if (gs && gs->LoggedIntoLevel())
		{
			// do sanctuary message stuff
			if (room && (level->Rooms[room].flags & ROOM_SANCTUARY))
				new_sanct = true;

			if (((room || last_room)) && new_sanct && !old_sanct)
			{
				LoadString (hInstance, IDS_ENTER_SANCTUARY, message, sizeof(message));
				display->DisplayMessage(message);
			}
			else if (((room || last_room)) && !new_sanct && old_sanct)
			{
				LoadString (hInstance, IDS_EXIT_SANCTUARY, message, sizeof(message));
				display->DisplayMessage(message);
			}

			old_sanct = new_sanct;
		}

		// show room names in training mode
		if ((room != last_room) && (options.welcome_ai) && ready)
		{
			LoadString (hInstance, IDS_ENTER_LEVEL, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, level->RoomName(room));
			display->DisplayMessage(message, false);
		}

#ifndef AGENT
		// log into the game at end of training
		if ((options.welcome_ai) && (room == THRESHOLD_QUAD))
		{ // log into gameserver at completion of training
			cItem *item;
			options.welcome_ai = false;

			// don't let the sneaky bastards keep their training talismans
			for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			{
				item->SetStatus(ITEM_DESTROYING);
				item->SetTerminate();
			}
			actors->IterateItems(DONE);

			if (options.network && gs && !(gs->LoggedIntoGame()))
			{
				gs->WelcomeAIComplete();
			}
		}
#endif
	}

	if (options.network && (room != last_room) && gs && gs->LoggedIntoLevel())
	{
		this->MarkLastLocation();
		gs->OnRoomChange((short)old_x, short(old_y));
	}


	return;
};

bool cPlayer::ActiveShieldValid(void)
{
	if (!active_shield || !actors->ValidItem(active_shield))
		return false;

	for (int i=0; i<active_shield->NumFunctions(); i++)
		if (active_shield->ItemFunction(i) == LyraItem::ARMOR_FUNCTION)
			return true;

	return false;
}

// returns true if there is now an active shield
bool cPlayer::SetActiveShield(cItem *value)
{
	LmAvatar new_avatar = avatar;

	if (active_shield && (!value || (active_shield == value)))
	{
		LoadString (hInstance, IDS_NO_SHIELD, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		value = NO_ITEM;
	}

	if (active_shield && actors->ValidItem(active_shield))
		active_shield->SetInventoryFlags(active_shield->InventoryFlags() & ~ITEM_ACTIVE_SHIELD);

	item_flags_sorting_changed = true;
	active_shield = value;
	if (active_shield)
		return true;
	else
		return false;
};



// Changes the current value of a player's stat.
int cPlayer::SetCurrStat(int stat, int value, int how, lyra_id_t origin_id)
{
	int amount = value;

	if (value != Stats::STAT_MIN)
		this->ValidateChecksums();

#ifdef GAMEMASTER // check for invulnerability
	if ((origin_id != this->ID()) && (how == SET_RELATIVE) &&
		(value < 0) && options.invulnerable)
		return stats[stat].current;
#endif

	if ((player->flags & ACTOR_PEACE_AURA) && 
		(stat == Stats::DREAMSOUL) &&
		(origin_id != this->ID()) &&
		(how == SET_RELATIVE) &&
		(value < 0))
	{
		LoadString (hInstance, IDS_PEACE_AURA, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		return stats[stat].current;
	}


	// check for armor on dreamsoul drains
	if ((stat == Stats::DREAMSOUL) && (how == SET_RELATIVE) && (value <0) &&
	  (origin_id != playerID))
	{

		if (this->ActiveShieldValid())
			amount = active_shield->AbsorbDamage(amount);
#ifdef GAMEMASTER // dark mares get an additional 25% shield
		if (this->GetAccountType() == LmAvatar::ACCT_DARKMARE)
			amount = (int)(amount*.75);
#endif
#ifdef PMARE // pmare bogroms get an additional 30% shield
		if (this->GetMonsterType() == Avatars::BOGROM)
			amount = amount*.70;
#endif
		if (amount)
			this->SetHit(true);
		int current = stats[stat].current;
		int max		= stats[stat].max;

		int current_stat_ratio = current*4/max;
		int new_stat_ratio	  = (current+amount)*4/max;
		if ((new_stat_ratio < 3) && (new_stat_ratio >= 0) && 
			(new_stat_ratio != current_stat_ratio))
		{
			const UINT KillProgressIDs[] = {IDS_KILL_PROGRESS_3,IDS_KILL_PROGRESS_2,IDS_KILL_PROGRESS_1};
			LoadString (hInstance, KillProgressIDs[new_stat_ratio] , disp_message, sizeof(disp_message));

#ifdef GAMEMASTER
			if(agentbox && agentbox->PosessionInProgress())
			{	// use monster type..not login name if possesd agent
			_stprintf(temp_message, _T(">%s %s "), NightmareName(avatar.AvatarType()), disp_message);
				gs->Talk(temp_message, RMsg_Speech::RAW_EMOTE, Lyra::ID_UNKNOWN, false, false);
			}
			else
#endif
				gs->Talk(disp_message, RMsg_Speech::EMOTE, Lyra::ID_UNKNOWN, false, false);
		}
	}

	// Modifiers go here
	if (how == SET_ABSOLUTE)
		stats[stat].current = amount;
	else
		stats[stat].current += amount;

	// ensure we're not over max or under min
	if (stats[stat].current > stats[stat].max)
		stats[stat].current = stats[stat].max;
	if (stats[stat].current <= Stats::STAT_MIN)
		stats[stat].current = Stats::STAT_MIN;

	if (origin_id != SERVER_UPDATE_ID)
		stats[stat].current_needs_update = true;

	//_tprintf("%d set to %d; new = %d max = %d\n",stat,value,stats[stat].current,stats[stat].max);

	stats[stat].checksum = ((stats[stat].current + stats[stat].max)) ^ MAGIC_XOR_VAR;

	cp->UpdateStats();

	if ((stat == Stats::DREAMSOUL) && (stats[stat].current == Stats::STAT_MIN))
		this->Dissolve(origin_id);

	return stats[stat].current;
}

// Changes the max value of a player's stat; can ONLY be set absolute
int cPlayer::SetMaxStat(int stat, int value, lyra_id_t origin_id)
{
	int old_value = stats[stat].max;

	if (value != Stats::STAT_MIN)
		this->ValidateChecksums();

	stats[stat].max = value;

	// ensure we're not over max or under min
	if (stats[stat].max <= Stats::STAT_MIN)
		stats[stat].max = Stats::STAT_MIN;
#ifndef AGENT // stat max doesn't apply to agents
	else if (stats[stat].max > Stats::STAT_MAX)
		stats[stat].max = Stats::STAT_MAX;
#endif

	stats[stat].checksum = ((stats[stat].current + stats[stat].max)) ^ MAGIC_XOR_VAR;

	// adjust current stat for change in max when max goes up, but NOT from 0, as this causes soulspheres to reform
	if ((stats[stat].max > old_value) && stats[stat].current)
		this->SetCurrStat(stat, stats[stat].max - old_value, SET_RELATIVE, playerID);

	stats[stat].checksum = ((stats[stat].current + stats[stat].max)) ^ MAGIC_XOR_VAR;

	// ensure current stat isn't over the max
	if (stats[stat].current > stats[stat].max)
		this->SetCurrStat(stat, stats[stat].max, SET_ABSOLUTE, playerID);

	stats[stat].checksum = ((stats[stat].current + stats[stat].max)) ^ MAGIC_XOR_VAR;

	cp->UpdateStats();

	return stats[stat].max;
}

void cPlayer::SetHeadID(int value, bool update)
{
	LmAvatar new_avatar = avatar;
	unsigned int old_head_id = avatar.Head();

	new_avatar.SetHead(value);
	if (new_avatar.Head() != old_head_id)
		this->SetAvatar(new_avatar, update);
}


void cPlayer::InitAvatar(void)
{
	int random, sex;

	random = rand()%2;
#ifdef PMARE
	sex = Avatars::MIN_NIGHTMARE_TYPE + 1;
#else
	if (random)
		sex = Avatars::FEMALE;
	else
		sex = Avatars::MALE;
#endif
	avatar.Init(sex, 0, 0, 0, 0, 0, Guild::NO_GUILD, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

bool cPlayer::IsMare(void)
{
	if (avatar.AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
		return true;
	else
		return false;
}

void cPlayer::SetAvatar(LmAvatar new_avatar, bool update_server)
{
	if (this->playerID != INVALID_PLAYERID)
	{

		// sanity check the new avatar first

		if ((new_avatar.AvatarType() > Avatars::MAX_AVATAR_TYPE) ||
			(new_avatar.AvatarType() < Avatars::MIN_AVATAR_TYPE))
			return;

		if (new_avatar.ShowGuild() &&
		   (this->GuildRank(new_avatar.GuildID()) != new_avatar.GuildRank()))
		{
			// clear it
		  new_avatar.SetShowGuild(0);
		  new_avatar.SetGuildRank(Guild::NO_RANK);
		  new_avatar.SetGuildID(Guild::NO_GUILD);
		}

		// check sphere
		if (new_avatar.ShowSphere() && (this->Sphere() != new_avatar.Sphere()))
		{
			 new_avatar.SetShowSphere(0);
			 new_avatar.SetSphere(this->Sphere());
		}

		// Reset halo if the player if a) had halo off, b) no art of train or sphere
		if (new_avatar.Teacher() && !this->IsTeacher()) // 		this->Skill(Arts::TRAIN))
			new_avatar.SetTeacher(0);

		// Reset halo if the player if a) had halo off, b) no art of quest
		if (new_avatar.Apprentice() && !this->IsApprentice())
			new_avatar.SetApprentice(0);

		// Reset double halo if not a master teacher
		if (new_avatar.MasterTeacher() && (0 == this->Skill(Arts::TRAIN_SELF)))
			new_avatar.SetMasterTeacher(0);

		// Reset gold sphere if not a dreamsmith
		if (new_avatar.DreamSmith() && (0 == this->Skill(Arts::DREAMSMITH_MARK)))
			new_avatar.SetDreamSmith(0);

		// Reset blue sphere if not a wordsmith
		if (new_avatar.WordSmith() && (0 == this->Skill(Arts::WORDSMITH_MARK)))
			new_avatar.SetWordSmith(0);

		// Reset red sphere if no dreamstrike
		if (new_avatar.Dreamstrike() && (0 == this->Skill(Arts::DREAMSTRIKE)))
			new_avatar.SetDreamstrike(0);

	}

#ifdef GAMEMASTER
	if (new_avatar.Hidden())
		flags = flags | ACTOR_NOCOLLIDE;
	else
		flags = flags & ~ACTOR_NOCOLLIDE;
#endif

	avatar = new_avatar;
	
	int focus = avatar.Focus();
	int mt = avatar.MasterTeacher();
	int teach = avatar.Teacher();
	int apprent = avatar.Apprentice();
	int g = avatar.ShowGuild();
	int gid = avatar.GuildID();
	int gr = avatar.GuildRank();
	int dsmith = avatar.DreamSmith();
	int dstrike = avatar.Dreamstrike();
	int ws = avatar.WordSmith();
	int sphere = avatar.Sphere();

	this->SetBitmapInfo(avatar.BitmapID());

	if (avatar.AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
	{
		avatar_poses = mare_poses;
	}
	else
	{
		avatar_poses = dreamer_poses;
	}

	if (this->playerID != INVALID_PLAYERID)
	{
		avatar.SetShowSphere(avatar.ShowSphere());
		avatar.SetSphere(this->Sphere());
	}

	if (!forming && !dissolving)
		currframe = avatar_poses[STANDING].start_frame;

	if (gs && update_server)
		gs->AvatarChange(avatar, true);

	if (cp)
		cp->AddAvatar();

	return;
}

void cPlayer::SetTransformedAvatar(LmAvatar new_avatar)
{
	 transformed_avatar = new_avatar;

	if (avatar.AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
		avatar_poses = mare_poses;
	else
		avatar_poses = dreamer_poses;

	if (!forming && !dissolving)
		currframe = avatar_poses[STANDING].start_frame;

	 this->SetBitmapInfo(transformed_avatar.BitmapID());
	 if (gs)
		gs->AvatarChange(transformed_avatar, false);
	 if (cp)
		cp->AddAvatar();

	 return;

}

// attack as a nightmare; used by both agents and transformed players
bool cPlayer::NightmareAttack(lyra_id_t target)
{
	if ((flags & ACTOR_PARALYZED) || (flags & ACTOR_SOULSPHERE))
		return FALSE;

	if (player->flags & ACTOR_CHAMELED)
		player->RemoveTimedEffect(LyraEffect::PLAYER_CHAMELED);
#ifndef AGENT
	this->PerformedAction();
#endif

#ifndef AGENT
	if (player->flags & ACTOR_PEACE_AURA)
	{
		LoadString (hInstance, IDS_PEACE_AURA_MARE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return FALSE;
	}
#endif

int mare_avatar = this->CurrentAvatarType();

#ifdef AGENT
	if (this->AvatarType() < Avatars::MIN_NIGHTMARE_TYPE)
	{ // Revenant borrow attack strength from the nightmare agent they replace based on agent username e.g. Shamblix_14=Shamblix
		int pi;
		TCHAR marename[Lyra::PLAYERNAME_MAX];
		// *** STRING LITERAL ***  
		if (_stscanf(agent_info[AgentIndex()].name, "%[^_]_%d", marename, &pi) != 2) {
		  // couldn't parse it
		 _tcsnccpy(marename, agent_info[AgentIndex()].name, sizeof(marename));
		}
		mare_avatar = WhichMonsterName(marename);
	}
#endif

	switch (this->CurrentAvatarType())
	{
#ifdef AGENT
		case Avatars::MALE:
		case Avatars::FEMALE:
			int rev_damage;
			int rev_effect;
			switch (mare_avatar){ // Revenant damage and effects based on nightmare they replaced
				case Avatars::EMPHANT: rev_damage = EMPHANT_DAMAGE; rev_effect = LyraEffect::NONE; break;
				case Avatars::BOGROM: rev_damage = BOGROM_DAMAGE; rev_effect = LyraEffect::PLAYER_CURSED; break;
				case Avatars::AGOKNIGHT: rev_damage = AGOKNIGHT_DAMAGE; rev_effect = LyraEffect::PLAYER_BLEED; break;
				case Avatars::SHAMBLIX: rev_damage = SHAMBLIX_DAMAGE; rev_effect = LyraEffect::PLAYER_POISONED; break;
				case Avatars::HORRON: rev_damage = HORRON_DAMAGE; rev_effect = LyraEffect::PLAYER_PARALYZED; break;
				default: rev_damage = SHAMBLIX_DAMAGE; rev_effect = LyraEffect::NONE; break;
			}
			switch (rand()%750)
			{ // All Revenant have a chance to apply these effects, strength based on mare type
				case 0: // Abjure the target instead
					gs->SendPlayerMessage(target, RMsg_PlayerMsg::ABJURE, (mare_avatar*10), 0);
					break;
				case 1: // Darkness the room instead
					gs->SendPlayerMessage(0, RMsg_PlayerMsg::DARKNESS, (mare_avatar*10), 0);
					break;
				case 3: // Terrorize the room instead
					gs->SendPlayerMessage(0, RMsg_PlayerMsg::TERROR, (mare_avatar*10),0);
					break;
				case 4: // Firestorm the room instead
					gs->SendPlayerMessage(0, RMsg_PlayerMsg::FIRESTORM, (mare_avatar*10),0);
					break;
				case 5: // Tempest the room instead
					gs->SendPlayerMessage(0, RMsg_PlayerMsg::TEMPEST, (mare_avatar*10), player->angle/4);
					break;
				case 6: // DreamQuake the room instead
					gs->SendPlayerMessage(0, RMsg_PlayerMsg::EARTHQUAKE, (mare_avatar*10),0);
					break;
				default:
					if ((mare_avatar) > rand()%10)
						return gs->PlayerAttack(LyraBitmap::FIREBALL_MISSILE, 3, rev_effect, rev_damage);
					else
						return gs->PlayerAttack(LyraBitmap::DREAMBLADE_MISSILE, MELEE_VELOCITY, rev_effect, rev_damage);
					break;
			}
#endif

		case Avatars::EMPHANT: // 1-4 damage, melee
			return gs->PlayerAttack(LyraBitmap::MARE_MELEE_MISSILE, MELEE_VELOCITY, LyraEffect::NONE, EMPHANT_DAMAGE);

		case Avatars::BOGROM:	// 4-7 damage, melee
#ifdef AGENT // special power for agents 
			if ((rand()%1000) == 0) // invis instead
				this->SetTimedEffect(LyraEffect::PLAYER_INVISIBLE, 10000, player->ID());
			else
#else
#ifdef PMARE //special power for pmares
			if ((rand()%100) == 0) // invis instead
				this->SetTimedEffect(LyraEffect::PLAYER_INVISIBLE, 10000, player->ID());
#endif
#endif
				return gs->PlayerAttack(LyraBitmap::MARE_MELEE_MISSILE, MELEE_VELOCITY, LyraEffect::NONE, BOGROM_DAMAGE);
				break;

		case Avatars::AGOKNIGHT:  // 10-20 damage, melee, can roar and blast
#ifdef AGENT
			if ((rand()%250) == 0) // roar and blast instead
			{
				if (target)
					gs->SendPlayerMessage(target, RMsg_PlayerMsg::BLAST, 30, 0);
				gs->SetLastSound(LyraSound::AGOKNIGHT_ROAR);//gs->SendPlayerMessage(0, RMsg_PlayerMsg::TRIGGER_SOUND, LyraSound::AGOKNIGHT_ROAR, 0);
			}
			else
#endif
				return gs->PlayerAttack(LyraBitmap::MARE_MELEE_MISSILE, MELEE_VELOCITY, LyraEffect::NONE, AGOKNIGHT_DAMAGE);
			break;

		case Avatars::SHAMBLIX: 
			// 10-30 damage, paralysis fireballs
			return gs->PlayerAttack(LyraBitmap::FIREBALL_MISSILE, 3, LyraEffect::PLAYER_PARALYZED, SHAMBLIX_DAMAGE);

		case Avatars::HORRON: // 12-40 damage, blinding fireballs
			{
#ifdef AGENT
			if (target)
			{
				switch (rand()%1500)
				{
				case 0: // Abjure the target instead
					gs->SendPlayerMessage(target, RMsg_PlayerMsg::ABJURE, 10, 0);
					gs->SetLastSound(LyraSound::MONSTER_ROAR);
					break;
				case 1: // Razorwind the room instead
					gs->SendPlayerMessage(0, RMsg_PlayerMsg::RAZORWIND, 50, 0);
					gs->SetLastSound(LyraSound::MONSTER_ROAR);
					break;
				case 3: // Tempest the room instead
					gs->SendPlayerMessage(0, RMsg_PlayerMsg::TEMPEST, 50, player->angle/4);
					gs->SetLastSound(LyraSound::MONSTER_ROAR);
					break;
				default:
					break;
				}

			}
#endif
			return gs->PlayerAttack(LyraBitmap::FIREBALL_MISSILE, -5, LyraEffect::PLAYER_BLIND, HORRON_DAMAGE);
			break;
			}
	}

	return false;
}

short cPlayer::CurrentAvatarType(void)
{
	if (player->flags & ACTOR_TRANSFORMED)
		return transformed_avatar.AvatarType();
	else
		return this->AvatarType();
}


void cPlayer::SetGuildRank(int guild_id, int value)
{
	guild_ranks[guild_id].rank = value;
	if ((value == 0) && (avatar.GuildID() == guild_id))
	{	
		LmAvatar new_avatar = avatar;
		new_avatar.SetGuildRank(0);
		new_avatar.SetGuildID(Guild::NO_GUILD);
		new_avatar.SetShowGuild(Patches::DONT_SHOW);
		this->SetAvatar(new_avatar, true);
	}
	return;
}

void cPlayer::ValidateChecksums(void)
{
#ifndef AGENT
	// skill formula: checksum = (value*1000) xor magic_value
	int i;
	int value;

	if (checksum_incorrect || (!options.network) || 
		(NULL == gs) || (!gs->LoggedIntoGame()))
		return;

	for (i=0; i<NUM_ARTS; i++)
	{
		value = (skills[i].checksum ^ MAGIC_XOR_VAR);
		if (skills[i].skill && (value != skills[i].skill))
		{ // checksum doesn't match, and skill is non-zero
			if (options.network && gs && gs->LoggedIntoGame())
			{
				LoadString (hInstance, IDS_ERROR_SKILL_CHECKSUM, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, arts->Descrip(i), value, skills[i].skill);
				gs->Talk(disp_message, RMsg_Speech::AUTO_CHEAT, Lyra::ID_UNKNOWN, false);
			}
			checksum_incorrect = true;
			//LoadString (hInstance, IDS_CORRUPT_DATA, message, sizeof(message));
			//GAME_ERROR(message);
			//return;
		}
	}

	for (i=0; i<NUM_PLAYER_STATS; i++)
	{
		value = (stats[i].checksum ^ MAGIC_XOR_VAR);
		if  (((stats[i].current != Stats::STAT_MIN) && (stats[i].max != Stats::STAT_MIN)) &&
			 (value != (stats[i].current + stats[i].max)))
		{ // checksum doesn't match, and stat is non-zero
			if (options.network && gs && gs->LoggedIntoGame())
			{
				LoadString (hInstance, IDS_ERROR_STAT_CHECKSUM, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, stats[i].name, value, stats[i].current + stats[i].max);
				gs->Talk(message, RMsg_Speech::AUTO_CHEAT, Lyra::ID_UNKNOWN, false);
			}
			checksum_incorrect = true;
			//LoadString (hInstance, IDS_CORRUPT_DATA, message, sizeof(message));
			//GAME_ERROR(message);
			//return false;
		}
	}
#endif

	return;
}

int cPlayer::SetSkill(int art_id, int value, int how, lyra_id_t origin_id, bool initializing)
{

	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{

		LoadString (hInstance, IDS_INVALID_ART, temp_message, sizeof(temp_message));
		_stprintf(errbuf, temp_message, art_id);
		NONFATAL_ERROR(errbuf);
		return 0;
	}

	// validate checksums first
	if (value)
		this->ValidateChecksums();

	int orig_skill = skills[art_id].skill;

	if (how == SET_ABSOLUTE)
		skills[art_id].skill = value;
	else
		skills[art_id].skill += value;

	if (skills[art_id].skill > Stats::SKILL_MAX)
		skills[art_id].skill = Stats::SKILL_MAX;

#ifdef PMARE
	if (skills[art_id].skill > 1)
		skills[art_id].skill = 1;
#endif

	skills[art_id].checksum = (skills[art_id].skill) ^ MAGIC_XOR_VAR;

	if (cp)
		cp->UpdateArt(art_id);

	if ((origin_id != SERVER_UPDATE_ID) && (skills[art_id].skill != orig_skill))
	{	// update server immediately on skill change
		skills[art_id].needs_update = true;
		if (options.network && gs)
			gs->UpdateServer();
	}

	if ((skills[art_id].skill != orig_skill) && !initializing &&
		((skills[art_id].skill%10) == 9) && (art_id != Arts::LEVELTRAIN))
	{ // need teacher to go up in skill now
		LoadString (hInstance, IDS_SKILL_NEEDTEACHER, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, arts->Descrip(art_id));
		display->DisplayMessage(message);
	}

	return skills[art_id].skill;
}

// Change player XP; returns new xp.
int cPlayer::SetXP(int value, bool initializing)
{
	int old_xp = xp;
	xp = value;

	if (xp < 0)
		xp = 0;

#ifndef AGENT

	int old_orbit = orbit;

	orbit = LmStats::OrbitFromXP(xp);

	if (!initializing && gs && gs->LoggedIntoGame())
	{
		if (xp > old_xp)
		{
			LoadString (hInstance, IDS_GOT_XP, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, (xp - old_xp));
			display->DisplayMessage(message);
		}
		else if (xp < old_xp)
		{
			LoadString (hInstance, IDS_LOST_XP, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, (old_xp - xp));
			display->DisplayMessage(message);
		}

#ifdef PMARE
		if (old_orbit != orbit)
			cp->UpdateStats();
#else

		if (old_orbit < orbit)
		{
			LoadString (hInstance, IDS_PLAYER_GAINORBIT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			if ((orbit%10) == 9) // need teacher for next orbit
			{
				LoadString (hInstance, IDS_NEED_TEACHER, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
			}
			// check to see what new skills you can learn now
			for (int i=0; i<NUM_ARTS; i++)
			/* if ((skills[i].skill == 0) &&

					(i != Arts::LEVELTRAIN) &&
					((arts->MinOrbit(i) <= MAX_OUT_OF_FOCUS_MIN_ORBIT) ||
					 (arts->Stat(i) == Stats::NO_STAT) ||
					 (arts->Stat(i) == Stats::DREAMSOUL) ||
					 (arts->Stat(i) == this->FocusStat())))*/
				if ((arts->MinOrbit(i) == orbit) && arts->Learnable(i) && arts->DisplayLearnable(i))
				{
					LoadString (hInstance, IDS_CAN_LEARN_ART, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, arts->Descrip(i));
					display->DisplayMessage(message);
				}
		}
		else if (old_orbit > orbit)
		{
			LoadString (hInstance, IDS_PLAYER_LOSEORBIT, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			if ((orbit%10) == 9) // need teacher for next orbit
			{
				LoadString (hInstance, IDS_NEED_TEACHER, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
			}

		}
#endif
	}
#endif

	return xp;
}

// reform after being a soulsphere
void cPlayer::ReformAvatar(void)
{
	if (!(flags & ACTOR_SOULSPHERE) || !(stats[Stats::DREAMSOUL].max))
		return;

	LoadString (hInstance, IDS_AVATAR_REFORMED, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message, false);
#if defined (AGENT) || defined (PMARE)
	this->SetCurrStat(Stats::DREAMSOUL, stats[Stats::DREAMSOUL].max, SET_ABSOLUTE, playerID);
#else
	if (this->CurrStat(Stats::DREAMSOUL) < 1)
		this->SetCurrStat(Stats::DREAMSOUL, 1, SET_ABSOLUTE, playerID);
#endif
	flags = flags & ~ACTOR_SOULSPHERE;
	forming = true;
	injured = dissolving = false;
	animate_ticks = 0;
	currframe = 0;
	gs->UpdateServer(); // update server on reformation

	cDS->PlaySound(LyraSound::HOLYLIGHT, x, y, true);
	if (flags & ACTOR_SOULEVOKE)
		this->RemoveTimedEffect(LyraEffect::PLAYER_SOULEVOKE);
	return;
}


// Player has just suffered an "avatar death"
// origin_id is the player id of whomever caused the little death
void cPlayer::Dissolve(lyra_id_t origin_id, int talisman_strength)
{
#ifndef AGENT
	cItem *item;
#endif

	cNeighbor *n;
	LmItem info;
	LmItemHdr header;
	int target=0,count=0,i,j;
	bool recall_active = (flags & ACTOR_RECALL);

	if (flags & ACTOR_SOULSPHERE)
		return;

	collapse_time = LyraTime();
	next_collapse_index++;
	if (next_collapse_index == COLLAPSES_TRACKED) 
		next_collapse_index = 0;

	collapses[next_collapse_index].collapser_id = origin_id;
	collapses[next_collapse_index].time = collapse_time; 

	this->PerformedAction(); // wrecks evoking, etc.

	n = actors->LookUpNeighbor(origin_id);
	if ((n == NO_ACTOR) || (origin_id == playerID))
	{	// dissolved by our own hand
#ifdef PMARE
		LoadString (hInstance, IDS_PMARE_DISSOLVED_BY_SELF, disp_message, sizeof(disp_message));
#else
		LoadString (hInstance, IDS_PLAYER_DISSOLVED_BY_SELF, disp_message, sizeof(disp_message));
#endif
		if (gs && gs->LoggedIntoLevel()) // don't display on login
			display->DisplayMessage(disp_message);
	}
	else // someone else took us down
	{
#ifdef PMARE
		LoadString (hInstance, IDS_PMARE_DISSOLVED_BY_OTHER, disp_message, sizeof(disp_message));
#else
		LoadString (hInstance, IDS_PLAYER_DISSOLVED_BY_OTHER, disp_message, sizeof(disp_message));
#endif
		_stprintf(message, disp_message, n->Name());
		display->DisplayMessage(message);

#ifndef AGENT
		LoadString(hInstance, IDS_ANNOUNCE_COLLAPSE, message, sizeof(message));
		if (n != NO_ACTOR) {
			_stprintf(disp_message, message, n->Name());
		} else {
			LoadString(hInstance, IDS_ANOTHER_DREAMER, temp_message, sizeof(temp_message));
			_stprintf(disp_message, message, temp_message);
		}
		gs->Talk(disp_message, RMsg_Speech::EMOTE, Lyra::ID_UNKNOWN);

		// check to see if this other entity has collapsed us too many
		// times recently - if so, issue a cheat report
		int num_collapses = 0;
		for (i=0; i<COLLAPSES_TRACKED; i++) 
		{
			if ((collapses[i].collapser_id == origin_id) &&
				((collapse_time - collapses[i].time) < COLLAPSE_TRACK_INTERVAL))
				num_collapses++;
		}
		if (num_collapses > COLLAPSE_CHEAT_THRESHHOLD)
		{
			LoadString (hInstance, IDS_TOO_MANY_COLLAPSES, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, origin_id, player->ID(), num_collapses, (int)((COLLAPSE_TRACK_INTERVAL/1000)));
			gs->Talk(message, RMsg_Speech::AUTO_CHEAT, Lyra::ID_UNKNOWN, true);
		}
#endif
	}

	for (i=0; i<NUM_TIMED_EFFECTS; i++)
		this->RemoveTimedEffect(i);

	// If we don't do this, regeneration kicks in and restores us.

	if ( player->flags & ACTOR_REGENERATING )
		player->flags &= ~ACTOR_REGENERATING;

	flags = flags | ACTOR_SOULSPHERE;
	dissolving = true;
	forming = false;
	currframe = 0;
	animate_ticks = 0;

	// determine if we're in danger of losing a sphere
	unsigned int sphere_xp = lyra_xp_table[this->Sphere()*10].xp_base;
	unsigned int new_xp = player->XP()-.01*player->XP();
	if ((player->Sphere() >= 1) &&
		(new_xp >= sphere_xp) && 
		((new_xp - .01*new_xp) < sphere_xp))
	{ // in danger of losing a sphere
		LoadString (hInstance, IDS_SPHERE_THREATENED, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
	}

	if (options.network && gs && gs->LoggedIntoLevel())
	{
		gs->SendPositionUpdate(TRIGGER_DEATH);
		gs->UpdateServer(); // update server on dissolution
		i = 1; // talisman value
		j = orbit;
#ifdef AGENT
	if (this->AvatarType() < Avatars::MIN_NIGHTMARE_TYPE)
	{ // Revenant borrow attack strength from the nightmare agent they replace based on agent username e.g. Shamblix_14=Shamblix
		int pi;
		TCHAR marename[Lyra::PLAYERNAME_MAX];
		// *** STRING LITERAL ***  
		if (_stscanf(agent_info[AgentIndex()].name, "%[^_]_%d", marename, &pi) != 2) {
		  // couldn't parse it
		 _tcsnccpy(marename, agent_info[AgentIndex()].name, sizeof(marename));
		}
		j = (100 +  WhichMonsterName(marename));
	}
	else
	{
		if (this->CurrStat(Stats::DREAMSOUL) > 0)
			i = this->CurrStat(Stats::DREAMSOUL);
		j = 100 + this->AvatarType(); // night mare = 100+
		blast_chance = 0; // reset Ago's Blast Chance on collapse
	}
#else 
// if player mare = 200+
#ifdef PMARE
		j = 150 + this->AvatarType();
#else 
#ifdef GAMEMASTER // nightmare possession, dark mare orbit = 200 + nightmare index
		if ((avatar.AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) &&
			!(flags & ACTOR_TRANSFORMED))
			j = 200 + this->AvatarType();
#endif // pmare
#endif // gm
#endif // agent
		gs->SendPlayerMessage(origin_id, RMsg_PlayerMsg::YOUGOTME, j, i);

#ifndef AGENT
		// drop artifacts
		for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			if ((item->Status() == ITEM_OWNED) && (item->AlwaysDrop()))
				item->Drop(x,y,angle);
		actors->IterateItems(DONE);

		// count items
		int item_count = 0;
		for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			if ((item->Status() == ITEM_OWNED) && item->Losable())
				item_count++;
		actors->IterateItems(DONE);

		if (item_count)
			target = rand()%(item_count);
		for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			if ((item->Status() == ITEM_OWNED) && item->Losable() &&
				(count == target))
			{
				item->Drop(x,y,angle);
				break;
			}
			else
				count++;
		actors->IterateItems(DONE);
#endif
	}

	if (level->ID() == 46)
		this->Teleport(6426,2534,angle, 46);
	else if (recall_active)
	{
		this->Teleport( recall_x, recall_y, recall_angle, recall_level);
		this->RemoveTimedEffect(LyraEffect::PLAYER_RECALL);
	}


	this->ResetEyeHeight();
	return;
}

// returns the # of guilds in which the player has the given rank;
// set rank to Guild::RULER_PENDING to see the # of guilds in which the
// player has any rank
int cPlayer::NumGuilds(int rank)
{
	int i, num_guilds;

	for (i=num_guilds=0; i<NUM_GUILDS; i++)
		if (((rank == Guild::RULER_PENDING) && (guild_ranks[i].rank >= Guild::INITIATE))
			|| (guild_ranks[i].rank == rank))
			num_guilds++;

	return num_guilds;
}

bool cPlayer::IsUninitiated(void)
{
	if (!this->IsInitiate() && !this->IsKnight() && !this->IsRuler())
		return true;
	else
		return false;
}

// set guild_id to Guild::NO_GUILD to see if the player is in any guild
bool cPlayer::IsInitiate(int guild_id)
{
	bool is_ruler = false;

	if (guild_id != Guild::NO_GUILD)
		return (guild_ranks[guild_id].rank == Guild::INITIATE);

	for (int i=0; i<NUM_GUILDS; i++)
		if (guild_ranks[i].rank == Guild::INITIATE)
			return true;

	return false;
}

// set guild_id to Guild::NO_GUILD to see if the player is a knight in any guild
bool cPlayer::IsKnight(int guild_id)
{
	bool is_knight = false;

	if (guild_id != Guild::NO_GUILD)
		return (guild_ranks[guild_id].rank == Guild::KNIGHT);

	for (int i=0; i<NUM_GUILDS; i++)
		if (guild_ranks[i].rank == Guild::KNIGHT)
			return true;

	return false;

}

// set guild_id to Guild::NO_GUILD to see if the player is a ruler in any guild
bool cPlayer::IsRuler(int guild_id)
{
	bool is_ruler = false;

	if (guild_id != Guild::NO_GUILD)
		return (guild_ranks[guild_id].rank == Guild::RULER);

	for (int i=0; i<NUM_GUILDS; i++)
		if (guild_ranks[i].rank == Guild::RULER)
			return true;

	return false;
}

// returns true if we're a Female; based on avatar type
bool cPlayer::IsFemale (void)
{
	if (avatar.AvatarType() == Avatars::FEMALE)
		return true;
	else
		return false;
}


// returns true if we're a Mmale; based on avatar type
bool cPlayer::IsMale (void)
{
	if (avatar.AvatarType() == Avatars::MALE)
		return true;
	else
		return false;
}

// returns true if we're a monster; based on avatar type
unsigned int cPlayer::GetMonsterType (void)
{
	return avatar.AvatarType();
}

// returns true if we're a monster; based on avatar type
unsigned int cPlayer::GetTransformedMonsterType (void)
{
	return transformed_avatar.AvatarType();
}

// returns true if we're a monster; based on avatar type
bool cPlayer::IsMonster (void)
{
	if (this->GetMonsterType() >= Avatars::MIN_NIGHTMARE_TYPE)
		return true;
	else
		return false;
}

// returns true if we're possessed;
bool cPlayer::IsPossesed (void)
{
	if (player->GetTransformedMonsterType() >= Avatars::MIN_NIGHTMARE_TYPE)
		return true;
	else
		return false;
}

// returns true if we're a pmare; based on account type
bool cPlayer::IsPMare (void)
{
	if (this->GetAccountType() == LmAvatar::ACCT_PMARE)
		return true;
	return false;
}

// returns true if we're a dreamer; based on account type
bool cPlayer::IsDreamerAccount (void)
{
	if ((this->GetAccountType() == LmAvatar::ACCT_DREAMER) ||
		(this->GetAccountType() == LmAvatar::ACCT_ADMIN))
		return true;
	else
		return false;
}

// make a guild flag byte based on guild membership;
// only include guilds of the given rank, unless rank = Guild::RULER_PENDING,
// in which case include any guild in which the player has rank
unsigned char cPlayer::GuildFlags(int rank)
{
	unsigned char guild_flags = 0;
	unsigned char curr_flag = 0x01;

	for (int i=0; i<NUM_GUILDS; i++)
	{
		if (((rank == Guild::RULER_PENDING) && (guild_ranks[i].rank >= Guild::INITIATE))
			|| (guild_ranks[i].rank == rank))
			guild_flags = guild_flags | curr_flag;
		curr_flag = curr_flag<<1;
	}
	return guild_flags;
}

DWORD cPlayer::EffectExpire(int effect)
{
	return timed_effects->expires[effect];
};

void cPlayer::DisplayTimedEffects(void)
{
	int num_effects = 0;
	LoadString (hInstance, IDS_ACTIVE_EFFECTS, message, sizeof(message));
	for (int i=1; i<NUM_TIMED_EFFECTS; i++)
		if (timed_effects->actor_flag[i] & flags)
		{
			// Do not show curse as an active effect (requested by Balthiir)
			if (i != LyraEffect::PLAYER_CURSED) {
				// Just load the string at the beginning - Jared
				//if (num_effects == 0)
				//		LoadString (hInstance, IDS_ACTIVE_EFFECTS, message, sizeof(message));
				num_effects++;	
			_tcscat(message, _T("  "));
			_tcscat(message, timed_effects->name[i]);
			_tcscat(message, _T(" ("));
				int duration = (timed_effects->expires[i] - LyraTime())/1000;
				if (duration < 30)
					LoadString (hInstance, IDS_DUR_VERY_SHORT, disp_message, sizeof(disp_message));
				else if (duration < 120)
					LoadString (hInstance, IDS_DUR_SHORT, disp_message, sizeof(disp_message));
				else if (duration < 600)
					LoadString (hInstance, IDS_DUR_MID, disp_message, sizeof(disp_message));
				else if (duration < 3000)
					LoadString (hInstance, IDS_DUR_LONG, disp_message, sizeof(disp_message));
				else
					LoadString (hInstance, IDS_DUR_VERY_LONG, disp_message, sizeof(disp_message));
			_tcscat(message, disp_message);
			_tcscat(message, _T(")    \n"));
			}
		}

#ifdef GAMEMASTER
	if (options.invulnerable)
		LoadString (hInstance, IDS_GM_INVUL, message, sizeof(message));
#endif

	if ((num_effects == 0) && (!options.invulnerable))
		LoadString (hInstance, IDS_NO_ACTIVE_EFFECTS, message, sizeof(message));

	display->DisplayMessage(message, false);
	return;
}

void cPlayer::MarkLastLocation(void)
{
	if (options.network && gs && gs->LoggedIntoLevel())
	{
		last_x = x;
		last_y = y;
		last_level = level->ID();
		last_loc_valid = true;
	}
	else
		last_loc_valid = false;
}


bool cPlayer::CanUseChatMacros()
{
#ifdef GAMEMASTER
	return true;
#endif
	return IsKnight() || IsRuler() || IsTeacher();
}


POINT cPlayer::BladePos(void)
{
	const static POINT blade_positions[MAX_RESOLUTIONS][6] =
	{
		{	{200,-80},	{160,0},  {120,0}, 	{80,80}, {40,160}, {00,240}}, 
		{	{310,-100},	{260,0},  {210,0}, 	{160,100}, {110,200}, {60,300}},
		{	{420,-128},	{356,0},  {292,0}, 	{228,128}, {144,256}, {100,384}}
	};

	const POINT *p = &blade_positions[cDD->Res()][blade_position_index];
	if (blade_position_index >= 5)
	{
		blade_position_index = 0;
		SetUsingBlade(false);
	}

	return *p;
}

int cPlayer::Skill(int art_id) 
{ 
	// if it's a PP evoke, return the skill we payed for!
	if (pp.in_use && (pp.cursel == GMsg_UsePPoint::USE_ART) &&
		(art_id == pp.art_id))
		return pp.skill;
#if defined (UL_DEBUG) || defined (GAMEMASTER) // no sphere restrictions in debug builds
	return skills[art_id].skill;
#else // otherwise, skill is limited by orbit, unless it is blade or flame,
	  // where lowering the skill level would cause a server error,
	  // or a focus skill, where the player couldn't use items in inventory
	if ((art_id == Arts::DREAMBLADE) || 
		((art_id >= Arts::GATESMASHER) && (art_id <= Arts::FLAMERUIN)) ||
		((art_id >= Arts::GATEKEEPER) && (art_id <= Arts::FATESENDER)))
		return skills[art_id].skill; 
	else if ((orbit == 0) && (skills[art_id].skill) == 1)
		return 1;
	else if ((skills[art_id].skill) <= orbit)
		return skills[art_id].skill; 
	else 
		return orbit;
#endif
}

bool cPlayer::RetryTeleport (void)
{
	return this->Teleport(tportx, tporty, tport_angle, tport_level, tport_sound, true);
}

// if level_id is NO_LEVEL, don't do level change
// if retry is true, it means we're retrying a previously blocked teleport, so don't 
// display any more error messages
bool cPlayer::Teleport( float x, float y, int facing_angle, int level_id, int sound_id, bool retry)
{

	VERIFY_XY(x, y);

	tportx=x; tporty=y; tport_angle=facing_angle; tport_level=level_id; tport_sound=sound_id;

	bool level_change = false;

	int	last_room = Room();
	float old_x = this->x;
	float old_y = this->y;

	attempting_teleport = true;

	if (acceptrejectdlg)
	{	// auto reject when leaving the area
		cDS->PlaySound(LyraSound::MESSAGE);
		SendMessage(hwnd_acceptreject, WM_COMMAND, MAKEWPARAM(IDC_REJECT,0), 0);
	//	LoadString (hInstance, IDS_RESPOND_FIRST, disp_message, sizeof(disp_message));
	//	display->DisplayMessage(disp_message);
	//	return false;
	}

	if (options.network && gs->LoggedIntoLevel() && !gs->LoggedIntoRoom())
	{ // don't allow them to leave until they answer a query, or
	  // if we haven't gotten the room entry item drop message yet
		LoadString (hInstance, IDS_AWAIT_ORIENTATION, disp_message, sizeof(disp_message));
		if (!retry)
		{
			display->DisplayMessage(disp_message);
			cDS->PlaySound(LyraSound::MESSAGE);
		}
		return false;
	}

	this->MarkLastLocation();

	// play sound for teleport
	cDS->PlaySound(sound_id,old_x,old_y);

	if ((level_id != NO_LEVEL) && (level_id != level->ID()))
	{ // go to new level, if it's reasonable
		if (!level->IsValidLevel(level_id))
		{
			player->Teleport(LastX(), LastY(), 0, LastLevel());
			return false;
		}

		if ((options.network) && gs && gs->LoggedIntoGame())
			gs->LevelLogout(RMsg_Logout::LOGOUT);

		if (options.network)
			cDD->ShowIntroBitmap();

		level->Load(level_id);

		level_change = true;

#ifdef GAMEMASTER
		if (agentbox)
			agentbox->SortAgents();
#endif

	}

	// move	to current location
	int new_sector = FindSector(x, y, 0, true);
	if (new_sector == DEAD_SECTOR)
	{
		player->Teleport(LastX(), LastY(), 0, LastLevel());
		return false;
	}

	this->PlaceActor(x, y, 0, facing_angle, SET_XHEIGHT, true);

	this->SetTeleporting(true);
	this->SetFreeMoves(NUM_FREE_MOVES);

	// reset owned items to be with the player; this is important
	for (cItem* item = actors->IterateItems(INIT); item != NO_ITEM; item = actors->IterateItems(NEXT))
		if (item->Status() == ITEM_OWNED)
			item->PlaceActor(this->x, this->y, 0, this->angle, SET_XHEIGHT, true);
	actors->IterateItems(DONE);

	if (level_change)
	{
		if (options.network)
			gs->LevelLogin();
	}
	else
	{
		if ((this->Room() != (lyra_id_t)last_room) && gs && gs->LoggedIntoLevel())
		{
			this->MarkLastLocation();
			gs->OnRoomChange((short)old_x, short(old_y));
		}
	}

	attempting_teleport = false;

	return true;
}

// Destructor
cPlayer::~cPlayer(void)
{
}

#ifdef CHECK_INVARIANTS
void cPlayer::CheckInvariants(int line, TCHAR *file)
{

}
#endif