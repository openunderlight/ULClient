// EffectMaker: converts and packs Effect pieces into
// one giant file.

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT
#define RLE

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "cGif.h"
#include "cBmp.h"
#include "effects.h"
#include "cWave.h"
#include "RLE.h"

//////////////////////////////////////////////////////////////////
// Constants


const int MAX_EFFECT_FRAMES = 25;
const int MAX_EFFECT_VIEWS = 10;
const int MAX_FILENAME = 512;
const int NUM_REQUIRED_FIELDS = 5;

const char outfile[] = "Effects.rlm";
const char palette_asc_file[] = "Palettes.asc";
const char visual_asc_file[] = "Visual Effects.asc";
const char sound_asc_file[] = "Sound Effects.asc";

#define printf DebugOut

const int NUM_PATCHPOINTS = 10;
//////////////////////////////////////////////////////////////////
// Structures

struct size_struct
{
	short			height;
	short			width;
};

struct patch_point
{
	BYTE col; 
	BYTE row;
};

//////////////////////////////////////////////////////////////////
// Function Prototypes

static int WritePalettes(FILE *fh);
static int WriteVisualEffects(FILE *fh);
static int WriteSoundEffects(FILE *fh);
static int FlipAndEncode(unsigned char *dst, unsigned char *src, int w, int h, 
												 void *extra_info = NULL, int extra_size=0);
static int LoadAndConcatImages(char *filename, unsigned char *buffer, size_struct *file_size, 
					        							int frames, int views,  BOOL flip, int bpp, bool avatar_bitmap);

static void ExtractAvatarPatchPoints(const char *filepath, patch_point patch_points[]);

void __cdecl DebugOut(char *args,...);

/////////////////////////////////////////////////
// Global Variables


/////////////////////////////////////////////////
// Functions


int PASCAL WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
  //MSG         msg;
	effect_file_header header;

	// open effect maker output file
	FILE *fh = fopen(outfile, "wb");
	if (fh == NULL)
	{
	  MessageBox(NULL, "Can't open output file", "Oops!", MB_OK);
	  return 0;
	}

	// make space for header at start of file
	fseek(fh,sizeof(header),SEEK_SET);

	// write out the palettes
	header.num_palettes = WritePalettes(fh);
	if (header.num_palettes < 0)
	   return 0;

    header.num_visual_effects = WriteVisualEffects(fh);
	if (header.num_visual_effects < 0)
	   return 0;
	 
	// go to end of file and write out the sound effects
	fseek(fh,0,SEEK_END);
	//printf("fpos: %d\n",ftell(fh));
    header.num_sound_effects = WriteSoundEffects(fh);
	if (header.num_sound_effects < 0)
	   return 0;

  // Go to start of file and write file header
	fseek(fh,0,SEEK_SET);
	if (fwrite(&header, sizeof(header), 1, fh)!= 1)
	{		
		MessageBox(NULL, "Error writing header", "Oops!", MB_OK);
		return 0;
	}

  fclose(fh);
  MessageBox(NULL, "EffectMaker Finished", "Cool", MB_OK);
//  return msg.wParam;
	return 0;
   
}

const int PALETTE_FILE_SIZE=9000;


