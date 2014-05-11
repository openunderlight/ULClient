// cSending: The local monster (sending) class.

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "Central.h"
#include "4dx.h"
#include "cDSound.h"
#include "cChat.h"
#include "cPlayer.h"
#include "cItem.h"
#include "Realm.h"
#include "cLevel.h"
#include "Move.h"
#include "cMissile.h"
#include "cEffects.h"
#include "cOrnament.h"
#include "cSending.h"
#include "resource.h"

////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cPlayer *player;
extern cDSound *cDS;
extern cChat *display;
extern cEffects *effects;
extern timing_t *timing;
extern cLevel *level;

/////////////////////////////////////////////////////////////////
// Class Defintion


// Constructor
cSending::cSending(float s_x, float s_y, int s_angle, int avatar_type, unsigned __int64 flags) :
	cActor(s_x, s_y, s_angle, flags, SENDING),
		avatar(avatar_type)

{
	// avatar id's are reserved as the low numbered effect id's
	if (!this->SetBitmapInfo(avatar))
		return;

	health = 15;
	last_shot = 0;

	return;
}


// Called for updating sendings.
// The AI for local monsters goes here.


bool cSending::Update(void)
{
	float dx,dy;

	bool animate = FALSE;

	if (terminate)
		return false;

	this->ModifyHeight();

	angle = player->FacingAngle(x,y); // face player

	if (health <= 0)
	{
		new cOrnament(x, y, 0, angle, ACTOR_NOCOLLIDE, LyraBitmap::ENTRYEXIT_EFFECT);
		return FALSE; // we're dead...
	}

	if (level->Sectors[sector]->room != (int)player->Room())
	{
// _tprintf("in wrong room; player r = %d, sec = %d, us r = %d sec =%d...\n",player->Room(),player->sector,sector,level->Sectors[sector]->room);
		return TRUE;
	}

	dx = player->x - x;
	dy = player->y - y;

	return TRUE;
}

// change health and check for dead...
void cSending::ChangeHealth(int healthchange)
{
	health += healthchange;
	return;
}


bool cSending::Render(void)
{
	return TRUE;

}


bool cSending::LeftClick(void)
{
	LoadString (hInstance, IDS_MONSTER_ANGRY, disp_message, 256);
	display->DisplayMessage (disp_message);

	return true;
}

// identify neighbor
bool cSending::RightClick(void)
{
	LoadString (hInstance, IDS_MONSTER_ANGRY, disp_message, 256);
	display->DisplayMessage (disp_message);

	return true;
}


// Destructor
cSending::~cSending(void)
{
	return;
}


// Check invariants

#ifdef DEBUG

void cSending::Debug_CheckInvariants(int caller)
{
	return;

}

#endif
