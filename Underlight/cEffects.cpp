
// Copyright Lyra LLC, 1996. All rights reserved.

#define STRICT

#include "cLevel.h"
#include "cPalettes.h"
#include "cDDraw.h"
#include "cDSound.h"
#include "resource.h"
#include "cEffects.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern cDSound *cDS;
extern cDDraw *cDD;
extern cPaletteManager *shader;
extern HINSTANCE hInstance;

long g_lEffectsFileCheckSum = 0;


//////////////////////////////////////////////////////////////////
//

const TCHAR *effect_file = _T("effects.rlm");
const int DEFAULT_ILIST_SIZE = 32;
const int MAX_ACTIVE_PALETTES = 32;


// performs a normal fread and does a checksum on the data read in
static size_t checksum_fread( void *buffer, size_t size, size_t count, FILE *stream )
{
	size_t result = fread(buffer,size, count, stream);

	for (size_t i = 0; i < size*count; i++)
		g_lEffectsFileCheckSum += ((byte *)buffer)[i];
	return result;
}

/////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
cEffects::cEffects(void)
{
	memset(bitmap_info,0, sizeof bitmap_info);
	effect_file_header file_header;

	visual_effect_bytes = sound_effect_bytes = 0;
	num_active_palettes = 0;

	palette_headers = NULL;
	visual_effect_headers = NULL;
	sound_effect_headers = NULL;

	fh =_tfopen(effect_file, _T("rb"));

	if (fh == NULL)
	{
		GAME_ERROR(IDS_NO_OPEN_FFX_FILE);
		return;
	}

	if (checksum_fread(&file_header, sizeof(effect_file_header), 1, fh) != 1)
	{
		GAME_ERROR(IDS_NO_READ_FFX_HEADER);
		return;
	}

	num_palettes = file_header.num_palettes;
	num_visual_effects = file_header.num_visual_effects;
	num_sound_effects  = file_header.num_sound_effects;

	//_tprintf("visual_effects: %d \n", num_visual_effects);

	// load palette headers
	palette_headers = new palette_header[num_palettes];
	if (checksum_fread(palette_headers, sizeof(palette_header), (size_t)num_palettes, fh) != (UINT)num_palettes)
	{
		//_tprintf("num palettes: %d\n",num_palettes);
		GAME_ERROR(IDS_NO_READ_PALETTE_HEADERS);
		return;
	}

	this->LoadPermanentPalettes();

	// load visual_effect headers - 1st seek to end of palette information
	if (fseek(fh, (palette_headers[num_palettes-1].file_position+PALETTE_SIZE), SEEK_SET))
	{
		//_tprintf("num palettes: %d\n",num_palettes);
		GAME_ERROR(IDS_NO_SEEK_TO_READ_FFX_HEADERS);
		return;
	}

	visual_effect_headers = new visual_effect_header[num_visual_effects];
	if (checksum_fread(visual_effect_headers, sizeof(visual_effect_header), (size_t)num_visual_effects, fh) != (UINT)num_visual_effects)
	{
		//_tprintf("num visual_effects: %d\n",num_visual_effects);
		GAME_ERROR(IDS_NO_READ_VISUAL_FFX_HEADERS);
		return;
	}

	this->LoadPermanentVisualEffects();

	if (cDS->Initialized())
	{ // load sound effects - 1st seek to end of palette information
		if (fseek(fh, (visual_effect_headers[num_visual_effects-1].file_position+
				(visual_effect_headers[num_visual_effects-1].height*visual_effect_headers[num_visual_effects-1].width*visual_effect_headers[num_visual_effects-1].frames*
				visual_effect_headers[num_visual_effects-1].views*visual_effect_headers[num_visual_effects-1].bpp)),
				SEEK_SET))
		{
			GAME_ERROR(IDS_NO_SEEK_TO_READ_SOUNDS);
			return;
		}
		sound_effect_headers = new sound_effect_header[num_sound_effects];
		if (checksum_fread(sound_effect_headers, sizeof(sound_effect_header), (size_t)num_sound_effects, fh) != (UINT)num_sound_effects)
		{
			//_tprintf("num sound_effects: %d\n",num_sound_effects);
			GAME_ERROR(IDS_NO_READ_SOUND_FFX_HEADERS);
			return;
		}
		 this->LoadPermanentSoundEffects();
	}

	//_tprintf("done with effect load...\n",ftell(fh));

	return;
}

