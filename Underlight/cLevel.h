#ifndef CLEVEL_H
#define CLEVEL_H

// Copyright Lyra LLC, 1996. All rights reserved. 

// Class for handling the game levels.

#include <stdio.h>
#include "4dx.h"
#include "Central.h"
#include "Txtrload.h"
#include "SharedConstants.h"

//////////////////////////////////////////////////////////////////
// Constants

const int NO_LEVEL = -1;
const int PERSONAL_VAULT_FLAG = -2;
const int ACTUAL_PERSONAL_VAULT_LEVEL = 3; // Upper Basin I guess
const int MAX_LEVELS = 100;
const int MAX_AGENT_TYPES = 10;

enum texture_status {
	UNUSED = 0, // texture not needed by current level
	USED = 1,  // texture used by current level
	REMOVE = 2, // texture used by old level, not by new level, so remove it
	ADD = 3, // texture used by new level, but not by old level, so add it
};

//////////////////////////////////////////////////////////////////
// Structs

struct actor_sector_map_t
{
	cActor *actor;
    int sector;
};

struct texture_info_t
{
	unsigned char load_status;
};


//////////////////////////////////////////////////////////////////
// Prototypes

//////////////////////////////////////////////////////////////////
// External Function Prototypes

//////////////////////////////////////////////////////////////////
// Class Definition


class cLevel
{
public: // level data is public for backwards compatibility
	vertex Vertices[MAX_VERTICES];   
	sector* Sectors[MAX_SECTORS];
	room Rooms[MAX_ROOMS];
	BITMAPINFO_4DX texture_info[MAX_TEXTURES];
	BITMAPINFO_4DX flat_info[MAX_FLATS];
	actor_sector_map_t OList [MAX_ACTORS];
	int SectorStart[MAX_SECTORS];
	int NumOList;

protected:

private:
	texture_info_t TextureLoadList[MAX_TEXTURES];
	texture_info_t FlatLoadList[MAX_FLATS];
	unsigned char flat_bits[MAX_SIMUL_FLATS*4096];
	level_file_header_t level_file_header;
	level_data_t level_data;
	int level_id;
	TCHAR level_file_name[DEFAULT_MESSAGE_SIZE];
	int num_levels;
	FILE *level_fh, *texture_fh;
	level_header_t *level_headers;
	texture_header_t *texture_headers;
	flat_header_t *flat_headers;
	int num_textures, num_flats;
	int texture_bytes, max_texture_bytes;
	int total_lines;
	bool level_loaded[MAX_LEVELS];
	file_vertex FileVertices[MAX_VERTICES];

#ifdef _DEBUG
	short TexturesUsed[MAX_TEXTURES];
	short FlatsUsed[MAX_FLATS];
	short GeneratorsUsed[MAX_LEVELS][Avatars::MAX_AVATAR_TYPE+1];
#endif

public:
    cLevel(TCHAR *filename); 
	void Load(int id);
    ~cLevel(void);

//#define UL3D
#ifdef UL3D

struct file_vertex3d
{
    short x; 
    short y; 
	short floor;
	short ceil;
};

	file_vertex3d FileVertices3D[MAX_VERTICES];

	struct  hull_t {
	int num_lines;
	bool assigned[512];
	int lines[512];
	int facing[512];
	int sector_index;
	int sector_id;
	float distance;
};

struct actor3d
{
    int flags; // above flags
    short x; // x,y coords
    short y;
    short angle; 
    short finfo; 
	int height; // height of actor
};

struct bbox_t {
	float minx;
	float miny;
	float maxx;
	float maxy;
};
	hull_t hulls[256]; 
	
	int dupverts[16];

	bool line_assigned[10000];

	void ProcessFiles(void);
	int BreakupSectors(file_vertex* verts, int num_sectors, int num_verts, int num_lines);
	int CreateHull(int sector_num, int line_num, int hull_num, int num_lines);
	int CombineHulls(int num_hulls, file_vertex* verts, int sector_num, int num_lines);
	FixBaseLines(int num_hulls, file_vertex* verts, int sector_num, bool floor, int line, bool facing_flag);
	void CreateEdgeSet(int v0_start, int v1_start, int v1_end, int hull_index,
						   hull_t& new_hull, bool first_from, bool first_to, int sec1, int sec2, int new_sec);
	void InitBBox(hull_t& hull, bbox_t& bbox, file_vertex* verts);
	void AddLineToHull(hull_t& hull, int l, int sec1, int sec2, int new_sec);
	int SplitHull(int shared_vert, int hull_num, int num_hulls, int num_dupverts);
	void MakeHullPiece(hull_t& old_hull, hull_t& new_hull, int start_line, int follow_line, int shared_vert, int num_dupverts);
	bool AddLinesToHull(hull_t& old_hull, hull_t& new_hull, int start_vert, int end_vert, int num_dupverts);

