// cActor: The base actor class.

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "4dx.h"
#include "cActorList.h"
#include "Utils.h"
#include "cEffects.h"
#include "cLevel.h"
#include "cNameTag.h"
#include "cActor.h"
#include "cChat.h"
#include "Realm.h"
#include "cDSound.h"
#include "cControlPanel.h"
#include "cGameServer.h"
#include "Move.h"
#include "resource.h"
#include "cPlayer.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern cActorList *actors;
extern actor_sector_map_t	OList [MAX_ACTORS];
extern int NumOList;
extern cDSound *cDS;
extern cPlayer *player;
extern cChat *display;
extern cControlPanel *cp;
extern timing_t *timing;
extern cEffects *effects;
extern cLevel *level;
extern cGameServer *gs;
extern HINSTANCE hInstance;
extern bool leveleditor;
extern bool exiting;

const int PLAYER_MISSILE_LAUNCH_DELAY = 200;
const int OTHER_MISSILE_LAUNCH_DELAY = 10;
const int JUMP_FOOTSTEP = 100;
const int MIN_JUMP_HEIGHT = 96;
const int TEN_FEET = 256;
const int MAX_JUMP_HEIGHT = TEN_FEET;
const float NEIGHBOR_WIDTH_CHEAT = 20.0f;

//////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor

cActor::cActor(float act_x, float act_y, int act_angle,
				unsigned __int64 act_flags, int act_type, int act_sector) : x(act_x),
				y(act_y), angle(act_angle),  sector(act_sector),
				flags(act_flags), type(act_type)
{

	//_tprintf("Entering actor constructor, for type %d\n",type);
	velocity = 0.0f;
	vertforce = 0.0f;

	l = new linedef;
	palette = 0;
	l->actor = this;
	animate_ticks = 0;
	num_color_regions = 0;
	bitmap_id = NO_BITMAP;
	name_tag = NULL;
	colorized = terminate = false;
	forming = dissolving = entering = false;

	missile_launch_time = 0;
	missile = NULL;

	for (int i=0; i<MAX_COLORED_REGIONS; i++)
		colors[i]=0;

	this->PlaceActor(x, y, 0, angle, SET_XHEIGHT, true);

	if (!(actors->InsertActor(this)))
	{
		LoadString(hInstance, IDS_ACTOR_NO_ADD, message, sizeof(message));
		WARN(message);
	}

	CheckInvariants(__LINE__);
	return;
}


// Puts the actor at the new x,y,z,angle.
// set_z determines how the z coordinate is set
// if set_sector_anywhere is true, will set the sector no matter
// where the actor is. if false, it will set the sector to DEAD_SECTOR
// if the actor is not in the room.
void cActor::PlaceActor(float new_x, float new_y, float new_z,
						int new_angle, int set_z, bool set_sector_anywhere)
{
	float old_x = x;
	float old_y = y;

	if (terminate)
		return;

	x = new_x;
	y = new_y;
	angle = new_angle;
	sector = FindSector(x, y, sector, set_sector_anywhere);
	if (set_z == SET_Z) // set passed parameter
		z = new_z;
	else if (set_z == SET_XHEIGHT) // set to xheight
		this->SetXHeight();

	if (this->IsPlayer())
		((cPlayer*)this)->SetRoom(old_x, old_y);

	CheckInvariants(__LINE__);
	return;
}

// put actors on ground
float cActor::SetXHeight(void)
{
	if (sector != DEAD_SECTOR)
	{
		if (flags & ACTOR_CEILHANG)
			z = level->Sectors[sector]->CeilHt(x,y);
		else if(!(flags & ACTOR_FLY))
			z = level->Sectors[sector]->FloorHt(x,y)+level->Sectors[sector]->HtOffset+physht;
		eyeheight = (float)(z-(physht*.2));
	}

	CheckInvariants(__LINE__);
	return z;
}


// Sets all the values dealing with the bitmap representation.
// Called by the constructor and any time the bitmaps change,
// such as neighbors with their modular avatar construction.

