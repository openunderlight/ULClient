// RMsg_Logout.h  -*- C++ -*-
// $Id: RMsg_Logout.h,v 1.12 1997-09-10 00:46:56-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// logout/death message

#ifndef INCLUDED_RMsg_Logout
#define INCLUDED_RMsg_Logout

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LmMesg.h"
#include "RMsg.h"

// forward references

// message class

class RMsg_Logout : public LmMesg {

public:

  enum {
    // how/why the player logged out
    UNKNOWN = 'U',  // invalid

    DEATH    = 'D',  // was killed
    GOALBOOK = 'G',  // logging out to read goalbook
    LOGOUT   = 'L',  // normal level to level logout
	FINAL    = 'F',  // final logout from game
  };

public:

  RMsg_Logout();
  ~RMsg_Logout();

  void Init(int status, int num_items = 0);

  // convenience functions
  void InitDeath();
  void InitGoalBook();
  void InitLogout();

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

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // valid index into inventory
  static bool item_num_ok(int item_num);

  // calculate actual message size
  void calc_size();

  struct item_state_t {
    int serial;  // the item's serial number (uniquer)
    short pos;   // its inventory position (NOTE: only the low 8 bits are saved)
    short flags; // its flags (NOTE: only the low 8 bits are saved)
  };

  // message data structure
  struct data_t {
    int status;
    short num_items;
    item_state_t items[Lyra::INVENTORY_MAX];
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_Logout.i"
#endif

#endif /* INCLUDED_RMsg_Logout */
