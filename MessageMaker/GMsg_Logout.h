// GMsg_Logout.h  -*- C++ -*-
// $Id: GMsg_Logout.h,v 1.11 1997-09-25 16:26:38-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// logout message

#ifndef INCLUDED_GMsg_Logout
#define INCLUDED_GMsg_Logout

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_Logout : public LmMesg {

public:

  enum {
    LOGOUT_NORMAL     = 'L', // client sent logout
    LOGOUT_FAKE       = 'F', // server faked logout (due to client disconnection)
    LOGOUT_POSSESS    = 'P', // gamemaster logout to possess a nightmare
    LOGOUT_SERVERDOWN = 'S'  // logout due to server being shut down
  };

public:

  GMsg_Logout();
  ~GMsg_Logout();

  void Init(int status = LOGOUT_NORMAL, int num_items = 0);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Status() const;
  int NumItems() const;
  int ItemSerial(int itemnum) const;
  int ItemPosition(int itemnum) const;
  int ItemFlags(int itemnum) const;

  int ItemX(int itemnum) const; // server's use only

  // mutators
  void SetStatus(int status);
  void SetNumItems(int num_items);
  void SetItem(int itemnum, int serial, int pos, int flags);

public: // logically private

  struct item_state_t {
    int serial;  // the item's serial number (uniquer)
    short pos;   // its inventory position (NOTE: only the low 8 bits are saved)
    short flags; // its flags (NOTE: only the low 8 bits are saved)
  };

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // valid index into inventory
  static bool item_num_ok(int item_num);

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    short status;
    short num_items;
    item_state_t items[Lyra::INVENTORY_MAX];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_Logout.i"
#endif

#endif /* INCLUDED_GMsg_Logout */
