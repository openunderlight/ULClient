// LmPeerUpdate.h  -*- C++ -*-
// $Id: LmPeerUpdate.h,v 1.11 1997-11-06 18:21:32-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// lyra_peer_update_t wrapper class

#ifndef INCLUDED_LmPeerUpdate
#define INCLUDED_LmPeerUpdate

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmBit32.h"

// forward decls

// base datatype

struct lyra_peer_update_t { // bit packed to the extreme
  // data split into meaningful bitfields (16-bit aligned!)

  //lyra_id_t playerid;  // 32
	// we use a unsigned char ID number here on a per-room basis
	// to avoid sending the whole 4 byte player ID
  unsigned char sound_id;
  unsigned char junk;
  short		    realtime_id; 
  short x;             // 48
  short y;             // 64
  short z;			  // 80
  short unused;		  // 96
  // NOTE: it is extremely important that the order of the below fields is not changed
  // arbitrarily (their sizes can be modified, look in the class constants).  If a
  // field is moved from u1 to u2 (say), then alot has to be changed.  Don't do this
  // unless it's absolutely necessary.

  // BIT31 weapon_damage:6 flags:9 attack_bits:6 angle:10 BIT0
  LmBit32 u1;          // 128
  
  // BIT31 unused:1 wave:1 scolor:3 pcolor:3 harmful:1 local:1 weapon_effect:5 height_delta:5 weapon_velocity:4
  //       hit: 5 weapon_bitmap:3 BIT0
  LmBit32 u2;          // 160

  // total = 160 bits, 20 bytes
};

// the class

class LmPeerUpdate {

public:

  enum {
    // bitfield widths for u1
    ANGLE_WIDTH = 10,        // 10
    ATTACKBITS_WIDTH = 5,    // 14
    FLAGS_WIDTH = 9,         // 23
    WEAPONDAMAGE_WIDTH = 6,  // 29
	FLIGHT_WIDTH = 1,
	UNUSED_WIDTH_U1 = 1,

    // bitfield widths for u2
    WEAPONBITMAP_WIDTH = 3,    // 3
    HITBITS_WIDTH = 5,		   // 8
    WEAPONVELOCITY_WIDTH = 4,  // 12
    HEIGHTDELTA_WIDTH = 5,     // 17
    WEAPONEFFECT_WIDTH = 5,    // 22
    LOCAL_WIDTH = 1,           // 23
    HARMFUL_WIDTH = 1,         // 24
    PRIMARY_COLOR_WIDTH = 3,   // 27
    SECONDARY_COLOR_WIDTH = 3, // 30
    WAVE_WIDTH = 1,			   // 31
    UNUSED_WIDTH_U2 = 1,	   // 32

    // starting positions for u1 (shouldn't need to be modified)
    ANGLE_START = 0,
    ATTACKBITS_START = (ANGLE_START + ANGLE_WIDTH),
    FLAGS_START = (ATTACKBITS_START + ATTACKBITS_WIDTH),
    WEAPONDAMAGE_START = (FLAGS_START + FLAGS_WIDTH),
	FLIGHT_START = (WEAPONDAMAGE_START + WEAPONDAMAGE_WIDTH),
    // starting positions for u2 (shouldn't need to be modified)
    WEAPONBITMAP_START = 0,
    HITBITS_START = (WEAPONBITMAP_START + WEAPONBITMAP_WIDTH),
    WEAPONVELOCITY_START = (HITBITS_START + HITBITS_WIDTH),
    HEIGHTDELTA_START = (WEAPONVELOCITY_START + WEAPONVELOCITY_WIDTH),
    WEAPONEFFECT_START = (HEIGHTDELTA_START + HEIGHTDELTA_WIDTH),
    LOCAL_START = (WEAPONEFFECT_START + WEAPONEFFECT_WIDTH),
    HARMFUL_START = (LOCAL_START + LOCAL_WIDTH),
    PRIMARY_COLOR_START = (HARMFUL_START + HARMFUL_WIDTH),
    SECONDARY_COLOR_START = (PRIMARY_COLOR_START + PRIMARY_COLOR_WIDTH),
    WAVE_START = (SECONDARY_COLOR_START + SECONDARY_COLOR_WIDTH),

    // flag values (up to 9)
    LG_WALKING     = 0x0001,
    LG_RUNNING     = 0x0002,
    LG_BACKWARDS   = 0x0004,
    LG_JUMPED      = 0x0008,
    LG_STRAFING    = 0x0010, // normal=left, reverse=right
    LG_INVISIBLE   = 0x0020,
    LG_SOULSPHERE  = 0x0040,
    LG_EVOKING     = 0x0080,
    LG_EVOKED      = 0x0100

  };

  static const LmPeerUpdate DEFAULT_INSTANCE;

public:

  LmPeerUpdate();

  void Init(short realtimeid, unsigned char sound_id, short x, short y, short z, int u1, int u2);
  void Init(const lyra_peer_update_t& update);

  // selector/mutator for base type
  const lyra_peer_update_t& Update() const;
  void SetUpdate(const lyra_peer_update_t& update);

  // selectors for bitfields
  short RealtimeID() const;
  unsigned char SoundID() const;
  int X() const;
  int Y() const;
  int Z() const;
  unsigned int Angle() const;
  unsigned int AttackBits() const;
  unsigned int Flags() const;
  unsigned int WeaponDamage() const;
  unsigned int HitBits() const;
  unsigned int WeaponBitmap() const;
  int WeaponVelocity() const;
  int HeightDelta() const;
  unsigned int WeaponEffect() const;
  unsigned int Local() const;
  unsigned int Harmful() const;
  unsigned int PrimaryColor() const;
  unsigned int SecondaryColor() const;
  unsigned int Wave() const;
  unsigned int Flying() const;

  bool FlagSet(int flag) const;
  
  // mutators for bitfields
  void SetRealtimeID(short realtimeid);
  void SetSoundID(unsigned char soundid);
  void SetPosition(int x, int y, int z);
  void SetAngle(unsigned int angle);
  void SetAttackBits(unsigned int attack_bits);
  void SetFlags(unsigned int flags);
  void SetWeaponDamage(unsigned int weapon_damage);
  void SetHitBits(unsigned int hit_bits);
  void SetWeaponBitmap(unsigned int weapon_bitmap);
  void SetWeaponVelocity(int weapon_velocity);
  void SetHeightDelta(int height_delta);
  void SetWeaponEffect(unsigned int weapon_effect);
  void SetLocal(unsigned int local);
  void SetHarmful(unsigned int harmful);
  void SetPrimaryColor(unsigned int primary);
  void SetSecondaryColor(unsigned int secondart);
  void SetWave(unsigned int wave);
  void SetFlying(unsigned int flying);
  // conversion functions 
  void ConvertToNetwork();
  void ConvertToHost();

  // internal fields
  unsigned int U1() const;
  unsigned int U2() const;

  void Dump(FILE* f, int indent = 0) const;

private:

  lyra_peer_update_t u_;

};

#ifdef USE_INLINE
#include "LmPeerUpdate.i"
#endif

#endif /* INCLUDED_LmPeerUpdate */
