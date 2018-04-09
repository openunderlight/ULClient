// Header file for cItem

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CITEM_H
#define CITEM_H

#include "Central.h"
#include "4dx.h"
#include "LyraDefs.h"
#include "GMsg_PutItem.h"
#include "LmItem.h"
#include "cActor.h"

//////////////////////////////////////////////////////////////////
// Constants

class cItem;
class cNeighbor;

#define NO_ITEM 0

const int NO_EXPLOSION_ORNAMENT = 0;
const int ITEM_DESCRIP_LENGTH = 48;
const unsigned char INFINITE_CHARGES = 255;

enum item_status {
	ITEM_UNOWNED = 1, // on the ground...
	ITEM_OWNED = 2,  // in the player's inventory
	ITEM_GETTING = 3, // in the process of getting
	ITEM_DROPPING = 4, // in the process of dropping
	ITEM_CREATING = 5, // in the process of creating
	ITEM_DESTROYING = 6, // in the process of destroying
	ITEM_GIVING = 7, // in the process of giving away
	ITEM_RECEIVING = 8, // in the process of being given
	ITEM_DUMMY = 9 // a dummy item that shouldn't be processed
};

enum item_inventory_flags {
	ITEM_IDENTIFIED = 0x01,
	ITEM_ACTIVE_SHIELD = 0x02
};


//////////////////////////////////////////////////////////////////
// Class Definition

// helper functions
cItem* CreateItem(float i_x, float i_y, int i_angle, LmItem& i_lmitem, 
				  unsigned __int64 i_flags, bool temp, DWORD ttl = GMsg_PutItem::DEFAULT_TTL,
				  TCHAR *description = NULL, int i_status = ITEM_CREATING);

class cItem : public cActor
{
   friend class cActorList;

   public:

   protected:
	   LmItem lmitem; // bitmap id, colors, flags, serial #, state descrip, state
	   int status; 
	   int selected_function; // current selected function (0-2)
	   TCHAR description[ITEM_DESCRIP_LENGTH]; // descrip at a distance
	   bool needsUpdate; // whether state has changed since last update
	   bool draggable; // true if can be dragged/picked up
	   bool destroy_at_zero; // destroy when state=0
	   bool marked_for_death; // true if picked up by someone else while we're getting it
	   bool marked_for_drop; // true for created items that should be dropped immediately
	   bool thrown; // true for items that can be thrown & return
	   bool temporary; // true for player created items with short lifespans
	   bool gravity; // should gravity act on the item?
	   bool	want_destroy_ack; // display message on destroy ack
	   bool redeeming;  // trying to redeem gratitude token?
	   DWORD expire_time; // time to expire temporary items
	   unsigned char icon[ICON_WIDTH*ICON_HEIGHT*BYTES_PER_PIXEL];
	   void *extra; // extra pointer; used for wards, etc.
	   int sort_index; // relative sort index for control panel
	   int inventory_flags;

   public:
	   cItem(float i_x, float i_y, int i_angle, const LmItem& i_lmitem, int i_status, 
		   unsigned __int64 i_flags = 0, bool temp = false, DWORD expires = 0,
		   int i_sector = DEAD_SECTOR); 
	  ~cItem(void);

	  bool WithinReach(cActor *actor); 
	  void DisplayCreateMessage(void);
	  void DisplayDropMessage(void);
	  void DisplayTakeMessage(void);
	  void DisplayGivenMessage(void);
	  void DisplayReceivedMessage(void);
		bool LeftClick(void);
	  bool RightClick(void);
	  bool Render(void);
	  void Use(void);
	  void DrainCharge(void);
	  void Drop(float drop_x, float drop_y, int drop_angle=0);
	  bool Destroy(void);
	  void Get(void);
	  bool Identify(bool from_art = false);
	  int AbsorbDamage(int amount);
	  bool Reweave(int amount);
	  bool Recharge(int plateau);
	  bool DrainEssence(int amount);
	  bool DrainMetaEssence(int amount);
	  bool AddMetaEssence(int amount);
	  void ApplyGratitude(cNeighbor* n);
	  bool SurviveLevelChange(void);
	  bool NoPickup(void);

	  // selectors
	  inline LmItemHdr& ID(void) { return lmitem.Header(); };
	  inline LmItem& Lmitem(void) { return lmitem; };
	  inline int Status(void) { return status; };
	  inline TCHAR *Describe(void) { return description; };
	  inline TCHAR *Name(void) { return lmitem.Name(); };
	  inline bool NeedsUpdate(void) { return needsUpdate;};
	  inline unsigned char* IconBits(void) { return (unsigned char*)icon; };
	  inline bool Draggable(void) { return draggable; };
	  inline void* Extra(void) { return extra; };
	  inline int SelectedFunction(void) { return selected_function; };
	  inline bool MarkedForDeath(void) { return marked_for_death; };
	  inline bool MarkedForDrop(void) { return marked_for_drop; };
	  inline bool Thrown(void) { return thrown; };
	  inline bool Temporary(void) { return temporary; };
	  inline int NumFunctions(void) { return lmitem.NumFields(); };
	  inline bool NoReap(void) { return lmitem.Header().Flags() & LyraItem::FLAG_NOREAP; };
	  inline bool AlwaysDrop(void) { return lmitem.Header().Flags() & LyraItem::FLAG_ALWAYS_DROP; };
	  inline int SortIndex(void) { return sort_index; };
	  inline int InventoryFlags(void) { return inventory_flags; };
	  inline bool WantDestroyAck(void) { return want_destroy_ack; };
	  inline bool Redeeming(void) { return redeeming; }; // for gratitude tokens
	  bool Losable(void);
	  int ItemFunction(int slot);
	  int MissleDamage(void);
	  bool IsAreaEffectItem(void);

	  // mutators
	  void SetStatus(int new_status);
	  inline void SetLmItem(const LmItem& new_lmitem) { lmitem = new_lmitem; };
	  inline void SetSelectedFunction(int new_function) { selected_function = new_function; };
	  inline void SetMarkedForDeath(bool value) { marked_for_death = value; };
	  inline void SetMarkedForDrop(void) { marked_for_drop = true; };
	  inline void SetThrown(bool value) { thrown = value; };
	  inline void SetNeedsUpdate(bool value) { needsUpdate = value;};
	  inline void SetInventoryFlags(int value) { inventory_flags = value; };
	  inline void SetWantDestroyAck(bool value) { want_destroy_ack = value; };
	  inline void SetRedeeming(bool value) { redeeming = false; }; // for gratitude tokens
	  void SetSortIndex(int value); 

   protected:

   private:
	  bool Update(void);
	  void SetGravity(void);
	  bool PlaceWard(void);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cItem(const cItem& x);
	cItem& operator=(const cItem& x);
	
#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};



#endif

