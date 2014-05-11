// txtrload.h
// 
// header file for Texture Loader
//
// Copyright Lyra LLC, 1996. All rights reserved. 
// 

#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

//#define SELECTFLATS							
//#define COMPRESSED								

#define printf DebugOut


//////////////////////////////////////////////////////////////////
// Constants


///////////////////////////////////////////////////////////////////
// Structures

struct texture_file_header
{
	int		num_textures;
	int		num_flats;
};


struct texture_header_t
{
	unsigned long	file_position;
	unsigned short	height;
	unsigned short	width;
	unsigned short  id;
	unsigned short  palette;
};

struct flat_header_t
{
	unsigned long	file_position;
	unsigned short	height;
	unsigned short	width;
	unsigned short  id;
	unsigned short  palette;
};


#endif


