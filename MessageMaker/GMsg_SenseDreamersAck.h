// GMsg_SenseDreamersAck.h  -*- C++ -*-
// $Id: GMsg_SenseDreamersAck.h,v 1.3 1998-02-26 14:35:44-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate player

#ifndef INCLUDED_GMsg_SenseDreamersAck
#define INCLUDED_GMsg_SenseDreamersAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

const int PLANES_SENSED_COUNT = 4;

class GMsg_SenseDreamersAck : public LmMesg {

public:

  GMsg_SenseDreamersAck();
  ~GMsg_SenseDreamersAck();

  void Init(void);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  //int NumDreamers(int index) const;
  lyra_id_t LevelID(int index) const;

  // mutators
  //void SetNumDreamers(int num_dreamers, int index);
  void SetLevelID(lyra_id_t level_id, int index);


private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    //unsigned short num_dreamers[PLANES_SENSED_COUNT];
	unsigned char level_id[PLANES_SENSED_COUNT ];
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_SenseDreamersAck.i"
#endif

#endif /* INCLUDED_GMsg_SenseDreamersAck */
