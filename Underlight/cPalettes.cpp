// Handles the goal-posting screens

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT
#include <math.h>
#include "resource.h"
#include <stdio.h>

#include "4dx.h"
#include "cPalettes.h"
#include "cDDraw.h"

//////////////////////////////////////////////////////////////////
// External Global Variables
extern cDDraw *cDD;
extern HINSTANCE hInstance;

//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// cPalette Class Definition

// Constructor -  colors is a pointer to a size byte palette structure
cPalette::cPalette(unsigned char *colors, int size) 
{
  int red_shift,green_shift;
  rgb = NULL; shades = NULL;

  ePixelFormat format = cDD->PixelFormat();
  rgb = new unsigned char[size];
  shades = new PIXEL*[NUM_SHADES];

  for (int i=0; i<NUM_SHADES; i++)
	  shades[i] = new PIXEL[size];

  switch (format)
	{
	 case PIXEL_FORMAT_565:
		 green_shift = 2;
		 red_shift   = 11;
		 break;
	 case PIXEL_FORMAT_555:
		 green_shift = 3;
		 red_shift   = 10;
		 break;
	 default:
		 GAME_ERROR(IDS_PIXEL_ERR1);
  }

  num_colors = size/3;
	
  memcpy(rgb, colors, size);

	// Takes a num_colors color initial palette of 3 byte (RGB) color elements and from this
	//creates NUM_SHADES 16bit palettes each progressivly darker.
	for (int j = 0; j <num_colors; j++)							// for each pallete entry..
	{
		int p = j*3;
		
 		 // get entry's r,g,b values, reduce them to fit in 16 bits
		PIXEL  red   = (colors[p])   >> 3;  
		PIXEL  green = (colors[p+1]) >> green_shift; 
		PIXEL  blue  = (colors[p+2]) >> 3;          
		
		for (int i=0; i<NUM_SHADES; i++)   
		{
			// merge r,g,b to form 16 bit value 
			shades[i][j] = (red << red_shift) | (green << 5)| blue;   

			//Note: this shading algorithm assumes there are around 32 shades
			// if there were less shades, you would need to decrease the colors at a faster rate.
			// if there were more shades, a slower rate would be needed
			
			// Darken each color value: 
			if (red)   
				red--;   // decrease by one since we have 32 shades of red                                
			
			if (green) 
				green--;
			if (green && format == PIXEL_FORMAT_565) 
			  	green--; // green has 64 shades in 565 so decrease by 2 
			
			if (blue)  // same as red
				blue--;
		}
	}

	return;
}

// Destructor
cPalette::~cPalette(void)
{
	if (rgb)
		delete [] rgb;
	if (shades)
	{
		for (int i=0; i<NUM_SHADES; i++)
			delete [] shades[i];
		delete [] shades;
	}
	return;
}

// Check invariants

#ifdef CHECK_INVARIANTS

void cPalette::CheckInvariants(int line, char *file)
{
	return;
}

#endif

//////////////////////////////////////////////////////////////////
// cPaletteManager Class Definition

// Constructor
cPaletteManager::cPaletteManager(void) 
{
	fade_rate = 0.0;
	max_distance = 65536; // max 4dx distance
	
	SetGlobalBrightness(1.0);         // set global brightness to max.

	for (int i=0; i<MAX_PALETTES; i++)
		pals[i] = NULL;

	return;
}


void cPaletteManager::SetDistances(unsigned int min_dist, unsigned int max_dist)
{
	
	// calculate the fade rate: 
	double range = fabs((long double)max_dist - min_dist);
	fade_rate    = NUM_SHADES/range;        

	// adjust min distance so its a multiple of NUM_SHADES
	min_distance = (min_dist/NUM_SHADES)*NUM_SHADES ;  

	// adjust max distance so its a multiple of NUM_SHADES
	max_distance = (max_dist/NUM_SHADES)*NUM_SHADES ;  
}

void cPaletteManager::SetGlobalBrightness(float new_brightness)
{
	new_brightness = MAX(0.0f,new_brightness);    //clamp to range 0.0..1.0
	new_brightness = MIN(1.0f,new_brightness);
	
	global_brightness_factor  =  (int)((float)NUM_SHADES*(1.0f - new_brightness));
}

void cPaletteManager::AddPalette(unsigned char *colors, int palette_id, int size)
{	
	if (pals[palette_id])
		this->FreePalette(palette_id);

	pals[palette_id] = new cPalette(colors, size);

	return;
}

void cPaletteManager::FreePalette(int palette_id)
{
	if (!pals[palette_id])
	{
		NONFATAL_ERROR(IDS_UNLOADED_PALETTE);
		return;
	}
	//delete pals[palette_id];
	//pals[palette_id] = NULL;
}



PIXEL* cPaletteManager::GetPalette(int palette, unsigned int distance, unsigned int light_level)
{
	unsigned int p_index;

	if (distance <= min_distance)        
		distance = 0;											// no distance shading before min distance 
	else if (distance > max_distance)  	   
		 distance = max_distance;         // limit distance to max distance 
  else 
		 distance -= min_distance;				// within distance shading range 

	// calculate which shaded palette to use: This can be fine tuned for different shading 
  p_index =  (float2int(distance*fade_rate) + light_level)/2; 
	
	p_index += global_brightness_factor;  

	if (p_index >= NUM_SHADES)
		 p_index = NUM_SHADES-1;

	if (pals[palette] != NULL)
		return pals[palette]->ShadeTable(p_index);
	else 	
	{
		GAME_ERROR1(IDS_PIXEL_ERR2, palette);
		return NULL;
  }
}


PIXEL* cPaletteManager::GetUnshadedPalette(int palette)
{
	return pals[palette]->ShadeTable(0);
}


// Destructor
cPaletteManager::~cPaletteManager(void)
{
	for (int i=0; i<MAX_PALETTES; i++)
		if (pals[i])
			delete pals[i];

	return;
}

// Check invariants

#ifdef CHECK_INVARIANTS

void cPaletteManager::CheckInvariants(int line, char *file)
{
	return;
}

#endif

