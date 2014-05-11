// GMsg_ViewItem.h  -*- C++ -*-
// $Id: GMsg_ViewItem.h,v 1.1 1997-12-05 14:44:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// view item from another player

#ifndef INCLUDED_GMsg_ViewItem
#define INCLUDED_GMsg_ViewItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItemHdr.h"
#include "LmItem.h"

// message class

class GMsg_ViewItem : public LmMesg {

public:

  GMsg_ViewItem();
  ~GMsg_ViewItem();

  void Init(lyra_id_t sourceid, const LmItemHdr& itemheader, const TCHAR* itemname);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t SourceID() const;
  const LmItemHdr& ItemHeader() const;
  const TCHAR* ItemName() const;

  // mutators
  void SetSourceID(lyra_id_t sourceid);
  void SetItemHeader(const LmItemHdr& itemheader);
  void SetItemName(const TCHAR* itemname);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t sourceid;
    LmItemHdr itemheader;
    TCHAR itemname[LmItem::NAME_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_ViewItem.i"
#endif

#endif /* INCLUDED_GMsg_ViewItem */
