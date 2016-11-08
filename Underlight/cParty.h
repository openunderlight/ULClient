// Header file for Party class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CPARTY_H
#define CPARTY_H

#include "Central.h"
#include "cActor.h"
#include "LyraDefs.h"
#include "RMsg_All.h"

//////////////////////////////////////////////////////////////////
// Constants

const int MAX_QUEUED_REQUESTS = 1; // max # of queued requests before auto reject
const int DEAD_REQUEST = -1; // to indicate an inactive record

// Note that we decrement MAX_PARTYSIZE for all our uses, since we
// don't store ourselves as a party member.

///////////////////////////////////////////////////////////////////
// Structures 

struct join_request_t
{
	lyra_id_t			playerID;
};

class cParty;

//////////////////////////////////////////////////////////////////
// Helpers

//////////////////////////////////////////////////////////////////
// Class Definition

class cParty
   {
   public:

   private:
	  lyra_id_t				leader; // ID of leader
	  int					party_members; // # of others in party
	  lyra_id_t				members[Lyra::MAX_PARTYSIZE-1]; 
	  lyra_id_t				channellers[Lyra::MAX_PARTYSIZE - 1];
	  int					num_requests; // # of outstanding requests from others to join our party
	  int					curr_request; // current live request from another to join our party
	  lyra_id_t				request_outstanding; // if we've queried another to join their party
	  int					accepts_outstanding; // if we're the leader, the # of outstanding people we've accepted but not received the join message
	  join_request_t		requests[MAX_QUEUED_REQUESTS]; 
	  bool					dissolving;
	  TCHAR					party_msg[DEFAULT_MESSAGE_SIZE];


   public:
      cParty(void);
	  ~cParty(void);

	  void OnPartyQuery(lyra_id_t playerID, bool auto_join); // someone wants to join
	  void RejectRequest(void); // reject
	  void AcceptRequest(void); // accept

	  void JoinedParty(RMsg_PartyInfo& partyinfo); // successful party join!
	  void Rejected(int reason, lyra_id_t player_id); // rejected!

	  void MemberEnter(const RmRemotePlayer& buddy); // new member in party! 
	  void MemberExit(lyra_id_t playerID, int status = RMsg_Party::LEAVE_NORMAL);  // party member bailed...
	  void JoinParty(lyra_id_t playerID, TCHAR *name, bool automatic = false); // we want to join this guy's party

	  void LeaveParty(void); // bail from party
	  void DissolveParty(bool userInitiated); // party has broken up

	  bool IsInParty(lyra_id_t player_id);
	  bool SetChanneller(realmid_t playerID, bool set = true);
	  
	  // Selectors
	  inline int Members(void) { return party_members; };
	  inline lyra_id_t MemberID(int i) { return members[i]; };
	  inline cNeighbor* CurrNeighbor(void);
	  TCHAR* CurrName(void);
	  BOOL  CurrValid(void);

	  // Misc
	  inline lyra_id_t RequestOutstanding(void) { return request_outstanding; };
	  inline int NumRequests(void) { return num_requests;};

	  void DisableRequest(int request_num);
	  void FindNewLeader(void); // for when the party leader bails
	  void DialogQuery(void); // pop up query dialog box

   private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cParty(const cParty& x);
	cParty& operator=(const cParty& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line);
#else
	inline void CheckInvariants(int line) {};
#endif
 };
#endif
