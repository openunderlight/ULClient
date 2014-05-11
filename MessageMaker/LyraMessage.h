// LyraMessage.h  -*- C++ -*-
// $Id: LyraMessage.h,v 1.1 1997-08-12 15:37:51-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// Global Message definitions

#ifndef INCLUDED_LyraMessage
#define INCLUDED_LyraMessage

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"

////
//// Global Message functions, constants
////

struct LyraMessage {

  // return true if message type is in valid range, and known
  static bool ValidMessageType(int mesg_type);

  // map message type to message name, for debugging
  static const TCHAR* MessageName(int mesg_type);

};

#endif /* INCLUDED_LyraMessage */
