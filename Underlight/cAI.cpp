 // The Player Class

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT
//#define AGDBG
//#define BEN

#include <winsock2.h>
#include <Windows.h> // needed for death dialog
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include "cGameServer.h"
#include "cMissile.h"
#include "AMsg_All.h"
#include "cActorList.h"
#include "cActor.h"
#include "cDDraw.h"
#include "cLevel.h"
#include "4dx.h"
#include "Move.h"
#include "cEffects.h"
#include "Effects.h"
#include "Options.h"
#include "cChat.h"
#include "Realm.h"
#include "cOrnament.h"
#include "cAI.h"
#include "Options.h"
#include "SharedConstants.h"
#include "utils.h"
#include "Resource.h"

#undef gs
#undef level
#undef actors
#undef timing
#undef output
#undef cDD

#if (defined(PLAYER_MARE)) || (defined(AGENT))

//////////////////////////////////////////////////////////////////
// Constants

// const unsigned int NEIGHBOR_CHECK_INTERVAL = 5000; // Original Lyra value - DiscoWay
const unsigned int NEIGHBOR_CHECK_INTERVAL = 5000; // Testing - Less attack delay on avatar portal entry? - DiscoWay
const unsigned int CLOSE_ANGLE = Angle_45;
const unsigned int STAT_WRITE_INTERVAL = (3600000 * 8); // once every 8 hours
const float MELEE_RANGE = 140.0f; // copied from cmissile
const int FIGHT_LEVEL = -1; // level where they can fight
const float MOVE_THRESHHOLD = 5.0f; // determine if stuck... // 15
const float STUCK_FRAMES_THRESHHOLD = 20; // 100
const int NUM_UNSTICK_MOVES = 10; // try to unstick // 25
const int NUM_AVOID_WALL_TRIES = 5; // try to avoid walls
const int SIGHTLESS_FRAMES_THRESHOLD = 800; // only search for a lost enemy so long
const float UNSTICK_WALL_DISTANCE = 300.0; // detect which wall we're stuck on
const float AVOID_WALL_DISTANCE = 150; // detect if we're running into a wall
const float STRAFE_DISTANCE = 50;
const int MAX_PLAYERS_AT_SPAWN = 2;
const int MAX_AGENTS_AT_SPAWN =  1;



//////////////////////////////////////////////////////////////////
// External Global Variables

// * important: don't extern in the variables that have related pointers
// in the agent_info array; instead access them directly using the pointers
// in the thread specific data

extern cEffects *effects;
extern options_t options;
extern HINSTANCE hInstance;

/////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
cAI::cAI(float xpos, float ypos, int anglepos, int delay, int mare_type /* = 5 */) :
		cPlayer(0)
{
	srand(LyraTime());

	x=xpos; y=ypos, angle=anglepos;

// agent_type = WhichMonsterName(options.username);
	agent_type = mare_type;

	if ((agent_type<Avatars::MIN_AVATAR_TYPE) || (agent_type>Avatars::MAX_AVATAR_TYPE))//	if (!agent_type)
		agent_type = 5; // default to Sham if not a valid agent type.  Male & Female are Revenant agents

	kills = deaths = num_stuck_frames = num_sightless_frames = 0;

	last_neighbor_check_time = last_stat_write_time = 0;

	num_rampaging_frames = alone_ticks = social_ticks = 0;

	num_frames_to_rampage = 100 + rand()%200; // How tempormental is this mare?

	reconnect = LyraTime() + delay;

	// _tprintf(_T("Agent spawn time = %d, delay = %d\n"), spawn_time, delay);

	login_time = LyraTime();

	distances_calculated = moved = FALSE;

	last_evasive_direction = LEFT;
	if (rand()%2 == 0)
		last_evasive_direction = RIGHT;

	speed = SHAMBLE_SPEED;

	last_target = Lyra::ID_UNKNOWN;
	last_target_x = last_target_y = -1;

	view_missile = NULL;

	if (agent_type<Avatars::MIN_NIGHTMARE_TYPE){ // If Revenant Male or Female form, attack nightmares in room
		attack_other_mares = true;
	} else {
		attack_other_mares = false; //false; //((rand()%10)==0); // false;
	}

	has_been_struck = rampaging = unsticking = wandering = false;
	alone = true;

	for (int i=0; i<NUM_PLAYER_STATS; i++)
	{
		stats[i].current = stats[i].max = Stats::STAT_MIN;
		stats[i].current_needs_update = false;
	}
}

void cAI::Init(void)
{
	view_missile = new cMissile(this, LyraBitmap::FIREBALL_MISSILE,
			this->angle,  this->HeightDelta(), MAX_VELOCITY,
			LyraEffect::NONE, 0, NULL, 0, sector);
}

TCHAR* cAI::Name(void)
{
#ifdef PLAYER_MARE
	return this->cPlayer::Name();
#else
	if (agent_info[AgentIndex()].gs_ptr->LoggedIntoGame())
		return NightmareName(agent_info[AgentIndex()].type);
	else
		return agent_info[AgentIndex()].name;
#endif
}

TCHAR* cAI::Password(void)
{
#ifdef PLAYER_MARE
	return this->cPlayer::Password();
#else
	return agent_info[AgentIndex()].passwd;
#endif
}

