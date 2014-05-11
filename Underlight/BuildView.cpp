// Copyright Lyra LLC, 1996. All rights reserved.
extern int opt_on;

#define STRICT

//#define BVBS_DEBUG

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <winsock2.h>

#include "resource.h"
#include "cActorList.h"
#include "4dx.h"
#include "Move.h"
#include "cLevel.h"
#include "Realm.h"
#include "cPlayer.h"
#include "cNeighbor.h"
#include "cEffects.h"
#include "RenderView.h"
#include "options.h"

//#ifdef _DEBUG
#include "cChat.h"
extern cChat *display;
//#endif

//externs----------------------------------------------
extern cActorList *actors;
extern cPlayer *player;
extern cLevel *level;
extern timing_t *timing;
extern options_t options;
extern cEffects *effects;
extern HINSTANCE hInstance;


// globals ----------------------------------------------
/*
long	  View_HalfHeight ;
long	  View_Height		;
float View_Heightfloat;
static float View_HalfHeightfloat;
*/

float  *TanTable;
float  *CosTable;
float  *SinTable;

//statics
static float View_HalfWidthfloat;
static float viewing_distance;
static float bvCos, bvSin;
static float ViewFactor;


static float perps2;
static float perpc2;
static int View_Width;
static int View_WidthMax;

static float	 bvxPlayer;
static float	 bvyPlayer;

static int wall_animation_ticks = 0;
static int sector_animation_ticks = 0;

static linedef *linelist[4096];
static int	PendingLines;
static int	linebase;
static int	LineIndex;

static unsigned char  vertlist[MAX_VERTICES];
static unsigned char  seclist[MAX_SECTORS];

#define ROTATED	1

// Function Prototypes------------------------------

void RotatePoint(float x,float y,float &x2,float &y2);
void SetLineAngles(struct linedef *aLine);

inline float GetDist(float x, float y)
{
	return (x*x)+(y*y);
}


int PointOnRightN(linedef *aLine)
{
	float x2,y2,temp;

	x2 = aLine->rx2;
	y2 = aLine->ry2;

	temp = ((aLine->rx1-x2)*(-y2))-((aLine->ry1-y2)*(-x2));

	if ( temp < 0.0f )
		 return 1;	// not on right!
	return 0;
}

int PointOnRight(linedef *aLine, float Px, float Py)
{
	// these are all normalized and rotated, in respect to us.
	float x1,y1,x2,y2,temp;

	x1 = aLine->rx1;
	y1 = aLine->ry1;
	x2 = aLine->rx2;
	y2 = aLine->ry2;

	temp = ((x1-x2)*(Py-y2))-((y1-y2)*(Px-x2));

	if ( temp < 0.0f ) return 1;	// not on right!
		return 0;
}


void lins(int j,linedef *ourline)
{
	if ( j < LineIndex)
	{
		memmove(linelist + j + 1, linelist + j,sizeof(linedef *) * (LineIndex - j));
	}
	linelist[j]=ourline;
	LineIndex ++;
	PendingLines++;
}

void ldel(int j)
{
	if ( j < LineIndex )
		memmove(linelist + j, linelist + j+1,sizeof(linedef *) * (LineIndex - j));
	LineIndex --;
	PendingLines--;
}


#define NO_OVERLAP	 0
#define LINE_CLOSER	-1
#define LINE_FARTHER  1
int lines_compared,overlapped_lines,lines_inserted,actors_inserted; // temp!

static int actor_order;
// This is used to set the order of actors that overlap lines. Most of the time, the actor is
// deemed to be closer than the line. However under certain conditions, this assumption can lead
// to circular sort orders i.e A < C , C < B, B < A. and the result is infinite recursion in
// Insert_Line. We thus switch the assumption when we detect a possible infinite recursion, which
// should rectify the problem. If we still recurse an error is generated.

