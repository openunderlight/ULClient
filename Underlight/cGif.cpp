// cGif: gif loading class

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT
#include <windows.h>
#include <tchar.h>
#include "cGif.h"


boolean Interlace, HasColormap;
boolean Verbose = True;

// variables
// CHECK WHICH OF THESE IS USED LATER, INCLUDE IN CLASS LATER
int BitOffset = 0,      // Bit Offset of next code
    XC = 0, YC = 0,     // Output X and Y coords of current pixel
    Pass = 0,           // Used by output routine if interlaced pic
    OutCount = 0,       // Decompressor output 'stack count'
    RWidth, RHeight,    // screen dimensions
    Width, Height,      // image dimensions
    LeftOfs, TopOfs,    // image offset
    BitsPerPixel,       // Bits per pixel, read from GIF header
    BytesPerScanline,   // bytes per scanline in output raster
    ColorMapSize,       // number of colors
    Background,         // background color
    CodeSize,           // Code size, read from GIF header
    InitCodeSize,       // Starting code size, used during Clear
    Code,               // Value returned by ReadCode
    MaxCode,            // limiting value for current code size
    ClearCode,          // GIF clear code
    EOFCode,            // GIF end-of-information code
    CurCode, OldCode,   // Decompressor variables
    InCode,             // Decompressor variables
    FirstFree,          // First free code, generated per GIF spec
    FreeCode,           // Decompressor, next free slot in hash table
    FinChar,            // Decompressor variable
    BitMask,            // AND mask for data size
    ReadMask,           // Code AND mask for current code size
    Misc;                       /* miscellaneous bits (interlace, local cmap)*/

int npixels, maxpixels; // Pulled out of ReadGif for debugging.

byte *Image;            // The result array
byte *RawGIF;           // The heap array to hold it, raw
byte *Raster;           // The raster data stream, unblocked

// The hash table used by the decompressor
int Prefix[4096];
int Suffix[4096];

// An output array used by the decompressor
int OutCode[1025];

// The color map, read from the GIF header
byte Red[256], Green[256], Blue[256], used[256];
int  numused;

int	 gif89 = 0;
char *id87 = "GIF87a";
char *id89 = "GIF89a";
char *idGF = "4DXGR";
// put in class later ^^^^^



cGif::cGif()
{
   return;
}

cGif::cGif(byte *GifData, int l, short flip)
{
	short gw,gh;

	haspal  = 0;
	goodbm  = 0;
	goodpic = 0;
	data    = NULL;
	struc   = NULL;

	GetGifDims(GifData,l,gw,gh);
	Init(gw,gh);
	ReadGif(GifData,l);
	return;
}

cGif::cGif(TCHAR *filename, short flip)
{
	FILE *pic;
	byte *GifData;
	int filesize;
	short gw,gh;

	haspal  = 0;
	goodbm  = 0;
	goodpic = 0;
	height = width = 0;
	data    = NULL;
	struc   = NULL;

	if ((pic = ResOpen(filename))== NULL)  return;
	filesize = ResFileSize(pic);

    if (!(GifData = (byte *) malloc(filesize))) {
		ResClose(pic);
		return;
	}

	if (fread(GifData, filesize, 1, pic) != 1) {
		free (GifData);
		ResClose(pic);
		return;
	}

	ResClose(pic);


	GetGifDims(GifData,filesize,gw,gh);
	Init(gw,gh);
	ReadGif(GifData,filesize);

	free (GifData);

	return;
}

cGif::~cGif()
{
	// from cGif.cpp
	if ( haspal ) delete [] picpal;
	if (!typebm) {
		if (struc)     delete [] struc;
    }
	struc  = 0;
	data   = 0;
	goodbm = 0;

}







// ************************************************************************
// below is from loadgif.cpp

/*
 * loadgif: Original copyright notice as follows.
 *
 * gif2ras.c - Converts from a Compuserve GIF (tm) image to a Sun Raster image.
 *
 * Copyright (c) 1988, 1989 by Patrick J. Naughton
 *
 * Author: Patrick J. Naughton
 * naughton@wind.sun.com
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 *
 */

