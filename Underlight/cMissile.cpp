 // cMissile: The missile class.

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "Central.h"
#include "cDSound.h"
#include "4dx.h"
#include "Move.h"
#include "Dialogs.h"
#include "cNeighbor.h" 
#include "Utils.h"
#include "cActorList.h"
#include "cChat.h"
#include "cItem.h"
#include "cLevel.h"
#include "cOrnament.h"
#include "cPlayer.h"
#include "cEffects.h"
#include "cSending.h"
#include "Realm.h"
#include "Utils.h"
#include "cMissile.h"
#include "resource.h"
#ifdef AGENT
#include "cAI.h"
#endif

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cDSound *cDS;  
extern cPlayer *player;
extern cActorList *actors; 
extern cChat *display;
extern cLevel *level;
extern cArts *arts;
extern timing_t *timing;
extern cEffects *effects;
extern cTimedEffects *timed_effects;

//////////////////////////////////////////////////////////////////
// Constants

const int FIREBALL_MAX_TICKS=1200;
const int RETURNING_TICKS=3000;
const int RETURNING_MAX_TICKS=5500;
const int EXPLOSION_FRAME_TICKS=50;
const float PUSH_DISTANCE = 75.0f;
const float LAUNCH_OFFSET= 6.0f;
const float COLLIDE_OFFSET = 32.0f;
const float MELEE_RANGE = 140.0f;
const int COMBAT_SKILL_CHANCE_INCREASE = 8;

//////////////////////////////////////////////////////////////////
// Class Defintion


// Constructor

