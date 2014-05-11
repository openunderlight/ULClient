// GMsg_SummonAvatarAck.h	-*- C++ -*-
// $Id: GMsg_SummonAvatarAck.h,v 1.3 1998-02-26 14:35:44-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Summon player

#ifndef INCLUDED_GMsg_SummonAvatarAck
#define INCLUDED_GMsg_SummonAvatarAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "GMsg.h"

// forward references

// message class

class GMsg_SummonAvatarAck : public LmMesg {

public:

	enum
	{
	// constants
		MAX_PLAYERS = 24,  // *note* must be same in GMsg_SummonAvatar.h!

		// Summon status values
		Summon_AGENT			  = 'A',  // player is an agent, cannot Summon!
		Summon_FOUND			  = 'F',  // player is in game, location is given
		Summon_HIDDEN 		  = 'H',  // player is in game, but hidden
		Summon_NOTLOGGEDIN	  = 'N',  // player not logged in game
		Summon_PLAYERNOTFOUND = 'P',  // player not in database
		Summon_UNKNOWN		  = 'U'	 // unknown status
	};

public:

	GMsg_SummonAvatarAck();
	~GMsg_SummonAvatarAck();

	void Init(int num_players);

	// standard public methods
	void Dump(FILE* f, int indent = 0) const;

	// selectors
	int NumPlayers() const;
	const char* PlayerName(int player_num) const;
	int Status(int player_num) const;
	lyra_id_t LevelID(int player_num) const;
	lyra_id_t RoomID(int player_num) const;

	// mutators
	void SetNumPlayers(int num_players);
	void SetPlayerName(int player_num, const char* playername);
	void SetStatus(int player_num, int status);
	void SetLocation(int player_num, lyra_id_t caster_id, int x, int y, int height, int angle, lyra_id_t levelid, lyra_id_t roomid);

public: // logically private

	// player info structure
	struct pinfo_t
	{
		char playername[Lyra::PLAYERNAME_MAX];
		int status;
      short x;
      short y;
      short height;
      unsigned short angle;
		lyra_id_t levelid;
		lyra_id_t roomid;
	};

private:

	// standard non-public methods
	void hton();
	void ntoh();

	void calc_size();
	void calc_players();

	// non-message data
	int num_players_;

	// message data structure
	struct data_t
	{
		pinfo_t players[MAX_PLAYERS];
	} data_;
};

#ifdef USE_INLINE
#include "GMsg_SummonAvatarAck.i"
#endif

#endif /* INCLUDED_GMsg_SummonAvatarAck */
