#define STRICT


// TextureMaker: converts and packs textures & flats into one  file

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "cGif.h"
#include "Effects.h"
#include "txtrload.h"

//////////////////////////////////////////////////////////////////
// Constants

#define  NAME "Texture Maker"
#define  TITLE "Texture Maker"
const char outfile[] = "Textures.rlm";
const char texture_asc_file[] = "Bm.asc";
const char flat_asc_file[] = "Flats.asc";
const int FLAT = 1;
const int WALL = 2;
const int FLAT_HEIGHT = 64;
const int FLAT_WIDTH = 64;
const int MAX_FILENAME = 512;

#define printf DebugOut

//////////////////////////////////////////////////////////////////
// Structures

struct size_struct
{
	short			height;
	short			width;
};


//////////////////////////////////////////////////////////////////
// Function Prototypes

static int WriteTextures(FILE *fh);
static int WriteFlats(FILE *fh);
void FlipSideways(unsigned char *dst, unsigned char *src, int w, int h);
int LoadAndConcatImages(char *filename, unsigned char *buffer, size_struct *file_size, BOOL flip, int type);
void __cdecl DebugOut(char *args,...);

/////////////////////////////////////////////////
// Global Variables


/////////////////////////////////////////////////
// Functions


int PASCAL WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;
	texture_file_header header;

	// open effect maker output file
	FILE *fh = fopen(outfile, "wb");
	if (fh == NULL)
	{
	  MessageBox(NULL, "Can't open output file", "Oops!", MB_OK);
	  return 0;
	}

	// make space for header at start of file
	fseek(fh,sizeof(header),SEEK_SET);

	// write out the Textures
	header.num_textures = WriteTextures(fh);
	if (header.num_textures < 0)
	   return 0;

	fseek(fh,0,SEEK_END);
    header.num_flats = WriteFlats(fh);
	if (header.num_flats < 0)
	   return 0;
	 
  // Go to start of file and write file header
	fseek(fh,0,SEEK_SET);
	if (fwrite(&header, sizeof(header), 1, fh)!= 1)
	{		
		MessageBox(NULL, "Error writing header", "Oops!", MB_OK);
		return 0;
	}

  fclose(fh);
  MessageBox(NULL, "TextureMaker Finished", "Cool", MB_OK);
  return msg.wParam;
   
}

int ids[MAX_TEXTURES];
int palettes[MAX_TEXTURES];
char filenames[MAX_TEXTURES][MAX_FILENAME];

