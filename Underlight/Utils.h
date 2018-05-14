// Header file for Utils.cpp

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef INCL_UTILS
#define INCL_UTILS

#include "Central.h"
#include "SharedConstants.h"
class cActor;

//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// Structures

struct modifier_t // type of damage/healing/etc. done by items
{
	short			base;
	unsigned char   die_type;
	unsigned char   num_dice;
	short			min_skill_to_use; // for weapons, min dart/blunt/edge skill to use
	short			min_skill_to_create; // min forge talisman skill to create this category
	UINT			descrip; // pointer to an entry in the string table
};

struct duration_t // duration of effects
{
	short			base; // base duration, in seconds
	short			random;   // (0-random) 1 sec modifiers
	short			min_skill_to_create; // min forge talisman skill to create this category
	UINT			descrip; // pointer to an entry in the string table
};

struct velocity_t // velocity types
{
	short			min_skill_to_create;
	UINT			descrip; // pointer to entry in string table
};


struct distance_t
{
	unsigned int cartesian_distance; // x,y diff
	unsigned int height_distance; // z diff
	short min_skill_to_create; 
	UINT descrip;
};

struct frequency_t
{
	int recurrences; // -1 for always recur
	int ms_between_recurrences; 
	UINT descrip;
};

struct freqtick_t
{
	int numrecurs;
	bool remove_with_room_change;
};

struct belief_t
{
	int string_id;
	int art_id;
};

class cTimedEffects { // timed effects
	public:	
	   DWORD expires[NUM_TIMED_EFFECTS];
	   TCHAR *start_descrip[NUM_TIMED_EFFECTS];
	   TCHAR *more_descrip[NUM_TIMED_EFFECTS];
	   TCHAR *expire_descrip[NUM_TIMED_EFFECTS];
	   TCHAR name[NUM_TIMED_EFFECTS][20];
	   __int64 actor_flag[NUM_TIMED_EFFECTS]; // holds the actor flag corresponding to the effect
	   short related_art[NUM_TIMED_EFFECTS];
	   short default_duration[NUM_TIMED_EFFECTS]; // duration for when caused by item
	   bool harmful[NUM_TIMED_EFFECTS]; // harmful or helful?
	   bool abjurable[NUM_TIMED_EFFECTS]; // abjurable?
	protected:

	public:
      cTimedEffects(void);
	  virtual ~cTimedEffects(void);

	protected:
	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cTimedEffects(const cTimedEffects& x);
	cTimedEffects& operator=(const cTimedEffects& x);

};



//////////////////////////////////////////////////////////////////
// Function Prototypes

void __cdecl CheckOptions(void);
bool __cdecl LoadGameOptions(void);
void __cdecl TimeOnline(int* minutes, int* seconds);
int CalculateModifier(int modifier);
int CalculateModifierMax(int modifier);
int CalculateModifierMin(int modifier);
int CalculateDuration(int index);
bool TranslateValue(int type, int value);
int PowerTokenCostToForge(int type, int value, bool combineItem);
bool CanPlayerForgeValue(int type, int value, int powerTokens);
int	MinModifierSkill(int value);
int MaxChargesForFunction(int function);
int NumberTranslations(int type);
TCHAR* GuildName(int guild_id);
int GuildFlag(int guild_id);
int	GuildID(int guild_flag);
int GuildLevel(int guild_id);
int LevelGuild(int level_id);
TCHAR* RankName(int rank);
TCHAR* RankGoalName(int rank);
TCHAR* MonsterColorName(int color);
TCHAR* ColorName(int color);
void ColorName(int color, TCHAR* buffer, int bufsize);
TCHAR* TokenName(int token_type);
TCHAR* __cdecl NightmareName(int id);
TCHAR* DreamweaponName(int color);
void Scream(int avatar_type, cActor* actor, bool propagate = false);
void CalculateDistance(int index, unsigned int* xydist, unsigned int* heightdist);
bool Within48Hours(SYSTEMTIME t1, SYSTEMTIME t2);
frequency_t Frequency(int freqidx);
int BeliefFromArtID(lyra_id_t art_id);

const char *DisperseCoordinate(unsigned int index);
unsigned int NumDisperseLocs(void);
unsigned int NumLocations(void);
const char *LocationNameAt(unsigned int index);
const char *LocationCoordinateAt(unsigned int index);
bool TeleportLocationAvailable(unsigned int index);

TCHAR *TalismanName(int bitmap_id);
unsigned int NumTalismans(void);

TCHAR *TalismanNameAt(unsigned int index);
int TalismanBitmapAt(unsigned int index);
bool TalismanForgable(unsigned int index);

DWORD __cdecl LyraTime(void);
void SetCounter(void);
bool LegalRoom(int roomnum); // true if a legal room
int WhichMonsterName(TCHAR* name);
void GetDSTTime(LPSYSTEMTIME dsttime);
void PrepareSrcRect(RECT* src, RECT* region, int stretch);
int MaxSkill(int art_id);

#endif
