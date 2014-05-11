// GMsg_GiveItemAck.h  -*- C++ -*-
// $Id: GMsg_GiveItemAck.h,v 1.1 1997-09-19 16:42:16-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// acknowledge item giver

#ifndef INCLUDED_GMsg_GiveItemAck
#define INCLUDED_GMsg_GiveItemAck

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

class GMsg_GiveItemAck : public LmMesg {

public:

  enum {
    // server->client
    GIVE_ERROR = 'E',
    GIVE_YES = 'Y',
    GIVE_NO = 'N',
    GIVE_REJECT_ALL = 'R'
  };

public:

  GMsg_GiveItemAck();
  ~GMsg_GiveItemAck();

  void Init(int status, const LmItemHdr& itemheader);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Status() const;
  const LmItemHdr& ItemHeader() const;

  // mutators
  void SetStatus(int status);
  void SetItemHeader(const LmItemHdr& itemheader);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int status;
    LmItemHdr itemheader;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_GiveItemAck.i"
#endif

#endif /* INCLUDED_GMsg_GiveItemAck */
