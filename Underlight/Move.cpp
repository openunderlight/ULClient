// Move: movement and collision detection

// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

// Trig 101:
// sine				= (y2-y1)/length;
// cosine			= (x2-x1)/length;
// norm_sine		= cosine;
// norm_cos 		= 0-sine;
// perp_sine		= 0-cosine
// perp_cos 		= sine;

#include "Central.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "4dx.h"
#include "cDDraw.h"
#include "cMissile.h"
#include "cPlayer.h"
#include "cChat.h"
#include "cDSound.h"
#include "cEffects.h"
#include "cGoalPosting.h"
#include "cQuestBuilder.h"
#include "cAgentBox.h"
#include "cActorList.h"
#include "cGameServer.h"
#include "Options.h"
#include "cLevel.h"
#include "Move.h"
#include "resource.h"
#include "realm.h"
#include "Utils.h"
#include "cGoalbook.h"

//////////////////////////////////////////////////////////////////
// Constants

const float BUFFER_DIST = 75.0f;
const int MAX_SLIDES = 3;
const unsigned int DONT_INTERSECT	 = 0;
const unsigned int DO_INTERSECT		 = 1;
const unsigned int COLLINEAR			 = 0;
const int NO_INITIATES = 4; // for teleportal locking


/////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cPlayer *player;
extern cDSound *cDS;
extern cActorList *actors;
extern cArts *arts;
extern cChat *display;
extern cGameServer *gs;
extern cLevel *level;
extern cDDraw *cDD;
extern cGoalPosting *goals;
extern cQuestBuilder *quests;
extern cGoalBook *goalbook;
extern cAgentBox *agentbox;
extern BOOL debugging;
extern options_t options;
extern cEffects *effects;
extern bool acceptrejectdlg;
extern bool show_training_messages;

////////////////////////////////////////////////////////////////////////////////////////
//Local Function Prototyes.
static int PlayerTripLine(linedef *aLine);

////////////////////////////////////////////////////////////////////////////////////////
int __cdecl compare( const void *p1, const void *p2 )
	{
		actor_sector_map_t  *a=(actor_sector_map_t  *)p1;
		actor_sector_map_t  *b=(actor_sector_map_t  *)p2;
		return(a->sector - b->sector);
	}

static void SortActors(void)
{
	int i,j,acts;

	j=9999;

	for (i=0;i<level->NumSectors();i++)
		level->SectorStart[i]=-1;

	acts = actors->NumActors();

	if ((acts < 0) || (acts > 1000))
		return;

	cActor* actor;
	level->NumOList=0;
	for (actor=actors->IterateActors(INIT); actor != NO_ACTOR; actor=actors->IterateActors(NEXT))
		//for (actor=actors->Head(); actor != NO_ACTOR; actor = actor->Next())
	{
		level->OList[level->NumOList].actor = actor;
		level->OList[level->NumOList].sector = level->OList[level->NumOList].actor->sector;
		level->NumOList++;
	}
	level->OList[level->NumOList].actor = NO_ACTOR;
	level->OList[level->NumOList].sector = -1;
	actors->IterateActors(DONE);

	qsort(&(level->OList[0]),acts,sizeof(actor_sector_map_t),compare);

	for (i=0;i<acts;i++)
	{
		if ( level->OList[i].sector != j)
		{
			j=level->OList[i].sector;
			level->SectorStart[j]=i;
		}
	}
}


void InitCollisionDetection(void)
{
	SortActors();
}

////////////////////////////////////////////////////////////////////////////////////////


// floating point abs
inline float lyra_abs(float num)
{
	if (num > 0.0f)
		return num;
	else
		return -num;
}


// Returns the angle of the vector from (x2,y2) to (x1,y1)
int GetFacingAngle(float x1, float y1, float x2, float y2)
{
	float dx,dy;
	int angle;

	dx = x1 - x2;
	dy = y1 - y2;

	// base angle on dx/dy and which quadrant...
	if (dy == 0.0f)
	{ // straight left/right
		if (dx >= 0.0f)
			angle = 0;
		else
			angle = Angle_180;
	} else if (dx == 0.0f)
	{ // straight up/down
		if (dy >= 0.0f)
			angle = Angle_90;
		else
			angle = Angle_270;
	}
	else if ((dy > 0.0f) && (dx > 0.0f))
	{ // quadrant 0
		if (dx >= dy) // between 0 and 45 deg
			angle = FixAngle((int)((float)Angle_45*(dy/dx)));
		else // between 45 and 90 deg
			angle = FixAngle((int)(Angle_90-((float)Angle_45*(dx/dy))));
	} else if ((dy > 0.0f) && (dx < 0.0f))
	{ // quadrant 1
		if (-dx >= dy) // between 135 and 180 deg
			angle = FixAngle((int)(Angle_180-((float)Angle_45*(dy/-dx))));
		else // between 90 and 135 deg
			angle = FixAngle((int)(Angle_90+((float)Angle_45*(-dx/dy))));
	} else if ((dy < 0.0f) && (dx < 0.0f))
	{ // quadrant 2
		if (-dx >= -dy) // between 180 and 225 deg
			angle = FixAngle((int)(Angle_180+((float)Angle_45*(dy/dx))));
		else // between 225 and 270 deg
			angle = FixAngle((int)(Angle_270-((float)Angle_45*(dx/dy))));
	} else if ((dy < 0.0f) && (dx > 0.0f))
	{ // quadrant 3
		if (dx >= -dy) // between 0 and 315 deg
			angle = FixAngle((int)(-(float)Angle_45*(-dy/dx)));
		else // between 270 and 315 deg
			angle = FixAngle((int)(Angle_270+((float)Angle_45*(dx/-dy))));
	}

	return FixAngle(angle);

}

// Returns the distance between the point and the line,
// extending the line to infiniti in both directions.
float LineDistance(float  cx, float cy, linedef *l)
{
	float x2,y2;

	y2 = cy-l->y2();
	x2 = l->x2()-cx;

	// If value becomes negative we are facing the line clockwise
	return -(((y2*(l->x1() - l->x2())) + (x2*(l->y1() - l->y2()))) / l->length);
}

// returns the square of the distance; this is all we need for
// sorting hits, since sqrt is monotonic

float sortdist(float x1, float y1, float x2, float y2)
{
	float dx,dy;
	dx = x1 - x2;
	dy = y1 - y2;
	return (dx*dx + dy*dy);
}

// the true distance between two points
float fdist(float x1, float y1, float x2, float y2)
{
	double dx,dy;
	dx = x1-x2;
	dy = y1-y2;
	double sum = (dx * dx) + (dy * dy);

	double result = sqrt(sum);

	float float_result = (float)result;

	//if (float_result < 0) 
	{
	//	_tprintf("NEGATIVE DISTANCE!!!!\n");
	//	_tprintf("x1: %f y1: %f  x2: %f y2: %f\n", x1, y1, x2, y2);
	//	_tprintf("dx: %e dy: %e sum: %e result: %e float result: %f\n", dx, dy, sum, result, float_result); 
	}

	return float_result;
}


// expands the line by xd and yd
// returns true if the point is within the band of the line,
// where the band is defined as the section of the plane bounded
// by lines perpendicular to the endpoints
int On_Line(float pcx, float pcy, float xd, float yd, linedef *l)
{
	float pax,pbx,pay,pby,delta_x,delta_y,line_len,r;

	pax = l->x1() - xd;
	pbx = l->x2() + xd;
	pay = l->y1() - yd;
	pby = l->y2() + yd;
	delta_x = pax-pbx;
	delta_y = pay-pby;

	/* the length of the line in **2 */
	line_len = delta_x*delta_x+delta_y*delta_y;
	r=(((pay-pcy) * delta_y)+((pax-pcx) * delta_x))/line_len;

	if ( r < 0 || r > 1)
		return 0;
	else
		return 1;
}


// Determine if a point is in a sector or not.
// Does the old "odd intersections" = inside bit.

