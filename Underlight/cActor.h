 // Header file for the Actor class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CACTOR_H
#define CACTOR_H

#include <windows.h>
#include "LyraDefs.h"
#include "Central.h"

//////////////////////////////////////////////////////////////////
// Constants

#define NO_ACTOR 0
const int STAND_INTERVAL = 500; // if an actor doesn't move for this long, set to frame 0

const int ANIMATION_TICKS=100; // speed of animation
const int MAX_COLORED_REGIONS=5; // max # of differently colored regions on an actor

class cNameTag;
class cMissile;

///////////////////////////////////////////////////////////////////
// Structures 

///////////////////////////////////////////////////////////////////
// Enumerations

enum set_z_type {
	SET_Z,
	SET_XHEIGHT,
	SET_NONE,
};

enum actor_type {
  GENERIC_ACTOR = 0,      // generic actor, no subclass
  NEIGHBOR = 1,           // neighbor subclass
  MISSILE = 2,            // missile subclass
  PLAYER = 3,			  // this actor is the player
  SENDING = 4,		      // local monster
  ITEM = 5,				  // item
  ORNAMENT=6,             // ornamental visual effect  
};

///////////////////////////////////////////////////////////////////
// Flags

// flags settable in editor
const unsigned __int64  ACTOR_ITEMGENERATOR	  = 0x0000000000000001;
const unsigned __int64  ACTOR_AGENTGENERATOR  = 0x0000000000000002;
const unsigned __int64  ACTOR_NOCOLLIDE		  = 0x0000000000000004;
const unsigned __int64  ACTOR_CEILHANG		  = 0x0000000000000008; 

// these can be reused for player/neighbor effects
const unsigned __int64	ACTOR_ENTRYPOINT      = 0x0000000000000010;
const unsigned __int64	ACTOR_RECALLPOINT     = 0x0000000000000020;



// flags set locally only on the player
// flags set on any actor locally
const unsigned __int64  ACTOR_PEACE_AURA	  = 0x0000000000000010;
const unsigned __int64  ACTOR_BLENDED		  = 0x0000000000000020;
const unsigned __int64  ACTOR_RECALL		  = 0x0000000000000040;
const unsigned __int64  ACTOR_SOUL_SHIELDED	  = 0x0000000000000080;
// flags set over the network on any actor
const unsigned __int64	ACTOR_SOULSPHERE	  = 0x0000000000000100;
const unsigned __int64	ACTOR_INVISIBLE		  = 0x0000000000000200;
const unsigned __int64  ACTOR_CHAMELED		  = 0x0000000000000400;
const unsigned __int64  ACTOR_CURSED		  = 0x0000000000000800;
// flags set locally only on the player
const unsigned __int64  ACTOR_RETURN		  = 0x0000000000001000;
const unsigned __int64  ACTOR_TRANSFORMED	  = 0x0000000000002000;
const unsigned __int64  ACTOR_REGENERATING	  = 0x0000000000004000;
const unsigned __int64  ACTOR_REFLECT		  = 0x0000000000008000;
const unsigned __int64  ACTOR_TRAILING		  = 0x0000000000010000;
const unsigned __int64	ACTOR_BLINDED		  = 0x0000000000020000;
const unsigned __int64  ACTOR_DEAFENED        = 0x0000000000040000;
const unsigned __int64  ACTOR_DRUNK           = 0x0000000000080000;
const unsigned __int64  ACTOR_SCARED          = 0x0000000000100000;
const unsigned __int64  ACTOR_PARALYZED       = 0x0000000000200000;
const unsigned __int64  ACTOR_POISONED		  = 0x0000000000400000;
const unsigned __int64	ACTOR_DETECT_INVIS	  = 0x0000000000800000;
const unsigned __int64  ACTOR_MIND_BLANKED	  = 0x0000000001000000;
const unsigned __int64  ACTOR_DETECT_CURSE    = 0x0000000002000000;
const unsigned __int64  ACTOR_NO_PARTY		  = 0x0000000004000000;
const unsigned __int64  ACTOR_PROT_FEAR		  = 0x0000000008000000;
const unsigned __int64  ACTOR_PROT_CURSE	  = 0x0000000010000000;
const unsigned __int64  ACTOR_FREE_ACTION	  = 0x0000000020000000;
const unsigned __int64  ACTOR_MEDITATING      = 0x0000000040000000;
const unsigned __int64  ACTOR_SOULEVOKE		  = 0x0000000080000000;

// having run out of 32 bit actor flags, we now have 64 flag bits
const unsigned __int64  ACTOR_NO_POISON		  = 0x000000000000000100000000;
const unsigned __int64  ACTOR_SPIN			  = 0x000000000000000200000000;
const unsigned __int64  ACTOR_BLEED			  = 0x000000000000000400000000;
const unsigned __int64	ACTOR_CRIPPLE		  = 0x000000000000000800000000;
const unsigned __int64	ACTOR_SHIELD		  = 0x000000000000001000000000;
const unsigned __int64	ACTOR_GKSHIELD		  = 0x000000000000002000000000;
//////////////////////////////////////////////////////////////////
// Class Definition
struct linedef;