void cEffects::LoadPermanentPalettes(void)
{
	for (int i=0; i<num_palettes; i++)
		if (palette_headers[i].count)
			this->LoadPalette(i,palette_headers[i].id);
 }

void cEffects::LoadPermanentVisualEffects(void)
{
	for (int i=0; i<num_visual_effects; i++)
	{
		if (visual_effect_headers[i].count)
			this->LoadVisualEffect(i,visual_effect_headers[i].id);
	}
	return;
}

void cEffects::LoadPermanentSoundEffects(void)
{
	for (int i=0; i<num_sound_effects; i++)
	{
		if (sound_effect_headers[i].count)
			this->LoadSound(i,sound_effect_headers[i].id);
	}
	return;
}

void cEffects::LoadPalette(int index, realmid_t effect_id)
{
	unsigned char *spal; // temporary palette data
#ifdef AGENT
	return;
#endif

	if ((effect_id == MAGIC_AVATAR_PALETTE_2) || (effect_id == MAGIC_AGENT_PALETTE_2)) 
		return; // don't load 2nd part of magic palettes


	if (fseek(fh, palette_headers[index].file_position, SEEK_SET))
	{
			GAME_ERROR(IDS_NO_SEEK_TO_FIND_PALETTE);
		return;
	}

	if (effect_id == MAGIC_AVATAR_PALETTE_1)
	{ // read two magic palettes & combine into one
		spal = new unsigned char[PALETTE_SIZE*2];
		if ((checksum_fread(spal, PALETTE_SIZE, 1, fh)) != 1)
		{
			GAME_ERROR(IDS_NO_READ_PALETTE);
			delete spal;
			return;
		} // note - part 2 MUST be right after part 1
		if (fseek(fh, palette_headers[index+1].file_position, SEEK_SET))
		{
			GAME_ERROR(IDS_NO_SEEK_TO_FIND_PALETTE);
			return;
		}
		if ((checksum_fread((spal + PALETTE_SIZE), PALETTE_SIZE, 1, fh)) != 1)
		{
			GAME_ERROR(IDS_NO_READ_PALETTE);
			delete spal;
			return;
		} // note - part 2 MUST be right after part 1
		// add double size palette
		 shader->AddPalette(spal, effect_id, PALETTE_SIZE*2);
	}
	else if (effect_id == MAGIC_AGENT_PALETTE_1)
	{ // read two magic palettes & combine into one
		spal = new unsigned char[PALETTE_SIZE*2];
		if ((checksum_fread(spal, PALETTE_SIZE, 1, fh)) != 1)
		{
			GAME_ERROR(IDS_NO_READ_PALETTE);
			delete spal;
			return;
		} // note - part 2 MUST be right after part 1
		if (fseek(fh, palette_headers[index+1].file_position, SEEK_SET))
		{
			GAME_ERROR(IDS_NO_SEEK_TO_FIND_PALETTE);
			return;
		}
		if ((checksum_fread((spal + PALETTE_SIZE), PALETTE_SIZE, 1, fh)) != 1)
		{
			GAME_ERROR(IDS_NO_READ_PALETTE);
			delete spal;
			return;
		} // note - part 2 MUST be right after part 1
		// add double size palette
		 shader->AddPalette(spal, effect_id, PALETTE_SIZE*2);
	}
	else
	{	// reading palette of normal size
		spal = new unsigned char[PALETTE_SIZE];

	//_tprintf("reading visual effect %d\n",i);
		if ((checksum_fread(spal, PALETTE_SIZE, 1, fh)) != 1)
		{
			GAME_ERROR(IDS_NO_READ_PALETTE);
			delete spal;
			return;
		}
		 shader->AddPalette(spal, effect_id);
	}
	// the palette id created here is also used as the index for the pm
	delete spal;
	return;
}


