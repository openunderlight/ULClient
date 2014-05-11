// LmStats.cpp  -*- C++ -*-
// $Id: LmStats.cpp,v 1.15 1997-12-19 18:19:54-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <string.h>

#include "LmStats.h"
#include "LmXPTable.h"

#ifndef USE_INLINE
#include "LmStats.i"
#endif

////
// constructor
////

extern xp_entry lyra_xp_table[];

LmStats::LmStats()
{
  Init();
}

////
// Init
////

void LmStats::Init()
{
  int i;
  // set all stats (curr/max) to minimum
  stats_.xp = 0;
  stats_.orbit = 0;
  stats_.focus = Stats::NO_STAT;
  for (i = 0; i < NUM_PLAYER_STATS; ++i) {
    stats_.curr[i] = Stats::STAT_MIN;
    stats_.max[i] = Stats::STAT_MIN;
  }
  // initialize guild ranks, xp pool
  for (i = 0; i < NUM_GUILDS; ++i) {
    stats_.rank[i] = Guild::NO_RANK;
    stats_.xp_pool[i] = 0;
  }
  stats_.quest_xp_pool = 0;
}

////
// ConvertToHost
////

void LmStats::ConvertToHost()
{
  NTOHL(stats_.xp);
  for (int i = 0; i < NUM_GUILDS; ++i) {
    NTOHL(stats_.xp_pool[i]);
  }
  NTOHL(stats_.quest_xp_pool);
  // not converted: focus, curr, max, rank, orbit
}

////
// ConvertToNetwork
////

void LmStats::ConvertToNetwork()
{
  HTONL(stats_.xp);
  for (int i = 0; i < NUM_GUILDS; ++i) {
    HTONL(stats_.xp_pool[i]);
  }
  HTONL(stats_.quest_xp_pool);
  // not converted: focus, curr, max, rank, orbit
}

////
// AdjustStats: adjust given stat by given delta; if under or overflow occurs, stat
//   is left at boundary value; both max and current are modified
///

static inline int adjust_aux(int val, int adj)
{
  // add
  val += adj;
  // check min, max
  if (val < Stats::STAT_MIN) {
    val = Stats::STAT_MIN;
  }
  if (val > Stats::STAT_MAX) {
    val = Stats::STAT_MAX;
  }
  return val;
}

void LmStats::AdjustStat(int stat_num, int adj)
{
  if (!StatNumOK(stat_num)) {
    return;
  }
  // update, with range check
  stats_.curr[stat_num] = adjust_aux(stats_.curr[stat_num], adj);
  stats_.max[stat_num] = adjust_aux(stats_.max[stat_num], adj);
}

////
// CheckIntegrity: check structure for any problems, return 1 if OK, < 0 if not
////

int LmStats::CheckIntegrity() const
{
  // check that each max stat is in valid range
  int i;
  for (i = 0; i < NUM_PLAYER_STATS; ++i) {
    if (!StatValueOK(stats_.max[i])) {
      return -1;
    }
  }
  // check that each current stat is in valid range
  for (i = 0; i < NUM_PLAYER_STATS; ++i) {
    if (!StatValueOK(stats_.curr[i])) {
      return -2;
    }
  }
  // check that each current stat is less than max
  for (i = 0; i < NUM_PLAYER_STATS; ++i) {
    if (stats_.curr[i] > stats_.max[i]) {
      return -3;
    }
  }
  // check that focus stat is defined
  if (!StatNumOK(stats_.focus)) {
    return -4;
  }
  // check that orbit is within range
  if (!OrbitOK(stats_.orbit)) {
    return -5;
  }
  // check that xp is within range?
  // ok
  return 1;
}

////
// Dump
////