static int DetermineSortOrder(linedef *aLine, linedef *bLine)
{

	int dir1,dir2;
	float ax1,ax2,ay1,ay2,bx1,bx2,by1,by2;

	ax1 = aLine->rx1;
	ay1 = aLine->ry1;
	ax2 = aLine->rx2;
	ay2 = aLine->ry2;
	bx1 = bLine->rx1;
	by1 = bLine->ry1;
	bx2 = bLine->rx2;
	by2 = bLine->ry2;

	int ourside=PointOnRightN(aLine);

	if ((bx1 == ax1 && by1 == ay1) || ( bx2 == ax1 && by2 == ay1 ))
	{
		if (PointOnRight(bLine, ax2, ay2) == ourside)
			return(LINE_CLOSER);
		else
			return(LINE_FARTHER);
}

	if (( bx1 == ax2 && by1 == ay2)||( bx2 == ax2 && by2 == ay2 ))
	{
		if (PointOnRight(bLine, ax1, ay1) == ourside)
			return(LINE_CLOSER);
		else
			return(LINE_FARTHER);
	}
	dir1 = PointOnRight(bLine,ax1,ay1);
	dir2 = PointOnRight(bLine,ax2,ay2);
	// if both points of insertee are on the right of arrayed line, then
	// we need to insert it.

	// Both points are closer.
	if ( dir1==ourside && dir2==ourside )
		return(LINE_CLOSER);

	// both points are farther.
	if ( dir1 != ourside && dir2 != ourside )
		return(LINE_FARTHER);

	dir1 = PointOnRight(aLine,bx1,by1);
	dir2 = PointOnRight(aLine,bx2,by2);
	// both sides of other line are closer, so this line is farther
	if ( dir1==ourside && dir2==ourside )
		return(LINE_FARTHER);

	// both sides of other line are farther, so this line is farther
	if ( dir1 != ourside && dir2 != ourside )
		return(LINE_CLOSER);
	// lines cross -- either an object or it's a BAD map.
	if ( aLine->actor)
		return actor_order;//return LINE_CLOSER;

	if ( bLine->actor)
		return -actor_order;//return LINE_FARTHER;

	// lines intersect, it must be bogus....
	return(NO_OVERLAP); // do not overlap.
}


inline int LinesOverlap(linedef *aLine, linedef *bLine)
{
	lines_compared++;
	if (aLine->BegCol < bLine->EndCol &&	aLine->EndCol >  bLine->BegCol )
	{
		overlapped_lines++;
		return DetermineSortOrder(aLine,bLine);
	}
	return(NO_OVERLAP); // do not overlap.
}


void Insert_Line(linedef *aLine, int depth = 0)
{
	int last			= LineIndex;
	float dist		= aLine->dist;
	int found		= 0;
	linedef *bLine;
	int cued			= 0;
	int retval;
	linedef *cuedsort[256];

	if ( depth > 20 )
	{
		double x1 = aLine->x1();
		double y1 = aLine->y1();
		
		double x2 = aLine->x2();
		double y2 = aLine->y2();

#ifdef _DEBUG
		LoadString(hInstance, IDS_UNRESOLVED_LINE_SORT, message, sizeof(message));
		_stprintf(disp_message, message, x1,y1,x2,y2);
		display->DisplayMessage(disp_message);
#endif
		return;
	}

	if (depth < 10)
		actor_order = LINE_CLOSER;
	else
		actor_order = LINE_FARTHER;

	int j = linebase;
	for(j = linebase; j < LineIndex;j++)
	{
		bLine = linelist[j];
		if ( dist < bLine->dist  )
		{
			last = j;
		}
		retval = LinesOverlap(aLine,bLine);
		if(retval==LINE_CLOSER)
		{
			// aLine is closer than bLine, so insert it.
			// lets save where it intersected.
			lins(j,aLine);
			j+=2; 		 // skip past aLine AND bLine
			found = 1;
			break;
		}
		else if ( retval == LINE_FARTHER )
		{
			last=j+1;	 // put it after this line.
		}
	}

	// no overlapping lines, so we best guess it.
	if ( !found )
	{
		// lets best guess it, at last dist.
		lins(last,aLine);
		return;
		//j=last;
	}

	// now lets see if anything after it is out of order.
	for(;j<LineIndex;j++)
	{
		bLine = linelist[j];
		if(LinesOverlap(aLine,bLine)==LINE_FARTHER)
		{
			cuedsort[cued++] = bLine;
			ldel(j);
			j--;
		}
	}
	for ( j=0;j<cued;j++)
		Insert_Line(cuedsort[j],depth+1);
}


// transforms a vertex into view-coords)
vertex *TransformVertex(int vert)
{
	vertex *v_ptr = &level->Vertices[vert];
	if (!vertlist[vert]) // if not already transformed
	{
		vertlist[vert]=ROTATED; // mark as transformed

		// translate
		float nx = v_ptr->n_x = v_ptr->x-bvxPlayer;
		float ny = v_ptr->n_y = v_ptr->y-bvyPlayer;

		// rotate vertex
		v_ptr->r_x = (nx*bvCos)+(ny*bvSin);
		v_ptr->r_y = (nx*bvSin)-(ny*bvCos);
	}
	return v_ptr;
}


// calculate the screen column for the given transformed point
inline int CalculateScreenColumn(float rx, float ry)
{
 return float2int(View_HalfWidthfloat - ((ry * viewing_distance) / rx));
}


