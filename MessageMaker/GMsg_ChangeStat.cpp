// GMsg_ChangeStat.cpp  -*- C++ -*-
// $Id: GMsg_ChangeStat.cpp,v 1.3 1997-07-29 19:32:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_ChangeStat.h"
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

#include "GMsg_ChangeStat.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_ChangeStat.i"
#endif

////
// constructor
////

GMsg_ChangeStat::GMsg_ChangeStat()
  : LmMesg(GMsg::CHANGESTAT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

GMsg_ChangeStat::~GMsg_ChangeStat()
{
  // empty
}

////
// Init
////

void GMsg_ChangeStat::Init(int num_changes)
{
  SetNumChanges(num_changes);
}

////
// hton
////

void GMsg_ChangeStat::hton()
{
  for (int i = 0; i < NumChanges(); ++i) {
    HTONS(data_.changes[i].requesttype);
    HTONS(data_.changes[i].stat);
    HTONL(data_.changes[i].value);
  }
}

////
// ntoh
////

void GMsg_ChangeStat::ntoh()
{
  calc_changes();
  for (int i = 0; i < NumChanges(); ++i) {
    NTOHS(data_.changes[i].requesttype);
    NTOHS(data_.changes[i].stat);
    NTOHL(data_.changes[i].value);
  }
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_ChangeStat::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_ChangeStat[%p,%d]: changes=%d>\n_T("), this, sizeof(GMsg_ChangeStat), NumChanges());
  if (ByteOrder() == ByteOrder::HOST) {
    for (int i = 0; i < NumChanges(); ++i) {
      INDENT(indent + 1, f);
     _ftprintf(f, _T("change %d: req=%d stat=%d val=%d\n_T("), i, RequestType(i), Stat(i), Value(i));
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
// SetNumChanges
////

void GMsg_ChangeStat::SetNumChanges(int num_changes)
{
  // avoid bad range
  if (change_num_ok(num_changes)) {
    num_changes_ = num_changes;
  }
  else {
    num_changes_ = 0;
  }
  calc_size();
}

////
// calc_changes - calculate number of changes based on message size
////

void GMsg_ChangeStat::calc_changes()
{
  int msgsize = MessageSize();
  // determine portion of message size related to fixed fields
  int fixed_size = sizeof(data_t) - sizeof(data_.changes);
  // subtract this from the overall message size; result is the
  // size attributable to the variable-sized field
  msgsize -= fixed_size;
  // calculate number of changes
  SetNumChanges(msgsize / sizeof(change_t));
}

////
// calc_size - calculate message size based on number of changes
////

void GMsg_ChangeStat::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.changes);
  // add space for changes
  size += NumChanges() * sizeof(change_t);
  SetMessageSize(size);
}
