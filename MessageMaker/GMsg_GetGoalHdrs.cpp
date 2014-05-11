// GMsg_GetGoalHdrs.cpp  -*- C++ -*-
// $Id: GMsg_GetGoalHdrs.cpp,v 1.6 1998-02-03 23:33:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_GetGoalHdrs.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "GMsg_GetGoalHdrs.h"
#include "LyraDefs.h"
#include "GMsg.h"

#ifndef USE_INLINE
#include "GMsg_GetGoalHdrs.i"
#endif

////
// constructor
////

GMsg_GetGoalHdrs::GMsg_GetGoalHdrs()
  : LmMesg(GMsg::GETGOALHDRS, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0, 0, 0);
}

////
// destructor
////

GMsg_GetGoalHdrs::~GMsg_GetGoalHdrs()
{
  // empty
}

////
// Init
////

void GMsg_GetGoalHdrs::Init(int sessionid, short guild, short level_num, lyra_id_t lastgoal)
{
  SetSessionID(sessionid);
  SetGuild(guild);
  SetLevelNum(level_num);
  SetLastGoal(lastgoal);
}

////
// hton
////

void GMsg_GetGoalHdrs::hton()
{
  HTONL(data_.sessionid);
  HTONS(data_.guild);
  HTONS(data_.level_num);
  HTONL(data_.lastgoal);
}

////
// ntoh
////

void GMsg_GetGoalHdrs::ntoh()
{
  NTOHL(data_.sessionid);
  NTOHS(data_.guild);
  NTOHS(data_.level_num);
  NTOHL(data_.lastgoal);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_GetGoalHdrs::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_GetGoalHdrs[%p,%d]: "), this, sizeof(GMsg_GetGoalHdrs));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("sessionid=%u guild=%d level=%d lastgoal=%u>\n"), SessionID(), Guild(), LevelNum(), LastGoal());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
