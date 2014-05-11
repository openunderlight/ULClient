// AMsg_LoginAck.i  -*- C++ -*-
// $Id: AMsg_LoginAck.i,v 1.1 1997-06-17 15:34:01-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void AMsg_LoginAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int AMsg_LoginAck::Version() const
{
  return data_.version;
}

INLINE int AMsg_LoginAck::Build() const
{
  return -1;
//  return data_.build;
}

INLINE int AMsg_LoginAck::SubBuild() const
{
  return -1;
//  return data_.subbuild;
}

INLINE int AMsg_LoginAck::Status() const
{
  return data_.request_status;
}

INLINE int AMsg_LoginAck::NumAgents() const
{
  return data_.num_agents;
}

INLINE const agent_id_t& AMsg_LoginAck::ID(int index) const
{
  return data_.ids[index];
}

INLINE void AMsg_LoginAck::SetVersion(int version)
{
  data_.version = version;
}

INLINE void AMsg_LoginAck::SetStatus(int status)
{
  data_.request_status = status;
}

INLINE void AMsg_LoginAck::SetNumAgents(int num_agents)
{
  data_.num_agents = MIN(num_agents, AMsg::MAX_AGENTS);
  calc_size();
}

INLINE void AMsg_LoginAck::SetID(int index, const agent_id_t& id)
{
  data_.ids[index] = id;
}
