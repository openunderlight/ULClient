// RMsg_Speech.cpp  -*- C++ -*-
// $Id: RMsg_Speech.cpp,v 1.14 1998-04-16 19:03:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_Speech.h"
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

#include "RMsg_Speech.h"
#include "RMsg.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "RMsg_Speech.i"
#endif

////
// constructor
////

RMsg_Speech::RMsg_Speech()
  : LmMesg(RMsg::SPEECH, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(RMsg_Speech::UNKNOWN, Lyra::ID_UNKNOWN, 0, _T("speech"));
}

////
// destructor
////

RMsg_Speech::~RMsg_Speech()
{
  // empty
}

////
// Init: initialize message data values
////

void RMsg_Speech::Init(int stype, lyra_id_t playerid, int babble, const TCHAR* txt)
{
  SetSpeechType(stype);
  SetBabble(babble);
  SetPlayerID(playerid);
  SetSpeechText(txt);
}

////
// hton
////

void RMsg_Speech::hton()
{
  HTONL(data_.playerid);
  HTONS(data_.speech_len);
  // no conversion: data_.speech_text, .speech_type, .babble
}

////
// ntoh
////

void RMsg_Speech::ntoh()
{
  NTOHL(data_.playerid);
  NTOHS(data_.speech_len);
  // no conversion: data_.speech_text, .speech_type, .babble
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_Speech::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_Speech[%p,%d]: "), this, sizeof(RMsg_Speech));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u type=%c babble = %u length=%d text='%s'>\n"),
	    PlayerID(), SpeechType(), Babble(), SpeechLength(), SpeechText());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetSpeechText: copy speech text, recalc message length
////

void RMsg_Speech::SetSpeechText(const TCHAR* txt)
{
 _tcsnccpy(data_.speech_text, txt, sizeof(data_.speech_text));
  TRUNC(data_.speech_text, sizeof(data_.speech_text));
  data_.speech_len =_tcslen(data_.speech_text);
  calc_size();
}

////
// RemoveNewlines: remove cr/nl/tabs from message, so that it will print on a single line
//   also remove % signs, they freak the logging out
////

void RMsg_Speech::RemoveNewlines()
{
  int len =_tcslen(data_.speech_text);
  for (int i = 0; i < len; ++i) {
    switch (data_.speech_text[i]) {
    case '\n':
    case '\r':
    case '\t':
    case '%':
      data_.speech_text[i] = ' ';
      break;
    default:
      break;
    }
  }
}

////
// calc_size: recalculate message length, based on current text
////

void RMsg_Speech::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.speech_text);
  // add string length, plus 1 for null
  size += (SpeechLength() + 1);
  SetMessageSize(size);
}