// set stats on an agent type specific basis
void cAI::SetAgentStats(void)
{
	avatar.SetAvatarType(agent_type);

// the main colors (regions 0 and 1 on the nightmare image)
// a. regions 0 and 1 are paired for the "red" hues
// b. regions 2 and 3 are paired for the "green"
// c. regions 4 and 5 and paired for "blue" hues

// the eye colors are reserved for regions 6 and 7

// regions 0 and 1 must be mapped to region "0 and 1" OR "2 and 3" OR "4 and 5" on the multi 64 palette
// regions 2 and 3 must be mapped to region "6 and 7" on the multi64 palette


// set eyes
	if (agent_type < Avatars::MIN_NIGHTMARE_TYPE){ // Revenants get random clothing colors
		avatar.SetColor2(rand()%NUM_ACTOR_COLORS);
		avatar.SetColor3(rand()%NUM_ACTOR_COLORS);
		avatar.SetColor4(rand()%NUM_ACTOR_COLORS);
	} else {
		avatar.SetColor2(6);
		avatar.SetColor3(7);
	}

// if color is set to 1, 2, or 3 in the agents file, set main mare
// colors to 0/1, 2/3, or 4/5, respectively
// if color is set to 0 in the agents file, pick randomly

	int color = agent_info[AgentIndex()].color;

	if (agent_type < Avatars::MIN_NIGHTMARE_TYPE){ // Revenants get limited skin, random hair colors
		color = rand()%6;
		if (color > 0)
			avatar.SetColor1(color+10);
		else
			avatar.SetColor1(color);

		avatar.SetColor0(rand()%NUM_ACTOR_COLORS);

	} else { // Nightmares have colors set by agent file

		if (!color)
			color = (rand()%3)+1;

		switch (color)
		{
			case 1:
				avatar.SetColor0(0);
				avatar.SetColor1(1);
				break;
			case 2:
				avatar.SetColor0(2);
				avatar.SetColor1(3);
				break;
			case 3:
				avatar.SetColor0(4);
				avatar.SetColor1(5);
				break;
			case 4:
				avatar.SetColor0(7);
				avatar.SetColor1(3);
				break;
			default:
				avatar.SetColor0(4);
				avatar.SetColor1(5);
				break;
		}
	}

	avatar.SetExtraDamage(0);

	avatar.SetHidden(0);

	switch (avatar.AvatarType())
	{
		case Avatars::MALE:
		case Avatars::FEMALE: // Male & Female Revenant agents get same stats
			stats[Stats::DREAMSOUL].current = stats[Stats::DREAMSOUL].max = 300;
			min_distance = 120;	//120.0f;
			melee_only = false;
			speed = SHAMBLE_SPEED;
			this->SetTimedEffect(LyraEffect::PLAYER_REFLECT, 10000000, this->ID(), EffectOrigin::ART_EVOKE);
			this->SetTimedEffect(LyraEffect::PLAYER_DETECT_INVISIBLE, 10000000, this->ID(), EffectOrigin::ART_EVOKE);
			flags = flags | ACTOR_REFLECT | ACTOR_DETECT_INVIS;
			break;
		case Avatars::EMPHANT:
			stats[Stats::DREAMSOUL].current = stats[Stats::DREAMSOUL].max = 10;
			min_distance = 120;	//120.0f;
			melee_only = true;
			speed = SHAMBLE_SPEED;
			break;
		case Avatars::BOGROM:
			stats[Stats::DREAMSOUL].current = stats[Stats::DREAMSOUL].max = 50;
			min_distance = 120;	//120.0f;
			melee_only = true;
			speed = SHAMBLE_SPEED;
			break;
		case Avatars::AGOKNIGHT:
			stats[Stats::DREAMSOUL].current = stats[Stats::DREAMSOUL].max = 100;
			min_distance = 120;	//120.0f;
			melee_only = true;
			speed = WALK_SPEED;
			if (rand()%2)
			{
				this->SetTimedEffect(LyraEffect::PLAYER_DETECT_INVISIBLE, rand(), this->ID(), EffectOrigin::ART_EVOKE);
				flags = flags | ACTOR_DETECT_INVIS;
			}
			this->SetTimedEffect(LyraEffect::PLAYER_DETECT_INVISIBLE, 28800000, this->ID(), EffectOrigin::ART_EVOKE);
			flags = flags | ACTOR_DETECT_INVIS;
			break;
		case Avatars::SHAMBLIX:
			stats[Stats::DREAMSOUL].current = stats[Stats::DREAMSOUL].max = 200;
			min_distance = 250;	
			melee_only = false;
			speed = RUN_SPEED;
			if (rand()%7 == 0)
			{
				this->SetTimedEffect(LyraEffect::PLAYER_DETECT_INVISIBLE, 10000000, this->ID(), EffectOrigin::ART_EVOKE);
				flags = flags | ACTOR_DETECT_INVIS;
			}
			break;
		case Avatars::HORRON:
			stats[Stats::DREAMSOUL].current = stats[Stats::DREAMSOUL].max = 300;
			min_distance = 250;
			melee_only = false;
			speed = RUN_SPEED;
			if (rand()%5 == 0)
			{
				this->SetTimedEffect(LyraEffect::PLAYER_DETECT_INVISIBLE, 10000000, this->ID(), EffectOrigin::ART_EVOKE);
				flags = flags | ACTOR_DETECT_INVIS;
			}
			break;
		default:
			LoadString(hInstance, IDS_UNK_MONSTER_TYPE, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message,avatar.AvatarType(),this->Name());
			GAME_ERROR(message);
			return;
	}
	avatar.SetAccountType(LmAvatar::ACCT_NIGHTMARE);
	this->SetAvatar(avatar, true);
}

bool cAI::DetermineAlone(void)
{
	cNeighbor *n;

	soulsphere_neighbors = false;

	alone = true;

	if (!view_missile)
		this->Init();

	this->CheckMissile();

	num_neighbors = 0;
	for (n = agent_info[AgentIndex()].actors_ptr->IterateNeighbors(INIT); n != NO_ACTOR; n = agent_info[AgentIndex()].actors_ptr->IterateNeighbors(NEXT))
	{
		if (n->ID() == agent_info[AgentIndex()].id)
		{
			break;
		}
		if (n->GetAccountType() != LmAvatar::ACCT_NIGHTMARE)
			// There's someone around to impress
			alone = false;
		if ((!(n->flags & ACTOR_INVISIBLE) || ((this->flags & ACTOR_DETECT_INVIS) && !n->Avatar().PlayerInvis())))
		{
			if (!(n->IsAgentAccount()) || (attack_other_mares && n->IsMonster()))  // Nightmares do not attack, but Revenant agents attack Nightmares
			{
				//_tprintf(_T("Neighbor %s, type %d found\n"), n->Name(), n->GetAccountType());
				neighbors[num_neighbors] = n;
				num_neighbors++;
				if (num_neighbors == MAX_NEIGHBORS)
					break;
			}
		}
		if (n->flags & ACTOR_INVISIBLE)
			invis_neighbors = true;

		if (n->flags & ACTOR_SOULSPHERE)
			soulsphere_neighbors = true;
	}
	agent_info[AgentIndex()].actors_ptr->IterateNeighbors(DONE);

	return alone;
}

