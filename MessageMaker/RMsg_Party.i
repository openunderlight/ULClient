// RMsg_Party.i  -*- C++ -*-
// $Id: RMsg_Party.i,v 1.6 1998-05-01 17:05:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_Party::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE void RMsg_Party::InitJoin(lyra_id_t playerid, int reason)
{
  Init(JOIN, playerid, reason);
}

INLINE void RMsg_Party::InitAccept(lyra_id_t playerid)
{
  Init(ACCEPT, playerid, RC_UNUSED);
}

INLINE void RMsg_Party::InitReject(lyra_id_t playerid, int reason)
{
  Init(REJECT, playerid, reason);
}

INLINE void RMsg_Party::InitLeave(lyra_id_t playerid, int reason)
{
  Init(LEAVE, playerid, reason);
}

INLINE int RMsg_Party::RequestType() const
{
  return data_.req_type;
}

INLINE lyra_id_t RMsg_Party::PlayerID() const
{
  return data_.playerid;
}

INLINE int RMsg_Party::ResponseCode() const
{
  return data_.rc;
}

INLINE void RMsg_Party::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void RMsg_Party::SetRequestType(int rt)
{
  data_.req_type = rt;
}

INLINE void RMsg_Party::SetResponseCode(int rc)
{
  data_.rc = rc;
}
