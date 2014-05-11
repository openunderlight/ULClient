// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef INCL_4DX
#define INCL_4DX

#include "Central.h"
#include "cActor.h"
#include "Effects.h"

const int MAX_VIEW_WIDTH = 768;
const int MAX_VIEW_HEIGHT = 480;

// Change this for different color modes
#define BITS_PER_PIXEL 16  

#if BITS_PER_PIXEL == 8
  typedef unsigned char  PIXEL;
  const int BYTES_PER_PIXEL = 1;
#else if BITS_PER_PIXEL == 16
  typedef unsigned short PIXEL; // short : for 16 bit color modes
  const int BYTES_PER_PIXEL = 2;
#endif

/////////////////////////////////////////////////////////////////
// Constants

const int ICON_HEIGHT = 16;
const int ICON_WIDTH = 16;

const unsigned int MIN_ITEM_DISTANCE  = 8<<8;
const unsigned int MAX_FRAMES         = 16;
const unsigned int MAX_DIRS           = 36;
const unsigned int MAX_ACTORS         = 1024;
//const unsigned int MAX_SECTORS        = 2048;
const unsigned int MAX_SECTORS        = 4096;
const unsigned int MAX_ROOMS		  = 64;
const unsigned int MAX_DIST           = 4096;
const unsigned int NUM_RAINDROPS      = 64;
const unsigned int NUM_SNOWFLAKES     = 128;

const int MAX_VERTICES = 8192;

const double M_PI    = 3.14159265358979323846;

const int  Angle_1   =   2;
const int  Angle_6   =  16;
const int  Angle_30  =  64;
const int  Angle_45  = 128;
const int  Angle_90  = 256;
const int  Angle_180 = 512;
const int  Angle_270 = 768;
const int  Angle_360 =1024;

extern float *TanTable;
extern float *CosTable;
extern float *SinTable;

/////////////////////////////////////////////////////////////
// Structures & Classes

#include "v5_structs.h" // defines file structures

class cPlayer;

struct vertex
{
    float x;       // Notice: X & Y are actual integers.
    float y;
                   // Following are for system use only...
    float n_x;     // normalized X (x - players x)
    float n_y;     // normalized y (y - players y)
    float r_x;     // after rotating into current view.
    float r_y;     // after rotation into current view.

};


class cSurface;

struct sector
{
		sector(file_sector &fs);
		virtual ~sector();

		float FloorHt(float x, float y); 
		float CeilHt(float x, float y); 

		// Further sector initialization that relies on the all the sector's linedefs 
		// being loaded and associated with the sector
		void PostLineLoadInit(void);

		bool SlopingFloor(void);
		bool SlopingCeiling(void);

		virtual void GetFloorTextureOffsets(int &x, int &y);
		virtual void GetCeilingTextureOffsets(int &x, int &y);
	
		short SecNo;					// Sector Number		
		short OnTop;					// in BITMAPINFO array, the top texture.
		short OnBottom;				// the bottom texture, 
		unsigned int flags;
		short room;

		float floor_height;			    
		float ceiling_height;    
		float HtOffset;         // height offset for actors in this sector

		short lightlevel;
		short tag; // used for sector-by-sector effects

		linedef *firstline;  // first line of linedefs for this sector

		cSurface *floor_surface; // floor and ceiling surfaces   
		cSurface *ceiling_surface;   
};

// animated sector : created when either of a sectors textures are animated, or the sector is illuminated,
// We subclass like this mainly to save space, since relativly few sectors are animated

class cAnimatedSector : public sector
{
	friend class cLevel;

	cAnimatedSector(file_sector &fs);

	// for animation of top and bottom textures
	short xanim_top;       
	short yanim_top;
	short xanim_bottom;
	short yanim_bottom;

	short tx,ty,bx,by;

	// for illumination
	int peak ;
	float step ;
	float first_peak;
	float second_peak;

	public:
		void AnimateTextures(void);
		void AnimateLight(void);
		void GetFloorTextureOffsets(int &x, int &y);
		void GetCeilingTextureOffsets(int &x, int &y);

};


struct BITMAPINFO_4DX
{
	unsigned char *address;
	unsigned char unused;
	short		  palette;
	short         w,h;
};

