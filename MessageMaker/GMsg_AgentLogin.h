// GMsg_AgentLogin.h  -*- C++ -*-
// $Id: GMsg_AgentLogin.h,v 1.1 1997-10-15 16:22:40-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// login message

#ifndef INCLUDED_GMsg_AgentLogin
#define INCLUDED_GMsg_AgentLogin

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"
#include "LnMD5.h"

// forward references

// message class

class GMsg_AgentLogin : public LmMesg {

public:

  enum {
    // default values
    DEFAULT_VERSION = 0
  };

public:

  GMsg_AgentLogin();
  ~GMsg_AgentLogin();

  void Init(int version, const TCHAR* playername, int server_port, lyra_id_t billing_id, short tcp_only);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;
  const TCHAR* PlayerName() const;
  //const TCHAR* Password() const;
  const MD5Hash_t* HashPtr() const;
  int ServerPort() const;
  short TCPOnly() const;
  lyra_id_t BillingID() const;

  // mutators
  void SetVersion(int version);
  void SetPlayerName(const TCHAR* playername);
  //void SetPassword(const TCHAR* password);
  void SetHash(const MD5Hash_t hash);
  void SetServerPort(int server_port);
  void SetBillingID(lyra_id_t billing_id);
  void SetTCPOnly(short tcp_only);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int version;                            // client version
    TCHAR playername[Lyra::PLAYERNAME_MAX];  // name
	//TCHAR password[Lyra::PASSWORD_MAX];
	MD5Hash_t hash;							// md5 hash
    int serv_port;                          // UDP port number for S->C local group updates
    lyra_id_t billing_id;                     // user's billing id
	short tcp_only;
	short unusued_;
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_AgentLogin.i"
#endif

#endif /* INCLUDED_GMsg_AgentLogin */
