// The Party Class

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "cDDraw.h"
#include "4dx.h" // for actor box lookup
#include "resource.h"
#include "Dialogs.h"
#include "cGameServer.h"
#include "cChat.h"
#include "cNeighbor.h"
#include "cLevel.h"
#include "cPlayer.h"
#include "Interface.h"
#include "Options.h"
#include "cActorList.h"
#include "cParty.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern cDDraw *cDD; // direct draw object
extern cGameServer *gs; // room server
extern cChat *display;
extern cPlayer *player;
extern cActorList *actors;
extern cLevel *level;
extern HINSTANCE hInstance;
extern options_t options;
extern bool acceptrejectdlg;
extern cArts* arts;

//////////////////////////////////////////////////////////////////
// Constants

void PartyDlgCallback(void *value)
{
	int success = *((int*)value);

	if (success)
		gs->Party()->AcceptRequest();
	else
		gs->Party()->RejectRequest();
}


/////////////////////////////////////////////////////////////////
// Class Definition

// Party members is only for other players in the party; we're
// not counted, and we're not in members[].

// Constructor
cParty::cParty(void)
{
	int i;

	leader = Lyra::ID_UNKNOWN;
	party_members = 0;
	num_requests = 0;
	curr_request = DEAD_REQUEST;
	request_outstanding = Lyra::ID_UNKNOWN;
	accepts_outstanding = 0;
	dissolving = false;

	for (i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++)
	{
		members[i] = Lyra::ID_UNKNOWN;
		channellers[i] = Lyra::ID_UNKNOWN;
	}

	for (i = 0; i<MAX_QUEUED_REQUESTS; i++)
		requests[i].playerID = Lyra::ID_UNKNOWN;

	this->CheckInvariants(__LINE__);
}