int WritePalettes(FILE *fh)
{
	int num_palettes=0,file_pos,counts[MAX_PALETTES],ids[MAX_PALETTES],i,j;
	int r,g,b;
	long file_start;
	unsigned char pal_data[PALETTE_SIZE];
	palette_header *palette_headers;
	FILE *pal, *asc;
	char *p, c;
	char filenames[MAX_PALETTES][MAX_FILENAME];
	char ASCII_data[PALETTE_FILE_SIZE];
	bool assigned[MAX_PALETTES];

	for (i=0; i<MAX_PALETTES; i++)
		assigned[i] = false;

    file_start = ftell(fh);
    file_pos = file_start;

	// open ASCII input file
	asc = fopen(palette_asc_file, "rb");
	if (asc == NULL)
	{
		MessageBox(NULL, "Can't open Palettes ASC file", "Oops!", MB_OK);
		return -1;
	}
	// 1st, count the # of palettes
	while (1)
	{
		if (fscanf(asc, "%s", filenames[num_palettes]) == EOF)
			break;

		if (filenames[num_palettes][0] == '#')
		{  // ignore comments - read until end of line
			c = ' ';
			while (c != '\n')
				fread(&c, 1, 1, asc);
			continue;
		}
		else 
		{
			if (num_palettes == MAX_PALETTES)
			{
				MessageBox(NULL, "Too many palettes!", "Oops!", MB_OK);
				return -1;
			}
			fscanf(asc, "%d %d", &(counts[num_palettes]), &(ids[num_palettes]));
			num_palettes++;
		}

	}
	printf("num palettes: %d\n",num_palettes);
	fclose(asc);

	// Write palette headers - this is a dummy for now because it will be filled in an rewritten later.
	palette_headers = new palette_header[num_palettes];
	if (fwrite(palette_headers, sizeof(palette_header), num_palettes, fh) != (UINT)num_palettes)
	{		
		MessageBox(NULL, "Error writing dummy palette headers", "Oops!", MB_OK);
		return -1;
	}
	file_pos += sizeof(palette_header)*num_palettes;

	for (i=0; i < num_palettes; i++)
	{
	    strcat(filenames[i],".pal");	
		printf("Processing palette %s...\n",filenames[i]);

		pal = fopen(filenames[i], "rb");
		if (pal == NULL)
		{
			MessageBox(NULL, "Can't open palette file!", "Oops!", MB_OK);
			return -1;
		}	
	    fread(ASCII_data, PALETTE_FILE_SIZE, 1, pal);
		fclose(pal);

		p = strchr(ASCII_data,'\n');  // skip header info
		p = strchr(++p,'\n');             //        version
				
		for (j = 0; j < 256; j++)
		{
 			p = strchr(++p,'\n');
			if (j == 255)
				p = p;
			sscanf(p,"%d %d %d", &r, &g, &b);
			pal_data[j*3] = (unsigned char)r;
			pal_data[j*3+1] = (unsigned char)g;
			pal_data[j*3+2] = (unsigned char)b;
		}
		
		if (fwrite(pal_data, PALETTE_SIZE, 1, fh) != 1)
		{		
			MessageBox(NULL, "Error writing palette!", "Oops!", MB_OK);
			return -1;
		}
		palette_headers[i].file_position = file_pos;
		palette_headers[i].count = counts[i];
		palette_headers[i].id = ids[i];
		if (assigned[ids[i]])
		{
			char message[512];
			sprintf(message, "Error: palette %d used twice!\n", ids[i]);
			printf("%s\n", message);
			MessageBox(NULL, message, "Oops!", MB_OK);
			return -1;

		}
		assigned[ids[i]] = true;
		file_pos += PALETTE_SIZE;
	}

	if (i != num_palettes)
	{		
		MessageBox(NULL, "Error - wrong number of palettes", "Oops!", MB_OK);
		return -1;
	}
	
	// now write out the file headers
	if (fseek(fh, file_start, SEEK_SET))
	{
		MessageBox(NULL, "Error seeking to rewrite palette headers", "Oops!", MB_OK);
		return -1;
	}

	if (fwrite(palette_headers, sizeof(palette_header), num_palettes, fh) != (UINT) num_palettes)
	{		
		MessageBox(NULL, "Error writing real palette headers", "Oops!", MB_OK);
		return -1;
	}

	if (fseek(fh, 0, SEEK_END))
	{		
		MessageBox(NULL, "Error seeking to end of palettes", "Oops!", MB_OK);
		return -1;
	}

	delete [] palette_headers;

    return num_palettes;
}

