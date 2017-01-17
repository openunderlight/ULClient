// cNeighbor: The Neighbor class.



// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "Central.h"
#include <math.h>
#include <limits.h>
#include "cPlayer.h"
#include "cDSound.h"
#include "cGameServer.h"
#include "cActorList.h"
#include "cParty.h"
#include "cChat.h"
#include "Realm.h"
#include "cNameTag.h"
#include "cLevel.h"
#include "cArts.h"
#include "Move.h"
#include "cControlPanel.h"
#include "Options.h"
#include "cEffects.h"
#include "Utils.h"
#include "Resource.h"
#include "cNeighbor.h"

#ifdef AGENT
#include "cAI.h"
#endif

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cPlayer *player;
extern cActorList *actors;
extern cGameServer *gs;
extern cDSound *cDS;
extern cChat *display;
extern cControlPanel *cp;
extern cArts *arts;
extern cLevel *level;
extern options_t options;
extern timing_t *timing;
extern cEffects *effects;
extern cNeighbor *test_avatar;
extern bool acceptrejectdlg;

//////////////////////////////////////////////////////////////////
// Constants

const int NEIGHBOR_WALK_ANIMATE_TICKS = 84;
const int NEIGHBOR_RUN_ANIMATE_TICKS = 56;
const int MONSTER_ANIMATE_TICKS = 200;
const float MAX_WHISPER_DISTANCE = 500000.0f;
const float MAX_WHISPER_HEIGHT = 10000.0f;


avatar_poses_t dreamer_poses[NUM_POSES] = { // start frame, end frame, increment
// start, end, cycle
	{ 14, 16, false},  // INJURED
	{ 8, 13, false}, // MELEE ATTACK
	{ 17, 17, false}, // MISSILE ATTACK
	{ 18, 18, true }, // EVOKING
	{ 23, 23, false},  // JUMP
	{ 22, 22, false},  // WAVE
	{ 0, 7, true }, // WALKING_FORWARD
	{ 19, 21, true }, // STANDING
};


avatar_poses_t mare_poses[NUM_POSES] = { // start frame, end frame, increment
	{ 12, 14, false  }, // INJURED
	{ 6, 11, false }, // MELEE ATTACK
	{ 6, 11, false }, // MISSILE ATTACK
	{ 0, 0, false	 }, // EVOKING;
	{ 0, 0, false	 }, // JUMP
	{ 0, 0, false	 }, // WAVE
	{ 0, 5, true	}, // WALKING_FORWARD
	{ 0, 0, true	}, // STANDING
};




//////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor

// Note that most of the actor parameters are included in the
// remote player info and it's position structure. Also note
// that initially all actors are created with generic finfo.

cNeighbor::cNeighbor (const RmRemotePlayer& info, unsigned __int64 flags, int n_sector)	:
					cActor (info.PeerUpdate().X(), info.PeerUpdate().Y(), 
							  info.PeerUpdate().Angle(), flags, NEIGHBOR, n_sector),
					avatar (info.Avatar())
{
	_tcscpy(name,info.PlayerName());

// z = info->position.height; // set height in parent class
	id = info.PlayerID();
	realtime_id = info.PeerUpdate().RealtimeID();
	room = info.Room();
	locked = moved = false;
	on_controlpanel = false;
  got_update = false;
	last_attack = last_move = last_hit = 0;
	visible = jumping = blasting = newbie_message = false;
	hwnd_acceptreject = NULL;
	enter_time = LyraTime();

	if (!this->SetBitmapInfo(avatar.BitmapID()))
		return;
	this->SetUpdateFlags(info.PeerUpdate());
	pose = -1; // set to -1 to force method to run
	this->SetPose(STANDING, true);
	this->SetAvatar(avatar);

	// set up icon
#ifndef AGENT
	RenderActor(this,(PIXEL *)icon,ICON_WIDTH,ICON_HEIGHT);
	name_tag = new cNameTag(name);
#endif

	halo.effect_id = LyraBitmap::AVATAR_HALO;
	halo.current_frame = halo.animate_ticks = 0;

	if ((this->ID() != player->ID()) &&
		!this->Avatar().Hidden()) {
		cDS->PlaySound(LyraSound::ENTRY,x,y,false);
	}

	entering = true;
	currframe = 0;

	this->MakeOutsider();
  
  cp->AddNeighbor(this);
	on_controlpanel = true;
  
  if (!options.network) {
    got_update = true;
	}

#ifdef UL_DEBUG
#ifndef AGENT
_stprintf(errbuf, _T("%s entered room %d in level %d, pid %d, rtid %d\n"), this->Name(),
			player->Room(), level->ID(), this->ID(), this->RealtimeID());
	INFO(errbuf);
#endif
#endif
	if ((this->ID() != Lyra::ID_UNKNOWN) && (id == player->LastLeaderID()) &&
		options.network && options.autorejoin && gs->Party())
		gs->Party()->JoinParty(this->ID(), this->Name(), true);

//#ifdef AGENT // force agent logout if two in same room
#if 0
	if (WhichMonsterName(this->Name()) &&
		(this->ID() < player->ID()))
	{
		gs->LevelLogout(RMsg_Logout::LOGOUT);
	_tprintf("Agent %d deferred to %d (%s) at %d.\n",
			player->ID(), this->ID(), this->Name(), LyraTime());
		((cAI*)(player))->SetReconnect(LyraTime() + 60000);
	}
#endif

}

