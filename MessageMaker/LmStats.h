// LmStats.h  -*- C++ -*-
// $Id: LmStats.h,v 1.13 1997-12-05 14:51:26-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// player stats

#ifndef INCLUDED_LmStats
#define INCLUDED_LmStats

#include <stdio.h>

#include "LyraDefs.h"
#include "SharedConstants.h"

// base datatype

struct lyra_stats_t {
  char focus;                  // focus stat
  char orbit;                  // orbit (dependent on xp)
  char curr[NUM_PLAYER_STATS]; // current stats
  char max[NUM_PLAYER_STATS];  // max stats
  char rank[NUM_GUILDS];       // rank in each guild
  int xp;                      // experience points
  int xp_pool[NUM_GUILDS];     // XP pool in each guild
  int quest_xp_pool;		   // Quest XP pool
  int pps;					   // Personality points
  int pp_pool;		   // PP pool
};

// the class

class LmStats {

public:

  // xp-table selectors
  static int OrbitXPBase(int orbit);
  static int OrbitXPGain(int orbit);
  static int SphereXPBase(int sphere);
  static int OrbitFromXP(int xp);

  // validation
  static bool StatNumOK(int stat_num);
  static bool StatValueOK(int stat_val);
  static bool GuildNumOK(int guild_num);
  static bool GuildRankOK(int rank);
  static bool OrbitOK(int orbit);

  // used for Personality Points
  static int StatAtLevel(int focus, int stat, int orbit);
  static int StatIncreaseCost(int focus, int stat, int orbit, int curr_stat_value);
  static int SphereIncreaseCost(int sphere);
  static int TrainPPCost(int art, int level, int min_orbit, int multiplier);
  static int EvokePPCost(int art, int level, int min_orbit, int multiplier);
  static int XPPPCost(int current_xp);

  // other
  static int TokensToDemote(int rank);

public:

  LmStats();

  void Init();

  // selectors
  int XP() const;
  int FocusStat() const;
  int MaxStat(int stat_num) const;
  int CurrentStat(int stat_num) const;
  int GuildRank(int guild_num) const;
  int PoolXP(int guild_num) const;
  int QuestPoolXP() const;
  int PP() const;
  int PPPool() const;

  int Orbit() const;
  int Sphere() const;
  int NumGuildsIn() const;
  bool IsInGuild(int guild_num) const;
  bool IsKnight() const;
  bool IsRuler() const;

  bool CanSetXP(int xp) const;
  bool CanSetFocusStat(int stat_num) const;
  bool CanSetMaxStat(int stat_num, int stat_val) const;
  bool CanSetCurrentStat(int stat_num, int stat_val) const;
  bool CanSetGuildRank(int guild_num, int rank) const;
  bool ReadyToAdvance(int sphere) const;

  // mutators
  void SetXP(int xp);
  void SetFocusStat(int stat_num);
  void SetMaxStat(int stat_num, int stat_val);
  void SetCurrentStat(int stat_num, int stat_val);
  void SetGuildRank(int guild_num, int rank);
  void SetPoolXP(int guild_num, int xp);
  void UsePoolXP(int guild_num, int xp);
  void SetQuestPoolXP(int xp);
  void UseQuestPoolXP(int xp);
  void SetPP(int pp);
  void SetPPPool(int pp);
  void UsePPPool(int pp);
  int AdjustXP(int adj);
  int PMareAdjustXP(int adj);
  void AdvanceToSphere(int sphere);
  void AdjustStat(int stat_num, int adj);

  // update stats (not xp) due to orbit gain/loss (of 1 only!)
  void GainOrbit(int new_orbit);
  void LoseOrbit(int new_orbit);

  // byteorder conversion
  void ConvertToHost();
  void ConvertToNetwork();

  int CheckIntegrity() const;
  void Dump(FILE* f, int indent = 0) const;

private:

  lyra_stats_t stats_;

};

#ifdef USE_INLINE
#include "LmStats.i"
#endif

#endif /* INCLUDED_LmStats */