void cEffects::LoadVisualEffect(int index, realmid_t effect_id)
{
	int j,views,frames,height,width,bpp,palette,rle;
	int total_size, bm_index = effect_id;
	unsigned char *effect_bits=NULL,*dst,*src;

#ifndef AGENT
	int pixels, frame_size;
	size_t count;
#endif

	views = visual_effect_headers[index].views;
	frames = visual_effect_headers[index].frames;
	height = visual_effect_headers[index].height;
	width = visual_effect_headers[index].width;
	bpp = visual_effect_headers[index].bpp;
	palette = visual_effect_headers[index].palette;
	rle = visual_effect_headers[index].rle;
	total_size = visual_effect_headers[index].size;


#ifndef AGENT
	effect_bits = new unsigned char[total_size];
	visual_effect_bytes += total_size;
	//_tprintf("seeking to %d...\n",visual_effect_headers[index].file_position);
	if (fseek(fh, visual_effect_headers[index].file_position, SEEK_SET))
	{
		GAME_ERROR(IDS_NO_SEEK_TO_FIND_VISUAL_FFX);
		return;
	}

	// read the bits for this entire chunk
	count = checksum_fread(effect_bits, total_size, 1, fh);
	if (count != 1)
	{
		LoadString(hInstance, IDS_EFFECT_ERR1, temp_message, sizeof(temp_message));
		_stprintf(errbuf, temp_message, (height * width * views * frames), index, bm_index, height, width, views, frames);
		_tprintf(_T("%s"), errbuf);
		GAME_ERROR(IDS_NO_READ_VISUAL_FFX_BITS);
		return;
	}

	pixels = total_size/bpp;

	// convert to 555 if bpp=2 and we are running in 555
	if (bpp == 2 && cDD->PixelFormat() == PIXEL_FORMAT_555)
	{
		//_tprintf("converting bitmap\n");
		PIXEL *chunk_ptr = (PIXEL *)effect_bits;
		while(pixels--)
		{
			int red_green = (*chunk_ptr >> 1) & 0x0000FFE0 ;
			int blue 	  =  *chunk_ptr		 & 0x0000001F ;
			*chunk_ptr++  = red_green | blue;
		}
	}

#endif
	src = effect_bits;
	 for (j=0; j<frames*views; j++)
	{	// set dst as the appropriate offset into the effect bitmap space
#ifdef AGENT
		dst = NULL;
#else
		if (rle)
		{
			int  size = *((int*)src); // get size of compressed frame
			src += sizeof size;		  // move onto real data
			size *= bpp;
			dst = new unsigned char[size];
			memcpy(dst, src, size);
			frame_size = size;
		}
		 else
	  {
			frame_size = height*width*bpp;
			dst = new unsigned char[frame_size];
			memcpy(dst, src, frame_size);
		 }
		src += frame_size;
#endif
		bitmap_info[bm_index].address = dst;
		bitmap_info[bm_index].h = width;
		bitmap_info[bm_index].w = height;
		bitmap_info[bm_index].palette = palette; // -1 for color tables
		bm_index++;
	}
#ifndef AGENT
	delete effect_bits;
#endif
	return;
}