// this method is called whenever a neighbor's avatar changes
void cNeighbor::SetAvatar(LmAvatar new_avatar)
{
	avatar = new_avatar;

	if (avatar.Hidden())
		flags = flags | ACTOR_NOCOLLIDE;
	else
		flags = flags & ~ACTOR_NOCOLLIDE;

	this->SetBitmapInfo(avatar.BitmapID());
	int curr_pose = pose;
	pose = -1;
	this->SetPose(curr_pose,true);
#ifndef AGENT
	RenderActor(this,(PIXEL *)icon,ICON_WIDTH,ICON_HEIGHT);
	cp->ReplaceNeighborIcon(this);
#endif

	return;
}

// set pose appropriately. if force is true, it forces into
// the pose; if force is false, it will not shift to a lower
// priority pose
void cNeighbor::SetPose(int value, bool force)
{
	 // do nothing if already in correct pose, or if we're not forcing
	// and the new pose is a lower priority
	if ((pose == value) || (!force && (pose < value)))
		return;

	pose = value;

	if (forming || dissolving || entering || (flags & ACTOR_SOULSPHERE))
		return;

	if (this->IsMonster())
		currframe = mare_poses[pose].start_frame;
	else
		currframe = dreamer_poses[pose].start_frame;

	frames_in_pose = 0;

	return;
}

// this method sets non-movement neighbor flags properly based on
// an LmPeerUpdate. Movement based flags are set only on receipt
// of position update packets in cGameServer
void cNeighbor::SetUpdateFlags(const LmPeerUpdate& update)
{
	if (update.Flags() & LmPeerUpdate::LG_INVISIBLE)
		flags = flags | ACTOR_INVISIBLE;
	else
		flags = flags & ~ACTOR_INVISIBLE;
	if (update.Flags() & LmPeerUpdate::LG_SOULSPHERE)
		flags = flags | ACTOR_SOULSPHERE;
	else
		flags = flags & ~ACTOR_SOULSPHERE;
	return;
}

int cNeighbor::TeacherID(void)
{
	if (this->IsMonster())
		return 0;
	else
		return avatar.Teacher();
}

// Called for updating neighbors; outsiders are moved to their destination
// position gradually, while local group members have their positions
// updated by peer updates.