int WriteVisualEffects(FILE *fh)
{
	int  views, frames, count, id, i, bpp ;
	char filename[MAX_FILENAME];
	char c;
	int file_pos;
	long file_start;
	int num_effects = 0;
	short palette;
	visual_effect_header *visual_effect_headers;
	unsigned char *big_buffer = new unsigned char[2000000]; // should be enough...
	unsigned char *small_buffer = new unsigned char[1024];
	size_struct file_size;
	bool flip;
	bool assigned[MAX_BITMAPS];
	int fields;
	bool avatar_bitmap=false;

	for (i=0; i<MAX_BITMAPS; i++)
		assigned[i] = false;

    file_start = ftell(fh);
    file_pos = file_start;

	// open ASCII input file
	FILE *asc = fopen(visual_asc_file, "rb");
	if (asc == NULL)
	{
		MessageBox(NULL, "Can't open Visual Effects ASC file", "Oops!", MB_OK);
		return -1;
	}

	// count the effects
	while (asc)
	{
		if (fscanf(asc, "%s", filename) == EOF)
			break;
		if (filename[0] == '#')
		{
			c = ' ';
			while (c != '\n')
				fread(&c, 1, 1, asc);
			continue;
		}
		else
		{	// ignore comments & lines with id=frames=views=0
			fscanf(asc, "%d %d %d %u %d", &views, &frames, &count, &id, &palette);
			if (id || views || frames)
			{
				if (num_effects == MAX_BITMAPS)
				{
					MessageBox(NULL, "Too many visual effect bitmaps!", "Oops!", MB_OK);
					return -1;
				}
				num_effects++;
			}
		}
	}
	printf("num effects: %d\n",num_effects);
	fseek(asc, 0, SEEK_SET); // back to the beginning...

	// Write effects header - this is a dummy for now because it will be filled in an rewritten later.
	visual_effect_headers = new visual_effect_header[num_effects];
	if (fwrite(visual_effect_headers, sizeof(visual_effect_header), num_effects, fh) != (UINT)num_effects)
	{		
		MessageBox(NULL, "Error writing dummy effect headers", "Oops!", MB_OK);
		return -1;
	}
	file_pos += sizeof(visual_effect_header)*num_effects;

	// go through and do all the effects
	 for (i=0; i < num_effects; i++)
	{
		if (fscanf(asc, "%s", filename) == EOF)
			break;
		if (filename[0] == '#')
		{  // ignore comments - read until end of line
			c = ' ';
			while (c != '\n')
				fread(&c, 1, 1, asc);
			i--;
			continue;
		}
		else
			fields = fscanf(asc, "%d %d %d %u %d", &views, &frames, &count, &id, &palette);

		printf("Processing effect %s; views = %d, frames = %d, count = %d, id = %d, pal = %d\n", 
			filename, views, frames, count, id, palette);

		if (fields != NUM_REQUIRED_FIELDS)
		{
			char message[512];
			sprintf(message, "Incorrect number of fields for %s", filename);
			printf("%s\n", message);
			MessageBox(NULL, message, "Oops!", MB_OK);
			return -1;
		}
		
		if (frames > MAX_EFFECT_FRAMES || views > MAX_EFFECT_VIEWS)
		{
			char message[512];
			sprintf(message, "Too many frames or views for file %s", filename);
			printf("%s\n", message);
			MessageBox(NULL, message, "Oops!", MB_OK);
			return -1;
		}

		if (id < 0 || id > MAX_BITMAPS)
		{
			char message[512];
			sprintf(message, "Bitmap ID out of range for file:\n '%s'", filename);
			printf("%s\n", message);
			MessageBox(NULL, message, "Oops!", MB_OK);
			return -1;
		}

			if (assigned[id])
		{
			char message[512];
			sprintf(message, "Error: bitmap id %d used twice!\n", id);
			printf("%s\n", message);
			MessageBox(NULL, message, "Oops!", MB_OK);
			return -1;

		}


		flip = true;
		bpp = 1;
		avatar_bitmap = false;

		// if name ends in a *, don't flip...
		if (filename[strlen(filename)-1] == '*')
		{
			filename[strlen(filename)-1] = '\0';
			flip = false;
		} // if name ends in ?, it's a 16 bit bmp
		else if (filename[strlen(filename)-1] == '?')
		{
			filename[strlen(filename)-1] = '\0';
			bpp = 2;
		}	 // if name ends in &, it's an avatar bitmap 
		else if (filename[strlen(filename)-1] == '&')
		{
			filename[strlen(filename)-1] = '\0';
			avatar_bitmap = true;
		}


		//printf("nm: %s views: %d frames: %d bpp: %d\n",filename,views,frames,bpp);

		assigned[id] = true;

		int len = LoadAndConcatImages(filename, big_buffer, &file_size, frames, views, flip, 
																	bpp,avatar_bitmap);
		visual_effect_headers[i].file_position = file_pos;
		visual_effect_headers[i].height        = file_size.height;
		visual_effect_headers[i].width         = file_size.width;
		visual_effect_headers[i].views         = views;
		visual_effect_headers[i].frames        = frames;
		visual_effect_headers[i].count         = count;
		visual_effect_headers[i].bpp	       = bpp;
		visual_effect_headers[i].id            = id;
		visual_effect_headers[i].palette       = palette;
		visual_effect_headers[i].size		   = len;
#ifdef RLE
		if ((bpp == 1) && flip)
			visual_effect_headers[i].rle	    = 1;
		else // 1 bpp flipped bitmaps are rle encoded
#endif
			visual_effect_headers[i].rle	    = 0;
				
		if (fwrite(big_buffer, len, 1, fh)  != 1)
		{		
			MessageBox(NULL, "Error writing effect bits", "Oops!", MB_OK);
			return -1;
		}
	 //	printf("writing %d bits at position %d for effects %s\n",len,file_pos,filename);
		file_pos += len;		
	}
	
	if (i != num_effects)
	{		
		MessageBox(NULL, "Error - wrong number of effects", "Oops!", MB_OK);
		return -1;
	}
	
	// now write out the file headers
	if (fseek(fh, file_start, SEEK_SET))
	{
		MessageBox(NULL, "Error seeking to rewrite effect headers", "Oops!", MB_OK);
		return -1;
	}

	if (fwrite(visual_effect_headers, sizeof(visual_effect_header), num_effects, fh) != (UINT) num_effects)
	{		
		MessageBox(NULL, "Error writing real effect headers", "Oops!", MB_OK);
		return -1;
	}
/*	for (i=0; i<num_effects; i++)
	{
		printf("effect header %d: pos = %u w = %d h= %d views = %d frames =%d perm = %d id = %d\n",i, 
				visual_effect_headers[i].file_position, visual_effect_headers[i].width, 
				visual_effect_headers[i].height, visual_effect_headers[i].views,
				visual_effect_headers[i].frames, visual_effect_headers[i].count,
				visual_effect_headers[i].id);
	}
  */

	fclose(asc);

	//printf("pos: %d size: %d\n",visual_effect_headers[num_effects-1].file_position,
				//visual_effect_headers[num_effects-1].height*visual_effect_headers[num_effects-1].width*visual_effect_headers[num_effects-1].frames*visual_effect_headers[num_effects-1].views*2);

	delete [] visual_effect_headers;
	delete [] big_buffer;
	delete [] small_buffer;
	
	return num_effects;
}