bool cAI::Update(void)
{
	velocity = MAXWALK;

	invis_neighbors = distances_calculated = moved = FALSE;

	((cAI*)player)->DetermineAlone();

	//_tprintf(_T("entering update\n"));
	//_tprintf(_T("dreamsoul: %d\n"),dreamsoul);

	// first, set up an array of neighbor pointers so we
	// can access neighbors by index number via the wrappers

	if ((LyraTime() - last_neighbor_check_time) > NEIGHBOR_CHECK_INTERVAL)
	{ // update stats on whether alone or with others
		last_neighbor_check_time = LyraTime();
		if (num_neighbors)
			social_ticks++;
		else
		{
			// if we're alone in the room, go back to our spawn position
			if (alone && agent_info[AgentIndex()].gs_ptr->LoggedIntoLevel() && !num_neighbors && !invis_neighbors && !soulsphere_neighbors)
			{
				this->PlaceActor(spawn_x, spawn_y, 0, angle, SET_XHEIGHT, true);
				alone_ticks++;
			}
		}
	}

	this->MakeMove();
	this->CheckStatus();
	this->ModifyHeight();

	DWORD tgt = LyraTime();

	if (reconnect && (tgt > reconnect))
	{
		if (agent_info[AgentIndex()].gs_ptr->LoggedIntoGame())
		{	// send message & query gs for # neighbors in each room
			reconnect = 0;

			// Send a "getplayers_msg")
			// This which will result in a "LEVELPLAYERS" message which will 
			// call "FindRespawn" to find us a home and log in.
			//TCHAR timebuf[128];
			_tprintf("Agent %d: Trying to find room populations at time %d\n",AgentIndex(), LyraTime());

			agent_info[AgentIndex()].gs_ptr->FindRoomPopulations(agent_info[AgentIndex()].level_ptr->ID());
		}
	}

#if 0 // disabled cuz no one ever looks at these!
	if ((tgt - last_stat_write_time) > STAT_WRITE_INTERVAL)
	{ // write out stats to log file every hour
		last_stat_write_time = tgt;
		int index = LookUpAgent(this->ID());
		int fr = agent_info[index].framerate;
		int stat = agent_info[index].status;
	_stprintf(disp_message,
			_T("INFO: AGENT STATUS")
			_T(" Curr DS = %d")	// stats[Stats::DREAMSOUL].current,
			_T(" Max DS = %d") 	// stats[Stats::DREAMSOUL].max,
			_T(" Kills = %d")		// this->Kills(),
			_T(" Deaths = %d") 	// this->Deaths(),
			_T(" Pct Busy = %d")	// this->PercentBusy(),
			_T(" FR = %d")			// agent_info[index].framerate,
			_T(" Status = %d\n"), // agent_info[index].status);

			stats[Stats::DREAMSOUL].current,
			stats[Stats::DREAMSOUL].max,
			this->Kills(),
			this->Deaths(),
			this->PercentBusy(),
			fr,
			stat);
		_tprintf(_T("%s"), disp_message);
	}
#endif

	//_tprintf(_T("exiting update\n"));

	return TRUE;
}

void cAI::MakeMove(void) 
{
	if (alone)
	{
		velocity = 0;
		return;
	}

	int target = -1;
	velocity = MAXWALK;

	// Reset eye level
	this->vertical_tilt_float = 0;
	this->vertical_tilt = (long)this->vertical_tilt_float;

	target = this->AcquireTarget();

	// if our target is still fading in, do nothing yet; this gives players
	// a chance to orient a bit before getting attacked
	
	// Do not uncomment the next line below, causes agentsrv to crash over and over - DiscoWay
	// _tprintf(_T("nb: %d %d %d\n"), neighbors[target]->CurrBitmap(0), LyraBitmap::ENTRYEXIT_EFFECT, (LyraBitmap::ENTRYEXIT_EFFECT + effects->EffectFrames(LyraBitmap::ENTRYEXIT_EFFECT)));
	
	// Uncommented as test - DiscoWay
	
	if ((target != -1) && this->TargetEntering(target)) {
		target = -1;
		last_target_x = -1;
		last_target_y = -1;
		velocity = 0;
	}
		
	// end of uncomment.  Was commented out between the else and the if just below - DiscoWay
	else if (flags & ACTOR_PARALYZED)
	{
		velocity = 0;
	}
	else if (target == -1) // No target
	{
		// if no one is visible, move towards last known location or just wander
		num_sightless_frames++;

		this->vertical_tilt_float = 0;
		this->vertical_tilt = (long)this->vertical_tilt_float;
		//visible = 0;
		if (rampaging)
			this->Rampage();
		else if (!this->PursueLostTarget())
		{
			// We're targetless and done looking for our last target. Wander
			this->Wander();
			last_target_x = last_target_y = -1;
			num_sightless_frames = SIGHTLESS_FRAMES_THRESHOLD + 1; // No need to count above this
		}
	}
	else if (neighbors[target]->flags & ACTOR_SOULSPHERE) // Target is a soulsphere. Wander
	{ 
		this->Wander();
	}
	else // Target is not a soulsphere and we can move, go get 'em!
	{ 
		this->PursueTarget(target);
	}
	return;
}