	int CompareZValues(int v0, int v1, int base_z, int hull_num, bool floor);


	file_sector all_fs[MAX_SECTORS];
	file_linedef all_lines[10000];

	FILE*   new_fh;
	int		offset;
	bool	write_data;
#endif


	// Selectors for Data on Current Level
	TCHAR* Name(int id);	
	TCHAR* RoomName(int id);
	inline int ID(void) { return level_id;};
	inline int NumLevels(void) { return num_levels;};
	inline int NumVertices(void) { return level_data.num_vertices;};
	inline int NumSectors(void) { return level_data.num_sectors;};
	inline int NumOrnaments(void) { return level_data.num_ornaments;};
	inline int NumLines(void) { return level_data.num_lines;};
	inline int NumRooms(void) { return level_data.num_rooms;};
	inline int TextureBytes(void) { return texture_bytes;};
	inline int MaxTextureBytes(void) { return texture_bytes;};

	bool FlatIsLoaded(int flat_id);
	inline bool IsValidLevel(int id) { if ((id >0) && (id < MAX_LEVELS)) return level_loaded[id]; else return false; };

	void Verify(void);	// Perform an integrity check on the level.
	bool SectorIsOpen(sector *s);

#ifdef UL_DEBUG
	void FindUsedTextures(int target_level = -1); // count all textures for all levels
	void CheckInterLevelPortals(void);
	void FindAllOpenSectors(void);
	void CheckInterLevelTeleports(void);
	void DumpMonsterGens(void);
#endif

private:
	void LoadBitmaps(void); 
	BOOL LoadLevelData(void);
	void CleanSectors(void);
	void OpenFiles(void);
	void CloseFiles(void);

#ifdef DEBUG
	void Debug_CheckInvariants(int caller);
#endif

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cLevel(const cLevel& x);
	cLevel& operator=(const cLevel& x);

};


extern cLevel *level;

struct linedef
 {
	  // vertex numbers
		short   from; 
		short   to;  

		// sector numbers
		short sector;       // the sector number that this contains
		short facingsector; // the sector number that this is facing
		
		// bitmaps
		short bitmap;                // bitmap for floor wall,actor bitmap etc.
		short cwall_bitmap;         //  bitmap for ceiling wall.
		short animation_start;      //  animation start bitmap
		short animation_end;        //  animation end bitmap
		
		// texture offsets for floor wall
		short fwall_u_offset;  // u offset : horizontal
		short fwall_v_offset;  // v offset : vertical
		
		// texture offsets for ceiling wall
		short cwall_u_offset;  // u offset : horizontal
		short cwall_v_offset;  // v offset : vertical
		
		int flags;

		// Trip values
		int TripFlags;
		short trip1;           // misc values for different trip wires.
		short trip2;
		short trip3;           // misc values for different trip wires.
		short trip4;		
    
		// transformed co-ords
		float rx1;
		float ry1;
		float rx2;
		float ry2;

		// beggining and end screen columns for linedef.
		short BegCol;
		short EndCol;

		float COS;           // which angle this is facing so we know when to draw floor etc.
		float SINE;          // the angle of the line from x1,y1 to x2,y2.
		
		float length;        // length of linedef
		float dist;          // distance of linedef to player

		cActor *actor;       // actor if linedef represents an actor
		
		linedef *nextline;   // next line in sector of this linedef
		
		inline linedef() { memset(this,0,sizeof(linedef));}
		linedef(file_linedef &f, bool is_in_sector);

		inline float x1() { return (actor) ? actor->x1 : level->Vertices[from].x;};
		inline float y1() { return (actor) ? actor->y1 : level->Vertices[from].y;};
		inline float x2() { return (actor) ? actor->x2 : level->Vertices[to].x  ;};
		inline float y2() { return (actor) ? actor->y2 : level->Vertices[to].y  ;};
};



#endif

