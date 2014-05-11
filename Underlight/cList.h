// Header file for List class

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CLIST_H
#define CLIST_H

#include <windows.h>
#include "cActor.h"
#include "Central.h"

//////////////////////////////////////////////////////////////////
// Constants



enum iterate_type {  
  INIT = 0,           // initialize new iteration
  NEXT = 1,           // get next value
  DONE = 2,           // finished with iteration
};

const int MAX_ITER_DEPTH = 12; 

///////////////////////////////////////////////////////////////////
// Structures 


//////////////////////////////////////////////////////////////////
// Class Definition

class cList
   {
   public:

   private:
	   cActor *list_head; // head of the entire list
	   cActor *list_tail; // tail of the entire list
	   int num_elements;  // number of total elements in the list

	   // variables for iterating
	   int iter_depth;	  // # of active iterations
	   cActor* next_iter[MAX_ITER_DEPTH]; // used for iterations

   public:
      cList(void);
	  ~cList(void);

	  // Inserting/Deleting Actors
	  BOOL Insert(cActor *actor);
	  void Remove(cActor *actor);

	  // Selection Functions
	  inline cActor *Head(void) { return list_head; };
	  inline cActor *Tail(void) { return list_tail; };
	  inline int NumElements(void) { return num_elements; };
	  inline int IterDepth(void) { return iter_depth; };

	  // Pointer Validation Function
	  BOOL ValidElement(cActor *actor); // is this pointer valid?

	  cActor* Iterate(int status);

	  void Dump(void); // print actor info to debug.out

   private:
	  // copy constructor and assignment operator are
	  // private and undefined -> errors if used
	  cList(const cList& x);
	  cList& operator=(const cList& x);

#ifdef CHECK_INVARIANTS
		void CheckInvariants(int line);
#else
		inline void CheckInvariants(int line) {};
#endif

   };
#endif