// change the bitmap number to animate wall textures
static void AnimateLineBitmap(linedef *line)
{
	static int	animation_ticks = 0;

	if (wall_animation_ticks >= ANIMATION_TICKS)
	{
		animation_ticks++;
		wall_animation_ticks = 0;
	}

	int animations = line->animation_end - line->animation_start;
	if (animations > 0)
		line->bitmap	= line->animation_start + (animation_ticks % animations);
}


static void insert_sector_actors(int sector_num)
{

	int bitmap,BegCol,EndCol;
	float x1,x2,y1,y2,rx,ry,xd,yd,bmh2;
	float nx,ny,dist;

	cActor	 *a;
	linedef	*actor_line;

	for (unsigned i = level->SectorStart[sector_num] ; i < (unsigned)level->NumOList ; i++)
	{
		if (level->OList[i].sector != sector_num)
			break;

		a = level->OList[i].actor;

		if (!a)
			continue;

		if (a->IsNeighbor())
			 ((cNeighbor *)a)->SetVisible(false);

		if (!a->Render())
			continue;

		actor_line = a->l;

		// normalize center point.
		nx 	= a->x-bvxPlayer;
		ny 	= a->y-bvyPlayer;

		// rotate the normalized X
		rx 	= (nx*bvCos)+(ny*bvSin);

		// leave if object is behind us.
		if ( rx < 0.0 )  continue;

		// rotate the normalized Y
		ry 	= (nx*bvSin)-(ny*bvCos);

		// get distance to item.
		dist	= GetDist(rx,ry);

		if ( dist < MIN_ITEM_DISTANCE) continue;

		bitmap = a->CurrBitmap(player->angle);

		// divide by 2 (to get half)
		bmh2		= (float)(effects->EffectBitmap(bitmap)->h/2);

		x1 =rx;
		y1 =ry+bmh2;
		x2 =rx;
		y2 =ry-bmh2;

		if( y1 > x1 && y2 > x2 || y1 < -x1 && y2 < -x2) continue;

		BegCol = 0;
		EndCol = View_Width;

		if (x1 && y1 < x1 && y1 > -x1)
		{
			BegCol = CalculateScreenColumn(x1,y1);
			if ( BegCol > View_Width ) continue;
			if ( BegCol < 0 ) BegCol = 0;
		}

		if (x2 && y2 < x2 && y2 > -x2)
		{
			EndCol = CalculateScreenColumn(x2,y2);
			if ( EndCol < 0 ) continue;
			if ( EndCol > View_Width) EndCol = View_Width;
		}

		actor_line->bitmap  = bitmap;
		actor_line->dist	  = dist;
		actor_line->length  = effects->EffectBitmap(bitmap)->h;
		actor_line->rx1	  = x1;
		actor_line->ry1	  = y1;
		actor_line->rx2	  = x2;
		actor_line->ry2	  = y2;
		actor_line->sector  = a->sector;
		actor_line->BegCol = BegCol;
		actor_line->EndCol = EndCol;

		xd = a->x+a->halfwit;
		yd = a->y;
		RotatePoint(a->x,a->y,xd,yd);

		xd = a->x-a->halfwit;
		yd = a->y;
		RotatePoint(a->x,a->y,xd,yd);

		Insert_Line(actor_line);
		actors_inserted++;
	}
}