void cAI::PursueTarget(int target)
{
	if (target == -1)
		return;
	if (agent_type>=Avatars::MIN_NIGHTMARE_TYPE) // Only nightmares should speak Maren (not Revenant)
	{
		int r = (rand()%10000);
		if (r == 0) {LoadString(hInstance, IDS_GRRZT_PAL, disp_message, sizeof(disp_message)); _stprintf(message,disp_message,NeighborName(target)); Speak(message,target);}
		if (r == 1) {LoadString(hInstance, IDS_PALGA_ULPDA, disp_message, sizeof(disp_message)); _stprintf(message,disp_message,this->Name()); Speak(message,-1);}
		if (r == 2) {LoadString(hInstance, IDS_PA_PLAHKA, disp_message, sizeof(disp_message)); Speak(disp_message, -1);}
		if (r == 3) {LoadString(hInstance, IDS_KLOPTA_VANG, disp_message, sizeof(disp_message)); _stprintf(message,disp_message,NeighborName(target)); Speak(message,target);}
		if (r == 4) {LoadString(hInstance, IDS_PRAZAH, disp_message, sizeof(disp_message)); _stprintf(message,disp_message,NeighborName(target)); Speak(message,target);}
		if (r == 5) {LoadString(hInstance, IDS_GRAAAH, disp_message, sizeof(disp_message)); Speak(disp_message,-1);}
		if (r == 6) {LoadString(hInstance, IDS_UUURA, disp_message, sizeof(disp_message)); Speak(disp_message,-1);}
	}

	bool angle_close = false;
	this->StopRampaging();
	this->StopWandering();
	int desired_angle=this->GetNeighborFacingAngle(target);
	this->TurnTo(desired_angle);

	if ((abs(angle - desired_angle) < CLOSE_ANGLE) ||
			(abs(desired_angle - angle) > (Angle_360 - CLOSE_ANGLE)))
		angle_close = true;

	float ndist = NeighborDistance(target);


	if ((ndist > min_distance) && angle_close)
	{
		if (ndist < MELEE_RANGE) 
		{
			velocity = 0;
			if (!this->NeighborHittable(target)) 
			{
			//I'm close enough to be hit, but I can't hit back I'm being trapped!
				this->TakeEvasiveAction();
			}
			else 
			{
				// Slow down if we're within range and not trapped?
				num_stuck_frames = 0; // It's ok to be stuck if we're hitting
			}
		}
		MoveForward();
	}
	// force agents to backpedal to a visible dist if they get too close
	//else if (ndist < (min_distance - 20))
	else if (ndist <= min_distance)
	{
		if (!this->NeighborHittable(target)) {
			//I'm close enough to be hit, but I can't hit back I'm being trapped
			this->TakeEvasiveAction();
		}
		else {
			// Close enough to hit and within range. All should be well
			num_stuck_frames = 0; // It's ok to be stuck if we're hitting
		}	
		Move((int)FixAngle(this->angle+Angle_180));
	}
	else {
		// Just turn
		velocity = 0.0f;
		num_stuck_frames = 0;
	}

	// view-tilting for attack is now handled in visibility check

	if (angle_close && !(melee_only && (ndist > MELEE_RANGE)) && (rand()%3))
	{ // attack
		if (neighbors[target]->velocity)
		{ // target is moving -- try to lead it!
			int target_move_angle;
			if ((neighbors[target]->Strafing()) && (neighbors[target]->velocity > 0))
				target_move_angle = FixAngle(this->angle - FixAngle(neighbors[target]->angle+Angle_90));
			else if ((neighbors[target]->Strafing()) && (neighbors[target]->velocity < 0))
				target_move_angle = FixAngle(this->angle - FixAngle(neighbors[target]->angle-Angle_90));
			else
				target_move_angle = FixAngle(this->angle - neighbors[target]->angle);

			if ((target_move_angle > Angle_30)
				&& (target_move_angle < Angle_180-Angle_30))
				// target heading left
				this->AddAngle(32);
			else if ((target_move_angle > Angle_180+Angle_30)
				&& (target_move_angle < Angle_360-Angle_30))
				// target heading right
			this->AddAngle(-32);
		}
		this->NightmareAttack(neighbors[target]->ID());
	}
	return;
}

bool cAI::UnstickSelf(void)
{
	// IMPORTANT!! If unsticking is not set true before calling this method, 
	// we may end up in an infinite loop since UnstickSelf calls Move and
	// Move calls UnstickSelf
	if (unsticking == false) 
		return true;

//	int target = -1;
	int Angle_51 = Angle_45 + Angle_6;
	move_result_t move;
	direction_t unstick_turning_direction;
	// Choose a general direction to try turning
	if (rand()%2 == 0)
		unstick_turning_direction = LEFT;
	else
		unstick_turning_direction = RIGHT;


	for (int i=0; i<NUM_UNSTICK_MOVES; i++)
	{
		this->AddAngle(Angle_51);
		this->FireViewMissile(this->angle,UNSTICK_WALL_DISTANCE,&move);
		if (move.hit != HIT_WALL) {
			this->Strafe(RIGHT);
			//MoveActor(this,FixAngle(angle),velocity*SHAMBLE_SPEED*agent_info[AgentIndex()].timing_ptr->nticks,MOVE_NORMAL,&agent_move);
			MoveForward();
		}
		else {
			this->AddAngle(-2*Angle_51);
			FireViewMissile(this->angle,UNSTICK_WALL_DISTANCE,&move);

			if (move.hit != HIT_WALL) {
				this->Strafe(LEFT);
				//MoveActor(this,FixAngle(angle),velocity*SHAMBLE_SPEED*agent_info[AgentIndex()].timing_ptr->nticks,MOVE_NORMAL,&agent_move);
				MoveForward();
			}
			else {
				// It appears we're blocked both ways forward. Let's try to strafe free
				this->AddAngle(Angle_51); // Straighten out
				// Try left first
				FireViewMissile(this->angle-Angle_90,UNSTICK_WALL_DISTANCE,&move);
				if (move.hit != HIT_WALL) {
					this->Strafe(LEFT);
					MoveForward();
				}
				// If that failed, try right
				else {
					FireViewMissile(this->angle+Angle_90,UNSTICK_WALL_DISTANCE,&move);
					if (move.hit != HIT_WALL) {
						this->Strafe(RIGHT);
						MoveForward();
					}
					// If that failed, we may be screwed
					else {
						int target = -1;
						target = this->FindNearestNeighbor(0);
						if (target != -1) // We can see someone, but we can't get unstuck. Bail.
							return false;
						else { // If we can't see a target, try turning around
							this->SetAngle(this->angle + Angle_180);
							FireViewMissile(this->angle,UNSTICK_WALL_DISTANCE,&move);
							if (move.hit != HIT_WALL)
								MoveForward();
							else // We're screwed
								return false;
						}
					}
				}
			}
		}

//		target = this->FindNearestNeighbor(0);

		FireViewMissile(this->angle,UNSTICK_WALL_DISTANCE,&move);
		if ((move.hit != HIT_WALL)) //&& (target != -1))
		{
//			if (this->NeighborVisible(target))
//			{ // we've gotten to a good position - run with it!
				return true;
//			}
		}
	}
	return false;
}

// Sidestep left or right
float cAI::Strafe(direction_t direction) 
{
	move_result_t move;
	move.dist = 0.0f;
	int strafe_angle;

	this->MarkLastLocation();

	if (direction == LEFT)
		strafe_angle = (this->angle - Angle_90);
	else
		strafe_angle = (this->angle + Angle_90);
	MoveActor(this,FixAngle(strafe_angle),STRAFE_DISTANCE,MOVE_NORMAL,&move);
	if ((move.hit == HIT_JUMPABLE) && (vertforce == 0))
	{
		this->Jump();
	}
	return move.dist;
}

