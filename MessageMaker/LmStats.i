// LmStats.i  -*- C++ -*-
// $Id: LmStats.i,v 1.13 1997-11-20 16:27:09-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void LmStats::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE bool LmStats::StatNumOK(int stat_num)
{
  return INDEX_OK(stat_num, 0, NUM_PLAYER_STATS);
}

INLINE bool LmStats::StatValueOK(int stat_val)
{
  return RANGE_OK(stat_val, Stats::STAT_MIN, Stats::STAT_MAX);
}

INLINE bool LmStats::GuildNumOK(int guild_num)
{
  return INDEX_OK(guild_num, 0, NUM_GUILDS);
}

INLINE bool LmStats::GuildRankOK(int rank)
{
  return RANGE_OK(rank, Guild::RULER_PENDING, Guild::RULER);
}

INLINE bool LmStats::OrbitOK(int orbit)
{
  return RANGE_OK(orbit, Stats::ORBIT_MIN, Stats::ORBIT_MAX);
}

INLINE int LmStats::GuildRank(int guild_num) const
{
  return GuildNumOK(guild_num) ? stats_.rank[guild_num] : Guild::NO_RANK;
}

INLINE int LmStats::PoolXP(int guild_num) const
{
  return GuildNumOK(guild_num) ? stats_.xp_pool[guild_num] : 0;
}

INLINE int LmStats::QuestPoolXP() const
{
  return stats_.quest_xp_pool;
}

INLINE int LmStats::PPPool() const
{
  return stats_.pp_pool;
}

INLINE int LmStats::PP() const
{
  return stats_.pps;
}

INLINE bool LmStats::IsInGuild(int guild_num) const
{
  return (GuildRank(guild_num) != Guild::NO_RANK) && (GuildRank(guild_num) > 0); // negative = "pending"
}

INLINE int LmStats::XP() const
{
  return stats_.xp;
}

INLINE int LmStats::Orbit() const
{
  return stats_.orbit;
}

INLINE int LmStats::Sphere() const
{
  return Orbit() / 10;
}

INLINE int LmStats::FocusStat() const
{
  return stats_.focus;
}

INLINE int LmStats::MaxStat(int stat_num) const
{
  return StatNumOK(stat_num) ? stats_.max[stat_num] : Stats::STAT_MIN;
}

INLINE int LmStats::CurrentStat(int stat_num) const
{
  return StatNumOK(stat_num) ? stats_.curr[stat_num] : Stats::STAT_MIN;
}

INLINE bool LmStats::CanSetMaxStat(int stat_num, int stat_val) const
{
  return StatNumOK(stat_num) && StatValueOK(stat_val);
}

INLINE bool LmStats::CanSetCurrentStat(int stat_num, int stat_val) const
{
  // check versus max
  return StatNumOK(stat_num) && StatValueOK(stat_val) && (stat_val <= stats_.max[stat_num]);
}

INLINE bool LmStats::CanSetXP(int /* xp */) const
{
  // TODO: check that xp is within valid range?
  return true;
}

INLINE bool LmStats::CanSetFocusStat(int stat_num) const
{
  // DS can't be focus
  return StatNumOK(stat_num) && (stat_num != Stats::DREAMSOUL);
}

INLINE bool LmStats::CanSetGuildRank(int guild_num, int rank) const
{
  return GuildNumOK(guild_num) && GuildRankOK(rank);
}

INLINE void LmStats::SetMaxStat(int stat_num, int stat_val)
{
  if (CanSetMaxStat(stat_num, stat_val)) {
    stats_.max[stat_num] = stat_val;
    // check if current is larger than max, set to max if so
    if (stats_.curr[stat_num] > stat_val) {
      stats_.curr[stat_num] = stat_val;
    }
  }
}

INLINE void LmStats::SetCurrentStat(int stat_num, int stat_val)
{
  if (CanSetCurrentStat(stat_num, stat_val)) {
    stats_.curr[stat_num] = stat_val;
  }
}

INLINE void LmStats::SetXP(int xp)
{
  if (CanSetXP(xp)) {
    stats_.xp = xp;
    stats_.orbit = OrbitFromXP(xp);
  }
}

INLINE void LmStats::SetFocusStat(int stat_num)
{
  if (CanSetFocusStat(stat_num)){
    stats_.focus = stat_num;
  }
}

INLINE void LmStats::SetGuildRank(int guild_num, int rank)
{
  if (CanSetGuildRank(guild_num, rank)) {
    stats_.rank[guild_num] = rank;
  }
}

INLINE void LmStats::SetPoolXP(int guild_num, int xp)
{
  if (GuildNumOK(guild_num)) {
    stats_.xp_pool[guild_num] = xp;
  }
}

INLINE void LmStats::SetQuestPoolXP(int xp)
{
	stats_.quest_xp_pool = xp;
}

INLINE void LmStats::SetPPPool(int pp)
{
	stats_.pp_pool = pp;
}

INLINE void LmStats::SetPP(int pp)
{
	stats_.pps = pp;
}


INLINE void LmStats::UsePoolXP(int guild_num, int xp)
{
  if (GuildNumOK(guild_num)) {
    stats_.xp_pool[guild_num] -= xp;
    if (stats_.xp_pool[guild_num] < 0) {
      stats_.xp_pool[guild_num] = 0;
    }
  }
}

INLINE void LmStats::UseQuestPoolXP(int xp)
{
    stats_.quest_xp_pool -= xp;
    if (stats_.quest_xp_pool < 0) {
      stats_.quest_xp_pool = 0;
    }
}

INLINE void LmStats::UsePPPool(int pp)
{
    stats_.pp_pool -= pp;
    if (stats_.pp_pool < 0) {
      stats_.pp_pool = 0;
    }
}

INLINE int LmStats::TokensToDemote(int rank)
{
  int retval = 0;
  switch (rank) {
  case Guild::NO_RANK:  retval = 0; break;
  case Guild::INITIATE: retval = Guild::DEMOTE_INITIATE; break;
  case Guild::KNIGHT:   retval = Guild::DEMOTE_KNIGHT;   break;
  case Guild::RULER:    retval = Guild::DEMOTE_RULER;    break;
  default:              retval = 0; break;
  }
  return retval;
}
