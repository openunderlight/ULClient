// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include "cNameTag.h"
#include "cChat.h"
#include "RLE.h"

//////////////////////////////////////////////////////////////////
// External 
extern cChat *display;

//////////////////////////////////////////////////////////////////
// Static data and contants

// Number of mipmaps for a name tag, 
//each at a lower resolution than the previous
const int NUM_MIPMAPS = 2;

// Windows  font used to generate the mipmaps
static LOGFONT font = 
{
		0,                // font height;
		0,                // width, let Windows choose
	0,0,                // escapement,orientation
	FW_NORMAL,
	0,0,0,              // italic,underline,stikeout
  ANSI_CHARSET,
	0,0,                // precision,clip
	0,                  //Quality
	0,                  // Pitch + Family
	_T("Arial")
};

// Font sizes to control the resolution of the bitmap
static const int font_sizes[NUM_MIPMAPS] = { 14, 12};

////////////////////////////////////////////////////////////////////////
// Function prototypes

unsigned char *BitmapTextOut(TCHAR *str, LOGFONT &font, int &width,int &height);

/////////////////////////////////////////////////////
/// Class Definition

// Constructer

cNameTag::cNameTag(TCHAR *str)
{

#ifdef AGENT
	return;
#endif

	mipmaps = new BITMAPINFO_4DX[NUM_MIPMAPS];
	 
	for (int i = 0; i < NUM_MIPMAPS; i++)
	{
		int width,height;
		
		unsigned char  *src = BitmapTextOut(str, font, width,height);
		unsigned char  *dest = new unsigned char [width * height];
		RLE_encode(dest,src, height,width );
		delete [] src;

		BITMAPINFO_4DX *info = &mipmaps[i];

		font.lfHeight = -font_sizes[i];
		info->address = dest;
		info->h       = width;
		info->w       = height;
		info->palette = 0; // will be overridded by actor classes
	}
}

// Destructor
cNameTag::~cNameTag(void)
{
#ifdef AGENT
	return;
#endif

	if (mipmaps)
	{
		for (int i = 0; i < NUM_MIPMAPS; i++)
		{
			if (mipmaps[i].address)
				delete [] mipmaps[i].address;
			mipmaps[i].address = NULL;
		}
		delete [] mipmaps;
		mipmaps = NULL;
	}
}

/// Return mipmap bitmaps for given index
BITMAPINFO_4DX *cNameTag::Bitmap(unsigned int index)
{
	if (mipmaps)
	{
		index = MIN(NUM_MIPMAPS-1,index);
		return &mipmaps[index];
	}
	else
		return NULL;
}

//////////////////////////////////////////////////////////////////
// Creates a 4DX bitmap of the given str and font. from a Windows bitmap, 
// returns the bitmap data, and its dimensions (width,height)

unsigned char *BitmapTextOut(TCHAR *str, LOGFONT &font, int &width,int &height)
{
	HDC hdcMem;

	// Create compatible memory DC
	{
	 HDC hdcDisplay = GetDC(display->Hwnd());
	 hdcMem = CreateCompatibleDC(hdcDisplay);
	 ReleaseDC(display->Hwnd(), hdcDisplay);
  }

	// Select font
	HFONT   hFont    = CreateFontIndirect(&font);
	HGDIOBJ hOldFont = SelectObject(hdcMem, hFont);

	// determine size of bitmap
	SIZE size;
	GetTextExtentPoint32(hdcMem,(LPCTSTR)str,_tcslen(str),&size);

	// Create Bitmap add bitmap to DC
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcMem,size.cx,size.cy);
	HGDIOBJ hOldBitmap = SelectObject(hdcMem,hBitmap);

	// Write text to mem DC (ie bitmap)
	TextOut(hdcMem,0,0,(LPCTSTR)str, _tcslen(str));
	
	// Allocate memory for 4DX bitmap
	unsigned char *data = new unsigned char[size.cx*size.cy];

	// Set pixels in 4DX bitmap according to pixels in Windows bitmap
	COLORREF t = GetTextColor(hdcMem);
	unsigned char *d = data;
	for( int x = 0; x < size.cx; x++)
	{
		for( int y = 0; y < size.cy; y++)
		{
			COLORREF c = GetPixel(hdcMem,x,y);
			*d++ = (c == t) ? 10 : 0;
		}
	}

	DeleteObject(SelectObject(hdcMem,hOldBitmap));
	DeleteObject(SelectObject(hdcMem,hOldFont));
	DeleteDC(hdcMem);
	
	width = size.cx;
	height = size.cy;
	return data;
}