static void InsertSector(int addsector)
{
	int BegCol, EndCol;
	float x1,x2,y1,y2;

	sector	*sec;
	vertex	*pvFromVert,*pvToVert;
	linedef	*line;

	int  sector_visible=FALSE;

	if(seclist[addsector])
		return;
	seclist[addsector]=1;

	sec = level->Sectors[addsector];
	if(!sec || !sec->firstline)
		return;

	insert_sector_actors(addsector);

	line = sec->firstline;
	if(line) do
	{
		if (line->flags & LINE_ANIMATED)
		{

			if ((line->TripFlags & TRIP_TELEPORT) && options.network)
			{
				int guild_id = GetTripGuild(line->flags);
				if (CanPassPortal(line->trip3, guild_id, true))
					AnimateLineBitmap(line);
			}
			else if ((line->TripFlags & TRIP_LEVELCHANGE) && options.network)
			{
				int guild_id = GetTripGuild(line->flags);
				unsigned char lock = (line->trip4>>8);
				if (CanPassPortal(lock, guild_id, true))
					AnimateLineBitmap(line);
			}
			else
				AnimateLineBitmap(line);
		}

		pvFromVert = TransformVertex(line->from);
		line->rx1 = x1 = pvFromVert->r_x;
		line->ry1 = y1 = pvFromVert->r_y;

		pvToVert = TransformVertex(line->to);
		x2 = line->rx2 = pvToVert->r_x;
		y2 = line->ry2 = pvToVert->r_y;

		// leave if wall is behind us.
		if( pvFromVert->n_x*pvToVert->n_y < pvToVert->n_x*pvFromVert->n_y)
			continue;

		if (fabs(x1*y2-x2*y1) < 0.1)	// line is 'side on' to player
		{
			InsertSector(line->facingsector); // So insert the facing sector
			continue;
		}

		if( x1 < 0.0  && x2 < 0.0)
			continue;  // <= 0??? would remove out later if??
		// check if wall 'off to left' or 'off to right'
		if( y1 > x1 && y2 > x2 || y1 < -x1 && y2 < -x2)
			continue;

		BegCol = 0;
		EndCol = View_Width;

		// get out if it ends before, or begins after screen.
		if (x1 && y1 < x1)
		{
			BegCol =  CalculateScreenColumn(x1,y1);
			if (BegCol > View_WidthMax) continue;	 // This wall starts after our view
			if (BegCol < 0) BegCol = 0;				 // Start at zero if line begins to the left
		}

		if (x2 && y2 > -x2 )
		{
			EndCol = CalculateScreenColumn(x2,y2);
			if (EndCol > View_Width) EndCol = View_Width;	 // Stop it if line extends too far to the right
		}

		if (EndCol < BegCol )
		 continue;
		line->dist	 = GetDist(x1,y1);
		line->BegCol = BegCol;
		line->EndCol = EndCol;
		line->rx1	 = x1;
		line->rx2	 = x2;
		line->ry1	 = y1;
		line->ry2	 = y2;

		Insert_Line(line);
		lines_inserted++;

		sector_visible = TRUE;

	} while(line = line->nextline);

	if (sector_visible)
	{
		if (sec->floor_surface)
			sec->floor_surface->setup_for_render();
		if (sec->ceiling_surface)
			sec->ceiling_surface->setup_for_render();
	}
}


void InsertActors(int player_sector)
{
	int bitmap,BegCol,EndCol;
	float x1,x2,y1,y2,rx,ry,xd,yd,bmh2;
	float nx,ny,dist;

	cActor	 *ouractor;
	linedef	*ourline;

	int players_room = level->Sectors[player_sector]->room;

	for (ouractor=actors->IterateActors(INIT); ouractor != NO_ACTOR;
	ouractor=actors->IterateActors(NEXT))
	{

		if (level->Sectors[ouractor->sector]->room != players_room)
			continue;

		if (ouractor->IsNeighbor())
			 ((cNeighbor *)ouractor)->SetVisible(false);

		if (!ouractor->Render())
		{
			continue;
		}

		ourline = ouractor->l;

		// normalize center point.
		nx 	= ouractor->x-bvxPlayer;
		ny 	= ouractor->y-bvyPlayer;

		// rotate the normalized X
		rx 	= (nx*bvCos)+(ny*bvSin);

		// leave if object is behind us.
		if ( rx < 0.0 )  continue;

		// rotate the normalized Y
		ry 	= (nx*bvSin)-(ny*bvCos);

		// get distance to item.
		dist	= GetDist(rx,ry);

		if ( dist < MIN_ITEM_DISTANCE) continue;

		bitmap = ouractor->CurrBitmap(player->angle);

		// divide by 2 (to get half)
		bmh2		= (float)(effects->EffectBitmap(bitmap)->h/2);

		x1 =rx;
		y1 =ry+bmh2;
		x2 =rx;
		y2 =ry-bmh2;

		if( y1 > x1 && y2 > x2 || y1 < -x1 && y2 < -x2) continue;

		BegCol = 0;
		EndCol = View_Width;

		if (x1 && y1 < x1 && y1 > -x1)
		{
			BegCol = CalculateScreenColumn(x1,y1);
			if ( BegCol > View_Width ) continue;
			if ( BegCol < 0 ) BegCol = 0;
		}

		if (x2 && y2 < x2 && y2 > -x2)
		{
			EndCol = CalculateScreenColumn(x2,y2);
			if ( EndCol < 0 ) continue;
			if ( EndCol > View_Width) EndCol = View_Width;
		}

		ourline->bitmap  = bitmap;
		ourline->dist	  = dist;
		ourline->length  = effects->EffectBitmap(bitmap)->h;
		ourline->rx1	  = x1;
		ourline->ry1	  = y1;
		ourline->rx2	  = x2;
		ourline->ry2	  = y2;
		ourline->sector  = ouractor->sector;
		ourline->BegCol = BegCol;
		ourline->EndCol = EndCol;

		xd = ouractor->x+ouractor->halfwit;
		yd = ouractor->y;
		RotatePoint(ouractor->x,ouractor->y,xd,yd);

		xd = ouractor->x-ouractor->halfwit;
		yd = ouractor->y;
		RotatePoint(ouractor->x,ouractor->y,xd,yd);

		Insert_Line(ourline);
		actors_inserted++;

	}
	actors->IterateActors(DONE);
}