int WriteSoundEffects(FILE *fh)
{
	int num_effects=0,file_pos,counts[MAX_SOUNDS],ids[MAX_SOUNDS],i;
	char c;
	char filename[MAX_SOUNDS][MAX_FILENAME];
	long file_start;
	sound_effect_header *sound_effect_headers;
	FILE *asc;
	cWave *wave;
	bool assigned[MAX_SOUNDS];

	for (i=0; i<MAX_SOUNDS; i++)
		assigned[i] = false;


    file_start = ftell(fh);
    file_pos = file_start;

	// open ASCII input file
	asc = fopen(sound_asc_file, "rb");
	if (asc == NULL)
	{
		MessageBox(NULL, "Can't open Sound Effects ASC file", "Oops!", MB_OK);
		return -1;
	}
	while (1)
	{		
		if (fscanf(asc, "%s", filename[num_effects]) == EOF)
			break;
		if (filename[num_effects][0] == '#')
		{
			c = ' ';
			while (c != '\n')
				fread(&c, 1, 1, asc);
			continue;
		}
		else 
		{
			if (num_effects == MAX_SOUNDS)
			{
				MessageBox(NULL, "Too many sound effects!", "Oops!", MB_OK);
				return -1;
			}
			fscanf(asc, "%d %d", &(counts[num_effects]), &(ids[num_effects]));
			num_effects++;
		}
	}
	printf("num sound effects: %d\n",num_effects);
	fclose(asc);

	// Write sound_effect headers - this is a dummy for now because it will be filled in an rewritten later.
	sound_effect_headers = new sound_effect_header[num_effects];
	if (fwrite(sound_effect_headers, sizeof(sound_effect_header), num_effects, fh) != (UINT)num_effects)
	{		
		MessageBox(NULL, "Error writing dummy sound effect headers", "Oops!", MB_OK);
		return -1;
	}
	file_pos += sizeof(sound_effect_header)*num_effects;

	for (i=0; i < num_effects; i++)
	{
        strcat(filename[i],".wav");	
		wave = new cWave(filename[i]);
		printf("processing sound effect %s\n",filename[i]);
		
		if (!wave->WaveOK())
		{
			MessageBox(NULL, "Error loading sound effect bits", "Oops", MB_OK);
			return -1;
		}
	
		if (assigned[ids[i]])
		{
			char message[512];
			sprintf(message, "Error: sound id %d used twice!\n", ids[i]);
			printf("%s\n", message);
			MessageBox(NULL, message, "Oops!", MB_OK);
			return -1;

		}
		assigned[ids[i]] = true;

		sound_effect_headers[i].file_position = file_pos;
		sound_effect_headers[i].format_length = sizeof(WAVEFORMATEX);
		sound_effect_headers[i].data_length = wave->GetWaveSize();
		sound_effect_headers[i].count = counts[i];
		sound_effect_headers[i].id = ids[i];

		//printf("sound effect %d, id %d, count %d, format len %d, data len %d\n", i, sound_effect_headers[i].id , sound_effect_headers[i].count, sound_effect_headers[i].format_length, sound_effect_headers[i].data_length);

		if (fwrite((void*)(wave->GetWaveFormatPtr()), sizeof(WAVEFORMATEX), 1, fh) != 1)
		{
			MessageBox(NULL, "Cannot write sound effect format data", "Oops!", MB_OK);
			return -1;
  		}

		if (fwrite((void*)(wave->GetWaveDataPtr()), wave->GetWaveSize(), 1, fh) != 1)
		{
			MessageBox(NULL, "Cannot write sound effect data", "Oops!", MB_OK);
			return -1;
  		}

		delete wave;

		file_pos += sound_effect_headers[i].format_length + sound_effect_headers[i].data_length;
	}

	if (i != num_effects)
	{		
		MessageBox(NULL, "Error - wrong number of sound effects", "Oops!", MB_OK);
		return -1;
	}
	
	// now write out the file headers
	if (fseek(fh, file_start, SEEK_SET))
	{
		MessageBox(NULL, "Error seeking to rewrite sound effect headers", "Oops!", MB_OK);
		return -1;
	}

	if (fwrite(sound_effect_headers, sizeof(sound_effect_header), num_effects, fh) != (UINT) num_effects)
	{		
		MessageBox(NULL, "Error writing real sound effect headers", "Oops!", MB_OK);
		return -1;
	}

	if (fseek(fh, 0, SEEK_END))
	{		
		MessageBox(NULL, "Error seeking to end of sound effects", "Oops!", MB_OK);
		return -1;
	}

	delete [] sound_effect_headers;

	return num_effects;
}