#ifdef USE_DEBUG
// map from stat number to string value
static const TCHAR* stat_string(int stat_num)
{
  switch (stat_num) {
  case Stats::NO_STAT:    return _T("NN"); break;
  case Stats::DREAMSOUL:  return _T("DS"); break;
  case Stats::WILLPOWER:  return _T("WP"); break;
  case Stats::INSIGHT:    return _T("IN"); break;
  case Stats::RESILIENCE: return _T("RS"); break;
  case Stats::LUCIDITY:   return _T("LU"); break;
  default:                return _T("U*"); break;
  }
  return _T("U*");
};

void LmStats::Dump(FILE* f, int indent) const
{
  int i;
  INDENT(indent, f);
 _ftprintf(f, _T("<LmStats[%p,%d]: orbit=%d xp=%d quest_xp_pool=%d focus=%s(%d) ok=%d>\n"), this, sizeof(LmStats),
	  Orbit(), XP(), QuestPoolXP(), stat_string(FocusStat()), FocusStat(), CheckIntegrity());
  INDENT(indent + 1, f);
 _ftprintf(f, _T("stats: "));
  for (i = 0; i < NUM_PLAYER_STATS; ++i) {
   _ftprintf(f, _T("%s(%d): %02d/%02d"), stat_string(i), i, CurrentStat(i), MaxStat(i));
    if (i != (NUM_PLAYER_STATS - 1)) {
     _ftprintf(f, _T(",  "));
    }
  }
 _ftprintf(f, _T("\n"));
  INDENT(indent + 1, f);
 _ftprintf(f, _T("guild ranks (%d): ["), NumGuildsIn());
  for (i = 0; i < NUM_GUILDS; ++i) {
   _ftprintf(f, _T("%d"), GuildRank(i));
    if (i != (NUM_GUILDS - 1)) {
     _ftprintf(f, _T(","));
    }
  }
 _ftprintf(f, _T("]\n"));
  INDENT(indent + 1, f);
 _ftprintf(f, _T("xp pool: ["));
  for (i = 0; i < NUM_GUILDS; ++i) {
   _ftprintf(f, _T("%d"), PoolXP(i));
    if (i != (NUM_GUILDS - 1)) {
     _ftprintf(f, _T(","));
    }
  }
 _ftprintf(f, _T("]\n"));
}
#endif /* USE_DEBUG */

////
// GainOrbit: apply stat improvements gained by moving up a single orbit, to the 
//   given new orbit
////

// stat improvement table
struct stat_adj_t {
  TCHAR adj[NUM_PLAYER_STATS];
};

static const stat_adj_t stat_adj[10] = {
  // NOTE: this depends highly on the order of the stats!
  //  DS WP IN RS LU     orbit % 10
  { { 1, 1, 1, 1, 1 } },  // 0
  { { 1, 0, 0, 0, 0 } },  // 1
  { { 1, 0, 0, 1, 0 } },  // 2
  { { 1, 0, 1, 0, 0 } },  // 3
  { { 1, 1, 0, 0, 0 } },  // 4
  { { 1, 0, 0, 0, 1 } },  // 5
  { { 1, 1, 0, 0, 0 } },  // 6
  { { 1, 0, 1, 0, 0 } },  // 7
  { { 1, 0, 0, 1, 0 } },  // 8
  { { 0, 0, 0, 0, 1 } },  // 9
};  

int LmStats::StatAtLevel(int focus, int stat, int orbit)
{
	int value = 10;
	if ((focus == stat) || (stat == Stats::DREAMSOUL))
		value = 20;

	for (int i=1; i<=orbit; i++) {
		int index = i % 10;
		value += stat_adj[index].adj[stat];
		if ((focus == stat) && (0 == i%2)) { // add 1 to focus stat every second orbit
			value += 1;
		}
	}
		
	return value;
}

int LmStats::StatIncreaseCost(int focus, int stat, int orbit, int curr_stat_value)
{
	int normal_stat = LmStats::StatAtLevel(focus, stat, orbit);

	int cost = 0;
	int new_stat = curr_stat_value + 1;
	for (int i=1; i<=new_stat-normal_stat; i++)
		cost += i;
	return cost+25;
}