void cAI::Jump(void) 
{
		agent_info[AgentIndex()].gs_ptr->SetJump();
		vertforce=-40.0f;
		jumped = true;
}

void cAI::TurnTo(int desired_angle)
{
	turnrate = (agent_info[AgentIndex()].timing_ptr->nticks*WALK_SPEED*20);
	if (((desired_angle > angle) && (desired_angle - angle > Angle_180)) ||
		((desired_angle < angle) && (angle - desired_angle < Angle_180)))
		turnrate = -turnrate;
	if ((abs(angle - desired_angle) < abs((int)turnrate)) ||
		(abs(angle - desired_angle) > (Angle_360 - abs((int)turnrate))))
		this->SetAngle(desired_angle); // close enough to directly face
	else // turn to face
		this->AddAngle((int)turnrate);
	return;
}

int cAI::SetAngle(int new_angle)
{
	this->angle = (int)FixAngle(new_angle);
	return this->angle;
}

int cAI::AddAngle(int new_angle)
{
	this->SetAngle(this->angle + new_angle);
	return this->angle;
}

// Wander - Turn aimlessly
int cAI::WanderAngle(void) 
{
	if (rand()%8 == 0) // Don't always turn
	{
		// Turn up to 30 degrees right or left
		if (rand()%2 == 0)  // right
			this->AddAngle(Angle_6 + rand()%Angle_30);
		else
			this->AddAngle(-(Angle_6 + rand()%Angle_30));
	}
	return this->angle;
}

int cAI::EvasiveAngle(direction_t direction)
{
	int Angle_15 = Angle_45 - Angle_30;
	if (direction == LEFT)
		return this->AddAngle(-(Angle_15 + rand()%Angle_30));
	else
		return this->AddAngle(Angle_15 + rand()%Angle_30);
}

// Rampage!!
int cAI::RampageAngle(void)
{
	// Ranged mares attack when rampaging. Melee mares just run.
	if (this->avatar.AvatarType() >= Avatars::SHAMBLIX)
	{
		int old_angle = this->angle;
		this->AddAngle(Angle_90 + rand()%Angle_90);
		if (rand()%500 == 0) 
		{
			LoadString(hInstance, IDS_ROAR_RAGE, disp_message, sizeof(disp_message));
			_stprintf(message,disp_message); Speak(message,-3);
			agent_info[AgentIndex()].gs_ptr->SetLastSound(LyraSound::MONSTER_ROAR); //SendPlayerMessage(0, RMsg_PlayerMsg::TRIGGER_SOUND, LyraSound::MONSTER_ROAR, 0);
		}
		this->NightmareAttack();
		this->SetAngle(old_angle);
	}
	return this->WanderAngle();
}

float cAI::Wander(void)
{
	if ((agent_type >= Avatars::MIN_NIGHTMARE_TYPE) && (rand()%10000 == 0)) // Revenant do not speak Maren
	{
		LoadString(hInstance, IDS_VDERE_KLOPTA, disp_message, sizeof(disp_message));
		_stprintf(message,disp_message); Speak(message,-2); // Shout
	}

	wandering = true;
	this->TurnTo(this->WanderAngle());
	return this->MoveForward();
}

float cAI::Rampage(void)
{
	if (num_rampaging_frames < num_frames_to_rampage) 
	{
		num_rampaging_frames++;
		this->TurnTo(this->RampageAngle());
	}
	else
		this->StopRampaging();
	return this->MoveForward();
}

// Evade - Strafe around an opponent
void cAI::TakeEvasiveAction(void) 
{
	move_result_t move;
	if (last_evasive_direction == LEFT) 
	{
		this->Strafe(LEFT);
		this->SetAngle(this->EvasiveAngle(LEFT));
		FireViewMissile(this->angle-Angle_90,UNSTICK_WALL_DISTANCE,&move);
		if ((move.hit == HIT_WALL) || (move.hit == HIT_ACTOR))
			// Hit something. Try the other direction
			last_evasive_direction = RIGHT;
	}
	else 
	{
		this->Strafe(RIGHT);
		this->SetAngle(this->EvasiveAngle(RIGHT));
		FireViewMissile(this->angle+Angle_90,UNSTICK_WALL_DISTANCE,&move);
		if ((move.hit == HIT_WALL) || (move.hit == HIT_ACTOR))
			// Hit something. Try the other direction
			last_evasive_direction = LEFT;
	}

	// Occasionally jump just to mix it up
	if (rand()%100 == 0) { 
		this->Jump();
	}

	this->MoveForward();
	return;
}

bool cAI::PursueLostTarget(void)
{

	if (((last_target_x != -1) || (last_target_y != -1))
		|| (num_sightless_frames > SIGHTLESS_FRAMES_THRESHOLD))
		return false;

	if ((fabs(last_target_x - x) < MOVE_THRESHHOLD) && (fabs(last_target_y - y) < MOVE_THRESHHOLD)) {
		// We've arrived at the last known location. We can stop seeking now.
		last_target_x = last_target_y = -1;
		num_sightless_frames = 0;
		return false;
	}
	else
	{
		int desired_angle=GetFacingAngle(last_target_x,last_target_y,x,y);
		this->TurnTo(desired_angle);
	}
	this->MoveForward();
	return true;
}

float cAI::MoveForward(void)
{
	return this->Move(this->angle);
}

// moves the agent with the given angle
float cAI::Move(int move_angle)
{
	move_result_t move;
	float move_speed = speed;

	move.dist = 0.0f;
	
	this->MarkLastLocation();

	move_angle = (this->AvoidWallAngle(move_angle));

	if (wandering) { // slow down when wandering
		move_speed = SHAMBLE_SPEED; 
	}
	if (rampaging) {
		move_speed = WALK_SPEED;
	}

	if (!moved) {
		MoveActor(this,FixAngle(move_angle),velocity*move_speed*agent_info[AgentIndex()].timing_ptr->nticks,MOVE_NORMAL,&move);
	}
	else {
		//_tprintf(_T("WARNING: tried to move twice during one frame!\n"));
		return 0.0;
	}

	if (move.hit == TELEPORTED)
	{
		bool teleporter = true;
	}

	if ((move.hit == HIT_JUMPABLE) && (vertforce == 0))
	{
		this->Jump();
	}

	// Don't get stuck
	if (!unsticking && (fabs(last_x - x) < MOVE_THRESHHOLD) && (fabs(last_y - y) < MOVE_THRESHHOLD))
	{
		num_stuck_frames++;
		if (num_stuck_frames > STUCK_FRAMES_THRESHHOLD)
		{ // we're stuck!
			num_stuck_frames = 0;
			unsticking = true; // Important. Not setting this may cause an infinite loop
			if (!this->UnstickSelf()) // go back to respawn
			{
				this->PlaceActor(spawn_x, spawn_y, 0, this->angle, SET_XHEIGHT, true);
			}
			unsticking = false;
		}
	}
	else
	{
		num_stuck_frames = 0;
	}

	return move.dist;
}

