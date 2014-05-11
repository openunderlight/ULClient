 // Header file for the Actor class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CNAMETAG_H
#define CNAMETAG_H

#include "4dx.h"
#include "Central.h"

class cNameTag
{
private:
		BITMAPINFO_4DX *mipmaps;

public:
		cNameTag(TCHAR *name);
	    ~cNameTag(void);

		BITMAPINFO_4DX *cNameTag::Bitmap(unsigned int index);

private:
		// copy constructor and assignment operator are
		// private and undefined -> errors if used
		cNameTag(const cNameTag& x);
		cNameTag& operator=(const cNameTag& x);
};

#endif