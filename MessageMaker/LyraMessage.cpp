// LyraMessage.cpp  -*- C++ -*-
// $Id: LyraMessage.cpp,v 1.18 1998-05-01 17:05:24-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LyraMessage.h"
#endif

#include "LyraMessage.h"
#include "RMsg.h"
#include "GMsg.h"
#include "SMsg.h"

////
// MessageName: map message constants to a string with message type
////

struct msg_table_entry {
  int mtype;
  const TCHAR* mname;
};

// pre-processory trickery
#define TABLE_ENTRY(T) { T, _T(#T) }

static const msg_table_entry msg_table[] = {

  // "none" message
  TABLE_ENTRY(Lyra::MSG_NONE),

#ifdef USE_DEBUG

  // Game server messages
  TABLE_ENTRY(GMsg::MIN),
  TABLE_ENTRY(GMsg::LOGIN),
  TABLE_ENTRY(GMsg::LOGINACK),
  TABLE_ENTRY(GMsg::LOGOUT),
  TABLE_ENTRY(GMsg::GETITEM),
  TABLE_ENTRY(GMsg::PUTITEM),
  TABLE_ENTRY(GMsg::ITEMDROP),
  TABLE_ENTRY(GMsg::ITEMPICKUP),
  TABLE_ENTRY(GMsg::UPDATESTATS),
  TABLE_ENTRY(GMsg::UPDATEITEM),
  TABLE_ENTRY(GMsg::SERVERDOWN),
  TABLE_ENTRY(GMsg::SERVERERROR),
  TABLE_ENTRY(GMsg::CREATEITEM),
  TABLE_ENTRY(GMsg::DESTROYITEM),
  TABLE_ENTRY(GMsg::GOTOLEVEL),
  TABLE_ENTRY(GMsg::CHANGESTAT),
  TABLE_ENTRY(GMsg::GOAL),
  TABLE_ENTRY(GMsg::GETGOALHDRS),
  TABLE_ENTRY(GMsg::GETREPORTHDRS),
  TABLE_ENTRY(GMsg::POSTGOAL),
  TABLE_ENTRY(GMsg::POSTREPORT),
  TABLE_ENTRY(GMsg::RCVGOALDETAILS),
  TABLE_ENTRY(GMsg::RCVGOALHDR),
  TABLE_ENTRY(GMsg::RCVGOALBOOKHDR),
  TABLE_ENTRY(GMsg::RCVGOALTEXT),
  TABLE_ENTRY(GMsg::RCVREPORTHDR),
  TABLE_ENTRY(GMsg::RCVREPORTTEXT),
  TABLE_ENTRY(GMsg::DESTROYROOMITEM),
  TABLE_ENTRY(GMsg::GETLEVELPLAYERS),
  TABLE_ENTRY(GMsg::LEVELPLAYERS),
  TABLE_ENTRY(GMsg::PING),
  TABLE_ENTRY(GMsg::CHANGEAVATAR),
  TABLE_ENTRY(GMsg::CREATEPLAYER),
  TABLE_ENTRY(GMsg::CREATEPLAYERACK),
  TABLE_ENTRY(GMsg::GIVEITEM),
  TABLE_ENTRY(GMsg::GIVEITEMACK),
  TABLE_ENTRY(GMsg::TAKEITEM),
  TABLE_ENTRY(GMsg::TAKEITEMACK),
  TABLE_ENTRY(GMsg::LOCATEAVATAR),
  TABLE_ENTRY(GMsg::AGENTLOGIN),
  TABLE_ENTRY(GMsg::LOCATEAVATARACK),
  TABLE_ENTRY(GMsg::VIEWITEM),
  TABLE_ENTRY(GMsg::SHOWITEM),
  TABLE_ENTRY(GMsg::ITEMDESCRIPTION),
  TABLE_ENTRY(GMsg::GETITEMDESCRIPTION),
  TABLE_ENTRY(GMsg::SETAVATARDESCRIPTION),
  TABLE_ENTRY(GMsg::GETPLAYERNAME),
  TABLE_ENTRY(GMsg::RCVPLAYERNAME),
  TABLE_ENTRY(GMsg::MAX),

  // Room server messages
  TABLE_ENTRY(RMsg::MIN),
  TABLE_ENTRY(RMsg::LOGOUT),
  TABLE_ENTRY(RMsg::GOTOROOM),
  TABLE_ENTRY(RMsg::SPEECH),
  TABLE_ENTRY(RMsg::LOGINACK),
  TABLE_ENTRY(RMsg::LEAVEROOM),
  TABLE_ENTRY(RMsg::ENTERROOM),
  TABLE_ENTRY(RMsg::ITEMDROP),
  TABLE_ENTRY(RMsg::ITEMPICKUP),
  TABLE_ENTRY(RMsg::PARTY),
  TABLE_ENTRY(RMsg::PARTYINFO),
  TABLE_ENTRY(RMsg::JOINEDPARTY),
  TABLE_ENTRY(RMsg::SERVERERROR),
  TABLE_ENTRY(RMsg::ITEMHDRDROP),
  TABLE_ENTRY(RMsg::PLAYERMSG),
  TABLE_ENTRY(RMsg::PLAYERUPDATE),
  TABLE_ENTRY(RMsg::UPDATE),
  TABLE_ENTRY(RMsg::PING),
  TABLE_ENTRY(RMsg::CHANGEAVATAR),
  TABLE_ENTRY(RMsg::GETAVATARDESCRIPTION),
  TABLE_ENTRY(RMsg::RCVAVATARDESCRIPTION),
  TABLE_ENTRY(RMsg::MAX),

  // Intra/Inter-server messages
  TABLE_ENTRY(SMsg::MIN),
  TABLE_ENTRY(SMsg::LOGIN),
  TABLE_ENTRY(SMsg::LOGOUT),
  TABLE_ENTRY(SMsg::PING),
  TABLE_ENTRY(SMsg::SERVERERROR),
  TABLE_ENTRY(SMsg::GETITEM),
  TABLE_ENTRY(SMsg::PUTITEM),
  TABLE_ENTRY(SMsg::ITEMDROP),
  TABLE_ENTRY(SMsg::ITEMPICKUP),
  TABLE_ENTRY(SMsg::PROXY),
  TABLE_ENTRY(SMsg::LEVELLOGIN),
  TABLE_ENTRY(SMsg::LS_LOGIN),
  TABLE_ENTRY(SMsg::LS_ACTION),
  TABLE_ENTRY(SMsg::GS_ACTION),
  TABLE_ENTRY(SMsg::GS_LOGIN),
  TABLE_ENTRY(SMsg::GS_LOGOUT),
  TABLE_ENTRY(SMsg::DESTROYROOMITEM),
  TABLE_ENTRY(SMsg::GETLEVELPLAYERS),
  TABLE_ENTRY(SMsg::RECVMESG),
  TABLE_ENTRY(SMsg::SENDMESG),
  TABLE_ENTRY(SMsg::GETSERVERSTATUS),
  TABLE_ENTRY(SMsg::SERVERSTATUS),
  TABLE_ENTRY(SMsg::PLAYERSTATUS),
  TABLE_ENTRY(SMsg::GIVEITEM),
  TABLE_ENTRY(SMsg::GIVEITEMACK),
  TABLE_ENTRY(SMsg::TAKEITEMACK),
  TABLE_ENTRY(SMsg::LOCATEAVATAR),
  TABLE_ENTRY(SMsg::PARTYLEADER),
  TABLE_ENTRY(SMsg::CONNSTATUS),
  TABLE_ENTRY(SMsg::SHOWITEM),
  TABLE_ENTRY(SMsg::SETAVATARDESCRIPTION),
  TABLE_ENTRY(SMsg::MAX),

#endif /* USE_DEBUG */
};

static const TCHAR* default_name = _T("(unknown)");

static const int msg_table_size = sizeof(msg_table) / sizeof(msg_table_entry);

////
// msg_table_index - return index into message table for given message, or -1 if not found
////

static int msg_table_index(int mesg_type)
{
  // linear search through table
  for (int i = 0; i < msg_table_size; ++i) {
    if (msg_table[i].mtype == mesg_type) {
      return i;
    }
  }
  return -1;
}

////
// ValidMessageType
////

bool LyraMessage::ValidMessageType(int mesg_type)
{
  // check if it's unknown
  if (mesg_type == Lyra::MSG_NONE) {
    return false;
  }
  // get index
  int i = msg_table_index(mesg_type);
  // message is known if index is valid
  return (i != -1);
}

////
// MessageName
////

const TCHAR* LyraMessage::MessageName(int mesg_type)
{
  int i = msg_table_index(mesg_type);
  return (i == -1) ? default_name : msg_table[i].mname;
}
