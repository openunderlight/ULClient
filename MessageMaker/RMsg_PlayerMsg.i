// RMsg_PlayerMsg.i  -*- C++ -*-
// $Id: RMsg_PlayerMsg.i,v 1.3 1997-07-18 17:26:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_PlayerMsg::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t RMsg_PlayerMsg::SenderID() const
{
  return data_.senderid;
}

INLINE lyra_id_t RMsg_PlayerMsg::ReceiverID() const
{
  return data_.receiverid;
}

INLINE int RMsg_PlayerMsg::MsgType() const
{
  return data_.mtype;
}

INLINE int RMsg_PlayerMsg::State1() const
{
  return data_.state1;
}

INLINE int RMsg_PlayerMsg::State2() const
{
  return data_.state2;
}

INLINE int RMsg_PlayerMsg::State3() const
{
	return data_.state3;
}

INLINE void RMsg_PlayerMsg::SetSenderID(lyra_id_t playerid)
{
  data_.senderid = playerid;
}

INLINE void RMsg_PlayerMsg::SetReceiverID(lyra_id_t playerid)
{
  data_.receiverid = playerid;
}

INLINE void RMsg_PlayerMsg::SetMsgType(int message_type)
{
  data_.mtype = message_type;
}

INLINE void RMsg_PlayerMsg::SetState1(int byte1)
{
  data_.state1 = byte1;
}

INLINE void RMsg_PlayerMsg::SetState2(int byte2)
{
  data_.state2 = byte2;
}

INLINE void RMsg_PlayerMsg::SetState3(int byte3) 
{
	data_.state3 = byte3;
}