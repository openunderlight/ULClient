// The List Class: a container implemented as a doubly-linked list

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <stdio.h>
#include "resource.h"
#include "cList.h"

const int MAX_ELEMENTS = 1024; // more than this should indicate an error

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;

///////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
cList::cList(void)
{
	list_head = list_tail = NO_ACTOR;
	num_elements = iter_depth = 0;
	for (int i=0; i<MAX_ITER_DEPTH; i++)
		next_iter[i] = NO_ACTOR;

	CheckInvariants(__LINE__);
	return;
}


// Add an element
BOOL cList::Insert(cActor *actor) 
{
	if (num_elements == MAX_ELEMENTS)
	{
		NONFATAL_ERROR(IDS_MAX_LIST);
		return FALSE;
	}

	// Always add new elements to the end of the list
	actor->next = NO_ACTOR;
	actor->prev = list_tail;
	list_tail = actor;

	if (num_elements == 0) // if this is the first actor added...
		list_head = actor;

	// now reset the next/prev pointers for surrounding actors
	if (actor->prev != NO_ACTOR)
		actor->prev->next = actor; // set previous actor's next to point to new actor
	if (actor->next != NO_ACTOR)
		actor->next->prev = actor; // set next actor's prev to point ot new actor
		
	num_elements++;

	//_tprintf("actor inserted; checking invariants.\n");
	
	CheckInvariants(__LINE__);
	return TRUE;
}


// Remove an element from the list
void cList::Remove(cActor *actor)
{
	// ensure this actor is not next in an iteration list
	for (int i=0; i<MAX_ITER_DEPTH; i++)
		if (next_iter[i] == actor)
		{
			//_tprintf("WHOA! Caught potential problem!\n");
			next_iter[i] = actor->Next();
		}

	// fix pointers for surrounding actors
	if (actor->prev != NO_ACTOR)
		actor->prev->next = actor->next;
	if (actor->next != NO_ACTOR)
		actor->next->prev = actor->prev;

	if (actor == list_tail)
		list_tail = actor->prev;
	if (actor == list_head)
		list_head = actor->next;

	num_elements--;

	//_tprintf("removed actor %d...type = %d num= %d\n",actor,actor->Type(),num_actors);

	CheckInvariants(__LINE__);
	return;
}

// This function will be used to iterate through the list; it
// ensures that Next() will never be called on a deleted element
// by setting the next pointer before returning the pointer.

cActor* cList::Iterate(int status)
{
	cActor *curriter;

	if (status == NEXT) // produce the next element for the current iteration
	{
		curriter = next_iter[iter_depth];
		if (next_iter[iter_depth] != NO_ACTOR)
			next_iter[iter_depth] = curriter->Next();
	}
	else if (status == INIT) // initialize for new iteration
	{
		iter_depth++;
		ASSERT (iter_depth < MAX_ITER_DEPTH);
		if (num_elements == 0)
		{
			next_iter[iter_depth] = NO_ACTOR;
			return NO_ACTOR;
		}
		else
		{
			curriter = list_head;
			next_iter[iter_depth] = curriter->Next();
		}
	}
	else if (status == DONE)
	{
		iter_depth--;
		curriter = NO_ACTOR;
	}

	return curriter;
}

// validate a pointer
BOOL cList::ValidElement(cActor *actor)
{
	cActor *a;
	
	for (a = list_head; a != NO_ACTOR; a = a->next)
		if (a == actor)
			return TRUE;

	return FALSE; // not found
}



// Dump out all elements!
void cList::Dump(void)
{
	cActor *actor=list_head;
	int i=0;
	LoadString(hInstance, IDS_NUM_ELEM , temp_message, sizeof(temp_message));
	_stprintf(errbuf, temp_message,num_elements);
	INFO(errbuf);

	while (actor != NO_ACTOR)
	{
		LoadString(hInstance, IDS_ACTORS , temp_message, sizeof(temp_message));
		_stprintf(errbuf, temp_message,i,actor->Type());
		INFO(errbuf);
		i++;
		actor = actor->next;
	}
}


// Destructor
cList::~cList(void)
{
	cActor *this_actor=list_head, *next_actor;

	while (num_elements && this_actor)
	{
		next_actor = this_actor->next;
		delete this_actor;
		this_actor = next_actor;
	}
}

#if 0
#ifdef _DEBUG
	if (this == (cList*)0xdddddddd)
	{
	_tprintf("WARNING: ~cList destructor (~cList) called for previously deleted cList");
		return;
	}
#endif//_DEBUG

	// This will loop, at each iteration causing the element to be deleted
	// and hence causing it to be removed from the list (since every actor or
	// derived class has a destructor that will remove it from the list)
	while (num_elements)
	{
#ifdef _DEBUG
		if((num_elements < 0) || num_elements > 10000)
			break;

		if(list_head && ((list_head->flags == 0xdddddddd) || // deleted (_DEBUG) element
							  (list_head->flags == 0xfeeefeee)) )
		{
		   cActor* next_iter[MAX_ITER_DEPTH]; // used for iterations

			num_elements--;
			continue;
		}
#endif//_DEBUG
		delete list_head;
//		list_head = NULL;//mket
	}
#endif

// Check invariants

#ifdef CHECK_INVARIANTS

void cList::CheckInvariants(int line)
{
	int i;
	cActor *actor;

	// Checks for the list as a whole

	// Ensure that # of elements in list == num_elements
	i=0;
	actor = list_head;
	while (actor != NO_ACTOR)
	{
		i++;
		actor = actor->next;
	}
	if (i != num_elements)
	{
		GAME_ERROR3(IDS_LISTERR1, num_elements,i,line);
	}


	// If 0 elements, list_head = list_tail = NO_ACTOR
	if ((num_elements == 0) && (list_head != NO_ACTOR))
	{
		GAME_ERROR1(IDS_LISTERR2, line);
	}
	if ((num_elements == 0) && (list_tail != NO_ACTOR))
	{
		GAME_ERROR1(IDS_LISTERR3, line);
	}
	// If 1 actor, list_head = list_tail != NO_ACTOR
	if ((num_elements == 1) && ((list_tail != list_head) || (list_tail == NO_ACTOR)))
	{
		GAME_ERROR3(IDS_LISTERR3, list_head, list_tail, line);
	}	

	return;

}

#endif
