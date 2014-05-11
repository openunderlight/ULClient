// Header file for the Missile class - derived from the Actor class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CMISSILE_H
#define CMISSILE_H

#include "Central.h"
#include "SharedConstants.h"
#include "4dx.h"
#include "cActor.h"
#include "cItem.h"

///////////////////////////////////////////////////////////////////
// Constants


///////////////////////////////////////////////////////////////////
// Helper Functions

bool LegalMissilePosition(float x, float y, int angle, int sector);

class cItem;


//////////////////////////////////////////////////////////////////
// Class Definition

class cMissile : public cActor
{
   friend class cActorList;
   public:

   protected:
		int			bitmap_id;
		int			ticks; // # of ticks passed since creation
		int			height_delta; // z angle of movement
		bool			activated; // has missile been activated yet?
		bool			expired; // true when missile has expired
		bool			bouncing; // true for missiles that bounce off walls
		bool			returning; // true for missiles that return to the owner
		bool			melee; // true for melee attacks; determined by bitmap type
		cActor*		owner; // don't ever dereference without validating!
		cItem*		counterpart; // for missiles that have an item counterpart
		realmid_t	owner_id; // use to dereference owner, if needed
		realmid_t	effect_id;
		int			expire_ticks;
		int			effect; // effect to cause on striking actor 
		int			damage_type; // damage category 
		int			state; // missile type dependant state
		int			velocity;
		int			skill; // skill of caster - used for to hit/damage
		bool			invisible; // for push missiles & other invisibles
		int			art_id;

   public:
	   cMissile(cActor *m_owner, int m_bitmap_id, int angle, int m_height_delta, 
				int m_velocity, int m_effect, int m_damage_type, 
				cItem *m_counterpart = NO_ITEM, unsigned __int64 flags=0,
				int m_sector = DEAD_SECTOR, int m_art_id = Arts::NONE); 
	   ~cMissile(void);
	   virtual bool Render(void);
	   void Activate(void);
	   bool Collision(int collision_type, linedef *l = NULL);
	   inline int HeightDelta(void) { return height_delta;};
	   inline cActor* Owner(void) { return owner;};
	   inline int Bitmap_ID(void) { return bitmap_id;};
	   inline bool SurviveLevelChange(void) { return FALSE; };
	   inline bool Bouncing(void) { return bouncing; };
	   inline bool Returning(void) { return returning; };
	   inline bool Activated(void) { return activated; };
	   inline bool Melee(void) { return melee; };
	   inline bool Invisible(void) { return invisible; };
	   void SetHeightDelta(int hd) { height_delta = hd; };
	   void SetLaunchParams(void);

   protected:
	   void StrikeActor(cActor *actor);
   private:
	   virtual bool Update(void);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cMissile(const cMissile& x);
	cMissile& operator=(const cMissile& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR* file) {};
#endif

};
#endif