bool cNeighbor::Update(void)
{

	int aticks, orig_frame;
	bool animate = false;
	 bool was_in_water = this->InWater();

	if (terminate)
		return false;

  if (options.network && gs && gs->LoggedIntoRoom() && gs->GotPeerUpdates() && !got_update)
  {
    got_update = true;
  }

	this->CheckMissile();

	evokingFX.Update();
	evokedFX.Update();
	this->UpdateHaloFX();

	if (sector == DEAD_SECTOR)
		return true;
	if (forming || dissolving || entering)
		aticks = ANIMATION_TICKS;
	else if (this->IsMonster())
		aticks = MONSTER_ANIMATE_TICKS;
	else if ((status == LOCAL) && ((velocity < -MAXWALK) || (velocity > MAXWALK)))
		aticks = NEIGHBOR_RUN_ANIMATE_TICKS;
	else
		aticks = NEIGHBOR_WALK_ANIMATE_TICKS;

	animate_ticks += timing->nmsecs;
	orig_frame = currframe;

	if (animate_ticks >= aticks)
	{
		animate_ticks = 0;
		animate = true;
	}


	if (forming || dissolving || entering || (flags & ACTOR_SOULSPHERE))
	{	// can't be forming and dissolving at the same time
		if (animate)
		{	//_tprintf("in entry cycle for %d; frame =%d\n",this,currframe);
			if (forming || dissolving)
			{
				currframe++;
				if (currframe >= effects->EffectFrames(LyraBitmap::FORMREFORM_EFFECT))
				{	// make them real again
					forming = dissolving = false;
					pose = -1;
					this->SetPose(STANDING, true);
				}
			}
			else if (entering && (++currframe >=  effects->EffectFrames(LyraBitmap::ENTRYEXIT_EFFECT)))
			{	// make them real again
				entering = false;
				pose = -1;
				this->SetPose(STANDING, true);
			}
			else if (!entering && (flags & ACTOR_SOULSPHERE) && (++currframe >=	effects->EffectFrames(LyraBitmap::SOULSPHERE_EFFECT)))
				currframe = 0;
		}
	}
	else if	(status == OUTSIDER)
	{
		this->ModifyHeight();
		if ((x != destx) || (y != desty))
		{
			float oldx, oldy, xdistance, ydistance, newx, newy;
			oldx = x; oldy = y;

			if (((fabs(destx - x)) < (MAXWALK*timing->nticks/2)) && ((fabs(desty - y)) < (MAXWALK*timing->nticks/2)))
			{ // close to target, just go there to avoid jitter...
				xdistance = (destx - x);
				ydistance = (desty - y);
			}
			else // move in x/y proportional to distance from destx/desty
			{
				xdistance = (float)((destx - x)/((fabs(destx - x)) + fabs(desty - y))*MAXWALK*timing->nticks);
				ydistance = (float)((desty - y)/((fabs(destx - x)) + fabs(desty - y))*MAXWALK*timing->nticks);
			}

			newx = x + xdistance;
			newy = y + ydistance;

			if ((newx != oldx) || (newy != oldy)) // reset sector
			{ // move if we're not going into a wall...
				int newsector = FindSector(newx, newy, sector, true);
				if ((newsector != DEAD_SECTOR) &&
					(level->Sectors[newsector]->room == (int)player->Room()))
				{
					sector = newsector;
					x = newx;
					y = newy;
				}
				else
					this->PlaceActor(destx, desty, z, angle, SET_XHEIGHT, true);
			}
		}

		if (animate)
		{
			this->SelectFrame();
			this->PlayFootstep();

			if ((x == destx) && (y == desty) && (pose == WALKING))
				this->SetPose(STANDING, true);
			else if ((pose == STANDING) && ((x != destx) || (y != desty)))
			{
				this->SetPose(WALKING, true);
				last_move = LyraTime();
			}


			if ((x == destx) && (y == desty))
			{
				if (pose == WALKING)
					this->SetPose(STANDING, true);
				velocity = 0.0f;
			}
			else
			{
				if (pose == STANDING)
					this->SetPose(WALKING, true);
				// set velocity +/- depending on neighbor orientation
				float d_x = destx;
				float d_y = desty;

				transform_point(x, y, angle, d_x, d_y);

				if (d_x > 0.0f)
					velocity = MAXWALK; // moving forward
				else
					velocity = -MAXWALK; // moving backward
				last_move = LyraTime();
			}
		}

		//_tprintf("outsider for id %d; pos = %d,%d; dest = %d,%d..\n",id,(int)x,(int)y,(int)destx,(int)desty);
	}
	else if (status == LOCAL)
	{
		this->ModifyHeight();
		if (animate)
		{
			this->SelectFrame();
			this->PlayFootstep();
			if ((velocity == 0) && (pose == WALKING) &&
				(LyraTime() - last_move) > STAND_INTERVAL)
				this->SetPose(STANDING, true);
			else if (velocity && (pose == STANDING))
				this->SetPose(WALKING, true);
		}

		if (velocity)
		{
			last_move = LyraTime();
			if (strafing)
				MoveActor(this,FixAngle(angle-Angle_90),velocity*timing->nticks,MOVE_NORMAL);
			else
				MoveActor(this,angle,velocity*timing->nticks,MOVE_NORMAL);
		}
	}
	float xheight = level->Sectors[sector]->FloorHt(x,y)+level->Sectors[sector]->HtOffset+physht;

	if (!was_in_water && this->InWater() && !(flags & ACTOR_SOULSPHERE)
		&& ((z > xheight - (.1*physht)) && (z < xheight + physht)))

		//&& ((z > xheight - (.1*physht)) && (z < xheight + (.1*physht))))
	{

			if (!((this->Avatar().Hidden()) && (player->ID() != this->ID())))
				cDS->PlaySound(LyraSound::ENTER_WATER,x,y,false);
	}

	//_tprintf("status for %s is %d; pose = %d; frame = %d\n",name, status, pose, currframe);

	return true;
}