const int MAX_ANIM_SECS = 512;
static cAnimatedSector *animated_sectors[MAX_ANIM_SECS];
int numanimsecs=0;

static void update_animated_sectors(int players_room)
{
	for (int i=0; i < numanimsecs; i++)
	{
		cAnimatedSector *sec = animated_sectors[i];
		if (sec->room == players_room)
		{
			sec->AnimateTextures();
			if (sec->flags & SECTOR_SELFILLUMINATING)
				sec->AnimateLight();
		}
	}
}


static void add_animated_sector(cAnimatedSector *sec)
{
	if (numanimsecs < MAX_ANIM_SECS)
		animated_sectors[numanimsecs++] = sec;
}


void BuildViewBySector(cPlayer *player, unsigned char *viewBuffer,int pitch)
{
	int		 r=1;
	sector *sec ;

		wall_animation_ticks += timing->nmsecs;

#ifndef AGENT
	if (viewBuffer)
		StartRenderView(viewBuffer,pitch, player);
#endif

	bvxPlayer	= player->x;
	bvyPlayer	= player->y;
	int bvAngle	= player->angle;
	bvCos 		= CosTable[bvAngle];
	bvSin 		= SinTable[bvAngle];

	int angle_perp2 = FixAngle(Angle_360-FixAngle(bvAngle+Angle_180));
	perps2=CosTable[angle_perp2];
	perpc2=SinTable[angle_perp2];

	memset(vertlist,0,MAX_VERTICES);
	memset(seclist, 0,MAX_SECTORS);

	LineIndex	 = 0;
	PendingLines = 0;
	linebase		 = 0;

	sec=level->Sectors[player->sector];
	if ( sec < 0 )
		return;
	update_animated_sectors(sec->room);

/*
static linedef *saved[1024];
static int num_saved= 0;


if (opt_on)
{
for (int i = 0;i<num_saved; i++)
  RenderLinedef(saved[i]);

EndRenderView();
return;
}
 num_saved = 0;
*/

//	if (!opt_on)
	InsertSector(player->sector);

//	static int prev_lines;
	int lines = 0;
//	int rendered =0;

	while(PendingLines)
	{
		linedef *line = linelist[linebase];
		lines++;

		if (viewBuffer)
			r = RenderLinedef(line);
// 	saved[num_saved++] = line;
		if (r == 2)
			break;
		PendingLines--;
		linebase++;
		if ( r )
			InsertSector(line->facingsector);
// 	else
// 		rendered++;
	}

	lines_compared = 0;
	overlapped_lines = 0;
	lines_inserted = 0;
	actors_inserted = 0;

	if (viewBuffer)
		EndRenderView();
}


long FixAngle(long ViewAngle)
	{
	if (ViewAngle < 0 )
		{
		while(ViewAngle < 0) ViewAngle += Angle_360;
		}
	if (ViewAngle >= Angle_360)
		{
		while(ViewAngle >= Angle_360) ViewAngle -= Angle_360;
		}
	return (ViewAngle);
	}


void AddLine(file_linedef &f)
{
	linedef *line	= new linedef(f, true /* line belongs to sector */);
	sector *s		= level->Sectors[f.sector];

	if ( f.sector == f.facingsector )
		return;

	// create a duplicate line to insert into the facing sector
	linedef *duplicate = new linedef(f,false);
}

