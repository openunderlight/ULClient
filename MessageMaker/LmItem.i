// LmItem.i  -*- C++ -*-
// $Id: LmItem.i,v 1.22 1997-08-14 19:40:54-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions
#ifdef PMARE
#include "Utils.h"
#endif

#ifndef USE_DEBUG
INLINE void LmItem::Dump(FILE*, int) const
{
  // empty
}

INLINE void LmItem::Dump1(FILE*) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const LmItemHdr& LmItem::Header() const
{
  return hdr_;
}

INLINE LmItemHdr& LmItem::Header()
{
  return hdr_;
}

INLINE int LmItem::State1() const
{
  return state_.s.s1;
}

INLINE int LmItem::State2() const
{
  return state_.s.s2;
}

INLINE int LmItem::State3() const
{
  return state_.s.s3;
}

INLINE int LmItem::StateByte(int bytenum) const
{
  return INDEX_OK(bytenum, 0, 12) ? state_.b[bytenum] : 0;
}

INLINE int LmItem::Charges() const
{
  return state_.b[0];
}

INLINE TCHAR* LmItem::Name()
{
#ifdef PMARE
  return TalismanName(Graphic());
#else
  return name_;
#endif
}

INLINE const TCHAR* LmItem::Name() const
{
#ifdef PMARE
  return TalismanName(Graphic());
#else
  return name_;
#endif
}

INLINE int LmItem::FieldLength(int field_num) const
{
  return LyraItem::FieldLength(hdr_.StateFormat(), field_num);
}

INLINE bool LmItem::FieldIsValid(int field_num) const
{
  return (FieldLength(field_num) > 0);
}

INLINE bool LmItem::IsArtifact() const
{
  return (Charges() == 255);
}

INLINE lyra_id_t LmItem::ItemID() const
{
  return hdr_.ItemID();
}

INLINE int LmItem::Serial() const
{
  return hdr_.Serial();
}

INLINE int LmItem::Flags() const
{
  return hdr_.Flags();
}

INLINE int LmItem::Graphic() const
{
  return hdr_.Graphic();
}

INLINE int LmItem::Color1() const
{
  return hdr_.Color1();
}

INLINE int LmItem::Color2() const
{
  return hdr_.Color2();
}

INLINE int LmItem::StateFormat() const
{
  return hdr_.StateFormat();
}

INLINE bool LmItem::FlagSet(int flag) const
{
  return hdr_.FlagSet(flag);
}

INLINE void LmItem::SetHeader(const LmItemHdr& hdr)
{
  hdr_ = hdr;
}

INLINE void LmItem::SetState(int s1, int s2, int s3)
{
  state_.s.s1 = s1;
  state_.s.s2 = s2;
  state_.s.s3 = s3;
}

INLINE void LmItem::SetCharges(int charges)
{
  state_.b[0] = charges;
}

INLINE bool LmItem::Equals(const LmItem& item) const
{
  // items are equivalent if their headers are equal
  return hdr_.Equals(item.hdr_);
}