void cNeighbor::UpdateHaloFX(void)
{
	halo.animate_ticks += timing->nmsecs;
	if (halo.animate_ticks >= ANIMATION_TICKS)
	{
		halo.animate_ticks = 0;
		if (++halo.current_frame >= effects->EffectFrames(halo.effect_id))
			halo.current_frame = 0;
	}
}


// determine the next frame based on current pose

void cNeighbor::SelectFrame(void)
{
	avatar_poses_t *pose_array;

	if (forming || dissolving || entering ||
		(flags & ACTOR_SOULSPHERE) || (this == test_avatar))
		return;

	if (this->IsMonster())
		pose_array = mare_poses;
	else
		pose_array = dreamer_poses;

	frames_in_pose++;

	//_stprintf(message, "pose: %d cf: %d\n", pose, currframe);
	//display->DisplayMessage(message);

	switch (pose)
	{
		case WALKING:
			if (velocity > 0)
			{
				if (++currframe > pose_array[pose].end_frame)
					currframe = pose_array[pose].start_frame;
			}
			else if (velocity < 0)
			{
				if (--currframe < pose_array[pose].start_frame)
					currframe = pose_array[pose].end_frame;
			}
			break;

		case STANDING:
			if (frames_in_pose > (100 + (rand()%100)))
			{ // switch to new stand every 100 frames or so
				frames_in_pose = 0;
				if (++currframe > pose_array[pose].end_frame)
					currframe = pose_array[pose].start_frame;
			}
			break;

		case MELEE_ATTACK:
			if (++currframe > pose_array[pose].end_frame)
				this->SetPose(STANDING, true);
			break;

		case MISSILE_ATTACK:
			if (++frames_in_pose > 2)
				this->SetPose(STANDING, true);
			break;

		case EVOKING: // cycle
			if (!evoking)
				this->SetPose(STANDING, true);
			else if (!this->IsMonster())
			{ // disable extra evoking frame
				//if (++currframe > pose_array[pose].end_frame)
					currframe = pose_array[pose].start_frame;
			}
			else
				this->SetPose(STANDING, true);
			break;

		case INJURED:
			if (++currframe > pose_array[pose].end_frame)
				this->SetPose(STANDING, true);
			break;

		case WAVE:
			if (!waving)
				this->SetPose(STANDING, true);
			break;

		case JUMP:
			if (!jumping)
				this->SetPose(STANDING, true);
			break;

		default:
			break;
	}
}

void cNeighbor::PlayFootstep(void)
{
	if ((pose != WALKING) || !options.footsteps ||
		(!strafing && (velocity < MAXWALK) && (velocity > -MAXWALK)) ||
		(strafing && (velocity < MAXSTRAFE) && (velocity > -MAXSTRAFE)) ||
		(flags & ACTOR_SOULSPHERE) || this->IsMonster() ||
		(z != (level->Sectors[sector]->FloorHt(x,y)+level->Sectors[sector]->HtOffset+physht)))
		return; // no footstep

	// dreamers have 8 frames for walking; play footsteps on 2,6
	int step1_frame = 6;
	int step2_frame = 2;
	if (this->Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) 
	{
		step1_frame = 4;
		step2_frame = 1;
	}

	if (!((this->Avatar().Hidden()) && (player->ID() != this->ID())))
	{
		if (currframe == step1_frame)
		{
			if (this->InWater())
				cDS->PlaySound(LyraSound::WATER_STEP_1, x, y, false);
			else
				cDS->PlaySound(LyraSound::OTHER_STEP_1, x, y, false);
		}
		else if (currframe == step2_frame)
		{
			if (this->InWater())
				cDS->PlaySound(LyraSound::WATER_STEP_2, x, y, false);
			else
				cDS->PlaySound(LyraSound::OTHER_STEP_2, x, y, false);
		}
	}
}