short
cGif::ReadGif(byte *GifData, int l, short flip, short x, short y)
{
	int                numcols, filesize;
	register unsigned  char ch, ch1;
	register byte      *ptr, *ptr1; 
	register int   i;
	int            aspect, gotimage; 
	float normaspect;

	short transparency = -1;
	BitOffset = 0;
	XC = YC  = 0;
	Pass     = 0;
	OutCount = 0;
	gotimage = 0;
	RawGIF = Raster = NULL;
	gif89 = 0;
	goodpic = 0;
	filesize = l;
	npixels = maxpixels = 0;

	if (!(ptr = RawGIF = (byte *) calloc((size_t) filesize+256, (size_t) 1))) return(ERR_NOMEM);

	if (!(Raster = (byte *) calloc((size_t) filesize+256, (size_t) 1))) {
		free( ptr );
		return (ERR_NOMEM);
	}

	memcpy(ptr, GifData, filesize);
	
	if (strncmp((char *) ptr, id87, (size_t) 6)==0) gif89 = 0;
	else if (strncmp((char *) ptr, id89, (size_t) 6)==0) gif89 = 1;
	else    {
		free (RawGIF);
		free (Raster);
		return(ERR_NOTGIF);
	}

	ptr += 6;

	// Get variables from the GIF screen descriptor
	ch           = NEXTBYTE;
	RWidth       = ch + 0x100 * NEXTBYTE; // screen dimensions... not used
	ch           = NEXTBYTE;
	RHeight      = ch + 0x100 * NEXTBYTE;

	//_tprintf("screen dims: %dx%d.\n", RWidth, RHeight);

	ch           = NEXTBYTE;
	HasColormap  = ((ch & COLORMAPMASK) ? True : False);

	BitsPerPixel = (ch & 7) + 1;
	numcols      = ColorMapSize = 1 << BitsPerPixel;
	BitMask      = ColorMapSize - 1;

	Background   = NEXTBYTE;      /* background color... not used. */

	aspect = NEXTBYTE;
	if (aspect) {
		if (!gif89) {
			free (RawGIF);
			free (Raster);
			return (ERR_CORRUPT);
		}
		else normaspect = (float) ((aspect + 15) / 64.0);   /* gif89 aspect ratio */
//	  if (DEBUG)_ftprintf(stderr,"GIF89 aspect = %f\n", normaspect);
	}


	// Read in global colormap

	if (HasColormap) {
		//if (Verbose)
		//_tprintf("gif is %dx%d, %d bits per pixel, (%d colors).\n",
		//	RWidth,RHeight,BitsPerPixel, ColorMapSize);
        if (haspal) {
			delete [] picpal;
        }
        picpal = new RGB[256];
        if ( picpal ) {
            haspal = 1;
            for(i=0;i<ColorMapSize;i++) {
				picpal[i].red   = (NEXTBYTE);// >>2;
				picpal[i].green = (NEXTBYTE);// >>2;
				picpal[i].blue  = (NEXTBYTE);// >>2;
				used[i]  = 0;
            }
        }
        else {
            free (RawGIF);
			free (Raster);
			return(ERR_NOMEM);
        }
		numused = 0;
	} // else no colormap in GIF file

	// look for image separator

	for (ch = NEXTBYTE ; ch != IMAGESEP ; ch = NEXTBYTE) {
		i = ch;
		//_tprintf("EXTENSION CHARACTER: %x\n", i);
		if (ch != START_EXTENSION) {
			free (RawGIF);
			free (Raster);
			return ERR_BADEXT;
        }

		// handle image extensions
		switch (ch = NEXTBYTE) {
			case GRAPHIC_EXT:
				ch = NEXTBYTE;
				if (ptr[0] & 0x1) {
					transparency = ptr[3];   // transparent color index
					//_tprintf("transparency index: %i\n", transparency);
				}
				ptr += ch;
				break;
			case PLAINTEXT_EXT:
				break;
			case APPLICATION_EXT:
				break;
			case COMMENT_EXT:
				break;
			default:
				free (RawGIF);
				free (Raster);
				return ERR_BADEXT;
		}
		while ((ch = NEXTBYTE)) ptr += ch;
	}

	// Now read in values from the image descriptor

	ch        = NEXTBYTE;
	LeftOfs   = ch + 0x100 * NEXTBYTE;
	ch        = NEXTBYTE;
	TopOfs    = ch + 0x100 * NEXTBYTE;
	ch        = NEXTBYTE;
	Width     = ch + 0x100 * NEXTBYTE;
	ch        = NEXTBYTE;
	Height    = ch + 0x100 * NEXTBYTE;

	Misc = NEXTBYTE;
	Interlace = ((Misc & INTERLACEMASK) ? True : False);

	if (Misc & 0x80) {
        if (haspal) {
			delete [] picpal;
        }
        picpal = new RGB[256];
        if ( picpal ) {
            haspal = 1;

			for (i=0; i< 1 << ((Misc&7)+1); i++) {
				picpal[i].red = (NEXTBYTE);
				picpal[i].green = (NEXTBYTE);
				picpal[i].blue = (NEXTBYTE);
				used[i]  = 0;
			}
		}
        else {
			free (RawGIF);
			free (Raster);
            return(ERR_NOMEM);
        }
		numused = 0;
	}

	//if (Verbose)
	//_tprintf("Reading a %d by %d %sinterlaced image...",
	//	Width, Height, (Interlace) ? "" : "non-");

	// Start reading the raster data. First we get the intial code size
	// and compute decompressor constant values, based on this code size.

	CodeSize  = NEXTBYTE;
	ClearCode = (1 << CodeSize);
	EOFCode   = ClearCode + 1;
	FreeCode  = FirstFree = ClearCode + 2;

	// The GIF spec has it that the code size is the code size used to
	// compute the above values is the code size given in the file, but the
	// code size used in compression/decompression is the code size given in
	// the file plus one. (thus the ++).

	CodeSize++;
	InitCodeSize = CodeSize;
	MaxCode      = (1 << CodeSize);
	ReadMask     = MaxCode - 1;

	// Read the raster data.  Here we just transpose it from the GIF array
	// to the Raster array, turning it from a series of blocks into one long
	// data stream, which makes life much easier for ReadCode().

	ptr1 = Raster;
	do {
       ch = ch1 = NEXTBYTE;
       while (ch--) *ptr1++ = NEXTBYTE;
       if ((ptr1 - Raster) > filesize) {
           free (RawGIF);
		   free (Raster);
		   return(ERR_CORRUPT);
       }
    } while(ch1);

	free(RawGIF);    /* We're done with the raw data now... */

	//if (Verbose) {
	//_tprintf("done.\n");
	//_tprintf("Decompressing...");
	//    }


	maxpixels = Width*Height;
	BytesPerScanline    = Width;


	// Decompress the file, continuing until you see the GIF EOF code.
	// One obvious enhancement is to add checking for corrupt files here.

	Code = ReadCode();
	while (Code != EOFCode) {
		// Clear code sets everything back to its initial value, then reads the
		// immediately subsequent code as uncompressed data.
		if (Code == ClearCode) {
			CodeSize = InitCodeSize;
			MaxCode  = (1 << CodeSize);
			ReadMask = MaxCode - 1;
			FreeCode = FirstFree;
			Code = ReadCode();
			CurCode  = OldCode = Code;
			FinChar  = CurCode & BitMask;
			AddToPixel(FinChar);
			npixels++;
        }
		else {

			/* if we're at maxcode and didn't get a clear, stop loading */
			if (FreeCode>=4096) { /*_tprintf("freecode blew up\n"); */
			    break; }

			// If not a clear code, then must be data: save same as CurCode and InCode
			CurCode = InCode = Code;

			// If greater or equal to FreeCode, not in the hash table yet;
			// repeat the last character decoded
			if (CurCode >= FreeCode) {
				CurCode = OldCode;
				if (OutCount > 4096) {  /*_tprintf("outcount1 blew up\n"); */ break; }
				OutCode[OutCount++] = FinChar;
            }

			// Unless this code is raw data, pursue the chain pointed to by CurCode
			// through the hash table to its end; each code in the chain puts its
			// associated output code on the output queue.
			while (CurCode > BitMask) {
				if (OutCount > 4096) {
					//_tprintf("\nCorrupt GIF file (OutCount)!\n");
					//_exit(-1);  /* calling 'exit(-1)' dumps core, so I don't */
					break;
				}
				OutCode[OutCount++] = Suffix[CurCode];
				CurCode = Prefix[CurCode];
            }

			if (OutCount > 4096) { /*_tprintf("outcount blew up\n"); */ break; }
 
			// The last code in the chain is treated as raw data.
			FinChar             = CurCode & BitMask;
			OutCode[OutCount++] = FinChar;

			// Now we put the data out to the Output routine.
			// It's been stacked LIFO, so deal with it that way...

			// safety thing:  prevent exceeding range
			if (npixels + OutCount > maxpixels) OutCount = maxpixels-npixels;
	
			npixels += OutCount;

			for (i = OutCount - 1; i >= 0; i--) AddToPixel(OutCode[i]);
			OutCount = 0;

			// Build the hash table on-the-fly. No table is stored in the file.
			Prefix[FreeCode] = OldCode;
			Suffix[FreeCode] = FinChar;
			OldCode          = InCode;

			// Point to the next slot in the table.  If we exceed the current
			// MaxCode value, increment the code size unless it's already 12.  If it
			// is, do nothing: the next code decompressed better be CLEAR
			FreeCode++;
			if (FreeCode >= MaxCode) {
				if (CodeSize < 12) {
					CodeSize++;
					MaxCode *= 2;
					ReadMask = (1 << CodeSize) - 1;
				}
            }
		}
		if (npixels >= maxpixels) break;
		Code = ReadCode();
	}
	free(Raster);
	//_tprintf("done.\n");
	goodpic = 1;
	return(0);
}

