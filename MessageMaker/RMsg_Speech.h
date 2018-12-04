// RMsg_Speech.h  -*- C++ -*-
// $Id: RMsg_Speech.h,v 1.17 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// speech message

#ifndef INCLUDED_RMsg_Speech
#define INCLUDED_RMsg_Speech

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// forward references

// message class

class RMsg_Speech : public LmMesg {

public:

  enum {
    // constants
    MAX_SPEECHLEN = Lyra::MAX_SPEECHLEN,

    // types of speech
    UNKNOWN        = 'U',  // invalid

    REPORT_BUG     = 'b',  // bug report
    REPORT_CHEAT   = 'c',  // cheater report
    AUTO_CHEAT     = 'C',  // auto-generated cheat report
	DISTRESS_CALL  = 'D',
    REPORT_DEBUG   = 'd',  // debug report (auto-generated)
    EMOTE          = 'E',  // emote
    GLOBALSHOUT    = 'G',  // level-wide shout
    SHOUT          = 'H',  // room-wide shout
    MONSTER_SPEECH = 'M',  // (player) monster speech
    PARTY          = 'P',  // party-only speech
    REPORT_QUEST   = 'q',  // quest report
	RP			   = 'r',  // role play report
    RAW_EMOTE      = 'R',  // "raw" emote
    SYSTEM_SPEECH  = 's',  // "system" speech
    SPEECH         = 'S',  // standard speech
    SERVER_TEXT    = 'T',  // server-initiated speech to clients
    SYSTEM_WHISPER = 'w',  // "system" whisper
    TELL_IP        = 't',  // roger wilco IP address
    WHISPER        = 'W',  // person-to-person whisper
	WHISPER_EMOTE  = 'Z',  // emote showing a whisper is happening
  };

public:

  RMsg_Speech();
  ~RMsg_Speech();

  void Init(int stype, lyra_id_t playerid, int babble, const TCHAR* txt);

  // convenience functions
  void InitSpeech(lyra_id_t playerid, const TCHAR* txt);
  void InitParty(lyra_id_t playerid, const TCHAR* txt);
  void InitShout(lyra_id_t playerid, const TCHAR* txt);
  void InitWhisper(lyra_id_t playerid, const TCHAR* txt);
  void InitGlobalShout(lyra_id_t playerid, const TCHAR* txt);
  void InitEmote(lyra_id_t playerid, const TCHAR* txt);
  void InitMonsterSpeech(lyra_id_t playerid, const TCHAR* txt);
  void InitRawEmote(lyra_id_t playerid, const TCHAR* txt);
  void InitSystemSpeech(lyra_id_t playerid, const TCHAR* txt);
  void InitSystemWhisper(lyra_id_t playerid, const TCHAR* txt);
  void InitReportBug(lyra_id_t playerid, const TCHAR* txt);
  void InitReportCheat(lyra_id_t playerid, const TCHAR* txt);
  void InitAutoCheat(lyra_id_t playerid, const TCHAR* txt);
  void InitReportDebug(lyra_id_t playerid, const TCHAR* txt);
  void InitServerText(lyra_id_t playerid, const TCHAR* txt);
  void SetUniverseWide(bool universe);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  bool IsUniverseWide() const;
  int SpeechType() const;
  const TCHAR* SpeechText() const;
  int SpeechLength() const;
  int Babble() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetSpeechType(int stype);
  void SetBabble(int babble);
  void SetSpeechText(const TCHAR* txt);

  void RemoveNewlines();

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t playerid;               // playerid of speaker/target
    char  speech_type;                // type of speech
	char  babble;					  // = 1 if babbled (marespeak)
    short speech_len;                 // length of text (not including null char)
    TCHAR speech_text[MAX_SPEECHLEN];  // null-terminated speech text
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_Speech.i"
#endif

#endif /* INCLUDED_RMsg_Speech */
