// Heart and Soul of the Graphics Engine.
// Copyright Lyra LLC, 1997. All rights reserved. 
int opt_on =0;

#define  MAX(a,b)           ((a)>(b)?(a):(b))
#define  MIN(a,b)           ((a)<(b)?(a):(b))
#include <math.h>

#include "4dx.h"
#include "cActor.h"
#include "cPalettes.h"
#include "cEffects.h"
#include "cPlayer.h"
#include "cActorList.h"

#include "RenderActor.h"
#include "RenderView.h"

// TEMPS!!!
//#include <stdio.h>
//#include "cChat.h"   
//#include "cOrnament.h"
//#include "RLE.h"
//#include "cItem.h"
//extern cChat *display;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// External Globals

extern cEffects *effects;
extern cPaletteManager *shader;
extern cActorList *actors;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals, used here and RenderActor

WallCol *wall_cols;        // column heights of already rendered columns, primay clipper
PIXEL   *ScreenRows[512];  // pointers to start of each screen row
float  *ScaleRatio;        // table of ratios of viewing_distance/zDistance 
unsigned int *DeltaV;      // step length when blitting vertical textures (16:16 fixed point)
int view_pitch;            
int view_width;
int view_height;
float viewing_distance;    // distance from player to screen

float player_height;
int   player_angle; 

int last_zDistance; // Used for rendering erroneus lines

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
const float sine_45 = 0.707106f;
#define MAX_PRE_ROTATED 8
#define NOT_PRE_ROTATED 0xFF
#define MAX_SEGMENTS 32


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stuctures

struct segment        // part of a screen row from start to end column
{
	short start_col;  
	short end_col;
	short sector_id;  // sector whose floor or ceiling will be rendered into this segment
	short surface;    // which surface will be rendered
};          

struct bg_panel      // background panel
{
	int   length;
	UCHAR *address;  // start address of panel in background bitmap
};

struct scanline       // the collection of segments for a given scanline (screen row)
{
	unsigned char num_segments;
	segment segments[MAX_SEGMENTS];
};


struct texture_settings
{
	UCHAR *texture       ; 
	int    texture_width ; 
	int	   texture_height; 

	float  u_length;        // length over which a texture is stretched
	int    u_mask; 
	int    u_offset;
	int    u_max;
	int    u_min;

	float  v_stretch_ratio;  // ratio determines how much to stretch texture (1.0 == no stretching)
	int    v_mask;
	int	   v_offset; 
	
	PIXEL *palette;
	short  palette_id; 

