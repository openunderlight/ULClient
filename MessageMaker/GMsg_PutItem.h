// GMsg_PutItem.h  -*- C++ -*-
// $Id: GMsg_PutItem.h,v 1.11 1997-07-18 17:25:43-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// drop item request message

#ifndef INCLUDED_GMsg_PutItem
#define INCLUDED_GMsg_PutItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItemHdr.h"
#include "LmPosition.h"

// forward references

// message class

class GMsg_PutItem : public LmMesg {

public:

  enum {
    // constants
    DEFAULT_TTL = 600,             // seconds - 10 minutes
    MAX_TTL = (60 * 60 * 24) * 24  // one day
  };

public:

  GMsg_PutItem();
  ~GMsg_PutItem();

  void Init(lyra_id_t roomid, const LmItemHdr& itemhdr, const LmPosition& position,
	    int ttl = DEFAULT_TTL);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t RoomID() const;
  int TTL() const;
  const LmItemHdr& ItemHeader() const;
  const LmPosition& Position() const;

  // mutators
  void SetRoomID(lyra_id_t roomid);
  void SetTTL(int ttl);
  void SetItemHeader(const LmItemHdr& itemhdr);
  void SetPosition(const LmPosition& position);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t roomid;
    LmItemHdr itemhdr;
    LmPosition position;
    int ttl;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_PutItem.i"
#endif

#endif /* INCLUDED_GMsg_PutItem */