int LmStats::SphereIncreaseCost(int sphere)
{
	return sphere*100;
}

int LmStats::TrainPPCost(int art, int level, int min_orbit, int multiplier)
{
	if (level == 1) { // learning new art
    switch (art) {
      case Arts::PUSH:
        return 37;
      case Arts::FLAMESEAR:
      case Arts::TRANCEFLAME:
      case Arts::FLAMESHAFT:
      case Arts::FLAMERUIN:
        return 41;
      default:
		    return 50 + min_orbit;
    }
	} else { // plateau
		return 25 + min_orbit/2 + level/2;
	}
}

int LmStats::EvokePPCost(int art, int level, int min_orbit, int multiplier)
{
	int base_cost = 1 + min_orbit/10;
	return base_cost + (level/10)*multiplier;
}


int LmStats::XPPPCost(int current_xp)
{
	int min = 100;
	int alt = current_xp/1000;
	if (min > alt)
		return min;
	else
		return alt;
}


void LmStats::GainOrbit(int new_orbit)
{
  if (!OrbitOK(new_orbit)) {
    return;
  }
  // get adjustment entry
  int index = new_orbit % 10;
  // adjust stats
  for (int i = 0; i < NUM_PLAYER_STATS; ++i) {
    AdjustStat(i, stat_adj[index].adj[i]);
  }
  // add 1 to focus stat every second orbit
  if ((new_orbit % 2) == 0) {
    AdjustStat(stats_.focus, 1);
  }
}

////
// LoseOrbit - undo stat gains from going from orbit x to x-1
////

void LmStats::LoseOrbit(int new_orbit)
{
  if (!OrbitOK(new_orbit)) {
    return;
  }
  // actually want to undo gains due to going to orbit (new_orbit + 1)
  new_orbit++;
  // get adjustment entry
  int index = new_orbit % 10;
  // adjust stats
  for (int i = 0; i < NUM_PLAYER_STATS; ++i) {
    AdjustStat(i, -stat_adj[index].adj[i]);
  }
  // add 1 to focus stat every second orbit
  if ((new_orbit % 2) == 0) {
    AdjustStat(stats_.focus, -1);
  }
}

////
// NumGuildsIn - return number of guilds player is a member of
////

int LmStats::NumGuildsIn() const
{
  int num = 0;
  for (int guild = 0; guild < NUM_GUILDS; ++guild) {
    if (IsInGuild(guild)) {
      num++;
    }
  }
  return num;
}

////
// IsKnight - return true if player is a knight in at least one guild
////

bool LmStats::IsKnight() const
{
  for (int guild = 0; guild < NUM_GUILDS; ++guild) {
    if (GuildRank(guild) == Guild::KNIGHT) {
      return true;
    }
  }
  return false;
}

////
// IsRuler - return true if player is a ruler in at least one guild
////

bool LmStats::IsRuler() const
{
  for (int guild = 0; guild < NUM_GUILDS; ++guild) {
    if (GuildRank(guild) == Guild::RULER) {
      return true;
    }
  }
  return false;
}

////
// ReadyToAdvance - return true if player is ready to advance to given sphere, false if not
////

bool LmStats::ReadyToAdvance(int sphere) const
{
  // check sphere -- can't "advance" to sphere 0, or above 9
  if ((sphere < 1) || (sphere > 9)) {
    return false;
  }
  // only advance to next sphere
  if ((sphere - 1) != Sphere()) {
    return false;
  }
  int curr_orbit = Orbit();
  if (curr_orbit == 99) { // can't go above this
    return false;
  }
  int new_orbit = sphere * 10;
  // check that we're ready -- must be one xp away from next sphere base
  if ((curr_orbit + 1) != new_orbit) {
    return false;
  }
  int curr_xp = XP();
  int new_xp = OrbitXPBase(new_orbit);
  if ((curr_xp + 1) != new_xp) {
    return false;
  }
  // looks ok
  return true;
}