/* Fetch the next code from the raster data stream.  The codes can be
 * any length from 3 to 12 bits, packed into 8-bit bytes, so we have to
 * maintain our location in the Raster array as a BIT Offset.  We compute
 * the byte Offset into the raster array by dividing this by 8, pick up
 * three bytes, compute the bit Offset into our 24-bit chunk, shift to
 * bring the desired code to the bottom, then mask it off and return it.
 */

int cGif::ReadCode( void )
{
	int RawCode, ByteOffset;

    ByteOffset = BitOffset / 8;
    RawCode	   = Raster[ByteOffset] + (Raster[ByteOffset + 1] << 8);
    if (CodeSize >= 8)
		RawCode += ( ((int) Raster[ByteOffset + 2]) << 16);
    RawCode  >>= (BitOffset % 8);
    BitOffset += CodeSize;

    return(RawCode & ReadMask);
}

short cGif::GetGifDims(byte *GifData, int l, short &x, short &y)
{
	int    numcols;
	register unsigned  char ch;
	register byte     *ptr;
	register int   i;
	short transparency = -1;
	BitOffset= 0;
	XC = YC  = 0;
	Pass     = 0;
	OutCount = 0;
	x        = 0;
	y        = 0;

//	if (!(ptr = RawGIF = (byte *) malloc(16384))) return(ERR_NOMEM);
    if (!(ptr = RawGIF = (byte *) malloc(l))) return(ERR_NOMEM);

//	memcpy(ptr, GifData, 16384);
    memcpy(ptr, GifData, l);

	if (strncmp((char *) ptr, id87, 6)) {
		if (strncmp((char *)ptr, id89, 6)) {
			return (ERR_NOTGIF);
		}
	}

	ptr += 10;

	ch           = NEXTBYTE;
	HasColormap  = ((ch & COLORMAPMASK) ? True : False);

	BitsPerPixel = (ch & 7) + 1;
	numcols      = ColorMapSize = 1 << BitsPerPixel;

	Background   = NEXTBYTE;      /* background color... not used. */

	if (NEXTBYTE) {		// should be NULL
		free (RawGIF);
		return ERR_CORRUPT;
    }

	if (HasColormap) ptr += (3*ColorMapSize);

	// look for image separator
	for (ch = NEXTBYTE ; ch != IMAGESEP ; ch = NEXTBYTE) {
		i = ch;
		if (ch != START_EXTENSION) {
			free (RawGIF);
			return ERR_BADEXT;
        }

		// handle image extensions
		switch (ch = NEXTBYTE) {
			case GRAPHIC_EXT:
				ch = NEXTBYTE;
				if (ptr[0] & 0x1) {
					transparency = ptr[3];   // transparent color index
                }
				ptr += ch;
				break;
        }
		while ((ch = NEXTBYTE)) ptr += ch;
    }

	// Now read in values from the image descriptor
	ch        = NEXTBYTE;
	LeftOfs   = ch + 0x100 * NEXTBYTE;
	ch        = NEXTBYTE;
	TopOfs    = ch + 0x100 * NEXTBYTE;
	ch        = NEXTBYTE;
	Width     = ch + 0x100 * NEXTBYTE;
	ch        = NEXTBYTE;
	Height    = ch + 0x100 * NEXTBYTE;
	Interlace = ((NEXTBYTE & INTERLACEMASK) ? True : False);

	x = Width;
	y = Height;

	free (RawGIF);  RawGIF = NULL;

	return(0);
}

