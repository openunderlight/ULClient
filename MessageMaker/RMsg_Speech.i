// RMsg_Speech.i  -*- C++ -*-
// $Id: RMsg_Speech.i,v 1.8 1998-04-17 16:27:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_Speech::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE void RMsg_Speech::InitSpeech(lyra_id_t playerid, const TCHAR* txt)
{
  Init(SPEECH, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitShout(lyra_id_t playerid, const TCHAR* txt)
{
  Init(SHOUT, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitWhisper(lyra_id_t playerid, const TCHAR* txt)
{
  Init(WHISPER, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitGlobalShout(lyra_id_t playerid, const TCHAR* txt)
{
  Init(GLOBALSHOUT, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitEmote(lyra_id_t playerid, const TCHAR* txt)
{
  Init(EMOTE, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitRawEmote(lyra_id_t playerid, const TCHAR* txt)
{
  Init(RAW_EMOTE, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitMonsterSpeech(lyra_id_t playerid, const TCHAR* txt)
{
  Init(MONSTER_SPEECH, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitSystemSpeech(lyra_id_t playerid, const TCHAR* txt)
{
  Init(SYSTEM_SPEECH, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitParty(lyra_id_t playerid, const TCHAR* txt)
{
  Init(PARTY, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitSystemWhisper(lyra_id_t playerid, const TCHAR* txt)
{
  Init(SYSTEM_WHISPER, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitReportBug(lyra_id_t playerid, const TCHAR* txt)
{
  Init(REPORT_BUG, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitReportCheat(lyra_id_t playerid, const TCHAR* txt)
{
  Init(REPORT_CHEAT, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitAutoCheat(lyra_id_t playerid, const TCHAR* txt)
{
  Init(AUTO_CHEAT, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitReportDebug(lyra_id_t playerid, const TCHAR* txt)
{
  Init(REPORT_DEBUG, playerid, 0, txt);
}

INLINE void RMsg_Speech::InitServerText(lyra_id_t playerid, const TCHAR* txt)
{
  Init(SERVER_TEXT, playerid, 0, txt);
}

INLINE int RMsg_Speech::SpeechType() const
{
  return (int)(data_.speech_type & 127);
}

INLINE int RMsg_Speech::Babble() const
{
  return (int)data_.babble;
}

INLINE lyra_id_t RMsg_Speech::PlayerID() const
{
  return data_.playerid;
}

INLINE const TCHAR* RMsg_Speech::SpeechText() const
{
  return data_.speech_text;
}

INLINE int RMsg_Speech::SpeechLength() const
{
  return data_.speech_len;
}

INLINE void RMsg_Speech::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void RMsg_Speech::SetSpeechType(int stype)
{
  data_.speech_type = stype;
}

INLINE void RMsg_Speech::SetBabble(int babble)
{
  data_.babble = babble;
}

INLINE void RMsg_Speech::SetUniverseWide(bool universe)
{
	if(universe)
		data_.speech_type |= (1<<7);
	else
		data_.speech_type &= 127;
}

INLINE bool RMsg_Speech::IsUniverseWide() const
{
	return (data_.speech_type & (1<<7));
}