// someone wants to join our party. 
// We maintain a queue of up to MAX_QUEUED_REQUESTS; once that fills up
// we auto-reject requests. We also reject if we're not the leader,
// if we have our own outstanding join party request, or if the party
// is full. Note that the party might be full because we have accepted
// members into the party that haven't officially joined yet.
void cParty::OnPartyQuery(realmid_t playerID, bool auto_join)
{
	int index;
#ifdef AGENT
	gs->RejectPartyQuery(RMsg_Party::REJECT_NO, playerID);
#endif
	//_tprintf("Got a party query for player ID %d\n",playerID);

	if (playerID == Lyra::ID_UNKNOWN)
	{ // not a valid request
		gs->RejectPartyQuery(RMsg_Party::REJECT_QUEUEFULL, playerID); // too many requests
	}
	else if (num_requests == MAX_QUEUED_REQUESTS) // maxed out the queue! reject!
	{
		gs->RejectPartyQuery(RMsg_Party::REJECT_QUEUEFULL, playerID); // too many requests
		//_tprintf("rejecting query b/c our queue is full!\n");
	}
	else if (party_members && (leader != player->ID()))
	{
		gs->RejectPartyQuery(RMsg_Party::REJECT_NOTLEADER, playerID); // not the leader
		//_tprintf("rejecting query b/c we are not the leader!\n");
	}
	else if (request_outstanding != Lyra::ID_UNKNOWN)
	{
		gs->RejectPartyQuery(RMsg_Party::REJECT_BUSY, playerID); // we're joining another's party
		//_tprintf("rejecting query b/c we made our own request!\n");
	}
	else if (party_members + accepts_outstanding >= Lyra::MAX_PARTYSIZE - 1)
	{
		gs->RejectPartyQuery(RMsg_Party::REJECT_PARTYFULL, playerID); // party full
		//_tprintf("rejecting query b/c our party is full!!\n");
	}
	else if (actors->LookUpNeighbor(playerID) == NO_ACTOR)
	{
		gs->RejectPartyQuery(RMsg_Party::REJECT_NO, playerID);
	}
	else if (options.autoreject)
	{
		gs->RejectPartyQuery(RMsg_Party::REJECT_NO, playerID);
	}
	else if (player->flags & ACTOR_NO_PARTY)
	{
		gs->RejectPartyQuery(RMsg_Party::REJECT_NO, playerID);
		LoadString(hInstance, IDS_NO_PARTY, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
	}
	else
	{ // requests will automatically be displayed each frame while available
		//_tprintf("request enqueued - curr: %d num: %d\n",curr_request,num_requests); 
		if (curr_request == DEAD_REQUEST)
			curr_request = index = 0; //
		else
			index = (curr_request + num_requests) % MAX_QUEUED_REQUESTS;
		//_tprintf("Adding new request in slot %d\n",index);
		requests[index].playerID = playerID;
		num_requests++;

		if (auto_join)
			gs->Party()->AcceptRequest();
	}

	this->CheckInvariants(__LINE__);
	return;
}

// return the neighbor making the current join party request
cNeighbor* cParty::CurrNeighbor(void)
{
	return actors->LookUpNeighbor(requests[curr_request].playerID);
};


// return the name of the player making the current join party request
TCHAR* cParty::CurrName(void)
{
	cNeighbor *n;

	n = actors->LookUpNeighbor(requests[curr_request].playerID);
	if (n != NO_ACTOR)
		return n->Name();
	else
	{
		LoadString(hInstance, IDS_UNKNOWN, party_msg, sizeof(party_msg));
		return party_msg;
	}
};

// returns true if the current request is a valid neighbor, false
// otherwise
BOOL cParty::CurrValid(void)
{
	cNeighbor *n;

	n = actors->LookUpNeighbor(requests[curr_request].playerID);
	if (n != NO_ACTOR)
		return TRUE;
	else
		return FALSE;
};


// throw up the dialog box to ask if the player can join the party
void cParty::DialogQuery(void)
{
	//_tprintf("doing dialog query for request %d!\n",request_num);
#ifndef AGENT
	if (!acceptrejectdlg)
	{
		HWND prevDlg = GetFocus();
		LoadString(hInstance, IDS_JP1, party_msg, sizeof(party_msg));
		_stprintf(message, party_msg, this->CurrName());
		HWND hDlg = CreateLyraDialog(hInstance, IDD_ACCEPTREJECT,
			cDD->Hwnd_Main(), (DLGPROC)AcceptRejectDlgProc);
		SendMessage(hDlg, WM_SET_CALLBACK, 0, (LPARAM)PartyDlgCallback);
		SendMessage(hDlg, WM_SET_AR_NEIGHBOR, 0, (LPARAM)this->CurrNeighbor());

		if (prevDlg) {
			SetActiveWindow(prevDlg);
			SetFocus(prevDlg);
		}
		else {
			SetActiveWindow(cDD->Hwnd_Main());
			SetFocus(cDD->Hwnd_Main());
		}
	}
	else
		this->RejectRequest();
#endif
}

// Reject the current join party query. This function is called
// from the query dialog box procedure. 

void cParty::RejectRequest(void)
{
	LoadString(hInstance, IDS_JP2, party_msg, sizeof(party_msg));
	_stprintf(errbuf, party_msg, curr_request);
	INFO(errbuf);

	gs->RejectPartyQuery(RMsg_Party::REJECT_NO, requests[curr_request].playerID);
	DisableRequest(curr_request);

	this->CheckInvariants(__LINE__);
	return;

}

// Accept the current join party request. This function is called 
// from the query dialog box procedure.

void cParty::AcceptRequest(void)
{
	gs->AcceptPartyQuery(requests[curr_request].playerID);

	accepts_outstanding++;

	player->SetLastLeaderID(Lyra::ID_UNKNOWN);

	DisableRequest(curr_request);

	this->CheckInvariants(__LINE__);
	return;
}

// A new party member has been successfully added.

void cParty::MemberEnter(const RmRemotePlayer& buddy)
{
	int i;
	cNeighbor *n;
	TCHAR name[Lyra::PLAYERNAME_MAX];

	LoadString(hInstance, IDS_UNKNOWN, name, sizeof(name));

	//_tprintf("Adding member %d to party!\n",buddy->playerid);

	if (buddy.PlayerID() == Lyra::ID_UNKNOWN)
	{
		NONFATAL_ERROR(IDS_JP3);
		return;
	}

	if ((party_members == 0) && (leader == Lyra::ID_UNKNOWN))
		leader = player->ID(); // we must be the leader
	// if we're the leader then we must have accepted this person
	if (leader == player->ID())
		accepts_outstanding--;

	for (i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++)
		if (members[i] == Lyra::ID_UNKNOWN)
		{
			channellers[i] = Lyra::ID_UNKNOWN;
			members[i] = buddy.PlayerID();
			party_members++;
			n = actors->LookUpNeighbor(buddy.PlayerID());
			if (n == NO_ACTOR)
			{ // index == -1 if a new party member is outside the room
				//_tprintf("Adding neighbor outside room! id = %d\n",buddy->playerid);
				n = new cNeighbor(buddy);
				// set position correctly; this may not have been done yet
				// if it's a party member & not a newly created neighbor
				RMsg_PlayerUpdate update;
				update.Init(n->ID(), 1);
				update.SetPeerUpdate(0, buddy.PeerUpdate());
				gs->HandlePositionUpdate(update);
				n->SetXHeight();
			}
			LoadString(hInstance, IDS_PARTY_PLAYERJOINED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, n->Name());
			display->DisplayMessage(message);
			n->Lock(); //rs->LockNeighbor(index);
			break;
		}
	
	if(!player->IsChannelling() && player->LastChannelTarget() == buddy.PlayerID())
		arts->SetChannel(buddy.PlayerID());

	this->CheckInvariants(__LINE__);
	return;
}

// An existing party member has bailed! If it is the leader, the
// party is dissolved

void cParty::MemberExit(realmid_t playerID, int status)
{
	int i;
	cNeighbor *n;
	TCHAR name[Lyra::PLAYERNAME_MAX];

	//_stprintf(message, "Member %d leaving party, status %d\n", playerID, status);
	//LOGIT(message);

	if (playerID == Lyra::ID_UNKNOWN)
		return;

	n = actors->LookUpNeighbor(playerID);

	if (player->IsChannelling() && playerID == player->ChannelTarget())
		arts->ExpireChannel(false);

	if (n == NO_ACTOR)
		LoadString(hInstance, IDS_JP4, name, sizeof(name));
	else
	{
		_tcscpy(name, n->Name());
		if (status == RMsg_Party::LEAVE_NORMAL)
		{
			LoadString(hInstance, IDS_PARTY_PLAYERLEFT, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, name);
		}
		else
		{
			LoadString(hInstance, IDS_PARTY_LEAVE_LOGOUT, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, name, level->Name(level->ID()));
		}

		display->DisplayMessage(message);
	}
	//_tprintf("On enter - Members: %d locked neighbors: %d\n",party_members,gs->LockedNeighbors());

	for (i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++)
		if (members[i] == playerID)
		{
			members[i] = Lyra::ID_UNKNOWN;
			if (channellers[i] != Lyra::ID_UNKNOWN)
			{
				LoadString(hInstance, IDS_CHANNEL_CLOSED, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, name);
				display->DisplayMessage(message);
			}
			channellers[i] = Lyra::ID_UNKNOWN;
			party_members--;
			if (playerID == leader)
			{
				if (status == RMsg_Party::LEAVE_NORMAL && !dissolving)
					player->SetLastLeaderID(Lyra::ID_UNKNOWN);
				leader = Lyra::ID_UNKNOWN;
				this->DissolveParty(false);
			}
			else if (!party_members && (leader == player->ID()))
				leader = Lyra::ID_UNKNOWN;
			//	_tprintf("Player %d left; Leader now: %d Members now: %d\n",playerID,leader,party_members);
			//	_tprintf("After find new leader - Members: %d locked neighbors: %d\n",party_members,gs->LockedNeighbors());
			if (n != NO_ACTOR)
				n->Unlock();
			break;
			//	_tprintf("After unlock - Members: %d locked neighbors: %d\n",party_members,gs->LockedNeighbors());

		}

	//if (i == (Lyra::MAX_PARTYSIZE-1))
	//_tprintf("ERROR: can't find party member %d to delete!!!!\n",playerID);

	this->CheckInvariants(__LINE__);
	return;
}

bool cParty::SetChanneller(realmid_t playerID, bool set)
{
	cNeighbor *n;

	//_stprintf(message, "Member %d leaving party, status %d\n", playerID, status);
	//LOGIT(message);

	if (playerID == Lyra::ID_UNKNOWN)
		return false;

	for (int i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++)
		if (members[i] == playerID)
		{
			channellers[i] = set ? playerID : Lyra::ID_UNKNOWN;
			return true;
		}

	return false;
}

// Send out a request to join this player's party, if we are not
// already in a party. If we're in a party, give an error.
// Automatic is true if this is an automated join request; for
// this case, error messages are not displayed

void cParty::JoinParty(realmid_t playerID, TCHAR *name, bool automatic)
{
	if (playerID == Lyra::ID_UNKNOWN)
	{
		NONFATAL_ERROR(IDS_JP5);
		return;
	}

	if (request_outstanding != Lyra::ID_UNKNOWN) // outstanding join party
	{
		if (!automatic)
		{
			LoadString(hInstance, IDS_PARTY_ALREADYJOINING, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
		}
		return;
	}
	else if (party_members > 0)
	{
		if (!automatic)
		{
			LoadString(hInstance, IDS_PARTY_MUSTLEAVE, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
		}
		return;
	}

	gs->JoinParty(playerID, automatic);

	LoadString(hInstance, IDS_PARTY_REQUEST, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, name);
	display->DisplayMessage(message, false);
	request_outstanding = playerID;

	this->CheckInvariants(__LINE__);
	return;
}


// We have succesfully joined a party!
void cParty::JoinedParty(RMsg_PartyInfo& partyinfo)
{
	int i;
	cNeighbor *n;
	TCHAR name[Lyra::PLAYERNAME_MAX];
	LoadString(hInstance, IDS_UNKNOWN, name, sizeof(name));

	request_outstanding = Lyra::ID_UNKNOWN;

	//_tprintf("party leader: %d\n",leader);
	for (i = 0; i<partyinfo.PartySize(); i++)
		if ((partyinfo.PartyMember(i).PlayerID() != Lyra::ID_UNKNOWN) &&
			(partyinfo.PartyMember(i).PlayerID() != player->ID()))
			this->MemberEnter(partyinfo.PartyMember(i));

	leader = partyinfo.LeaderID();
	player->SetLastLeaderID(leader);

	n = actors->LookUpNeighbor(leader);
	if (n != NO_ACTOR)
		_tcscpy(name, n->Name());

	LoadString(hInstance, IDS_PARTY_JOINED, disp_message, sizeof(disp_message));

	_stprintf(message, disp_message, name, party_members + 1);
	display->DisplayMessage(message);

	//_tprintf("We've joined a party; leader = %d, members = %d\n",leader,party_members);

	this->CheckInvariants(__LINE__);
	return;
}

// We have been rejected from joining the party!
void cParty::Rejected(int reason, realmid_t player_id)
{
	cNeighbor *n;
	TCHAR name[Lyra::PLAYERNAME_MAX];
	LoadString(hInstance, IDS_ELSEWHERE, name, sizeof(name));

	request_outstanding = Lyra::ID_UNKNOWN;
	player->SetLastLeaderID(Lyra::ID_UNKNOWN);

	n = actors->LookUpNeighbor(player_id);
	if (n != NO_ACTOR)
		_tcscpy(name, n->Name());

	switch (reason)
	{
	case RMsg_Party::REJECT_LEFT:
		LoadString(hInstance, IDS_JOINREJ_LEFT, message, sizeof(message));
		display->DisplayMessage(message);
		return;

	case RMsg_Party::REJECT_NOTLEADER:
		if (n == NO_ACTOR)
			LoadString(hInstance, IDS_JOINREJ_LEADER_UNKNOWN, message, sizeof(message));
		else
		{
			LoadString(hInstance, IDS_PARTY_REJ_NOTLEADER, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, name);
		}
		break;
	case RMsg_Party::REJECT_PARTYFULL:
		LoadString(hInstance, IDS_PARTY_REJ_FULL, message, sizeof(message));
		break;
	case RMsg_Party::REJECT_MUSTLEAVEPARTY:
		LoadString(hInstance, IDS_PARTY_REJ_LEAVE, message, sizeof(message));
		break;
	case RMsg_Party::REJECT_NO:
		LoadString(hInstance, IDS_PARTY_REJ_NO, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, name);
		break;
	case RMsg_Party::REJECT_BUSY:
		LoadString(hInstance, IDS_PARTY_REJ_BUSY, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, name);
		break;
	case RMsg_Party::REJECT_QUEUEFULL:
		LoadString(hInstance, IDS_PARTY_REJ_TOOMANY, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, name);
		break;
	default:
		LoadString(hInstance, IDS_PARTY_REJ_UNKNOWN, message, sizeof(message));
	}

	display->DisplayMessage(message);

	this->CheckInvariants(__LINE__);
	return;
}


// Leave the current party
void cParty::LeaveParty(void)
{
	if (party_members == 0)
	{
		LoadString(hInstance, IDS_PARTY_NOTMEMBER, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
	}
	else
	{
		player->SetLastLeaderID(Lyra::ID_UNKNOWN);
		this->DissolveParty(true);
		gs->LeaveParty(player->ID());
	}
	this->CheckInvariants(__LINE__);
	return;
}

// Party has been disbanded. 
void cParty::DissolveParty(bool userInitiated)
{
	int i;

	dissolving = true;

	if (player->IsChannelling())
		arts->ExpireChannel(userInitiated);

	for (i = 0; i<(Lyra::MAX_PARTYSIZE - 1); i++)
		if (members[i] != Lyra::ID_UNKNOWN)
			this->MemberExit(members[i]);

	dissolving = false;


	return;
}

// A request has been satisfied, so remove from the queue. 

void cParty::DisableRequest(int request_num)
{
	LoadString(hInstance, IDS_DISABLE, party_msg, sizeof(party_msg));
	_stprintf(errbuf, party_msg, request_num, num_requests);
	INFO(errbuf);

	requests[request_num].playerID = Lyra::ID_UNKNOWN;
	if (num_requests == 1) // last request
	{
		num_requests = 0;
		curr_request = DEAD_REQUEST;
	}
	else // else make the next the current
	{
		num_requests--;
		curr_request++;
		if (curr_request == MAX_QUEUED_REQUESTS)
			curr_request = 0;
		if (party_members + accepts_outstanding >= Lyra::MAX_PARTYSIZE - 1)
			this->RejectRequest();

	}

	this->CheckInvariants(__LINE__);
	return;
}

// Currently, the lowest numbered player ID is made the leader. 
void cParty::FindNewLeader(void)
{
	int i;
	cNeighbor *n;

	if (party_members == 0)
		leader = Lyra::ID_UNKNOWN;
	else
	{

		leader = player->ID();
		for (i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++)
			if ((members[i] != Lyra::ID_UNKNOWN) && (members[i] < leader))
				leader = members[i];
		if (leader == player->ID())
			LoadString(hInstance, IDS_PARTY_YOULEAD, message, 128);
		else
		{
			n = actors->LookUpNeighbor(leader);
			if (n == NO_ACTOR)
				LoadString(hInstance, IDS_JP6, message, sizeof(message));
			else
			{
				LoadString(hInstance, IDS_PARTY_OTHERLEAD, disp_message, 116);
				_stprintf(message, disp_message, n->Name());
			}
		}
		display->DisplayMessage(message);
	}

	this->CheckInvariants(__LINE__);
	return;
}


// returns true if this neighbor is in our party
bool cParty::IsInParty(lyra_id_t player_id)
{
	for (int i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++) // the members struct is NOT 
		if (this->MemberID(i) == player_id)
			return true;

	return false;
}

// Destructor
cParty::~cParty(void)
{
}

#ifdef CHECK_INVARIANTS
void cParty::CheckInvariants(int line)
{
	int i, j;

	// all active requests should have real player ID's, and all
	// inactive requests should have Lyra::ID_UNKNOWN
	if (curr_request == DEAD_REQUEST) // no active requests
	{
		for (i = 0; i<MAX_QUEUED_REQUESTS; i++)
			if (requests[i].playerID != Lyra::ID_UNKNOWN)
			{
				LoadString(hInstance, IDS_JP7, party_msg, sizeof(party_msg));
				_stprintf(errbuf, party_msg, i, requests[i].playerID, line);
				NONFATAL_ERROR(errbuf);
			}
	}
	else // some active requests
	{
		i = curr_request;
		while (i != ((curr_request + num_requests) % MAX_QUEUED_REQUESTS))
		{	// check active requests
			if (requests[i].playerID == Lyra::ID_UNKNOWN)
			{
				LoadString(hInstance, IDS_JP8, party_msg, sizeof(party_msg));
				_stprintf(errbuf, party_msg, i, line);
				NONFATAL_ERROR(errbuf);
			}
			i++;
			if (i == MAX_QUEUED_REQUESTS)
				i = 0;
		}
		while (i != curr_request)
		{   // check inactive requests
			if (requests[i].playerID != Lyra::ID_UNKNOWN)
			{
				LoadString(hInstance, IDS_JP9, party_msg, sizeof(party_msg));
				_stprintf(errbuf, party_msg, i, requests[i].playerID, line);
				NONFATAL_ERROR(errbuf);
			}
			i++;
			if (i == MAX_QUEUED_REQUESTS)
				i = 0;
		}
	}

	// Can't be the leader if no one is in the party
	if (!dissolving && (leader != Lyra::ID_UNKNOWN) && !party_members)
	{
		LoadString(hInstance, IDS_JP10, party_msg, sizeof(party_msg));
		_stprintf(errbuf, party_msg, line);
		NONFATAL_ERROR(errbuf);
	}


	// Make sure we have a leader in the array if there are party members
	if (party_members>0)
	{
		if ((leader == Lyra::ID_UNKNOWN) && !dissolving)
		{
			GAME_ERROR2(IDS_JP11, party_members, line);
		}
		for (i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++)
			if (leader == members[i])
				break;
		if ((i == (Lyra::MAX_PARTYSIZE - 1)) && (leader != player->ID()) && !dissolving)
		{
			GAME_ERROR3(IDS_JP12, party_members, leader, line);
		}
	}

	// Proper # of people in the party? party_members should be equal
	// to the # of non-unknown entries in the members array
	j = 0;
	for (i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++)
		if (members[i] != Lyra::ID_UNKNOWN)
			j++;
	if (party_members != j)
	{
		GAME_ERROR3(IDS_JP13, party_members, j, line);
	}

	// Make sure we're not in the members structure
	for (i = 0; i<Lyra::MAX_PARTYSIZE - 1; i++)
		if ((player->ID() != Lyra::ID_UNKNOWN) && (members[i] == player->ID()))
		{
			GAME_ERROR2(IDS_JP14, i, line);
		}

	// Make sure there is a sane # of accepts outstanding; < MAX - party_members
	if (accepts_outstanding >(Lyra::MAX_PARTYSIZE - party_members - 1))
	{
		GAME_ERROR3(IDS_JP15, accepts_outstanding, party_members, line);
	}

	return;

}

#endif