int PointInSector(float x,float y,sector * sec)
{
	int num;
	float x1,y1,x2,y2;
	linedef *aLine;
	num = 0;
	if (sec==NULL)
	{
		NONFATAL_ERROR(IDS_NULL_PT_IN_SECTOR);
		return 0;
	}

	aLine=sec->firstline;
	if(aLine)
		do {
			x1 = aLine->x1()- x;
			y1 = aLine->y1()- y;
			x2 = aLine->x2()- x;
			y2 = aLine->y2()- y;

			if( x1 <   0 && x2 < 0)   continue;
			if( y1 >  x1 && y2 > x2)  continue;
			if( y1 < -x1 && y2 < -x2) continue;

	/*		if( y1 >= 0 && y2 < 0 || y1 < 0 && y2 >= 0)
			{
				// straddles line, lets find X.
				x_int = x1-((y1*(x2-x1))/(y2-y1));
				if( x_int >= 0.0)
				{
					num++;
				}
			}
	*/
			if( y1 >= 0 && y2 < 0 )
			{
				if (x1*(y2-y1) < y1*(x2-x1))
					num++;
			}
			else if (y1 < 0 && y2 >= 0)
			{
				if (x1*(y2-y1) >= y1*(x2-x1))
					num++;
			}
		} while(aLine = aLine->nextline);

	return(num&1);
	}

// returns linedef with vertices (from,to); returns NULL
// if not found.
linedef *FindLDef(short from, short to)
{
	sector	*sec;
	linedef *line;
	int sectornum;

	for (sectornum = level->NumSectors()-1; sectornum > DEAD_SECTOR; sectornum--)
	{
		sec = level->Sectors[sectornum];
		if (sec != NULL && sec->firstline)
		{
			line = sec->firstline;
			do {
				if ((line->from == from) && (line->to == to))
					return line;
			} while (line=line->nextline);
		}
	}
	return NULL;
}

// If oldsector is not -1 (the default), does a breadth first
// search of the current sector and the surrounding sectors to
// find the sector the point is in.
// If oldsector is -1, or if the bfs fails to find the sector,
// it loops through all sectors.

// caller:
// 1 = placeactor
// 2 = setbitmapinfo
// 3 = dsound
// 4 = citem
// 5 = missile collision 1
// 6 = missile collision 2
// 7 = missile collision 3
// 8 = missile legal pos
// 9 = neighbor update
// 10 = ornament update
// 11 = vector move 1
// 12 = vector move 2
// 13 = vector move 3
// 14 = level

int FindSector(float x, float y, int oldsector, bool set_sector_anywhere)
{
	// colors: 0=white, 1=gray, 2=black
	static unsigned char SectorColors[MAX_SECTORS];
	// queue of sectors indexes to check
	static short SectorQueue[MAX_SECTORS];
	int curr; // index into SectorQueue for current sector
	int top; // highest index into sector queue
	int sectornum;
	BOOL found = FALSE;
	linedef *line;
	sector  *pSec;

	if (level == NULL)
		return DEAD_SECTOR;

	// zero out the colors
	memset(SectorColors, 0, MAX_SECTORS);
	sectornum = DEAD_SECTOR;
	top = 0;

	if (oldsector == DEAD_SECTOR) // don't know current sector...
	{
		curr = 1; // set curr>top so bfs loop won't run
	}
	else
	{
		curr = 0;
		SectorQueue[0] = oldsector;
	}

	// do a breadth first search of all the sectors, starting
	// with the current sector and adding all facing sectors
	// in turn.

	while (curr <= top)
	{
		sectornum = SectorQueue[curr];
		SectorColors[sectornum] = 2; // set to black
		if (NULL == level->Sectors[sectornum])
		{
			curr++;
			continue;
		}

		if (PointInSector(x,y,level->Sectors[sectornum]))
		{
			found = TRUE;
			break;
		}

		line=level->Sectors[sectornum]->firstline;
		if (line)
			do {
				if ((SectorColors[line->facingsector] == 0) && 
					(level->Sectors[line->facingsector] != NULL))
				{ // add to queue if it's white
					top++;
					SectorQueue[top] = line->facingsector;
					SectorColors[line->facingsector] = (unsigned char)1; // set to gray
				}
			} while(line = line->nextline);
		curr++;
	}

	if (!found)
	{	  // didn't find via bfs; try looping...
	 // _tprintf("looking up via exhaustive search...\n");
		for (sectornum = level->NumSectors(); sectornum > DEAD_SECTOR; sectornum--)
		{
			pSec = level->Sectors[sectornum];
			if (pSec != NULL && pSec->firstline)
				if (set_sector_anywhere || (pSec->room == player->Room()))
					if ( PointInSector(x,y,pSec))
					{
						found = TRUE;
						break;
					}
		}
	}

	if (!found)
		sectornum = DEAD_SECTOR;
	//_tprintf("returning: %d\n",sectornum);
	return sectornum;
}


void AddHit(move_params_t *m, linedef *l, int bound, float dist, float x, float y, float slide_x, float slide_y, float slide_distance)
{
	if ( m->numhits < HITMAX )
	{
		m->hits[m->numhits].l 	= l;
		m->hits[m->numhits].bound= bound;
		m->hits[m->numhits].dist = dist;
		m->hits[m->numhits].x	 = x;
		m->hits[m->numhits].y	 = y;
		m->hits[m->numhits].slide_x = slide_x;
		m->hits[m->numhits].slide_y = slide_y;
		m->hits[m->numhits].slide_distance = slide_distance;
		m->numhits++;
	}
}


int __cdecl SortHits( const void *p1, const void *p2 )
{
	hit_t *a=(hit_t *)p1;
	hit_t *b=(hit_t *)p2;
	if (a->dist < b->dist)
		return -1;
	else
		return 1;
}


int OnRight(linedef *aLine, float Px, float Py)
{
	// these are all normalized and rotated, in respect to us.
	float x1,y1,x2,y2,temp;

	x1 = aLine->x1();
	y1 = aLine->y1();
	x2 = aLine->x2();
	y2 = aLine->y2();

	temp = ((x1-x2)*(Py-y2))-((y1-y2)*(Px-x2));

	if ( temp < 0 ) return 1;	// not on right!
	return 0;
}

int SameSigns (float x, float y)
{
	if (x >= 0.0f && y >= 0.0f) return 1;
	if (x < 0.0f && y <	 0.0f) return 1;
	return 0;
}

long lines_intersect(move_params_t *m, float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4)
{
	float Ax,Bx,Cx,Ay,By,Cy,d,e,f,num,offset;
	float x1lo,x1hi,y1lo,y1hi;

	Ax = x2-x1;
	Bx = x3-x4;

	if(Ax<0)						/* X bound box test*/
	{
		x1lo=x2;
		x1hi=x1;
	} 
	else
	{
		x1hi=x2; 
		x1lo=x1;
	}

	if(Bx>0) 
	{
		if(x1hi < x4 || x3 < x1lo)
			return DONT_INTERSECT;
	}
	else
	{
		if(x1hi < x3 || x4 < x1lo)
			return DONT_INTERSECT;
	}

	Ay = y2-y1;
	By = y3-y4;

	if(Ay<0)						/* Y bound box test*/
	{
		y1lo=y2;
		y1hi=y1;
	}
	else
	{
		y1hi=y2;
		y1lo=y1;
	}
	if(By>0)
	{
		if(y1hi < y4 || y3 < y1lo) 
			return DONT_INTERSECT;
	}
	else
	{
		if(y1hi < y3 || y4 < y1lo)
			return DONT_INTERSECT;
	}

	// now check that points are on opposite sides of the other line
	d = ((x1-x2)*(y3-y2))-((y1-y2)*(x3-x2));
	e = ((x1-x2)*(y4-y2))-((y1-y2)*(x4-x2));

	if ((d == 0) || (e == 0) || ((d < 0) && (e < 0)) ||  ((d > 0) && (e > 0)))
		return DONT_INTERSECT;

	d = ((x3-x4)*(y1-y4))-((y3-y4)*(x1-x4));
	e = ((x3-x4)*(y2-y4))-((y3-y4)*(x2-x4));

	if ((d == 0) || (e == 0) || ((d < 0) && (e < 0)) ||  ((d > 0) && (e > 0)))
		return DONT_INTERSECT;

	Cx = x1-x3;
	Cy = y1-y3;
	d = By*Cx - Bx*Cy;			/* alpha numerator*/
	f = Ay*Bx - Ax*By;			/* both denominator*/
	
	if(f>0.0f)						/* alpha tests*/
	{
		if(d<0.0f || d>f) 
			return DONT_INTERSECT;
	}
	else
	{
		if(d>0.0f || d<f) 
			return DONT_INTERSECT;
	}

	e = Ax*Cy - Ay*Cx;				/* beta numerator*/
	if(f>0.0f)							/* beta tests*/
	{
		if(e<0.0f || e>f)
			return DONT_INTERSECT;
	}
	else
	{
		if(e>0.0f || e<f)
			return DONT_INTERSECT;
	}

	/*compute intersection coordinates*/

	if(f==0.0f)
		return COLLINEAR;

	num = d*Ax; 												/* numerator */
	offset = SameSigns(num,f) ? f/2.0f : -f/2.0f;	/* round direction*/
	m->inter_x = x1 + (num+offset) / f; 				/* intersection x */

	num = d*Ay;
	offset = SameSigns(num,f) ? f/2.0f : -f/2.0f;
	m->inter_y = y1 + (num+offset) / f; 				/* intersection y */

	return DO_INTERSECT;
}