void cGif::SetClip(short minx, short miny, short wd, short ht)
{
   if (!wd) wd = width;
   if (!ht) ht = height;
   clip.SetBox(minx,miny,wd,ht);
}

void cGif::SetClip(BOX &cbox)
{
   clip = cbox;
}

void cGif::AddToPixel(byte Index)
{
    if (YC<Height) {
		//*(Image + YC * BytesPerScanline + XC) = Index;
        SetPoint(XC,YC,Index);
    }

    if (!used[Index]) { used[Index]=1;  numused++; }

    // Update the X-coordinate, and if it overflows, update the Y-coordinate

    if (++XC == Width) {
		// If a non-interlaced picture, just increment YC to the next scan line.
        // If it's interlaced, deal with the interlace as described in the GIF
        // spec.  Put the decoded scan line out to the screen if we haven't gone
        // past the bottom of it
        XC = 0;
        if (!Interlace) YC++;
        else {
			switch (Pass) {
                case 0:
                    YC += 8;
                    if (YC >= Height) { Pass++; YC = 4; }
                    break;
                case 1:
                    YC += 8;
                    if (YC >= Height) { Pass++; YC = 2; }
                    break;
                case 2:
                    YC += 4;
                    if (YC >= Height) { Pass++; YC = 1; }
                    break;
                case 3:
                    YC += 2;
                    break;
                default:
                    break;
            }
        }
    }
}


