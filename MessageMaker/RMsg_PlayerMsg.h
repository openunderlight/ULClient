// RMsg_PlayerMsg.h  -*- C++ -*-
// $Id: RMsg_PlayerMsg.h,v 1.29 1998-05-11 10:56:01-07 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// player<->player message (for teaching, glamours, etc)

#ifndef INCLUDED_RMsg_PlayerMsg
#define INCLUDED_RMsg_PlayerMsg

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"

// forward references

// message class

class RMsg_PlayerMsg : public LmMesg {

public:

  enum {
    // message types, and state bytes used
    UNKNOWN = 0,
    // arts related messages
    RESIST_FEAR,         // skill, not used [01]
    RESIST_CURSE,        // skill, not used
    RESIST_PARALYSIS,    // skill, not used
    JUDGEMENT,           // skill, not used
    IDENTIFY_CURSE,      // skill, not used [05]
    VISION,              // skill, not used
    BLAST,               // skill, not used
    RESTORE,             // skill, not used     
    PURIFY,              // skill, not used
    DRAIN_SELF,          // stat, amount [10]
    ABJURE,              // skill, not used
    POISON,              // skill, not used
    ANTIDOTE,            // skill, not used     
    CURSE,               // skill, not used
    ENSLAVE,             // skill, not used [15]
    SCARE,               // skill, not used
    STAGGER,             // skill, not used
    DEAFEN,              // skill, not used
    BLIND,               // skill, not used
    DARKNESS,	         // skill, not used [20]
    PARALYZE,            // skill, not used
    TRAIN,               // art_id, teacher_skill/success
    RANDOM,              // random number, random number
    FIRESTORM,           // skill, not used
    RAZORWIND,           // skill, not used [25]
    TRAP_NIGHTMARE,      // skill, not used
    RETURN,		 // not used, not used
    DREAMSTRIKE,         // skill, not used
    DREAMSTRIKE_ACK,     // success, not used
    INITIATE,            // guild, success [30]
    INITIATE_ACK,        // guild, accept
    KNIGHT,              // guild, success
    ASCEND,              // guild, not used (CS) / success (SC)
    DEMOTE,              // guild, number tokens available
    DEMOTE_ACK,          // guild, number tokens used [35]
    DEMOTE_FAIL,         // guild, number tokens needed
    TRAIN_SPHERE,        // success, not used
    MIND_BLANK,          // hide on/off, not used
    MIND_BLANK_OTHER,    // skill, not used
    SOULEVOKE,           // unused, unused [40]
    GRANT_RP_XP,         // units of 1000, units of 100
    RECALL,              // not used, not used DEPRECATED
    EARTHQUAKE,          // skill, not used
    HYPNOTIC_WEAVE,      // skill, not used
    VAMPIRIC_DRAW,       // skill, stat [45]
    VAMPIRIC_DRAW_ACK,   // amount, stat
    TERROR,              // skill, not used
    HEALING_AURA,		 // skill, not used
    ROGER_WILCO,         // not used, not used
	TRAIN_SELF,			// art_id, not used (CS) / success (SC) [50]
	SPHERE_REPLY,		// success, sphere
	SOUL_SHIELD,		// skill, not used
	REFLECT_ART,		// art_id, not used
	BLAST_ACK,			// unused, unused
	SENSE_DREAMERS,		// unused, unused [55]
	EXPEL,				// skill, guild_id
	LOCATE_NEWLIES,		// unused, unused
	REDEEM_GRATITUDE,   // low 2 bytes of creator ID (CS)/success (SC)
	EMPATHY,			// thousands, hundreds (CS)/success (SC)
	RADIANT_BLAZE,		// skill, unused [60]
	POISON_CLOUD,		// skill, unused
	BREAK_COVENANT,		// skill, unused
	PEACE_AURA,			// skill, unused
	SABLE_SHIELD,		// skill, unused
	ENTRANCEMENT,		// skill, unused [65]
	SHADOW_STEP,		// skill, unused
	DAZZLE,				// skill, unused
	TEHTHUS_OBLIVION,   // unused, unused
	TEHTHUS_OBLIVION_ACK, // unused, unused 
	TRAIN_ACK,			// success, unused [70]		
	CHAOS_PURGE,		// skill, unused 
	CUP_SUMMONS,		// skill, unused
	HOUSE_MEMBERS,		// guild ID, unused
	SUMMON_PRIME,		// guild ID, success 
	GRANT_PPOINT,		// unused, unused

	// Nether Prophesy clone arts here
	SCAN,				// skill, not used     
    HEAL,               // skill, not used     
    SANCTIFY,           // skill, not used
	REMOVE_CURSE,		// skill, not used
	HOLD_AVATAR,		// skill, not used [80]

	// more Art here
	LOCATE_MARES,		// unused, unused
  TEMPEST,      // skill, angle
  KINESIS,        // skill, angle
  MISDIRECTION,   // skill, unused
  CHAOTIC_VORTEX, // skill, unused
  RALLY,				// unused, unused
  
    // GM-only messages
    FINGER_OF_DEATH = 90,// not used, not used
    GRANT_XP,            // units of 1000, units of 100
    TERMINATE,           // not used, not used
    GRANT_XP_NEGATIVE,   // units of 1000, units of 100 
    UNTRAIN,			 // art_id, not used
    BOOT,				// not used, not used
	SUMMON,				// not used, not used
	SUSPEND,			// # of days, not used

    // xp related messages
    YOUGOTME = 100,      // (victim's orbit || 100 + nightmare index), dreamsoul at dissolution
    PARTYKILL,           // (victim's orbit || 100 + nightmare index), # of party members (+ 100 if killer)

    // other triggers
	// sound triggers deprecated - put into real time packets instead
    //TRIGGER_SOUND = 200, // sound id, not used

    NEWBIE_ENTERED       // new player has entered recruiting area
  };

public:

  RMsg_PlayerMsg();
  ~RMsg_PlayerMsg();

  void Init(lyra_id_t send_id, lyra_id_t recv_id, int mtype, int state1 = 0, int state2 = 0);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t SenderID() const;
  lyra_id_t ReceiverID() const;
  int MsgType() const;
  int State1() const;
  int State2() const;

  static int ArtType(int msgtype);

  // mutators
  void SetSenderID(lyra_id_t playerid);
  void SetReceiverID(lyra_id_t playerid);
  void SetMsgType(int message_type);
  void SetState1(int byte1);
  void SetState2(int byte2);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t senderid;
    lyra_id_t receiverid;
    short mtype;
    unsigned char state1;
    unsigned char state2;
  } data_;

};

#ifdef USE_INLINE
#include "RMsg_PlayerMsg.i"
#endif

#endif /* INCLUDED_RMsg_PlayerMsg */
