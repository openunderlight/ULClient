#ifndef CEFFECTS_H
#define CEFFECTS_H

// Class for handling all visual and sound effects file

// Copyright Lyra LLC, 1996. All rights reserved. 
#include <stdio.h>
#include "4dx.h"
#include "Central.h"
#include "LyraDefs.h"
#include "Effects.h"
#include "SharedConstants.h"

//////////////////////////////////////////////////////////////////
// Constants

const int NO_PALETTE = -1;
const int NO_BITMAP = -1;
const int NO_SOUND = -1;

//////////////////////////////////////////////////////////////////
// Macros


//////////////////////////////////////////////////////////////////
// Class Definition

class cDSound;

class cEffects
{

public: 

protected:
	FILE *fh;

private:

	BITMAPINFO_4DX bitmap_info[MAX_BITMAPS]; // effects bitmap data
		

	palette_header       *palette_headers;
	visual_effect_header *visual_effect_headers;
	sound_effect_header  *sound_effect_headers;
	int					 num_visual_effects;
	int					 num_palettes;
	int					 num_active_palettes;
	int					 num_sound_effects;

	int					 visual_effect_bytes;
	int					 sound_effect_bytes;

public:
    cEffects(); 
    ~cEffects(void);

	int  LoadEffectBitmaps(realmid_t effect_id, int caller=0);
	void FreeEffectBitmaps(realmid_t effect_id); 
	int  LoadEffectPalette(realmid_t palette_id, int caller=0);
	void FreeEffectPalette(realmid_t effect_id); 
	int  LoadSoundEffect(realmid_t effect_id, int caller=0);
	void FreeSoundEffect(realmid_t effect_id); 

	BITMAPINFO_4DX *EffectBitmap(int id);

	int EffectFrames(realmid_t effect_id); 
	int EffectViews(realmid_t effect_id); 
	int EffectHeight(realmid_t effect_id);
	int EffectWidth(realmid_t effect_id); 
	int EffectPalette(realmid_t effect_id); 
	HBITMAP Create8bppBitmapFromBits(unsigned char *bits, int w, int h, int paletteid);
	HBITMAP Create16bppBitmapFromBits(unsigned char *bits, int w, int h);
	HBITMAP CreateBitmap(int id);

	int VEBytes(void) { return visual_effect_bytes; };
	int SEBytes(void) { return sound_effect_bytes; };

private:

	void LoadPermanentPalettes(void);
	void LoadPermanentVisualEffects(void);
	void LoadPermanentSoundEffects(void);
    void LoadVisualEffect(int index, realmid_t effect_id);
	void LoadPalette(int index, realmid_t effect_id);
	void LoadSound(int index, realmid_t effect_id);

	int FindBitmapIndex(realmid_t effect_id);

#ifdef DEBUG
	void Debug_CheckInvariants(int caller);
#endif
	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cEffects(const cEffects& x);
	cEffects& operator=(const cEffects& x);

};


#endif