void cNeighbor::MakeLocal(void)
{
	if (status == LOCAL)
	{
		return; // do nothing if repeat

	}
	status = LOCAL;
	velocity = 0.0f;

}

// turn this person into an outsider
void cNeighbor::MakeOutsider(void)
{
	if (status == OUTSIDER)
		return;

	status = OUTSIDER;
	destx = x;
	desty = y;
	velocity = 0.0f;

}

// do nothing on left click
bool cNeighbor::LeftClick(void)
{
	return false;
}

// identify neighbor
bool cNeighbor::RightClick(void)
{
	LoadString (hInstance, IDS_WHO_IS_THAT, disp_message, sizeof(disp_message));
#if defined PMARE
	if (IsMonster())
	_stprintf(message, disp_message,name);
	else
		if (IsMale())
		_stprintf(message,disp_message,"a male dreamer");
		else if (IsFemale())
		_stprintf(message,disp_message,"a female dreamer");
		else // Dreamers in nightmare form are neither male or female
		_stprintf(message,disp_message,"a Dreamer");
#else
	
	// can only see the focus of a normal dreamer
	if (this->SphereID() < 1)
		_stprintf(message, "That's %s (%s)", name, player->StatName(this->Avatar().Focus()));
	else
		_stprintf(message, disp_message,name);
#endif
	display->DisplayMessage(message, false);

	return true;
}

// called to cause the neighbor to fade in from a soulsphere.
void cNeighbor::Form(void)
{
	if (forming || (flags & ACTOR_SOULSPHERE))
		return;
	if (!dissolving)
		currframe=0;
	forming = true;
	dissolving = entering = false;
	return;
}

// called to cause the neighbor to dissolve into a soulsphere
void cNeighbor::Dissolve(void)
{
	if (dissolving || !(flags & ACTOR_SOULSPHERE))
		return;
	if (!forming)
		currframe=0;
	forming = entering = false;
	dissolving = true;
	return;
}

bool cNeighbor::Render(void)
{
	if (id == player->ID()) // don't render dummy neighbor
		return false;
	else if (room != player->Room())
		return false;
	else
		return (this->cActor::Render());
}


// returns true if we're a Female; based on avatar type
bool cNeighbor::IsFemale (void)
{
	if (avatar.AvatarType() == Avatars::FEMALE)
		return true;
	else
		return false;
}


// returns true if we're a Mmale; based on avatar type
bool cNeighbor::IsMale (void)
{
	if (avatar.AvatarType() == Avatars::MALE)
		return true;
	else
		return false;
}

// returns avatar type
unsigned int cNeighbor::GetAvatarType (void)
{
	return avatar.AvatarType();
}

// returns avatar name
TCHAR* cNeighbor::Name (void)
{
#ifdef PMARE
		  if ((this->GetAccountType() == LmAvatar::ACCT_DREAMER) || (this->GetAccountType() == LmAvatar::ACCT_ADMIN))
			  if (this->IsMale())
			  {
				  LoadString(hInstance, IDS_MALE_DREAMER , generic_name, sizeof(generic_name));
				  return generic_name;
			  }
			  else if (this->IsFemale())
			  {
				  LoadString(hInstance, IDS_FEMALE_DREAMER , generic_name, sizeof(generic_name));
				  return generic_name;
			  }
			  else
			  {
				  LoadString(hInstance, IDS_UNKNOWN_DREAMER , generic_name, sizeof(generic_name));
				  return generic_name;
			  }
			else
				return name;
#else
	if (avatar.Hidden())
	{
	  LoadString(hInstance, IDS_UNKNOWN_DREAMER , generic_name, sizeof(generic_name));
	  return generic_name;
	}
	else
		return name; 
#endif
};

// returns account type
unsigned int cNeighbor::GetAccountType (void)
{
	return avatar.AccountType();
}