bool cActor::SetBitmapInfo(realmid_t effect_id)
{
	if (effect_id == 0) // oops!
	{
		LoadString(hInstance, IDS_ACTOR_OF_TYPE, message, sizeof(message));
		_stprintf(errbuf, message, type, (int)x, (int)y);
		WARN(errbuf);
		this->SetTerminate();
		return false;
	}

	// validate #
	switch (type)
	{
		case ITEM:
			break;
		case MISSILE:
			if ((effect_id < 0) || (effect_id > 255))
			{
				LoadString(hInstance, IDS_ILLEGAL_ITEM_BITMAP, message, sizeof(message));
				_stprintf(errbuf, message, effect_id);
				WARN(errbuf);
			}
			break;
		default:
			break;
	}
	if (effects->LoadEffectBitmaps(effect_id, 4) == NO_BITMAP)
	{
		LoadString(hInstance, IDS_BITMAP_NO_FIND, message, sizeof(message));
		_stprintf(errbuf,message,effect_id);
		NONFATAL_ERROR(errbuf);
		if (type == ITEM)
		{
			((cItem*)this)->Lmitem().SetCharges(0);
			gs->DestroyItem((cItem*)this);
		}
		else
			this->SetTerminate();
		return false;
	}

	bitmap_id = effect_id;
	currframe = animate_ticks = 0;
	views = effects->EffectViews(effect_id);
	frames = effects->EffectFrames(effect_id);
	palette = effects->EffectPalette(effect_id);
	if (palette == USE_ACTOR_COLOR_TABLES)
	{
			colorized = true;
		bool agent = false;
		// Jared 7-21-00 Added check for player or neighbor
		if (((this->Type() == NEIGHBOR) || (this->Type() == PLAYER)) && ((cNeighbor*)this)->IsMonster())
			agent = true;


	//	if (agent || (player->IsMonster() && this->Type() == NEIGHBOR))
		if (agent)
			palette = MAGIC_AGENT_PALETTE_1;
		else
			palette = MAGIC_AVATAR_PALETTE_1;

		if ((this->Type() == NEIGHBOR) || (this->Type() == PLAYER))
		{
			LmAvatar avtr;
			if (this->Type() == NEIGHBOR)
				avtr = ((cNeighbor*)this)->Avatar();
			else if (this->Type() == PLAYER)
				avtr = ((cPlayer*)this)->Avatar();

			num_color_regions = 5;
			colors[0] = avtr.Color0();
			colors[1] = avtr.Color1();
			colors[2] = avtr.Color2();
			colors[3] = avtr.Color3();
			colors[4] = avtr.Color4();
		}
		else if (this->Type() == ITEM)
		{
			cItem *item = (cItem*)this;
			num_color_regions = 2;
			colors[0] = item->Lmitem().Color1();
			colors[1] = item->Lmitem().Color2();
		}
	}
	else if (palette == USE_FX_COLOR_TABLES)
	{
			colorized = true;
		palette = MAGIC_FX_PALETTE;
		// missiles and ornaments will set the colors correctly
		// in the constructor
	}
	else
	{
		colorized = false;
		num_color_regions = 0;
		memset(colors, 0, sizeof(short)*MAX_COLORED_REGIONS);
	}


	halfwit	= (float)(effects->EffectBitmap(bitmap_id)->h>>1);
	if (type == NEIGHBOR) // cheat on neighbor widths for more realistic cd
		 halfwit -= NEIGHBOR_WIDTH_CHEAT;
	physht	= effects->EffectBitmap(bitmap_id)->w;
	sector	= FindSector(x, y, sector, true);

	this->SetXHeight();

	CheckInvariants(__LINE__);
	return true;
}

// selector for current bitmap
int cActor::CurrBitmap(int view_angle)
{
	int offset;

	if (forming)
		return (LyraBitmap::FORMREFORM_EFFECT + currframe);
	else if (dissolving)
		return (LyraBitmap::FORMREFORM_EFFECT + effects->EffectFrames(LyraBitmap::FORMREFORM_EFFECT) - currframe);
	else if (entering)
		return (LyraBitmap::ENTRYEXIT_EFFECT + currframe);
	else if (flags & ACTOR_SOULSPHERE)
		return (LyraBitmap::SOULSPHERE_EFFECT + currframe);

	//AngleDiff = view_angle - angle;
	//View = FixAngle(AngleDiff+32)/(Angle_360/views);
	//if (View == 0)
	// offset = currframe*views;
	//else
	// offset = (currframe*views) + views - View;
	offset = currframe*views + CurrentView(view_angle);
	return (bitmap_id + offset);
}

// bitmap for icon view of actor; default to "front" view for frame 0
int cActor::IconBitmap(void)
{
	if ((type == PLAYER) && !leveleditor) // return control panel selected view
		return (bitmap_id + currframe*views + cp->CurrAvatarView());
	else
		return (bitmap_id + (currframe*views) + views/2);
}

// returns current view
int cActor::CurrentView(int view_angle)
{
	int AngleDiff,view;

	AngleDiff = view_angle - angle;
	view = FixAngle(AngleDiff+32)/(Angle_360/views);

	return (view == 0) ? 0 : views - view;
}

// returns the angle that the actor is at relative to the
// point (p_x,p_y)

int cActor::FacingAngle(float p_x, float p_y)
{
	return GetFacingAngle(x,y,p_x,p_y);
}


