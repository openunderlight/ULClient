#ifndef EFFECTS_H
#define EFFECTS_H

//  Effects.h: constants, structures, etc. shared between
// the Underlight and EffectMaker projects

// Copyright Lyra LLC, 1996. All rights reserved. 


//////////////////////////////////////////////////////////////////
// Constants

const int MAX_PALETTES = 1024; // max # of palettes 
const int MAX_SIMUL_PALETTES = 32;
const int MAX_SOUNDS = 256;	// max # of sounds 
const int MAX_BITMAPS = 2048; // max # of bitmaps
const int MAX_FLATS = 768; 
const int MAX_SIMUL_FLATS = 32;
const int MAX_TEXTURES = 2048;
const int PALETTE_SIZE = 768;  
const int USE_ACTOR_COLOR_TABLES = -1; // palette = -1 for colorized actors
const int USE_FX_COLOR_TABLES = -2; // palette = -2 for colorized effects
const int MAGIC_AGENT_PALETTE = 1019;
const int MAGIC_FX_PALETTE = 1020;
const int MAGIC_AVATAR_PALETTE_1 = 1022;
const int MAGIC_AVATAR_PALETTE_2 = 1023;

///////////////////////////////////////////////////////////////////
// Structures

struct effect_file_header
{
	int		num_palettes;
	int		num_visual_effects;
	int		num_sound_effects;
};


struct palette_header
{
	unsigned long	file_position;
	unsigned short	count;
	unsigned short  id;
};

struct visual_effect_header
{
	unsigned long	file_position;
	unsigned short	height;
	unsigned short	width;
	unsigned char	views;
	unsigned char   frames;
	unsigned short	count;
	unsigned short  bpp;
	unsigned short  id;
	short			palette;
	short			rle; 
	int				size;
};

struct sound_effect_header
{
	unsigned long	file_position;
	unsigned long	format_length;
	unsigned long	data_length;
	unsigned short  count;
	unsigned short  id;
};


#endif