// Copyright Lyra LLC, 1996-97. All rights reserved.

// cItem: Item base class. All actors that can be picked
// up or dropped are items.

#define STRICT

#include "cPlayer.h"
#include "cDDraw.h"
#include "cDSound.h"
#include "cGameServer.h"
#include "cChat.h"
#include "Move.h"
#include "cArts.h"
#include "cActorList.h"
#include "Options.h"
#include "cQuestBuilder.h"
#include "cLevel.h"
#include "cControlPanel.h"
#include "cEffects.h"
#include "Realm.h"
#include "LmItemDefs.h"
#include "LmAvatar.h"
#include "Utils.h"
#include "cItem.h"
#include "resource.h"

////////////////////////////////////////////////////////////////
// Local Global Variables

static int max_sort_index = 0;

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cPlayer *player;
extern cChat *display;
extern cControlPanel *cp;
extern cGameServer *gs;
extern cDSound *cDS;
extern cDDraw *cDD;
extern cArts *arts;
extern cLevel *level;
extern options_t options;
extern cEffects *effects;
extern cActorList *actors;
extern cQuestBuilder *quests;
extern timing_t *timing;
extern bool showing_map,map_shows_current_level;


//////////////////////////////////////////////////////////////////
// Constants

const float MAX_GRAB_DISTANCE = 500000.0f;
const float MAX_GRAB_HEIGHT = 10000.0f;
const float WARD_OFFSET = 20;
const float WARD_HEIGHT = 70;
const int NEED_LITTLE_MORE_SKILL = 10;
const int NEED_SOME_MORE_SKILL = 30;
const int BASE_CHANCE_OF_RECHARGE_DRAIN = 10;
const int SCROLL_INFINITE_CHARGES = 254;
const int RECHARGE_LIMIT = 100; // max recharge
const int RECHARGE_BACKFIRE = 10; // amt of drain on recharge
const int BASE_CHANCE_DESTRUCTION = 1;


//////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor

cItem::cItem(float i_x, float i_y, int i_angle, const LmItem& i_lmitem, int i_status,
			 unsigned __int64 i_flags, bool temp, DWORD expires, int i_sector) :
	cActor(i_x, i_y, i_angle, (i_flags | ACTOR_NOCOLLIDE), ITEM, i_sector),
		temporary(temp), expire_time(expires)
{
	this->SetLmItem(i_lmitem);
	expire_time_is_ttl = false;
	needsUpdate = marked_for_death = thrown =  redeeming = false;
	marked_for_drop = marked_for_death = destroy_at_zero = false;
	draggable = gravity = true;
	want_destroy_ack = false;
	extra = NULL;
	selected_function = inventory_flags = 0;
	max_sort_index++;
	sort_index = max_sort_index;
	// GMs are always draggable!
#if ! (defined (UL_DEBUG) || defined (GAMEMASTER))
	if ((lmitem.Header().Flags() & LyraItem::FLAG_NOREAP && !(lmitem.Header().Flags() & LyraItem::FLAG_ALWAYS_DROP)) ||
		ItemFunction(0) == LyraItem::PORTKEY_FUNCTION)
		draggable = false;
#endif

	if (palette == MAGIC_AVATAR_PALETTE_1)
		num_color_regions = 2;

	_tcscpy(description, TalismanName(lmitem.Graphic()));

	if (!this->SetBitmapInfo(lmitem.Graphic()))
	{	// use dummy icon until destroyed
		this->SetBitmapInfo(LyraBitmap::DREAMBLADE);
		if (i_status == ITEM_OWNED)
		{
			status = ITEM_OWNED;
			this->Destroy();
		}
		else
			this->SetTerminate();
		return;
	}

	// set up icon
#ifndef AGENT
	RenderActor(this,(PIXEL *)icon,ICON_WIDTH,ICON_HEIGHT);
#endif

	// it's important that status is set before the big switch
	status = -1;
	this->SetStatus(i_status);

///_tprintf("spawning item with id %u..\n",lmitem.ItemID());

	return;
}


bool cItem::Update(void)
{	// destroy if out of charges or expired temporary item...
	if (terminate)
		return false;

	if (((lmitem.Charges() <= 0) && (destroy_at_zero) && (status != ITEM_DESTROYING))
		|| (temporary && (LyraTime() > expire_time)))
	{
		if (status != ITEM_DUMMY)
		{
			LoadString (hInstance, IDS_ITEM_DISCHARGED, disp_message, sizeof(disp_message));
			_stprintf(message,disp_message,this->Name());
			display->DisplayMessage(message, false);
			cDS->PlaySound(LyraSound::EXPENDED_ITEM);
		}
		return (this->Destroy());
	}

	animate_ticks += timing->nmsecs;
	if (animate_ticks >= ANIMATION_TICKS)
	{
		animate_ticks = 0;
		if (++currframe >= frames)
			currframe = 0;
	}

	// don't modify height for no gravity flags, owned items, & wards
	if (gravity)
		this->ModifyHeight();

	return TRUE;
}

bool cItem::Render(void)
{
	if (this == cp->DragItem())
		return FALSE;
	else if ((status == ITEM_UNOWNED) || (status == ITEM_DROPPING))
		return TRUE;
	else
		return FALSE;
}

// determine if gravity should act on the item
void cItem::SetGravity(void)
{
	gravity = true;

	if (status != ITEM_UNOWNED)
		gravity = false;

	else if ((this->ItemFunction(0) == LyraItem::WARD_FUNCTION) || (this->ItemFunction(0) == LyraItem::PORTKEY_FUNCTION) && (extra != NULL))
		gravity = false;

	return;
}


// returns true if the item is within reach of the actor, false
// otherwise
bool cItem::WithinReach(cActor *actor)
{
	float dx = (actor->x - x);
	float dy = (actor->y - y);
	float dz = (actor->z - actor->physht - z);

	if ((dx*dx + dy*dy > MAX_GRAB_DISTANCE) ||
		 ((actor->z - actor->physht - z) > MAX_GRAB_HEIGHT) ||
		((z - actor->z) > MAX_GRAB_HEIGHT))
		 return FALSE;
	else
		return TRUE;

}


