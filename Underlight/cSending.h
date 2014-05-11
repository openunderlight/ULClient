// Header file for cSending

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CSENDING_H
#define CSENDING_H

#include "Central.h"
#include "LmAvatar.h"
#include "cActor.h"


//////////////////////////////////////////////////////////////////
// Constants

#define NO_SENDING 0

const float MAX_SENDING_MOVE = 2.0f; 
const float MIN_SENDING_DIST = 10000.0f; 
const DWORD SENDING_SHOT_INTERVAL = 3000;

//////////////////////////////////////////////////////////////////
// Enumerations

enum sending_type {
	GENERIC_SENDING = 0,
	UGLYDUDE = 1,
};


///////////////////////////////////////////////////////////////////
// Structures 


//////////////////////////////////////////////////////////////////
// Class Definition

class cSending : public cActor
{
   friend class cActorList;

   public:

   protected:
		int			avatar; // type of sending
		int			health;
		DWORD		last_shot;


   public:
	   cSending(float s_x, float s_y, int s_angle, int avatar_type, unsigned __int64 flags = 0); 
	  ~cSending(void);

	  void ChangeHealth(int healthchange);
	  virtual bool LeftClick(void);
	  virtual bool RightClick(void);
	  virtual bool Render(void);
	  inline bool SurviveLevelChange(void) { return FALSE; };



   protected:

   private:
	  virtual bool Update(void);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cSending(const cSending& x);
	cSending& operator=(const cSending& x);

	
#ifdef DEBUG
	  void Debug_CheckInvariants(int caller);
#endif

};
#endif

