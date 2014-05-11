// AMsg_ControlAgent.i  -*- C++ -*-
// $Id: AMsg_ControlAgent.i,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void AMsg_ControlAgent::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE void AMsg_ControlAgent::InitStart(lyra_id_t agent_id)
{
  Init(agent_id, COMMAND_START);
}

INLINE void AMsg_ControlAgent::InitStop(lyra_id_t agent_id)
{
  Init(agent_id, COMMAND_STOP);
}

INLINE lyra_id_t AMsg_ControlAgent::AgentID() const
{
  return data_.agent_id;
}

INLINE int AMsg_ControlAgent::Command() const
{
  return data_.command;
}

INLINE void AMsg_ControlAgent::SetAgentID(lyra_id_t agent_id)
{
  data_.agent_id = agent_id;
}

INLINE void AMsg_ControlAgent::SetCommand(int command)
{
  data_.command = command;
}
