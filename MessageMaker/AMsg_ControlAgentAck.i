// AMsg_ControlAgentAck.i  -*- C++ -*-
// $Id: AMsg_ControlAgentAck.i,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void AMsg_ControlAgentAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t AMsg_ControlAgentAck::AgentID() const
{
  return data_.agent_id;
}

INLINE int AMsg_ControlAgentAck::Status() const
{
  return data_.status;
}

INLINE void AMsg_ControlAgentAck::SetAgentID(lyra_id_t agent_id)
{
  data_.agent_id = agent_id;
}

INLINE void AMsg_ControlAgentAck::SetStatus(int status)
{
  data_.status = status;
}
