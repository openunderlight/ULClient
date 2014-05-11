// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef GIF_H
#define GIF_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <string.h>
#include "wbox.h"
  
#ifndef  NULL
#define  NULL 0L
#endif

#ifdef __SC__
#define ispathdelim(c)  ((c) == '\\' || (c) == ':' || (c) == '/')
#endif

#ifndef __OS2__
#define LONG   unsigned long
#define ULONG  unsigned long
#define UCHAR  unsigned char
#define USHORT unsigned short
#define DWORD  unsigned long
#else
#include <os2def.h>
#endif

#define  UCHAR  unsigned char
#define  USHORT unsigned short
#define  MAX(a,b)   		((a)>(b)?(a):(b))
#define  MIN(a,b)   		((a)<(b)?(a):(b))
#define  ABS(a)        	(((a)<0) ? -(a) : (a))
#define  SGN(a)   		(((a)<0) ? -1 : 1)
#define NEXTBYTE        (*ptr++)
#define EXTENSION     0x21
#define TRAILER       0x3b
#define IMAGESEP        0x2c
#define GRAPHIC_EXT     0xf9
#define PLAINTEXT_EXT   0x01
#define APPLICATION_EXT 0xff
#define COMMENT_EXT     0xfe
#define START_EXTENSION 0x21
#define INTERLACEMASK   0x40
#define COLORMAPMASK    0x80
#define ERR_NOFILE  -1
#define ERR_NOMEM   -2
#define ERR_READERR -3
#define ERR_NOTGIF  -4
#define ERR_CORRUPT -5
#define ERR_BADEXT  -6
#define True     1
#define False    0
#define MakeID(d,c,b,a) ((ULONG)(a)<<24 | (ULONG)(b)<<16 | (ULONG)(c)<<8 | (ULONG)(d) )
#define ID_BMHD MakeID('B','M','H','D')

// Typedefs                                                           


typedef unsigned char byte;

typedef struct {
    USHORT w, h;                    // raster width & height in pixels
    USHORT  x, y;                   // position for this image
    UCHAR nPlanes;                 // # source bitplanes
    UCHAR masking;                 // masking technique
    UCHAR compression;             // compression algoithm
    UCHAR pad1;                    // UNUSED.  For consistency, put 0 here.
    USHORT transparentColor;        // transparent "color number"
    UCHAR xAspect, yAspect;        // aspect ratio, a rational number x/y
    USHORT  pageWidth, pageHeight;  // source "page" size in pixels
    } BitMapHeader;
// A CMAP chunk is a packed array of RGB's (3 bytes each).

typedef ULONG ID;  /* An ID is four printable ASCII chars like FORM or DPPV */

union sgpt
{
	UCHAR *p;
	struct { USHORT offs,seg; }wrd;
};

typedef struct
{
	ID    type;
	long  cksize;
	ID    subtype;
} form_chunk;

typedef struct 
{
	ID     ckID;
	ULONG  ckSize;
} ChunkHeader;

typedef struct 
{
	ID     ckID;
	ULONG  ckSize;
	UCHAR ckData[ 1 /*REALLY: ckSize*/ ];
} Chunk;

typedef struct 
{ 
	UCHAR red, green, blue; 
} RGB;

// CLASS DEFINITION
class cGif	
{
	public:
		cGif();							// BC makes us put this in.
        cGif(byte *GifData, int l, short flip=0);	// Open with a gif in memory.
		cGif(TCHAR *filename, short flip=0); // Open with a gif file.
        ~cGif();			
		inline  short   Wd()	{ return width;  };
		inline  short   Ht()	{ return height; };
        inline  short   Found()	{ return goodpic;};
        inline  short   Valid()	{ return goodbm; };
        inline  short   Type()	{ return typebm; };
        inline  RGB  *Colors()	{ return picpal; };
        inline	UCHAR *Address(void) { return data; };
		short	ReadGif   (byte *GifData, int l, short flip=0, short x=0, short y=0);
		int		ReadCode (void);
		short	GetGifDims(byte *GifData, int l, short &x, short &y);
		void	SetClip  (short minx=0, short miny=0, short wd=0, short ht=0);
		void	SetClip  (BOX &clp);
		void	SetClip  (BOX *clp);
		void	Init (unsigned short x, unsigned short y);
		void	Init (unsigned char *imgdata,unsigned short x, unsigned short y);
		void   ByteFlipLong(unsigned long *NUMBER);
		USHORT swab(unsigned short swapper);
		void	SetPoint  (short x, short y, UCHAR c);
		void	Line(short x1,short y1,short x2,short y2,short c);

	protected:
		void	AddToPixel(unsigned char Index); // used by gif reader.
		UCHAR   *struc;
		UCHAR   *data;
		UCHAR   masked;
		BOX		clip;		// the box that defines clip boundaries
		short	width;		// width of this bitmap in pixels
		short	height;		// height of this bitmap in pixels
		RGB		*picpal;	// a palette if any.
		short	goodbm;		// TRUE if this bitmap is good.
		short	goodpic;	// TRUE if last ReadIff worked correctly.
		UCHAR	haspal;		// TRUE if this bitmap has a pallete
		UCHAR	typebm;		// type of bitmap
		UCHAR	loadpals;	// whether to load palettes from pics or not.
	
	public:
		FILE	*ResOpen(TCHAR *fname);			// open, else opens it from dos.
        void	ResClose(FILE *fptr);			// Close a file opened with ResOpen.
        long	ResFileSize(FILE *fp);
    };

#endif