cMissile::cMissile(cActor *m_owner, int m_bitmap_id, int angle, int m_height_delta, 
				int m_velocity, int m_effect, int m_damage_type, cItem *m_counterpart, 
				unsigned __int64 flags, int m_sector, int m_art_id) : 
			cActor(m_owner->x, m_owner->y,
				   angle, flags, MISSILE, m_sector),
				   height_delta(m_height_delta), bitmap_id(m_bitmap_id), 
				   owner(m_owner), counterpart(m_counterpart),
				   velocity(m_velocity),  effect(m_effect), 
				   damage_type(m_damage_type),
				   art_id(m_art_id)
{ 
	if (!this->SetBitmapInfo(bitmap_id))
	{
		this->SetTerminate();
		return;
	}

	switch (bitmap_id)
	{ 	
		case LyraBitmap::PUSH_MISSILE:
			if (player->Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
				invisible = false;
			else if ((m_owner->IsNeighbor()) && (((cNeighbor*)m_owner)->IsMonster()))
				invisible = false;
			else
				invisible = true;
			break;
		case LyraBitmap::FIREBALL_MISSILE: 
		case LyraBitmap::DREAMBLADE_MISSILE:
//		case LyraBitmap::MARE_MELEE_MISSILE:
			invisible = false;
			break;
		default:
			break;
	}


	colors[0] = CalculateModifierMax(damage_type)/8;
	if (colors[0] > 7)
		colors[0] = 7;
	colors[1] = CalculateModifierMin(damage_type)/8;
	if (colors[1] > 7)
		colors[1] = 7;

	// neg velocity means bounce; multiply velocity by 5 to get true velocity
	velocity = velocity * 8;
	bouncing = returning = activated = melee = false;

	if (velocity < 0)
	{
		velocity = -velocity;
		bouncing = true;
	} 
	else if (velocity == MELEE_VELOCITY) // constant = 0
		melee = true; // velocity parameter ignored if melee

	ticks = state = 0;

	if (owner->IsPlayer())
		owner_id = player->ID();
	else if (owner->IsNeighbor())
		owner_id = ((cNeighbor*)owner)->ID();
	else
		owner_id = Lyra::ID_UNKNOWN;

	//_tprintf("spawned missile of type %d; owner = %d, player = %d\n",missile_type,owner,player); 

	return;
}

void cMissile::SetLaunchParams(void)
{
	this->PlaceActor(owner->x + LAUNCH_OFFSET*CosTable[angle], 
					 owner->y + LAUNCH_OFFSET*SinTable[angle],
					 0, owner->angle, SET_XHEIGHT, false);
	z = owner->eyeheight;

	switch (bitmap_id)
	{ 	
		case LyraBitmap::FIREBALL_MISSILE: 
			expire_ticks = FIREBALL_MAX_TICKS;
			cDS->PlaySound(LyraSound::LAUNCH, owner->x, owner->y, false);
			break;
		case LyraBitmap::PUSH_MISSILE:
		case LyraBitmap::DREAMBLADE_MISSILE:
//		case LyraBitmap::MARE_MELEE_MISSILE:
			melee = true;
			break;
		default:
			break;
	}

	expired = false;

}


void cMissile::Activate(void)
{
	activated = true;
	if (!actors->ValidActor(owner))
		expired = true;
	else // set to launch
		this->SetLaunchParams();
	this->Update();
}

bool cMissile::Update(void)
{
   bool animate = false;

 // _tprintf("doing missile update; at %d,%d\n",(int)x,(int)y);
	if (terminate)
		return false;
	else if (!activated)
	   return true;

	ticks += timing->nmsecs;
	z += timing->nticks*velocity*height_delta*0.085f;
	
	if (z > level->Sectors[this->sector]->CeilHt(x,y))
		this->Collision(HIT_CEILING);
	else if ((z - physht) < level->Sectors[this->sector]->FloorHt(x,y))
		this->Collision(HIT_FLOOR);

	if (returning && (ticks >= RETURNING_TICKS) && (state == 0))
	{	// return to owner
		state = 1;
		if (actors->ValidActor(owner)) 
			angle = owner->FacingAngle(x,y);
		else // owner no longer around - end of the missile
			expired = true;
	}

	if (melee && !expired) // melee weapons get just one attack
		MoveActor(this,angle,MELEE_RANGE,MOVE_NORMAL);
	else if (!expired)
		MoveActor(this,angle,velocity*timing->nticks,MOVE_NORMAL);

	if (melee || (ticks >= expire_ticks)) // expire the missile
		expired = true;

	if (expired)
	{
		if (returning && actors->ValidItem(counterpart) && (counterpart->Thrown()))		
			counterpart->SetThrown(false);

		switch (bitmap_id)
		{  // explode/delete missile
			case LyraBitmap::FIREBALL_MISSILE:
			{
				cOrnament *o = new cOrnament(x,y,z,	angle, 
					ACTOR_NOCOLLIDE, LyraBitmap::FIREBALL_EXPLODE);
				o->SetColor(0, colors[0]);
				o->SetColor(1, colors[1]);
			}
				break;
			case LyraBitmap::PUSH_MISSILE:
				break;
		}
		return false;
    }

	//_tprintf("finished missile update for %d\n",this);
	return true;            
}

// cause the missile to strike the actor, damaging, etc.
void cMissile::StrikeActor(cActor* actor)
{
	int damage;

	if ( actor->IsNeighbor() && (owner == player) && (art_id != Arts::NONE))
	{
		arts->IncreaseSkill(art_id,COMBAT_SKILL_CHANCE_INCREASE);
	}

#if defined (UL_DEBUG) && !defined (LIVE_DEBUG)
	//  Allow bouncing charms to hit owner in debugging
	if (actor->IsPlayer() /* && (owner != player) */ && effect) {
		//if ((effect == LyraEffect::PLAYER_POISONED) && (player->poison_strength < MinModifierSkill(damage_type))) {
		//		player->poison_strength = MinModifierSkill(damage_type);
		//}
		player->SetTimedEffect(effect, CalculateDuration(timed_effects->default_duration[effect]), owner_id, EffectOrigin::MISSILE);
	}
#else
	if (actor->IsPlayer() && (owner != player)  && effect) {
		//if ((effect == LyraEffect::PLAYER_POISONED) && (player->poison_strength < MinModifierSkill(damage_type))) {
		//		player->poison_strength = MinModifierSkill(damage_type);
		//}
		player->SetTimedEffect(effect, CalculateDuration(timed_effects->default_duration[effect]), owner_id, EffectOrigin::MISSILE);
	}
#endif
	if ((actor->IsPlayer()) && (owner == player) && returning)
	{  // forcibly expire
		if (actors->ValidItem(counterpart) && counterpart->Thrown())		
			counterpart->SetThrown(false);
		expired = true;
		return;
	}

	// handle special effects based on bitmap type
	switch (bitmap_id)
	{
		case LyraBitmap::PUSH_MISSILE: // push actor along angle of missile
			
			if ((owner->IsNeighbor()) && (((cNeighbor*)owner)->IsMonster())
				|| (owner->IsPlayer() && (cPlayer*)owner->IsMonster()))
				break;

			if (actor->IsOrnament())
				break;

			MoveActor(actor,angle,PUSH_DISTANCE,MOVE_NORMAL);
			if (actor->IsNeighbor())
				((cNeighbor*)actor)->SetMoved(true);
			else if (actor->IsPlayer())
			{
				LoadString (hInstance, IDS_PLAYER_GOTPUSHED, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
				player->PerformedAction();
			}
			break;
	}

	if (damage_type && (owner != actor))
	{
		damage = CalculateModifier(damage_type);
		if (owner->IsNeighbor())
		{
			int extra_dmg = ((cNeighbor*)owner)->Avatar().ExtraDamage();
			
			if (player->flags & ACTOR_CRIPPLE && player->cripple_strength > 0)
			{
				// Increase damage by 3/4 of the cripple strength
				int new_damage = (int)(round(damage+(.75 * player->cripple_strength)));

#ifdef UL_DEV
				if (new_damage != damage) {
					_stprintf(temp_message, "Initial damage of %d but %d was applied due to being tiny tim", damage, new_damage);
					display->DisplayMessage(temp_message);
				}
#endif
				damage = new_damage;
			}

			if (extra_dmg > 0) {
				if (extra_dmg > 9) extra_dmg = 9;
#ifdef UL_DEV
				_stprintf(message, "An extra %d damage is being applied due to %s's damage bonus", extra_dmg, ((cNeighbor*)owner)->Name());
				display->DisplayMessage(message);
#endif
				damage += extra_dmg;
			}
		}
	}
	else
		damage = 0; 

	unsigned int maretype = player->Avatar().AvatarType();
#ifdef AGENT
	if (player->Avatar().AvatarType() < Avatars::MIN_NIGHTMARE_TYPE)
	{ // Revenant borrow shielding from the nightmare agent they replace based on agent username e.g. Shamblix_14=Shamblix
		int pi;
		TCHAR marename[Lyra::PLAYERNAME_MAX];
		// *** STRING LITERAL ***  
		if (_stscanf(agent_info[AgentIndex()].name, "%[^_]_%d", marename, &pi) != 2) {
			// couldn't parse it
			_tcsnccpy(marename, agent_info[AgentIndex()].name, sizeof(marename));
		}
		maretype = WhichMonsterName(marename);
}
#endif //AGENT

    // certain agents can only be attacked from some angles
	if (maretype >= Avatars::MIN_NIGHTMARE_TYPE)
	{
		if (!actors->ValidActor(owner))
			damage = 0;
		else
		{ // determine angle of attack
			int view = FixAngle((owner->angle - player->angle)+32)/(Angle_360/Avatars::VIEWS);
// 0 - back
// 1 - back left
// 2 - front left
// 3 - front
// 4 - front right
// 5 - back right

#ifdef PMARE
			// flip a pmare if they've been hit in the face while evoking
			if (maretype > Avatars::AGOKNIGHT && view >= 2 && view <= 4 && arts->CurrentArt() != Arts::NONE)
				player->angle = FixAngle(player->angle + Angle_180);
#endif

			switch (maretype)
			{
				case Avatars::EMPHANT:
				case Avatars::BOGROM: 
					break;
				case Avatars::AGOKNIGHT: // only hit from front
					if (view != 3)
#ifdef PMARE
						// pmares don't get invulnerability, only 70% shield
						damage = damage*.30;
#else
						damage = 0;
#endif
					break;
				case Avatars::SHAMBLIX: // only hit from rear
					if ((view > 1) && (view < 5))
#ifdef PMARE
						// pmares don't get invulnerability, only 75% shield
						damage = damage*.25;
#else
						damage = 0;
#endif
					break;
				case Avatars::HORRON: 
					// side back shots to a horron do 50% damage
					if (view == 1 || view == 5)
						damage = damage*.50;
					else if (view != 0) // only hit from directly behind for full damage
#ifdef PMARE
						// pmares don't get invulnerability, only 80% shield
						damage = damage*.20;
#else
						damage = 0;
#endif
					break;
			}
#ifdef AGENT // Inform agents when they've been struck
			if (damage > 0)
				((cAI*)player)->HasBeenStruck(view);
#endif
		}
#ifdef AGENT
		if (actors->ValidNeighbor(this->owner) && 
			(damage > 0) && (rand()%3 == 0)) // change attackers!
		{
			((cAI*)player)->SetNextTargetID(((cNeighbor*)owner)->ID());
		}
#endif

	}

	// play appropriate scream sound
	if ((actor != NULL) && damage)
	{ // play scream sound for neighbors
		if (actor->IsPlayer() && !(player->flags & ACTOR_SOULSPHERE))
		{
			int avatar_type = player->Avatar().AvatarType();
			if (player->flags & ACTOR_TRANSFORMED)
				avatar_type = player->GetTransformedMonsterType();
			Scream(avatar_type, actor, false);

			if (actors->ValidNeighbor(owner) && (owner->Type() == NEIGHBOR))
			{
			if (damage>0)
				{
					player->SetLastAttackerID(((cNeighbor*)owner)->ID());
					LoadString (hInstance, IDS_GOT_THWACKED, disp_message, sizeof(disp_message));
					static UINT thwackings[20] = 
					{	
						IDS_JOSTLED,
						IDS_SMACKED,
						IDS_HIT,
						IDS_KNOCKED, 
						IDS_THWACKED,
						IDS_SHOWN_THING_OR_TWO,
						IDS_WHACKED, 
						IDS_SMASHED, 
						IDS_LICKIN,
						IDS_HAVOC,
						IDS_BEAT_DOWN,
						IDS_FRACTURED,
						IDS_BASHED, 
						IDS_POUNDED,
						IDS_CRUSHED, 
						IDS_WAILED, 
						IDS_THRASHED, 
						IDS_SHATTERED, 
						IDS_DESTRUCTION
					};
					if (damage>94) damage = 94;
					int thwack_index = (damage/5);
					LoadString (hInstance, thwackings[thwack_index], temp_message, sizeof(temp_message));
					_stprintf(message, disp_message, ((cNeighbor*)owner)->Name(), temp_message);
					display->DisplayMessage (message);
				}
			}
			// Cancel the guildhouse evoke if the player is struck by a missile
			if (arts->CurrentArt() == Arts::GUILDHOUSE)
				arts->CancelArt();

			player->SetInjured(true);
			player->SetCurrStat(Stats::DREAMSOUL, -damage, SET_RELATIVE, owner_id);
			//_tprintf("took %d damage!\n",damage);
		}
		else if (actor->IsSending())
		{
			((cSending*)actor)->ChangeHealth(-damage);
			cDS->PlaySound(LyraSound::EMPHANT_ROAR,actor->x,actor->y,false);
		}
	}
	return;
}

// Missile has struck this line; calls strikeactor if needed.
// L is the linedef for the actor or wall that was struck; l = NULL
// if a floor or ceiling was struck. 
// Returns false if the missile has expired due to the strike, or
// returns true if the missile lives on.
bool cMissile::Collision(int collision_type, linedef *l)
{
	if (bouncing)
	{
		switch (collision_type)
		{	
			case HIT_ACTOR:
				if (!returning)
					break; // expire, explode on actor strike if not returning
				this->StrikeActor(l->actor); // else fall through
			case HIT_WALL: // set angle for proper bounce
		 	  	angle = FixAngle(angle + 2*FindIntersectAngle(this,l));
				sector = FindSector(x, y, sector, false);
				return true;
			case HIT_FLOOR:
			case HIT_CEILING: // reverse height delta
				height_delta = -height_delta;
				sector = FindSector(x, y, sector, false);
				return true;
		}
	}
	// mark as expired & back off a bit so effects render properly
	expired = true;
	switch (collision_type)
	{	
		case HIT_ACTOR:
			if (l->actor->IsPlayer())
			{  // put in front of player
				x = player->x + CosTable[player->angle]*COLLIDE_OFFSET;
				y = player->y + SinTable[player->angle]*COLLIDE_OFFSET;
			}
			else
			{	// back off a bit to render between actor and player
				x+= CosTable[FixAngle(GetFacingAngle(player->x, player->y, l->actor->x, l->actor->y))]*COLLIDE_OFFSET;
				y+= SinTable[FixAngle(GetFacingAngle(player->x, player->y, l->actor->x, l->actor->y))]*COLLIDE_OFFSET;
			}
			// need to strike actor AFTER x/y adjustment so explosions
			// and other ornaments appear correctly
			this->StrikeActor(l->actor); // fall thru
			break;
		case HIT_WALL: // set angle for proper bounce
			x += CosTable[FixAngle(angle + Angle_180)]*COLLIDE_OFFSET;
			y += SinTable[FixAngle(angle + Angle_180)]*COLLIDE_OFFSET;
			break;
		case HIT_FLOOR:
			z = level->Sectors[this->sector]->FloorHt(x,y) + physht;
			break;
		case HIT_CEILING:
			z = level->Sectors[this->sector]->CeilHt(x,y) - 2*physht;
			break;
	}
	sector = FindSector(x, y, sector, false);
	return false;
}


bool cMissile::Render(void)
{
	if (!activated)
		return false;
	else if (bitmap_id == LyraBitmap::PUSH_MISSILE)
		return false;
	else
		return (this->cActor::Render());
}

// Destructor

cMissile::~cMissile(void)
{
	//_tprintf("deleting missile \n"); 
	return;
}


// Check invariants

#ifdef CHECK_INVARIANTS
void cMissile::CheckInvariants(int line, TCHAR *file)
{

}
#endif


//////////////////////////////////////////////////////////////
// Helper Functions

// Returns true if we can launch a missile from this position.
// Basically, if the launch offset is not in the DEAD_SECTOR
// and is in a legal room then we're OK.

bool LegalMissilePosition(float x, float y, int angle, int sector)
{
	int newsector = FindSector(x + 4*LAUNCH_OFFSET*CosTable[angle],
						y + 4*LAUNCH_OFFSET*SinTable[angle], sector, false);

	return (LegalRoom(level->Sectors[newsector]->room));

}