int InsertL(linedef *l, int currsec, move_params_t *m, unsigned char *seclist)
{
	sector *dest;
	sector *origin;
	int bound=0,hit=0;
	float newdist,olddist;
	BOOL addhit;
	float x1,y1,x2,y2,xd,yd,bufx1,bufx2,bufy1,bufy2;

	float mdx = BUFFER_DIST*CosTable[m->moveangle];
	float mdy = BUFFER_DIST*SinTable[m->moveangle];

	// determine whether we can pass the line or not.
	if (l->flags & BOUND)
		bound = 1;
	if ((l->flags & LINE_S_IMPASS) && (m->ouractor->Type() != MISSILE))
	  bound = 1; //super_impass always bounds non-missile actors
	else if (l->actor)
	{
		if (!(l->actor->flags & ACTOR_NOCOLLIDE))
		{	 // bounds if our bottom is below actor's top and our top is above actor's bottom
			if (((m->ouractor->z - m->ouractor->physht) <= (l->actor->z)) &&
				(m->ouractor->z >= (l->actor->z - l->actor->physht)))
				bound = 1;
		}
	}
	else if (l->facingsector != currsec)
	{
		dest = level->Sectors[l->facingsector];
		origin = level->Sectors[currsec];

		int hitline = lines_intersect(m, m->oldx, m->oldy, m->newx+mdx, m->newy+mdy, l->x1(), l->y1(), l->x2(), l->y2());

		float check_x, check_y;

		// pick the point at which to take floor and ceiling ht measurements
		if (hitline)
		{
			check_x = m->inter_x; check_y = m->inter_y;
		}
		else
		{
			if (  sortdist(m->newx, m->newy, l->x1(), l->y1())
				 < sortdist(m->newx, m->newy, l->x2(), l->y2()) )
			{
				check_x = l->x1(); check_y = l->y1();
			}
			else
			{
				check_x = l->x2(); check_y = l->y2();
			}
		}

		if (m->ouractor->z >= dest->CeilHt(check_x,check_y))
			bound =1; // actor hits ceiling

		else	if  ((m->ouractor->Type() == MISSILE)
				&& ((m->ouractor->z - m->ouractor->physht) <= dest->FloorHt(check_x,check_y)))
					bound = 1; // missile hits floor (no floor offset for missiles!)

		else	if (((origin->CeilHt(check_x, check_y) - (dest->FloorHt(check_x, check_y)+dest->HtOffset)) <= m->ouractor->physht)
				|| ((dest->CeilHt(check_x, check_y) - (origin->FloorHt(check_x, check_y)+origin->HtOffset)) <= m->ouractor->physht)
				|| ((origin->CeilHt(check_x, check_y) - (origin->FloorHt(check_x, check_y)+origin->HtOffset)) <= m->ouractor->physht)
				|| ((dest->CeilHt(check_x, check_y) - (dest->FloorHt(check_x, check_y)+dest->HtOffset)) <= m->ouractor->physht))
			bound = 1; // not enough vertical space for actor at boundary

		else	if ((m->ouractor->Type() != MISSILE)
				&& (dest->FloorHt(check_x,check_y)+dest->HtOffset > (origin->FloorHt(check_x, check_y) - origin->FloorHt(m->ouractor->x, m->ouractor->y) + m->ouractor->z - (m->ouractor->physht*.75))))
				// should no longer be needed, preserved until significant testing
//				|| (!hitline && (dest->FloorHt(m->oldx+mdx,m->oldy+mdy)+dest->HtOffset > (m->ouractor->z  - (m->ouractor->physht*.75))))))
			bound = 1; // too high for non-missile actor to step up into

		else if ((m->ouractor->Type() != MISSILE) && (m->move_type == MOVE_NORMAL))
			// if move to new sector is not bound, make an update to the move's z param to allow stair-climbing
		{	// only if an upwards move -- must let gravity affect the downward check
			if (hitline && (origin->FloorHt(m->inter_x, m->inter_y)+origin->HtOffset < dest->FloorHt(m->inter_x, m->inter_y)+dest->HtOffset)
			&& (LineDistance(m->newx, m->newy, l) < 12) 
			&& (m->ouractor->z < (origin->FloorHt(m->ouractor->x, m->ouractor->y)+(1.1*m->ouractor->physht))) 
			&& (m->ouractor->z > (origin->FloorHt(m->ouractor->x, m->ouractor->y)+(.9*m->ouractor->physht))))
			{
				m->ouractor->z = dest->FloorHt(m->inter_x,m->inter_y) + dest->HtOffset + m->ouractor->physht;
			}
		}
	}

	// get distance to line from m->newx and m->newy.
	// if less than bufferzone, lets also enter this line.
	if ( bound )
	{
		xd = BUFFER_DIST*l->COS;
		yd = BUFFER_DIST*l->SINE;

		// find the buffer zone line
		bufx1 = l->x1() - xd - yd;
		bufx2 = l->x2() + xd - yd;
		bufy1 = l->y1() - yd + xd;
		bufy2 = l->y2() + yd + xd;

		// Movements for the missile and non-missile actors are
		// handled differently.

		// missile:
		//		* If a missile actor crosses a physical line, a
		//		  bounding hit is recorded at the current location.
		//		* If the non-missile crosses the buffer zone, a bounding hit
		//		  is recorded at the point of crossing.A recommended slide
		//		  location is then put into the hit, and a recursive call
		//		  to VectorMove may result.
		//		* If the non-missile is in a buffer zone, a bounding hit is
		//		  recorded at the current location if the movement doesn't
		//		  move the non-missile farther from the line. A recommended
		//		  slide location is then put into the hit, as above.

		if (m->ouractor->IsMissile())
		{	// check three lines for hits - one from the current location
			// to the left edge of the new locaiton, one from the current
			// location to the right edge of the new location, one from current
			// location to the center of the new location
			if ((lines_intersect(m, m->oldx,m->oldy,
				(m->newx + m->ouractor->halfwit*CosTable[FixAngle(m->ouractor->angle + Angle_90)]),
				(m->newy + m->ouractor->halfwit*SinTable[FixAngle(m->ouractor->angle + Angle_90)]),
				l->x1(),l->y1(),l->x2(),l->y2())) ||
				(lines_intersect(m, m->oldx,m->oldy,
				(m->newx + m->ouractor->halfwit*CosTable[FixAngle(m->ouractor->angle - Angle_90)]),
				(m->newy + m->ouractor->halfwit*SinTable[FixAngle(m->ouractor->angle - Angle_90)]),
				l->x1(),l->y1(),l->x2(),l->y2())) ||
				(lines_intersect(m, m->oldx,m->oldy,m->newx,m->newy,l->x1(),l->y1(),l->x2(),l->y2())))
			{ // offset back by COLLIDE_OFFSET, after calculating middle intersect pos
					//convenient debugging stuff
//			if ((m->ouractor->Type() == MISSILE) &&
//				((l->from == 25) || (l->from == 28)) && ((l->to == 25) || (l->to == 28)))
//				int junk = 0;

				if (lines_intersect(m, m->oldx,m->oldy,m->newx+mdx,m->newy+mdy,l->x1(),l->y1(),l->x2(),l->y2()))
					AddHit(m, l, bound, sortdist(m->oldx,m->oldy,m->inter_x,m->inter_y),
							m->inter_x, m->inter_y, m->inter_x, m->inter_y, 0.0f);
				else // hitting the edge of something...
					if (sortdist(m->oldx, m->oldy, l->x1(), l->y1()) < sortdist(m->oldx, m->oldy, l->x2(), l->y2()))
						AddHit(m, l, bound, sortdist(m->oldx, m->oldy, l->x1(), l->y1()),
							l->x1(), l->y1(), l->x1(), l->y1(), 0.0f);
					else
						AddHit(m, l, bound, sortdist(m->oldx, m->oldy, l->x2(), l->y2()),
							l->x2(), l->y2(), l->x2(), l->y2(), 0.0f);
			}
		}
		else // need to slide...
		{
			addhit = FALSE; // to determine if we need a hit or not

			olddist = LineDistance(m->oldx,m->oldy,l);
			newdist = LineDistance(m->newx,m->newy,l);

			if ((olddist >= 0.0f) && (olddist <= BUFFER_DIST) && (On_Line(m->oldx,m->oldy,xd,yd,l)))
			{ // we're between the line and the buffer line
				m->numbufferzones++;

				if ((int)newdist < (int)olddist)
				{ // don't let get closer to the wall if already in bufferzone
					addhit = TRUE;
					x1 = m->oldx;
					y1 = m->oldy;
				}
			}
			else
			{
				if (lines_intersect(m, m->oldx,m->oldy,m->newx,m->newy,bufx1,bufy1,bufx2,bufy2))
				{
					x1 = m->inter_x;
					y1 = m->inter_y;
					addhit = TRUE;
				}
			}
			if (addhit)
			{	// don't slide off missiles
				if (l->actor && l->actor->Type() == MISSILE)
				{
					x2 = x1;
					y2 = y1;
				}
				else	if (	(((lyra_abs((FixAngle(m->moveangle+Angle_90)) - (GetFacingAngle(l->x1(),l->y1(),l->x2(),l->y2())))) < 24)
							|| ((lyra_abs((FixAngle(m->moveangle+Angle_90)) - (GetFacingAngle(l->x1(),l->y1(),l->x2(),l->y2())))) > 1000))
						||
								(((lyra_abs((FixAngle(m->moveangle+Angle_90)) - (GetFacingAngle(l->x2(),l->y2(),l->x1(),l->y1())))) < 24)
							|| ((lyra_abs((FixAngle(m->moveangle+Angle_90)) - (GetFacingAngle(l->x2(),l->y2(),l->x1(),l->y1())))) > 1000)))
				{ // don't slide if impact with wall is fairly flat
					x2 = x1;
					y2 = y1;
				}
				else if ((((m->oldx + m->movedist*CosTable[m->moveangle]) - m->oldx)*(l->x2() - l->x1()) + ((m->oldy + m->movedist*SinTable[m->moveangle]) - m->oldy)*(l->y2() - l->y1())) > 0)
				{ // angling the same way...
					x2 = x1 + m->movedist*l->COS;
					y2 = y1 + m->movedist*l->SINE;
				}
				else
				{ // opposite
					x2 = x1 - m->movedist*l->COS;
					y2 = y1 - m->movedist*l->SINE;
				}
				AddHit(m,l,bound,sortdist(m->oldx,m->oldy,x1,y1),x1,y1,x2,y2,(m->movedist - fdist(m->oldx, m->oldy, x1, y1)));
			}
		}
	}

	if ( !bound && !l->actor)
	{
		if (lines_intersect(m, m->oldx,m->oldy,m->newx,m->newy,l->x1(),l->y1(),l->x2(),l->y2()))
		{
			AddHit(m,l,bound,sortdist(m->oldx,m->oldy,m->inter_x,m->inter_y));
			AddLines(l->facingsector, m->ouractor->sector,m,(unsigned char*)seclist);
			//_tprintf("non-bounding sector cross from %d to %d\n",currsec,l->facingsector);
		}
	}
	return bound;
}