// Is our target just entering (forming)?
bool cAI::TargetEntering(int index)
{
	return ((index != -1) &&
		(neighbors[index]->CurrBitmap(0) >= LyraBitmap::ENTRYEXIT_EFFECT) &&
		(neighbors[index]->CurrBitmap(0) <=	(LyraBitmap::ENTRYEXIT_EFFECT + effects->EffectFrames(LyraBitmap::ENTRYEXIT_EFFECT))));
}


bool cAI::HasBeenStruck() 
{
	// If we haven't been hit from a direction,
	// assume we've been hit from behind
	return this->HasBeenStruck(0);
}

// This method is called in cMissile when an agent has been struck.
// Return whether or not the agent has been struck before (cMissile wants this info)
// but also take this opportunity to react to being hit
bool cAI::HasBeenStruck(int view)
{
	bool struck_before = has_been_struck; // Have we been hit before?
	has_been_struck = true; // We have now!

	int Angle_150 = Angle_180 - Angle_30;
	int Angle_60 = Angle_30 * 2;

	// Powerful agents may get vision when injured
	if (this->avatar.AvatarType() >= Avatars::AGOKNIGHT) 
	{
		//if (!struck_before && (rand()%15 == 0)) 
		if (rand()%15 == 0)
		{
			this->SetTimedEffect(LyraEffect::PLAYER_DETECT_INVISIBLE, 10000000, player->ID(), EffectOrigin::ART_EVOKE);
		}
		else if (this->last_target == Lyra::ID_UNKNOWN)
		// We've been hit, we have no target, and we didn't get vision
		{
			if (this->avatar.AvatarType() >= Avatars::SHAMBLIX) 
			{
				// Turn and fire!
				//this->AddAngle(Angle_150 + rand()%Angle_60);
				// Turn in the general direction that we were hit
				this->AddAngle(Angle_60*(view-3) - Angle_6 + rand()%(2*Angle_6));
				this->NightmareAttack();
				// Rampage!
				rampaging = true;
				num_rampaging_frames = 0;
			}
		}
	}
	else if (this->last_target == Lyra::ID_UNKNOWN)
	// Bog and Emphs run for cover if they can't see who hits them
	{
		this->AddAngle(Angle_60*view - Angle_6 + rand()%(2*Angle_6));
		rampaging = true;
		num_rampaging_frames = 0;
	}
	return struck_before;
}

// Given an angle, check to see if we're going to bump into a wall.
// If so, turn to avoid the wall and return our new angle.
// If not, return the input angle
int cAI::AvoidWallAngle(int angle)
{
	if (angle != this->angle)
		// If we're not moving straight ahead, don't adjust for walls
		return angle;

	const int Angle_15 = Angle_45 - Angle_30;
	move_result_t move;
		// AVOID WALLS
	for (int i=0; i<NUM_AVOID_WALL_TRIES; i++){
		this->FireViewMissile(angle,AVOID_WALL_DISTANCE,&move);		
		if (move.hit == HIT_WALL) {
			if (move.l->SINE <= 0)  
				// Wall is to our left. Turn right
				this->AddAngle(Angle_15 + rand()%Angle_45);
			else if (move.l->SINE > 0) 
				// Wall is to our right. Turn left
				this->AddAngle(-(Angle_15 + rand()%Angle_45));		
			if (rand()%10 == 0) // Sometimes turn all the way around
				this->AddAngle(Angle_180);
			angle = this->angle;
		}
		else
			break;
	}
	return angle;
}

void cAI::FireViewMissile(int angle,float distance,move_result_t *move)
{
		view_missile->SetHeightDelta(this->HeightDelta());
		view_missile->SetLaunchParams();
		MoveActor(view_missile,FixAngle(angle),distance,MOVE_CHECK,move);
}

int cAI::AcquireTarget(void)
{
	int target = -1;
	int victim = -1;
	int i =0;


	// if someone hit us, target them

	for (i =0; i < num_neighbors; i++)
	{
		if (last_attacker_id == neighbors[i]->ID())
			return i;
	}

	// see if last target is still present
	for (i =0; i < num_neighbors; i++)
	{
		victim = this->FindNearestNeighbor(i);
		if (neighbors[victim]->ID() == last_target)
		{
			if (neighbors[victim]->IsMonster())
			{
				if (rand()%2)
				{
					last_target = Lyra::ID_UNKNOWN;
					target = -1;
					break;
				}
			}

			if (this->NeighborVisible(victim))
			{
				target = victim;
				last_target_x = neighbors[target]->x;
				last_target_y = neighbors[target]->y;
				num_sightless_frames = 0;
			}
			break;
		}
	}

	// else try to find the closest visible neighbor
	if (target == -1) {
		for (int i =0; i < num_neighbors; i++)
		{
			victim = this->FindNearestNeighbor(i);
			if (this->NeighborVisible(victim) && (target == -1))
			{
				target = victim;
				last_target_x = neighbors[target]->x;
				last_target_y = neighbors[target]->y;
				num_sightless_frames = 0;
				break;
			}
		}
	}

	if ((target == -1) || (neighbors[target]->flags & ACTOR_SOULSPHERE))
		last_target = Lyra::ID_UNKNOWN;
	else
		last_target = neighbors[target]->ID();
	return target;
}

