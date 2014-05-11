// LmPeerUpdate.i  -*- C++ -*-
// $Id: LmPeerUpdate.i,v 1.12 1998-04-16 19:03:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#include "SharedConstants.h" // need bitmap constants

#ifndef USE_DEBUG
INLINE void LmPeerUpdate::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const lyra_peer_update_t& LmPeerUpdate::Update() const
{
  return u_;
}

INLINE void LmPeerUpdate::SetUpdate(const lyra_peer_update_t& update)
{
  u_ = update;
}

INLINE short LmPeerUpdate::RealtimeID() const
{
  return u_.realtime_id;
}

INLINE unsigned char LmPeerUpdate::SoundID() const
{
  return u_.sound_id;
}


INLINE int LmPeerUpdate::X() const
{
  return u_.x;
}

INLINE int LmPeerUpdate::Y() const
{
  return u_.y;
}

INLINE unsigned int LmPeerUpdate::Angle() const
{
  return u_.u1.GetBits(ANGLE_START, ANGLE_WIDTH);
}

INLINE unsigned int LmPeerUpdate::AttackBits() const
{
  return u_.u1.GetBits(ATTACKBITS_START, ATTACKBITS_WIDTH);
}

INLINE unsigned int LmPeerUpdate::Flags() const
{
  return u_.u1.GetBits(FLAGS_START, FLAGS_WIDTH);
}

INLINE bool LmPeerUpdate::FlagSet(int flag) const
{
  return (Flags() & flag) ? true : false;
}

INLINE unsigned int LmPeerUpdate::WeaponDamage() const
{
  return u_.u1.GetBits(WEAPONDAMAGE_START, WEAPONDAMAGE_WIDTH);
}

INLINE unsigned int LmPeerUpdate::WeaponBitmap() const
{
  unsigned int normalized_bitmap = 
	(u_.u2.GetBits(WEAPONBITMAP_START, WEAPONBITMAP_WIDTH) + LyraBitmap::MINIMUM_MISSILE_BITMAP);
  return normalized_bitmap;
}

INLINE unsigned int LmPeerUpdate::HitBits() const
{
  return u_.u2.GetBits(HITBITS_START, HITBITS_WIDTH);
}

INLINE int LmPeerUpdate::WeaponVelocity() const
{
  int retval = u_.u2.GetBits(WEAPONVELOCITY_START, WEAPONVELOCITY_WIDTH);
  if (retval > 8) {
    retval -= 8;
    retval = -retval;
  }
  return retval;
}

INLINE int LmPeerUpdate::HeightDelta() const
{
  return u_.u2.GetBits(HEIGHTDELTA_START, HEIGHTDELTA_WIDTH);
}

INLINE unsigned int LmPeerUpdate::WeaponEffect() const
{
  return u_.u2.GetBits(WEAPONEFFECT_START, WEAPONEFFECT_WIDTH);
}

INLINE unsigned int LmPeerUpdate::Local() const
{
  return u_.u2.GetBits(LOCAL_START, LOCAL_WIDTH);
}

INLINE unsigned int LmPeerUpdate::Harmful() const
{
  return u_.u2.GetBits(HARMFUL_START, HARMFUL_WIDTH);
}

INLINE unsigned int LmPeerUpdate::PrimaryColor() const
{
  return u_.u2.GetBits(PRIMARY_COLOR_START, PRIMARY_COLOR_WIDTH);
}

INLINE unsigned int LmPeerUpdate::SecondaryColor() const
{
  return u_.u2.GetBits(SECONDARY_COLOR_START, SECONDARY_COLOR_WIDTH);
}

INLINE unsigned int LmPeerUpdate::Wave() const
{
  return u_.u2.GetBits(WAVE_START, WAVE_WIDTH);
}


INLINE unsigned int LmPeerUpdate::U1() const
{
  return u_.u1.Value();
}

INLINE unsigned int LmPeerUpdate::U2() const
{
  return u_.u2.Value();
}

INLINE void LmPeerUpdate::SetRealtimeID(short realtimeid)
{
  u_.realtime_id = realtimeid;
}

INLINE void LmPeerUpdate::SetSoundID(unsigned char soundid)
{
  u_.sound_id = soundid;
}


INLINE void LmPeerUpdate::SetPosition(int x, int y)
{
  u_.x = x;
  u_.y = y;
}

INLINE void LmPeerUpdate::SetAngle(unsigned int angle)
{
  u_.u1.SetBits(ANGLE_START, ANGLE_WIDTH, angle);
}

INLINE void LmPeerUpdate::SetAttackBits(unsigned int attack_bits)
{
  u_.u1.SetBits(ATTACKBITS_START, ATTACKBITS_WIDTH, attack_bits);
}

INLINE void LmPeerUpdate::SetFlags(unsigned int flags)
{
  u_.u1.SetBits(FLAGS_START, FLAGS_WIDTH, flags);
}

INLINE void LmPeerUpdate::SetWeaponDamage(unsigned int weapon_damage)
{
  u_.u1.SetBits(WEAPONDAMAGE_START, WEAPONDAMAGE_WIDTH, weapon_damage);
}

INLINE void LmPeerUpdate::SetHitBits(unsigned int hit_bits)
{
  u_.u2.SetBits(HITBITS_START, HITBITS_WIDTH, hit_bits);
}

INLINE void LmPeerUpdate::SetWeaponBitmap(unsigned int weapon_bitmap)
{	// to reduce the # of bits used, we normalize the weapon bitmap
  unsigned int normalized_bitmap = weapon_bitmap - LyraBitmap::MINIMUM_MISSILE_BITMAP;
  u_.u2.SetBits(WEAPONBITMAP_START, WEAPONBITMAP_WIDTH, normalized_bitmap);
}

INLINE void LmPeerUpdate::SetWeaponVelocity(int weapon_velocity)
{
  if (weapon_velocity < 0) {
    weapon_velocity = (- weapon_velocity) + 8;
  }
  u_.u2.SetBits(WEAPONVELOCITY_START, WEAPONVELOCITY_WIDTH, weapon_velocity);
}

INLINE void LmPeerUpdate::SetHeightDelta(int height_delta)
{
  u_.u2.SetBits(HEIGHTDELTA_START, HEIGHTDELTA_WIDTH, height_delta);
}

INLINE void LmPeerUpdate::SetWeaponEffect(unsigned int weapon_effect)
{
  u_.u2.SetBits(WEAPONEFFECT_START, WEAPONEFFECT_WIDTH, weapon_effect);
}

INLINE void LmPeerUpdate::SetHarmful(unsigned int harmful)
{
  u_.u2.SetBits(HARMFUL_START, HARMFUL_WIDTH, harmful);
}

INLINE void LmPeerUpdate::SetPrimaryColor(unsigned int primary)
{
  u_.u2.SetBits(PRIMARY_COLOR_START, PRIMARY_COLOR_WIDTH, primary);
}

INLINE void LmPeerUpdate::SetSecondaryColor(unsigned int secondary)
{
  u_.u2.SetBits(SECONDARY_COLOR_START, SECONDARY_COLOR_WIDTH, secondary);
}

INLINE void LmPeerUpdate::SetLocal(unsigned int local)
{
  u_.u2.SetBits(LOCAL_START, LOCAL_WIDTH, local);
}

INLINE void LmPeerUpdate::SetWave(unsigned int wave)
{
  u_.u2.SetBits(WAVE_START, WAVE_WIDTH, wave);
}

