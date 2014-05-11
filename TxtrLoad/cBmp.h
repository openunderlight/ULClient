// Header file for 24 bit bitmap loader
// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef cBMP
#define cBMP

#define STRICT

#include <windows.h>

//////////////////////////////////////////////////////////////////////
// Constants

const int BYTES_PER_PIXEL_16 = 2;
const int BYTES_PER_PIXEL_24 = 3;

//////////////////////////////////////////////////////////////////////
// Enumerations

//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Class Definition

class cBmp 
{

public:
private:
	BITMAPFILEHEADER	fileHeader;
	BITMAPINFO			bitmapInfo;
	BYTE*				bits;

public:
	cBmp(const char *fileName);
	~cBmp(void);

	inline BYTE* Address(void) { return bits; };
	inline int Size(void) { return (this->Wd()*this->Ht()*BYTES_PER_PIXEL_16); };
	inline int Wd(void) { return bitmapInfo.bmiHeader.biWidth; };
	inline int Ht() { return bitmapInfo.bmiHeader.biHeight; };


protected:
	void LoadAndConvert(const char* filename);

private:

};

#endif