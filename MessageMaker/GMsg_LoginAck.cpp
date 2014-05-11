// GMsg_LoginAck.cpp  -*- C++ -*-
// $Id: GMsg_LoginAck.cpp,v 1.19 1997-10-01 17:53:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "GMsg_LoginAck.h"
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

#include "GMsg_LoginAck.h"

#ifndef USE_INLINE
#include "GMsg_LoginAck.i"
#endif

////
// constructor
////

GMsg_LoginAck::GMsg_LoginAck()
  : LmMesg(GMsg::LOGINACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  SetVersion(DEFAULT_VERSION);
  SetStatus(LOGIN_UNUSED);
  SetPlayerID(Lyra::ID_UNKNOWN);
  SetServerPort(Lyra::PORT_UNKNOWN);
  SetXPGained(0);
  SetXPLost(0);
  SetNumItems(0);
  //SetLoginTime(0);
  SetMaxMinutesOnline(0);
  SetX(0);
  SetY(0);
  SetLevelID(0);
  SetGamesite(GAMESITE_LYRA);
  SetGamesiteID(0);
  SetPPoints(0);
  SetPPPool(0);
  for (int i = 0; i < Lyra::INVENTORY_MAX; ++i) {
    SetItem(i, LmItem::DEFAULT_INSTANCE, 0);
  }
  SetDescription(_T("none"));
}

////
// destructor
////

GMsg_LoginAck::~GMsg_LoginAck()
{
  // empty
}

////
// Init
////

void GMsg_LoginAck::Init(int version, int status, lyra_id_t playerid, int server_port,
			 const LmStats& stats, const LmAvatar& avatar, const LmArts& arts,
			 int xp_gain, int xp_loss, int num_items, short max_minutes_online, 
			 short session_minutes,  unsigned short x, unsigned short y, 
			 unsigned char level_id, unsigned char gamesite, lyra_id_t gamesite_id,
			 unsigned short ppoints, unsigned short pp_pool, 
			 const TCHAR* description)
{
  SetVersion(version);
  SetStatus(status);
  SetPlayerID(playerid);
  SetServerPort(server_port);
  SetPlayerStats(stats);
  SetAvatar(avatar);
  SetArts(arts);
  SetXPGained(xp_gain);
  SetXPLost(xp_loss);
  SetNumItems(num_items);
  SetDescription(description);
  SetMaxMinutesOnline(max_minutes_online);
  SetSessionMinutes(session_minutes);
  SetX(x);
  SetY(y);
  SetLevelID(level_id);
  SetGamesite(gamesite);
  SetGamesiteID(gamesite_id);
  SetPPoints(ppoints);
  SetPPPool(pp_pool);

}

////
// hton
////

void GMsg_LoginAck::hton()
{
  HTONL(data_.playerid);
  data_.stats.ConvertToNetwork();
  data_.avatar.ConvertToNetwork();
  data_.arts.ConvertToNetwork();
  HTONL(data_.xp_gain);
  HTONL(data_.xp_loss);
  //HTONL(data_.login_time);
  HTONS(data_.version);
  HTONS(data_.request_status);
  HTONS(data_.server_port);
  for (int i = 0; i < data_.num_items; ++i) {
    data_.items[i].item.ConvertToNetwork();
    HTONS(data_.items[i].pos);
    HTONS(data_.items[i].flags);
  }
  HTONS(data_.num_items);  
  HTONS(data_.x);
  HTONS(data_.y);
}

////
// ntoh
////

void GMsg_LoginAck::ntoh()
{
  NTOHL(data_.playerid);
  data_.stats.ConvertToHost();
  data_.avatar.ConvertToHost();
  data_.arts.ConvertToHost();
  NTOHL(data_.xp_gain);
  NTOHL(data_.xp_loss);
  //NTOHL(data_.login_time);
  NTOHS(data_.version);
  NTOHS(data_.request_status);
  NTOHS(data_.server_port);
  NTOHS(data_.x);
  NTOHS(data_.y);
  NTOHS(data_.num_items);
  for (int i = 0; i < data_.num_items; ++i) {
    data_.items[i].item.ConvertToHost();
    NTOHS(data_.items[i].pos);
    NTOHS(data_.items[i].flags);
  }
  calc_size(); // variable-length message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void GMsg_LoginAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GMsg_LoginAck[%p,%d]: "), this, sizeof(GMsg_LoginAck));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("version=%d status=%c playerid=%u servport=%d xpgain=%d xploss=%d x=%d y=%d level_id=%dnumitems=%d>\n"),
	    Version(), Status(), PlayerID(), ServerPort(), XPGained(), XPLost(), X(), Y(), LevelID(), NumItems());
    PlayerStats().Dump(f, indent + 1);
    Avatar().Dump(f, indent + 1);
    Arts().Dump(f, indent + 1);
    for (int i = 0; i < NumItems(); ++i) {
      Item(i).Dump(f, indent + 1);
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
// SetNumItems
////

void GMsg_LoginAck::SetNumItems(int num_items)
{
  // prevent overflow
  data_.num_items = MIN(num_items, Lyra::INVENTORY_MAX);
  calc_size();
}

////
// SetDescription
////

void GMsg_LoginAck::SetDescription(const TCHAR* description)
{
 _tcsnccpy(data_.description, description, sizeof(data_.description));
  TRUNC(data_.description, sizeof(data_.description));
  //  calc_size();
}

////
// calc_size
////

void GMsg_LoginAck::calc_size()
{
  // initial size: entire struct, minus variable-length inventory minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.items);
  // add inventory length
  size += (data_.num_items * sizeof(item_state_t));
  SetMessageSize(size);
}
