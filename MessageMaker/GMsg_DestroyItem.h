// GMsg_DestroyItem.h  -*- C++ -*-
// $Id: GMsg_DestroyItem.h,v 1.5 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// destroy an item in the inventory

#ifndef INCLUDED_GMsg_DestroyItem
#define INCLUDED_GMsg_DestroyItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItemHdr.h"

// forward references

// message class

class GMsg_DestroyItem : public LmMesg {

public:

  GMsg_DestroyItem();
  ~GMsg_DestroyItem();

  void Init(const LmItemHdr& itemhdr);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  const LmItemHdr& ItemHeader() const;
  void SetItemHeader(const LmItemHdr& itemhdr);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmItemHdr itemhdr;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_DestroyItem.i"
#endif

#endif /* INCLUDED_GMsg_DestroyItem */
