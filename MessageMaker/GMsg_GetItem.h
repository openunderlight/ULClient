// GMsg_GetItem.h  -*- C++ -*-
// $Id: GMsg_GetItem.h,v 1.8 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item pickup request message

#ifndef INCLUDED_GMsg_GetItem
#define INCLUDED_GMsg_GetItem

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

class GMsg_GetItem : public LmMesg {

public:

  GMsg_GetItem();
  ~GMsg_GetItem();

  void Init(lyra_id_t roomid, const LmItemHdr& itemhdr);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t RoomID() const;
  const LmItemHdr& ItemHeader() const;

  // mutators
  void SetRoomID(lyra_id_t roomid);
  void SetItemHeader(const LmItemHdr& itemhdr);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t roomid;
    LmItemHdr itemhdr;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GetItem.i"
#endif

#endif /* INCLUDED_GMsg_GetItem */
