// GMsg_LoginAck.i  -*- C++ -*-
// $Id: GMsg_LoginAck.i,v 1.11 1997-10-01 17:53:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_LoginAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE bool GMsg_LoginAck::item_num_ok(int item_num)
{
  return INDEX_OK(item_num, 0, Lyra::INVENTORY_MAX);
}

INLINE int GMsg_LoginAck::Version() const
{
  return data_.version;
}

INLINE int GMsg_LoginAck::Build() const
{
  return Lyra::GM_DELTA;
//  return data_.build;
}

INLINE int GMsg_LoginAck::SubBuild() const
{
  return 1088;
//  return data_.subbuild;
}

INLINE int GMsg_LoginAck::Status() const
{
  return data_.request_status;
}

INLINE const TCHAR* GMsg_LoginAck::Description() const
{
  return data_.description;
}

INLINE lyra_id_t GMsg_LoginAck::PlayerID() const
{
  return data_.playerid;
}

INLINE int GMsg_LoginAck::ServerPort() const
{
  return data_.server_port;
}

INLINE const LmStats& GMsg_LoginAck::PlayerStats() const
{
  return data_.stats;
}

INLINE const LmAvatar& GMsg_LoginAck::Avatar() const
{
  return data_.avatar;
}

INLINE const LmArts& GMsg_LoginAck::Arts() const
{
  return data_.arts;
}

INLINE int GMsg_LoginAck::XPGained() const
{
  return data_.xp_gain;
}

INLINE int GMsg_LoginAck::XPLost() const
{
  return data_.xp_loss;
}

INLINE int GMsg_LoginAck::NumItems() const
{
  return data_.num_items;
}

INLINE short GMsg_LoginAck::MaxMinutesOnline() const
{
  return data_.max_minutes_online;
}

INLINE short GMsg_LoginAck::SessionMinutes() const
{
  return data_.session_minutes;
}

INLINE short GMsg_LoginAck::X() const
{
  return data_.x;
}

INLINE short GMsg_LoginAck::Y() const
{
  return data_.y;
}

INLINE unsigned char GMsg_LoginAck::LevelID() const
{
  return data_.level_id;
}

INLINE unsigned char GMsg_LoginAck::Gamesite() const
{
  return data_.gamesite;
}

INLINE short GMsg_LoginAck::PPoints() const
{
  return data_.ppoints;
}

INLINE short GMsg_LoginAck::PPPool() const
{
  return data_.pp_pool;
}

INLINE lyra_id_t GMsg_LoginAck::GamesiteID() const
{
  return data_.gamesite_id;
}

/* INLINE unsigned int GMsg_LoginAck::LoginTime() const
{
  return data_.login_time;
} */
 
INLINE const LmItem& GMsg_LoginAck::Item(int num) const
{
  return item_num_ok(num) ? data_.items[num].item : LmItem::DEFAULT_INSTANCE;
}

INLINE int GMsg_LoginAck::ItemPosition(int num) const
{
  return item_num_ok(num) ? data_.items[num].pos : -1;
}

INLINE int GMsg_LoginAck::ItemFlags(int num) const
{
  return item_num_ok(num) ? data_.items[num].flags : 0;
}

INLINE void GMsg_LoginAck::SetVersion(int version)
{
  data_.version = version;
}

INLINE void GMsg_LoginAck::SetStatus(int status)
{
  data_.request_status = status;
}

INLINE void GMsg_LoginAck::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void GMsg_LoginAck::SetServerPort(int server_port)
{
  data_.server_port = server_port;
}

INLINE void GMsg_LoginAck::SetPlayerStats(const LmStats& stats)
{
  data_.stats = stats;
}

INLINE void GMsg_LoginAck::SetAvatar(const LmAvatar& avatar)
{
  data_.avatar = avatar;
}

INLINE void GMsg_LoginAck::SetArts(const LmArts& arts)
{
  data_.arts = arts;
}

INLINE void GMsg_LoginAck::SetXPGained(int xp_gain)
{
  data_.xp_gain = xp_gain;
}

INLINE void GMsg_LoginAck::SetXPLost(int xp_loss)
{
  data_.xp_loss = xp_loss;
}

INLINE void GMsg_LoginAck::SetMaxMinutesOnline(short minutes)
{
  data_.max_minutes_online = minutes;
}

INLINE void GMsg_LoginAck::SetSessionMinutes(short minutes)
{
  data_.session_minutes = minutes;
}

INLINE void GMsg_LoginAck::SetX(short x)
{
  data_.x = x;
}

INLINE void GMsg_LoginAck::SetY(short y)
{
  data_.y = y;
}

INLINE void GMsg_LoginAck::SetGamesite(unsigned char gamesite)
{
  data_.gamesite = gamesite;
}

INLINE void GMsg_LoginAck::SetPPoints(short value)
{
  data_.ppoints= value;
}

INLINE void GMsg_LoginAck::SetPPPool(short value)
{
  data_.pp_pool = value;
}

INLINE void GMsg_LoginAck::SetGamesiteID(lyra_id_t gamesite_id)
{
  data_.gamesite_id = gamesite_id;
}

INLINE void GMsg_LoginAck::SetLevelID(unsigned char level_id)
{
  data_.level_id = level_id;
}

/*INLINE void GMsg_LoginAck::SetLoginTime(unsigned int login_time)
{
  data_.login_time = login_time;
}*/

INLINE void GMsg_LoginAck::SetItem(int num, const LmItem& item, int x_value)
{
  if (item_num_ok(num)) {
    data_.items[num].item = item;
    // x_value: low 8 bits = position, high 8 bits = flags
    data_.items[num].pos = x_value & 0xFF;
    data_.items[num].flags = (x_value >> 8) & 0xFF;
  }
}
