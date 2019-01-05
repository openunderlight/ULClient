// GMsg_SenseDreamersAck.cpp  -*- C++ -*-
// $Id: GMsg_SenseDreamersAck.cpp,v 1.1 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_SenseDreamersAck.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "GMsg_SenseDreamersAck.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_SenseDreamersAck.i"
#endif

////
// constructor
////

GMsg_SenseDreamersAck::GMsg_SenseDreamersAck()
  : LmMesg(GMsg::SENSEDREAMERSACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init();
}

////
// destructor
////

GMsg_SenseDreamersAck::~GMsg_SenseDreamersAck()
{
  // empty
}

////
// Init
////

void GMsg_SenseDreamersAck::Init(void)
{
  int i;
  for (i=0; i<PLANES_SENSED_COUNT; i++) {
	data_.level_id[i] = 0;
//	data_.num_dreamers[i] = 0;
  }
}

////
// hton
////

void GMsg_SenseDreamersAck::hton()
{
#if 0
  for (int i = 0; i < PLANES_SENSED_COUNT; ++i) {
    // not converted: PlayerName
    HTONS(data_.num_dreamers[i]);
  }
#endif
  HTONL(data_.total);
}

////
// ntoh
////

void GMsg_SenseDreamersAck::ntoh()
{
#if 0
  for (int i = 0; i < PLANES_SENSED_COUNT; ++i) {
    // not converted: PlayerName
    NTOHS(data_.num_dreamers[i]);
  }
#endif
  NTOHL(data_.total);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_SenseDreamersAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_SenseDreamersAck[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
    for (int i = 0; i < PLANES_SENSED_COUNT; ++i) {
      INDENT(indent + 1, f);
	//     _ftprintf(f, _T("level=%u num dreamers=%u\n"), LevelID(i), NumDreamers(i));
	_ftprintf(f, _T("index = %u; level=%u\n"), i, LevelID(i));
    }
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetNumDreamers
////

#if 0
void GMsg_SenseDreamersAck::SetNumDreamers(int num_dreamers, int index)
{
  // avoid bad range
  if ((index < 0) || (index >= PLANES_SENSED_COUNT)) {
    index = 0;
  }
  else {
    data_.num_dreamers[index] = (unsigned short)num_dreamers;
  }
}
#endif

////
// SetLevelID
////

void GMsg_SenseDreamersAck::SetLevelID(lyra_id_t level_id, int index)
{
  // avoid bad range
  if ((index < 0) || (index >= PLANES_SENSED_COUNT)) {
    index = 0;
  }
  else {
    data_.level_id[index] = (unsigned char)level_id;
  }

}


