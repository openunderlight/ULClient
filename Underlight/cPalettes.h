// Header file for Palettes and the Palette Manager classes

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CPALETTES_H
#define CPALETTES_H

#define STRICT

#include <windows.h>
#include "Central.h"
#include "Effects.h"
#include "4dx.h"

//////////////////////////////////////////////////////////////////
// Constants

const int NUM_SHADES = 32;

//////////////////////////////////////////////////////////////////
// Types

//////////////////////////////////////////////////////////////////
// Helpers

//////////////////////////////////////////////////////////////////
// Class Definitions


class cPalette
{

public: 

private:
	unsigned char *rgb;
	PIXEL **shades;
	int num_colors;
	
public:
    cPalette(unsigned char *colors, int size=PALETTE_SIZE);
	~cPalette(void);

	// RGB Selectors
	inline unsigned char Red(int index) { return rgb[index*3];};
	inline unsigned char Green(int index) { return rgb[index*3+1];};
	inline unsigned char Blue(int index) { return rgb[index*3+2];};

	// Shade Table Selectors
	inline PIXEL* ShadeTable(int index) { return &(shades[index][0]);};

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cPalette(const cPalette& x);
	cPalette& operator=(const cPalette& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, char *file);
#else
	inline void CheckInvariants(int line, char *file) {};
#endif

};

class cPaletteManager
{

public: 

private:
	 // For distance shading:
	 unsigned int min_distance;       // Before this distance, no distance shading occurs
	 unsigned int max_distance;       // After this  distance, all colors are black
	 double   fade_rate;              // The rate at which shading occurs, calculated from above
	
	 int global_brightness_factor;

	 cPalette* (pals[MAX_PALETTES]);  

public:
    cPaletteManager(void);
    ~cPaletteManager(void);

	// Set the minimum and maximum distances;
	void SetDistances(unsigned int min_dist, unsigned int max_dist); 

	// Set the global brightness: range 0.0..1.0, where 0.0 is pitch black, 1.0 is brightest 
	void SetGlobalBrightness(float new_brightness);

	// Get a shaded palette, for the given distance and ambient light level.
    PIXEL *GetPalette(int palette, unsigned int distance, unsigned int light_level);
	
	// Get the original unshaded palette
	PIXEL *GetUnshadedPalette(int palette); 

	// Add a new palette
	void AddPalette(unsigned char *colors, int palette_id, int size = PALETTE_SIZE);
	void FreePalette(int palette_id);

	inline unsigned char Red(int palette, int index) { return pals[palette]->Red(index);};
	inline unsigned char Green(int palette, int index) { return pals[palette]->Green(index);};
	inline unsigned char Blue(int palette, int index) { return pals[palette]->Blue(index);};

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cPaletteManager(const cPaletteManager& x);
	cPaletteManager& operator=(const cPaletteManager& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, char *file);
#else
	inline void CheckInvariants(int line, char *file) {};
#endif

};

#endif