void AddActors(int addsec, int currsec, move_params_t *m, unsigned char *seclist)
{
	int i,angle_perp;
	cActor *tactor;
	cMissile *missile;
	float xd,yd;
	linedef *ourline;


	i = level->SectorStart[addsec];
	if (i > -1 )
	{
		while(i < level->NumOList)
		{
			if (level->OList[i].sector != addsec) break;

			tactor = level->OList[i].actor;

			// don't collide with ourself, and don't collide with soulspheres
			// neighbors when we're moving just after a teleport
			if ((tactor == NO_ACTOR) || (tactor == m->ouractor)|| (tactor->flags & ACTOR_SOULSPHERE)||
				(tactor->IsNeighbor() && m->ouractor->IsPlayer() && player->FreeMoves()))
			{
				i++;
				continue;
			}

			 // don't collide with unactivated missiles or the player's melee missiles
			if (tactor->IsMissile())
			{
				missile = (cMissile*)tactor;
				if ((!missile->Activated()) || ((missile->Owner() == m->ouractor) && (missile->Melee())))
				{
					i++;
					continue;
				}
			}


			if (m->ouractor->IsMissile())
			{
				missile = (cMissile*)m->ouractor;
				if ((!missile->Activated()) || ((missile->Owner() == tactor) && (missile->Melee())))
				{
					i++;
					continue;
				}
			}

			// no collide zones are ignored by missiles
			if (!(level->Sectors[addsec]->flags & SECTOR_NOCOLLIDE) || m->ouractor->Type() == MISSILE)
			{
				// make a line from the actor
				ourline = tactor->l;
				// perp angle changes depending on whether or not we're going
				// forwards or m->backwards
				if (m->backwards)
					angle_perp = FixAngle(Angle_360-FixAngle(m->ouractor->angle));
				else
					angle_perp = FixAngle(Angle_360-FixAngle(m->ouractor->angle+Angle_180));

				xd = tactor->halfwit*SinTable[angle_perp];
				yd = tactor->halfwit*CosTable[angle_perp];
				tactor->x1		 = tactor->x + xd;
				tactor->y1		 = tactor->y + yd;
				tactor->x2		 = tactor->x - xd;
				tactor->y2		 = tactor->y - yd;
				ourline->COS	  = m->perpc2;
				ourline->SINE	  = m->perps2;
				ourline->sector  = addsec;
				ourline->facingsector = addsec;
				ourline->length = fdist(tactor->x1,tactor->y1,tactor->x2,tactor->y2);
				InsertL(ourline,currsec,m,seclist);
			}
			i++;
		}
	}
}


// addsec is sector to be added; m->ouractor is moving into addsec from currsec
void AddLines(int addsec, int currsec, move_params_t *m, unsigned char *seclist)
{
	linedef *ourline;
	sector  *moveSector;
	int bound;

	// Don't queue up a sector more than once.
	if ( seclist[addsec] ) return;
	seclist[addsec]=1;

	moveSector	= level->Sectors[addsec];
	if(!moveSector)  return;

	if (!(m->ouractor->flags & ACTOR_SOULSPHERE))
		AddActors(addsec, currsec, m, seclist);

	ourline = moveSector->firstline;
	if(!ourline) return;

	do {
		bound = 0;
		if ( OnRight(ourline,m->ouractor->x,m->ouractor->y))
		{// new stuff starts here
			bound = InsertL(ourline,currsec,m,seclist);
		// if we're very close to the line, and it's non-bounding and leading
		// to a different sector, add the other sector's lines
		if ((ourline->facingsector != ourline->sector) &&
			(LineDistance(m->newx,m->newy,ourline) < BUFFER_DIST) &&
			!bound)
			AddLines(ourline->facingsector,m->ouractor->sector,m,seclist);

		}
	} while(ourline=ourline->nextline);
}


inline void DMemSet(void *dst, int value, int sz)
{
	_asm
	{
		mov	edi,dst
		mov	eax,value
		mov	ecx,sz
		rep	stosd
	}
}

// Transform a point (x,y) to given (co-ord,angle)
void transform_point(float cx,float cy,int angle, float &x, float &y )
{
	// translate
	float nx = x - cx;
	float ny = y - cy;
	
	// rotate
	float cosine = CosTable[angle];
	float sine	 = SinTable[angle];
	x = (nx*cosine) +(ny *sine);
	y = (nx*sine) -(ny *cosine);
}

