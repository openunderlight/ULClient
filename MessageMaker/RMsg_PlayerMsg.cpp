// RMsg_PlayerMsg.cpp  -*- C++ -*-
// $Id: RMsg_PlayerMsg.cpp,v 1.6 1998-02-04 19:03:58-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "RMsg_PlayerMsg.h"
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

#include "RMsg_PlayerMsg.h"
#include "LyraDefs.h"
#include "SharedConstants.h"
#include "RMsg.h"

#ifndef USE_INLINE
#include "RMsg_PlayerMsg.i"
#endif

////
// constructor
////

RMsg_PlayerMsg::RMsg_PlayerMsg()
  : LmMesg(RMsg::PLAYERMSG, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, Lyra::ID_UNKNOWN, RMsg_PlayerMsg::UNKNOWN, 0, 0);
}

////
// destructor
////

RMsg_PlayerMsg::~RMsg_PlayerMsg()
{
  // empty
}

////
// Init
////

void RMsg_PlayerMsg::Init(lyra_id_t send_id, lyra_id_t recv_id, int mtype, int state1, int state2, int state3)

{
  SetSenderID(send_id);
  SetReceiverID(recv_id);
  SetMsgType(mtype);
  SetState1(state1);
  SetState2(state2);
  SetState3(state3);
  SetUniversal(false);
}

void RMsg_PlayerMsg::Init(lyra_id_t send_id, lyra_id_t recv_id, int mtype, int state1, int state2, int state3, bool universal)
{
	Init(send_id, recv_id, mtype, state1, state2, state3);
	SetUniversal(universal);
}

////
// hton
////

void RMsg_PlayerMsg::hton()
{
  HTONL(data_.senderid);
  HTONL(data_.receiverid);
  HTONS(data_.state1);
  HTONS(data_.state2);
  HTONS(data_.state3);
  // no conversion: state1, state2
}

////
// ntoh
////

void RMsg_PlayerMsg::ntoh()
{
  NTOHL(data_.senderid);
  NTOHL(data_.receiverid);
  NTOHS(data_.state1);
  NTOHS(data_.state2);
  NTOHS(data_.state3);
  // no conversion: state1, state2, state3
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_PlayerMsg::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<RMsg_PlayerMsg[%p,%d]: "), this, sizeof(RMsg_PlayerMsg));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("sender=%u receiver=%u mtype=%d universal=%d state=(%d,%d,%d)>\n"),
	    SenderID(), ReceiverID(), MsgType(), Universal(), State1(), State2(), State3());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// ArtType - map between given message type, and player art constant
////