linedef::linedef(file_linedef &f, bool line_in_sector)
{

	from			 = f.from;
	to 			 = f.to;
	flags 		 = f.flags;
	bitmap		 = f.fwall_bitmap;
	cwall_bitmap = f.cwall_bitmap;
	sector		 = f.sector;
	facingsector = f.facingsector;
	TripFlags	 = f.TripFlags;
	trip1 		 = f.trip1;
	trip2 		 = f.trip2;
	trip3 		 = f.trip3;
	trip4 		 = f.trip4;
	actor 		 = NO_ACTOR;

	animation_start = bitmap;
	animation_end	 = f.animation_end;

	fwall_u_offset = f.fwall_u_offset;
	fwall_v_offset = f.fwall_v_offset;
	cwall_u_offset = f.cwall_u_offset;
	cwall_v_offset = f.cwall_v_offset;

	//if texture is to be stretched/compressed, offsets are not used
	if (f.flags & LINE_STRETCH_FWALL_HORZ)
		fwall_u_offset = 0;

	if (f.flags & LINE_STRETCH_FWALL_VERT)
		fwall_v_offset = 0;

	if (f.flags & LINE_STRETCH_CWALL_HORZ)
		cwall_u_offset = 0;

	if (f.flags & LINE_STRETCH_CWALL_VERT)
		cwall_v_offset = 0;

	bool	floor_base	 = (flags & LINE_SECTOR_FLOOR_BASE );
	bool	ceiling_base = (flags & LINE_SECTOR_CEILING_BASE);

	if (line_in_sector == false) // line is in facing sector
	{
		// swap vertices, and sectors;
		from = f.to;
		to   = f.from;
		sector = f.facingsector;
		facingsector = f.sector;

		// set base booleans
		floor_base	 = (flags & LINE_FACING_FLOOR_BASE );
		ceiling_base = (flags & LINE_FACING_CEILING_BASE);
	}

	struct sector *sec = level->Sectors[sector];

	// determine length
	length =  fdist(x1(),y1(),x2(),y2());
	if(length == 0.0)
		length = 0.01f; // if 0 length give it little length;

	// set the sine and cosine of the line
	COS	= (x2() - x1())/length;
	SINE	= (y2() - y1())/length;

	// setup surfaces and baselines if line is surface baseline
	if (floor_base)
	{
		if (!sec->floor_surface) // if the floor surface is not created (as a sloping surface)
			sec->floor_surface = new cSurface(sec->floor_height,0.0f, sec->flags & SECTOR_NO_STRETCH_FLOOR);
		sec->floor_surface->set_baseline(this);
	}

	if (ceiling_base)
	{
		if (!sec->ceiling_surface) // if the ceiling surface is not created (as a sloping surface)
				sec->ceiling_surface = new cSurface(sec->ceiling_height,0.0f, sec->flags & SECTOR_NO_STRETCH_CEILING);
		sec->ceiling_surface->set_baseline(this);
	}

	// linkup line to the line's sector.
	linedef *firstline = sec->firstline;  // save first line
	sec->firstline 	 = this; 			  // add line to beggining of linked list
	this->nextline 	 = firstline;		  // link up rest of list to line
}


sector::sector(file_sector &fs)
{

	SecNo 		= fs.id;
	OnTop 		= fs.ceiling_bitmap;
	OnBottom 	= fs.floor_bitmap;

	floor_height	= fs.floor_height;
	ceiling_height = fs.ceiling_height;
	HtOffset 		= fs.height_offset;

	flags 		= fs.flags;
	room			= fs.room;
	tag			= fs.tag;

	lightlevel = fs.lightlevel;
	firstline = (linedef *)NULL;

	floor_surface	= NULL;
	ceiling_surface = NULL;

	float floor_slope_angle = fs.floor_slope_angle + fs.floor_slope_frac * 0.01f;
	if (floor_slope_angle)
		floor_surface = new cSurface(floor_height,floor_slope_angle,flags & SECTOR_NO_STRETCH_FLOOR);

	float ceiling_slope_angle = fs.ceiling_slope_angle + fs.ceiling_slope_frac*0.01f;
	if (ceiling_slope_angle && !(flags & SECTOR_SKY)) //only slope ceiling if it not sky
		ceiling_surface = new cSurface(ceiling_height,-ceiling_slope_angle, flags & SECTOR_NO_STRETCH_CEILING);
}

void sector::GetFloorTextureOffsets(int &x, int &y)
{
	x = y = 0;
}

void sector::GetCeilingTextureOffsets(int &x, int &y)
{
	x = y  = 0;
}

cAnimatedSector::cAnimatedSector(file_sector &fs) :
	sector(fs)
{
	xanim_top	 = fs.xanim_top;
	yanim_top	 = fs.yanim_top;
	xanim_bottom = fs.xanim_bottom;
	yanim_bottom = fs.yanim_bottom;

	tx =	ty =	bx =	by = 0;

	peak =0;
	step =0;

	add_animated_sector(this);
}

void cAnimatedSector::AnimateTextures(void)
{
	const int mask = 63;
	tx = (tx + xanim_top)	& mask;
	ty = (ty + yanim_top)	& mask;

	bx = (bx + xanim_bottom)& mask;
	by = (by + yanim_bottom)& mask;
}

void cAnimatedSector::AnimateLight(void)
{
	static const short levels[] = {4,19,6,1,18,1,8,4,8,1};

	if (sector_animation_ticks = !sector_animation_ticks)
	{
	 sector_animation_ticks = 0;

		if (!step)
		{
			first_peak = levels[peak];
			peak++;
			if (peak >= (sizeof levels/sizeof levels[0]))
				peak = 0;
			second_peak = levels[peak];
			step = (second_peak - first_peak)/4;
		}
		first_peak += step;
		if ((step > 0 && first_peak > second_peak)|| (step <0 && second_peak > first_peak))
		{
			 step = 0.0;
			 first_peak = second_peak;
		}
	}

	lightlevel= float2int(first_peak);
}

