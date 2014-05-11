// LmBit32.cpp  -*- C++ -*-
// $Id: LmBit32.cpp,v 1.3 1997-06-30 20:21:30-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmBit32.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "LmBit32.h"
#include "LyraDefs.h"

// optionally inline methods

#ifndef USE_INLINE
#include "LmBit32.i"
#endif

////
// ConvertToHost
////

void LmBit32::ConvertToHost()
{
  NTOHL(x_);
}

////
// ConvertToNetwork
////

void LmBit32::ConvertToNetwork()
{
  HTONL(x_);
}

////
// UnParse
////

void LmBit32::UnParse(char* str, int len) const
{
  // don't bother if string is too short
  if (len < 36) { // 32 + 3 .'s, 1 NULL
    return;
  }
  // print 31 to 0, left to right
  int j = 0;
  for (int i = 31; i >= 0; --i, ++j) {
    if (x_ & (1 << i)) {
      str[j] = '1';
    }
    else {
      str[j] = '0';
    }
    if (i && ((i % 8) == 0)) {
      str[++j] = '.';  // add .'s in between bytes, except for last one
    }
  }
  str[j] = '\0';
}
