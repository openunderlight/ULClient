// GMsg_AgentLogin.i  -*- C++ -*-
// $Id: GMsg_AgentLogin.i,v 1.1 1997-10-15 16:22:40-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_AgentLogin::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_AgentLogin::Version() const
{
  return data_.version;
}

INLINE short GMsg_AgentLogin::TCPOnly() const 
{
	return data_.tcp_only;
}

INLINE const TCHAR* GMsg_AgentLogin::PlayerName() const
{
  return data_.playername;
}
/*
INLINE const TCHAR* GMsg_AgentLogin::Password() const
{
  return data_.password;
}
*/
INLINE const MD5Hash_t* GMsg_AgentLogin::HashPtr() const
{
  return &(data_.hash);
}

INLINE int GMsg_AgentLogin::ServerPort() const
{
  return data_.serv_port;
}

INLINE lyra_id_t GMsg_AgentLogin::BillingID() const
{
  return data_.billing_id;
}

INLINE void GMsg_AgentLogin::SetVersion(int version)
{
  data_.version = version;
}

INLINE void GMsg_AgentLogin::SetServerPort(int serv_port)
{
  data_.serv_port = serv_port;
}

INLINE void GMsg_AgentLogin::SetBillingID(lyra_id_t billing_id)
{
  data_.billing_id = billing_id;
}
INLINE void GMsg_AgentLogin::SetTCPOnly(short tcp_only)
{
  data_.tcp_only = tcp_only;
}