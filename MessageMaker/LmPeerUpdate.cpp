// LmPeerUpdate.cpp  -*- C++ -*-
// $Id: LmPeerUpdate.cpp,v 1.10 1997-11-06 18:21:32-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmPeerUpdate.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "LmPeerUpdate.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmPeerUpdate.i"
#endif

// default object
const LmPeerUpdate LmPeerUpdate::DEFAULT_INSTANCE;

////
// Constructor
////

LmPeerUpdate::LmPeerUpdate()
{
  Init(Lyra::ID_UNKNOWN, 0, 0, 0, 0, 0);
}

////
// Init
////

void LmPeerUpdate::Init(short realtimeid, unsigned char soundid, short x, short y, int u1, int u2)
{
  SetRealtimeID(realtimeid);
  SetSoundID(soundid);
  SetPosition(x, y);
  u_.u1.SetValue(u1);
  u_.u2.SetValue(u2);
}

void LmPeerUpdate::Init(const lyra_peer_update_t& update)
{
  SetUpdate(update);
}

////
// ConvertToNetwork
////

void LmPeerUpdate::ConvertToNetwork()
{
  HTONS(u_.realtime_id);
  HTONS(u_.x);
  HTONS(u_.y);
  u_.u1.ConvertToNetwork();
  u_.u2.ConvertToNetwork();
}

////
// ConvertToHost
////

void LmPeerUpdate::ConvertToHost()
{
  NTOHS(u_.realtime_id);
  NTOHS(u_.x);
  NTOHS(u_.y);
  u_.u1.ConvertToHost();
  u_.u2.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void LmPeerUpdate::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
	_ftprintf(f, _T("<LmPeerUpdate[%p,%d]: real time id=%u sound id=%u pos=(%d,%d) u1=%u u2=%u>\n"), this, sizeof(LmPeerUpdate),
	  RealtimeID(), SoundID(), X(), Y(), U1(), U2());
  // print u1 stuff
  INDENT(indent + 1, f);
  //char str[80];
  //u_.u1.UnParse(str, sizeof(str));
  ///_ftprintf(f, _T("u1 := %s\n"), str);
  //INDENT(indent + 1, f);
	_ftprintf(f, _T("hit=%d weapon_damage=%d flags=0x%x attack_bits=0x%x angle=%u\n"),
	  HitBits(), WeaponDamage(), Flags(), AttackBits(), Angle());
  // print u2 stuff
  INDENT(indent + 1, f);
  //u_.u2.UnParse(str, sizeof(str));
  //_ftprintf(f, _T("u2 := %s\n"), str);
  //INDENT(indent + 1, f);
	_ftprintf(f, _T("local=%u wpn_effect=%d ht_delta=%d wpn_vel=%d wpn_bmap=%d harmful=%d color=%d/%d wave=%u\n"),
	  Local(), WeaponEffect(), HeightDelta(), WeaponVelocity(), WeaponBitmap(), Harmful(),
	  PrimaryColor(), SecondaryColor(), Wave());
}
#endif /* USE_DEBUG */