//  try to pick up item on left click
bool cItem::LeftClick(void)
{
	// can't pick up items while under certain effects
	if ((player->flags & ACTOR_DRUNK) || (player->flags & ACTOR_SCARED) ||
		(player->flags & ACTOR_PARALYZED))
		return true;

	if (Status() == ITEM_UNOWNED && Draggable())
	{ // calculate hot spot based on item size + click offset; add 1 to avoid divide by zeros
		if (player->flags & ACTOR_SOULSPHERE)
		{
			LoadString (hInstance, IDS_SOULSPHERE_NO_PICKUP, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
		else if (WithinReach(player))
			// cp->StartDrag(item, (l_click_pos.x - boxes[i].x)/((effects->EffectWidth(item->BitmapID())/ICON_WIDTH)+1) ,
			// 	(l_click_pos.y - boxes[i].y)/((effects->EffectHeight(item->BitmapID())/ICON_HEIGHT)+1));
			cp->StartDrag(this, ICON_WIDTH/2,ICON_HEIGHT);
		else
		{
			LoadString (hInstance, IDS_ITEM_TOOFAR, disp_message, 256);
			display->DisplayMessage (disp_message);
		}
	}
	return true; // true == click was used
}

// identify Item
bool cItem::RightClick(void)
{
	// Check if we're allowed to right click
	if (!gs->AllowRightClick()) return false;

	TCHAR buffer[64];

	if (lmitem.Header().Flags() & LyraItem::FLAG_HASDESCRIPTION)
	{	// item has description - retrieve if not a scroll!
		bool is_scroll = false;
		for (int i=0; i<this->NumFunctions(); i++)
			if (this->ItemFunction(i) == LyraItem::SCROLL_FUNCTION)
				is_scroll = true;

		if ((!is_scroll) || ((lmitem.Charges() >= INFINITE_CHARGES - 1) && (status == ITEM_UNOWNED)))
			gs->SendItemDescripRequest(lmitem.Header());
	}
		
	if (this->ItemFunction(0) == LyraItem::META_ESSENCE_FUNCTION)
	{
		const void *state = lmitem.StateField(0);
		lyra_item_meta_essence_t meta_essence;
		memcpy(&meta_essence, state, sizeof(meta_essence));
		LoadString (hInstance, IDS_IDENTIFY_META_ESSENCE, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, GuildName(meta_essence.guild_id),
			meta_essence.strength(), meta_essence.num_mares());
		display->DisplayMessage(message, false);
	}
	else if (this->ItemFunction(0) == LyraItem::META_ESSENCE_NEXUS_FUNCTION)
	{
		const void *state = lmitem.StateField(0);
		lyra_item_meta_essence_nexus_t nexus;
		memcpy(&nexus, state, sizeof(nexus));
		LoadString(hInstance, IDS_IDENTIFY_ESSENCE_NEXUS, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, nexus.essences, nexus.strength, nexus.essence_cap, nexus.strength_cap);
		display->DisplayMessage(message, false);
	}
	else
	{
#if ! (defined (UL_DEBUG) || defined (GAMEMASTER))
		// forged ornament - don't go further, we'll display the description text on Right Click
		// but they shouldn't see the That's a W_F_BOOBIES_1 message.
		if (!draggable)
			return true;
#endif
		LoadString (hInstance, IDS_ITEM_DESCRIBE, disp_message, sizeof(disp_message));
		// only colorize talismans t0-t9
		if (bitmap_id == LyraBitmap::DREAMBLADE)
		_stprintf(buffer, _T("%s"), DreamweaponName(lmitem.Color1()));
		else if ((bitmap_id < LyraBitmap::TALISMAN0) || (bitmap_id > LyraBitmap::TALISMAN8))
		_stprintf(buffer, _T("%s"), description);
		else if (lmitem.Color1() == lmitem.Color2()) // single color
		_stprintf(buffer, _T("%s %s"), ColorName(lmitem.Color1()), description);
		else // dual color
		{	// we call the alternate ColorName function because we need to store two different color names
			TCHAR color1[DEFAULT_MESSAGE_SIZE]; 
			TCHAR color2[DEFAULT_MESSAGE_SIZE];
			ColorName(lmitem.Color1(), color1, DEFAULT_MESSAGE_SIZE);
			ColorName(lmitem.Color2(), color2, DEFAULT_MESSAGE_SIZE);
			_stprintf(buffer, _T("%s %s %s"), color1, color2, description);

		}
	_stprintf(message,disp_message,buffer);
#ifdef PMARE // Pmares don't need to see anything other than the item name
	display->DisplayMessage(message,false);
#else

		if (status == ITEM_OWNED)
		{
				for (int i=0; i<this->NumFunctions(); i++)
				if (this->ItemFunction(i) == LyraItem::MISSILE_FUNCTION)
				{
					const void *state;
					state = lmitem.StateField(i);

					lyra_item_missile_t missile;
					memcpy(&missile, state, sizeof(missile));

					int relevant_art;
					TCHAR buffer[80];
					// determine relevant art based on coloration;
					// take primary color, mod 4, and add to GATEKEEPER
					relevant_art = Arts::GATEKEEPER + (int)(lmitem.Header().Color1())/4;
					int req_skill = MinModifierSkill(missile.damage);
					LoadString (hInstance, IDS_NEEDED_SKILL, disp_message, sizeof(disp_message));
				_stprintf(buffer,disp_message, req_skill, arts->Descrip(relevant_art));
				_tcscat(message,buffer);
				}
			if ((inventory_flags & ITEM_IDENTIFIED))
			{
				_tcscat(message,_T("\n"));
				display->DisplayMessage(message, false);
				this->Identify();
			}
			else
			{
				LoadString (hInstance, IDS_ITEM_NO_ID, disp_message, sizeof(disp_message));
				_tcscat(message,disp_message);
				display->DisplayMessage(message, false);
			}
		}
		else
		{
			_tcscat(message,_T("\n"));
			display->DisplayMessage(message, false);
		}
#endif
	}
	return true;
}

void cItem::DisplayCreateMessage(void)
{
	if ((this->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION) ||
		(this->ItemFunction(0) == LyraItem::SUPPORT_FUNCTION) ||
		(this->ItemFunction(0) == LyraItem::AREA_EFFECT_FUNCTION  && _tcsicmp(this->Name(), "Razorwind") == 0) ||
		(this->ItemFunction(0) == LyraItem::SUPPORT_TRAIN_FUNCTION) ||
		quests->Active())
	{
		// do nothing
	}
	else
	{
		LoadString (hInstance, IDS_ITEM_CREATED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Name());
		display->DisplayMessage(message);
	}
	return;
}


void cItem::DisplayDropMessage(void)
{
	if (this->ItemFunction(0) == LyraItem::WARD_FUNCTION)
	{
		LoadString (hInstance, IDS_PLACED_WARD, message, sizeof(message));
		display->DisplayMessage(message, false);
	}
	else
	{
		LoadString (hInstance, IDS_ITEM_DROPPED, disp_message, sizeof(message));
	_stprintf(message,disp_message,this->Name());
		display->DisplayMessage(message);
	}
	return;
}

void cItem::DisplayTakeMessage(void)
{
	LoadString (hInstance, IDS_ITEM_TAKEN, disp_message, sizeof(message));
_stprintf(message, disp_message, this->Name());
	display->DisplayMessage(message);
	return;
}

void cItem::DisplayGivenMessage(void)
{
	LoadString (hInstance, IDS_ITEM_GIVEN, disp_message, sizeof(message));
_stprintf(message, disp_message, this->Name());
	display->DisplayMessage(message);
	return;
}

void cItem::DisplayReceivedMessage(void)
{
	LoadString (hInstance, IDS_ITEM_RECEIVED, disp_message, sizeof(message));
_stprintf(message, disp_message, this->Name());
	display->DisplayMessage(message);
	return;
}


// set status; reset height if dropped
void cItem::SetStatus(int new_status)
{
	int old_status = status;

	if (old_status == new_status)
		return;

	status = new_status;

	if (old_status == ITEM_OWNED)
	{
		cp->DeleteItem(this); // remove from control panel
		if (cp->SelectedItem() == this)
			cp->SetSelectedItem(NO_ITEM);
		if (player->ActiveShield() == this)
		{
			player->SetActiveShield(NO_ITEM);
			inventory_flags = inventory_flags & ~ITEM_ACTIVE_SHIELD;
			player->SetItemNeedFlagsOrSortingUpdate(true);
			needsUpdate = true;
		}
	}

	if (new_status == ITEM_UNOWNED)
		destroy_at_zero = false;
	else if (new_status == ITEM_OWNED)
	{
		destroy_at_zero = true;
		max_sort_index++;
		sort_index = max_sort_index;
		cp->AddItem(this);
	}


	this->SetGravity();

	if ((status == ITEM_UNOWNED) && gravity)
		this->SetXHeight();

	switch (this->ItemFunction(0))
	{	// special case handling of certain item types
		case LyraItem::WARD_FUNCTION:
			{ // unowned wards get placed
			if (status != ITEM_UNOWNED)
				break;
			lyra_item_ward_t ward;
			const void* state;
			state = lmitem.StateField(0);
			memcpy(&ward, state, sizeof(ward));
			if (!(this->PlaceWard()))
			{	// couldn't place the ward
					this->SetTerminate();
					return;
			}
		}
			break;
		default:
			break;
	}

	cp->SetUpdateInvCount(true);
	return;
}

// returns the function associated with a function slot (0-2)
int cItem::ItemFunction(int slot)
{
	if (!lmitem.FieldIsValid(slot))
		return LyraItem::NO_FUNCTION;

	const void* state = lmitem.StateField(slot);
	return (*((unsigned char*)state));
}

bool cItem::NoPickup(void)
{
	return (lmitem.Header().Flags() & LyraItem::FLAG_NOREAP && !(lmitem.Header().Flags() & LyraItem::FLAG_ALWAYS_DROP)) ||
		ItemFunction(0) == LyraItem::PORTKEY_FUNCTION;
}

// can the item be lost on dissolution?
bool cItem::Losable(void)
{
	for (int i=0; i<lmitem.NumFields(); i++)
	// now identify the individual funtions
		if (!LyraItem::FunctionLosable(this->ItemFunction(i)))
			return false;

	return true;
}


void cItem::Use(void)
{
	const void *state;

#ifdef PMARE
	LoadString (hInstance, IDS_PMARE_NO_ITEMS, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	return;
#endif


	bool drain_charge = false;

	if (!lmitem.Charges())
		return;

	player->PerformedAction();

	if (!options.welcome_ai && !gs->LoggedIntoLevel())
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		return;
	}

	if ((status != ITEM_OWNED) || (!lmitem.FieldIsValid(selected_function)))
	{
		cDS->PlaySound(LyraSound::MESSAGE);
		return;
	}

	if (player->flags & ACTOR_SOULSPHERE) // && !(player->flags & ACTOR_SOULEVOKE))
	{
		LoadString (hInstance, IDS_SOULSPHERE_NO_USE, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		return;
	}

	// Make absolutely sure no one can use items while in NMF or as a pmare
	//  Not sure why this isn't active, but putting it here to be sure

#ifdef PMARE
		player->NightmareAttack();
		return;
#endif

	if (player->flags & ACTOR_TRANSFORMED)
	{
		player->NightmareAttack();
		return;
	}
	// Disallow use of talismans while paralyzed!
#if 0
	if (player->flags & ACTOR_PARALYZED)
	{
		// This should be in a macro somewhere....
		LoadString (hInstance, IDS_NO_ITEM_PARALYZED, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		return;
	}
#endif 0

	// only allow item use every 500 ms or so
	if (options.network) 
	{
		if (!gs->CanUseItem())
			return;
	}
	state = lmitem.StateField(selected_function);

	switch (this->ItemFunction(selected_function))
	{
		case LyraItem::CHANGE_STAT_FUNCTION:
		{
			lyra_item_change_stat_t changestat;
			memcpy(&changestat, state, sizeof(changestat));
			drain_charge = true;

			// 1 -> ds plat level
			int ds_mod = 0; 
			int ds_plat = player->SkillSphere(Arts::DREAMSEER);

			if (ds_plat > 0) {
				ds_mod = (rand() % ds_plat) + 1;
#ifdef UL_DEV
				_stprintf(message, "Adding additional %d points to the elemen usage due to your ties to Insight", ds_mod);
				display->DisplayMessage(message);
#endif
			}

			int modifier = CalculateModifier(changestat.modifier) + ds_mod;

			if (((player->CurrStat(changestat.stat) == player->MaxStat(changestat.stat)) && (modifier >= 0)) ||
				((player->CurrStat(changestat.stat) == Stats::STAT_MIN) && (modifier <= 0)))
			{
				LoadString (hInstance, IDS_NOTHING_HAPPENS, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
			}
			else if (modifier > 0)
			{
				player->SetCurrStat(changestat.stat, modifier, SET_RELATIVE, player->ID());
				LoadString (hInstance, IDS_ITEM_FEELREFRESHED, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
			}
			else
			{
				player->SetCurrStat(changestat.stat, modifier, SET_RELATIVE, player->ID());
				LoadString (hInstance, IDS_ITEM_FEELDRAINED, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
			}
		}
			break;
		case LyraItem::MISSILE_FUNCTION:
		{
			if (player->flags & ACTOR_PARALYZED)
			{
				// This should be in a macro somewhere....
				LoadString (hInstance, IDS_NO_ITEM_PARALYZED, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message);
			} else {

			if (player->flags & ACTOR_PEACE_AURA)
			{
				LoadString (hInstance, IDS_PA_NO_ARTS, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message);
				return;
			}

			lyra_item_missile_t missile;
			memcpy(&missile, state, sizeof(missile));
			drain_charge = true;
			if (options.network) // drain only on successful launch
			{	// 1st check to see if we're skilled enough
				int relevant_art, skill_delta;
				// determine relevant art based on coloration;
				// take primary color, mod 4, and add to GATEKEEPER
				relevant_art = Arts::GATEKEEPER + (int)(lmitem.Header().Color1())/4;
				skill_delta = MinModifierSkill(missile.damage) - player->Skill(relevant_art);
				if ((player->Skill(relevant_art) > 0) && (skill_delta <= 0)) // skilled enough, send it off...
				{	// weed out invalid missiles
					if (missile.bitmap_id == 0)
					{
						lmitem.SetCharges(0);
						needsUpdate = true;
					}
					else if (gs->PlayerAttack(missile.bitmap_id, missile.velocity,
						missile.effect, missile.damage, this, relevant_art))
					{ // only increase skill if we actually fire
						arts->BeginArt(relevant_art);
						drain_charge = true;
					}
					else
						drain_charge = false;
				}
				else
				{ // not skilled enough - display message
					drain_charge = false;
					if (skill_delta <= NEED_LITTLE_MORE_SKILL)
						LoadString (hInstance, IDS_WEAPON_DENIED_ALMOST, disp_message, sizeof(disp_message));
					else if (skill_delta <= NEED_SOME_MORE_SKILL)
						LoadString (hInstance, IDS_WEAPON_DENIED_MODERATE, disp_message, sizeof(disp_message));
					else
						LoadString (hInstance, IDS_WEAPON_DENIED_DREAMON, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, arts->Descrip(relevant_art));
					display->DisplayMessage (message);
				}
			}
			else // unnetworked, just launch
				new cMissile(player, missile.bitmap_id, player->angle,
					player->HeightDelta(), missile.velocity,
					missile.effect, missile.damage, this);
			}
		}
			break;
		case LyraItem::EFFECT_PLAYER_FUNCTION:
		{
			lyra_item_effect_player_t effectplayer;
			memcpy(&effectplayer, state, sizeof(effectplayer));
			drain_charge = true;
			player->SetTimedEffect(effectplayer.effect, CalculateDuration(effectplayer.duration), player->ID(), EffectOrigin::USE_ITEM);
		}
			break;
		case LyraItem::META_ESSENCE_FUNCTION:
		{
			lyra_item_meta_essence_t meta_essence;
			lyra_item_essence_t essence;
			lyra_item_meta_essence_nexus_t nexus;
			bool drains = false;
			memcpy(&meta_essence, state, sizeof(meta_essence));

			// hack code to reset essences
			/*
			state = this->Lmitem().StateField(0);
			memcpy(&essence, state, sizeof(essence));
			unsigned int new_strength = meta_essence.strength() - 65000;
			meta_essence.set_strength(new_strength);
			//meta_essence.set_num_mares(0);
			needsUpdate = true;
			lmitem.SetStateField(0, &meta_essence, sizeof(meta_essence));
			break;
			*/

			// check that the proper # of ruler support tokens are carried
			for (cItem *item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
				if (item->Status() == ITEM_OWNED)
				{
					switch (item->ItemFunction(0))
					{
					case LyraItem::ESSENCE_FUNCTION:
					{
						// it's essence - add to meta if it's not a user
						state = item->Lmitem().StateField(0);
						memcpy(&essence, state, sizeof(essence));
						if (essence.mare_type >= Avatars::MIN_NIGHTMARE_TYPE)
						{ // add strength to meta talisman
							meta_essence.set_strength(meta_essence.strength() + essence.strength);
							meta_essence.set_num_mares(meta_essence.num_mares() + 1);
							item->Lmitem().SetCharges(0);
							drains = true;
						}
					}
					break;
					case LyraItem::META_ESSENCE_NEXUS_FUNCTION:
					{
						state = item->Lmitem().StateField(0);
						memcpy(&nexus, state, sizeof(nexus));
						if (nexus.essences > 0)
							meta_essence.set_num_mares(meta_essence.num_mares() + nexus.essences);
						if (nexus.strength > 0)
							meta_essence.set_strength(meta_essence.strength() + nexus.strength);
						// MDA: Note, should this just be Destroy()?
						item->Lmitem().SetCharges(0);
						drains = true;
					}
					break;
					}
				}

			actors->IterateItems(DONE);
			if (drains)
			{
				LoadString(hInstance, IDS_META_ESSENCE_SUCCESS, disp_message, sizeof(disp_message));
				lmitem.SetStateField(0, &meta_essence, sizeof(meta_essence));
				needsUpdate = true;
			}
			else
				LoadString(hInstance, IDS_META_ESSENCE_FAILURE, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
		}
		break;

		case LyraItem::META_ESSENCE_NEXUS_FUNCTION:
		{
			lyra_item_meta_essence_nexus_t nexus;
			lyra_item_essence_t essence;
			bool drains = false, full = false;
			memcpy(&nexus, state, sizeof(nexus));

			for (cItem *item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			{
				if (nexus.strength >= nexus.strength_cap || nexus.essences >= nexus.essence_cap) {
					if (!drains)
						LoadString(hInstance, IDS_CHAOS_WELL_FULL, disp_message, sizeof(disp_message));

					full = true;
					break;
				}
				if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION))
				{ // it's essence - add to meta if it's not a user
					int avail_space = nexus.strength_cap - nexus.strength;
					state = item->Lmitem().StateField(0);
					memcpy(&essence, state, sizeof(essence));
					if (essence.mare_type >= Avatars::MIN_NIGHTMARE_TYPE)
					{ 
						// add strength to meta talisman
						if (avail_space >= essence.strength) {
							// meta talisman can take the entire essence
							nexus.strength += essence.strength;
							// only increase essences if it's entirely absorbed
							nexus.essences++;
							item->Lmitem().SetCharges(0);
						}
						else {
							// partial use of essence
							nexus.strength += avail_space;
							essence.strength -= avail_space;
							item->Lmitem().SetStateField(0, &essence, sizeof(essence));
							needsUpdate = true;
						}
						drains = true;
					}
				}
			}

			actors->IterateItems(DONE);
			// if I drained, regardless if I filled it, display success
			if (drains)
			{
				LoadString(hInstance, IDS_CHAOS_WELL_SUCCESS, disp_message, sizeof(disp_message));
				lmitem.SetStateField(0, &nexus, sizeof(nexus));
				needsUpdate = true;
			} // if I didn't drain and I'm not full display failure
			else if (!full)
			{
				LoadString(hInstance, IDS_CHAOS_WELL_FAILURE, disp_message, sizeof(disp_message));
			}
			
			display->DisplayMessage(disp_message);
		}
		break;

		case LyraItem::ARMOR_FUNCTION:
			if (player->SetActiveShield(this))
			{
				inventory_flags = inventory_flags | ITEM_ACTIVE_SHIELD;
				player->SetItemNeedFlagsOrSortingUpdate(true);
				LoadString (hInstance, IDS_NEW_SHIELD, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, this->Name());
				display->DisplayMessage(message);
			}

			needsUpdate = true;
			break;

		case LyraItem::SCROLL_FUNCTION:
		{
			TCHAR buffer[DEFAULT_MESSAGE_SIZE];
			lyra_item_scroll_t scroll;
			memcpy(&scroll, state, sizeof(scroll));

			int tid = scroll.targetid();

			gs->SendItemDescripRequest(lmitem.Header());
			// drain a charge only if scroll has limited number of reads
			if (lmitem.FlagSet(LyraItem::FLAG_CHANGE_CHARGES)) {
			  drain_charge = true;
			}

			if (lmitem.Charges() == SCROLL_INFINITE_CHARGES)
				LoadString (hInstance, IDS_ANY_NUM, buffer, sizeof(buffer));
			else
				_stprintf(buffer, _T("%d"), (lmitem.Charges() - 1));

			if (scroll.art_id == 0) 
			{
				LoadString (hInstance, IDS_READ_SCROLL, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, buffer);
			}
			else
			{
				LoadString (hInstance, IDS_READ_QUEST, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, arts->Descrip(scroll.art_id-1), buffer);
			}

			display->DisplayMessage(message);
			break;
		}

		case LyraItem::MAP_FUNCTION:
		{
			lyra_item_map_t map;
			memcpy(&map, state, sizeof(map));
			showing_map = true;
			map_shows_current_level = (level->ID() == 20);//(map.level_id == level->ID());
			drain_charge = true;
		}
		break;

		case LyraItem::TELEPORTER_FUNCTION:
		{
			lyra_item_teleporter_t tport;
			memcpy(&tport, state, sizeof(tport));
			player->Teleport( tport.x, tport.y, player->angle, tport.level_id);
			drain_charge = true;
		}
		break;

		case LyraItem::GRATITUDE_FUNCTION:
		{
			lyra_item_gratitude_t gratitude;
			memcpy(&gratitude, state, sizeof(gratitude));

			lyra_id_t creator_id = gratitude.creatorid();
			lyra_id_t target_id = gratitude.target;
			// no target means it is unassigned
			if (creator_id == player->ID())
			{
				if (0 != target_id) 
				{
					LoadString (hInstance, IDS_GRATITUDE_ASSIGNED, disp_message, sizeof(disp_message));
					display->DisplayMessage (disp_message);
					break;
				}
				else if (actors->NumNonHiddenNeighbors() > 0) 
				{
					LoadString (hInstance, IDS_GRATITUDE_HELP, disp_message, sizeof(disp_message));
					display->DisplayMessage (disp_message);
					arts->art_in_use = Arts::SHOW_GRATITUDE;
					arts->SetGratitudeItem(this); 
					arts->ApplyArt();
					break;
				} 
				else
				{
					LoadString (hInstance, IDS_GRATITUDE_HELP2, disp_message, sizeof(disp_message));
					display->DisplayMessage (disp_message);
					break;
				}
			}
			else
			{
				lyra_id_t id = player->ID();
				id = id & 0x0000ffff; // mask off lower bits only
				if (id == target_id) 
				{ 
#if 0 // gratitute maturity period deprecated
					// targetted at us, check maturity
					// convert current date PST to # of days since 1/1/2000
					SYSTEMTIME dsttime;
					GetDSTTime(&dsttime);
					int time_num_days = (dsttime.wYear - 2000)*365 + dsttime.wDay;
					switch (dsttime.wMonth) {
					case 12:
						time_num_days += 30;
					case 11:
						time_num_days += 31;
					case 10:
						time_num_days += 30;
					case 9:
						time_num_days += 31;
					case 8:
						time_num_days += 31;
					case 7:
						time_num_days += 30;
					case 6:
						time_num_days += 31;
					case 5:
						time_num_days += 30;
					case 4:
						time_num_days += 31;
					case 3:
						time_num_days += 28;
					case 2:
						time_num_days += 31;
					default:
						break;
					}
					if (time_num_days < gratitude.maturity_date)
					{
						LoadString (hInstance, IDS_GRATITUDE_NOT_YET, disp_message, sizeof(disp_message));
						_stprintf(message, disp_message, gratitude.maturity_date - time_num_days);
						display->DisplayMessage(message);
						break;
					} 
					else 
#endif
					//{
						LoadString (hInstance, IDS_REDEEMING, disp_message, sizeof(disp_message));
						display->DisplayMessage (disp_message);	
						unsigned char state1 = ((gratitude.creator_lo & 0xff00) >> 8);
						unsigned char state2 = (gratitude.creator_lo & 0x00ff);
						gs->SendPlayerMessage(0, RMsg_PlayerMsg::REDEEM_GRATITUDE, state1, state2);						
						redeeming = true;
						break;
					//}

				} 
				else if (target_id != 0) 
				{
					LoadString (hInstance, IDS_GRATITUDE_NOTYOURS2, disp_message, sizeof(disp_message));
					display->DisplayMessage (disp_message);
					break;
				}
				else // not targetted at us
				{
					LoadString (hInstance, IDS_GRATITUDE_NOTYOURS1, disp_message, sizeof(disp_message));
					display->DisplayMessage (disp_message);
					break;
				}
			}		
			break;
		}

		case LyraItem::SUPPORT_FUNCTION:
		{
			lyra_item_support_t token;
			lyra_item_support_t other_token;
			bool drains = false, full = false;
			int max_size = 10;

			memcpy(&token, state, sizeof(token));

			// only combine power tokens
			if (token.token_type() == Tokens::POWER_TOKEN)
			{
				for (cItem *item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
				{
					if (lmitem.Charges() >= max_size)
					{
						// no mas, we're full
						full = true;
						break;
					}

					if ((item->Status() == ITEM_OWNED) && item->ItemFunction(0) == LyraItem::SUPPORT_FUNCTION && 
						item->Lmitem().Charges() < max_size && this->ID().Serial() != item->ID().Serial())
					{
						state = item->Lmitem().StateField(0);
						memcpy(&other_token, state, sizeof(other_token));

						// make sure this is a power token and the same guild as the one we're trying to combine
						if (other_token.token_type() == Tokens::POWER_TOKEN && other_token.guild_id() == token.guild_id())
						{
							int avail_space = max_size - lmitem.Charges();

							// absorb the entire item
							if (avail_space > item->Lmitem().Charges())
							{
								lmitem.SetCharges(lmitem.Charges() + item->Lmitem().Charges());
								item->Lmitem().SetCharges(0);
							}
							// only eat as many charges as we need
							else
							{
								lmitem.SetCharges(lmitem.Charges() + avail_space);
								item->Lmitem().SetCharges(item->Lmitem().Charges() - avail_space);
								needsUpdate = true;
							}
							drains = true;
						}
					}
				}

				actors->IterateItems(DONE);

				// if I drained, regardless if I filled it, display success
				if (drains)
				{
					LoadString(hInstance, IDS_PT_COMBINED, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
					lmitem.SetStateField(0, &token, sizeof(token));
					needsUpdate = true;

					// break here so we can show 'nothing happens' if anything else happens.
					break;
				} 
			}
		}
		// the SUPPORT_FUNCTION case will intentionally fall through if it's the wrong support type
		case LyraItem::ESSENCE_FUNCTION:
		case LyraItem::WARD_FUNCTION:
		case LyraItem::AMULET_FUNCTION:
		case LyraItem::AREA_EFFECT_FUNCTION:
		default:
			LoadString (hInstance, IDS_NOTHING_HAPPENS, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			break;
	}

	if (drain_charge)
		this->DrainCharge();

	return;
}

void cItem::ApplyGratitude(cNeighbor* n)
{
	const void* state = lmitem.StateField(0);

	lyra_item_gratitude_t gratitude;
	memcpy(&gratitude, state, sizeof(gratitude));

	lyra_id_t target = n->ID();
	target = target & 0x0000FFFF; // take only lower 2 bytes
	gratitude.target = target;
	lmitem.SetStateField(0, &gratitude, sizeof(gratitude));
	needsUpdate = true;

	LoadString (hInstance, IDS_APPLY_GRATITUDE, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name(), n->Name());
	display->DisplayMessage(message);

}


// drain a charge from the item
void cItem::DrainCharge(void)
{
	if (lmitem.Charges() == INFINITE_CHARGES)
		return;

	int charges = lmitem.Charges()-1;
	if (charges < 0)
		charges = 0;
	lmitem.SetCharges(charges);
	needsUpdate = true;
}

// do any necessary processing upon dropping; drop_x, drop_y, and
// drop_angle are relevant only for items not being destroyed
void cItem::Drop(float drop_x, float drop_y, int drop_angle)
{
	if (redeeming) 
		return; // can't drop a gratitude token while it is being redeemed
	
	if (ItemFunction(0) == LyraItem::PORTKEY_FUNCTION && arts->GetPortkey(ITEM_UNOWNED))
	{
		display->DisplayMessage("A room may only contain one portkey!");
		return;
	}

	if (this->NeedsUpdate())
		this->Update();

	player->PerformedAction();

	if (temporary && ItemFunction(0) != LyraItem::PORTKEY_FUNCTION)
	{ // temporary items are destroyed on a drop
		lmitem.SetCharges(0);
		cp->SetUpdateInvCount(true);
		return;
	}
	if (status == ITEM_DROPPING)
	{ // ensure we're not already trying to get it
		LoadString (hInstance, IDS_ITEM_ALREADYDROPPING, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return;
	} else if (status != ITEM_OWNED)
	{ // ensure we're not already trying to get it
		LoadString (hInstance, IDS_ITEM_CANTDROP, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return;
	}

	// place item for drop
	this->PlaceActor(drop_x, drop_y, z, drop_angle, SET_Z, false);

	if (options.network && !options.welcome_ai)
		gs->DropItem(this);
	else
		this->SetStatus(ITEM_UNOWNED); // no wait for server ack when non-networked

	return;
}

// destroy the item; returns false if it should be immediately
// destroyed, or true otherwise
bool cItem::Destroy(void)
{
	if ((status != ITEM_OWNED) && ((this->ItemFunction(0) != LyraItem::WARD_FUNCTION) || (status != ITEM_UNOWNED)))
	{
		if (status != ITEM_DUMMY)
			{ // ensure we're not already trying to get it
				LoadString (hInstance, IDS_ITEM_CANTDESTROY, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
				return true;
			}	
	}

	if (temporary || options.welcome_ai)
		return false;
	else if (options.network)
	{
		gs->DestroyItem(this);
		return true;
	}
	else
		return false;
}

// do necessary processing when picked up
void cItem::Get(void)
{
	if (player->flags & ACTOR_SOULSPHERE)
	{
		LoadString (hInstance, IDS_SOULSPHERE_NO_PICKUP, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return;
	}

	if (cp->InventoryFull())
	{ // carrying too much stuff
		LoadString (hInstance, IDS_ITEM_TOOMUCH, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return;
	}
	if (status == ITEM_GETTING)
	{ // ensure we're not already trying to get it
		LoadString (hInstance, IDS_ITEM_ALREADYGETTING, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return;
	} else if (status != ITEM_UNOWNED)
	{ // ensure we're not already trying to get it
		LoadString (hInstance, IDS_ITEM_CANTGET, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return;
	}

	//display->DisplayMessage("Trying to get the item...\n");

	if (options.network && !options.welcome_ai)
		gs->GetItem(this);
	else
		this->SetStatus(ITEM_OWNED);

	return;
}

void cItem::SetSortIndex(int value)
{
	sort_index = value;
	if (sort_index > max_sort_index)
		max_sort_index = value;
}


// identifies the item by displaying it's properites on the screen
bool cItem::Identify(bool from_art)
{
	TCHAR charges[DEFAULT_MESSAGE_SIZE];
	TCHAR num_functions[DEFAULT_MESSAGE_SIZE];
	TCHAR function_descrip[DEFAULT_MESSAGE_SIZE];
	TCHAR entry_descrip[DEFAULT_MESSAGE_SIZE];
	char *buffer;
	int function,i,j;
	int value,offset;
	const void* state;

	if (status != ITEM_OWNED)
	{
		LoadString (hInstance, IDS_CANT_IDENTIFY, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return false;
	}

	if (from_art && (inventory_flags & ITEM_IDENTIFIED))
	{ // already identified
		LoadString (hInstance, IDS_ALREADY_IDENTIFIED, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		return false;
	}

	if (!(inventory_flags & ITEM_IDENTIFIED))
	{
		inventory_flags = inventory_flags | ITEM_IDENTIFIED;
		player->SetItemNeedFlagsOrSortingUpdate(true);
		needsUpdate = true;
	}

	LoadString (hInstance, IDS_START_IDENTIFY, disp_message, sizeof(disp_message));
	if (lmitem.Charges() == INFINITE_CHARGES)
		LoadString (hInstance, IDS_INFINITE_CHARGES, charges, sizeof(charges));
	else if (lmitem.Charges() == 1)
		LoadString (hInstance, IDS_1CHARGE, charges, sizeof(charges));
	else
	{
		LoadString (hInstance, IDS_NCHARGES, message, sizeof(message));
		_stprintf(charges, message, lmitem.Charges());
	}
	if (lmitem.NumFields() == 1)
		LoadString (hInstance, IDS_1FUNCTION, num_functions, sizeof(num_functions));
	else
	{
		LoadString (hInstance, IDS_NFUNCTIONS, message, sizeof(message));
		_stprintf(num_functions,message,lmitem.NumFields());
	}

	_stprintf(message, disp_message, this->Name(), num_functions, charges);
	display->DisplayMessage (message, false);

	for (i=0; i<lmitem.NumFields(); i++)
	{	// now identify the individual funtions
		function = this->ItemFunction(i);
		function_descrip[0] = '\0'; // initialize
		state = lmitem.StateField(i);
		offset = 1; // 1st byte is always the type
		// fill in function descrip based on the various fields
		for (j=0; j<LyraItem::FunctionEntries(function); j++)
		{
			value = 0;
			int size = LyraItem::EntrySize(function, j);
			buffer = new char[LyraItem::EntrySize(function, j)];
			memcpy(buffer, (char*)state + offset , LyraItem::EntrySize(function, j));
			value = (int)(*buffer);
			offset += LyraItem::EntrySize(function, j);
			if (LyraItem::EntryIdentifiable(function,j))
			{	// translatevalue sticks output into message
				TranslateValue(LyraItem::EntryTranslation(function,j), value);
				_stprintf(entry_descrip, _T(" %s : %s;"),LyraItem::EntryName(function,j),message);
				_tcscat(function_descrip, entry_descrip);
			}
			delete buffer;
		}
		LoadString (hInstance, IDS_IDENTIFY_FUNCTION, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, i+1,LyraItem::FunctionName(function), function_descrip);
		display->DisplayMessage (message, false);
	}

	return true;
}


// this method absorbs damage that the player would take, if
// applicable, and returns the amount of damage remaining
int cItem::AbsorbDamage(int amount)
{
	lyra_item_armor_t armor;
	int extra_damage = -amount; // damage not absorbed
	int absorbed;
	float percent_absorbed;
	const void* state;

	if (status != ITEM_OWNED)
		return amount;

	for (int i=0; i<this->NumFunctions(); i++)
		if (this->ItemFunction(i) == LyraItem::ARMOR_FUNCTION)
		{
			state = lmitem.StateField(i);
			memcpy(&armor, state, sizeof(armor));
			//_tprintf("checking func %d on item %d; curr = %d, max = %d\n",i,this,armor.curr_durability,armor.max_durability);
			percent_absorbed = ((float)armor.absorption)/100;
			absorbed = (int)(extra_damage * percent_absorbed);
			if (absorbed > armor.curr_durability)
				absorbed = armor.curr_durability;
			if (absorbed)
			{
				armor.curr_durability -= absorbed;
				extra_damage -= absorbed;
				lmitem.SetStateField(i, &armor, sizeof(armor));
				if (armor.curr_durability == 0)
					lmitem.SetCharges(0);
				needsUpdate = true;
				//_tprintf("cool! func %d on item %d absorbed %d damage; left = %d!\n",i,this,absorbed,armor.curr_durability);
			}
		}
	return -extra_damage;
}

// this method attempts to repair armor; it returns true
// if armor was repaired, or false otherwise
bool cItem::Reweave(int amount)
{
	lyra_item_armor_t armor;
	int repair_left = amount; // amount of repair not used
	int repaired;
	const void* state;
	bool is_armor = false;

	if (status != ITEM_OWNED)
		return false;

	for (int i=0; i<this->NumFunctions(); i++)
		if (this->ItemFunction(i) == LyraItem::ARMOR_FUNCTION)
		{
			is_armor = true;
			state = lmitem.StateField(i);
			memcpy(&armor, state, sizeof(armor));
			if (armor.curr_durability < armor.max_durability)
			{
				repaired = repair_left;
				if (repaired > (armor.max_durability - armor.curr_durability))
					repaired = (armor.max_durability - armor.curr_durability);
				if (repaired)
				{
					armor.curr_durability += repaired;
					repair_left -= repaired;
					lmitem.SetStateField(i, &armor, sizeof(armor));
					needsUpdate = true;
					//_tprintf("cool! func %d repaired %d damage!curr = %d, max = %d\n",i,repaired,armor.curr_durability,armor.max_durability);
				}
				LoadString (hInstance, IDS_ARMOR_REPAIRED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, this->Name());
				display->DisplayMessage (message, false);
				if (armor.curr_durability == armor.max_durability)
				{
					LoadString(hInstance, IDS_SHIELD_MAX_NOW, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message, false);
				}
				return true;
			}
			else
			{
				LoadString (hInstance, IDS_SHIELD_MAX, disp_message, sizeof(disp_message));
				display->DisplayMessage (disp_message);
				return false;
			}
		}
	LoadString (hInstance, IDS_NOT_ARMOR, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message, false);
	return false;
}

bool cItem::IsAreaEffectItem(void)
{
	int item_func = ItemFunction(0);
	return (this != NO_ITEM && (Status() == ITEM_UNOWNED) &&
		(item_func == LyraItem::AREA_EFFECT_FUNCTION || item_func == LyraItem::PORTKEY_FUNCTION));
}
// return the amount of damage this missle does
// this is a total HACK --- insure field information is maintained
int cItem::MissleDamage(void)
{
	#define ITEM_FUNCTION_0 0
	#define ITEM_MISSLE_DAMAGE_FLD 2

	int damage = 0;
	int function,i;
	int offset = 1; // 1st byte is always the type
	const void* state;
	char *buffer;

	function = this->ItemFunction(ITEM_FUNCTION_0);
	state = lmitem.StateField(ITEM_FUNCTION_0);

	if (LyraItem::FunctionEntries(function) >= ITEM_MISSLE_DAMAGE_FLD)
	{
		for(i=0;i<ITEM_MISSLE_DAMAGE_FLD;i++)
			offset += LyraItem::EntrySize(function, ITEM_MISSLE_DAMAGE_FLD);
		buffer = new char[LyraItem::EntrySize(function, ITEM_MISSLE_DAMAGE_FLD)];
		memcpy(buffer, (char*)state + offset , LyraItem::EntrySize(function, ITEM_MISSLE_DAMAGE_FLD));
		damage = (int)(*buffer);
		delete buffer;
	}
	return damage;
}

// this method attempts to recharge an item; it always failed on
// artifacts & infinitely charged items. Chance of backfiring during recharge
// = 10% - sphere of skill.
bool cItem::Recharge(int plateaua)
{
	int i;
	if (this->NoReap() || (lmitem.Charges() >= (INFINITE_CHARGES - 1)) ||
		(status != ITEM_OWNED))
	{
		LoadString (hInstance, IDS_RECHARGE_FAILED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Name());
		display->DisplayMessage (message);
		return false;
	}

	bool immutable = true;
	for (i=0; i<this->NumFunctions(); i++)
		if (!LyraItem::FunctionImmutable(this->ItemFunction(i)) &&
			(this->ItemFunction(i) != LyraItem::ARMOR_FUNCTION))
			immutable = false;

	if (immutable) // can't recharge, so do nothing
	{
		LoadString (hInstance, IDS_NOTHING_HAPPENS, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return false;
	}

	int limit = RECHARGE_LIMIT;

	// find and use lowest" recharge
	for (i = 0; i<this->NumFunctions(); i++)
	{
		int function = this->ItemFunction(i);

		if (function == LyraItem::NOTHING_FUNCTION)
			limit = min(limit, 200);
		else if (function == LyraItem::MISSILE_FUNCTION && MissleDamage() == 0)
			limit = min(limit, RECHARGE_LIMIT);// charms
		else
			limit = min(limit, MaxChargesForFunction(function));
	}

	int soft_limit = limit - 1;
	// don't go up if we're already at the soft limit (1 less than the genned max)
	if (lmitem.Charges() >= soft_limit)
	{
		LoadString(hInstance, IDS_TALISMAN_MAXCHARGE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Name());
		display->DisplayMessage(message);
		return false;
	}

	int random = rand()%100;
	if (random < BASE_CHANCE_DESTRUCTION)
	{
		LoadString (hInstance, IDS_RECHARGE_DESTROYED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Name());
		display->DisplayMessage (message);
		lmitem.SetCharges(0);
		needsUpdate = true;
		if (gs)
			gs->UpdateServer();
		return true;
	}

	if (random < (BASE_CHANCE_OF_RECHARGE_DRAIN - plateaua))
	{ // item has been drained!
		LoadString (hInstance, IDS_RECHARGE_DRAINED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Name());
		display->DisplayMessage (message);
		int new_charges = lmitem.Charges() - RECHARGE_BACKFIRE;
		if (new_charges <0)
			new_charges = 0;
		lmitem.SetCharges(new_charges);
		needsUpdate = true;
		if (gs)
			gs->UpdateServer();
		return true;
	}

	int new_charges = lmitem.Charges() + rand()%plateaua + 1;

	// check if we exceed the limit and set the charges to the soft limit if we do
	if (new_charges >= limit)
	{
		new_charges = soft_limit;
		LoadString(hInstance, IDS_TALISMAN_RECHARGED_NOW, disp_message, sizeof(disp_message));
	}
	// successful normal recharge evoke 
	else
	{
		LoadString (hInstance, IDS_TALISMAN_RECHARGED, disp_message, sizeof(disp_message));
	}
	_stprintf(message, disp_message, this->Name());
	display->DisplayMessage (message);
	lmitem.SetCharges(new_charges);
	needsUpdate = true;
	return true;
}

// this method drains essence and adds to player stats
bool cItem::DrainEssence(int amount)
{
	lyra_item_essence_t essence;
	lyra_item_meta_essence_t meta_essence;
	lyra_item_meta_essence_nexus_t nexus;

	int drain;
	int multiplier = 1; // drain multiplier
	const void* state;

	if (status != ITEM_OWNED)
		return false;

	for (int i=0; i<this->NumFunctions(); i++)
		if (this->ItemFunction(i) == LyraItem::ESSENCE_FUNCTION)
		{	// each item can have only one essence function
			state = lmitem.StateField(i);
			memcpy(&essence, state, sizeof(essence));
			// mares drain dreamer essences and vice versa
#ifdef PMARE
			multiplier = 10;  // pmares get 10 points from draining a dreamer essence
			if ((essence.mare_type < Avatars::MIN_NIGHTMARE_TYPE) && (essence.strength > 0))
#else
			if ((essence.mare_type >= Avatars::MIN_NIGHTMARE_TYPE) && (essence.strength > 0))
#endif
			{ // can't enslaved/banished essence
				if (amount > essence.strength)
					drain = essence.strength;
				else
					drain = amount;

				player->SetCurrStat(player->SelectedStat(), drain*multiplier, SET_RELATIVE, player->ID());
				essence.strength -= drain;
				if (essence.strength == 0) // unsigned values can't go negative
				{
					LoadString (hInstance, IDS_ITEM_DISCHARGED, disp_message, sizeof(disp_message));
				_stprintf(message,disp_message,this->Name());
					display->DisplayMessage(message);
					this->Destroy();
					return true;
				}
				lmitem.SetStateField(i, &essence, sizeof(essence));
				needsUpdate = true;
				return true;
			}
		}

	if (this->ItemFunction(0) == LyraItem::META_ESSENCE_FUNCTION)
	{	// each item can have only one essence function
		state = lmitem.StateField(0);
		memcpy(&meta_essence, state, sizeof(meta_essence));
		if (amount > meta_essence.strength())
			drain = meta_essence.strength();
		else
			drain = amount; // can't destroy meta essence by over draining
		player->SetCurrStat(player->SelectedStat(), drain, SET_RELATIVE, player->ID());
		meta_essence.set_strength(meta_essence.strength() - drain);
		lmitem.SetStateField(0, &meta_essence, sizeof(meta_essence));
		needsUpdate = true;
		return true;
	}

	if (this->ItemFunction(0) == LyraItem::META_ESSENCE_NEXUS_FUNCTION)
	{
		state = lmitem.StateField(0);
		memcpy(&nexus, state, sizeof(nexus));
		if (amount > nexus.strength)
			drain = nexus.strength;
		else
			drain = amount;
		player->SetCurrStat(player->SelectedStat(), drain, SET_RELATIVE, player->ID());
		nexus.strength -= drain;
		if (nexus.strength <= 0) {
			this->Destroy();
			return true;
		}

		lmitem.SetStateField(0, &nexus, sizeof(nexus));
		needsUpdate = true;
		return true;
	}

	return false;
}

bool cItem::AddMetaEssence(int amount)
{
	lyra_item_meta_essence_t meta_essence;
	const void* state;

	if (status != ITEM_OWNED)
	{
		LoadString(hInstance, IDS_META_NOT_OWNED, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		return false;
	}

	if (this->ItemFunction(0) == LyraItem::META_ESSENCE_FUNCTION)
	{	// each item can have only one essence function
		state = lmitem.StateField(0);
		memcpy(&meta_essence, state, sizeof(meta_essence));

		meta_essence.set_strength(meta_essence.strength() + amount);
		lmitem.SetStateField(0, &meta_essence, sizeof(meta_essence));
		needsUpdate = true;
		return true;
	}

	return false;
}

// this method just drains meta essences.
bool cItem::DrainMetaEssence(int amount)
{
	lyra_item_meta_essence_t meta_essence;
	int drain;
	const void* state;

	if (status != ITEM_OWNED)
	{
		LoadString (hInstance, IDS_META_NOT_OWNED, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		return false;
	}

	if (this->ItemFunction(0) == LyraItem::META_ESSENCE_FUNCTION)
	{	// each item can have only one essence function
		state = lmitem.StateField(0);
		memcpy(&meta_essence, state, sizeof(meta_essence));
		
		if (amount > meta_essence.strength())
		{
			LoadString (hInstance, IDS_META_NOT_STRONG_ENOUGH, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
			return false;
		}
		else
			drain = amount; // can't destroy meta essence by over draining
		
		meta_essence.set_strength(meta_essence.strength() - drain);
		lmitem.SetStateField(0, &meta_essence, sizeof(meta_essence));
		needsUpdate = true;
		return true;
	}

	return false;
}

// this method attempts to place the ward based on the level->Vertices
bool cItem::PlaceWard(void)
{
	const void* state;
	lyra_item_ward_t ward;
	state = lmitem.StateField(0);
	memcpy(&ward, state, sizeof(ward));

	extra = FindLDef(ward.from_vert, ward.to_vert);
	if (extra == NULL)
	{
// REG 5/27/00 - Messenge sent to all who enter room with dropped pocket ward.
//		LoadString (hInstance, IDS_WARD_NOT_PLACED, disp_message, sizeof(disp_message));
//		display->DisplayMessage(disp_message);
		return false;
	}
	linedef *line; line = (linedef*)extra;
	// set position of ward to be right in front of teleportal
	float mid_x = (level->Vertices[line->from].x + level->Vertices[line->to].x)/2;
	float x_diff = level->Vertices[line->to].x - level->Vertices[line->from].x;
	float mid_y = (level->Vertices[line->from].y + level->Vertices[line->to].y)/2;
	float y_diff = level->Vertices[line->to].y - level->Vertices[line->from].y;
	float length = fdist(level->Vertices[line->from].x, level->Vertices[line->from].y,
					level->Vertices[line->to].x, level->Vertices[line->to].y);
	if (length == 0.0f)
		return false;

	//_tprintf("lx: %d ly: %d px: %d py: %d\n",(int)mid_x, (int)mid_y, (int)player->x, (int)player->y);

	mid_x -= WARD_OFFSET*y_diff/length;
	mid_y += WARD_OFFSET*x_diff/length;

	//_tprintf("len: %d xdiff: %d ydiff: %d\n", (int)length, (int)x_diff, (int)y_diff);
	//_tprintf("wx: %d wy: %d\n", (int)mid_x, (int)mid_y);

	int sec = FindSector(mid_x, mid_y, sector, true);

	if (sec == DEAD_SECTOR)
		return false;

	this->PlaceActor(mid_x, mid_y, 0, 0, SET_XHEIGHT, false);
	z += WARD_HEIGHT;
	flags = flags & ~ACTOR_NOCOLLIDE; // collide with actors
	draggable = false;
	this->SetGravity();

	return true; // placed ward successfully
}

bool cItem::SurviveLevelChange(void)
{
	if ((status == ITEM_OWNED) || (status == ITEM_GIVING))
		return true;
	else
		return false;
};


// Destructor

cItem::~cItem(void)
{
	if(cp)
	{
		if (this == cp->DragItem())
			cp->EndDrag();

		cp->DeleteItem(this); // won't do anything if not in inventory
		if (cp->SelectedItem() == this)
			cp->SetSelectedItem(NO_ITEM);
	}

///_tprintf("deleting item with id %d...\n",lmitem.ItemID());

	return;
}

// Check invariants

#ifdef CHECK_INVARIANTS
void cItem::CheckInvariants(int line, TCHAR *file)
{

}
#endif

// helper function to create new items
cItem* CreateItem(float i_x, float i_y, int i_angle, LmItem& i_lmitem,
				  unsigned __int64 i_flags, bool temp, DWORD ttl, TCHAR *description, 
				  int i_status)
{
	cItem *item;

	if ((i_status != ITEM_DUMMY) && cp->InventoryFull())
	{ // carrying too much stuff
		LoadString (hInstance, IDS_ITEM_TOOMUCH, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		return NO_ITEM;
	}

	item = new cItem(i_x, i_y, i_angle, i_lmitem, i_status,
		i_flags, temp, ttl);

	if (i_status != ITEM_DUMMY)
	{
		if (options.network && !temp && !options.welcome_ai)
			gs->CreateItem(item, ttl, description);
		else 
			item->SetStatus(ITEM_OWNED);
	}

	return item;
}