void cAnimatedSector::GetFloorTextureOffsets(int &x, int &y)
{
	x	= bx; y = by;
}

void cAnimatedSector::GetCeilingTextureOffsets(int &x, int &y)
{
	x = tx;	y = ty;
}

sector::~sector()
{
	if (floor_surface)
		delete floor_surface;

	if (ceiling_surface)
		delete ceiling_surface;

	floor_surface = ceiling_surface = NULL;
}

void __cdecl Init4DX(int width, int height)
{
	double	Radians;
	double	i_180;
	long		InternalDegree;

	TanTable   = new float[Angle_360];
	CosTable   = new float[Angle_360];
	SinTable   = new float[Angle_360];

	i_180=(double)Angle_180;
	for (InternalDegree = 0; InternalDegree < Angle_360; ++InternalDegree)
	{
		/* Radians = (2pi InternalDegree) / Angle360 reduces to: */
		Radians = (3.1415927 * (double)InternalDegree) / i_180;
		SinTable[InternalDegree] = (float)sin( Radians);
		CosTable[InternalDegree] = (float)cos( Radians);
		TanTable[InternalDegree] = (float)tan( Radians);
	}

	//for ( i=0;i<FIXED_1;i++ )
	//{
	// put the angle for each ratio in the table.
	//ATanTable[i]=(int)((atan((float)i)*((float)Angle_360)/(float)6.2831854)*(float)65536.0f);
	//}


	View_Width		= width;
	View_WidthMax	= View_Width	-1;

	View_HalfWidthfloat	= (float)(View_Width/2);
	viewing_distance		= (float)View_Width *.7483f; //

	InitRenderView(width,height,viewing_distance);
}


void __cdecl DeInit4DX(void)
{
	if(TanTable) delete [] TanTable; TanTable = 0;
	if(CosTable) delete [] CosTable; CosTable = 0;
	if(SinTable) delete [] SinTable; SinTable = 0;
	//if(flats) delete [] flats;

	DeInitRenderView();
}
// dotproduct	  = (x1*y1) + (x2*y2); // normalize both
// Cross Product = (x1*y2) + (x2*y1);


/*
void BuildTables(int width, int height)
	{

	View_Width		 = width;
	View_WidthMax	 = View_Width	-1;

	View_HalfWidthfloat	= (float)(View_Width/2);


	 InitRenderView(width,height,viewing_distance);
	}
*/

int secvert[256];
int numsecvert=0;
void addvert(int vert)
{
	int i;
	for(i=0;i<numsecvert;i++)
	{
		if ( vert == secvert[i] )
			return;
	}
	secvert[numsecvert]=vert;
	numsecvert++;
}

void RotatePoint(float x,float y,float &x2, float &y2)
{
	float nx,ny,rx,ry;

	nx = x2 - x;
	ny = y2 - y;

	// rotate the vertexes by n degrees.
	rx 	= (nx*perpc2)-(ny*perps2);
	ry 	= (nx*perps2)+(ny*perpc2);
	x2 	= rx+x;
	y2 	= ry+y;
}

void SetLineAngles(struct linedef *aLine)
{
	float lx1,lx2,ly1,ly2;

	lx1 = level->Vertices[aLine->from].x;
	lx2 = level->Vertices[aLine->to].x;
	ly1 = level->Vertices[aLine->from].y;
	ly2 = level->Vertices[aLine->to].y;

	aLine->COS	= (lx2 - lx1)/aLine->length;
	aLine->SINE	= (ly2 - ly1)/aLine->length;
}

