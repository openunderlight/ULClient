// Copyright Lyra LLC, 1997. All rights reserved. 
#include "RLE.h"
#include "memory.h"
/*
Bitmaps encoded with the RLE function below have the following format:

Each scanline (of length _width_) is encoded seperately. Runs of pixels are
determined and inserted into the destination as 2 bytes, run_length first, followed by
the pixel. 

For each scanline, the offset from the start of the dest. bitmap to the start of the
run is saved in an array of  _height_ short ints. The array is at the start of the dest. bitmap.

Clipping info is also saved for each scanline. This is used to reduce overdraw in the renderer.
The longest non-transparent run is determined for each scanline. The start and end offsets (from the 
beginning of the scanline) of this run are saved in the first 2 bytes of the scanline. 

Scanlines that are totally transparent are handled differently: They are given a start of 0, and
no (run,pixel) info or clipping info is saved, saving 4 bytes per scanline and allowing the
renderer to exclude these lines from the rendering process.

*/
const int MAX_RUN = 255;

int RLE_encode(unsigned char *dst, unsigned char *src, int width, int height, 
							 void *extra_info, int extra_size)
{
  unsigned char *d = dst;               // destination pointer 
	unsigned char *s = src;               // source pointer
	unsigned short *starts = (unsigned short *)dst; // scanline start info array
	unsigned char pixel;
  int           run_length;

	d += height * sizeof (short); // make room for scanline starts
	
	if (extra_info)
	{
		memcpy(d,extra_info,extra_size);
		d += extra_size;              // make room for extra information
	}

	int h = 0;
	for (h = 0; h < height; h++)
	{
		starts[h] = d - dst;							// add scanline start offset
		d += 2;														// make room for clipping info
		pixel = *s++;         
		run_length = 1;
		for (int w = 1; w < width; w++)   // scan source scanline and determine run lengths
		{
			if (*s == pixel && run_length < MAX_RUN)  
				run_length++;
			else
			{
				*d++ = run_length;
				*d++ = pixel;
				pixel = *s;
				run_length = 1;
			}
			s++;
		}

		*d++ = run_length;
		*d++ = pixel;
		
		if ((pixel & 0x1F) == 0 && run_length == width) // if scanline is totally transparent
		{
			d -= 4;            // don't include its' run or its clipping info
			starts[h] = 0;     // but set start to 0 to indicate a transparent scanline
		}
	}

	// now re-scan source to insert clipping info into encoded dest.
	s = src;
	for (h = 0; h < height; h++)
	{
		 if (!starts[h])  // if scanline is totally transparent
		{
		  s += width;      /// move onto next scanline in source
			continue;
		}

		int longest_run = 0;
		int longest_run_start = 0;
		int solid_run_length  = 0;
		int w = 0;
		for (w = 0; w < width; w++)
		{
			if (*s++ & 0x1F)      // if source pixel is non_transparent
				solid_run_length++;
			else
			{
				if (solid_run_length > longest_run)
				{
					longest_run       = solid_run_length;
					longest_run_start = w - solid_run_length;
				}
				solid_run_length = 0;
			}
		}
		
		if (solid_run_length > longest_run)
		{
			longest_run = solid_run_length;
			longest_run_start = w - solid_run_length;
		}
		
		dst[starts[h]]   = longest_run_start;
		dst[starts[h]+1] = longest_run_start + longest_run;
	}
	
	//_tprintf("encoded %d bytes to %d bytes\n", width*height, (d - dst));

	return d - dst;
}