// agent has been taken down
void cAI::Dissolve(lyra_id_t origin_id, int talisman_strength)
{
	if (flags & ACTOR_SOULSPHERE)
		return;

	this->cPlayer::Dissolve(origin_id,talisman_strength);
	this->SetInjured(false);
	this->NewDeath();
//	attack_other_mares = false; //(rand()%10); // false;
	agent_info[AgentIndex()].gs_ptr->LevelLogout(RMsg_Logout::DEATH);
	this->SetReconnect(LyraTime() + 180000); // restart in 3 minute // Original Lyra Value 60000 milliseconds

	//TCHAR timebuf[128];

	cNeighbor *n = agent_info[AgentIndex()].actors_ptr->LookUpNeighbor(origin_id);
#if 0
	if (n == NO_ACTOR)
	{
	LoadString(hInstance, IDS_AGENT_DIE_TIME, message, sizeof(message));
	_tprintf(message, agent_info[AgentIndex()].name, agent_info[AgentIndex()].id, _tstrtime(timebuf));
	}
	else
	{
	LoadString(hInstance, IDS_AGENT_DIE_HANDS, message, sizeof(message));
	_tprintf(message), agent_info[AgentIndex()].name, agent_info[AgentIndex()].id, n->Name(), _tstrtime(timebuf));
	}
#endif

	return;
}

int cAI::SetXP(int value, int how, bool initializing)
{
	switch (avatar.AvatarType())
	{
		case Avatars::EMPHANT:
			orbit = 3;
			break;
		case Avatars::BOGROM:
			orbit = 10;
 			break;
		case Avatars::AGOKNIGHT:
			orbit = 20;
			break;
		case Avatars::SHAMBLIX:
			orbit = 35;
			break;
		case Avatars::HORRON:
			orbit = 50;
			break;
	}

	return 0;
}


// return the % of time in the game that neighbors have been around
// negative if currently alone, pos if currently has neighbors
unsigned char cAI::PercentBusy(void)
{
	float percent_busy = (float)(100*social_ticks/(social_ticks + alone_ticks + 1));

	if (num_neighbors)
		return (char)percent_busy;
	else
		return (char)-percent_busy;
}

void cAI::FindRespawn(GMsg_LevelPlayers& players_msg)
{
	cActor *generator=NULL;
	cActor *respawn_points[MAX_ACTORS];
	cActor *open_respawn_points[MAX_ACTORS];
	cActor *clear_respawn_points[MAX_ACTORS];
	int num_points=0;
	int nAgentIndex = AgentIndex();

	TCHAR timebuf[128];
   _tprintf(_T("Trying to find a respawn point for agent %s(%d). reconnect = %d at time %s\n"),  agent_info[nAgentIndex].name,agent_info[nAgentIndex].id, reconnect, _tstrtime(timebuf));

	this->SetAgentStats();

	// build list of possible respawn points
	//
	cActor* gen;
#ifndef BEN
	for (gen = agent_info[nAgentIndex].actors_ptr->IterateActors(INIT); 
				gen != NO_ACTOR; gen = agent_info[nAgentIndex].actors_ptr->IterateActors(NEXT))
	{
		
		if (gen->flags & ACTOR_AGENTGENERATOR)
		{
//			_tprintf(_T("Found gen of type %d; we are type %d\n"), 
//				((cOrnament*)gen)->Data(), this->AvatarType());
			if (this->AvatarType()>=Avatars::MIN_NIGHTMARE_TYPE) // Nightmares find proper respawn gen
			{
				if ((((cOrnament*)gen)->Data()) == this->AvatarType())
					respawn_points[num_points++] = gen;
			} else { // Revenant respawn at any agent respawn in level
				respawn_points[num_points++] = gen;
			}
		}
	}
	agent_info[nAgentIndex].actors_ptr->IterateActors(DONE);

	if (num_points==0)
	{
		LoadString(hInstance, IDS_NO_SPAWN_GEN, message, sizeof(message));
		_tprintf(message);
		return;
	}

	// how many open points?
	int num_open_gens = 0;
	int num_clear_gens = 0;
	for (int n = 0 ; n < num_points ; n++)
	{
		// for each generator, check to see if there are neighbors

		int sector	 = respawn_points[n]->sector;
		short room	 = agent_info[nAgentIndex].level_ptr->Sectors[sector]->room;
		int index;
		for (index=0; index<Lyra::MAX_LEVELROOMS; index++)
		{
			if (players_msg.RoomID(index) == room)
				break;
		}
		if (index == Lyra::MAX_LEVELROOMS)
			continue;

		int nPlayers = players_msg.NumPlayers(index);
		int nAgents	 = players_msg.NumAgents(index);

	_tprintf(_T("Gen scan for agent %d at time %u at sector %d, room %d has %d players and %d agents present\n"),
				 agent_info[nAgentIndex].id, LyraTime(), sector, room, nPlayers, nAgents);

		if ((nPlayers <= MAX_PLAYERS_AT_SPAWN) && (nAgents <= MAX_AGENTS_AT_SPAWN))
		{

			int mare_type = respawn_points[n]->angle;
			open_respawn_points[num_open_gens] = respawn_points[n];
			num_open_gens++;
			
			if (nAgents == 0)
			{
				clear_respawn_points[num_clear_gens] = respawn_points[n];
				num_clear_gens++;
			}
		}
	}

	//_tprintf(_T("Gen scan for agent %d has %d open out of %d gens"), agent_info[nAgentIndex].id, num_open_gens, num_points);

	if (num_open_gens==0)
	{
		LoadString(hInstance, IDS_NO_OPEN_SPAWN_GEN, message, sizeof(message));
		_tprintf(message);
		this->SetReconnect(LyraTime() + 10000);
		return;
	}

	int pos = 0;
	if (num_clear_gens > 0) // Prioritize spawning in rooms with no other agents.
	{
		pos = rand() % num_clear_gens;
		generator = clear_respawn_points[pos];
	}
	else // If agents are everywhere, spawn with a buddy.
	{
		pos = rand() % num_open_gens;  // randomize position to start from in list
		// now move to the pos'th open point
		generator = open_respawn_points[pos];
	}

	//_tprintf(_T("Agent %s(%d) respawned at the %d of %d open gen. At sector %d, room %d,  X:%d, Y:%d\n"),
	//		 agent_info[nAgentIndex].name,agent_info[nAgentIndex].id,
	//		 pos, num_open_gens,
	//		 generator->sector, agent_info[nAgentIndex].level_ptr->Sectors[generator->sector]->room,
	//		 (int)generator->x, (int)generator->y);
#else
	x = 6958;
	y = 7522;
	sector = 699;
	generator = this;
	int pos;
	int num_open_gens;
#endif
	Teleport(generator->x, generator->y, 0);
	spawn_x = generator->x;
	spawn_y = generator->y;
	LoadString(hInstance, IDS_SPAWNING_AT, message, sizeof(message));
	_tprintf(message, agent_info[nAgentIndex].name,agent_info[nAgentIndex].id,
			 pos, num_open_gens, generator->sector, agent_info[nAgentIndex].level_ptr->Sectors[generator->sector]->room,
			 (int)generator->x, (int)generator->y);
	this->ReformAvatar();
	agent_info[nAgentIndex].gs_ptr->LevelLogin();

	return;
}