/*

void RaiseFloor(int sec,float raiseval)
	{
	sector *oursec = level->Sectors[sec];

	oursec->floor_height += raiseval;
	if ( oursec->floor_height > oursec->ceiling_height) oursec->floor_height = oursec->ceiling_height;
	// check all the actors in this sector, and check to see if we squished someone.
	// also modify the heights of anyone on this sector.
	// *** WE NEED TO DEAL WITH THIS
	//if (us->sector =sec) SetActorData(us);
	}

void RaiseCeiling(int sec,float raiseval)
	{
	sector *oursec = level->Sectors[sec];

	oursec->ceiling_height += raiseval;
	if ( oursec->ceiling_height < oursec->floor_height) oursec->ceiling_height = oursec->floor_height;
	// check all the actors in this sector, and check to see if we squished someone.
	}

void RotateSector(int sec, float x, float y, int ndegrees)
	{
	sector *oursec = level->Sectors[sec];
	float nx,ny,rx,ry,rcos,rsin;
	int i;
	linedef *line;

	rcos = CosTable[ndegrees];
	rsin = SinTable[ndegrees];

	numsecvert = 0;
	// extract out all the vertexes
	line = oursec->firstline;
	if(line)do
		{
		addvert(line->from);
		addvert(line->to);
		} while(line=line->nextline);

	for ( i=0;i<numsecvert;i++ )
		{
		// normalize each vertex
		nx = level->Vertices[secvert[i]].startx - x;
		ny = level->Vertices[secvert[i]].starty - y;

		// rotate the vertexes by n degrees.
		rx 	= (nx*rcos)-(ny*rsin);
		ry 	= (nx*rsin)+(ny*rcos);

		// unnormalize them
		level->Vertices[secvert[i]].x = rx+x;
		level->Vertices[secvert[i]].y = ry+y;
		}

	line = oursec->firstline;
	if(line)do
		{
		SetLineAngles(line);
		} while(line=line->nextline);
	}

void MoveSector(int sec, float x, float y)
	{
	sector *oursec = level->Sectors[sec];
	linedef *line;
	int	 i;

	// extract out all the vertexes
	line = oursec->firstline;
	if(line) do {
		addvert(line->from);
		addvert(line->to);
		} while (line=line->nextline);

	// add x,y to each vertex.
	for ( i=0;i<numsecvert;i++ )
		{
		level->Vertices[secvert[i]].x += x;
		level->Vertices[secvert[i]].y += y;
		}

	line = oursec->firstline;
	if(line)do
		{
		SetLineAngles(line);
		} while(line=line->nextline);
	}
*/


float sector::CeilHt(float x, float y)
{
	if (!ceiling_surface)
		return ceiling_height;
	else
		return ceiling_height + ceiling_surface->height_at(x,y);
}

void sector::PostLineLoadInit(void)
{
	if (floor_surface && !floor_surface->Baseline())
	{
		delete floor_surface;
		floor_surface = NULL;
	}

	if (ceiling_surface && !ceiling_surface->Baseline())
	{
		delete ceiling_surface;
		ceiling_surface = NULL;
	}

/*
	int count = 0;
	for (linedef *line = firstline;line != NULL ;line = line->nextline)
		count++;

	lines = new linedef [count];
	num_lines=count;

	count = 0;
	for (line = firstline;line != NULL ;line = line->nextline)
		lines[count++] = *line;

	for (linedef *line = firstline;line != NULL ;line = line->nextline)
	{
		BOOL sloping_facing_sector = level->Sectors[line->facingsector]->floor_surface  ||
																 level->Sectors[line->facingsector]->ceiling_surface;

		if (floor_surface || ceiling_surface || sloping_facing_sector)
			((sloping_linedef *)line)->determine_heights();
	}*/
}

cSurface::cSurface(float a_base_height,float a_slope, bool no_stretch)
{
	baseline				 = NULL;
	slope 				 = (float)tan(a_slope*M_PI/180);
	base_height 		 = a_base_height;
	no_texture_stretch = no_stretch;
}


float cSurface::height_at(float x, float y)
{
	if (!slope)
		return 0.0f;

	if (!baseline)
	{
		GAME_ERROR(IDS_BASELINE_NOT_SET);
		return 0.0;
	}

	// sector co-ord system: origin is at baseline line's (x1,y1), y axis is parallel to line.
	float sector_cos			= -baseline->SINE;
	float sector_sin			=  baseline->COS;
	float sector_origin_x	=  baseline->x1();
	float sector_origin_y	=  baseline->y1();

	// transform to sector co-ords;
	x -= sector_origin_x;
	y -= sector_origin_y;
	float sec_x   =  x*sector_cos + y*sector_sin;
	//float sec_y =  x*sector_sin - y*sector_cos;

	return(sec_x * slope);
}

float sector::FloorHt(float x, float y)
{
	if (!floor_surface)
		return floor_height;
	else
		return floor_height + floor_surface->height_at(x,y);
}


// Determines the heights of the lines' vertices.
// Since both the floor and the ceiling can be sloping,there are possibly 2 heights per vertex.
// Note this must be called AFTER the sector's sloping surfaces baseline(s) have been set.
// (The baseline is required in height_at(), which is called from FloorHt and CeilHt)

/*
void sloping_linedef::determine_heights(void)
{
	struct sector *sec = level->Sectors[sector];

  floor_height1 = sec->FloorHt(x1(),y1());
	floor_height2 = sec->FloorHt(x2(),y2());

  ceiling_height1 = sec->CeilHt(x1(),y1());
	ceiling_height2 = sec->CeilHt(x2(),y2());
}
*/



bool sector::SlopingCeiling(void)
{
	return (ceiling_surface && ceiling_surface->Slope());
}

bool sector::SlopingFloor(void)
{
	return (floor_surface && floor_surface->Slope());
}