int RMsg_PlayerMsg::ArtType(int msgtype)
{
  int retval = Arts::NONE;
  switch (msgtype) {
  case RESIST_FEAR:         retval = Arts::RESIST_FEAR; break;
  case RESIST_CURSE:        retval = Arts::PROTECTION; break;
  case RESIST_PARALYSIS:    retval = Arts::FREE_ACTION; break;
  case JUDGEMENT:           retval = Arts::JUDGEMENT; break;
  case IDENTIFY_CURSE:      retval = Arts::IDENTIFY_CURSE; break;
  case VISION:              retval = Arts::VISION; break;
  case BLAST:               retval = Arts::BLAST; break;
  case RESTORE:             retval = Arts::RESTORE; break;
  case PURIFY:              retval = Arts::PURIFY; break;
  case DRAIN_SELF:          retval = Arts::DRAIN_SELF; break;
  case ABJURE:              retval = Arts::ABJURE; break;
  case POISON:              retval = Arts::POISON; break;
  case ANTIDOTE:            retval = Arts::ANTIDOTE; break;
  case CURSE:               retval = Arts::CURSE; break;
  case ENSLAVE:             retval = Arts::ENSLAVE_NIGHTMARE; break;
  case SCARE:               retval = Arts::SCARE; break;
  case STAGGER:             retval = Arts::STAGGER; break;
  case DEAFEN:              retval = Arts::DEAFEN; break;
  case BLIND:               retval = Arts::BLIND; break;
  case DARKNESS:	    retval = Arts::DARKNESS; break;
  case PARALYZE:            retval = Arts::PARALYZE; break;
  case TRAIN:               retval = Arts::TRAIN; break;
  case INITIATE:            retval = Arts::INITIATE; break;
  case KNIGHT:              retval = Arts::KNIGHT; break;
  case RANDOM:              retval = Arts::RANDOM; break;
  case FIRESTORM:           retval = Arts::FIRESTORM; break;
  case RAZORWIND:           retval = Arts::RAZORWIND; break;
  case DREAMSTRIKE:         retval = Arts::DREAMSTRIKE; break;
  case TRAP_NIGHTMARE:      retval = Arts::TRAP_NIGHTMARE; break;
  case RETURN:	            retval = Arts::RETURN; break;
  case DEMOTE:              retval = Arts::DEMOTE; break;
  case MIND_BLANK_OTHER:    retval = Arts::MIND_BLANK; break;
  case EARTHQUAKE:          retval = Arts::EARTHQUAKE; break;
  case HYPNOTIC_WEAVE:      retval = Arts::HYPNOTIC_WEAVE; break;
  case VAMPIRIC_DRAW:		retval = Arts::VAMPIRIC_DRAW; break;
  case TERROR:              retval = Arts::TERROR; break;
  case HEALING_AURA:        retval = Arts::HEALING_AURA; break;
  case TRAIN_SELF:		    retval = Arts::TRAIN_SELF; break; 
  case SOUL_SHIELD:		    retval = Arts::SOUL_SHIELD; break;
  case REFLECT_ART:         retval = Arts::REFLECT; break;
  case SUMMON:				retval = Arts::SUMMON; break;
  case SUSPEND:				retval = Arts::SUSPEND; break;
  case SENSE_DREAMERS:		retval = Arts::SENSE_DREAMERS; break;
  case EXPEL:				retval = Arts::EXPEL; break;
  case CHAOS_PURGE:			retval = Arts::CHAOS_PURGE; break;
  case CUP_SUMMONS:		    retval = Arts::CUP_SUMMONS; break;
  case ROGER_WILCO:         retval = Arts::ROGER_WILCO; break;
  case SUMMON_PRIME:		retval = Arts::SUMMON_PRIME; break;
  case SCAN:				retval = Arts::SCAN; break; 
  case HEAL:	            retval = Arts::HEAL; break; 
  case SANCTIFY:            retval = Arts::SANCTIFY; break; 
  case REMOVE_CURSE:		retval = Arts::REMOVE_CURSE; break;
  case HOLD_AVATAR:			retval = Arts::HOLD_AVATAR; break;
  case TEMPEST:         retval = Arts::TEMPEST; break;
  case KINESIS:         retval = Arts::KINESIS; break;
  case MISDIRECTION:    retval = Arts::MISDIRECTION; break;
  case CHAOTIC_VORTEX:  retval = Arts::CHAOTIC_VORTEX; break;
  case RALLY:				retval = Arts::RALLY; break;
  case ENFEEBLEMENT: retval = Arts::ENFEEBLEMENT; break; 
  default:
    retval = Arts::NONE;
  }
  return retval;
}

bool RMsg_PlayerMsg::AllowedToDreamwideBroadcast(int msgtype)
{	
	switch (msgtype) {
	case RESIST_FEAR:         
	case RESIST_CURSE:        
	case RESIST_PARALYSIS:    
	case JUDGEMENT:           
	case IDENTIFY_CURSE:      
	case VISION:              
	case BLAST:               
	case RESTORE:             
	case PURIFY:              
	case DRAIN_SELF:          
	case ABJURE:              
	case POISON:              
	case ANTIDOTE:            
	case CURSE:               
	case ENSLAVE:             
	case SCARE:               
	case STAGGER:             
	case DEAFEN:              
	case BLIND:               
	case DARKNESS:	    
	case PARALYZE:      
	case RANDOM:              
	case FIRESTORM:           
	case RAZORWIND:           
	case MIND_BLANK_OTHER:    
	case EARTHQUAKE:          
	case HYPNOTIC_WEAVE:      
	case VAMPIRIC_DRAW:		
	case TERROR:            
	case SOUL_SHIELD:
	case REFLECT_ART:         
	case SUMMON:			
	case SUSPEND:			
	case SENSE_DREAMERS:	
	case EXPEL:				
	case CHAOS_PURGE:		
	case CUP_SUMMONS:		
	case SCAN:				
	case HEAL:	            
	case SANCTIFY:          
	case REMOVE_CURSE:		
	case HOLD_AVATAR:		
	case TEMPEST:         
	case KINESIS:         
	case MISDIRECTION:    
	case CHAOTIC_VORTEX:  
	case RALLY:				
	case ENFEEBLEMENT: 
		return true;
	default:
		return false;
	}	
}