int WriteTextures(FILE *fh)
{
	int num_textures=0;
	int file_pos;
	int i;
	unsigned char *big_buffer = new unsigned char[200000]; // should be enough...
	unsigned char *small_buffer = new unsigned char[1024];
	long file_start;
	char c;
	size_struct file_size;
	texture_header_t *texture_headers;
	FILE *asc;
	bool assigned[MAX_TEXTURES];

	for (i=0; i<MAX_TEXTURES; i++)
		assigned[i] = false;

    file_start = ftell(fh);
    file_pos = file_start;

	// open ASCII input file
	asc = fopen(texture_asc_file, "rb");
	if (asc == NULL)
	{
		MessageBox(NULL, "Can't open textures ASC file", "Oops!", MB_OK);
		return -1;
	}
	while (1)
	{
		if (fscanf(asc, "%s", filenames[num_textures]) == EOF)
			break;

		if (filenames[num_textures][0] == '#')
		{  // ignore comments - read until end of line
			c = ' ';
			while (c != '\n')
				fread(&c, 1, 1, asc);
			continue;
		}
		else 
		{
			if (num_textures == MAX_TEXTURES)
			{
				MessageBox(NULL, "Too many textures!", "Oops!", MB_OK);
				return -1;
			}

			fscanf(asc, "%d %d", &(ids[num_textures]), &(palettes[num_textures]));
			num_textures++;
		}

	}
	printf("num textures: %d\n",num_textures);
	fclose(asc);

	// Write texture headers - this is a dummy for now because it will be filled in an rewritten later.
	texture_headers = new texture_header_t[num_textures];
	if (fwrite(texture_headers, sizeof(texture_header_t), num_textures, fh) != (UINT)num_textures)
	{		
		MessageBox(NULL, "Error writing dummy texture headers", "Oops!", MB_OK);
		return -1;
	}
	file_pos += sizeof(texture_header_t)*num_textures;

	for (i=0; i < num_textures; i++)
	{
		int len = LoadAndConcatImages(filenames[i], big_buffer, &file_size, TRUE, WALL);
		
//		if (ids[i] == 134)// || ids[i] == 0)
//		{
			int j;
			BOOL blank=TRUE;
		//	printf("***************************************\n");
			for (j=0; j<file_size.height*file_size.width; j++) 
			{
				unsigned char z = *(big_buffer+j);
				if (z != 205)
				{
					blank = FALSE;
					break;
				}
			}
			if (blank)
				printf("WARNING: bitmap %d is blank!\n",ids[i]);

//		}
		

		if (fwrite(big_buffer, len, 1, fh)  != 1)
		{		
			MessageBox(NULL, "Error writing texture bits", "Oops!", MB_OK);
			return -1;
		}

		if (assigned[ids[i]])
		{		
			char message[256];
			sprintf(message, "Error: texture %d assigned twice\n", ids[i]);
			printf("%s\n",message);
			MessageBox(NULL, message, "Oops!", MB_OK);
			return -1;
		}

		assigned[ids[i]] = true;
		texture_headers[i].file_position = file_pos;
		texture_headers[i].height        = file_size.height;
		texture_headers[i].width         = file_size.width;
		texture_headers[i].palette		 = palettes[i];
		texture_headers[i].id			 = ids[i];
		printf("Texture %d, h = %d, w = %d, pal = %d at pos %d\n",ids[i], 
			file_size.height, file_size.width, palettes[i], file_pos); 
		file_pos += len;
	}

	if (i != num_textures)
	{		
		MessageBox(NULL, "Error - wrong number of textures", "Oops!", MB_OK);
		return -1;
	}
	
	// now write out the file headers
	if (fseek(fh, file_start, SEEK_SET))
	{
		MessageBox(NULL, "Error seeking to rewrite texture headers", "Oops!", MB_OK);
		return -1;
	}

	if (fwrite(texture_headers, sizeof(texture_header_t), num_textures, fh) != (UINT) num_textures)
	{		
		MessageBox(NULL, "Error writing real texture headers", "Oops!", MB_OK);
		return -1;
	}

	if (fseek(fh, 0, SEEK_END))
	{		
		MessageBox(NULL, "Error seeking to end of textures", "Oops!", MB_OK);
		return -1;
	}

	delete [] texture_headers;
	delete [] big_buffer;
	delete [] small_buffer;

    return num_textures;
}