// ************************************************************************
// below is from bitmap.cpp

void cGif::Init(unsigned short x, unsigned short y)
{
	// the +4 is so we can encode the width and height.
	// the +4 is so we can encode the width and height.
	// struc = new UCHAR [(x*y)+2];

	struc= new UCHAR [(x*y)+6];

	loadpals = 1;
	typebm   = 0;

	if (!struc) {
		goodbm = 0;
    }
	else {
		data = struc+6;
		goodbm = 1;
		width  = x;
		height = y;
		haspal = 0;
		masked = 0;

		struc[0]=(unsigned char)(width%256);
		struc[1]=(unsigned char)(width/256);
		struc[2]=(unsigned char)(height%256);
		struc[3]=(unsigned char)(height/256);
		struc[4]=8;
		struc[5]=0;

		SetClip();
	}
}

void cGif::Init(unsigned char *imgdata, unsigned short x, unsigned short y)
{
	struc = NULL;
	loadpals = 1;
	typebm   = 1;
	data   = imgdata;
	goodbm = 1;
	width  = x;
	height = y;
	haspal = 0;
	masked = 0;
	SetClip();
}

void cGif::SetPoint  (short x, short y, UCHAR c)
{
   if ( x>=clip.x && y>= clip.y && x<clip.x2 && y < clip.y2) {
      *(data+x+(y*width)) = c;
   }
}

void cGif::Line(short x1,short y1,short x2,short y2,short c)
{
	short d, x, y, ax, ay, sx, sy, dx, dy;

	// Yet another Bresenham's Algorithm.
	dx = x2-x1;  ax = (short)(ABS(dx)<<1);  sx = (short)SGN(dx);
	dy = y2-y1;  ay = (short)(ABS(dy)<<1);  sy = (short)SGN(dy);
	x  = x1;     y  = y1;

	if (ax>ay) {         /* x dominant */
		d = ay-(ax>>1);
		for (;;) {
			SetPoint(x,y,(unsigned char)c);
			if (x==x2) return;
			if (d>=0) {
				y += sy;
				d -= ax;
            }
			x += sx;
			d += ay;
		}
	}
	else {                 /* y dominant */
		d = ax-(ay>>1);
		for (;;) {
			SetPoint(x,y,(unsigned char)c);
			if (y==y2) return;
			if (d>=0) {
				x += sx;
				d -= ay;
            }
			y += sy;
			d += ax;
        }
   }
}






// ************************************************************
// from wresource.cpp

FILE * cGif::ResOpen(TCHAR *fname)
{
	return _tfopen(fname,_T("rb"));
}

// close a file
void cGif::ResClose(FILE *fptr)
{
	fclose(fptr);
}

long LastFileSize;

long cGif::ResFileSize(FILE *fp)
{
	long filesize;

	fseek(fp, 0L, 2);
	filesize = ftell(fp);
	fseek(fp, 0L, 0);
    
	return(filesize);
}

void cGif::ByteFlipLong(unsigned long *NUMBER)
{
   // Hey, I didn't right this function, okay?
   long Y, T;
   short I;

	T = *NUMBER;
	Y=0;for (I=0;I<4;I++){Y = Y | (T & 0xFF);if (I<3) {Y = Y << 8;T = T >> 8;}}
	*NUMBER = Y;
}

unsigned short cGif::swab(unsigned short swapper)
{
   unsigned short temp1,temp2,temp3;

   temp1 = swapper >>8;
   temp2 = swapper <<8;
   temp3 = temp1|temp2;

   return(temp3);
}
