void cEffects::LoadSound(int index, realmid_t effect_id)
{
#ifndef AGENT
	int id,count;
	WAVEFORMATEX sound_format;
	int data_size,format_size;
	void *sound_data;

	format_size = sound_effect_headers[index].format_length;
	data_size = sound_effect_headers[index].data_length;
	id = sound_effect_headers[index].id;

	sound_data = new char[data_size];
	sound_effect_bytes += sound_effect_headers[index].data_length;

	if (fseek(fh, sound_effect_headers[index].file_position, SEEK_SET))
	{
		GAME_ERROR(IDS_NO_SEEK_TO_FIND_SOUND_FFX);
		return;
	}

	//_tprintf("reading %d bits for sound %d at file pos %d\n",data_size,id,sound_effect_headers[index].file_position);
	// read the bits for this entire chunk
	count = checksum_fread(&sound_format, format_size, 1, fh);
	if (count != 1)
	{
		GAME_ERROR(IDS_NO_READ_SOUND_FFX_FORMAT);
		return;
	}

	count = checksum_fread(sound_data, data_size, 1, fh);
	if (count != 1)
	{
		GAME_ERROR(IDS_NO_READ_SOUND_FFX_BITS);
		return;
	}

	cDS->CreateSoundBuffer(id, sound_format, data_size, sound_data);
	delete [] sound_data;
#endif
	return;
}


int cEffects::LoadEffectPalette(realmid_t palette_id, int caller)
{
	for (int i=0; i<num_palettes; i++)
		if (palette_headers[i].id == palette_id)
		{
			// allocate and load, if needed
			if (!(palette_headers[i].count))
			{
				num_active_palettes++;
				if (num_active_palettes >= MAX_ACTIVE_PALETTES)
				{
					GAME_ERROR(IDS_LOAD_TOO_MANY_PALETTES);
					return NO_PALETTE;
				}

				this->LoadPalette(i,palette_id);
			}
			palette_headers[i].count++;
			return palette_id;
		}
	LoadString(hInstance, IDS_EFFECT_ERR2, temp_message, sizeof(temp_message));
	_stprintf(errbuf, temp_message, palette_id, caller);
	NONFATAL_ERROR(errbuf);
	return NO_PALETTE;
}

void cEffects::FreeEffectPalette(realmid_t palette_id)
{
	for (int i=0; i<num_palettes; i++)
		if (palette_headers[i].id == palette_id)
		{
			palette_headers[i].count--;
#ifndef AGENT
			if (!(palette_headers[i].count))
			{
				num_active_palettes--;
				shader->FreePalette(palette_id);
			}
#endif
			return;
		}

	NONFATAL_ERROR(IDS_EFFECT_ERR3);
	return;
}

int cEffects::LoadEffectBitmaps(realmid_t effect_id, int caller)
{	// HACK: look up by looping through
	for (int i=0; i<num_visual_effects; i++)
		if (visual_effect_headers[i].id == effect_id)
		{
		///_tprintf("found match for load at %d!\n",i);
			// allocate and load, if needed
			if (!(visual_effect_headers[i].count))
				this->LoadVisualEffect(i,effect_id);
			visual_effect_headers[i].count++;
			return effect_id;
		}
	LoadString(hInstance, IDS_EFFECT_ERR4, temp_message, sizeof(temp_message));
	_stprintf(errbuf, temp_message,effect_id, caller);
	NONFATAL_ERROR(errbuf);
	return NO_BITMAP;
 }


void cEffects::FreeEffectBitmaps(realmid_t effect_id)
{
	for (int i=0; i<num_visual_effects; i++)
		if (visual_effect_headers[i].id == effect_id)
		{// deallocate the RAM, if needed
			visual_effect_headers[i].count--;
#ifndef AGENT
			if (!(visual_effect_headers[i].count))
			{
				visual_effect_bytes -= (visual_effect_headers[i].size);
				for (int j=0; j<(visual_effect_headers[i].frames * visual_effect_headers[i].views); j++)
					if (bitmap_info[effect_id + j].address)
						delete [] bitmap_info[effect_id + j].address;
			}
#endif
			return;
		}
	NONFATAL_ERROR(IDS_EFFECT_ERR8);
}

