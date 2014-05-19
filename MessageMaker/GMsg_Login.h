// GMsg_Login.h  -*- C++ -*-
// $Id: GMsg_Login.h,v 1.14 1998-04-17 16:37:44-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// login message

#ifndef INCLUDED_GMsg_Login
#define INCLUDED_GMsg_Login

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

class GMsg_Login : public LmMesg {

public:

  enum {
    // default values
    DEFAULT_VERSION = 0
  };

public:

  GMsg_Login();
  ~GMsg_Login();

  void Init(int version, const TCHAR* playername, int server_port, short pmare_type,
	    int subversion, short tcp_only);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Version() const;
  int SubVersion() const;
  const TCHAR* PlayerName() const;
  const MD5Hash_t* HashPtr() const;
  // const TCHAR* Password() const;
  int ServerPort() const;
  //int Firewall() const;
  short PMareType() const;
  short TCPOnly() const;

  // mutators
  void SetVersion(int version);
  void SetSubVersion(int subversion);
  void SetPlayerName(const TCHAR* playername);
  //void SetPassword(const TCHAR* password);
  void SetHash(const MD5Hash_t hash);
  void SetServerPort(int server_port);
  void SetPMareType(short pmare_type);  
  void SetTCPOnly(short tcp_only);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // variable-length message
  void calc_size();

  // message data structure
  struct data_t {
    int version;                            // client version
    TCHAR playername[Lyra::PLAYERNAME_MAX];  // name
    // TCHAR password[Lyra::PASSWORD_MAX];      // password
	MD5Hash_t hash;							// md5 hash of server challenge
    int serv_port;                          // UDP port number for S->C local group updates
    short pmare_type;							// for pmares, type selected
	short tcp_only;							// TCP only, for firewalls/NAT/etc.
    int subversion;                         // another nt to use for version checking
	//int firewall;							// indicates server must send back updates from same UDP address as they're sent to
  } data_;

};

#ifdef USE_INLINE
#include "GMsg_Login.i"
#endif

#endif /* INCLUDED_GMsg_Login */