// Flips a bitmap sideways and mirrors it for the renderer.
// Note that with the modular Items it is also necessary
// to do some fancy footwork at load time; see cEffects.cpp
// in the Underlight project. 

const int MAX_RUN = 255;

// This also RLE encodes the bitmap.
static int FlipAndEncode(unsigned char *dst, unsigned char *src, int w, int h, 
												 void *extra_info , int extra_size)
{
	unsigned char *s,*t,*temp;
    int width,height;

	temp = new unsigned char[w*h]; // temp buffer

	s  = src;
	t = temp; // b is a temp pointer to the buffer temp

    // now flip it to 90 degrees counter clockwise, and mirror it,
    // so that the renderer can get it sideways.
    for(height=0;height<h;height++)
    {
        t = temp+height;
        for(width=0;width<w;width++)
        {
           *t=*s;
           s++;
           t+=h;
        }
    }
#ifndef RLE // no rle
	memcpy(dst, temp, width*height);
	return (width*height);
#endif

		
	// now the bitmap has been flipped; rle encode it,and add any extra info
	int size = RLE_encode(dst+sizeof(size),temp,h,w,extra_info,extra_size);

	
	// save size of encoded bitmap
	(*(int *)dst) = size;

	delete temp;
	return (size+sizeof(size));
}