// find the angle of intersection of a missle and a line (assumes they intersect)
int FindIntersectAngle(cActor *a,linedef *line)
{
	// first transform the line to missile space.
	float rx1,ry1, rx2,ry2;
	rx1 = line->x1();
	ry1 = line->y1();
	transform_point(a->x,a->y,a->angle, rx1,ry1);
	rx2 = line->x2();
	ry2 = line->y2();
	transform_point(a->x,a->y,a->angle, rx2,ry2);

	// determine angle;
	double iangle	= acos((rx2-rx1)/line->length);
// double iangle	= asin((ry1-ry2)/line->length);	 // returns radians from -pi/2 to +pi/2
	// convert to our internal (0..4096) angles

	int internal_angle = int(Angle_180*iangle/3.1415927);
	internal_angle = FixAngle(internal_angle);
	return internal_angle;
}


bool PointInBox (linedef *line1, linedef *line2, float x, float y)
{
	float x1lo,x1hi,y1lo,y1hi;
	float x2lo,x2hi,y2lo,y2hi;

	if(line1->x1()>line1->x2()) 					 // X bound box test
	{
		x1lo=line1->x2(); x1hi=line1->x1();
	}
	else
	{
		x1hi=line1->x2(); x1lo=line1->x1();
	}

	if(line1->y1()>line1->y2()) 					 // Y bound box test
	{
		y1lo=line1->y2(); y1hi=line1->y1();
	}
	else
	{
		y1hi=line1->y2(); y1lo=line1->y1();
	}

	if (((x < x1lo) || (x > x1hi)) && ((y < y1lo) || (y > y1hi)))
		return false;

	if(line2->x1()>line2->x2())					// X bound box test
	{ 					 
		x2lo=line2->x2(); x2hi=line2->x1();
	}
	else
	{
		x2hi=line2->x2(); x2lo=line2->x1();
	}

	if(line2->y1()>line2->y2()) 					 // Y bound box test
	{
		y2lo=line2->y2(); y2hi=line2->y1();
	}
	else
	{
		y2hi=line2->y2(); y2lo=line2->y1();
	}

	if (((x < x2lo) || (x > x2hi)) && ((y < y2lo) || (y > y2hi)))
		return false;

	return true;
}


// if move is false, don't actually move the actor
// if result is non-null, fill in with the result of the move
static void VectorMove(move_params_t *m, move_result_t *result)
{
	linedef *l;
	hit_t *h;
	int i,newsector,slideangle;
	unsigned char seclist[MAX_SECTORS];
	sector *dest;

	// Clean the sectors so we can check whether to queue them again or not.
	memset(seclist,0,MAX_SECTORS);
	m->numhits = 0;
	m->numbufferzones = 0;

	AddLines(m->ouractor->sector, m->ouractor->sector, m, (unsigned char*)seclist);

	// sort the lines if we have more than 1 hit.
	if ( m->numhits > 1)
		 qsort(&(m->hits[0]),m->numhits,sizeof(hit_t),SortHits);

	// we have a sorted list of the lines that our path crossed, in order of distance.
	for ( i=0;i<m->numhits;i++ )
	{
		// get the hit and the linedef.
		h=&(m->hits[i]);
		l=h->l;

		if (m->move_type == MOVE_FIND_TELEPORTAL)
		{
			if (l->TripFlags & TRIP_TELEPORT)
			{
				if (result)
				{
					 result->hit = TELEPORTED;
					 result->l = l;
					 result->dist = h->dist;
				}
				return;
			}
			else // go to next hit
				continue;
		}

		if (m->ouractor->IsPlayer())
		{
			// check for trips
			if (m->move_type == MOVE_TRIP)
			{
				if ((l->flags & TRIP_ACTIVATE) && (PlayerTripLine(l) == TELEPORTED))
					return;
				else // go to next hit
					continue;
			}
			else if (m->move_type == MOVE_NORMAL)
			{
#ifndef AGENT // Mares do not need to teleport when crossing portal trigger lines.  Mares will teleport when GM possessed.
				if ((l->flags & TRIP_CROSS) &&   (PlayerTripLine(l) == TELEPORTED))
				{
					if (result)
					{
						result->hit = TELEPORTED;
						result->dist = h->dist;
						result->l = l;
					}
					return;
				}
#endif //AGENT
			}
		}

		if ( h->bound)
		{
			// if line was already ignored as a result of corner checking, ignore again
			if (l == m->ignorecorner)
				continue;

			// if multiple bounding hits, check to see if actor is in a corner
			// or stuck on the corner of two walls
			if ((m->move_type == MOVE_NORMAL) && (m->numbufferzones > 1) && (!m->corner_tested))
			{
				bool within_box = true;
				linedef *hitline;

				for (int j = 0; j < m->numhits; j++)
				{
					if (i != j)
					{
						hitline = m->hits[j].l;
						if ((hitline->from == l->from) || (hitline->from == l->to)
							|| (hitline->to == l->from) || (hitline->to == l->to))
						{	// if the lines share a vertex (ie, form a corner), determine if actor is
							// inside of corner or outside of corner
							within_box = PointInBox(hitline, l, h->x, h->y);
							if (!within_box)
							{ // if actor is outside of corner, pick which line to ignore
								if (LineDistance(h->x, h->y, hitline) < LineDistance(h->x,h->y,l))
								{
									m->ignorecorner = hitline;
									m->hits[j].bound = 0;
								}
								else
								{
									m->ignorecorner = l;
									h->bound = 0;
								}
							}
							m->corner_tested = true;
							break;
						}
					}
				}
				if ((!within_box) && (m->ignorecorner == l)) // if actor is outside of corner, quit processing this hit
					continue;
			}

			m->newx = h->x; // where we are moving too...
			m->newy = h->y;

			if (result)
			{
				result->dist = h->dist;
				result->l = l;
				if (l->actor)
					result->hit = HIT_ACTOR;
				else
				{
					if (m->ouractor->sector == l->facingsector)
						dest = level->Sectors[l->sector];
					else
						dest = level->Sectors[l->facingsector];

					if (((dest->FloorHt(h->x, h->y) + dest->HtOffset) <
						(level->Sectors[m->ouractor->sector]->FloorHt(h->x, h->y)
						+ level->Sectors[m->ouractor->sector]->HtOffset
						+ (.7*m->ouractor->physht)))
						&& (m->ouractor->Type() == PLAYER))
						result->hit = HIT_JUMPABLE;
					else
						result->hit = HIT_WALL;
				}
			}

			// don't handle missile strikes unless we're actually moving
			if ((m->ouractor->Type() == MISSILE) && (m->move_type == MOVE_NORMAL))
			{ // true value return from Collision means we're done moving the missile
				int collide_type;
				if (l->actor)
					collide_type = HIT_ACTOR;
				else
					collide_type = HIT_WALL;

				// if (l->actor && !(((cMissile*)m->ouractor)->Collision(HIT_ACTOR,l)))
					//return;

				if (!(((cMissile*)m->ouractor)->Collision(collide_type,l)))
				{
						m->ouractor->x = h->x;
						m->ouractor->y = h->y;
						m->ouractor->x += CosTable[FixAngle(m->ouractor->angle + Angle_180)]*32.0f;
						m->ouractor->y += SinTable[FixAngle(m->ouractor->angle + Angle_180)]*32.0f;
				}
				return;

				h->slide_distance = 0.0f; // kill sliding
			}
			//_tprintf("m->newx: %f m->newy: %f hitx: %f hity: %f\n",m->newx,m->newx,h->x,h->y);
			//_tprintf("px: %f py: %f sx: %f sy: %f\n",player->X(),player->Y(),missile->x,missile->y);

			if (	(h->slide_distance != 0.0f)
				&&	(m->move_type == MOVE_NORMAL)
				&& (m->slide_count < MAX_SLIDES)
				&& ((lyra_abs((int)(m->ouractor->velocity))) == ((int)MAXWALK)))
			{	
				// recurse for slide, but only if we're actually moving,
				// and not if we're slowing down
				slideangle = GetFacingAngle(h->slide_x, h->slide_y, m->oldx, m->oldy);

				if ((lyra_abs(slideangle - m->origangle) > Angle_90) &&
					(lyra_abs(slideangle - m->origangle) < Angle_270))
				{
					//_tprintf("Slide-from: %.f, %.f to %.f, %.f\n", m->oldx, m->oldy, h->slide_x, h->slide_y);
					//_tprintf("Stopped: original angle is %i, slideangle is %i\n", m->origangle, slideangle);
					//_tprintf("Hit bound condition is %i, line bound condition is %i\n", h->bound, (h->l->flags & BOUND));
					break; // angle too severe, stop slide
				}

				if (!(PointInSector(m->newx,m->newy,level->Sectors[m->ouractor->sector])))
				{
					newsector = FindSector(m->newx, m->newy, m->ouractor->sector, false);
					if (newsector == DEAD_SECTOR)
						break;
					else
						m->ouractor->sector = newsector;
				}

				//_tprintf("slide # %d, from (%0f,%0f), staring with angle %d\n",m->slide_count,	m->newx,m->newy,m->moveangle);
				m->movedist = h->slide_distance;
				m->ouractor->x = m->oldx	 = h->x;
				m->ouractor->y = m->oldy	 = h->y;
				m->ouractor->sector = FindSector(m->ouractor->x, m->ouractor->y, m->ouractor->sector, false);
				m->newx   = h->slide_x;
				m->newy   = h->slide_y;
				m->moveangle = slideangle;
				//m->moveangle = GetFacingAngle(m->newx,m->newy,m->oldx,m->oldy);
				//_tprintf("slide # %d, to (%0f,%0f), going to angle%d\n",m->slide_count,	m->newx,m->newy,m->moveangle);
				m->slide_count++;
				VectorMove(m,NULL);
				return;
			}
			break;
		}
		else if (m->move_type == MOVE_NORMAL)
		{	// we are moving to a new sector, update actor's sector
			if (l->sector == m->ouractor->sector)
				m->ouractor->sector = l->facingsector;
			else
				m->ouractor->sector = l->sector;
		}
	}


	// in-sector ceiling height restriction
	dest = level->Sectors[m->ouractor->sector];
	if ((dest->CeilHt(m->newx, m->newy) - (dest->FloorHt(m->newx, m->newy)+dest->HtOffset)) <= m->ouractor->physht)
	{ // won't fit
		m->newx = m->oldx;
		m->newy = m->oldy;
		if (result)
		{
			result->dist = 0.0f;
			result->l = NULL;
			result->hit = HIT_CEILING;
		}
	}

	// in-sector floor height restriction
	float mdx = BUFFER_DIST*CosTable[m->moveangle];
	float mdy = BUFFER_DIST*SinTable[m->moveangle];
	if ((PointInSector(m->oldx+mdx, m->oldy+mdy, dest))
		&& ((dest->FloorHt(m->oldx+mdx, m->oldy+mdy)+dest->HtOffset) > (m->ouractor->z  - (m->ouractor->physht*.6))))
	{ // too steep
		m->newx = m->oldx;
		m->newy = m->oldy;
		if (result)
		{
			result->dist = 0.0f;
			result->l = NULL;
			result->hit = HIT_FLOOR;
		}
	}



	if (m->move_type == MOVE_NORMAL)
	{	// move the actor...

		if (!(PointInSector(m->newx,m->newy,level->Sectors[m->ouractor->sector])))
		{
			newsector = FindSector(m->newx, m->newy, m->ouractor->sector, false);
			m->ouractor->x = m->newx;
			m->ouractor->y = m->newy;
			m->ouractor->sector = newsector;
		}
		else // no sector change, go to new spot
		{
				m->ouractor->x = m->newx;
				m->ouractor->y = m->newy;
		}
	}

	return;
}


