// Header file for Player class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CPLAYER_H
#define CPLAYER_H

#include "Central.h"
#include <windows.h>
#include "SharedConstants.h"
#include "LmAvatar.h"
#include "cArts.h" 
#include "cActor.h"
#include "cLevel.h"
#include <limits.h>

//////////////////////////////////////////////////////////////////
// Constants

const lyra_id_t SERVER_UPDATE_ID = 0; // for stats/skills updated by server
const lyra_id_t REFLECT_ID = UINT_MAX; // to be used as caster_id for reflected arts
const int MAX_ORBIT = 5;
const float MAXWALK=6.0f; 
const float MAXSTRAFE=4.0f;
const float	SHAMBLE_SPEED=.85f; // for monsters
const float	WALK_SPEED=1.0f;
const float RUN_SPEED=1.5f;
// track up to 64 collapses in the last 20 minutes to detect cheating
const int COLLAPSES_TRACKED = 64;
const int COLLAPSE_TRACK_INTERVAL = 1000*60*5; // 20 minutes, in ms
const int COLLAPSE_CHEAT_THRESHHOLD = 5;

enum set_stat_enum
{  
  SET_ABSOLUTE = 1,    
  SET_RELATIVE = 2,          
};

// for showing guild/sphere/shield patches
enum show_patch_enum
{  
  DONT_SHOW = 0,
  SHOW_FRONT = 1,    
  SHOW_BACK  = 2          
};

enum strafing_t 
{
  NO_STRAFE = 0,
  STRAFE_LEFT = 1,
  STRAFE_RIGHT = 2,
};

///////////////////////////////////////////////////////////////////
// Structures 

struct avatar_poses_t;

struct player_stat_t 
{
	int		current;	// current natural stat value
	int		max;		// max natural stat value
	bool    current_needs_update;
	TCHAR	name[16];	// stat name
	int		checksum;
};

struct player_skill_t
{
	int		skill;
	bool	needs_update;
	int		checksum;
};

struct player_guild_t
{
	int		rank;
	int		xp_award_pool;
	int		checksum;
};

struct player_collapse_t
{
	lyra_id_t	collapser_id;
	DWORD		time;
};

//////////////////////////////////////////////////////////////////
// Class Definition

class cPlayer : public cActor
{
   friend class cActorList; 

   public:

	   enum { INVALID_PLAYERID = -1 };

   protected:
	   lyra_id_t playerID;
	   TCHAR upper_name[Lyra::PLAYERNAME_MAX];
	   LmAvatar avatar;
	   LmAvatar transformed_avatar; // temporary avatar
	   int start_angle, start_level, return_angle;
	   int last_level, return_level, recall_angle, recall_level;
	   float start_x, start_y, return_x, return_y;
	   float last_x, last_y, recall_x, recall_y;
	   bool last_loc_valid;
	   short room; // current room the player is in
	   int xp, orbit;
	   int step_frame;
	   float speed; // 1=walking, 1.5=running
	   float reset_speed;  
	   int num_items;
	   float turnrate;
	   int strafe;
	   bool injured;
	   bool jumped;
	   bool waving;
	   bool attempting_teleport;
	   DWORD last_footstep;
	   int focus_stat;
	   int selected_stat;
	   unsigned __int64 player_flags;
	   int free_moves;
	   bool teleporting; 
	   cItem *active_shield;
	   avatar_poses_t *avatar_poses;
	   bool old_sanct;
	   bool checksum_incorrect;
	   lyra_id_t last_party_leader;
	   unsigned int collapse_time;
	   bool item_flags_sorting_changed;
	   lyra_id_t gamesite_id;
	   unsigned char gamesite;
	   long session_id; // for multiplayer.com
	   lyra_id_t last_attacker_id; // ID of last neighbor to hit us
	   
	   player_guild_t guild_ranks[NUM_GUILDS];
	   player_skill_t skills[NUM_ARTS];
	   player_stat_t stats[NUM_PLAYER_STATS];
	   int quest_xp_pool;
	   int ppoints;
	   int pp_pool;
	   int granting_pp;