// returns true if we're a monster; based on avatar type
unsigned int cNeighbor::GetTransformedMonsterType (void)
{
	return 0;
}

// returns true if we're a monster; based on avatar type
bool cNeighbor::IsMonster (void)
{
	if (this->GetAvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
		return true;
	else
		return false;
}

// returns true if we're an agent mare; based on account type
bool cNeighbor::IsAgentAccount (void)
{
	if (this->GetAccountType() == LmAvatar::ACCT_NIGHTMARE)
		return true;
	else
		return false;
}

// returns true if we're a dreamer; based on account type
bool cNeighbor::IsDreamerAccount (void)
{
	if ((this->GetAccountType() == LmAvatar::ACCT_DREAMER) ||
		(this->GetAccountType() == LmAvatar::ACCT_ADMIN))
		return true;
	else
		return false;
}

// ToDo: Move this to cActor and make it more generalized to any two actors
bool cNeighbor::IsVulnerable (void)
{
	// must be visible to site of vulnerability to effect blast
	bool fIsVulnerable = false;
	if (this->Visible())
	{
		// determine angle of attack
		int view = FixAngle((this->angle - player->angle)+32)/(Angle_360/Avatars::VIEWS);
		switch (this->GetAvatarType())
		{
			case Avatars::MALE:
			case Avatars::FEMALE:
			case Avatars::EMPHANT:
			case Avatars::BOGROM:
					fIsVulnerable = true;
				break;
			case Avatars::AGOKNIGHT: // only hit from front
				if (view == 3)
					fIsVulnerable = true;
				break;
			case Avatars::SHAMBLIX: // only hit from rear
				if (view == 0 || view == 1 || view == 5)
					fIsVulnerable = true;
				break;
			case Avatars::HORRON: // only hit from directly behind
				if (view == 0)
					fIsVulnerable = true;
				break;
		}
	}
	return fIsVulnerable;
}


void cNeighbor::Unlock(void)
{
	locked = FALSE;
	if (level->Sectors[sector]->room != level->Sectors[player->sector]->room)
		this->SetTerminate();
	if (room != player->Room())
		this->SetTerminate();
}

bool cNeighbor::CanWhisper(void)
{
	if (gs->Party()->IsInParty(id))
		return true;
#ifndef GAMEMASTER // GMs can whisper any distance
	float dx = (player->x - x);
	float dy = (player->y - y);
	float dz = (player->z - player->physht - z);

	if ((dx*dx + dy*dy > MAX_WHISPER_DISTANCE) ||
		 ((player->z - player->physht - z) > MAX_WHISPER_HEIGHT) ||
		((z - player->z) > MAX_WHISPER_HEIGHT))
		 return FALSE;
	else
#endif //GM
		return TRUE;
}


// Destructor

cNeighbor::~cNeighbor(void)
{
	if (acceptrejectdlg && hwnd_acceptreject)
	{
		PostMessage(hwnd_acceptreject, WM_COMMAND, (WPARAM) IDC_REJECT, 0);
		LoadString (hInstance, IDS_AUTO_REJECT, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Name());
		display->DisplayMessage(message, false);
	}

	// cancel art if dummy neighbor is deleted
#ifndef AGENT
	if (player && (id == player->ID()) && arts->Waiting())
		arts->CancelArt();
	if (gs && gs->Party() && gs->Party()->IsInParty(id))
		gs->Party()->MemberExit(id);
	if (on_controlpanel)
		cp->DeleteNeighbor(this);
	if (cp->SelectedNeighbor() == this)
		cp->SetSelectedNeighbor(NO_ACTOR);
#endif

	return;
}

void cNeighbor::SetRoom(short value) 
{ 
	if (value == room)
		return;

	room = value; 

	// if we're not in the player's room, don't collide
	if (room <= 0) {
		flags = flags | ACTOR_NOCOLLIDE;
		x = DEAD_X;
		y = DEAD_Y;
		z = DEAD_Z;
	}
	else {
		flags = flags & ~ACTOR_NOCOLLIDE;
		this->SetXHeight();		
		//entering = true;
	}	
}


// Check invariants

#ifdef CHECK_INVARIANTS
void cNeighbor::CheckInvariants(int line, TCHAR *file)
{

}
#endif

