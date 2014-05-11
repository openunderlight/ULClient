// Copyright Lyra LLC, 1997. All rights reserved. 

// Level file version 5 structures

// GAME DATA

#ifndef V5_STRUCTS_H
#define V5_STRUCTS_H

#include <memory.h>
#include "LyraDefs.h"

// Changes from V4:

// * linedef fields renamed
// * offsets added to linedef textures
// * unused fields added to linedefs
// * palette option added for background

#define CURRENT_VERSION 5
const int LEVEL_NAME_SIZE = 64;
const int ROOM_NAME_SIZE = 32;

struct level_file_header_t
{
	int version;
	int num_levels;
	char padding[256];
};

struct level_header_t
{
	unsigned short level_id;
	unsigned short checksum1;
	unsigned int file_position;
	char name[LEVEL_NAME_SIZE];
};

struct level_data_t // formerly GameDat
{
    long num_vertices;
    long num_sectors;
    long num_ornaments;
    long num_lines;
    short num_rooms;
	unsigned short checksum2;
};

//* CurrRooms is the size of an array of room structures


struct file_vertex
{
    short x; 
    short y; 
};


struct file_linedef
{
		  // vertex numbers
		short   from;
		short   to;

		// sector numbers
		short sector;          // the sector number that this contains
		short facingsector;    // the sector number that this is facing
		
		// bitmaps
		short fwall_bitmap;          // fwall is the main wall
		short animation_end;         // the end of the animation cycle for the fwall
		short cwall_bitmap;           // cwall is for the "top slice".

		// offsets
		short fwall_u_offset;	// *** NEW; u = horizontal			
		short fwall_v_offset;   // *** NEW; v = vertical
		short cwall_u_offset;	// *** NEW
		short cwall_v_offset;	// *** NEW
		
		int flags;

		// Trip values
		int TripFlags;
		short trip1;           // misc values for different trip wires.
		short trip2;
		short trip3;           // misc values for different trip wires.
		short trip4;

		// for forward compatibility
		short unused1;          // switched bitmap
		short unused2;			// *** NEW
		short unused3;			// *** NEW
		short unused4;			// *** NEW
		short unused5;			// *** NEW			
		short unused6;			// *** NEW

};


// SECTOR - changes:

struct file_sector
{
	short id;
	
	short ceiling_bitmap;    // in BITMAPINFO array, the top texture.
	short floor_bitmap; // the bottom texture.
	
	short floor_height;
	short ceiling_height;		

	char floor_slope_angle; // NEW
	unsigned char floor_slope_frac;
	
	char ceiling_slope_angle; // NEW
	unsigned char ceiling_slope_frac;
	
	short height_offset; // height offset for actors in this sector
	
	unsigned int flags;
	
	short xanim_top; // pixels per frame animation for floor and ceiling
	short yanim_top;
	short xanim_bottom;
	short yanim_bottom;
	
	short lightlevel;
	
	short tag; 
	short room; 
};


struct actor
{
    int flags; // above flags
    short x; // x,y coords
    short y;
    short angle; 
    short finfo; 
};

struct room
{
	realmid_t id;
	char  name[ROOM_NAME_SIZE];
	short background;
	short background_palette; // *** NEW
	unsigned int flags;
};

#endif