////
// AdvanceToSphere - move up (by teaching) to given sphere
////

void LmStats::AdvanceToSphere(int sphere)
{
  if (ReadyToAdvance(sphere)) {
    // gain an extra 1.5% percent buffer, so that if player dies immediately they
    // won't lose the sphere
    int extra_xp = (int) ((double) SphereXPBase(sphere) * 0.015);
    SetXP(SphereXPBase(sphere) + extra_xp);
  }
}

////
// AdjustXP - adjust player's XP by given amount (positive or negative), only allowing
//   an orbit change of +/- 1 at most, and not allowing a sphere change; takes multiple
//   guild membership into account; returns actual adjustment made
////

int LmStats::AdjustXP(int adj)
{
  // quick check
  if (adj == 0) {
    return 0;
  }
  // if in multiple guilds, then positive adjustments are smaller
  int num_guilds = NumGuildsIn();
  if ((num_guilds > 1) && (adj > 0)) {
    adj = adj / num_guilds; // for every guild, you lose alot
  }
  int curr_orbit = Orbit();
  int new_xp = XP() + adj;
  if (adj < 0) { // loss
    int min_xp = 0; // don't go negative with xp
    if (curr_orbit > 0) { // at least orbit 1
      min_xp = OrbitXPBase(curr_orbit - 1) + 1;
    }
    // can't drop below minimum
    if (new_xp < min_xp) {
      new_xp = min_xp;
    }
  }
  else { // gain
    int max_xp = Lyra::MAX_XP;
    if (curr_orbit < 99) { // can still gain an orbit
      if ((curr_orbit % 10) == 9) { // at a sphere boundary
	// can't go to next sphere by xp gain, must be taught
	max_xp = OrbitXPBase(curr_orbit + 1) - 1;
      }
      else {
	// can only go up a single orbit
	max_xp = OrbitXPBase(curr_orbit + 1) + 1; //+ OrbitXPGain(curr_orbit + 1) - 1;
      }
#if 0
      // HACK: no sphere limit, since teaching isn't implemented
      // can only go up a single orbit
      max_xp = OrbitXPBase(curr_orbit + 1) + 1; //+ OrbitXPGain(curr_orbit + 1) - 1;
#endif
    }
    // can't exceed maximum
    if (new_xp > max_xp) {
      new_xp = max_xp;
    }
  }
  // compute actual change made
  adj = new_xp - XP();
  // set it
  SetXP(new_xp);
  // return actual change
  return adj;
}




////
// PMareAdjustXP - adjust player's XP by given amount (positive or negative), ignoring
//   all the usual player factors
////

int LmStats::PMareAdjustXP(int adj)
{
  // quick check
  if (adj == 0) {
    return 0;
  }
  int new_xp = XP() + adj;
  if (new_xp < 0)
    new_xp = 0;

  adj = new_xp - XP();
  // set it
  SetXP(new_xp);

  // return actual change
  return adj;
}


////
// OrbitFromXP - return player's orbit, given xp
////

int LmStats::OrbitFromXP(int xp)
{
  // linear search of xp table
  for (int orbit = 1; orbit < 100; ++orbit) {
    if (xp < OrbitXPBase(orbit)) {
      return orbit - 1;
    }
  }
  // if we reached here, xp is greater than base for 99, so orbit is 99
  return 99;
}

////
// OrbitXPBase - return xp_base for given orbit
////

int LmStats::OrbitXPBase(int orbit)
{
  if (OrbitOK(orbit)) {
    return lyra_xp_table[orbit].xp_base;
  }
  return 0;
}

////
// OrbitXPGain - return xp_gain for given orbit
////

int LmStats::OrbitXPGain(int orbit)
{
  if (OrbitOK(orbit)) {
    return lyra_xp_table[orbit].xp_gain;
  }
  return 0;
}

////
// SphereXPBase - return xp_base for given sphere
////

int LmStats::SphereXPBase(int sphere)
{
  return OrbitXPBase(sphere * 10);
}
