// GMsg_ShowItem.h  -*- C++ -*-
// $Id: GMsg_ShowItem.h,v 1.2 1997-12-11 16:25:07-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_GMsg_ShowItem
#define INCLUDED_GMsg_ShowItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItemHdr.h"

// message class

class GMsg_ShowItem : public LmMesg {

public:

  GMsg_ShowItem();
  ~GMsg_ShowItem();

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
#include "GMsg_ShowItem.i"
#endif

#endif /* INCLUDED_GMsg_ShowItem */