// Modify our height if we're falling or jumping.
void cActor::ModifyHeight(void)
{

	float xheight;
	int i;
//	int damage;

	//_tprintf("modifying height for %d, sector %d\n",this,sector);
	// don't screw with the zero rooms...

	//if (!LegalRoom(level->Sectors[sector]->room)) // old check, prob won't need again
	if (sector == DEAD_SECTOR)
		return;

	if (flags & ACTOR_CEILHANG)
		return;

	xheight = level->Sectors[sector]->FloorHt(x,y)+level->Sectors[sector]->HtOffset+physht;
	//_tprintf("height: %f physht: %f xheight: %f\n",height,physht,xheight);
	bool looking_down = IsPlayer() && ((cPlayer*)this)->LookingDown();
	bool looking_up = IsPlayer() && ((cPlayer*)this)->LookingUp();

	for (i=0; i<timing->sync_ticks; i++)
	{
		if ( (z == xheight) && (vertforce > 0.0))
		{
			vertforce = 0.0f; // we just hit the ground
		}
		else	if (( z < xheight ) && !(vertforce < 0))
		{
			vertforce = 0.0f;
			if (this->IsPlayer())
			{	// move up, FAST.
				if ( z < ((xheight-(physht*.5))+1) )
					z = (float)(xheight-(physht*.5))+1;
				z += 10.0f;
				if ( z > xheight) z = xheight;
			}
			else
				z = xheight;
		}
		else if (z > xheight || vertforce < 0)
		{ // accelerate down; check if we're at the peak of our
			vertforce += 6.0f;
			if ((vertforce > 0.0f) && (vertforce <= 6.0f))
				fall_height = z; // we're at the top of a fall
			
			if (flags & ACTOR_FLY)
			{
				if (!looking_up && !looking_down && vertforce > 0)
				{
					vertforce = 0;
					continue;
				}

				if (looking_up)
					vertforce = -3;
				if (looking_down)
					vertforce = 3;
			}

			z -= vertforce;
			if (z >= level->Sectors[sector]->CeilHt(x,y))
			{
				z = level->Sectors[sector]->CeilHt(x,y)-1;
				vertforce = 0;
			}
			if ((vertforce > 0) && (z <= xheight))
			{
			}
			if ( z < xheight)
			{
				z = xheight;
				if (!(this->flags & ACTOR_SOULSPHERE) &&
					((this->IsPlayer() && player->Jumped()) ||
					 (this->IsNeighbor() && ((cNeighbor*)this)->Jumping())))
				{
					if (this->InWater())
						cDS->PlaySound(LyraSound::ENTER_WATER, x, y, false);
					else
						cDS->PlaySound(LyraSound::JUMPLANDING, x, y, false);
					if (this->IsPlayer())
						player->SetJumped(false);
					if (this->IsNeighbor())
						((cNeighbor*)this)->SetJumping(false);
				}

				if (fall_height - z > MIN_JUMP_HEIGHT && !(player->flags & ACTOR_SOULSPHERE))
				{
 // fall damage is out for now - TEMP TESTING - DEV ONLY
 				if ((fall_height - z > MAX_JUMP_HEIGHT) && this->IsPlayer())
					{	// fall damage - d1-3 per 10' (10' = 256 pixels)
						int damage = 0;
						while (fall_height - z > TEN_FEET)
						{
							fall_height -= TEN_FEET;
							damage+=(rand()%3)+1;
						}
					if (damage>99) damage = 99;
					player->SetCurrStat(Stats::DREAMSOUL, -damage, SET_RELATIVE_NO_COLLAPSE, player->ID());
					if (this->InWater())
						cDS->PlaySound(LyraSound::ENTER_WATER, x, y, false);
					else
						cDS->PlaySound(LyraSound::JUMPLANDING, x, y, false);
					}
				}
			}
		}
	}

	if (this->IsNeighbor() && (z == xheight))
		((cNeighbor*)this)->SetJumping(false);
	eyeheight = (float)(z - (physht*.2));
	if ( eyeheight <= level->Sectors[sector]->FloorHt(x,y) )
		eyeheight = level->Sectors[sector]->FloorHt(x,y)+1.0f;

	CheckInvariants(__LINE__);
	return;
}


// actor will fire this missile after a short delay
void cActor::SetMissile(cMissile *new_missile)
{
	if (missile && (actors->ValidActor(missile)))
		missile->Activate();

	missile = new_missile;

	if (type == PLAYER)
		missile_launch_time = LyraTime() + PLAYER_MISSILE_LAUNCH_DELAY;
	else
		missile_launch_time = LyraTime() + OTHER_MISSILE_LAUNCH_DELAY;
	return;
}