int WriteFlats(FILE *fh)
{
	int num_flats=0,file_pos,ids[MAX_FLATS],palettes[MAX_FLATS],i;
	unsigned char *big_buffer = new unsigned char[2000000]; // should be enough...
	unsigned char *small_buffer = new unsigned char[1024];
	long file_start;
	char c;
	size_struct file_size;
	flat_header_t *flat_headers;
	FILE *asc;
	bool assigned[MAX_TEXTURES];

	for (i=0; i<MAX_TEXTURES; i++)
		assigned[i] = false;

    file_start = ftell(fh);
    file_pos = file_start;

	// open ASCII input file
	asc = fopen(flat_asc_file, "rb");
	if (asc == NULL)
	{
		MessageBox(NULL, "Can't open flats ASC file", "Oops!", MB_OK);
		return -1;
	}
	// 1st, count the # of flats
	while (1)
	{
		if (fscanf(asc, "%s", filenames[num_flats]) == EOF)
			break;

		if (filenames[num_flats][0] == '#')
		{  // ignore comments - read until end of line
			c = ' ';
			while (c != '\n')
				fread(&c, 1, 1, asc);
			continue;
		}
		else 
		{
			if (num_flats == MAX_FLATS)
			{
				MessageBox(NULL, "Too many textures!", "Oops!", MB_OK);
				return -1;
			}
			fscanf(asc, "%d %d", &(ids[num_flats]), &(palettes[num_flats]));
			num_flats++;
		}

	}
	printf("num flats: %d\n",num_flats);
	fclose(asc);

	// Write flat headers - this is a dummy for now because it will be filled in an rewritten later.
	flat_headers = new flat_header_t[num_flats];
	if (fwrite(flat_headers, sizeof(flat_header_t), num_flats, fh) != (UINT)num_flats)
	{		
		MessageBox(NULL, "Error writing dummy flat headers", "Oops!", MB_OK);
		return -1;
	}
	file_pos += sizeof(flat_header_t)*num_flats;

	for (i=0; i < num_flats; i++)
	{
		int len = LoadAndConcatImages(filenames[i], big_buffer, &file_size, FALSE, FLAT);

		if (fwrite(big_buffer, len, 1, fh)  != 1)
		{		
			MessageBox(NULL, "Error writing flat bits", "Oops!", MB_OK);
			return -1;
		}
		if (assigned[ids[i]])
		{		
			char message[256];
			sprintf(message, "Error: flat %d assigned twice\n", ids[i]);
			printf("%s\n",message);
			MessageBox(NULL, message, "Oops!", MB_OK);
			return -1;
		}

		assigned[ids[i]] = true;

		flat_headers[i].file_position = file_pos;
		flat_headers[i].height        = file_size.height;
		flat_headers[i].width         = file_size.width;
		flat_headers[i].palette		 = palettes[i];
		flat_headers[i].id			 = ids[i];
		file_pos += len;
	}

	if (i != num_flats)
	{		
		MessageBox(NULL, "Error - wrong number of flats", "Oops!", MB_OK);
		return -1;
	}
	
	// now write out the file headers
	if (fseek(fh, file_start, SEEK_SET))
	{
		MessageBox(NULL, "Error seeking to rewrite flat headers", "Oops!", MB_OK);
		return -1;
	}

	if (fwrite(flat_headers, sizeof(flat_header_t), num_flats, fh) != (UINT) num_flats)
	{		
		MessageBox(NULL, "Error writing real flat headers", "Oops!", MB_OK);
		return -1;
	}

	if (fseek(fh, 0, SEEK_END))
	{		
		MessageBox(NULL, "Error seeking to end of flats", "Oops!", MB_OK);
		return -1;
	}

	delete [] flat_headers;
	delete [] big_buffer;
	delete [] small_buffer;

    return num_flats;
}
// Flips a bitmap for the renderer.
void FlipSideways(unsigned char *dst, unsigned char *src, int w, int h)
{
	unsigned char *s,*d;
    int width,height;

	s  = src;
    d  = dst;

    for(height=0;height<h;height++)
    {
        d = dst+height;
        for(width=0;width<w;width++)
        {
           *d=*s;
           s++;
           d+=h;
        }
    }

}


// Load in the bitmaps for a visual effect. 
int LoadAndConcatImages(char *filename, unsigned char *buffer, size_struct *file_size, BOOL flip, int type)
{
	char file[64];
	char message[132];
	cGif *gif;
	unsigned char *src, *dst;
	int width=0; 
	int height=0;
	dst = buffer;

	wsprintf(file,"%s.gif",filename);
	gif = new cGif(file);
//	printf("gif file name: %s wd: %d ht: %d\n",file,gif->Wd(),gif->Ht());
	width = file_size->width = gif->Wd();
	if (width == 0)
	{
		sprintf(message,"Can not open file %s\n",file);
		MessageBox(NULL, message, "Oops!", MB_OK);
		exit(-1);
	}
	height = file_size->height = gif->Ht();
	if ((type == FLAT) && ((height != FLAT_HEIGHT) || (width != FLAT_WIDTH)))
	{
		sprintf(message,"Flat %s is wrong size - is %d x %d, should be %d x %d\n",file, width, height, FLAT_WIDTH, FLAT_HEIGHT);
		MessageBox(NULL, message, "Oops!", MB_OK);
		exit(-1);
	}
	if ((type == WALL) && (((height%2) != 0) || ((width%2) != 0)))
	{
		sprintf(message,"Wall Texture %s must have height/width as powers of 2; current size is %d x %d",file, width, height);
		MessageBox(NULL, message, "Oops!", MB_OK);
		exit(-1);
	}

	src = gif->Address();
	if (flip)
		FlipSideways(dst, src, width, height);
	else
		memcpy(dst, src, width*height);
	delete gif;

	return (width*height);
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


