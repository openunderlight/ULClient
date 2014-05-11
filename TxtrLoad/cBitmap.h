// Copyright Lyra LLC, 1997. All rights reserved. 
// Header file for 24 bit bitmap loader

#ifdef CBITMAP
#define CBITMAP

#define STRICT

#include <windows.h>

//////////////////////////////////////////////////////////////////////
// Constants

const int BYTES_PER_PIXEL_16 = 2;
const int BYTES_PER_PIXEL_24 = 3;

//////////////////////////////////////////////////////////////////////
// Enumerations

//////////////////////////////////////////////////////////////////////


cl//////////////////////////////////////////////////////////////////////
// Class Definition
ass cBitmap 
{
public:
private:
	BITMAPFILEHEADER	fileHeader;
	BITMAPINFO			bitmapInfo;
	BYTE*				bits;

public:
	cBitmap(const char *fileName);
	~cBitmap(void);

	int Size();
	int Wd();
	int Ht();

protected:
	void LoadAndConvert(const char* filename);

private:

};

#endif