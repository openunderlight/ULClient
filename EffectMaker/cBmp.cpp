// Copyright Lyra LLC, 1996. All rights reserved. 
// 24-bit bitmap loader -> 16 bit converter class
// Loads a 24-bit bitmap and converts it to 16 bit format

#include <stdio.h>
#include "cBmp.h"

void __cdecl DebugOut(char *args,...);
#define printf DebugOut

//////////////////////////////////////////////////////////////////////
// Class Definition


cBmp::cBmp(const char *fileName)
{
	bits = NULL;
	this->LoadAndConvert(fileName);

	return;
}

cBmp::~cBmp(void)
{
	if (bits)
		delete bits;
}

void cBmp::LoadAndConvert(const char* filename)
{	
	FILE *fh;
	char messenger[128];
	fh = fopen(filename,"rb");
	int i,j,scan_pad,total_size;
	unsigned short red,green,blue,pixel;
	unsigned char *buffer, *src, *dst;

	// open bitmap file
	if (fh == NULL) 
	{
		sprintf(messenger,"Can't open bitmap file %s!\n", filename);
		MessageBox(NULL, messenger, "ERROR", MB_OK);
		return;
	}

	// read in file header
	if ((fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fh)) != 1)
	{
		sprintf(messenger,"Error reading file header for bitmap file %s!\n", filename);
		MessageBox(NULL, messenger, "ERROR", MB_OK);
		return;
	}
	if (fileHeader.bfType != 0x4d42)
	{
		sprintf(messenger,"File %s is not a proper bitmap file!\n", filename);
		MessageBox(NULL, messenger, "ERROR", MB_OK);
		return;
	}

	// read in bitmap header
	if ((fread(&bitmapInfo, sizeof(BITMAPINFO), 1, fh)) != 1)
	{
		sprintf(messenger,"Error reading bitmap header for bitmap file %s!\n", filename);
		MessageBox(NULL, messenger, "ERROR", MB_OK);
		return;
	}

	// seek to beginning of bitmap bits
	fseek(fh, fileHeader.bfOffBits, SEEK_SET);

//	printf("header - ht: %d wd: %d bc: %d compr: %d size: %d clrused: %d clrimp: %d\n",bitmapInfo.bmiHeader.biHeight , bitmapInfo.bmiHeader.biWidth , bitmapInfo.bmiHeader.biBitCount ,bitmapInfo.bmiHeader.biCompression , bitmapInfo.bmiHeader.biSizeImage , bitmapInfo.bmiHeader.biClrUsed ,		bitmapInfo.bmiHeader.biClrImportant);

	// allocate buffers
	bits = new unsigned char[this->Wd()*this->Ht()*BYTES_PER_PIXEL_16];
	scan_pad = (this->Wd()*BYTES_PER_PIXEL_24)%4;
	if (scan_pad == 3)
		scan_pad = 1;
	else if (scan_pad == 1)
		scan_pad = 3;

	// total buffer size = image size + scan line padding
	total_size = (this->Wd()*this->Ht()*BYTES_PER_PIXEL_24) + (this->Ht()-1)*scan_pad;
	buffer = new unsigned char[total_size];	

	/*
	int sz = fileHeader.bfSize;
	int off = fileHeader.bfOffBits;
	printf("sz: %d offset: %d\n",sz,off);
	int pos1 = ftell(fh);
	fseek(fh, 0, SEEK_END);
	int pos2 = ftell(fh);
	fseek(fh, pos1, SEEK_SET);
	printf("pos1: %d pos2: %d size: %d\n",pos1,pos2,total_size);
	int pos3 =  sizeof(BITMAPFILEHEADER);
	int pos4 =  sizeof(BITMAPINFO);
	printf("h1: %d h2: %d\n",pos3,pos4);
*/
  	
	if (fread(buffer, total_size, 1, fh) != 1)
	{
		sprintf(messenger,"Error reading bitmap bits for file %s!\n", filename);
		MessageBox(NULL, messenger, "ERROR", MB_OK);
		return;		
	}

	// start dst at beginning of bottommost scan line
	dst = bits + this->Ht()*this->Wd()*BYTES_PER_PIXEL_16;
    src = buffer;
	//printf("scan pad: %d\n",scan_pad);

	// go through and reduce 24 bit color to 5-6-5 16 bit color
	// note that each scan line must be padded by scan_pad
	for (i=0; i<this->Ht(); i++)
	{	// go to beginning of scan line
		dst -= this->Wd() * BYTES_PER_PIXEL_16;
		for (j=0; j<this->Wd(); j++)
		{
			blue = *src;
			src++;
			green = *src;
			src++;
			red = *src;
			src++;
			red = red >> 3;
			green = green >> 2;
			blue = blue >> 3;
			pixel = (red << 11) | (green << 5) | blue;
			memcpy(dst, &pixel, BYTES_PER_PIXEL_16);
			dst+=BYTES_PER_PIXEL_16;
		}  // now backup to where we started
		dst -= this->Wd() * BYTES_PER_PIXEL_16;
		src +=scan_pad;
	}
	delete buffer;

	return;
}