// if move is true, moves the actor; otherwise it just determines
// the distance that can be moved without actually moving the actor
// if result is non-null, it is filled in with the result of the move
void MoveActor(cActor *movactor, long angle, float distance, int move_type, move_result_t *result)
{
	float ourcos,oursin;
	move_params_t m;

	// fill in result
	if (result)
	{
		result->dist = 0.0f;
		result->hit = NO_HIT;
		result->l = NULL;
	}

	if (distance == 0.0f) return;

	m.ouractor = movactor;
	m.moveangle = m.origangle = FixAngle(angle);

	if (distance < 0.0f)
	{
		distance = (float) fabs(distance);
		m.moveangle = m.origangle = FixAngle(m.moveangle+Angle_180);
		m.backwards = TRUE;
	}
	else
		m.backwards = FALSE;

	ourcos  = CosTable[m.moveangle];
	oursin  = SinTable[m.moveangle];

	int angle_perp2 = FixAngle(Angle_360-FixAngle(movactor->angle+Angle_180));
	m.perps2=CosTable[angle_perp2];
	m.perpc2=SinTable[angle_perp2];


	m.movedist = distance;
	m.oldx	= m.ouractor->x;
	m.oldy	= m.ouractor->y;

	m.newx	= m.oldx + (ourcos*m.movedist);
	m.newy	= m.oldy + (oursin*m.movedist);

	//_tprintf("moving dist = %f, x = %f, y = %f, m.newx = %f, m.newy = %f\n",m.movedist,m.oldx,m.oldy,m.newx,m.newy);
	m.slide_count = 0;
	m.ignorecorner = NULL;
	m.corner_tested = false;
	m.move_type = move_type;
	VectorMove(&m,result);

	return;
}