/////////////////////////////////////////////////////////////////
// Flags

// teleport to location. trip1 = x, trip2 = y, trip3 = min rank, trip4 = angle.
const unsigned int TRIP_TELEPORT                   = 0x00000020;
const unsigned int TRIP_FLIP                       = 0x00000040;
const unsigned int TRIP_GOALPOSTING				   = 0x00000080;
const unsigned int TRIP_LEVELCHANGE				   = 0x00000100; // trip1 = x, trip2 = y, trip3 = angle, trip4 = levelid
const unsigned int TRIP_SOUNDEFFECT				   = 0x00000200; // trip1 = sound id
const unsigned int TRIP_SHOWMESSAGE				   = 0x00000400; // trip1 = message id
const unsigned int TRIP_QUESTBUILDER			   = 0x00000800;

// Linedef flags - shared with level editor ( bits 0 - 23 )
const unsigned int NORM                      = 0x00000000;
const unsigned int BOUND                     = 0x00000001;  // line spans from floor to ceiling
const unsigned int TRIP_CROSS                = 0x00000002;  // crossing the line "trips" something.
const unsigned int TRIP_ACTIVATE             = 0x00000004;  // pressing space bar or mouse button.
const unsigned int LINE_GOES_TO_RECRUITING	 = 0x00000008;  //
const unsigned int LINE_ANIMATED             = 0x00000010;  // The line is animated
const unsigned int LINE_GOES_TO_HOUSE		 = 0x00000020;
const unsigned int LINE_SECTOR_FLOOR_BASE    = 0x00000040;  // baseline for sloping floor of line's sector 
const unsigned int LINE_SECTOR_CEILING_BASE  = 0x00000080;  // baseline for sloping ceiling of line's sector
const unsigned int LINE_FACING_FLOOR_BASE    = 0x00000100;  // baseline for sloping floor of line's facing sector
const unsigned int LINE_FACING_CEILING_BASE  = 0x00000200;  // baseline for sloping ceiling of line's facing sector
const unsigned int LINE_S_IMPASS             = 0x00000400;

const unsigned int LINE_STRETCH_FWALL_HORZ   = 0x00000800;  // Stretch texture across floor wall, don't tile
const unsigned int LINE_STRETCH_FWALL_VERT   = 0x00001000;  // Stretch texture down  floor wall, don't tile
const unsigned int LINE_STRETCH_CWALL_HORZ   = 0x00002000;  // Stretch texture across ceiling wall, don't tile
const unsigned int LINE_STRETCH_CWALL_VERT   = 0x00004000;  // Stretch texture down ceiling wall,  don't tile

const unsigned int LINE_NO_WARD				 = 0x00008000;  // linedef not wardable

const unsigned int LINE_FLIP_FWALL_HORZ      = 0x00010000;  // Flip texture horizontally on floor wall
const unsigned int LINE_FLIP_FWALL_VERT      = 0x00020000;  // Flip texture vertically on floor wall
const unsigned int LINE_FLIP_CWALL_HORZ      = 0x00040000;  // Flip texture horizontally on ceiling wall
const unsigned int LINE_FLIP_CWALL_VERT      = 0x00080000;  // Flip texture vertically on ceiling wall

const unsigned int LINE_SABLE_MOON           = 0x00100000;   
const unsigned int LINE_ECLIPSE              = 0x00200000;   
const unsigned int LINE_SHADOW               = 0x00400000;   
const unsigned int LINE_COVENT               = 0x00800000;   
const unsigned int LINE_RADIANCE             = 0x01000000;   
const unsigned int LINE_CALENTURE            = 0x02000000;   
const unsigned int LINE_ENTRANCED            = 0x04000000;   
const unsigned int LINE_LIGHT                = 0x08000000;   


// sector flags
const unsigned int SECTOR_SELFILLUMINATING  = 0x00000001;
const unsigned int SECTOR_SKY               = 0x00000002;
const unsigned int SECTOR_NO_STRETCH_FLOOR  = 0x00000004;
const unsigned int SECTOR_NO_STRETCH_CEILING= 0x00000008;
const unsigned int SECTOR_NOCOLLIDE         = 0x00000010;
const unsigned int SECTOR_ALTSHADE          = 0x00000020;
const unsigned int SECTOR_ITEMS_INVISIBLE   = 0x00000040;
const unsigned int SECTOR_ANIMATING			= 0X00000080;