	   int next_collapse_index;
	   player_collapse_t collapses[COLLAPSES_TRACKED]; // record last 100 collapses
	   lyra_id_t last_poisoner,last_bleeder;
	   DWORD next_heal,next_poison,next_bleed,next_trail,next_nightmare_check,next_regeneration,next_sector_tag;

	   int   vertical_tilt;
	   int   vertical_tilt_origin;
	   float vertical_tilt_float;
	   float max_vertical_tilt;

		 bool using_blade;
		 int  blade_position_index;
		 int  blade_ticks;
		 
		 bool hit;

		float tportx, tporty; // for retrying failed teleports
		int tport_angle, tport_level, tport_sound;
    lyra_id_t channelTarget;
		lyra_id_t lastChannelTarget;
        
   public:
      cPlayer(short viewport_height);
	  virtual ~cPlayer(void);
	  void InitPlayer(void);
	  void SetStartPos(float starting_x, float starting_y, int starting_angle, int starting_level);
	  void ReformAvatar(void); // reform after being a soulsphere
	  void PerformedAction(void); // cancel meditation, etc.
	  void ValidateChecksums(void); // check for stat tampering

	  int curse_strength; // Curse Strength public field keeps track of art failure rate
	  int blast_chance; // Blast Chance tracks the chance an Ago will reciprocate your Blast
	  int poison_strength;
	  int reflect_strength;
	  int cripple_strength;
	  int avatar_armor_strength;
	  
	  // Selection Functions
	  virtual TCHAR* Name(void);
	  virtual TCHAR* Password(void);
	  bool IsMare(void);
	  inline TCHAR* UpperName(void) { return upper_name; };
	  inline LmAvatar Avatar(void) { return avatar; };
	  inline LmAvatar TransformedAvatar(void) { return transformed_avatar; };
	  inline short AvatarType(void) { return avatar.AvatarType(); };
	  inline short Room(void) { return room; };
	  inline int XP(void) { return xp; };
	  inline int Orbit(void) { return orbit; };
	  inline int Sphere(void) { return (int)(orbit/10); };
	  inline float Speed(void) { return speed; };
	  inline lyra_id_t ID(void) { return playerID; };
	  inline float StartX (void) { return start_x; };
	  inline float StartY (void) { return start_y; };
	  inline int  StartAngle (void) { return start_angle; };
	  inline int  StartLevel (void) { return start_level; };
	  inline float ReturnX (void) { return return_x; };
	  inline float ReturnY (void) { return return_y; };
	  inline int  ReturnAngle (void) { return return_angle; };
	  inline int  ReturnLevel (void) { return return_level; };
	  inline float RecallX (void) { return recall_x; };
	  inline float RecallY (void) { return recall_y; };
	  inline int  RecallAngle (void) { return recall_angle; };
	  inline float LastX (void) { return last_x; };
	  inline float LastY (void) { return last_y; };
	  inline int  LastLevel (void) { return last_level; };
	  inline int  Strafe (void) { return strafe;};
	  DWORD EffectExpire(int effect);
	  void DisplayTimedEffects(void);
	  inline bool Injured(void) { return injured; };
	  inline bool Teleporting(void) { return teleporting; };
	  inline bool FreeMoves(void) { return free_moves != 0; };
	  inline bool IsChannelling(void) { return channelTarget != 0 && channelTarget != playerID; };
	  inline lyra_id_t ChannelTarget(void) { return channelTarget; };
	  inline lyra_id_t LastChannelTarget(void) { return lastChannelTarget; };
	  inline int FocusStat(void) { return focus_stat; };
	  inline int SelectedStat(void) { return selected_stat; };
	  int Skill(int art_id);
	  inline int SkillSphere(int art_id) { return (int)(Skill(art_id)/10); };
	  inline bool SkillNeedsUpdate(int art_id) { return skills[art_id].needs_update; };
	  int HeightDelta (void);
	  int NumGuilds(int rank);
	  inline int QuestXPPool(void) { return quest_xp_pool; };
	  inline int PPPool(void) { return pp_pool; };
	  inline int PPoints(void) { return ppoints; };
	  inline int GrantingPP(void) { return granting_pp;};
	  inline bool NeedItemFlagsOrSortingUpdate(void) { return item_flags_sorting_changed;};

