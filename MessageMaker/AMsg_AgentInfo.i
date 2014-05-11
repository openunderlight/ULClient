// AMsg_AgentInfo.i  -*- C++ -*-
// $Id: AMsg_AgentInfo.i,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void AMsg_AgentInfo::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t AMsg_AgentInfo::AgentID() const
{
  return data_.agent_id;
}

INLINE short AMsg_AgentInfo::Kills() const
{
  return data_.kills;
}

INLINE short AMsg_AgentInfo::Deaths() const
{
  return data_.deaths;
}

INLINE unsigned char AMsg_AgentInfo::FrameRate() const
{
  return data_.frame_rate;
}

INLINE unsigned char AMsg_AgentInfo::Status() const
{
  return data_.status;
}

INLINE char AMsg_AgentInfo::Busy() const
{
  return data_.busy;
}

INLINE unsigned char AMsg_AgentInfo::RoomID() const
{
  return data_.room_id;
}

INLINE short AMsg_AgentInfo::X() const
{
  return data_.x;
}

INLINE short AMsg_AgentInfo::Y() const
{
  return data_.y;
}

INLINE void AMsg_AgentInfo::SetAgentID(lyra_id_t agent_id)
{
  data_.agent_id = agent_id;
}

INLINE void AMsg_AgentInfo::SetKills(short kills)
{
  data_.kills = kills;
}

INLINE void AMsg_AgentInfo::SetDeaths(short deaths)
{
  data_.deaths = deaths;
}

INLINE void AMsg_AgentInfo::SetFrameRate(unsigned char frame_rate)
{
  data_.frame_rate = frame_rate;
}

INLINE void AMsg_AgentInfo::SetStatus(unsigned char status)
{
  data_.status = status;
}

INLINE void AMsg_AgentInfo::SetBusy(char percent)
{
  data_.busy = percent;
}

INLINE void AMsg_AgentInfo::SetRoomID(unsigned char room_id)
{
  data_.room_id = room_id;
}

INLINE void AMsg_AgentInfo::SetX(short x)
{
  data_.x = x;
}

INLINE void AMsg_AgentInfo::SetY(short y)
{
  data_.y = y;
}
