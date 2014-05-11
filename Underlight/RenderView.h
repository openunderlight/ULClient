// Copyright Lyra LLC, 1996. All rights reserved.

#ifndef RenderView_H
#define RenderView_H

#include "4dx.h"
#include "cLevel.h"

// assembler #defines for the different screen modes
#if BITS_PER_PIXEL == 8

#define PIX_REG_A           al   
#define CLEAR_PIX_REG_A                   // does not need to clear
#define DOUBLE_PIX_REG_A                  // or double
#define PIX_REG_C           cl
#define CLEAR_PIX_REG_D                   // does not need to clear
#define DOUBLE_PIX_REG_D
#define PIXEL_SIZE            1

#else if BITS_PER_PIXEL == 16

#define PIX_REG_A           ax   
#define CLEAR_PIX_REG_A     xor eax,eax
#define DOUBLE_PIX_REG_A    shl eax,1
#define PIX_REG_C           cx
#define CLEAR_PIX_REG_D     xor edx,edx
#define DOUBLE_PIX_REG_D    shl edx,1
#define PIXEL_SIZE          2
#endif

typedef unsigned int dword;
typedef unsigned short word;
typedef unsigned char  byte;


// data types

struct WallCol
{
	short usedf;
	short usedc;
};



enum surfaces {FLOOR , CEILING };

class cSurface
{

private:

	// texture mapping 'magic numbers'
	float Oa; 
	float Ha; 
	float Va; 

	float Ob; 
	float Hb; 
	float Vb; 

	float Oc; 
	float Hc; 
	float Vc; 
	/////////////////////////////////
	
	float    slope;
	float    base_height;
	int      span_len_shift;
	linedef *baseline;
	bool     no_texture_stretch;

public:	

	cSurface(float a_base_height, float slope, bool no_stretch);
		
	float height_at( float x, float y );
	void render_segment(int row, int xstart, int xend, int sector_id, surfaces surface);
	void setup_for_render(void);
	void calc_pick_coords();

	void set_baseline(linedef *base) {baseline = base;};
	linedef *Baseline(void)          {return baseline;};
	float Slope(void) { return slope; };
};




// Initializes render view, called once before any other call in RenderView
void InitRenderView(int view_width,int view_height, float viewing_distance);

// De-initializes render view, called once before shutdown.
void DeInitRenderView(void);

// Sets up render data in order to render the view,  called each time a new view is rendered
void StartRenderView(UCHAR *viewBuffer, int pitch, cPlayer *povactor);

// Render a linedef
int RenderLinedef(linedef *aLine);

// Called each time after a view is rendered.
void EndRenderView(void);

// calculate the screen row of a point with given  view x coord and height
double fcalc_screen_row( float view_x, float height);
int calc_screen_row( float view_x, float height);

// calculate the screen column for the given transformed point
double fcalc_screen_column(float view_x, float view_y);
int calc_screen_column(float view_x, float view_y);


#endif