// Load in the bitmaps for a visual effect. 
int LoadAndConcatImages(char *filename, unsigned char *buffer, size_struct *file_size, int frames, int views, 
												BOOL flip, int bpp, bool avatar_bitmap)
{
	int j,k;
	char file[132];
	char errmsg[132];
	cGif *gif;
	cBmp *bmp;
	unsigned char *src, *dst;
	int width=0; 
	int height=0;
	int frame_size, total_size=0;
	dst = buffer;

	for (j=0; j<frames; j++)
	{
		for (k=0; k<views; k++)
		{
			if (bpp == 1)
			{
				wsprintf(file,"%s%c%c.gif",filename,'a'+ j,'0' + k);
				gif = new cGif(file);
				//printf("gif file name: %s wd: %d ht: %d\n",file,gif->Wd(),gif->Ht());
				if ((gif->Wd() == 0) || (gif->Ht() == 0))
				{
					wsprintf(errmsg, "MISSING GIF FILE: %s\n",file);
					MessageBox(NULL, errmsg, "ERROR", MB_OK);
					delete gif;
					continue;
				}
				if (width == 0)
				{ // set width/height
					width = file_size->width = gif->Wd();
					height = file_size->height = gif->Ht();
				} // otherwise check to ensure consistency
				else if (gif->Wd() != width)
				{
					wsprintf(errmsg, "INVALID WIDTH: %s width = %d correct width = %d\n",file,gif->Wd(),width);
					MessageBox(NULL, errmsg, "ERROR", MB_OK);
				}
				if (gif->Ht() != height)
				{
					wsprintf(errmsg, "INVALID HEIGHT: %s height = %d correct height = %d\n",file,gif->Ht(),height);
					MessageBox(NULL, errmsg, "ERROR", MB_OK);
				}
				src = gif->Address();
				if (flip) // flip & encode
				{
					if (avatar_bitmap)
					{ 
						patch_point points[NUM_PATCHPOINTS];
						ExtractAvatarPatchPoints(file, points);
						frame_size = FlipAndEncode(dst, src, width, height,points,sizeof points);
					}
					else
						frame_size = FlipAndEncode(dst, src, width, height);
				//	printf("frame of size %d compressed to %d\n",width*height, frame_size);
				}
				else // don't flip
				{
					frame_size = width*height;
					memcpy(dst, src, width*height);
				}
				dst += frame_size;
				total_size += frame_size;
				delete gif;
			}
			else if (bpp == 2)
			{
				if ((frames != 1) || (views != 1))
					MessageBox(NULL, "Hi Color bitmaps must be single-view, single-frame", "Oops", MB_OK);
				sprintf(file,"%s.bmp",filename);
				bmp = new cBmp(file);
				if ((bmp->Wd() == 0) || (bmp->Ht() == 0))
				{
					wsprintf(errmsg, "MISSING BMP FILE: %s\n",file);
					MessageBox(NULL, errmsg, "ERROR", MB_OK);
					delete bmp;
					continue;
				}
				width = file_size->width = bmp->Wd();
				height = file_size->height = bmp->Ht();
				//printf("bitmap file name: %s wd: %d ht: %d\n",file,bmp->Wd(),bmp->Ht());
				src = bmp->Address();
				memcpy(dst, src, width*height*bpp);
				frame_size = width*height*bpp;
				dst += frame_size;
				total_size += frame_size;
				delete bmp;
			}
		}
	}
	//printf("bitmaps of size %d compressed to %d\n", width*height*bpp*frames*views, total_size);
	return total_size;
}


static void ExtractAvatarPatchPoints(const char *filepath, patch_point patch_points[])
{
	struct 
	{
	int row_total;
	int col_total;
	int num_points;
	} points[NUM_PATCHPOINTS];


	char dir  [_MAX_DIR];
	char fname[_MAX_FNAME];
	char file [_MAX_PATH];
	
	memset(patch_points,0, sizeof patch_point * NUM_PATCHPOINTS);
	memset(points,0, sizeof points);


	// create patch points file name from corrosponding file name; 
	// <avatar_dir>\PatchPoints\<avatar_file>
	_splitpath( filepath, NULL, dir, fname, NULL );
	sprintf(file,"%s%s%s.gif",dir,"PatchPoints/",fname);

	// open gif file
	cGif gif(file);
	
	if ((gif.Wd() == 0) || (gif.Ht() == 0))  // if gif could not be opened
	{
		char errmsg[132];
		wsprintf(errmsg, "MISSING GIF FILE: %s\n",file);
		//MessageBox(NULL, errmsg, "ERROR", MB_OK);
		printf(errmsg);
		return;
	}

	// search gif for patch points
	unsigned char *src = gif.Address();
	int width = gif.Wd();
	for(int i  = 0; i < width * gif.Ht(); i++)
	{
		unsigned char c = src[i];
		if (c >= 1 && c <= NUM_PATCHPOINTS)       // patch points occupy this range in palette
		{
			c--;
			//patch_points[c].col = i%width;
			//patch_points[c].row = i/width;
			points[c].col_total += i%width;   // accumulate row and col positions
			points[c].row_total += i/width;  
			points[c].num_points ++;
		}
	}

	// average out totals to get actual points
	for(i = 0; i < NUM_PATCHPOINTS; i++)
	{
		if (points[i].num_points)
		{
			points[i].num_points *= 4; //patchpoint bitmaps are 4 times actual bitmap size
			patch_points[i].col = points[i].col_total/points[i].num_points;
			patch_points[i].row = points[i].row_total/points[i].num_points;
		}
	}
}


void __cdecl DebugOut(char *args,...)
{
   char DebugBuffer[256];
   FILE *out;
   va_list ap;
   va_start(ap,args);
   vsprintf(DebugBuffer,args,ap);
   va_end(ap);
   out = fopen("debug.out","a+");
   if ( out!=NULL )
   {
		fputs(DebugBuffer,out);
		fclose(out);
   }
}


