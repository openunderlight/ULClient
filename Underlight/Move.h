// Header file for init.cpp
// Header file for move.cpp

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef MOVE_H
#define MOVE_H

#define STRICT

#include "SharedConstants.h"
#include "4dx.h"
#include "cLevel.h"
#include "Central.h"

//////////////////////////////////////////////////////////////////
// Constants

const float MANUAL_TRIP_DISTANCE = 128.0f;
const int HITMAX = 128;


enum hit_type {
	NO_HIT = 0,				// clear move
	HIT_ACTOR = 1,          // hit actor
	HIT_WALL = 2,           // hit wall
	HIT_FLOOR = 3,			// hit the floor
	HIT_CEILING = 4,		// hit the ceiling
	TELEPORTED = 5,
	HIT_JUMPABLE = 6,		// hit a wall that's jumpable (for agents)
};

enum move_type {
	MOVE_NORMAL = 1, // normal move
	MOVE_CHECK = 2, // check for collisions only
	MOVE_TRIP = 3,  // check for manual trips only
	MOVE_FIND_TELEPORTAL = 4, // find a teleportal
};

enum trip_message_type {
	NO_TRIP_MESSAGE = 0,
	TRAIN_JUMP,
	TRAIN_TELEPORTAL,

	TRAIN_CHAT_START,
	TRAIN_TALK,
	TRAIN_SHOUT,
	TRAIN_WHISPER,
	TRAIN_EMOTE,
	TRAIN_BOW,
	TRAIN_CHAT_FINISH,
	
	TRAIN_SKILLS_START, // 10
	TRAIN_SKILLS_SECOND,
	TRAIN_SKILLS_THIRD,
	TRAIN_SKILLS_KNOW,
	TRAIN_SKILLS_MEDITATE,
	TRAIN_SKILLS_TRAIL,
	TRAIN_SKILLS_DRAIN_NIGHTMARE,
	TRAIN_SKILLS_RANDOM,
	TRAIN_SKILLS_FOCUS,
	TRAIN_SKILLS_JOINPARTY,
	TRAIN_SKILLS_FINISH, // 20
	
	TRAIN_INVENTORY_START,
	TRAIN_INVENTORY_MID,
	TRAIN_INVENTORY_FINISH,
	
	TRAIN_HOUSES_START,
	TRAIN_HOUSES_MOON,
	TRAIN_HOUSES_ECLIPSE,
	TRAIN_HOUSES_SHADOW,
	TRAIN_HOUSES_COVENANT,
	TRAIN_HOUSES_RADIANCE,
	TRAIN_HOUSES_CALENTURE, // 30
	TRAIN_HOUSES_ENTRANCED,
	TRAIN_HOUSES_LIGHT,

	TRAIN_FOLLOW_TORCHES,
	TRAIN_SKILLS_GIVE,
	TRAIN_SKILLS_MAJORMINOR,
	TRAIN_SKILLS_POWER,
	TRAIN_SKILLS_ORBIT,
	TRAIN_HOUSES_CITYMAP,
	TRAIN_HOUSES_CONSIDER,
	TRAIN_HOUSES_FINDTEACHER, // 40
	TRAIN_HOUSES_TRAINAGAIN,
	
	TRAIN_SANCTUARY_START,
	TRAIN_SANCTUARY_FINISH,
	TRAIN_SKILLS_LOCATE_AVATAR, 
	TRAIN_INVENTORY_EXIT,

	LIBRARY_ENTRANCE,

	TRAIN_SKILLS_SENSE,
	TRAIN_BELIEF_MOON,
	TRAIN_BELIEF_ECLIPSE,
	TRAIN_BELIEF_SHADOW, //50
	TRAIN_BELIEF_COVENANT,
	TRAIN_BELIEF_RADIANCE,
	TRAIN_BELIEF_CALENTURE,
	TRAIN_BELIEF_ENTRANCED,
	TRAIN_BELIEF_LIGHT, //55

	TRAIN_QUEST_BUILDER1,
	TRAIN_QUEST_BUILDER2

};



///////////////////////////////////////////////////////////////////
// Structures 

// for reporting the results of a move
struct move_result_t
{
	float			dist;
	int				hit;
	linedef*		l;
};

struct hit_t
   {
   linedef *l;
   int   bound;
   float    x;
   float    y;
   float  slide_x; // suggested position for sliding on walls
   float  slide_y;
   float slide_distance;
   float dist;
   };

// parameters for current move
struct move_params_t
{
	float	oldx;
	float	oldy;
	float	newx;
	float	newy;
	float   inter_x;
	float	inter_y;
	float	movedist;
	int		moveangle;
	int		origangle;
	float   perps2;
	float   perpc2;
	int		slide_count;
	int		numhits;
	int		numbufferzones;
	bool	corner_tested;
	linedef *ignorecorner;
	hit_t	hits[HITMAX];
	cActor  *ouractor;
	BOOL	backwards;
	int		move_type;
};


//////////////////////////////////////////////////////////////////////
// Function Prototypes

void InitCollisionDetection(void);

float sortdist(float x1, float y1, float x2, float y2);
int GetFacingAngle(float x1, float y1, float x2, float y2);
void AddEvent(long type, long sector1, long sector2, long newval, long addval, float x, float y);
int PointInSector(float x,float y, sector * sec);
void AddHit(move_params_t *m, linedef *l, int bound, float dist, float x=0.0f, float y=0.0f, float slide_x=0.0f, float slide_y=0.0f, float slide_distance=0.0f);
int __cdecl SortHits( const void *p1, const void *p2 );
int OnRight(linedef *aLine, float Px, float Py);
int InsertL(linedef *l, int currsec, move_params_t *m, unsigned char *seclist);
void AddLines(int addsec, int currsec, move_params_t *m, unsigned char *seclist);
inline void DMemSet(void *dst, int value, int sz);
static void VectorMove(move_params_t *m, move_result_t *result = NULL);
void MoveActor(cActor *movactor, long angle, float distance, int move_type = MOVE_NORMAL, move_result_t *result = NULL);
int FindIntersectAngle(cActor *a, linedef *line);
linedef* FindTeleportal(cActor *actor);
linedef *FindLDef(short from, short to);
bool CanPassPortal(int lock, int guild_id, bool rendering = false);
int  FindSector(float  x, float  y, int oldsector, bool set_sector_anywhere);
float fdist(float x1,float  y1,float  x2,float y2);
// Returns distance from point to line. The line is extended, so be careful!
float LineDistance(float  cx, float cy, linedef *l);
// returns true if the point is on the face of the line, false if it is off to one side.
int   On_Line(float pcx, float pcy, linedef *l);
// returns true if point tx,ty is actually physically on the line aLine
int PointOnLine(float tx, float ty, linedef *aLine);
int GetTripGuild(int flags);
void transform_point(float cx,float cy,int angle, float &x, float &y );

///////////////////////////////////////////////////////
// External Function Prototypes
void SetLineAngles(struct linedef *aLine);


#endif


