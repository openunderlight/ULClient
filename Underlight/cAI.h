// Header file for AI class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CAI_H
#define CAI_H

#include "Central.h"
#include <windows.h>
#include "GMsg_All.h"
#include "cPlayer.h"
#include "Move.h"

//////////////////////////////////////////////////////////////////
// Constants

const int MAX_NEIGHBORS = 256;
//const int MAX_PREDET_MOVES = 3;
///////////////////////////////////////////////////////////////////
// Structures 

struct neighbor_dist_t
{
	int		index;
	float	dist;
};

enum direction_t
{
	LEFT = 0,
	RIGHT = 1,
};


// qsort comparison function
int __cdecl dist_compare( const void *arg1, const void *arg2 );


//////////////////////////////////////////////////////////////////
// Class Definition

class cAI : public cPlayer
{
   public:

   protected:
	  int	agent_type; // avatar type for this agent
	  cNeighbor *neighbors[MAX_NEIGHBORS]; // array of current neighbors, used by AI
	  int num_neighbors;	// number of current neighbors
	  neighbor_dist_t neighbor_dists[MAX_NEIGHBORS]; // distances to neighbors
	  bool distances_calculated; // set to show that distances have been calculated for a given update
	  bool moved; // enforces only one move per frame
	  float spawn_x, spawn_y;
	  DWORD reconnect;
	  short kills;
	  short deaths;
	  int	min_distance;
	  DWORD login_time;
	  DWORD last_neighbor_check_time;
	  DWORD last_stat_write_time; // save stats every hour
	  int   alone_ticks;
	  int   social_ticks;
	  bool  melee_only; // type of attacks
	  bool	invis_neighbors; // any invisible neighbors?
	  lyra_id_t last_target;
	  lyra_id_t next_target_id;
	  float last_target_x, last_target_y;
	  cMissile *view_missile; // used to determine neighbor visibility
	  int	num_stuck_frames; // number of frames we've been stuck & can't move
	  int	num_sightless_frames; // number of frames since we've seen our target
	  int	num_rampaging_frames; // number of frames we've been rampaging
	  int	num_frames_to_rampage; // number of frames we want to rampage
	  direction_t last_evasive_direction;
	  bool	alone; // are there any players with us?
	  bool  has_been_struck; // have we been hit yet?
	  bool	unsticking;
	  bool	wandering;
	  bool	rampaging;
	  bool  attack_other_mares;
      bool soulsphere_neighbors;

   public:
#if (defined(PMARE)) || (defined(AGENT))
	  cAI(float xpos, float ypos, int anglepos, int delay, int mare_type = 0);
#else
	  cAI(float xpos, float ypos,  int anglepos, int delay, int mare_type = -1);
#endif
	  virtual ~cAI(void);
	  bool DetermineAlone(void);
	  inline bool Alone(void) {	return alone; };
	  void Init(void); // creates the view missile
	  void FindRespawn(GMsg_LevelPlayers& players_msg);
	  void SetAgentStats(void);

	  // overloaded player methods
	  int SetXP(int value, int how, bool initializing=false);
	  void Dissolve(lyra_id_t origin_id, int talisman_strength=1);
	  TCHAR* Name(void);
	  TCHAR* Password(void);
	  virtual bool Update(void);

	  // Selection Functions

	  unsigned char PercentBusy(void);
	  inline short Kills(void) { return kills; };
	  inline short Deaths(void) { return deaths; };
	  bool HasBeenStruck(void);
	  bool HasBeenStruck(int view);

	  // Modify Member Functions
	  inline void NewKill(void) { kills++; };
	  inline void NewDeath(void) { deaths++; };
	  inline void SetNextTargetID(lyra_id_t value) { next_target_id = value; };
	  inline void StopRampaging(void) { rampaging = false; num_rampaging_frames = 0; };
	  inline void StopWandering(void) { wandering = false;};
	  int SetAngle(int new_angle);
	  int AddAngle(int new_angle);

	  // Enemy detection
	  int AcquireTarget(void);
	  int GetNeighborFacingAngle(int index);
	  int FindNearestNeighbor(int position);
	  float NeighborDistance(int index);
	  bool SetVerticalTilt(int target);
	  bool NeighborVisible(int index);
	  bool NeighborHittable(int index);
	  bool TargetEntering(int index);
	  TCHAR* NeighborName(int index);
	  void FireViewMissile(int angle,float distance,move_result_t *move);

	  // Moving the agent
	  void MakeMove(void); // Decide and make a move
	  float Move(int angle); // Move the actor
	  float MoveForward(void); // Move forward
	  float Strafe(direction_t direction);
	  void Jump(void);
	  void TurnTo(int desired_angle);

	  // Behavioral movement
	  float Wander(void); // Wander
	  float Rampage(void); // Rampage!!
	  void PursueTarget(int index); // Get 'em!
	  bool PursueLostTarget(void); // Are we still pursuing?
	  void TakeEvasiveAction(void);
	  bool UnstickSelf(void);
	  // Behavioral angles
	  int  EvasiveAngle(direction_t direction);
	  int  WanderAngle(void);
	  int  RampageAngle(void);
	  int  AvoidWallAngle(int angle);	

	  void Speak(TCHAR *message, int index);
	  void SetReconnect(DWORD value) { reconnect = value; };
	
   private:



#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cAI(const cAI& x);
	cAI& operator=(const cAI& x);
   };

#endif






