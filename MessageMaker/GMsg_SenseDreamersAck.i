// GMsg_SenseDreamers.i  -*- C++ -*-
// $Id: GMsg_SenseDreamers.i,v 1.3 1997-11-25 15:53:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void GMsg_SenseDreamersAck::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t GMsg_SenseDreamersAck::LevelID(int index) const
{
  return (lyra_id_t)(data_.level_id[index]);
}

INLINE unsigned int GMsg_SenseDreamersAck::Total() const
{
	return data_.total;
}

INLINE void GMsg_SenseDreamersAck::SetTotal(unsigned int total)
{
	data_.total = total;
}

//INLINE int GMsg_SenseDreamersAck::NumDreamers(int index) const
//{
//  return (int)(data_.num_dreamers[index]);
//}
