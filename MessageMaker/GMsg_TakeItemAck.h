// GMsg_TakeItemAck.h  -*- C++ -*-
// $Id: GMsg_TakeItemAck.h,v 1.2 1997-09-19 16:42:16-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_GMsg_TakeItemAck
#define INCLUDED_GMsg_TakeItemAck

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

class GMsg_TakeItemAck : public LmMesg {

public:

  enum {
    // server->client
    TAKE_ERROR = 'E',
    TAKE_OK = 'K',

    // client->server
    TAKE_YES = 'Y',
    TAKE_NO = 'N',
	TAKE_REJECT_ALL = 'R'
  };

public:

  GMsg_TakeItemAck();
  ~GMsg_TakeItemAck();

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
#include "GMsg_TakeItemAck.i"
#endif

#endif /* INCLUDED_GMsg_TakeItemAck */