	  unsigned int GetMonsterType (void);
	  unsigned int GetTransformedMonsterType (void);

	  bool IsUninitiated(void);
	  bool IsInitiate(int guild_id=Guild::NO_GUILD);
	  bool IsKnight(int guild_id=Guild::NO_GUILD);
	  bool IsRuler(int guild_id=Guild::NO_GUILD);

	  bool IsFemale (void);
	  bool IsMale (void);
	  bool IsMonster (void);
	  bool IsPossesed (void);
	  bool IsPMare (void); 
	  bool IsDreamerAccount (void);

	  unsigned char GuildFlags (int rank);
	  inline int GuildRank(int guild_id) { return guild_ranks[guild_id].rank; };
	  inline int GuildXPPool(int guild_id) { return guild_ranks[guild_id].xp_award_pool; };
	  inline int CurrStat(int stat) { return stats[stat].current; }; 
	  inline int MaxStat(int stat) { return stats[stat].max; };
	  inline int CurrStatNeedsUpdate(int stat) { return stats[stat].current_needs_update; }; 
	  inline TCHAR* StatName(int stat) { return stats[stat].name; };
	  inline int VerticalTilt(void) { return vertical_tilt; };
	  inline bool Jumped(void) { return jumped; }; 
	  inline int GuildPatchID(void) { return avatar.GuildID(); };
	  inline int GuildRankID(void) { return avatar.GuildRank(); };
	  inline int ShowGuildID(void) { return avatar.ShowGuild(); };
	  inline int HeadID(void) { return avatar.Head(); };
	  inline int ShowSphereID(void) { return avatar.ShowSphere(); };
	  inline int SphereID(void) { return avatar.Sphere(); };
	  int TeacherID(void);
	  inline cItem* ActiveShield(void) { return active_shield; };
	  bool ActiveShieldValid(void);
	  inline bool Waving(void) { return waving; }; 
	  inline lyra_id_t LastLeaderID(void) { return last_party_leader;};
	  inline bool IsTeacher(void) { return Skill(Arts::TRAIN) || Skill(Arts::LEVELTRAIN);};
	  inline bool IsApprentice(void) { return !IsTeacher() && Skill(Arts::QUEST); };
	  inline bool IsDreamSmith(void) { return Skill(Arts::DREAMSMITH_MARK);}; 
	  inline bool IsWordSmith(void) { return Skill(Arts::WORDSMITH_MARK);}; 
	  inline bool IsDreamstriker(void) { return Skill(Arts::DREAMSTRIKE);}; 
	  inline bool LastLocValid(void) { return last_loc_valid; };
   	  short  CurrentAvatarType(void);
   	  unsigned int GetAccountType (void);	
	  inline unsigned char Gamesite() const { return gamesite; };
	  inline lyra_id_t GamesiteID() const { return gamesite_id; };
	  inline long SessionID() const { return session_id; };
	  inline bool AttemptingTeleport() const { return attempting_teleport; };

		void  SetUsingBlade(bool new_val) { using_blade = new_val; };
		bool  UsingBlade(void) {return using_blade;};
		POINT BladePos(void);

		bool Hit() { return hit; };
		void  SetHit(bool value) { hit = value;};

		bool CanUseChatMacros();