int cAI::GetNeighborFacingAngle(int index)
{
	//_tprintf(_T("index: %d nn: %d\n"),index,num_neighbors);
	if (index < num_neighbors)
		return neighbors[index]->FacingAngle(x,y);
	else
		return 0;
}

// returns distance to neighbor
// should only be called from within the Update function
float cAI::NeighborDistance(int index)
{
	if ((index < num_neighbors) && (index >= 0))
	{
		if (distances_calculated) {
			return neighbor_dists[index].dist;
		}
		else {
			float myX,myY,targetX,targetY;
			myX = x; myY = y;
			targetX = neighbors[index]->x; targetY = neighbors[index]->y;
			return fdist(targetX,targetY,myX,myY);
		}
	}
	else
		return 0.0f;
}

// returns true if there is no wall between the agent and the neighbor
bool cAI::NeighborVisible(int index)
{
	if ((index < num_neighbors) && (index >= 0))
	{	// don't target ss's
		if (neighbors[index]->flags & ACTOR_SOULSPHERE)
			return false;
		// do tilting before visibility check, so test missile goes in right direction...
		float ndist = NeighborDistance(index);
		float angle_origin = 0;
		float target_z = neighbors[index]->z;
		float my_z = this->z;
		float dz = target_z - my_z;

		if (abs((int)dz) > 0.2*this->physht)
		{ // target is above or below agent -- tilt to attack!
			float radian_tilt = (float)atan(dz/ndist);
			float abs_radian_tilt;
			if (radian_tilt < 0)
				abs_radian_tilt = -radian_tilt;
			else
				abs_radian_tilt = radian_tilt;
			float tilt_angle = (float)((int)(abs_radian_tilt / .035)-1)*28;
			if (radian_tilt < 0)
				tilt_angle = -tilt_angle;
			this->vertical_tilt_float = angle_origin + tilt_angle;
			this->vertical_tilt = (long)this->vertical_tilt_float;
			//_tprintf(_T("radian tilt: %f, tilt_angle: %f\n"), radian_tilt, tilt_angle);
		}

		move_result_t move;

		this->FireViewMissile(neighbors[index]->FacingAngle(x,y),
			this->NeighborDistance(index),&move);
		return (move.hit != HIT_WALL);
	}
	else
	{
		return false;
	}
}

// returns true if the agent can hit neighbor
// For now, assume non-melee creatures can hit what they can see
bool cAI::NeighborHittable(int index)
{
	if (!this->NeighborVisible(index) || (index >= num_neighbors) || (neighbors[index]->flags & ACTOR_SOULSPHERE))
		return false;
	float target_z = neighbors[index]->z;
	float my_z = this->z;
	float dz = my_z - target_z;
	float ndist = NeighborDistance(index);
	if ((dz > 0.2*this->physht) && (ndist < MELEE_RANGE))
	{ // target is below agent
		if (this->melee_only)
			return false;
	}
	return true;
}


// returns the name for the neighbor, or NULL if invalid
TCHAR* cAI::NeighborName(int index)
{
	if (index < num_neighbors)
		return neighbors[index]->Name();
	else
		return NULL;
}

// say something; target = -3 to emote, -2 to shout, -1 to speak, <index>
// to whisper to a neighbor
void cAI::Speak(TCHAR *message, int index)
{
	if (_tcslen(message) > Lyra::MAX_SPEECHLEN-Lyra::PLAYERNAME_MAX-2)
	{
		LoadString(hInstance, IDS_WARN_MSG_TOO_LONG, disp_message, sizeof(disp_message));
		_tprintf(disp_message,message);
		return;
	}

	if (index == -1)
		agent_info[AgentIndex()].gs_ptr->Talk(message,RMsg_Speech::SPEECH,0);
	else if (index == -2)
		agent_info[AgentIndex()].gs_ptr->Talk(message,RMsg_Speech::SHOUT,0);
	else if (index == -3)
		agent_info[AgentIndex()].gs_ptr->Talk(message,RMsg_Speech::EMOTE,0);
	else if (index < num_neighbors)
		agent_info[AgentIndex()].gs_ptr->Talk(message,RMsg_Speech::WHISPER,neighbors[index]->ID());
}

// return the index for the (position-1)'th nearest neighbor,
// or -1 if we're alone or if position>=num_neighbors.
// call with position=0 to get the nearest neighbor
// should only be called from within the Update function
// distances_calculated stores whether or not the calculations have
// been made for a given frame, to avoid unnecessary processing
int cAI::FindNearestNeighbor(int position)
{
	int i;

	if ((position >= num_neighbors) || (position < 0))
		return -1;

	if (!distances_calculated)
	{
		for (i=0; i<num_neighbors; i++)
		{
			neighbor_dists[i].index = i;
			neighbor_dists[i].dist = this->NeighborDistance(i);
		}
		distances_calculated = TRUE;
	}

	qsort(neighbor_dists, num_neighbors, sizeof(neighbor_dist_t), dist_compare);

	return (neighbor_dists[position].index);
}


// Destructor
cAI::~cAI(void)
{
	if (view_missile)
		view_missile->SetTerminate();
}

#ifdef CHECK_INVARIANTS
void cAI::CheckInvariants(int line, TCHAR *file)
{

}
#endif

//////////////////////////////////////////////////////////////////
// Related functions

// qsort comparison function for neighbor distance

int __cdecl dist_compare( const void *arg1, const void *arg2 )
{
	neighbor_dist_t *dist1 = (neighbor_dist_t*)arg1;
	neighbor_dist_t *dist2 = (neighbor_dist_t*)arg2;

	if (dist1->dist < dist2->dist)
		return -1;
	else
		return 1;
}

#endif