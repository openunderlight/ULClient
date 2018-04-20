// LmItemHdr.i  -*- C++ -*-
// $Id: LmItemHdr.i,v 1.9 1997-07-29 18:15:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifdef WIN32
#pragma warning( disable : 4800 )
#endif

#ifndef USE_DEBUG
INLINE void LmItemHdr::Dump(FILE*, int) const
{
  // empty
}

INLINE void LmItemHdr::Dump1(FILE*) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t LmItemHdr::ItemHdr1() const
{
  return item_.itemid.Value();
}

INLINE lyra_id_t LmItemHdr::ItemHdr2() const
{
  return item_.h2.Value();
}

INLINE int LmItemHdr::Serial() const
{
  return item_.serial;
}

INLINE int LmItemHdr::Flags() const
{
  return item_.h2.GetBits(FLAGS_START, FLAGS_WIDTH);
}

INLINE int LmItemHdr::StateFormat() const
{
  return item_.h2.GetBits(FORMAT_START, FORMAT_WIDTH);
}

INLINE int LmItemHdr::Graphic() const
{
  return item_.itemid.GetBits(GRAPHIC_START, GRAPHIC_WIDTH);
}

INLINE int LmItemHdr::Color1() const
{
  return item_.itemid.GetBits(COLOR1_START, COLOR1_WIDTH);
}

INLINE int LmItemHdr::Color2() const
{
  return item_.itemid.GetBits(COLOR2_START, COLOR2_WIDTH);
}

INLINE bool LmItemHdr::FlagSet(int flag) const
{
  return (Flags() & flag);
}

INLINE void LmItemHdr::SetItemID(lyra_id_t itemid, lyra_id_t i2)
{
  item_.itemid.SetValue(itemid);
  item_.h2.SetValue(i2);
}

INLINE void LmItemHdr::SetSerial(int serial)
{
  item_.serial = serial;
}

INLINE void LmItemHdr::SetFlags(int flags)
{
  item_.h2.SetBits(FLAGS_START, FLAGS_WIDTH, flags);
}

INLINE void LmItemHdr::SetFlag(int flag)
{
  SetFlags(Flags() | flag);
}

INLINE void LmItemHdr::ClearFlag(int flag)
{
  SetFlags(Flags() & ~flag);
}

INLINE void LmItemHdr::SetColor1(int color)
{
  item_.itemid.SetBits(COLOR1_START, COLOR1_WIDTH, color);
}

INLINE void LmItemHdr::SetColor2(int color)
{
  item_.itemid.SetBits(COLOR2_START, COLOR2_WIDTH, color);
}

INLINE void LmItemHdr::SetGraphic(int graphic)
{
  item_.itemid.SetBits(GRAPHIC_START, GRAPHIC_WIDTH, graphic);
}

INLINE void LmItemHdr::SetStateFormat(int format)
{
  item_.itemid.SetBits(FORMAT_START, FORMAT_WIDTH, format);
}

INLINE bool LmItemHdr::Equals(const LmItemHdr& item) const
{
  return ((ItemHdr1() == item.ItemHdr1()) && (ItemHdr2() == item.ItemHdr2()) && (Serial() == item.Serial()));
}