bool cActor::InWater(void)
{
	if ((sector != DEAD_SECTOR) && level &&
		(sector < level->NumSectors()) &&
		level->Sectors[sector]->HtOffset &&
		(level->Sectors[sector]->flags & SECTOR_ANIMATING))
		return true;
	else
		return false;
}


void cActor::CheckMissile(void)
{
	if (missile && ((int)LyraTime() > missile_launch_time))
	{
		if (actors->ValidActor(missile))
			missile->Activate();
		missile = NULL;
	}
}

bool cActor::Render(void)
{
	if (terminate)
		return false;
	else if ((actors->ValidNeighbor(this)))
	{
		if(((cNeighbor*)this)->Avatar().Hidden())
			return false;
		else if (((cNeighbor*)this)->Avatar().PlayerInvis()) {
			return ((player->flags & ACTOR_DETECT_INVIS) && player->Skill(Arts::DREAMSEER) > 0);
		}
	}
	//else if ((actors->ValidNeighbor(this)))
	//{ // for debugging
		//if  (((cNeighbor*)this)->Avatar().Hidden())
//			return false;
	//}
	else if (((flags & ACTOR_INVISIBLE) || (flags & ACTOR_CHAMELED)) &&
		(player) && !(player->flags & ACTOR_DETECT_INVIS))
		return false;
	else
		return true;
}

short* cActor::ColorRegions(void)
{
	return colorized ? colors : NULL;
};

bool cActor::Colorized(void)
{
	return colorized;
};

int cActor::Palette(void)
{
	return palette;
};


bool cActor::LeftClick(void)
{
	return false;	// click not used
}
bool cActor::RightClick(void)
{
	return false;	// click not used
}

// Destructor

cActor::~cActor(void)
{
#ifndef AGENT
	if (!actors->ActorDeleteOK() && !exiting)
	{
		LoadString(hInstance, IDS_ACTOR_DELETE_BAD_TIME, message, sizeof(message));
		_stprintf(errbuf,message);
		NONFATAL_ERROR(errbuf);
	}
#endif

	//_tprintf("destroying actor %d of type %d; olist = %d \n",this,type,NumOList);
	delete l;

	if (level)
	{// remove ourselves from the OList...
		for (int i = 0; i<= level->NumOList; i++)
			if (level->OList[i].actor == this)
				level->OList[i].actor = NO_ACTOR;
	}

	if (name_tag)
		delete name_tag;


	if(effects && (bitmap_id != NO_BITMAP))
		effects->FreeEffectBitmaps(bitmap_id);

	if(actors)
		actors->RemoveActor(this);

#ifdef AGENT
	if (type == PLAYER)
		TlsSetValue(tlsPlayer, NULL);
#endif

	return;
}


// Check invariants

#ifdef CHECK_INVARIANTS

void cActor::CheckInvariants(int line)
{
	return;
}

#endif

//////////////////////////////////////////////////////////////////////////////
#include "cArts.h"
extern cArts *arts ;

cArtFX::cArtFX()
{
	 current_frame = animate_ticks = 0;
	 active = false;
	 memset(color_table,0,sizeof color_table);
	 active = evoking = false;
}

void cArtFX::Activate(int art_id, bool _evoking )
{
	 bool harmful			 = !arts->UseInSanctuary(art_id);
	int primary_color = arts->Stat( art_id) + 1;
	if (arts->Stat(art_id) == Stats::NO_STAT)
		primary_color = 0;

	int secondary_color	=	arts->MinOrbit( art_id)/10;
	Activate(harmful,primary_color, secondary_color, _evoking);
}

void cArtFX::Activate(bool _harmful, int primary_color, int secondary_color, bool _evoking)
{
	active	  = true;
	evoking	  = _evoking;

	if (_harmful)
		harmful = 1;
	else
		harmful = 0;

	if (evoking)
		effect_id =  harmful ? LyraBitmap::HARMFUL_EVOKING : LyraBitmap::HARMLESS_EVOKING;
	else
		effect_id =  harmful ? LyraBitmap::HARMFUL_EVOKED : LyraBitmap::HARMLESS_EVOKED;

	color_table[0] =	primary_color;   // Primary color is art stat. (element);
	color_table[1] =	secondary_color; // 2nd  color reflects art orbit;
}


void cArtFX::DeActivate()
{
	evoking = false;
}

bool cArtFX::Update()
{
	if (!active)
		return false;

	animate_ticks += timing->nmsecs;
	if (animate_ticks >= (ANIMATION_TICKS*3)/2)
	{
		animate_ticks = 0;
		if (++current_frame >= effects->EffectFrames(effect_id))
		{
			current_frame = 0;
			if (!evoking)
				active = false;
		}
		return true;
	}
	return false;
}