int cEffects::LoadSoundEffect(realmid_t effect_id, int caller)
{
	for (int i=0; i<num_sound_effects; i++)
		if (sound_effect_headers[i].id == effect_id)
		{
			// allocate and load, if needed
			if (!(sound_effect_headers[i].count))
				this->LoadSound(i,effect_id);
			sound_effect_headers[i].count++;
			return effect_id;
		}
	LoadString(hInstance, IDS_EFFECT_ERR5, temp_message, sizeof(temp_message));
	_stprintf(errbuf, temp_message, effect_id, caller);
	NONFATAL_ERROR(errbuf);
	return NO_SOUND;
}

void cEffects::FreeSoundEffect(realmid_t effect_id)
{
	for (int i=0; i<num_sound_effects; i++)
		if (sound_effect_headers[i].id == effect_id)
		{	// have cDS free,if needed
#ifndef AGENT
			if (!(sound_effect_headers[i].count))
			{
				sound_effect_bytes -= sound_effect_headers[i].data_length;
				cDS->ReleaseBuffer(sound_effect_headers[i].id);
			}
#endif
			return;
		}
	return;
}

// Look up index in headers based on id; prints error message
// if not found.
int cEffects::FindBitmapIndex(realmid_t effect_id)
{
	for (int i=0; i<num_visual_effects; i++)
		if (visual_effect_headers[i].id == effect_id)
			return i;
#ifdef UL_DEBUG //4/12/03 MDA: temporary fix for forge bug.
	LoadString(hInstance, IDS_EFFECT_ERR6, temp_message, sizeof(temp_message));
	_stprintf(errbuf, temp_message,effect_id);
	NONFATAL_ERROR(errbuf);
#endif

	return NO_BITMAP;
}
	// selectors for visual effects

BITMAPINFO_4DX *cEffects::EffectBitmap(int effect_id)
{
	BITMAPINFO_4DX *bitmap = &bitmap_info[effect_id];
	if (bitmap == NULL || bitmap->w == 0 || bitmap->h == 0)
	{
		LoadString(hInstance, IDS_EFFECT_ERR7, temp_message, sizeof(temp_message));
		_stprintf(errbuf, temp_message,effect_id);
		GAME_ERROR(errbuf);
	}
	return bitmap;
}


int cEffects::EffectFrames(realmid_t effect_id)
{
	int i;
	if ((i = FindBitmapIndex(effect_id)) != NO_BITMAP)
		return visual_effect_headers[i].frames;
	else
		return 0;
};

int cEffects::EffectViews(realmid_t effect_id)
{
	int i;
	if ((i = FindBitmapIndex(effect_id)) != NO_BITMAP)
		return visual_effect_headers[i].views;
	else
		return 0;
};

int cEffects::EffectHeight(realmid_t effect_id)
{
	int i;
	if ((i = FindBitmapIndex(effect_id)) != NO_BITMAP)
		return visual_effect_headers[i].height;
	else
		return 0;
};

int cEffects::EffectWidth(realmid_t effect_id)
{
	int i;
	if ((i = FindBitmapIndex(effect_id)) != NO_BITMAP)
		return visual_effect_headers[i].width;
	else
		return 0;
};

int cEffects::EffectPalette(realmid_t effect_id)
{
	int i;
	if ((i = FindBitmapIndex(effect_id)) != NO_BITMAP)
		return visual_effect_headers[i].palette;
	else
		return 0;
};

// Destructor
cEffects::~cEffects(void)
{
	if (fh)
		fclose(fh);

	//_tprintf("before free - VE bytes: %d SE bytes: %d\n",visual_effect_bytes, sound_effect_bytes);

#ifndef AGENT
	// deallocate visual effect RAM
	for (int i=0; i<num_visual_effects; i++)
		if (visual_effect_headers[i].count)
		{	// set count to 1 to force free
			visual_effect_headers[i].count = 1;
			this->FreeEffectBitmaps(visual_effect_headers[i].id);
		}
#endif

	delete [] visual_effect_headers;
	delete [] palette_headers;
	if (sound_effect_headers)
		delete [] sound_effect_headers;

	return;
}


// Check invariants

#ifdef DEBUG

void cEffects::Debug_CheckInvariants(int caller)
{
	return;
}

#endif