static int PlayerTripLine(linedef *aLine)
{
	cItem *item;
	int i;
	static bool got_item = false;


 /*	if (aLine->TripFlags & TRIP_FLIP)
	{
		int temp = aLine->bitmap;
		aLine->bitmap = aLine->bitmap2;
		aLine->bitmap2= temp;
		temp			  = aLine->topbm;
		aLine->topbm  = aLine->topbm2;
		aLine->topbm2 = temp;

	}*/


	if ( aLine->TripFlags & TRIP_GOALPOSTING )
	{
		goals->Activate(aLine->trip1, aLine->trip2);
		return (0);
	}

#ifndef PMARE
	if ( aLine->TripFlags & TRIP_QUESTBUILDER )
	{
		quests->Activate();
		return (0);
	}
#endif



	if ((aLine->TripFlags & TRIP_SHOWMESSAGE) && (player->velocity > 0))
	{ // only display on forward movement over line
		TCHAR trip_message[DEFAULT_MESSAGE_SIZE];

		if (show_training_messages)
		{ // messages only shown in training

			switch (aLine->trip1)
			{
				case TRAIN_JUMP:
					LoadString (hInstance, IDS_TRAIN_JUMP, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_FOLLOW_TORCHES:
					LoadString (hInstance, IDS_TRAIN_FOLLOW_TORCHES, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_TELEPORTAL:
					LoadString (hInstance, IDS_TRAIN_TELEPORTAL, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_CHAT_START:
					LoadString (hInstance, IDS_TRAIN_CHAT_START, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_TALK:
					LoadString (hInstance, IDS_TRAIN_TALK, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SHOUT:
					LoadString (hInstance, IDS_TRAIN_SHOUT, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_WHISPER:
					LoadString (hInstance, IDS_TRAIN_WHISPER, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_EMOTE:
					LoadString (hInstance, IDS_TRAIN_EMOTE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BOW:
					LoadString (hInstance, IDS_TRAIN_BOW, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_CHAT_FINISH:
					LoadString (hInstance, IDS_TRAIN_CHAT_FINISH, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_START:
					// give player starting skills
					if (options.welcome_ai)
					{
						player->SetXP(0, true);
						player->SetSkill(Arts::KNOW, 50, SET_ABSOLUTE, player->ID());
						player->SetSkill(Arts::MEDITATION, 50, SET_ABSOLUTE, player->ID());
						player->SetSkill(Arts::TRAIL, 50, SET_ABSOLUTE, player->ID());
						player->SetSkill(Arts::JOIN_PARTY, 50, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::DREAMSOUL, 10, player->ID());
						player->SetCurrStat(Stats::DREAMSOUL, 10, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::WILLPOWER, 40, player->ID());
						player->SetCurrStat(Stats::WILLPOWER, 40, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::INSIGHT, 40, player->ID());
						player->SetCurrStat(Stats::INSIGHT, 40, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::RESILIENCE, 40, player->ID());
						player->SetCurrStat(Stats::RESILIENCE, 40, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::LUCIDITY, 40, player->ID());
						player->SetCurrStat(Stats::LUCIDITY, 40, SET_ABSOLUTE, player->ID());
					}

					LoadString (hInstance, IDS_TRAIN_SKILLS, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_SECOND:
					LoadString (hInstance, IDS_TRAIN_SKILLS_SECOND, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_THIRD:
					LoadString (hInstance, IDS_TRAIN_SKILLS_THIRD, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_KNOW:
					LoadString (hInstance, IDS_TRAIN_KNOW, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_GIVE:
					LoadString (hInstance, IDS_TRAIN_SKILLS_GIVE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_SENSE:
					LoadString (hInstance, IDS_TRAIN_SENSE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_MAJORMINOR:
					LoadString (hInstance, IDS_TRAIN_SKILLS_MAJORMINOR, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_POWER:
					LoadString (hInstance, IDS_TRAIN_SKILLS_POWER, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_ORBIT:
					LoadString (hInstance, IDS_TRAIN_SKILLS_ORBIT, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_LOCATE_AVATAR:
					LoadString (hInstance, IDS_TRAIN_SKILLS_LOCATE_AVATAR, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_MEDITATE:
					LoadString (hInstance, IDS_TRAIN_MEDITATE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_TRAIL:
					LoadString (hInstance, IDS_TRAIN_TRAIL, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_DRAIN_NIGHTMARE:
					LoadString (hInstance, IDS_TRAIN_DRAIN_NIGHTMARE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_RANDOM:
					LoadString (hInstance, IDS_TRAIN_RANDOM, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_FOCUS:
					LoadString (hInstance, IDS_TRAIN_FOCUS, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_JOINPARTY:
					LoadString (hInstance, IDS_TRAIN_JOINPARTY, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SKILLS_FINISH:
					if (options.welcome_ai)
					{
						player->SetSkill(Arts::KNOW, 0, SET_ABSOLUTE, player->ID());
						player->SetSkill(Arts::MEDITATION, 0, SET_ABSOLUTE, player->ID());
						player->SetSkill(Arts::TRAIL, 0, SET_ABSOLUTE, player->ID());
						player->SetSkill(Arts::JOIN_PARTY, 0, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::WILLPOWER, 0, player->ID());
						player->SetCurrStat(Stats::WILLPOWER, 0, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::INSIGHT, 0, player->ID());
						player->SetCurrStat(Stats::INSIGHT, 0, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::RESILIENCE, 0, player->ID());
						player->SetCurrStat(Stats::RESILIENCE, 0, SET_ABSOLUTE, player->ID());
						player->SetMaxStat(Stats::LUCIDITY, 0, player->ID());
						player->SetCurrStat(Stats::LUCIDITY, 0, SET_ABSOLUTE, player->ID());
					}

					LoadString (hInstance, IDS_TRAIN_SKILLS_FINISH, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_INVENTORY_START:
				{
					LoadString (hInstance, IDS_TRAIN_INVENTORY_START, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);

					if (options.welcome_ai)
					{
						// make a test item
						if (!got_item)
						{
							got_item = true;

							LmItem info;
							LmItemHdr header;
							lyra_item_missile_t missile = { LyraItem::MISSILE_FUNCTION, -4, 0, 1, LyraBitmap::FIREBALL_MISSILE}; // chakram

							header.Init(0, 0);
							header.SetFlags(0); //	LyraItem::FLAG_SENDSTATE;
							header.SetGraphic(LyraBitmap::TALISMAN3);
							header.SetColor1(1); header.SetColor2(2);
							header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::MISSILE_FUNCTION),0,0));

							LoadString(hInstance, IDS_TRAIN_TALIS, temp_message, sizeof(temp_message));
							info.Init(header, temp_message, 0, 0, 0);

							info.SetStateField(0, &missile, sizeof(missile));
							info.SetCharges(4);
							cItem * item = CreateItem(player->x, player->y, player->angle, info, 0, false);
						}

						player->SetSkill(Arts::GATEKEEPER, 50, SET_ABSOLUTE, player->ID());

						LoadString (hInstance, IDS_TRAIN_INVENTORY_MID, trip_message, sizeof(trip_message));
						display->DisplayMessage (trip_message);
					}
				}
				break;

				case TRAIN_INVENTORY_FINISH:
					LoadString (hInstance, IDS_TRAIN_INVENTORY_FINISH, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_INVENTORY_EXIT:
					if (options.welcome_ai)
					{
						for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
						{
							item->SetStatus(ITEM_DESTROYING);
							item->SetTerminate();
						}
						actors->IterateItems(DONE);
						player->SetSkill(Arts::GATEKEEPER, 0, SET_ABSOLUTE, player->ID());
					}

					LoadString (hInstance, IDS_TRAIN_INVENTORY_EXIT, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SANCTUARY_START:
					LoadString (hInstance, IDS_TRAIN_SANCTUARY_START, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_SANCTUARY_FINISH:
					LoadString (hInstance, IDS_TRAIN_SANCTUARY_FINISH, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_START:
					LoadString (hInstance, IDS_TRAIN_HOUSES, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_CITYMAP:
					LoadString (hInstance, IDS_TRAIN_HOUSES_CITYMAP, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_CONSIDER:
					LoadString (hInstance, IDS_TRAIN_HOUSES_CONSIDER, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_FINDTEACHER:
					LoadString (hInstance, IDS_TRAIN_HOUSES_FINDTEACHER, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BELIEF_MOON:
					LoadString (hInstance, IDS_TRAIN_BELIEF_MOON, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BELIEF_ECLIPSE:
					LoadString (hInstance, IDS_TRAIN_BELIEF_ECLIPSE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BELIEF_SHADOW:
					LoadString (hInstance, IDS_TRAIN_BELIEF_SHADOW, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BELIEF_COVENANT:
					LoadString (hInstance, IDS_TRAIN_BELIEF_COVENANT, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BELIEF_RADIANCE:
					LoadString (hInstance, IDS_TRAIN_BELIEF_RADIANCE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BELIEF_CALENTURE:
					LoadString (hInstance, IDS_TRAIN_BELIEF_CALENTURE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BELIEF_ENTRANCED:
					LoadString (hInstance, IDS_TRAIN_BELIEF_ENTRANCED, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_BELIEF_LIGHT:
					LoadString (hInstance, IDS_TRAIN_BELIEF_LIGHT, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_QUEST_BUILDER1:
					LoadString (hInstance, IDS_QUEST_BUILDER1, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_QUEST_BUILDER2:
					LoadString (hInstance, IDS_QUEST_BUILDER2, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				default:
					break;
			}
		}

		// message-trips that always show up

		switch (aLine->trip1)
		{
				case LIBRARY_ENTRANCE:
					LoadString (hInstance, IDS_LIBRARY_ENTRANCE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_MOON:
					LoadString (hInstance, IDS_TRAIN_HOUSES_MOON, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_ECLIPSE:
					LoadString (hInstance, IDS_TRAIN_HOUSES_ECLIPSE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_SHADOW:
					LoadString (hInstance, IDS_TRAIN_HOUSES_SHADOW, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_COVENANT:
					LoadString (hInstance, IDS_TRAIN_HOUSES_COVENANT, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_RADIANCE:
					LoadString (hInstance, IDS_TRAIN_HOUSES_RADIANCE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_CALENTURE:
					LoadString (hInstance, IDS_TRAIN_HOUSES_CALENTURE, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_ENTRANCED:
					LoadString (hInstance, IDS_TRAIN_HOUSES_ENTRANCED, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_LIGHT:
					LoadString (hInstance, IDS_TRAIN_HOUSES_LIGHT, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				case TRAIN_HOUSES_TRAINAGAIN:
					LoadString (hInstance, IDS_TRAIN_HOUSES_TRAINAGAIN, trip_message, sizeof(trip_message));
					display->DisplayMessage (trip_message);
				break;

				default:
					break;
		}
		return (0);
	}

	if ( aLine->TripFlags & TRIP_TELEPORT )
	{	// make sure this teleportal is not locked
		// since we can only do one item iteration at a time, we
		// make a list of all the amulet keys we have, and then
		// check them against the wards
		bool has_proper_amulet;
		lyra_item_ward_t ward;
		lyra_item_amulet_t amulet;
		lyra_id_t amulet_keys[Lyra::INVENTORY_MAX];
		cItem *amulets[Lyra::INVENTORY_MAX];
		int num_amulets = 0;
		const void* state;

		for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			if ((item->BitmapID() == LyraBitmap::AMULET) && (item->Status() == ITEM_OWNED))
			{
				amulets[num_amulets] = item;
				state = item->Lmitem().StateField(0);
				memcpy(&amulet, state, sizeof(amulet));
				amulet_keys[num_amulets] = amulet.player_id;
				amulets[num_amulets] = item;
				num_amulets++;
				//_tprintf("encountered amulet with player id = %d\n",amulet.player_id);
			}
		actors->IterateItems(DONE);

		for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			if ((item->ItemFunction(0) == LyraItem::WARD_FUNCTION) && (aLine == ((linedef*)item->Extra())))
			{
				has_proper_amulet = false;
				state = item->Lmitem().StateField(0);
				memcpy(&ward, state, sizeof(ward));
				//_tprintf("encountered ward with player id = %d\n",ward.player_id());
				for (i=0; i<num_amulets; i++)
					if (amulet_keys[i] == ward.player_id())
					{
						has_proper_amulet = true;
						// uncomment to give amulets charges
						//if (actors->ValidItem(amulets[i]))
						// amulets[i]->DrainCharge();
					}
				if (!has_proper_amulet && player->flags & ACTOR_BLENDED)
				{ // player is blended, so kill the blending but pass the ward
					has_proper_amulet = true;
					player->RemoveTimedEffect(LyraEffect::PLAYER_BLENDED);
				}
				if (!has_proper_amulet)
				{
					LoadString (hInstance, IDS_TELEPORTAL_WARDED, disp_message, 256);
					display->DisplayMessage (disp_message);
					actors->IterateItems(DONE);
					return 0;
				}
			}
		actors->IterateItems(DONE);

		int guild_id = GetTripGuild(aLine->flags);
		if (options.network && !CanPassPortal(aLine->trip3, guild_id))
			return 0;


		player->Teleport((float)aLine->trip1, (float)aLine->trip2,	aLine->trip4);
		return TELEPORTED;
	}

	if (aLine->TripFlags & TRIP_LEVELCHANGE)
	{

		int guild_id = GetTripGuild(aLine->flags);
		unsigned char lock = (aLine->trip4>>8);
		unsigned char level_id = (aLine->trip4 & 0xff);
		if (options.network && !CanPassPortal(lock, guild_id))
			return 0;

		player->Teleport(aLine->trip1, aLine->trip2, aLine->trip3, level_id);
		player->SetXHeight();

		return TELEPORTED;
	}


	/*
	if ( aLine->TripFlags & TRIP_CHANGEFLOOR )
	{
		tag = aLine->trip1;
		for (i=0; i<Tags[tag].num_sectors; i++)
			AddEvent(TRIP_CHANGEFLOOR, Tags[tag].sector_list[i], 0, aLine->trip2, aLine->trip3, 0.0f, 0.0f);
	}

	if ( aLine->TripFlags & TRIP_CHANGECEILING )
	{
		tag = aLine->trip1;
		for (i=0; i<Tags[tag].num_sectors; i++)
			AddEvent(TRIP_CHANGECEILING, Tags[tag].sector_list[i], 0, aLine->trip2, aLine->trip3, 0.0f, 0.0f);
	}
	*/

	return(0);
}


bool CanPassPortal(int lock, int guild_id, bool rendering)
{ // this code also used by buildview to decide whether or not to animate portals

//#ifndef GAME_LYR // bypass ALL locks
#if !defined(GAME_LYR) || defined(LIVE_DEBUG) // bypass ALL locks
#ifndef PMARE // pmares bypass sphere locks
	if (lock >= 10 && lock <= 19) // locking is by sphere
	{
		lock -= 10;
		if (player->Sphere() < lock)
		{
			if (!rendering)
			{
				LoadString (hInstance, IDS_TEL_SPHERE_TOO_LOW, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message);
			}
			return false;
		}
		return true;
	}
#endif

#ifdef PMARE // locks for pmares: 30 = bog, 31 = ago, etc.
	if (lock >=30 && lock <= 39)
	{
		lock -= 27;
		if (player->AvatarType() < lock)
		{
			if (!rendering)
			{
				LoadString (hInstance, IDS_PMARE_TYPE_TOO_LOW, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message);
			}
			return false;
		}
		return true;
	}
#endif


	if (lock >= 40 && lock <= 49) // locking is by specific art
	{
		int art_id;

		switch (lock)
		{// which art lock is active
			case 40:
				art_id = Arts::GATEKEEPER;
				break;
			case 41:
				art_id = Arts::DREAMSEER;
				break;
			case 42:
				art_id = Arts::SOULMASTER;
				break;
			case 43:
				art_id = Arts::FATESENDER;
				break;
			case 44:
				art_id = Arts::QUEST;
				break;
			case 45:
				art_id = Arts::TRAIN;
				break;
			case 46:
				art_id = Arts::TRAIN_SELF;
				break;
			case 47:
				art_id = Arts::DREAMSMITH_MARK;
				break;
			case 48:
				art_id = Arts::WORDSMITH_MARK;
				break;
			case 49:
				art_id = Arts::NP_SYMBOL;
				break;
		}
		if (player->Skill(art_id) < 1)
		{
			if (!rendering)
			{
				LoadString (hInstance, IDS_TEL_MUST_KNOW_ART, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, arts->Descrip(art_id));
				display->DisplayMessage(message);
			}
			return false;
		}
		return true;
	}



	switch (lock)	// locking is by rank
	{// check that we have the appropriate rank
		case Guild::INITIATE:
			if (!player->IsInitiate(guild_id) &&
				!player->IsKnight(guild_id) &&
				!player->IsRuler(guild_id))
			{
				if (!rendering)
				{
					LoadString (hInstance, IDS_TEL_MUST_BE_INITIATE, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, GuildName(guild_id));
					display->DisplayMessage(message);
				}
				return false;
			}
			return true;
		case Guild::KNIGHT:
			if (!player->IsRuler(guild_id) &&
				!player->IsKnight(guild_id))
			{
				if (!rendering)
				{
					LoadString (hInstance, IDS_TEL_MUST_BE_KNIGHT, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, GuildName(guild_id));
					display->DisplayMessage(message);
				}
				return false;
			}
			return true;
		case Guild::RULER:
			if (!player->IsRuler(guild_id))
			{
				if (!rendering)
				{
					LoadString (hInstance, IDS_TEL_MUST_BE_RULER, disp_message, sizeof(disp_message));
					_stprintf(message, disp_message, GuildName(guild_id));
					display->DisplayMessage(message);
				}
				return false;
			}
			return true;
		case NO_INITIATES:
			if (player->IsInitiate(Guild::NO_GUILD) && !player->IsKnight(guild_id) &&
				!player->IsRuler(guild_id))
			{
				if (!rendering)
				{
					LoadString (hInstance, IDS_NO_INITIATES, disp_message, sizeof(disp_message));
					display->DisplayMessage(disp_message);
				}
				return false;
			}
			return true;
		default:
		case Guild::NO_RANK:
			return true;
	}
#endif
	return true;
}

// returns the guild id specified by the flags, or NO_GUILD
// if none present
int GetTripGuild(int flags)
{
	if (flags & LINE_SABLE_MOON)
		return Guild::MOON;
	else if (flags & LINE_ECLIPSE)
		return Guild::ECLIPSE;
	else if (flags & LINE_SHADOW)
		return Guild::SHADOW;
	else if (flags & LINE_COVENT)
		return Guild::COVENANT;
	else if (flags & LINE_RADIANCE)
		return Guild::RADIANCE;
	else if (flags & LINE_CALENTURE)
		return Guild::CALENTURE;
	else if (flags & LINE_ENTRANCED)
		return Guild::ENTRANCED;
	else if (flags & LINE_LIGHT)
		return Guild::LIGHT;
	else
		return Guild::NO_GUILD;
}

// tries to find a teleportal in front of the actor;
// returns NULL if none found, or the linedef if found
linedef* FindTeleportal(cActor *actor)
{
	move_result_t result;

	MoveActor(actor, actor->angle, MANUAL_TRIP_DISTANCE,
				MOVE_FIND_TELEPORTAL, &result);

	return result.l;

}