	  // Modify Member Functions
	  void SetRoom(float last_x, float last_y); 
	  inline void SetID(lyra_id_t new_id) { playerID = new_id;}; 
	  int SetSkill(int art_id, int value, int how, lyra_id_t origin_id, bool initializing=false);
	  int SetCurrStat(int stat, int value, int how, lyra_id_t origin_id);
	  int SetMaxStat(int stat, int value, lyra_id_t origin_id);
	  inline void SetCurrStatNeedsUpdate(int stat, bool value) { stats[stat].current_needs_update = value; }; 
	  virtual int SetXP(int value, bool initializing=false);
	  void SetGuildRank(int guild_id, int value);
	  inline void SetGuildXPPool(int guild_id, int value) { guild_ranks[guild_id].xp_award_pool = value; };
	  virtual void SetAvatar(LmAvatar new_avatar, bool update_server);
	  void InitAvatar(void);
	  void SetTransformedAvatar(LmAvatar new_avatar);
	  void SetFocusStat(int stat) { focus_stat = stat; };
	  void SetSelectedStat(int stat) { selected_stat = stat; };
	  bool SetTimedEffect(int effect, DWORD duration, lyra_id_t caster_id, int effect_origin);
	  void ApplyAvatarArmor(int art_level, int sm_plat, lyra_id_t caster_id);
	  void ApplyCrippleEffect(int pmsg, int art_level, int fs_plat, lyra_id_t caster_id);
	  void RemoveTimedEffect(int effect);
	  inline void SetInjured(bool value) { injured = value; };
	  inline void SetChannelTarget(lyra_id_t value) { channelTarget = value; };
		inline void SetLastChannelTarget(lyra_id_t value) { lastChannelTarget = value; };
	  inline void SetTeleporting(bool value) { teleporting = value; };
	  inline void SetFreeMoves(int value) { free_moves = value; };
	  inline void SetSkillNeedsUpdate(int art_id, bool value) { skills[art_id].needs_update = value; };
	  inline void SetReturn(float x, float y, int angle, int level_id) { return_x = x; return_y = y; return_angle = angle; return_level = level_id; };
	  inline void SetRecall(float x, float y, int angle, int level_id) { recall_x = x; recall_y = y; recall_angle = angle; recall_level = level_id; };
	  inline void SetJumped(bool value) { jumped = value; }; 
	  void SetHeadID(int value, bool update = true);
	  bool SetActiveShield(cItem *value);
	  inline void SetWaving(bool value) { waving = value; }; 
	  inline void SetLastLeaderID(lyra_id_t value) { last_party_leader = value;};
	  inline void SetLastLocValid(bool value) { last_loc_valid = value; };
	  void MarkLastLocation(void);
	  inline void SetQuestXPPool(int value) { quest_xp_pool = value; };
	  inline void SetPPPool(int value) { pp_pool = value; };
	  inline void SetPPoints(int value) { ppoints = value; };
	  inline void AddGrantingPP(void) { granting_pp++; };
	  inline void RemoveGrantingPP(void) { if (granting_pp > 0) granting_pp--; };
	  inline void SetItemNeedFlagsOrSortingUpdate (bool value) { item_flags_sorting_changed = value;};
	  inline void SetGamesite(unsigned char gamesite_) { gamesite = gamesite_; };
	  inline void SetGamesiteID(lyra_id_t gamesite_id_) { gamesite_id = gamesite_id_; };
	  inline void SetLastAttackerID(lyra_id_t attacker_id) { last_attacker_id = attacker_id; };
	  inline void SetSessionID(long session_id_) { session_id = session_id_; };

	  // Other
	  void ResetEyeHeight(void);
	  virtual void Dissolve(lyra_id_t origin_id, int talisman_strength = 1); // player has suffered avatar death
	  inline bool SurviveLevelChange(void) { return TRUE;};
	  int  IconBitmap(void); // overloaded for avatar display
	  bool NightmareAttack(lyra_id_t target = 0);
	  bool Update(void); // run every frame
	  bool RetryTeleport(void);
	  bool Teleport(float x, float y, int facing_angle, int level_id = NO_LEVEL, 
		            int sound_id = LyraSound::TELEPORT, bool retry = false);

   protected:
	  void CheckStatus(void); // expire special timed effects, heal, etc.
	  DWORD CalculateBreakthrough(DWORD duration, int effect_origin);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cPlayer(const cPlayer& x);
	cPlayer& operator=(const cPlayer& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif



   };

#endif






