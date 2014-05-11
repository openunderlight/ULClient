// Header file for Actor List class - actor container

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CACTORLIST_H
#define CACTORLIST_H

#include "Central.h"
#include "cNeighbor.h"
#include "cItem.h"
#include "cMissile.h"
#include "cList.h"
#include "LyraDefs.h"

//////////////////////////////////////////////////////////////////
// Constants

enum iterating_type {  
  ITER_NONE = 0,
  ITER_NEIGHBOR = 1,           
  ITER_ITEM = 2,   
  ITER_OTHER = 3, 
};

///////////////////////////////////////////////////////////////////
// Structures 

//////////////////////////////////////////////////////////////////
// Class Definition

class cActorList
{
   public:

   private:
	   cList *neighbors;
	   cList *items;
	   cList *others;
		   
	   int num_actors;		// number of total actors in the list
	   int num_neighbors;   // number of neighbors in the list
	   int num_items;		// number of items in the list
	   int num_others;		// number of non neighbor/item actors

	   int iter_type[MAX_ITER_DEPTH]; // type of actors we're iterating now
	   int iter_depth; // # of active actor iterations

	   bool deleting;
		 bool actor_delete_ok;
	   
   public:
      cActorList(void);
	  ~cActorList(void);

	  // Inserting/Deleting Actors
	  BOOL InsertActor(cActor *actor);
	  void RemoveActor(cActor *actor);

	  // Selection Functions
	  inline int NumActors(void) { return num_actors; };
	  inline int NumNeighbors(void) { return num_neighbors; };
	  int NumNonHiddenNeighbors(void);
	  inline int NumItems(void) { return num_items; };
	  inline int NumOthers(void) { return num_others; };
	  inline bool ActorDeleteOK(void) {return actor_delete_ok;};

	  // Pointer Validation Functions
	  BOOL ValidActor(cActor *actor); // is this actor pointer valid?
	  BOOL ValidNeighbor(cActor *actor);
	  BOOL ValidItem(cActor *actor);

	  cActor* IterateActors(int status);
	  cNeighbor* IterateNeighbors(int status);
	  cItem* IterateItems(int status);

	  cNeighbor* LookUpNeighbor(realmid_t playerID);
	  cItem* LookUpItem(const LmItemHdr& item_id);

		void Purge(void);  // removes all actors that have been set to terminate

   private:
	  // copy constructor and assignment operator are
	  // private and undefined -> errors if used
	  cActorList(const cActorList& x);
	  cActorList& operator=(const cActorList& x);

#ifdef CHECK_INVARIANTS
		void CheckInvariants(int line);
#else
		inline void CheckInvariants(int line) {};
#endif

   };
#endif



