// The Actor List Class: a container for actors, implemented as
// a doubly-linked list

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "resource.h"
#include "cActorList.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;

///////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
cActorList::cActorList(void)
{
	for (int i=0; i<MAX_ITER_DEPTH; i++)
		iter_type[i] = ITER_NONE;

	deleting = false;
	num_actors = num_neighbors = num_items = num_others = iter_depth = 0;
	neighbors = new cList();
	items = new cList();
	others = new cList();

	actor_delete_ok = false;

	CheckInvariants(__LINE__);
	return;
}


// Add an Actor
BOOL cActorList::InsertActor(cActor *actor) 
{
	switch (actor->Type())
	{
		case ITEM: 
			items->Insert(actor);
			num_items++;
			break;
		case NEIGHBOR:	
			neighbors->Insert(actor);
			num_neighbors++;
			break;
		default: 
			others->Insert(actor);
			num_others++;
			break;
	}		
	num_actors++;

	CheckInvariants(__LINE__);
	//_tprintf("Inserted actor...type = %d num = %d neighbors = %d\n",actor->Type(),num_actors,num_neighbors);
	return TRUE;
}


// Remove an Actor from the list
void cActorList::RemoveActor(cActor *actor)
{
	switch (actor->Type())
	{
		case ITEM:
			items->Remove(actor);
			num_items--;
			break;
		case NEIGHBOR: 
			neighbors->Remove(actor);
			num_neighbors--;
			break;
		default:
			others->Remove(actor);
			num_others--;
			break;
	}
	num_actors--;

	CheckInvariants(__LINE__);
	return;
}

// This function will be used to iterate through the actors

cActor* cActorList::IterateActors(int status)
{
	cActor *current = NO_ACTOR;

	if (deleting)
		return NO_ACTOR;

	if (status == NEXT) 
	{	// send items, then neighbors, then others
		if (iter_type[iter_depth] == ITER_ITEM)
		{  // return an item if any left, else try to return a neighbor
			current = items->Iterate(NEXT);
			if (current != NO_ACTOR)
				return current;
			else
			{  // no items left, shift to neighbor or other mode
				items->Iterate(DONE);
				if (num_neighbors)
				{
					iter_type[iter_depth] = ITER_NEIGHBOR;
					return neighbors->Iterate(INIT);
				}
				else
				{
					iter_type[iter_depth] = ITER_OTHER;
					return others->Iterate(INIT);
				}
			}
		}
		else if (iter_type[iter_depth] == ITER_NEIGHBOR)
		{
			current = neighbors->Iterate(NEXT);
			if (current != NO_ACTOR)
				return current;
			else
			{  // no neighborsleft, shift to other mode
				neighbors->Iterate(DONE);
				iter_type[iter_depth] = ITER_OTHER;
				return others->Iterate(INIT);
			}
		}
		else if (iter_type[iter_depth] == ITER_OTHER)
		{
			current = others->Iterate(NEXT);
			if (current != NO_ACTOR)
				return current;
			else
			{ // reached end of actors
				others->Iterate(DONE);
				iter_type[iter_depth] = ITER_NONE;
				return NO_ACTOR;
			}
		}
	}
	else if (status == INIT) // initialize for new iteration
	{
		iter_depth++;
		ASSERT (iter_depth < MAX_ITER_DEPTH);
		// only return NO_ACTOR if list is empty
		if (num_items)
		{
			iter_type[iter_depth] = ITER_ITEM;
			return items->Iterate(INIT);
		}
		else if (num_neighbors)
		{
			iter_type[iter_depth] = ITER_NEIGHBOR;
			return neighbors->Iterate(INIT);
		}
		else if (num_others)
		{
			iter_type[iter_depth] = ITER_OTHER;
			return others->Iterate(INIT);
		}
		else
			return NO_ACTOR;
	}
	else if (status == DONE)
	{
		if (iter_type[iter_depth] == ITER_ITEM)
			items->Iterate(DONE);
		else if (iter_type[iter_depth] == ITER_NEIGHBOR)
			neighbors->Iterate(DONE);
		else if (iter_type[iter_depth] == ITER_OTHER)
			others->Iterate(DONE);
		iter_type[iter_depth] = ITER_NONE;
		iter_depth--;
		return NO_ACTOR;
	}
	return NO_ACTOR; 
}

cNeighbor* cActorList::IterateNeighbors(int status) 
{ 
	if (deleting)
		return NO_ACTOR;

	return (cNeighbor*)(neighbors->Iterate(status)); 
}

cItem* cActorList::IterateItems(int status) 
{
	if (deleting)
		return NO_ACTOR;

	return (cItem*)(items->Iterate(status)); 
}


// Given a player ID, looks up the neighbor in the actor list

