// RMsg_LoginAck.i  -*- C++ -*-
// $Id: RMsg_LoginAck.i,v 1.9 1997-11-17 14:05:06-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_LoginAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int RMsg_LoginAck::Status() const
{
  return data_.status;
}

INLINE lyra_id_t RMsg_LoginAck::RoomID() const
{
  return data_.roomid;
}

INLINE lyra_id_t RMsg_LoginAck::LevelID() const
{
  return data_.levelid;
}

INLINE unsigned long RMsg_LoginAck::ServerIP() const
{
  return data_.server_ip;
}

INLINE int RMsg_LoginAck::ServerPort() const
{
  return data_.server_port;
}

INLINE void RMsg_LoginAck::SetStatus(int status)
{
  data_.status = status;
}

INLINE void RMsg_LoginAck::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void RMsg_LoginAck::SetLevelID(lyra_id_t levelid)
{
  data_.levelid = levelid;
}

INLINE void RMsg_LoginAck::SetServerIP(unsigned long server_ip)
{
  data_.server_ip = server_ip;
}

INLINE void RMsg_LoginAck::SetServerPort(int server_port)
{
  data_.server_port = server_port;
}
