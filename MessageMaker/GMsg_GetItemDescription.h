// GMsg_GetItemDescription.h  -*- C++ -*-
// $Id: GMsg_GetItemDescription.h,v 1.2 1997-12-11 16:25:07-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// request item description

#ifndef INCLUDED_GMsg_GetItemDescription
#define INCLUDED_GMsg_GetItemDescription

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItemHdr.h"

// message class

class GMsg_GetItemDescription : public LmMesg {

public:

  GMsg_GetItemDescription();
  ~GMsg_GetItemDescription();

  void Init(const LmItemHdr& itemheader);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  const LmItemHdr& ItemHeader() const;

  // mutators
  void SetItemHeader(const LmItemHdr& itemheader);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmItemHdr itemheader;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GetItemDescription.i"
#endif

#endif /* INCLUDED_GMsg_GetItemDescription */
