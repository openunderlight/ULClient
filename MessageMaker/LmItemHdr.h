// LmItemHdr.h  -*- C++ -*-
// $Id: LmItemHdr.h,v 1.12 1997-07-25 15:25:03-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item header class

#ifndef INCLUDED_LmItemHdr
#define INCLUDED_LmItemHdr

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmBit32.h"

// base datatype

struct lyra_itemhdr_t {
  LmBit32 itemid;    // id - contains flags, state format, graphic, color
  // itemid layout: [bit31 flags format graphic color1 color2 bit0] (or the reverse :)
  LmBit32 h2; // field 2 -- all flags will be moved here.
  int serial;        // serial number -- unique among all items, in fact
};

// the class

class LmItemHdr {

public:

  // constants
	enum {
		// field widths in bits (make sure they don't add to more than 32!)
	UNUSED_WIDTH = 6,
	GRAPHIC_WIDTH = 10,  // 16
	FORMAT_WIDTH = 8,    // 24
	COLOR1_WIDTH = 4,    // 28
	COLOR2_WIDTH = 4,    // 32,
	MAX_COLORS = 16,
	ANY_COLOR = MAX_COLORS, // used by mission builder
	FLAGS_WIDTH = 16,

    // starting bit positions (shouldn't need to be modified)
    FLAGS_START = 0,
	UNUSED_START = 0, 
    GRAPHIC_START = (UNUSED_START + UNUSED_WIDTH),
    FORMAT_START = (GRAPHIC_START + GRAPHIC_WIDTH),
    COLOR1_START = (FORMAT_START + FORMAT_WIDTH),
    COLOR2_START = (COLOR1_START + COLOR1_WIDTH),

    DEFAULT_SERIAL = 0
  };

  // default instance
  static const LmItemHdr DEFAULT_INSTANCE;

public:

  LmItemHdr();

  // re-constructor
  void Init(lyra_id_t itemid, lyra_id_t h2, int serial);

  // selectors
  lyra_id_t ItemHdr1() const;
  lyra_id_t ItemHdr2() const;
  int Serial() const;
  int Flags() const;
  int Graphic() const;
  int Color1() const;
  int Color2() const;
  int StateFormat() const;
  bool FlagSet(int flag) const;

  // mutators
  void SetItemID(lyra_id_t itemid, lyra_id_t h2);
  void SetSerial(int serial);
  void SetFlags(int flags);
  void SetGraphic(int graphic);
  void SetStateFormat(int format);
  void SetColor1(int color);
  void SetColor2(int color);

  void SetFlag(int flag);
  void ClearFlag(int flag);

  // comparison
  bool Equals(const LmItemHdr& item) const;

  // byteorder conversion
  void ConvertToNetwork();
  void ConvertToHost();

  // read from string, write to string
  int Parse(const TCHAR* str);
  void UnParse(TCHAR* str, int strlen) const;

  // print contents for debugging
  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  lyra_itemhdr_t item_;

};

#ifdef USE_INLINE
#include "LmItemHdr.i"
#endif

// inline functions

inline int operator==(const LmItemHdr& li, const LmItemHdr& ri)
{
  return ((li.ItemHdr1() == ri.ItemHdr1()) && (li.ItemHdr2() == ri.ItemHdr2()) &&
	  (li.Serial() == ri.Serial()));
}

#endif /* INCLUDED_LmItemHdr */
