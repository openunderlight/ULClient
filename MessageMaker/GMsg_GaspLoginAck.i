// GMsg_GaspLoginAck.i  -*- C++ -*-
// $Id: GMsg_GaspLoginAck.i,v 1.1 1998-01-22 15:08:27-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_GaspLoginAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_GaspLoginAck::Version() const
{
  return data_.version;
}

INLINE int GMsg_GaspLoginAck::Build() const
{
  return -1;
//  return data_.build;
}

INLINE int GMsg_GaspLoginAck::SubBuild() const
{
  return -1;
//  return data_.subbuild;
}

INLINE int GMsg_GaspLoginAck::Status() const
{
  return data_.request_status;
}

INLINE int GMsg_GaspLoginAck::ServerPort() const
{
  return data_.server_port;
}

INLINE void GMsg_GaspLoginAck::SetVersion(int version)
{
  data_.version = version;
}

INLINE void GMsg_GaspLoginAck::SetStatus(int status)
{
  data_.request_status = status;
}

INLINE void GMsg_GaspLoginAck::SetServerPort(int server_port)
{
  data_.server_port = server_port;
}
