// Header file for the Ornament class - derived from the Actor class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CORNMAMENT_H
#define CORNMAMENT_H

#include "Central.h"
#include "cActor.h"

//////////////////////////////////////////////////////////////////
// Constants

#define NO_ORNAMENT		0

///////////////////////////////////////////////////////////////////
// Structures 
struct damaging_ornament_t {
	int bitmap_id;
	int stat;
	int modifier;
	int effect;
	int duration;
};

///////////////////////////////////////////////////////////////////
// Helper Functions


//////////////////////////////////////////////////////////////////
// Class Definition

class cOrnament : public cActor
{
   friend class cActorList;

   public:

   protected:
		realmid_t id; 
		float dest_x,dest_y;
		bool moving;
		int data; // generic data for ornaments
		bool is_damaging_ornament;
		damaging_ornament_t damage;

   public:
	   cOrnament(float x, float y, float relative_z, int angle,  unsigned __int64 flags, int a_id, 
		   TCHAR *name = NULL, float d_x = 0.0f, float d_y = 0.0f); 
	  ~cOrnament(void);
	   inline realmid_t ID(void) { return id;};
	   inline int Data(void) { return data;};
	   inline void SetData(int value) { data = value; };
	   inline bool IsDamagingOrnament(void) { return is_damaging_ornament; };
	   inline damaging_ornament_t GetDamageInfo(void) { return damage; };

   protected:
 	  virtual bool Update(void);
	  virtual bool LeftClick(void);  
	  virtual bool RightClick(void);
	  virtual bool Render(void);
	  virtual int CurrBitmap(int view_angle);
	  inline bool SurviveLevelChange(void) { return FALSE; };


   private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cOrnament(const cOrnament& x);
	cOrnament& operator=(const cOrnament& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif
};
#endif