class cArtFX
{
	int effect_id;
	short color_table[MAX_COLORED_REGIONS];
	int animate_ticks;
	int current_frame;
	bool active;
	bool evoking;
	int harmful;
public:

	cArtFX();
	bool  Update();
	void  Activate(int art_id, bool _evoking);
	void  Activate(bool _harmful, int primary_color, int secondary_color, bool _evoking);
	void  DeActivate(void);

	//Selectors
	bool   Active() { return active; };
	int	   CurrentBitmap() { return effect_id + current_frame;};
	short *ColorTable() { return color_table;};
	
	int    Harmful() { return harmful;};
	bool   Evoking()      {return evoking;};
	int    MainColor()    {return color_table[0];};
    int    SecondColor()  {return color_table[1];};
};




class cActor
   {
   friend class cActorList;

   public:
		__int64 flags; // see above for flag definitions
		float	x,x1,x2; // x pos and x coords for actor line
		float	y,y1,y2; // y pos and y coords for actor line
		float	z;
		int		angle; 
		float	velocity;
		float	vertforce;
		float	eyeheight;

		int     currframe,frames,views; 
		float	physht;
		float	halfwit;
		int		sector; 
		linedef *l;
		int		animate_ticks; // msecs since last animation

		cActor *next; // front & back pointers in actor list
		cActor *prev;

   protected:
		int			palette;					// palette # to use
		int			bitmap_id;				// bitmap id
		int			type;          
		float			fall_height;			// track highest point for falls
		DWORD			last_move;				// time of last actor movement
		cNameTag*	name_tag;
		int			num_color_regions;
		bool			colorized;				// actor has color regions
		short			colors[MAX_COLORED_REGIONS]; // color regions
		cMissile*	missile;					// missile to be launched
		int			missile_launch_time; // time to launch the missile
		bool			terminate;				// delete on next frame update
		cArtFX		evokingFX, evokedFX;
		bool			forming, dissolving; // going to/from ss?
		bool			entering;				// entering room? 

   public:
		cActor(float act_x, float act_y, int act_angle,  
		      unsigned __int64 act_flags=0, int act_type=GENERIC_ACTOR, int act_sector = DEAD_SECTOR); 
		virtual ~cActor(void);
		void PlaceActor(float new_x, float new_y, float new_z, int new_angle, 
			int set_z, bool set_sector_anywhere);
		void ModifyHeight(void);
		float SetXHeight(void);

		// 4DX-specific methods
		bool SetBitmapInfo(realmid_t effect_id);
		int FacingAngle(float p_x, float p_y);
		virtual int CurrBitmap(int view_angle);  // current bitmap for actor
		virtual int IconBitmap(void);            // bitmap for icon view of actor
		int CurrentView(int view_angle);
		  
  		virtual bool Render(void); 
		virtual bool Update(void)=0;   // Update movement, etc. returns TRUE normally,FALSE if object should be deleted
		virtual bool LeftClick(void);    
		virtual bool RightClick(void);
		virtual bool SurviveLevelChange(void)=0;

		cArtFX &EvokingFX() { return evokingFX; };
		cArtFX &EvokedFX() { return evokedFX; };

		  // Selection Functions
		inline int Type		(void) { return type; };
		inline int BitmapID	(void) { return bitmap_id; };
		virtual int Palette	(void);
		inline bool Terminate(void) { return terminate; };
		inline bool Forming(void) { return forming;};
		inline bool Dissolving(void) { return dissolving;};
   	    inline bool Entering(void) { return entering;};

		inline int NumRegions(void) { return num_color_regions; };
		inline short Color(int region) { return colors[region]; };
		virtual short *ColorRegions(void); 
		virtual bool Colorized (void);
		virtual void CheckMissile(void);
		bool InWater(void);

		// Mutator Functions
		inline void SetColor(int region, int value) { colors[region] = value; };
		inline cNameTag *NameTag(void) { return name_tag;}; 
		inline cActor* Next (void) { return next; };
		inline cActor* Prev (void) { return prev; };
		inline void SetTerminate(void) { terminate = true; };
		virtual void SetMissile(cMissile *new_missile);

		// Syntactic Sugar
		inline bool IsPlayer (void) { return (type == PLAYER); };
		inline bool IsNeighbor (void) { return (type == NEIGHBOR); };
		inline bool IsMissile (void) { return (type == MISSILE); };
		inline bool IsSending (void) { return (type == SENDING); };
		inline bool IsItem (void) { return (type == ITEM); };
		inline bool IsOrnament (void) { return (type == ORNAMENT); };

		virtual bool IsFemale (void) { return false; }
		virtual bool IsMale (void) { return false; }
		virtual bool IsMonster (void) { return false; }

   protected:

   private:
		// copy constructor and assignment operator are
		// private and undefined -> errors if used
		cActor(const cActor& x);
		cActor& operator=(const cActor& x);

#ifdef CHECK_INVARIANTS
		void CheckInvariants(int line);
#else
		inline void CheckInvariants(int line) {};
#endif

   };
#endif



