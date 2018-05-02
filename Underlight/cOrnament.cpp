// cOrnament: Ornamment Class.

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "Central.h"
#include <math.h>
#include "cDSound.h"
#include "cGameServer.h"
#include "cOrnament.h"
#include "Options.h"
#include "cNameTag.h"
#include "Move.h"
#include "cLevel.h"
#include "Realm.h"
#include "cPlayer.h"
#include "cEffects.h"
#include "cChat.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cChat *display;
extern cGameServer *gs;
extern options_t options;
extern cPlayer *player;
extern cDSound *cDS;
extern cLevel *level;
extern cEffects *effects;
extern timing_t *timing;

//////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
// relative_z is for putting an ornament relative to a given height,
// such as for explosions

damaging_ornament_t damaging_ornaments[] = {
	{LyraBitmap::FI_SM, Stats::DREAMSOUL, -12, LyraEffect::NONE, 0 },
	{LyraBitmap::FI_MD, Stats::DREAMSOUL, -14, LyraEffect::NONE, 0},
	{LyraBitmap::FIRE, Stats::DREAMSOUL, -52, LyraEffect::NONE, 0},
	{LyraBitmap::W_HOLE, Stats::DREAMSOUL, -13, LyraEffect::PLAYER_POISONED, 10},
	{LyraBitmap::S_ALT2, Stats::DREAMSOUL, -12, LyraEffect::NONE, 0},
	{LyraBitmap::LIGHTNING, Stats::DREAMSOUL, -13, LyraEffect::PLAYER_PARALYZED, 6},
	{LyraBitmap::LIGHTNING_SM, Stats::DREAMSOUL, -12, LyraEffect::PLAYER_PARALYZED, 5 },
	{LyraBitmap::TORCH, Stats::DREAMSOUL, -14, LyraEffect::NONE, 0 },
	{LyraBitmap::ANOTHER_DAMN_TORCH, Stats::DREAMSOUL, -14, LyraEffect::NONE, 0},
};

const int NumDamagingOrnaments = sizeof(damaging_ornaments) / sizeof(damaging_ornament_t);

cOrnament::cOrnament(float x, float y, float relative_z, int angle,
					 unsigned __int64 flags, int a_id, TCHAR *name, float d_x, float d_y) :
			cActor(x, y,angle, flags, ORNAMENT), id(a_id),
				dest_x(d_x), dest_y(d_y)

{
	is_damaging_ornament = false;
	if (!this->SetBitmapInfo(id))
		return;
	if (name != NULL)
		name_tag = new cNameTag(name);

	moving = false; // true if we're moving towards a destination
	data = 0;

	if ((dest_x != 0.0f) && (dest_y != 0.0f))
		moving = true;

	switch (id)
	{	// play sounds
		case LyraBitmap::FIREBALL_EXPLODE:
			z=relative_z + 60; // cheat on height for proper placement
			cDS->PlaySound(LyraSound::EXPLODE,x,y,false);
			flags = flags | ACTOR_NOCOLLIDE;
			break;
		default:
			break;
	}
	
	for (int i = 0; i < NumDamagingOrnaments; i++)
	{
		if (damaging_ornaments[i].bitmap_id == BitmapID())
		{
			is_damaging_ornament = true;
			damage = damaging_ornaments[i];
		}
	}
}

// Update movement, etc.
// returns TRUE normally, and FALSE if the object should be deleted

bool cOrnament::Update(void)
{
	if (terminate)
		return false;

	animate_ticks += timing->nmsecs;

	if (animate_ticks >= ANIMATION_TICKS)
	{
		animate_ticks = 0;
		if (++currframe >= frames)
			switch (id)
			{	// delete missiles on exit
				case LyraBitmap::FIREBALL_EXPLODE:
				case LyraBitmap::ENTRYEXIT_EFFECT:
					return false;
				default:
					currframe=0;
			}
	}

	//_tprintf("outsider for id %d; pos = %d,%d; dest = %d,%d..\n",id,(int)x,(int)y,(int)destx,(int)desty);
	if (moving && (( x != dest_x) || (y != dest_y)))
	{
		this->ModifyHeight();
		float oldx = x; float oldy= y;
		float xdistance, ydistance, newx, newy;
		int newsector;

		if (((fabs(dest_x - x)) < (MAXWALK*timing->nticks/2)) && ((fabs(dest_y - y)) < (MAXWALK*timing->nticks/2)))
		{ // close to target, just go there to avoid jitter...
			xdistance = (dest_x - x);
			ydistance = (dest_y - y);
		}
		else // move in x/y proportional to distance from dest_x/dest_y
		{
			xdistance = (float)((dest_x - x)/((fabs(dest_x - x)) + fabs(dest_y - y))*MAXWALK*RUN_SPEED*timing->nticks);
			ydistance = (float)((dest_y - y)/((fabs(dest_x - x)) + fabs(dest_y - y))*MAXWALK*RUN_SPEED*timing->nticks);
		}


		newx = x + xdistance;
		newy = y + ydistance;

		if ((newx != oldx) || (newy != oldy)) // reset sector
		{ // move if we're not going into a wall...
			newsector = FindSector(newx, newy, sector, false);
			if (level->Sectors[newsector]->room == (int)player->Room())
			{
				sector = newsector;
				x = newx;
				y = newy;
			}
			else
				this->PlaceActor(dest_x, dest_y, z, angle, SET_XHEIGHT, true);
		}
	}

	return TRUE;
}


bool cOrnament::Render(void)
{
	return TRUE;
}

int cOrnament::CurrBitmap(int view_angle)
{
	if (id == LyraBitmap::ENTRYEXIT_EFFECT) // go backwards through entry cycle
		return (bitmap_id + effects->EffectFrames(LyraBitmap::ENTRYEXIT_EFFECT) - currframe - 1);
	else
		return (this->cActor::CurrBitmap(view_angle));
}

bool cOrnament::LeftClick(void)
{
	//if (id == 99) // clicked on Xena
	// cDS->PlaySound(LyraSound::SCREAM5);
	return false;

}

bool cOrnament::RightClick(void)
{
	LoadString (hInstance, IDS_EXAMINE_DEFAULT, disp_message, 256);
	display->DisplayMessage (disp_message, false);
	return true;
}

// Destructor

cOrnament::~cOrnament()
{
	return;
}

// Check invariants

#ifdef CHECK_INVARIANTS
void cOrnament::CheckInvariants(int line, TCHAR *file)
{

}
#endif

