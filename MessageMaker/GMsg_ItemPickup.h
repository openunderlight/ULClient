// GMsg_ItemPickup.h  -*- C++ -*-
// $Id: GMsg_ItemPickup.h,v 1.10 1997-08-29 18:05:06-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item pickup message

#ifndef INCLUDED_GMsg_ItemPickup
#define INCLUDED_GMsg_ItemPickup

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LmItem.h"

// forward references

// message class

class GMsg_ItemPickup : public LmMesg {

public:

  enum {
    // constants
    PICKUP_UNKNOWN     = 'U',  // invalid

    PICKUP_CREATE      = 'C',  // item created
    PICKUP_ERRORCREATE = 'D',  // item not created
    PICKUP_ERROR       = 'E',  // item not acquired
    PICKUP_OK          = 'K'   // item acquired
  };

public:

  GMsg_ItemPickup();
  ~GMsg_ItemPickup();

  void Init(int status, const LmItem& item);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Status() const;
  const LmItem& Item() const;

  // mutators
  void SetStatus(int status);
  void SetItem(const LmItem& item);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int status;
    LmItem item;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_ItemPickup.i"
#endif

#endif /* INCLUDED_GMsg_ItemPickup */