cNeighbor* cActorList::LookUpNeighbor(realmid_t playerID)
{
	cActor *a;

	if (deleting)
		return NO_ACTOR;
	for (a = neighbors->Iterate(INIT); a != NO_ACTOR; a = neighbors->Iterate(NEXT)) 
	{
	//	for (a = neighbors->Head(); a != NO_ACTOR; a = a->Next())
		if (((cNeighbor*)a)->ID() == playerID) {
			neighbors->Iterate(DONE);
			return ((cNeighbor*)a);
		}
	}
	neighbors->Iterate(DONE);
	return NO_ACTOR; // not found
}

// return the # of neighbors that are not hidden (GM invis)
int cActorList::NumNonHiddenNeighbors(void)
{
	cActor *a;

	if (deleting)
		return 0;

	int count = 0;
	for (a = neighbors->Iterate(INIT); a != NO_ACTOR; a = neighbors->Iterate(NEXT))
	{
		// for (a = neighbors->Head(); a != NO_ACTOR; a = a->Next())
		if (0 == ((cNeighbor*)a)->Avatar().Hidden())
			count++;
	}
	neighbors->Iterate(DONE);
	return count;

}

cItem* cActorList::LookUpItem(const LmItemHdr& item_id)
{
	cActor *a;
	
	if (deleting)
		return NO_ACTOR;

	for (a = items->Iterate(INIT); a != NO_ACTOR; a = items->Iterate(NEXT))
	{
		// for (a = items->Head(); a != NO_ACTOR; a = a->Next())
		if (((cItem*)a)->ID().Equals(item_id)) {
			items->Iterate(DONE);
			return ((cItem*)a);
		}
	}
	items->Iterate(DONE);
	return NO_ITEM; // not found
}

// validate an actor pointer
BOOL cActorList::ValidActor(cActor *actor)
{
	if (deleting)
		return FALSE;

	if (items->ValidElement(actor) || neighbors->ValidElement(actor) || others->ValidElement(actor))
		return TRUE;
	else
		return FALSE; // not found
}

BOOL cActorList::ValidNeighbor(cActor *actor) 
{ 
	if (deleting)
		return FALSE;

	return neighbors->ValidElement(actor); 
}

BOOL cActorList::ValidItem(cActor *actor) 
{ 
	if (deleting)
		return FALSE;

	return items->ValidElement(actor); 
}

void cActorList::Purge()
{
	actor_delete_ok = true;
	for (cActor *actor=IterateActors(INIT); actor != NO_ACTOR; actor=IterateActors(NEXT))
		if (actor->Terminate())
			delete actor;
	IterateActors(DONE);
	actor_delete_ok = false;
}

// Destructor
cActorList::~cActorList(void)
{
	deleting = true;

#ifdef UL_DEBUG
	if (neighbors == (cList*)0xdddddddd)
	_tprintf(_T("WARNING: ~cActorList deleting allready deleted \"neighbors\""));
	else
#endif//UL_DEBUG
	if (neighbors)
	{
		delete neighbors; 
		neighbors = NULL; 
	}
#ifdef UL_DEBUG
	if (items == (cList*)0xdddddddd)
	_tprintf(_T("WARNING: ~cActorList deleting allready deleted \"items\""));
	else
#endif//UL_DEBUG
	if (items)
	{
		delete items;
		items = NULL;
	}
#ifdef UL_DEBUG
	if (others == (cList*)0xdddddddd)
	_tprintf(_T("WARNING: ~cActorList deleting allready deleted \"others\""));
	else
#endif//UL_DEBUG
	if (others)
	{
		delete others;
		others = NULL;
	}

#ifdef AGENT
	TlsSetValue(tlsActors, NULL);
#endif

}

// Check invariants

#ifdef CHECK_INVARIANTS

void cActorList::CheckInvariants(int line)
{
	if (num_actors != (num_items + num_neighbors + num_others))
	{
		LoadString(hInstance, IDS_ACTORLIST_INVARIANT1, message, sizeof(message));
		_stprintf(errbuf,message,num_actors,num_items,num_neighbors,num_others, line);
		GAME_ERROR(errbuf);
		return;
	}

	// Ensure valid element counts
	if (neighbors && (num_neighbors != neighbors->NumElements()))
	{
		LoadString(hInstance, IDS_ACTORLIST_INVARIANT2, message, sizeof(message));
		_stprintf(errbuf,message,num_neighbors, neighbors->NumElements(), line);
		GAME_ERROR(errbuf);
		return;
	}	

	if (items && (num_items != items->NumElements()))
	{
		LoadString(hInstance, IDS_ACTORLIST_INVARIANT3, message, sizeof(message));
		_stprintf(errbuf,message,num_items, items->NumElements(), line);
		GAME_ERROR(errbuf);
		return;
	}	

	if (others && (num_others != others->NumElements()))
	{
		LoadString(hInstance, IDS_ACTORLIST_INVARIANT4, message, sizeof(message));
		_stprintf(errbuf,message,num_others, others->NumElements(), line);
		GAME_ERROR(errbuf);
		return;
	}	

	return;

}

#endif