// sector tags: effects of sector tags are applied every two seconds
const unsigned int SECTOR_WILLPOWER  =		1; // add 1 point 
const unsigned int SECTOR_INSIGHT  =		2;
const unsigned int SECTOR_RESILIENCE =		3;
const unsigned int SECTOR_LUCIDITY  =		4;
const unsigned int SECTOR_DREAMSOUL  =		5;
const unsigned int SECTOR_DAMAGE	  =		6; // lose 1 point
const unsigned int SECTOR_CURSE  =			7;
const unsigned int SECTOR_NO_REGEN  =		8;


// room flags
const unsigned int ROOM_SANCTUARY           = 0x00000001;
const unsigned int ROOM_NOREAP	            = 0x00000002;
const unsigned int ROOM_SABLE_MOON          = 0x00000004;   
const unsigned int ROOM_ECLIPSE             = 0x00000008;   
const unsigned int ROOM_SHADOW              = 0x00000010;   
const unsigned int ROOM_COVENT              = 0x00000020;   
const unsigned int ROOM_RADIANCE            = 0x00000040;   
const unsigned int ROOM_CALENTURE           = 0x00000080;   
const unsigned int ROOM_ENTRANCED           = 0x00000100;   
const unsigned int ROOM_LIGHT               = 0x00000200;   


//////////////////////////////////////////////////////////
// Function Prototype

int map(unsigned char *sCurPal, int r, int g, int b);

void BuildScreenRows(unsigned char *data, int pitch);

long lines_intersect(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4);
void Init_It(int w, int h);
void NewLine(int x1,int y1,int x2,int y2,unsigned int flags,int bitmap,int sector,int tosector,long tripflags=0,int trip1=0, int trip2=0, int trip3=0, int trip4=0, int bitmap2=0, int topbm=0, int topbm2=0);
int  PointInSector(float x, float y,sector * sec);
void SetVertScale(void);
int PointOnRight(linedef *aLine, float Px, float Py);

// Initializes the data. Must be called before any other functions!
void __cdecl Init4DX(int width, int height);

// DeInitializes the 4DX Data 
void __cdecl DeInit4DX(void);

// creates tables needed for a certain width/height. Must be called before
// rendering the first time, and also must be called whenever the width
// or height of the view changes.
//void BuildTables (int width, int height);

// Specifies to the renderer how to shade. Must call before rendering first time.
//void AssignShadeTables(SHADETABLE *standard, SHADETABLE *alternate);

// Builds a view into the databuffer.
void BuildViewBySector(cPlayer *player, unsigned char *viewBuffer,int pitch);

// massage the Angle to be sure it is within range.
long FixAngle(long Angle);

// This one writes out like a printf, but to the file "debug.out".
void __cdecl DebugOut(TCHAR *args,...);

// Add a line to a sector. Only call once, don't use for the line that is facing opposite.
void AddLine( file_linedef &file_line  );

// Renders the given actor to buffer,with given dimensions
void RenderActor(cActor *actor, PIXEL *buffer, int x, int y);


// Picking Functions: use to get the world co-ords of a screen position (col,row) :
// Call SetPickScreenCo_ords with desired col,row before a view is rendered, then call GetPickWorldCo_ords after
// rendering for the results. 
void SetPickScreenCo_ords(int col, int row);
int GetPickWorldCo_ords(float &x, float &y);

// returns the actor at the given screen point, NO_ACTOR if none exists
cActor *ActorAtScreenPoint(int col, int row);

// returns the linedef (or NULL) at the screen co-ords (set using SetPickScreenCo_ords)
linedef *GetPickLinedef(void);


int FindRoomOnMap(int x,int y);	

//inline functions

// A quicker alternative to casting, to convert floats to ints.
// Note: the values returned are rounded not clipped. i.e 4.5 -> 5, not 4
static inline int float2int(double double_val)
{
	long * long_ptr = (long *)&double_val;
	long long_val;

	double_val += 6755399441055744.0;// add the "magic number" to align the bits
	long_val = *long_ptr;// read the long integer value from double_val's bits

	return long_val;
}

#endif
