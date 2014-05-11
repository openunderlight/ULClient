// RMsg_LoginAck.h  -*- C++ -*-
// $Id: RMsg_LoginAck.h,v 1.17 1997-11-17 14:05:06-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// login acknowledgement

#ifndef INCLUDED_RMsg_LoginAck
#define INCLUDED_RMsg_LoginAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// forward references

// message class

class RMsg_LoginAck : public LmMesg {

public:

  enum {
    // ack status
    LOGIN_UNKNOWN        = 'U',

    LOGIN_ALREADYIN      = 'A',
    LOGIN_ERROR          = 'E',
    LOGIN_ROOMFULL       = 'F',
    LOGIN_OK             = 'K',
    LOGIN_LEVELNOTFOUND  = 'L',
    LOGIN_PLAYERNOTFOUND = 'P',
    LOGIN_ROOMNOTFOUND   = 'R',
    LOGIN_SERVERDOWN     = 'S'
  };

public:

  RMsg_LoginAck();
  ~RMsg_LoginAck();

  void Init(int status, lyra_id_t roomid, lyra_id_t levelid);
  void Init(int status, lyra_id_t roomid, lyra_id_t levelid, unsigned long server_ip, int server_port);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Status() const;
  lyra_id_t RoomID() const;
  lyra_id_t LevelID() const;
  unsigned long ServerIP() const;
  int ServerPort() const;

  //mutators
  void SetStatus(int status);
  void SetRoomID(lyra_id_t roomid);
  void SetLevelID(lyra_id_t roomid);
  void SetServerIP(unsigned long server_ip);
  void SetServerPort(int server_port);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int status;
    lyra_id_t roomid;
    lyra_id_t levelid;
    unsigned int server_ip;
    int server_port;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_LoginAck.i"
#endif

#endif /* INCLUDED_RMsg_LoginAck */
