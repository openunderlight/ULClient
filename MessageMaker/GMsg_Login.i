// GMsg_Login.i  -*- C++ -*-
// $Id: GMsg_Login.i,v 1.8 1998-04-17 16:37:44-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_Login::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int GMsg_Login::Version() const
{
  return data_.version;
}

INLINE int GMsg_Login::SubVersion() const
{
  return data_.subversion;
}

INLINE const TCHAR* GMsg_Login::PlayerName() const
{
  return data_.playername;
}

//INLINE const TCHAR* GMsg_Login::Password() const
//{
//  return data_.password;
//}
INLINE const MD5Hash_t* GMsg_Login::HashPtr() const
{
  return &(data_.hash);
}

INLINE int GMsg_Login::ServerPort() const
{
  return data_.serv_port;
}

//INLINE int GMsg_Login::Firewall() const
//{
//  return data_.firewall;
//}


INLINE short GMsg_Login::PMareType() const
{
  return data_.pmare_type;
}


INLINE short GMsg_Login::TCPOnly() const
{  return data_.tcp_only; }

INLINE void GMsg_Login::SetVersion(int version)
{
  data_.version = version;
}

INLINE void GMsg_Login::SetSubVersion(int subversion)
{
  data_.subversion = subversion;
}

INLINE void GMsg_Login::SetServerPort(int serv_port)
{
  data_.serv_port = serv_port;
}

//INLINE void GMsg_Login::SetFirewall(int firewall)
//{
//  data_.firewall = firewall;
//}


INLINE void GMsg_Login::SetPMareType(short pmare_type)
{
  data_.pmare_type = pmare_type;
}

INLINE void GMsg_Login::SetTCPOnly(short tcp_only)
 {  data_.tcp_only = tcp_only;  }
