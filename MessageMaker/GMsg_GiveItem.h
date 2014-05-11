// GMsg_GiveItem.h  -*- C++ -*-
// $Id: GMsg_GiveItem.h,v 1.1 1997-09-18 16:51:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_GMsg_GiveItem
#define INCLUDED_GMsg_GiveItem

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

class GMsg_GiveItem : public LmMesg {

public:

  GMsg_GiveItem();
  ~GMsg_GiveItem();

  void Init(lyra_id_t targetid, const LmItemHdr& itemheader);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t TargetID() const;
  const LmItemHdr& ItemHeader() const;

  // mutators
  void SetTargetID(lyra_id_t targetid);
  void SetItemHeader(const LmItemHdr& itemheader);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t targetid;
    LmItemHdr itemheader;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GiveItem.i"
#endif

#endif /* INCLUDED_GMsg_GiveItem */
