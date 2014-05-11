// GMsg_ItemDrop.h  -*- C++ -*-
// $Id: GMsg_ItemDrop.h,v 1.11 1997-11-21 23:33:53-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item drop acknowledge message

#ifndef INCLUDED_GMsg_ItemDrop
#define INCLUDED_GMsg_ItemDrop

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

class GMsg_ItemDrop : public LmMesg {

public:

  enum {
    // constants
    DROP_UNKNOWN  = 'U',  // invalid

    DROP_DESTROY  = 'D',  // item destroyed
    DROP_ERROR    = 'E',  // item not dropped/destroyed
    DROP_ROOMFULL = 'F',  // item not dropped due to room being full
    DROP_OK       = 'K'   // item dropped
  };

public:

  GMsg_ItemDrop();
  ~GMsg_ItemDrop();

  void Init(int status, const LmItemHdr& itemhdr);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Status() const;
  const LmItemHdr& ItemHeader() const;

  // mutators
  void SetStatus(int status);
  void SetItemHeader(const LmItemHdr& itemhdr);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int status;
    LmItemHdr itemhdr;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_ItemDrop.i"
#endif

#endif /* INCLUDED_GMsg_ItemDrop */