	BOOL   done;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Variables

// general spatial information vars
static int   view_halfwidth;
static float view_width_float,view_halfwidth_float;
static float view_cos, view_sin;
static int   view_half_height;
static float start_delta;
static float player_x,player_y;
static float viewing_dist_inverse;

//#define MAX_INVERSES  8096
//static float inverses[MAX_INVERSES];   

static double cross_product_inverse;  

// vars used as parameters for  blitting functions, see blit_texture_column()
static   UCHAR  *texture_column_ptr;
static   PIXEL  *screencol;
static   int destheight;
static   int deltav,  v_mask;   
static   PIXEL *palette;
static unsigned int v;

// vars used to generate flat segments
static short   *startx;
static int lastfy1, lastfy2;
static int lastcy1, lastcy2;
static short cstarts[MAX_VIEW_HEIGHT];
static scanline *scanlines;
static short current_sector_id;

// vars for blitting of flats
static dword  x_frac;
static byte   x_int; 
static dword  y_frac;
static byte   y_int; 
static PIXEL *endloc; 
static dword  perp_sin,perp_cos;

// vars for pre-rotation optimization
static bool  use_pre_rotated_flats = false;
static int   pr_perp_sin,pr_perp_cos;
static unsigned char pr_bits[MAX_PRE_ROTATED*4096];
static UCHAR *pr_addresses[MAX_PRE_ROTATED];
static byte pre_rotated[MAX_FLATS];  // keeps track of which flats are pre-rotated

// background variables
static short background;
static short prev_background = NO_BITMAP;
static int bg_height  ;
static int bg_width   ;
static int bg_row     ;
static PIXEL *bg_palette;
static PIXEL  bg_below_color, bg_above_color;
static bg_panel bg_panels[12];
static bg_panel bg_screen_panels[4];

// picking variables
static bool  picking = false;
static int pick_row,pick_col;        // screen co-ords
static float pick_x, pick_y;         // world co-ords
static cActor *rendered_actors[64];   // list of actors rendered, used to find an actor at point
static int num_rendered_actors = 0;
static linedef *picked_line; 

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void build_screen_rows(unsigned char *data, int pitch);

// wall rendering
static void  blit_texture_column(void); //(unsigned char *texture_column_ptr,int v,int destheight,int deltav, PIXEL *screencol,PIXEL *palette);

// texture setup for wall rendering
static void init_floor_wall_texture_settings(struct texture_settings &fwall, linedef *line );
static void init_ceiling_wall_texture_settings(struct texture_settings &cwall, linedef *line );

// floor and ceiling segment generation
static void generate_floor_segments(int col, int thisy1,int thisy2);
static void generate_ceiling_segments(int col, int thisy1,int thisy2);
static void init_scanlines(int view_height);
static void de_init_scanlines(void);
static void save_segment(int start_col, int end_col, int row, surfaces surface);

// floor,ceiling rendering
static void render_scanlines(void);
static void render_segment(int y, segment *seg);
static void setup_row_blit(int deltav,int perp_sin, int perp_cos);
static void blit_texture_row(unsigned char *bmap,PIXEL *palette, PIXEL *screen,int textx, int texty, int len);
static void double_blit_texture_row(unsigned char *bmap,PIXEL *palette, PIXEL *screen,int textx, int texty, int len);

// pre rotation
static void pre_rotate_flats(int players_room_id );

// background
static void load_new_background(void);
static void update_background(int player_angle);
static void blit_background(PIXEL *screen,int x, int y, int len);

// picking
static void transform2world( float rx,float ry, float &x, float &y);
static void calc_pick_coords(float  sector_height);
static void calc_floor_pickpoint(sector *sec);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Initialization and de-initialization, called once at program startup and shutdown
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void InitRenderView(int width,int height, float viewing_dist)
{
	int i;
	
	view_width           = width;
	view_halfwidth       = width/2;
	view_width_float     = (float)width;
	view_halfwidth_float = view_width_float/2.0f;
	view_height          = height;
	
	viewing_distance  = viewing_dist;
	viewing_dist_inverse  = 1.0f/viewing_distance;
	
	start_delta     = (0.0f-view_halfwidth_float) * viewing_dist_inverse;
	
	if (startx)  delete [] startx;
	startx  = new short [view_height];
	
	if (wall_cols) delete [] wall_cols;
	wall_cols = new WallCol [view_width];
	
	DeltaV            = new unsigned int  [(MAX_DIST+1)];
	ScaleRatio        = new float          [MAX_DIST+1];
	
	for(i=1;i<MAX_DIST+1;i++)
	{
		ScaleRatio[i] = (viewing_distance/(float)i);
		DeltaV[i]     =  float2int((i*65536.0f)/(viewing_distance));
	}
	
	
/*	// calculate inverses 
	for (i = 0; i < MAX_INVERSES; i++)
	{
		inverses[i] = 1.0f/i;
	}*/
	
	init_scanlines(view_height);
	column_clipper.init(view_width);  // clipper used to reduce actor overdraw
}


void DeInitRenderView(void)
{
	de_init_scanlines();
	
	if (startx)     delete [] startx;     startx    = 0;
	if (wall_cols)  delete [] wall_cols;  wall_cols = 0;
	if (DeltaV)     delete [] DeltaV;     DeltaV    = 0;
	if (ScaleRatio) delete [] ScaleRatio; ScaleRatio= 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Frame Setup and Cleanup, must be called each at the Start and End of each frame rendering
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StartRenderView(UCHAR *viewBuffer, int pitch, cPlayer *player)
{ 
	player_angle      = player->angle;
	player_height     = player->eyeheight;
	player_x		  = player->x;
	player_y		  = player->y;
	view_half_height  = player->VerticalTilt();
	
	view_pitch = pitch;
  
	view_cos = CosTable[player_angle];
	view_sin = SinTable[player_angle];

	int	angle_perp = FixAngle(player_angle+Angle_90);
	perp_sin  = float2int(CosTable[angle_perp]*65536.0f);
	perp_cos  = float2int(SinTable[angle_perp]*65536.0f);
	
	use_pre_rotated_flats = (fabs(view_sin) < sine_45);
	if (use_pre_rotated_flats)
	{
		int pr_angle_perp = FixAngle(player_angle+Angle_180);
		pr_perp_sin  = float2int(CosTable[pr_angle_perp]*65536.0f);
		pr_perp_cos  = float2int(SinTable[pr_angle_perp]*65536.0f);
	}

	build_screen_rows(viewBuffer,view_pitch);
		
	// reset column clipping values
	for (int i=0;  i < view_width; i++)
	{
		wall_cols[i].usedc    = 0;
		wall_cols[i].usedf    = view_height-1;
	}
	
#ifndef AGENT // agents aren't happy about this
	StartRenderActor(viewBuffer, pitch, player);
#endif
	
	// setup background
	background = level->Rooms[level->Sectors[player->sector]->room].background;
	if (background )
	{
		if (background != prev_background)
			load_new_background();
		update_background(player_angle);
	}
	
	// setup pre-rotated flats
	static realmid_t prev_player_room = -1;
	if (player->Room() != prev_player_room) // if player changes rooms
	{
		prev_player_room = player->Room();
		pre_rotate_flats(player->Room());   // generate a new set of pre-rotated flats
	}

	num_rendered_actors = 0;
}


void EndRenderView(void)
{
	render_scanlines();
	EndRenderActor();
	//draw_cursor();
}

static void build_screen_rows(unsigned char *data, int pitch)
{
	for(int i=0; i < view_height; i++)
	{
		ScreenRows[i] = (PIXEL*)data;
		data += pitch;
	}
}

/*
static void draw_cursor(void)
{
	if (cp->DragItem() != NO_ITEM)
		return;

	POINT cursor;
	
	GetCursorPos(&cursor);
	
	if (cursor.x >= view_width || cursor.y >= view_height )
		return;

	BITMAPINFO_4DX *bm = &effects->EffectBitmap(LyraBitmap::CURSOR];
	if (!bm || !bm->address)
		return;

	PIXEL *palette     = shader->GetUnshadedPalette(bm->palette); 

	int x_end = min(view_width,cursor.x + bm->h);
  int y_end = min(view_height-1,  cursor.y + bm->w);
	
	for (int y = cursor.y; y < y_end ; y++)
	{
		PIXEL *dest = ScreenRows[y] + cursor.x;
		PIXEL *src = ((PIXEL *)bm->address) + (y-cursor.y)*bm->h;
		for (int x = cursor.x; x < x_end; x++)
		{
			if (*src)
				*dest = *src;
			src++;
			dest++;
		}
	}

}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General purpose functions used throughout the engine
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// calculate the screen row of a point with given  view x coord and height
double fcalc_screen_row( float view_x, float height)
{
	return view_half_height - (height * viewing_distance)/(view_x);
}

int calc_screen_row( float view_x, float height)
{
	return float2int(fcalc_screen_row(view_x,height));
}

// calculate the screen column for the given transformed point
double fcalc_screen_column(float view_x, float view_y)
{
	return (view_halfwidth_float - ((view_y * viewing_distance) / (view_x))); 
}

int calc_screen_column(float view_x, float view_y)
{
	return float2int(fcalc_screen_column(view_x,view_y));
}

// transform a point from world to view co-ords
static void transform_point(float x, float y, float &x_out, float &y_out)
{	
	// translate
	float nx = x - player_x;
	float ny = y - player_y;
	
	// rotate
  x_out = (nx*view_cos) +(ny *view_sin);
  y_out = (nx*view_sin) -(ny *view_cos);
}


// gives inverse of given number. Optimization: Pentium FDIVS are VERY SLOW
// val MUST be > 0
inline double inverse(double val)
{
	return 1/val; 
/*	if (val >= 0.5f) 
		return inverses[float2int(val)];  
	else 
		return -inverses[float2int(-val)];*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main Rendering Function, Renders walls, generates floor,ceiling segments for rendering
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// helper functions prototypes
inline UCHAR *calc_texture_column(texture_settings &ts, double rdy_ratio);
static void calc_line_equation(linedef *line, sector *sec, enum surfaces surface,
								double &slope, double &start_row);


int RenderLinedef(linedef *line)
{
	if ( line->actor )  // actor rendering
	{ 
 		if (render_actor(line->actor))  // if actor was actually rendered
		{
			rendered_actors[num_rendered_actors++] = line->actor;  // add to list
			if (picking && point_in_actor(line->actor,pick_row,pick_col)) 
			{
				transform2world(line->rx1 - 1, (line->ry1 + line->ry2)/2,pick_x, pick_y);
				picking = false;
			}
		}
		return 0;
	}
	

	texture_settings cwall,fwall;
	fwall.done = cwall.done = FALSE;

	// sector related initializations
	sector *from_sec      = level->Sectors[line->sector];
	sector *to_sec        = level->Sectors[line->facingsector];
 	int  seclight         = from_sec->lightlevel;
 	BOOL outdoors         = from_sec->flags & SECTOR_SKY;
//	current_sector        = from_sec; // global 
	current_sector_id     = line->sector; // global 

	// linedef related intializations
	int flags = line->flags;
	float rx1 = line->rx1;
	float ry1 = line->ry1;
	float rx2 = line->rx2;
	float ry2 = line->ry2;
	float bxdiff = rx2-rx1;

	// straight line vars used to determine rows as a function of columns ie, row = slope*col + constant 
	// In the for-loop below, these are calculated by initializing the rows to the start row, 
	// and incrementing by the slope, each iteration of the loop.  

	if (fabs(line->rx1) < 0.1f)  // make sure x1 is not zero or nearly zero
	{
		line->rx1 = (line->rx1 > 0) ? 0.1f : -0.1f;
	}

 	double fwall_top_slope,   fwall_top_row;    
 	double fwall_bottom_slope,fwall_bottom_row;
 	double cwall_top_slope,   cwall_top_row; 
 	double cwall_bottom_slope,cwall_bottom_row;

	cross_product_inverse  = 1/(line->rx1*ry2 - rx2*ry1); // used in calc_line_equation()
	calc_line_equation(line, from_sec, FLOOR, fwall_bottom_slope,fwall_bottom_row);
	if (!(flags & BOUND)) // normal wall
	{
		calc_line_equation(line, to_sec,  FLOOR,  fwall_top_slope   ,fwall_top_row);
		calc_line_equation(line, from_sec,CEILING,cwall_top_slope   ,cwall_top_row);
		calc_line_equation(line, to_sec,  CEILING,cwall_bottom_slope,cwall_bottom_row);
	}
	else // bounding wall, goes from the floor to the ceiling
	{
		if (flags & LINE_S_IMPASS  && outdoors) // line is an outdoor boundary 
		{	
			fwall_top_row   = fwall_bottom_row;         // so don't draw floor wall at all
			fwall_top_slope = fwall_bottom_slope;

			cwall_bottom_row   = fwall_top_row;            // and move the ceiling to the floor
			cwall_bottom_slope = fwall_top_slope;          // so that only the sky is drawn
		}
		else
		{                                             // make top of floor wall top of ceiling wall
			calc_line_equation(line,from_sec,CEILING,fwall_top_slope,fwall_top_row);
			cwall_bottom_row = cwall_bottom_slope = 0.0;     //  and don't draw ceiling wall. 
		}
		cwall_top_row   = fwall_top_row, 
		cwall_top_slope = fwall_top_slope;
	}
	
	// adjust rows for the case when the wall is clipped on the left 
	{
		double start_col = fcalc_screen_column(line->rx1,line->ry1);
		double clipped_col = (line->rx1 > 0) ? start_col : 0.0;
		clipped_col = MAX(0.0,clipped_col);
		clipped_col = MIN(view_width_float,clipped_col);
		
		double col_diff = (clipped_col - start_col);
		if (col_diff != 0.0)
		{
			fwall_top_row    += col_diff * fwall_top_slope;
			fwall_bottom_row += col_diff * fwall_bottom_slope;
			cwall_top_row    += col_diff * cwall_top_slope;
			cwall_bottom_row += col_diff * cwall_bottom_slope;
		}
	}

	lastfy1=-1;
	lastfy2=-1;
	lastcy1=-1;
	lastcy2=-1;

	int rval = 0;
	
	int BegCol = line->BegCol;
	int EndCol = line->EndCol;

	// vars used to approximate the zdistance of a given wall column.
	double delta = start_delta+(BegCol*viewing_dist_inverse);
	double rdy1 = ry1 + rx1*delta;
	double rdy2 = ry2 + rx2*delta;
	double rx1_delta = rx1*viewing_dist_inverse;
	double rx2_delta = rx2*viewing_dist_inverse;

	int col = BegCol;
	for(; col < EndCol; col++, 	// Start main wall rendering loop 
		fwall_top_row    += fwall_top_slope,
		fwall_bottom_row += fwall_bottom_slope,
		cwall_top_row    += cwall_top_slope,
		cwall_bottom_row += cwall_bottom_slope,
		rdy1 += rx1_delta,
		rdy2 += rx2_delta )
	{
		int usedf = wall_cols[col].usedf;
		int usedc = wall_cols[col].usedc;
		
		if ( usedf == usedc )
		{
			generate_floor_segments(col,-1,-1);
			generate_ceiling_segments(col,-1,-1);
			continue;// this column is full!
		}
		rval = 1;
		bool check_picking = picking && (pick_col == col);
	
		// convert rows to integers
		int fwall_top     = float2int(fwall_top_row);
		int fwall_bottom  = float2int(fwall_bottom_row);
		int cwall_top     = float2int(cwall_top_row);
		int cwall_bottom  = float2int(cwall_bottom_row);

		// generate floor segments
		if (fwall_bottom < usedc) fwall_bottom = usedc;
		if (fwall_bottom < usedf)
		{
			if (check_picking &&  pick_row >= fwall_bottom  && pick_row < usedf) 
				calc_floor_pickpoint(from_sec);
			
			generate_floor_segments(col,fwall_bottom,usedf-1);
			usedf = fwall_bottom;
		}
		else
		{
			generate_floor_segments(col,-1,-1);
		}
		
		// generate floor segments
		if ( cwall_top > usedf )
			cwall_top = usedf;
		if ( cwall_top > usedc )
		{
			// picking 
			if (check_picking ) 
				if (pick_row >= usedc  && pick_row < cwall_top) 
			{	
				if (flags & LINE_S_IMPASS  && outdoors) // line is an outdoor boundary 
				{
					pick_row = float2int(cwall_top_row)+1; // drop point to floor 
					calc_floor_pickpoint(from_sec); 
				}
				else 
				{
					if (from_sec->ceiling_surface)	from_sec->ceiling_surface->calc_pick_coords();
					else calc_pick_coords(from_sec->ceiling_height);
				}
			}

			generate_ceiling_segments(col,usedc,cwall_top-1); 
			usedc = cwall_top;
		}
		else
		{
			generate_ceiling_segments(col,-1,-1);
		}
		
		// render ceiling wall
		if(cwall_bottom > usedc )
		{
			if ( cwall_bottom > usedf )
				cwall_bottom = usedf;
			
			if (check_picking &&  pick_row >= usedc  && pick_row < cwall_bottom)
			{
				pick_row = float2int(fwall_bottom_row)+1; // drop point to floor 
				calc_floor_pickpoint(from_sec); 
				picked_line = line;
			}

			if (!cwall.done)
				init_ceiling_wall_texture_settings(cwall,line);
			
			// determine zDistance;
			double rdy_ratio = rdy1/(rdy1 - rdy2);
			int zDistance = float2int(rx1+(bxdiff*rdy_ratio));
			// Jared 12-13-2000
			// Added error checking here. Greatly improves client stability
			if ((zDistance > MAX_DIST) || (zDistance < 0)) {
//				_stprintf(errbuf,"Error rendering ceiling line from %d to %d", line->from, line->to);
//				INFO(errbuf);
				zDistance = last_zDistance;
			}
			else {
				last_zDistance = zDistance;
			}
			// End new error checking code

			texture_column_ptr = calc_texture_column(cwall,rdy_ratio);

			deltav     = float2int(DeltaV[zDistance]*cwall.v_stretch_ratio);
			v          = cwall.v_offset + (usedc - view_half_height)*deltav;
			//if (cwall_bottom != usedf && v < 0)
			//	v = 0;
			v_mask     = cwall.v_mask;
			
			destheight = cwall_bottom-usedc;
			screencol  = ScreenRows[usedc]+col;
			palette    = outdoors ? shader->GetPalette(cwall.palette_id,zDistance,seclight): cwall.palette;
			blit_texture_column();//(texture_column_ptr,v,destheight,deltav,screencol,palette);
			
			usedc = cwall_bottom;
		}
		

		// render floor wall
		if(fwall_top < usedf)
		{
			if ( fwall_top < usedc ) // if the wall starts above the top of screen
				fwall_top = usedc;
			
			if (check_picking && pick_row >= fwall_top && pick_row < usedf)
			{
				pick_row = float2int(fwall_bottom_row)+1;
				calc_floor_pickpoint(from_sec);
				picked_line = line;
			}
			
			int blit2_start=0; int blit2_end=0; // vars for split blit
			
			// test column span (from fwall_top to usedf) against vertical span clipper
			int span_end = usedf;
			usedf = fwall_top;
			switch (column_clipper.test_span(col,fwall_top,span_end))
			{
			case CONTAINED:   //move usedf to start of clipper span (no blit takes place)
				span_end = fwall_top = usedf = column_clipper.span_start(col);
				break;
			case CONTAINS:		//split blit into 2 
				blit2_start = column_clipper.span_end(col); // 2nd blit starts where clipper span ends
				blit2_end   = span_end;                      // ..and ends where original blit ended
				span_end    = column_clipper.span_start(col);    //  .. which now ends at clipper span start
				break;
			case START_OVERLAP:   
				span_end = column_clipper.span_start(col); // reduce blit length
				break;
			case END_OVERLAP:
				fwall_top = column_clipper.span_end(col); // move start of blit down
				usedf     = column_clipper.span_start(col); 
				break;
			}
			
			if(fwall_top < span_end  )
			{
				if (!fwall.done)
					init_floor_wall_texture_settings(fwall,line);
				
				destheight = span_end - fwall_top;
				screencol  = ScreenRows[fwall_top] + col;
				v_mask     = fwall.v_mask;

				// determine zDistance;

				double rdy_ratio = rdy1/(rdy1 - rdy2);
				int zDistance = float2int(rx1+(bxdiff*rdy_ratio));

				// Jared 12-13-2000
				// Added error checking here. Greatly improves client stability
				if ((zDistance > MAX_DIST) || (zDistance < 0)) {
//					_stprintf(errbuf,"Error rendering floor line from %d to %d", line->from, line->to);
//					INFO(errbuf);
					zDistance = last_zDistance;
				}
				else {
					last_zDistance = zDistance;
				}
				// End new error checking code

				texture_column_ptr = calc_texture_column(fwall,rdy_ratio);
				deltav    = float2int(DeltaV[zDistance]*fwall.v_stretch_ratio);
				
				v  = fwall.v_offset + (fwall_top  - view_half_height)*deltav;
				palette    = outdoors ? shader->GetPalette(fwall.palette_id,zDistance,seclight): fwall.palette;
				blit_texture_column();//(texture_column_ptr,v,destheight,deltav,screencol,palette,v_mask);
				
				if (blit2_end) // if blit is split
				{
					v += (blit2_start-fwall_top)*deltav;
					destheight = blit2_end - blit2_start;
					screencol = ScreenRows[blit2_start]+col;
					blit_texture_column();
				}
			}
		}
		wall_cols[col].usedf=usedf;
		wall_cols[col].usedc=usedc;
	} // end for loop
	
	if(rval)
	{
		generate_ceiling_segments(col,-1,-1);
		generate_floor_segments(col,-1,-1);
	}   
			
	return rval ;
}

// Blit a wall texture to the screen, vars are passed in as statics to save time

static void blit_texture_column()
//(unsigned char *texture_column_ptr,int v,int destheight,int deltav, PIXEL *screencol, PIXEL *palette)
{
#ifdef UL_DEBUG
	dword height;
#endif

	_asm { 
		// loop instructions re-ordered from orignal, to eliminate AGI's and Read after Write conflicts
#ifndef UL_DEBUG
			mov         ebx, v
			mov         eax,ebx
			cdq
			and         edx,0FFFFh
			add         eax,edx
			mov         edx,eax
			
			mov    ecx,destheight 
			cmp    ecx,0
			jle    short outtahere
			push   ebp
			mov    edi,screencol  
			mov    esi,palette
			mov    ebp,texture_column_ptr;
			sub    edi,view_pitch
			xor    eax,eax
			
			sar  edx,16
			add  ebx,deltav
			and  edx,v_mask
tloop:
		CLEAR_PIX_REG_A                     
			mov  al, [ebp][edx]
			mov  edx,ebx
			add  edi, view_pitch
			sar  edx,16
			mov  PIX_REG_A,[esi][eax*PIXEL_SIZE]  
			add  ebx,deltav
			and  edx,v_mask
			mov  [edi],PIX_REG_A
			dec  ecx
			jnz  tloop
			pop  ebp
outtahere:

#else
		// debug version: preserves ebp to make stack trace possible
			mov         ebx, v
			mov         eax,ebx
			cdq
			and         edx,0FFFFh
			add         eax,edx
			mov         edx,eax
			
			mov    ecx,destheight 
			cmp    ecx,0
			jle    short outtahere
			mov    height,ecx
			mov    edi,screencol  
			mov    esi,palette
			mov    ecx,texture_column_ptr;
			sub    edi,view_pitch
	
			xor    eax,eax
			sar  edx,16
			add  ebx,deltav
			and  edx,v_mask
tloop:
				CLEAR_PIX_REG_A                     
					mov  al, [ecx][edx]
					mov  edx,ebx
					add  edi, view_pitch
					sar  edx,16
					mov  PIX_REG_A,[esi][eax*PIXEL_SIZE]  
					and  edx,v_mask
					add  ebx,deltav
					mov  [edi],PIX_REG_A
					dec  height
					jnz  tloop
outtahere:
#endif
	}
 }

inline UCHAR *calc_texture_column(texture_settings &ts, double rdy_ratio)
{
	int column = (ts.u_offset + float2int(ts.u_length*rdy_ratio)); // rdy_ratio range 0..1.0f

	// Now restrict column to min..max to prevent unwanted 'lines' in wall textures
	// caused by inaccuracies in rdy_ratio (which cannot be helped). Note, engine works fine without
	// this clamping.
	if (column < ts.u_min)	
		column = ts.u_min;
	else
		if( column > ts.u_max)
		column = ts.u_max;

	column  &=  ts.u_mask; // & to tile
	return  ts.texture + (column * ts.texture_height);
}



static void calc_line_equation(linedef *line, sector *sec, enum surfaces surface,
																		double &slope, double &start_row)
{
	float h1; 
	float h2; 
	
	if (surface == FLOOR)
	{
		h1 = player_height-sec->FloorHt(line->x1(),line->y1());
		h2 = player_height-sec->FloorHt(line->x2(),line->y2()); 
	}
	else
	{
		h1 = player_height-sec->CeilHt(line->x1(),line->y1());
		h2 = player_height-sec->CeilHt(line->x2(),line->y2()); 
	}
	
	float x1 = line->rx1;
//	float y1 = line->ry1;
	float x2 = line->rx2;
//	float y2 = line->ry2;

	slope = (x2*h1 - x1*h2)*cross_product_inverse; 
  start_row = fcalc_screen_row(x1,-h1);
}


static void init_floor_wall_texture_settings(struct texture_settings &fwall, linedef *line )
{
 	int flags = line->flags;
	sector *from_sec      = level->Sectors[line->sector];
	sector *to_sec        = level->Sectors[line->facingsector];

	fwall.done = TRUE;

	int bitmap = line->bitmap;
	fwall.texture        = level->texture_info[bitmap].address;
	fwall.texture_width  = level->texture_info[bitmap].h;
	fwall.texture_height = level->texture_info[bitmap].w;

	fwall.u_offset = line->fwall_u_offset;
	fwall.u_mask	 = fwall.texture_width-1;  // mask to tile texture
	fwall.u_length = (flags & LINE_STRETCH_FWALL_HORZ) ?  float(fwall.texture_width) : line->length;
	fwall.u_length--;

	if (flags & LINE_FLIP_FWALL_HORZ)
	{
		fwall.u_offset += fwall.u_mask;
		fwall.u_length = -fwall.u_length;

		fwall.u_max = fwall.u_offset;
		fwall.u_min = fwall.u_max + float2int(fwall.u_length);  
	}
	else
	{
		fwall.u_min = fwall.u_offset;
		fwall.u_max = fwall.u_min + float2int(fwall.u_length);
	}

	fwall.v_stretch_ratio= 1.0; 
	
	if (flags & LINE_STRETCH_FWALL_VERT)
	{
		float tosec_floor_height;
		if (flags & BOUND) 
			tosec_floor_height = from_sec->ceiling_height;
		else if (to_sec->SlopingFloor())
			tosec_floor_height = max(to_sec->FloorHt(line->x1(),line->y1()),to_sec->FloorHt(line->x2(),line->y2()));
		else
			tosec_floor_height = to_sec->floor_height;

		fwall.v_stretch_ratio    = fwall.texture_height/(tosec_floor_height - from_sec->floor_height);
	}
	fwall.v_mask = fwall.texture_height-1;

	float height_origin;
	if (flags & LINE_FLIP_FWALL_VERT)
	{
		fwall.v_stretch_ratio = -fwall.v_stretch_ratio;
		height_origin = from_sec->floor_height;
	}
	else
		height_origin = to_sec->floor_height;

	fwall.v_offset = float2int((height_origin - player_height + line->fwall_v_offset)
															* fwall.v_stretch_ratio*65536.0);

	fwall.palette_id = level->texture_info[bitmap].palette;
	// no distance shading for non-sky sectors
	if (!(from_sec->flags & SECTOR_SKY))
		fwall.palette   = shader->GetPalette(fwall.palette_id,0,from_sec->lightlevel); 
}


// ceiling wall texture related initializations
static void init_ceiling_wall_texture_settings(struct texture_settings &cwall, linedef *line)
{
	int flags = line->flags;
	sector *from_sec      = level->Sectors[line->sector];
	sector *to_sec        = level->Sectors[line->facingsector];
	cwall.done = TRUE;

	int bitmap = line->cwall_bitmap;

	cwall.texture        = level->texture_info[bitmap].address;
	cwall.texture_width  = level->texture_info[bitmap].h;
	cwall.texture_height = level->texture_info[bitmap].w;

	cwall.u_offset = line->cwall_u_offset;
	cwall.u_mask	 = cwall.texture_width-1;  // mask to tile texture
	cwall.u_length = (flags & LINE_STRETCH_CWALL_HORZ) ?  float(cwall.texture_width) : line->length;
	cwall.u_length--;

	if (flags & LINE_FLIP_CWALL_HORZ)
	{
		cwall.u_offset += cwall.u_mask;
		cwall.u_length = -cwall.u_length;
		cwall.u_max = cwall.u_offset;
		cwall.u_min = cwall.u_max + float2int(cwall.u_length); 
	}
	else
	{
		cwall.u_min = cwall.u_offset;
		cwall.u_max = cwall.u_min + float2int(cwall.u_length); 
	}
	
	cwall.v_stretch_ratio = 1.0;      
	if (flags & LINE_STRETCH_CWALL_VERT)
	{
		float to_sec_ceiling_height;
		if (to_sec->SlopingCeiling())
			to_sec_ceiling_height = min(to_sec->CeilHt(line->x1(),line->y1()),to_sec->CeilHt(line->x2(),line->y2()));
		else
			to_sec_ceiling_height = to_sec->ceiling_height;
		cwall.v_stretch_ratio  = cwall.texture_height/(from_sec->ceiling_height - to_sec_ceiling_height);
	}

	cwall.v_mask   = cwall.texture_height-1;

	if (flags & LINE_FLIP_CWALL_VERT)
	{
		cwall.v_stretch_ratio = -cwall.v_stretch_ratio;
		cwall.v_offset = float2int((to_sec->ceiling_height - player_height + 
																 line->cwall_v_offset) * cwall.v_stretch_ratio * 65536.0);
	}
	else
		cwall.v_offset = float2int((from_sec->ceiling_height - player_height +
		                             line->cwall_v_offset) * cwall.v_stretch_ratio * 65536.0) ;

	cwall.palette_id  = level->texture_info[bitmap].palette;
	// no distance shading for non-sky sectors
	if (!(from_sec->flags & SECTOR_SKY))  
		cwall.palette   = shader->GetPalette(cwall.palette_id,0,from_sec->lightlevel);  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generation of ceiling and floor segments
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void save_segments(int end_col, int start_row, int end_row, surfaces surface,short starts[])
{
	for (int row = start_row; row <= end_row; row++)
	{
		int start_col  =  starts[row];
		if ( end_col-start_col > 0 ) 
		{
			
			save_segment(start_col,end_col,row, surface);
		}
	}
}

static void add_starts(int x, int start_row, int end_row, short startx[])
{
	for(int row = start_row ;row <= end_row ;row++ )
	{
		startx[row]=x;
	}
}

static void generate_floor_segments(int col, int thisy1,int thisy2)
{
	int y;
	
	if (( thisy1 == -1 && lastfy1 == -1)) return;
	
	if (thisy1 < lastfy1)
	{
		// add starts for all the Y's from thisy1 to min(thisy2,lasty1-1);
		if ( thisy1 > -1 )
		{
			y = MIN(thisy2,lastfy1-1);
			add_starts(col,thisy1,y,startx);
		}
	}
	else if ( thisy1 > lastfy1)
	{
		// actually draw horzslices from lasty1 to min(lasty2,thisy1-1)
		if ( lastfy1 > -1)
		{
			y=MIN(lastfy2,thisy1-1);
			save_segments(col,lastfy1,y, FLOOR, startx);
		}
	}
	if ( thisy2 < lastfy2)
	{
		// actually draw horzslices from max(thisy2+1,lasty1) to lasty2)
		y=MAX(thisy2+1,lastfy1);
		save_segments(col,y, lastfy2,FLOOR,startx);
	}
	else if ( thisy2 > lastfy2 )
	{
		// add starts from max(thisy1,lasty2+1) to thisy2
		y = MAX(thisy1,lastfy2+1);
		add_starts(col,y,thisy2,startx);
	}
	lastfy1  =thisy1;
	lastfy2  =thisy2;
}

static void generate_ceiling_segments(int col, int thisy1,int thisy2)
{
	int y;
	
	if (( thisy1 == -1 && lastcy1 == -1)) return;
	
	if (thisy1 < lastcy1)
	{
		// add starts for all the Y's from thisy1 to min(thisy2,lasty1-1);
		if ( thisy1 > -1 )
		{
			y = MIN(thisy2,lastcy1-1);
			add_starts(col,thisy1,y,cstarts);
		}
	}
	else if ( thisy1 > lastcy1)
	{
		// actually draw horzslices from lasty1 to min(lasty2,thisy1-1)
		if ( lastcy1 > -1)
		{			 
			y=MIN(lastcy2,thisy1-1);
			save_segments(col,lastcy1,y,CEILING,cstarts);
		}
	}
	
	if ( thisy2 < lastcy2)
	{
		// actually draw horzslices from max(thisy2+1,lasty1) to lasty2)
		y=MAX(thisy2+1,lastcy1);
		save_segments(col,y,lastcy2,CEILING,cstarts);
	}
	else if ( thisy2 > lastcy2 )
	{
		// add starts from max(thisy1,lasty2+1) to thisy2
		y = MAX(thisy1,lastcy2+1);
		  add_starts(col,y,thisy2,cstarts);
	}
	lastcy1  =thisy1;
	lastcy2  =thisy2;
}

inline int mergeable(segment *seg, surfaces surface)
{
	return (seg->sector_id == current_sector_id) && (seg->surface == surface);
	/*  if (y < view_half_height)
		return seg->seg_sector->OnTop == current_sector->OnTop && seg->seg_sector->lightlevel == current_sector->lightlevel;
		else
		return seg->seg_sector->OnBottom == current_sector->OnBottom && seg->seg_sector->lightlevel == current_sector->lightlevel;
	*/
}

static void save_segment(int start_col, int end_col, int row, surfaces surface)
{	
	short start = start_col; short end = end_col;

	int num_segments    = scanlines[row].num_segments;
	if (num_segments >= MAX_SEGMENTS)
		return;

	segment *segments   = scanlines[row].segments;
	segment *newseg     = &segments[num_segments];  // assume new segment is at end of list
	
	segment *seg_behind = NULL;
	for (int s = 0; s < num_segments; s++)
	{
		segment *seg = &segments[s];

		if (end_col > seg->start_col) // if segment to be saved  comes after seg.
		{
			seg_behind = seg;
			continue;
		}
		
		if (seg_behind && (start_col == seg_behind->end_col) && mergeable(seg_behind,surface))
		{
			seg_behind->end_col = end_col;
			if (end_col == seg->start_col && mergeable(seg,surface)) // the segment to be saved connects 2 segments in list
			{
				// merge 2 segments in list
				seg_behind->end_col = seg->end_col;
				memmove(seg,seg+1,sizeof segments[0]*(num_segments-s-1));	
				scanlines[row].num_segments--;
			}
			return;
		}
		
		if (end_col == seg->start_col && mergeable(seg,surface))
		{
			seg->start_col = start_col;
			return;
		}
		// segment to be saved could not be merged, so add it to list
		memmove(seg+1,seg,sizeof segments[0]*(num_segments-s)); // make space for it 
		newseg = seg;
		seg_behind = NULL;
		break;  
	}
	
	if (seg_behind && (start_col == seg_behind->end_col) && mergeable(seg_behind,surface)) // can it be merged with last seg.
	{
		seg_behind->end_col = end_col;
		return;
	}
	
	// add new segment
	newseg->start_col  = start_col;
	newseg->end_col    = end_col;	
	newseg->sector_id  = current_sector_id;
	newseg->surface    = surface;
	scanlines[row].num_segments++;
}

static void init_scanlines(int view_height)
{
	scanlines = new scanline[view_height];
	for (int i = 0; i < view_height; i++)
		scanlines[i].num_segments = 0;
}

static void de_init_scanlines()
{
	if(scanlines) delete [] scanlines; scanlines = 0;
}	


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Floor and Ceiling Rendering
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Loops thro' all the screen row segments and calls a render_segment function, depending on the type of 
// surface, and if has a slope or not.
static void render_scanlines(void)
{
	for (int row = 0; row < view_height; row++)
	{
		int num_segments =  scanlines[row].num_segments;

		scanlines[row].num_segments = 0;
		
		segment *segments = scanlines[row].segments;
		for (int s = 0; s < num_segments; s++)
		{	
			segment *seg = &segments[s];
			sector *seg_sector = level->Sectors[seg->sector_id];

			if( seg_sector->floor_surface && seg->surface == FLOOR)
			{
				seg_sector->floor_surface->render_segment(row, seg->start_col, seg->end_col, seg->sector_id,surfaces(seg->surface));
				continue;
			}

			if ( seg_sector->ceiling_surface && seg->surface == CEILING)
			{
				seg_sector->ceiling_surface->render_segment(row, seg->start_col, seg->end_col, seg->sector_id,surfaces(seg->surface));
				continue;
			}
			//render_segment(row, seg->start_col, seg->end_col, seg->sector_id,surfaces(seg->surface));
			render_segment(row, seg);  // plain vanilla render segment
		}
	}
}

static void render_segment(int y, segment *seg)
{
	unsigned char *bm;
	float height;
	int xanim;
	int yanim;
	int flat_id;
	
	double ydiff;
	
	PIXEL *palette;
	PIXEL *scr;
	short palette_id;
	bool  stretch_texture;
	
	int x   =   seg->start_col;
	int len =   seg->end_col - x;
	
	if (len < 1)
		return;
	
	scr    = ScreenRows[y]+x;
	sector * seg_sector = level->Sectors[seg->sector_id];
	
	if (background && (seg_sector->flags & SECTOR_SKY) && seg->surface == CEILING) 
	{
		blit_background(scr,x,y,len);	
		return;	
	}
	
	if ( y < view_half_height )
	{
		height =  seg_sector->ceiling_height-player_height;
		ydiff  = inverse( view_half_height-y);
		flat_id = seg_sector->OnTop;
		seg_sector->GetCeilingTextureOffsets(xanim,yanim);
		stretch_texture = !(seg_sector->flags & SECTOR_NO_STRETCH_CEILING);
	}
	else
	{
		height =  player_height - seg_sector->floor_height; 
		ydiff  = inverse(y-view_half_height+1);
		seg_sector->GetFloorTextureOffsets(xanim,yanim);
		stretch_texture = !(seg_sector->flags & SECTOR_NO_STRETCH_FLOOR);
		flat_id = seg_sector->OnBottom;
	}
	
	float view_x = (float)fabs((height * viewing_distance)*ydiff);
	float view_y = (view_x*(view_halfwidth_float-(float)x))*viewing_dist_inverse;
	
	if ( view_x >= MAX_DIST)
	{
		memset(scr,0,len*sizeof(PIXEL));    // draw a black line.
		return;
	}
	
	int view_x_int  = float2int(view_x);
	int delta_v = DeltaV[view_x_int];
	
	float world_x,world_y;
	transform2world(view_x,view_y, world_x,world_y);
	
	if (use_pre_rotated_flats && pre_rotated[flat_id] != NOT_PRE_ROTATED)
	{
		float swap = world_x;
		world_x = -world_y;
		world_y  = swap;
		
		bm  = pr_addresses[pre_rotated[flat_id]];
		setup_row_blit(delta_v,pr_perp_sin,pr_perp_cos);
		
		int temp = xanim;
		xanim = -yanim;
		yanim = temp;
	}
	else
	{
		bm  = level->flat_info[flat_id].address;
		setup_row_blit(delta_v,perp_sin,perp_cos);
	}
	
	if (!(seg_sector->flags & SECTOR_SKY)) // if we are indoors...
		view_x_int = 0;												 // ..don't do distance shading
	palette_id = level->flat_info[flat_id].palette;
	palette = shader->GetPalette(palette_id,view_x_int,seg_sector->lightlevel);
	
	if (stretch_texture)
	{
		const	float shift=65536.0f/2.0f; 
		int text_x = float2int(world_x*shift);
		int text_y = float2int(world_y*shift);
		double_blit_texture_row(bm,palette,scr,text_x+(xanim<<16),text_y+(yanim<<16), len );
	}
	else
	{
		const	float shift=65536.0f; 
		int text_x = float2int(world_x*shift);
		int text_y = float2int(world_y*shift);
		blit_texture_row(bm,palette,scr,text_x+(xanim<<16),text_y+(yanim<<16),len);
	}
}

void setup_row_blit(int deltav, int perp_sin, int perp_cos)
{
	 _asm {
	    pushad
				
        mov  ebx,deltav  
        mov  eax,[perp_sin]
        mov  edx,0
        imul ebx              ; multiply deltav by sine
        add  eax,8000h        ; round it.
        adc  edx,0
        cmp  dx,255
        jle  storex
        mov  dx,255           ; its too big anyway, lets just do it.
				
storex:
			; store fraction and into into inner loop below.
        mov  ax,0                   ; clear out bottom of eax.
				
		mov x_frac, eax
		mov x_int,dl
				
		mov eax,[perp_cos]
        mov edx,0
        imul ebx
        add  eax,8000h
        adc  edx,0
        mov  ax,0                   ; clear out bottom of eax.
        cmp  dx,255
        jle  storey
        mov  dx,255                 ; int portion might as well be 255. its too far anyway!
				
storey:
		mov y_frac,eax
		mov y_int,dl 
				
        ; store eax into the loop below.
        ; get the end of our screen.
			//	mov endloc, ecx
				popad
	 }
}


#ifndef UL_DEBUG
void blit_texture_row(unsigned char *bmap,PIXEL *palette, PIXEL *screen,int textx, int texty, int len)
{
	endloc = screen + len - 1;
	_asm {
			push ebp
			mov  esi,bmap
			mov  edi,palette
			mov  eax,screen
			mov  ebx,textx
			mov  ecx,texty
			mov  ebp,eax
			
			mov  dx,bx                       // finagle textx so that fraction is in upper 16 bits
			sal  edx,16
			sar  ebx,16
			or   ebx,edx
			
			mov  dx,cx                       // finagle texty so that fraction is in upper 16 bits
			sal  edx,16
			sar  ecx,16
			or   ecx,edx
			
			mov  eax,0                       // clear out top portion for indexing
			mov al,bl                        // store the ints in ax
			mov ah,cl                        //
			and eax,0x3F3F                   // and it out
			mov edx,0                        // clear out so we can index from it
			sub ebp,PIXEL_SIZE
hloop:	
			mov  dl,[esi][eax]						// get pixel from texture
			add  ecx,y_frac							// add a reversed fraction 	          
			adc  ah,y_int							// add integer part 
			add  ebp,PIXEL_SIZE 					// increment screen index	
			mov  PIX_REG_C, [edi][edx*PIXEL_SIZE]	// get shaded pixel  
			add  ebx,x_frac							// just like x coord above	
			adc  al,x_int
			mov  [ebp],PIX_REG_C					// put pixel onto screen
			and  eax,0x3F3F							// and with mask to wrap 	
			cmp  ebp,endloc							// has the end screen location been reached?  
			jnz hloop								// continue if it has not	
			pop ebp
	}
}


void double_blit_texture_row(unsigned char *bmap,PIXEL *palette, PIXEL *screen,int textx, int texty, int len)
{
	static int odd;
	odd = (len & 1);
	endloc = screen + (len & 0xFFFE);
	_asm {
			push ebp
			mov  esi,bmap
			mov  edi,palette
			mov  eax,screen
			mov  ebx,textx
			mov  ecx,texty
			mov  ebp,eax
			
			mov  dx,bx                       // finagle textx so that fraction is in upper 16 bits
			sal  edx,16
			sar  ebx,16
			or   ebx,edx
			
			mov  dx,cx                       // finagle texty so that fraction is in upper 16 bits
			sal  edx,16
			sar  ecx,16
			or   ecx,edx
			
			mov  eax,0                       // clear out top portion for indexing
			mov al,bl                        // store the integers in ax
			mov ah,cl                        //
			and eax,0x3F3F                   // and it out
			mov edx,0                        // clear out so we can index from it
			cmp ebp,endloc					 // if there only one pixel to be displayed
			je  do_odd						 // goto do_odd
			
hloop:	
			mov  dl,[esi][eax]				 		// get pixel from texture
			add  ecx,y_frac							// add a reversed fraction 	          
			adc  ah,y_int							// add integer part 
			add  ebp,PIXEL_SIZE*2					// increment screen index	
			mov  PIX_REG_C, [edi][edx*PIXEL_SIZE]	// get shaded pixel  
			add  ebx,x_frac							// just like x coord above	
			adc  al,x_int
			mov [ebp - PIXEL_SIZE*2], PIX_REG_C		// put pixel onto screen
			mov [ebp - PIXEL_SIZE],PIX_REG_C		// put again at next location	
			and  eax,0x3F3F							// and with mask to wrap 	
			cmp  ebp,endloc							// has the end screen location been reached?  
			jb  hloop								// continue if it has not	
			
			cmp odd,0								// if there is an even number of pixels to display
			je skip_odd								// skip over displaying the last pixel
do_odd:
			mov  dl,[esi][eax]						// get pixel from texture
			mov  PIX_REG_C, [edi][edx*PIXEL_SIZE]	// get shaded pixel  
			mov  [ebp],PIX_REG_C					// put pixel onto screen
skip_odd:
		pop ebp
	}
}

#else  
 // debug version : preserve ebp for stack trace

void blit_texture_row(unsigned char *bmap,PIXEL *palette, PIXEL *screen,int textx, int texty, int len)
{
	endloc = screen + len - 1;
	 _asm {
			 mov  esi,bmap
			 //mov  edi,palette
			 mov  eax,screen
			 mov  ebx,textx
			 mov  ecx,texty
			 mov  edi,eax
			 
			 mov  dx,bx                       // finagle textx so that fraction is in upper 16 bits
			 sal  edx,16
			 sar  ebx,16
			 or   ebx,edx
			 
			 mov  dx,cx                       // finagle texty so that fraction is in upper 16 bits
			 sal  edx,16
			 sar  ecx,16
			 or   ecx,edx
			 
			 //add bx,[__xplay]              // add our offsets to pre-anded coords
			 //add cx,[__yplay]             
		     mov  eax,0                       // clear out top portion for indexing
			 mov al,bl                        // store the ints in ax
			 mov ah,cl                      
			 and eax,0x3F3F                    // and it out
			 sub edi,PIXEL_SIZE
hloop:	
			 mov  edx,0									// clear out so we can index from it
			 mov  dl,[esi][eax]							// get pixel from texture
			 add  ecx,y_frac							// add a reversed fraction 	          
			 adc  ah,y_int								// add integer part 
			 add  edi,PIXEL_SIZE 						// increment screen index	
			 //mov  PIX_REG_C, [edi][edx*PIXEL_SIZE]	// get shaded pixel  
			 shl  edx,1
			 add  edx, palette
			 mov  PIX_REG_C,[edx];

			 add  ebx,x_frac							// just like x coord above	
			 adc  al,x_int
			 mov  [edi],PIX_REG_C						// put pixel onto screen
			 and  eax,0x3F3F							// and with mask to wrap 	
			 cmp edi,endloc								// has the end screen location been reached?  
			 jnz hloop									// continue if it has not	
	 }
 }


void double_blit_texture_row(unsigned char *bmap,PIXEL *palette, PIXEL *screen,int textx, int texty,int len)
{
  int odd = (len & 1);
	endloc = screen + (len & 0xFFFE);
	 _asm {
			 mov  esi,bmap
			 //mov  edi,palette
			 mov  eax,screen
			 mov  ebx,textx
			 mov  ecx,texty
			 mov  edi,eax
			 
			 mov  dx,bx                       // finagle textx so that fraction is in upper 16 bits
			 sal  edx,16
			 sar  ebx,16
			 or   ebx,edx
			 
			 mov  dx,cx                       // finagle texty so that fraction is in upper 16 bits
			 sal  edx,16
			 sar  ecx,16
			 or   ecx,edx
			 
		   mov  eax,0                       // clear out top portion for indexing
			 mov al,bl                        // store the ints in ax
			 mov ah,cl                        //
			 and eax,0x3F3F                    // and it out
			 cmp edi,endloc
			 je  do_odd
			// sub edi,PIXEL_SIZE*2
			
hloop:	
			 mov  edx,0							    // clear out so we can index from it
			 mov  dl,[esi][eax]						// get pixel from texture
			 add  ecx,y_frac						// add a reversed fraction 	          
			 adc  ah,y_int							// add integer part 
			 add  edi,PIXEL_SIZE*2 					// increment screen index	
			 //mov  PIX_REG_C, [edi][edx*PIXEL_SIZE]// get shaded pixel  
			 shl  edx,1
			 add  edx, palette
			 mov  PIX_REG_C,[edx];

			 add  ebx,x_frac						// just like x coord above	
			 adc  al,x_int
			 mov [edi - PIXEL_SIZE*2], PIX_REG_C	// put pixel onto screen
			 mov [edi - PIXEL_SIZE],PIX_REG_C
			 and  eax,0x3F3F						// and with mask to wrap 	
  			 cmp edi,endloc							// has the end screen location been reached?  
			 jb  hloop								// continue if it has not	
			 
			 cmp odd,0 
			 je skip_odd
do_odd:
			 mov  edx,0                              // clear out so we can index from it
			 mov  dl,[esi][eax]						// get pixel from texture
			 shl  edx,1
			 add  edx, palette
			 mov  PIX_REG_C,[edx];
			 mov  [edi],PIX_REG_C
skip_odd:

	 }
 }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-rotation of flat textures optimization
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void pre_rotate_flat(int flat_id,int pr_id);

static void pre_rotate_flats(int players_room_id  )
{

	memset(pre_rotated,NOT_PRE_ROTATED, MAX_FLATS);
	int num_pre_rotated = 0;

	//find the first MAX_PRE_ROTATED flats that will be pre-rotated
	for (int s = 0; s < MAX_SECTORS; s++)
	{ 
		sector *sec = level->Sectors[s];
		if (sec && sec->room == players_room_id)
		{
			if ((pre_rotated[sec->OnBottom] == NOT_PRE_ROTATED) && !sec->SlopingFloor()) 
			// if flat is not already pre-rotated and surface is not sloping
			{
				pre_rotated[sec->OnBottom] = num_pre_rotated++; 
				pre_rotate_flat(sec->OnBottom, pre_rotated[sec->OnBottom]);
				if (num_pre_rotated >= MAX_PRE_ROTATED)
					break;
			}

 			if (pre_rotated[sec->OnTop] == NOT_PRE_ROTATED && !sec->SlopingCeiling())
			{
				pre_rotated[sec->OnTop] = num_pre_rotated++;
				pre_rotate_flat(sec->OnTop,pre_rotated[sec->OnTop] );
				if (num_pre_rotated >= MAX_PRE_ROTATED)
					break;
			}
		}
	}
}

void pre_rotate_flat(int flat_id,int pr_id)
{
	UCHAR *src  = level->flat_info[flat_id].address;
	UCHAR *dest = pr_addresses[pr_id] = &pr_bits[ ((pr_id/4)*4096*4) + (pr_id%4)*64];
	for (int i = 0; i <64; i++)
	{
		for (int j = 0; j<64; j++)
		{
			dest[63-j] = src[j*256];
		}	
		dest += 256; 
		src++;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cSurface methods, for sloped surfaces, and flats that have textures aligned with a given baseline
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// first some helper structs and funtions
struct vector
{
	float x;
	float y;
	float z;
	inline vector(float X,float Y, float Z) { x=X; y=Y; z=Z;};
	inline vector() { x= y= z= 0.0f ;};
	inline void scalar_multiply(float X,float Y,float Z) { x *= X; y *= Y; z *= Z; };
	void resize(float new_size);
};

void subtract(vector &A, vector &B, vector &C)
{
  C.x = A.x - B.x;
  C.y = A.y - B.y;
  C.z = A.z - B.z;
}

void vector::resize(float new_size)
{
	float length = (float)sqrt(x*x + y*y + z*z);
	float len_ratio = new_size/length;
	scalar_multiply(len_ratio,len_ratio,len_ratio);
}

void cross_product(vector &A, vector &B, vector &C)
{
  C.x = (A.y * B.z) - (A.z * B.y);
  C.y = (A.z * B.x) - (A.x * B.z);
  C.z = (A.x * B.y) - (A.y * B.x);
}

float dot_product(vector &A, vector &B)
{
  return A.x*B.x + A.y*B.y + A.z*B.z;
}

void cSurface::setup_for_render(void)
{

  // determine the span length based on the view angle of the baseline 
	// (if the player is parallel to the slope, we can use larger span lens, 
	//  if the player is almost perpendicular, we need to shorten the spans for more accuracy.)
	float sector_sine = (baseline->rx2 - baseline->rx1)/baseline->length;

	if(slope)
		span_len_shift = (fabs(sector_sine) < sine_45) ? 6 : 4;
	else
		span_len_shift = 9;

	/*
	assume a RIGHT-handed 3D coordinate system, with X positive
	to the right, Y positive disappearing into the screen/distance,
	and Z positive up.  
	*/
	
	float h1 = base_height - player_height ;

	vector V0(baseline->ry1,-baseline->rx1,h1);
	vector V1(baseline->ry2,-baseline->rx2,h1);

	vector P  = V0;
  
	float sec_x=baseline->x1()+ baseline->SINE;
	float sec_y=baseline->y1()- baseline->COS;
	float h2 = height_at(sec_x,sec_y) + h1;
	transform_point(sec_x,sec_y,sec_x,sec_y);
	vector V3(sec_y,-sec_x,h2);

	vector M;  
	vector N;  
	/*if (use_pre_rotated_flats )
	{
		subtract(V3,V0,M);
		subtract (V1,V0,N);
	}
	else*/
	{ 
		subtract(V1,V0,M);
		subtract (V3,V0,N);
	}

	float stretch_factor = 1.0f; //no_texture_stretch ? 1.0f : 2.0f;
	M.resize(stretch_factor);
	N.resize(stretch_factor);

	// compute 'magic' vectors 

	Oa = (N.x*P.z - N.z*P.x)*viewing_distance;
	Ha = N.z*P.y - N.y*P.z;
	Va = N.y*P.x - N.x*P.y;

	Ob = (M.x*P.z - M.z*P.x)*viewing_distance;
	Hb = M.z*P.y - M.y*P.z;
	Vb = M.y*P.x - M.x*P.y;

	Oc = (M.z*N.x - M.x*N.z)*viewing_distance;
	Hc = M.y*N.z - M.z*N.y;
	Vc = M.x*N.y - M.y*N.x;

		
}

/*			// original unoptimized algorithm
	for (int j = top_row ; j <= bottom_row ; j++)
	{
		int left_col = left_columns[j];
		int right_col = right_columns[j];

		PIXEL *scr_row = ScreenRows[j];
		
		int jj = j - view_half_height;
		 jjVa = jj*Va;
		 jjVb = jj*Vb;
		 jjVc = jj*Vc;

		for(int i = left_col; i < right_col; i++)
		{
			int ii = i - view_halfwidth_float;
			
			  a = Oa + ii*Ha + jjVa;
			  b = Ob + ii*Hb + jjVb;
			  c = Oc + ii*Hc + jjVc;

			  c_inverse = 1/c;
			
			int u = float2int(texture_width  * a * c_inverse)&63;
			int v = float2int(texture_height * b * c_inverse)&63;
			scr_row[i] =  palette[texture[u + v*256]];
		}
*/

inline void update_uv(float a, float b, float c_inverse, int &u, int &v)
{
	u = float2int(a * c_inverse*65536.0f);
	v = float2int(b * c_inverse*65536.0f);
}

static int x_animation;
static int y_animation;

void blit_flat_double(dword u1,dword v1, dword du, dword dv,
														 int start_col, int length, int row, UCHAR *texture, PIXEL *palette)
{
		// statics: we kill the stack in _asm below, so these must be static to be used in _asm
	static	dword u_fraction;              
	static	byte  u_integer ;
	static	dword v_fraction;
	static	byte  v_integer;
	static  PIXEL *endloc;
	static int odd;
	odd = (length & 1);  
		
		u_fraction = (du & 0x0000FFFF)<<16; // create fixed point number of du, u_fraction:u_integer
		u_integer  = du >> 16;
		
		v_fraction = (dv & 0x0000FFFF)<<16; // create fixed point for dv
		v_integer  = dv >> 16;
		
		PIXEL *dest =  ScreenRows[row] + start_col;
		endloc = dest + (length & 0xFFFE);
			_asm {
				mov  ebx,u1
				mov  ecx,v1
				mov  esi,texture
				mov  edi,palette
				
				shr ebx,1        // divide by 2
 				shr ecx,1        // since we are pixel doubling below

				add  ebx,x_animation 
				add  ecx,y_animation

				mov  eax,dest
				push ebp
				mov  ebp,eax

				mov  dx,bx                      
				sal  edx,16
				sar  ebx,16
				or   ebx,edx

				mov  dx,cx                       
				sal  edx,16
				sar  ecx,16
				or   ecx,edx
				
				mov  eax,0                       // clear out top portion for indexing
				mov al,bl                        // store the ints in ax
				mov ah,cl                        //
				and eax,0x3F3F                   // and it out
				mov edx,0                        // clear out so we can index from it

				cmp ebp,endloc					 // if there only one pixel to be displayed
				je  do_odd						 // goto do_odd

		hloop:	
	  			mov  dl,[esi][eax]						// get pixel from texture
				add  ecx,v_fraction						// add a reversed fraction 	          
				adc  ah,v_integer						// add integer part 
				add  ebp,PIXEL_SIZE*2 					// increment screen index	
				mov  PIX_REG_C, [edi][edx*PIXEL_SIZE]	// get shaded pixel  
				add  ebx,u_fraction						// just like x coord above	
				adc  al,u_integer
				mov [ebp - PIXEL_SIZE*2], PIX_REG_C		// put pixel onto screen
				mov [ebp - PIXEL_SIZE],PIX_REG_C			// put again at next location	
				and  eax,0x3F3F 						// and with mask to wrap 	
				cmp  ebp,endloc							// has the end screen location been reached?  
				jb  hloop								// continue if it has not	

				cmp odd,0								// if there is an even number of pixels to display
				je skip_odd								// skip over displaying the last pixel
do_odd:
			 mov  dl,[esi][eax]							// get pixel from texture
			 mov  PIX_REG_C, [edi][edx*PIXEL_SIZE]		// get shaded pixel  
			 mov  [ebp],PIX_REG_C						// put pixel onto screen
skip_odd:
				pop ebp
		}
}

void blit_flat_single(dword u1,dword v1, dword du, dword dv,
														 int start_col, int length, int row, UCHAR *texture, PIXEL *palette)
{
		// statics: we kill the stack in _asm below, so these must be static to be used in _asm
		static	dword u_fraction;              
		static	byte  u_integer ;
		static	dword v_fraction;
		static	byte  v_integer;
		static  PIXEL *endloc;
		
		u_fraction = (du & 0x0000FFFF)<<16; // create fixed point number of du, u_fraction:u_integer
		u_integer  = du >> 16;
		
		v_fraction = (dv & 0x0000FFFF)<<16; // create fixed point for dv
		v_integer  = dv >> 16;
		
		PIXEL *dest =  ScreenRows[row] + start_col;
		endloc = dest + length;
			_asm {
				mov  ebx,u1
				mov  ecx,v1
				mov  esi,texture
				mov  edi,palette
				
				add  ebx,x_animation 
				add  ecx,y_animation

				mov  eax,dest
				push ebp
				mov  ebp,eax

				mov  dx,bx                      
				sal  edx,16
				sar  ebx,16
				or   ebx,edx

				mov  dx,cx                       
				sal  edx,16
				sar  ecx,16
				or   ecx,edx
				
				mov  eax,0                       // clear out top portion for indexing
				mov al,bl                        // store the ints in ax
				mov ah,cl                        //
				and eax,0x3F3F                   // and it out
				mov edx,0                        // clear out so we can index from it

		hloop:	
	  			mov  dl,[esi][eax]								// get pixel from texture
				add  ecx,v_fraction								// add a reversed fraction 	          
				adc  ah,v_integer								// add integer part 
				add  ebp,PIXEL_SIZE 							// increment screen index	
				mov  PIX_REG_C, [edi][edx*PIXEL_SIZE]			// get shaded pixel  
				add  ebx,u_fraction								// just like x coord above	
				adc  al,u_integer
				mov [ebp - PIXEL_SIZE],PIX_REG_C				// put again at next location	
				and  eax,0x3F3F									// and with mask to wrap 	
				cmp  ebp,endloc									// has the end screen location been reached?  
				jb  hloop										// continue if it has not	

				pop ebp
		}
}

void cSurface::render_segment(int row, int start_col, int end_col, int sector_id, surfaces surface)
{
  sector *sec=level->Sectors[sector_id];
	int flat_id= surface == FLOOR ? sec->OnBottom: sec->OnTop;
	BITMAPINFO_4DX *info = &(level->flat_info[flat_id]);

	PIXEL *palette = shader->GetPalette(info->palette,0,sec->lightlevel);
	//UCHAR *texture = use_pre_rotated_flats ? pr_addresses[pre_rotated[flat_id]]: info->address;
	UCHAR *texture =  info->address;

	if (texture == NULL || palette == NULL)
		return;
	
  int left_col =  start_col;
	int right_col = end_col ;
	
	if (surface == FLOOR)
		sec->GetFloorTextureOffsets (x_animation,  y_animation);
	else
		sec->GetCeilingTextureOffsets(x_animation, y_animation);

	 x_animation <<= 16;
	 y_animation <<= 16;

	int   span_len = 1 << span_len_shift;  
	
	int u1,v1,u2,v2;
	float a,b,c,c_inverse;
	
	int jj = row - view_half_height;
	float	 jjVa = jj*Va;
	float	 jjVb = jj*Vb;
	float	 jjVc = jj*Vc;

	int ii = left_col - view_halfwidth;
	a = Oa + ii*Ha + jjVa;
	b = Ob + ii*Hb + jjVb;
	c = Oc + ii*Hc + jjVc;
	c_inverse = 1/c;
	update_uv(a,b,c_inverse,u1,v1);
	
	ii+=span_len;
	a = Oa + ii*Ha + jjVa;
	b = Ob + ii*Hb + jjVb;
	c = Oc + ii*Hc + jjVc;
	c_inverse = 1/c;
	update_uv(a,b,c_inverse,u2,v2);

 void (*blit_flat)(dword u1,dword v1, dword du, dword dv,
														 int start_col, int length, int row, UCHAR *texture, PIXEL *palette);


	blit_flat = (no_texture_stretch) ? blit_flat_single : blit_flat_double;
	for(int i = left_col; i < right_col; )
	{	
		ii+=span_len;
		a = Oa + ii*Ha + jjVa;
		b = Ob + ii*Hb + jjVb;
		c = Oc + ii*Hc + jjVc;
		
		int remaining = right_col - i;
		int span = (remaining < span_len) ? remaining : span_len;
		if (!span) 
			break;
		int du = (u2 - u1) >> span_len_shift;  // change in u : (u2 - u1)/span_len
		int dv = (v2 - v1) >> span_len_shift;  // change in v : (v2 - v1)/span_len

		_asm fld1
   	_asm fdiv c

		blit_flat(u1, v1, du,  dv,  i,  span,  row, texture, palette);
		
		_asm fstp c_inverse

		u1 = u2;
		v1 = v2;
		update_uv(a,b,c_inverse,u2,v2);
		i+= span;
	}
}

void cSurface::calc_pick_coords()
{
	{	
		// first find texture co-ords (u,v) for pick_row,col
		// NB: Texture co-ords are really offsets from the baseline origin.
		int jj = pick_row - view_half_height;
		int ii = pick_col - view_halfwidth;
		float a = Oa + ii*Ha + jj*Va;
		float b = Ob + ii*Hb + jj*Vb;
		float c = Oc + ii*Hc + jj*Vc;
		float c_inverse = -1/c;
		
		float u = a * c_inverse;
		float v = b * c_inverse;
		
		// v is in plane of slope, so xform to flat plane, (u is already in flat plane)
		v *= (float)cos(atan(slope));

		// transform from sector co-ords to world co-ords
		float x = -baseline->SINE *v +  baseline->COS * u;
		float y = baseline->COS * v +  baseline->SINE * u;
		pick_x = x + baseline->x1();
		pick_y = y + baseline->y1();
		
		picking = false;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Background functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void load_new_background(void)
{
	if (prev_background != NO_BITMAP)
	{
		effects->FreeEffectBitmaps(prev_background);
		effects->FreeEffectPalette(prev_background);  
	}
	
	prev_background = background;
	
	if (effects->LoadEffectBitmaps(background) != NO_BITMAP)
	{	
		UCHAR *bg_address;
		
		effects->LoadEffectPalette(background);
		bg_palette = shader->GetPalette(background,0,0);
		
		bg_height  = effects->EffectBitmap(background)->w;
		bg_width   = effects->EffectBitmap(background)->h;
		bg_address = effects->EffectBitmap(background)->address;	
		
		// setup background panels: first the 4 non tiled panels
		for (int i = 0; i < 4; i++)
		{
			bg_panels[i*2].address = bg_address + (i+1) * (320/2);
			bg_panels[i*2].length  = 320;
		}
		
		// now the 8 tiled panels
		UCHAR *small_tiles_start = bg_address ;
		int tiles = 0;
		for(int i = 1; i < 8;  i+=2, tiles++)
		{
			bg_panels[i]  .address = small_tiles_start ;
			bg_panels[i].length =320;
		}
	}
}

void update_background(int player_angle)
{
	// find  background panel that first screen column is in
	int col = ((320*8) * player_angle) / Angle_360;
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (col < bg_panels[i].length)
			break;
		col -= bg_panels[i].length;
	}

	// now setup screen panels from this
	for (int j = 0; j < 4; j++)
	{
		bg_screen_panels[j] = bg_panels[i++];
		if (i >= 8)
			i=0;
	}

	// adjust first screen panel's length and start address to reflect start column.
	bg_screen_panels[0].address += col/2;
	bg_screen_panels[0].length  -= col;

	// calculate background row that corrosponds to first row of viewport
	bg_row   =  bg_height/3 - (view_half_height - view_height/2);
	
	// change palette for fade-ins
	//	if (player->Teleporting())
	bg_palette = shader->GetPalette(background,0,0); 

	// set colors for above and below the bg bitmap. 
	UCHAR *bg_address = effects->EffectBitmap(background)->address;	
	bg_above_color = bg_palette[*(bg_address+1)];
	bg_below_color = bg_palette[*(bg_address + bg_width*bg_height -1)];
}

static void blit_background(PIXEL *screen,int x, int y, int len)
{
	int row = bg_row + y;

	if (row < 0)              // the background above the bitmap
	{
		while(len--)
			*screen++ = bg_above_color;
	}
	else if (row >= bg_height) // the background below the bitmap
	{
		while(len--)
			*screen++ = bg_below_color;
	}
	else                        // the bitmap itself
	{
		// determine which panel the blit will start in
		int i = -1;
		while (x >= bg_screen_panels[++i].length)
			x -= bg_screen_panels[i].length;
		
		// determine blit length and source pointer from panel
		int blit_len = MIN(len,bg_screen_panels[i].length - x);
		UCHAR *src = bg_screen_panels[i++].address + row * bg_width + x/2;
		
		if (x & 1) // if the start position is odd
		{
			*screen++ = bg_palette[*src++]; // blit a pixel and increment the src ptr
			blit_len--; len--;
		}
		
		while (len)
		{
			len -= blit_len;
			while(blit_len--) // perform the blit for a panel
			{
				*screen++ = bg_palette[*src];
				if (!blit_len--)
					break; // break if the blit length is odd
				*screen++ = bg_palette[*src++];
			}
			
			blit_len = MIN(len,bg_screen_panels[i].length); // determine next blit length
			src = bg_screen_panels[i++].address + row * bg_width; // and source panel;
		} 
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Picking Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// transform a view co-ord point to world co-ords
static void transform2world( float rx,float ry, float &x, float &y)
{
	// rotate
  float nx = rx*view_cos + ry *view_sin;
  float ny = rx*view_sin - ry *view_cos;
	
	// translate
	x = nx + player_x;
	y = ny + player_y;
}


void SetPickScreenCo_ords(int col, int row)
{
	pick_row = row;
	pick_col = col;
	picking  = TRUE; 
	picked_line = NULL;
}

int GetPickWorldCo_ords(float &x, float &y)
{
	x = pick_x;
	y = pick_y;
	return TRUE;
}

linedef *GetPickLinedef(void)
{
	return picked_line;
}

cActor *ActorAtScreenPoint(int col,int row)
{
	for	(int i = 0; i < num_rendered_actors; i++)
	{
		cActor *a = rendered_actors[i];
		if (actors->ValidActor(a) && point_in_actor(a,row,col))
			return a;
	}
	return NO_ACTOR;
}

static void calc_pick_coords(float  sector_height)
{	
	
	float row_diff = (float)fabs((long double)view_half_height- pick_row);
	float height   =  player_height - sector_height; 
	
	// calculate view co-ords
	float pick_rx  = (float)fabs((height * viewing_distance)/row_diff)+1;
	float pick_ry  = (pick_rx*(view_halfwidth_float-(float)pick_col))*viewing_dist_inverse;
	
	transform2world(pick_rx,pick_ry,pick_x,pick_y);
	picking = false;
}


static void calc_floor_pickpoint(sector *sec)
{
	if (sec->floor_surface)
		sec->floor_surface->calc_pick_coords();
	else
		calc_pick_coords(sec->floor_height);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
