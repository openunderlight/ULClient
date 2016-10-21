// Copyright Lyra LLC, 1997. All rights reserved.
// Graphics Engine : Actor Related Rendering
// Interface
// 	RenderActor(),  (used by all)
// 	render_actor()  (used internally by graphics engine)

extern int opt_on;

#include "4dx.h"
#include "cActor.h"
#include "Options.h"
#include "cPalettes.h"
#include "cNameTag.h"
#include "cNeighbor.h"
#include "cEffects.h"
#include "cPlayer.h"
#include "cItem.h"
#include "resource.h"
#include "cDDraw.h"
#include "cLevel.h"	// for linedef
#include "cControlPanel.h"

#include "RenderActor.h"
#include "RenderView.h"


/////////////////////////////////////////////////////////////////////
// externs
extern options_t options;
extern cEffects *effects;
extern cPaletteManager *shader;
extern cControlPanel *cp;
extern cPlayer *player;
extern cDDraw *cDD;

// externs from RenderView
extern WallCol 		*wall_cols;
extern PIXEL			*ScreenRows[2048];
extern float			*ScaleRatio;
extern unsigned int	*DeltaV;

extern int view_pitch;
extern int view_width;
extern int view_height;
extern float viewing_distance;

extern float player_height;
extern int	 player_angle;

/////////////////////////////////////////////////////////////////////
// Globals

class cColumnClipper column_clipper;

/////////////////////////////////////////////////////////////////////
// Local Data Types,Constants

struct PostDraw
{
	unsigned char *ColumnPtr;
	int v;
	int destheight;
	int scale;
	PIXEL *screencol;
	PIXEL *palette;
	short *color_table;
	short repeat;
};

struct avatar_patch
{
	int row,col;
	float resolution;
	int bitmap;
	int palette_id;
	short *color_table;
};

const int NUM_PATCHES = 8;
//const int NUM_PATCHES = 1;

const int SPANS_PER_ROW = 8;
struct span
{
	short start;
	short end;
} ;

typedef span span_row[SPANS_PER_ROW];


class cRowClipper
{
private:
	span_row  span_rows[MAX_VIEW_HEIGHT];
	unsigned char num_spans[MAX_VIEW_HEIGHT];
	void add_span(int row, int i, int span_start, int span_end);
  void delete_span(int row, int i);
	eClipTestResult	test_span(span &s,int start,int end);

public:
	void init(int view_height);
	void frame_reset(void);
	void insert_span(int row, int span_start, int span_end);
	bool test_span_and_clip(int row, short &span_start, short &span_end);

};


//static cRowClipper row_clipper;

/////////////////////////////////////////////////////////////////////
//Static Globals
const int  MAX_POST_COUNT = 4096;
static PostDraw PostDrawList[MAX_POST_COUNT];
static int PostCount;



/////////////////////////////////////////////////////////////////////
// Function prototypes

static void blit_RLE_column(PostDraw *pdraw);
static void draw_postdraw_list(void);
static bool get_avatar_patch(cActor *actor, int patch_order, avatar_patch &patch);


// renders to main viewport: blits bitmap at top_row,left_col, taking existing geometry into account
static bool render_RLE_bitmap(BITMAPINFO_4DX *bitmap, int top_row, int left_col,
																			float zDistance, PIXEL *palette,short *color_table=NULL);

// blit RLE bitmap to destination of given width , height. Scales bitmap to fit given width,height
static void blit_RLE_bitmap(BITMAPINFO_4DX *bm_info, PIXEL *dest,  int width,  int height,
														 PIXEL *palette, short *color_table );

// copies RLE bitmap to main viewport at (col,row), does not scale bitmap
static void copy_RLE_bitmap(BITMAPINFO_4DX *bm_info,	int col, int row, PIXEL *palette, short *color_table);

static void add_to_postdraw_list(unsigned char *ColumnPtr,int v,int destheight,int deltav,
															PIXEL *screencol, PIXEL *palette, short *color_table);


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void InitRenderActor(void)
{

}

void DeInitRenderActor(void)
{

}

//byte *rle2[6*24];
//byte *rle2_addr;

void StartRenderActor(UCHAR *viewBuffer, int pitch, cPlayer *player)
{
	PostCount = 0;
	//row_clipper.frame_reset();
	column_clipper.frame_reset();

	if (player->EvokingFX().Active())
	{
	 cArtFX &ev  = player->EvokingFX();
		BITMAPINFO_4DX *bitmap = effects->EffectBitmap(ev.CurrentBitmap());
	PIXEL *palette = shader->GetUnshadedPalette(LyraPalette::FX_PALETTE);
	int top_row[MAX_RESOLUTIONS] = {120, 150, 192};
		int left_col[MAX_RESOLUTIONS] = {160, 200, 256};
		float zDistance = 130.0f;
		render_RLE_bitmap(bitmap ,top_row[cDD->Res()],left_col[cDD->Res()],zDistance, palette , ev.ColorTable());
	}

/*

	static int done  = true;
	if (!done)
	{
		done = true;
		for (int i = 400; i < (400 + 6*24) ; i++)
		{
			BITMAPINFO_4DX *bm = effects->EffectBitmap(i);
			short *dstarts 	 = (short *)bm->address;

			byte *src = rle2[i-400] = new byte [bm->h*bm->w];
			byte *s = src;
			memcpy(src,bm->address, bm->h * sizeof (short) + 20);

			short *sstarts 	 = (short *)src;

			s += bm->h*sizeof(short) + 20;

			for (int w = 0; w < bm->h; w++)
			{
				if (!dstarts[w])
					continue;

				sstarts[w] = s - src;

				unsigned char *col = bm->address + dstarts[w];
				*s++ = *col++;
				*s++ = *col++;

				int len= 0;
				for (byte *d = col; len <= bm->w; d+=2)
				{
					len += *d;
				}

				byte *end = d;
				d = col;

				byte *ti = s;
				s++;

				byte run_len		 = *d;
				byte t_offset		 = *(d+1) & 0x1F;
				byte table_index = *(d+1) & 0xE0;

				while (d != end)
				{
					if (run_len < 8)
					{
						*s   = (run_len << 5) | t_offset;
						run_len =	  *d++;
						t_offset=	  *d++ & 0x1F;
					}
					else
					{
						*s   = (0x7 << 5) | t_offset;
						run_len -= 0x7;
					}
					s++;

					if ((*(d+1) & 0xE0) != table_index || s-ti == 31)
					{
						int tlen = s - ti;
						*ti = table_index | tlen;
						table_index = *(d+1) & 0xE0;
						ti = s;
						s++;
					}
				}
				int tlen = s - ti;
				*ti = table_index | tlen;
			}
		}
	}*/
}

void EndRenderActor(void)
{
	draw_postdraw_list();

	if (player->UsingBlade())
	{
		BITMAPINFO_4DX *bitmap = effects->EffectBitmap(LyraBitmap::BLADE+cDD->Res());
		PIXEL *palette = shader->GetUnshadedPalette(player->Palette());
		POINT p = player->BladePos();
		copy_RLE_bitmap(bitmap ,p.x ,p.y , palette, player->ColorRegions());
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//Renders Actor to given buffer of (width, height)
void RenderActor(cActor *actor, PIXEL *buffer, int width, int height)
{

	// clear out destination buffer
	memset(buffer, 0, width * height * BYTES_PER_PIXEL);

	int old_pitch = view_pitch;
	view_pitch = (width * BYTES_PER_PIXEL);
	BITMAPINFO_4DX *bm_info = effects->EffectBitmap(actor->IconBitmap());

	if (!actor->IsPlayer())
	{
		// Blit actor bitmap
		blit_RLE_bitmap(bm_info,  buffer, width, height, shader->GetUnshadedPalette(actor->Palette()),
										actor->ColorRegions());
	}
	else
	{

		float scale_w = (float)width/bm_info->h;
		float scale_h = (float)height/bm_info->w;
		for (int i =0; i < NUM_PATCHES;	i++)	// get patches from bottom patch to top patch
		{
			avatar_patch patch;

			if (get_avatar_patch(actor,i,patch) )
			{
				PIXEL *buffer_offset = buffer +
						 float2int(patch.row*scale_w) * width +  float2int(patch.col*scale_h );

				BITMAPINFO_4DX *bitmap = effects->EffectBitmap(patch.bitmap);

				int w = float2int(bitmap->h*scale_w/patch.resolution);
				w = min(w,width);

				int h = float2int(bitmap->w*scale_h/patch.resolution);
				h = min(h,height);

				PIXEL *palette = shader->GetUnshadedPalette(patch.palette_id);

				blit_RLE_bitmap(bitmap, buffer_offset, w, h, palette, patch.color_table);
			}
		}
	}
	view_pitch = old_pitch;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
static void blit_RLE_bitmap(BITMAPINFO_4DX *bm_info, PIXEL *dest, int width,int height, PIXEL *palette,
														short *color_table )
{
	int	 bm_width	= bm_info->h;
	int	 bm_height	= bm_info->w;
	UCHAR *bm_address = bm_info->address;
	short *starts = (short*)bm_address;

	float width_delta = (float)bm_width/width;

	// set up vars used in blitting function
	PostDraw p;
	p.palette		= palette;
	p.scale			= ((height+1)<<16)/bm_height; 	// vertical scale
	p.v				= 0;												// start in source bitmap
	p.destheight	= height;								// destination height
	p.color_table	= color_table;
	p.repeat 		= 0;

	for (int x_index = 0; x_index < width ; x_index++)
	{
		int column =  (int)(x_index*width_delta);
		int start = starts[column];
		if (!start)  // if column is empty
			continue;
		p.ColumnPtr = bm_address + start + 2;	// +2 skips clipping info in column
		p.screencol = dest		 + x_index;
		blit_RLE_column(&p);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////
static void copy_RLE_bitmap(BITMAPINFO_4DX *bm_info,	int start_column, int start_row,
														PIXEL *palette, short *color_table)
{
	int	 bm_width	= bm_info->h;
	int	 bm_height	= bm_info->w;
	UCHAR *bm_address = bm_info->address;
	short *starts = (short*)bm_address;

	// row clipping
	int v_offset = max(0,-start_row);
	bm_height = min(view_height - start_row-1, bm_height);

	// set up vars used in blitting function
	PostDraw p;
	p.palette		= palette;
	p.scale			= 1 << 16;									// vertical scale
	p.v				= v_offset << 16; 						// start in source bitmap
	p.destheight	= bm_height - v_offset; 			// destination height
	p.color_table	= color_table;
	p.repeat 		= 0;

	int screen_start_col = MAX(0,start_column);
	int screen_start_row = MAX(0,start_row);
	int screen_end_col	= MIN(view_width,start_column + bm_width);

	int bitmap_column = max(0, - start_column);
	for(int i = screen_start_col; i < screen_end_col; i++, bitmap_column++)
	{
		int start = starts[bitmap_column];
		if (!start)  // if column is empty
			continue;
		p.ColumnPtr = bm_address + start + 2;	// +2 skips clipping info in column
		p.screencol = ScreenRows[screen_start_row] + i;
		blit_RLE_column(&p);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
struct patch_point
{
	BYTE col;
	BYTE row;
};


const int PP_HEAD  = 0;
const int PP_FRONT = 5;
const int PP_BACK  = 8;
const int PP_EVOKING = 6;

// Get avatar patch with given patch order : where 0 = bottom patch..NUM_PATCHES = top patch
static bool get_avatar_patch(cActor *actor, int patch_order, avatar_patch &patch)
{
	LmAvatar avatar;
	int bm_id;
	int current_view;
	bool patch_visible = false;
	unsigned int  show_mask;
	int which_side;
	bool evoking = false;

	if (patch_order != 0 && (actor->Forming() || actor->Dissolving() || actor->Entering() || actor->flags & ACTOR_SOULSPHERE))
		return false; //	no patching for avatars in these states

	if (actor->IsPlayer())
	{
		avatar = ((cPlayer *)actor)->Avatar();
		bm_id  = actor->IconBitmap();
		current_view = cp->CurrAvatarView();
	}
	else if (actor->IsNeighbor())
	{
		avatar = ((cNeighbor *)actor)->Avatar();
		bm_id  = actor->CurrBitmap(player_angle);
	  current_view   = actor->CurrentView(player_angle);
		evoking = ((cNeighbor *)actor)->Evoking();
	}
	else
		return false;	// don't render if actor is not a neigbor or a player


	// extract patch points from main avatar bitmap
	BITMAPINFO_4DX  *main_bitmap = effects->EffectBitmap(bm_id);
	patch_point *points	= (patch_point *)(main_bitmap->address + main_bitmap->h * sizeof(short));

	// determine if we are looking at front views or back views,
	if (current_view >=2 && current_view <= 4)  // front views
	{
		which_side = PP_FRONT;
		show_mask  = SHOW_FRONT;
	}
	else
	{
		which_side = PP_BACK;
		show_mask  = SHOW_BACK;
	}


	switch (patch_order)
	{
		case 0:	// main avatar bitmap
		{
			patch.row = patch.col = 0;
			patch.resolution	= 1.0f;
			patch.bitmap		= bm_id;
			patch.palette_id	= actor->Palette();
			patch.color_table = actor->ColorRegions();
			patch_visible = true;
		}
		break;

		case 1:	// head
		{
			break; // TEMP!!!!
			patch_point *head_pos = points+PP_HEAD;
			int head_bitmap = (avatar.AvatarType() == Avatars::MALE || 1) ? 1220 : LyraBitmap::FEMALE_HEAD1;
			if (head_bitmap && head_pos->row && head_pos->col)
			{
				head_bitmap += current_view;
				int bitmap_center = effects->EffectBitmap(head_bitmap)->h/2;
				patch.resolution	= 2.0f;
				static int xc = 2 ;
				patch.row			= head_pos->row -xc;
				patch.col			= head_pos->col - float2int(bitmap_center/patch.resolution);
				patch.bitmap		= head_bitmap;
				patch.palette_id	= actor->Palette();			// use avatar palette
				patch.color_table = actor->ColorRegions();	// use avatar color table
				patch_visible = true;
			}
		}
		break;

		case 2: //guild symbol
		{
			if (avatar.GuildID() == Guild::NO_GUILD ||
			 ((avatar.ShowGuild() & show_mask) != show_mask))
				break;

		 patch_point *guild_pos = &points[which_side];
			if (guild_pos->row && guild_pos->col)
			{
				// setup guild color table, based on rank
				short *guild_colors;
				static short guild_initiate[2] = { 7,0 };
				static short guild_knight	[2] = { 7,1 };
				static short guild_ruler	[2] = { 15,7};
				static short guild_seneschal[2] = { 15,1 };

				switch (avatar.GuildRank())
				{
					case Guild::INITIATE: guild_colors = guild_initiate; break;
					case Guild::KNIGHT  : guild_colors = guild_knight;   break;
					case Guild::RULER   : 
						if (avatar.AccountType()==LmAvatar::ACCT_ADMIN)
							guild_colors = guild_seneschal;
						else
							guild_colors = guild_ruler;	  
						break;
					default : guild_colors = guild_initiate;
				}

				int guild_bitmap	= LyraBitmap::AVATAR_GUILD_START + avatar.GuildID()*3;
		//		if (avatar.NPSymbol())
		//			guild_bitmap = LyraBitmap::AVATAR_NP_START;
				guild_bitmap		+= (6-current_view) % 3; 	  // guild symbols have only 3 views.
				int center_col 	= effects->EffectBitmap(guild_bitmap)->h/2;
				int center_row 	= effects->EffectBitmap(guild_bitmap)->w/2;

				patch.resolution	= 2.0f;
				patch.row			= guild_pos->row - float2int(center_row/patch.resolution);
				patch.col			= guild_pos->col - float2int(center_col/patch.resolution);
				patch.bitmap		= guild_bitmap;
				patch.palette_id	= actor->Palette();		 // use avatar palette
				patch.color_table = guild_colors;
				patch_visible = true;
			}
		}
		break;
		case 3: // sphere symbol
		{
			if ((avatar.ShowSphere() & show_mask) != show_mask)
				break;

			patch_point *sphere_pos = &points[which_side];

			if (sphere_pos->row && sphere_pos->col)
			{
				int sphere_bitmap;	//MDA 4/12/03 defined here, for NP symbol... 
			/*
				int sphere_bitmap	= LyraBitmap::AVATAR_SPHERE_START + avatar.Sphere()*3;
				sphere_bitmap		+= (6-current_view) % 3;		// sphere symbols have only 3 views.
				int center_col 	= effects->EffectBitmap(sphere_bitmap)->h/2;
				int center_row 	= effects->EffectBitmap(sphere_bitmap)->w/2;

				patch.resolution	= 2.0f;
				patch.row			= sphere_pos->row - float2int(center_row/patch.resolution);
				patch.col			= sphere_pos->col - float2int(center_col/patch.resolution);
				patch.bitmap		= sphere_bitmap;
				patch.palette_id	= 0; // default palette
				patch.color_table = NULL;
				patch_visible = true;
				*/

				// Use gold spheres for DreamSmiths
				// Use blue spheres for WordSmiths
				// Use green spheres for both smiths
				// Use red borders for Dreamstrike
				static short* smith_colors;
				static short smith_unmarked [4] = { 0,7,15,2 };
				static short dreamsmith_marked	[4] = { 3,7,15,2 };
				static short wordsmith_marked	[4] = { 6,7,15,2 };
				static short bothsmith_marked	[4] = { 4,7,15,2  };
				static short ds_smith_unmarked [4] = { 0,1,15,2 };
				static short ds_dreamsmith_marked	[4] = { 3,1,15,2 };
				static short ds_wordsmith_marked	[4] = { 6,1,15,2 };
				static short ds_bothsmith_marked	[4] = { 4,1,15,2  };
				
				if (!avatar.NPSymbol()) //MDA 4/12/03
				{
					if (avatar.Dreamstrike())
					{
						smith_colors = ds_smith_unmarked;
						if (avatar.DreamSmith() && !avatar.WordSmith()) 
							smith_colors = ds_dreamsmith_marked;
						else if (!avatar.DreamSmith() && avatar.WordSmith()) 
							smith_colors = ds_wordsmith_marked;
						else if (avatar.DreamSmith() && avatar.WordSmith()) 
							smith_colors = ds_bothsmith_marked;
					}
					else
					{
						smith_colors = smith_unmarked;
						if (avatar.DreamSmith() && !avatar.WordSmith()) 
							smith_colors = dreamsmith_marked;
						else if (!avatar.DreamSmith() && avatar.WordSmith()) 
							smith_colors = wordsmith_marked;
						else if (avatar.DreamSmith() && avatar.WordSmith()) 
							smith_colors = bothsmith_marked;
					}
					sphere_bitmap	= LyraBitmap::AVATAR_SPHERE_START + avatar.Sphere()*3;
				}
				else
					sphere_bitmap = LyraBitmap::AVATAR_NP_START;
				sphere_bitmap		+= (6-current_view) % 3;		// sphere symbols have only 3 views.
				int center_col 	= effects->EffectBitmap(sphere_bitmap)->h/2;
				int center_row 	= effects->EffectBitmap(sphere_bitmap)->w/2;

				patch.resolution	= 2.0f;
				patch.row			= sphere_pos->row - float2int(center_row/patch.resolution);
				patch.col			= sphere_pos->col - float2int(center_col/patch.resolution);
				patch.bitmap		= sphere_bitmap;
				patch.palette_id	= actor->Palette(); // actor palette
				patch.color_table   = smith_colors;
				patch_visible       = true;
				// end Gold Sphere code
			}

		}
		break;

		case 4: // Evoking effect
		{
			if (!evoking)
				break;
int pp;
//int center_col =0;
		if (avatar.BitmapID() == LyraBitmap::FEMALE_AVATAR)
			{
				pp = PP_EVOKING + 1;
// 			if (current_view == 3)
// 				center_col -= 16;
			}
			else
				pp = PP_EVOKING;

			cArtFX &ev = actor->EvokingFX();
			patch_point *evoking_pos = points + pp;
			if (ev.Active() && evoking_pos->row && evoking_pos->col )
			{
				int ev_bitmap		= ev.CurrentBitmap();
				int center_col 	= effects->EffectBitmap(ev_bitmap)->h/2;

				patch.resolution	= 2.0f;
				patch.row			= evoking_pos->row - float2int(effects->EffectBitmap(ev_bitmap)->w/patch.resolution);
				patch.col			= evoking_pos->col - float2int(center_col/patch.resolution);
				patch.bitmap		= ev_bitmap;
				patch.palette_id	= LyraPalette::FX_PALETTE;
				patch.color_table = ev.ColorTable();
				patch_visible = true;
			}
		}
		break;

		case 5: // Evoked effect
		{
			cArtFX &ev = actor->EvokedFX();
			if (ev.Active())
			{
				patch.row			= 0; //main_bitmap->w - effects->EffectBitmap(ev.CurrentBitmap())->w ;
				patch.col			= (main_bitmap->h - effects->EffectBitmap(ev.CurrentBitmap())->h)/2;
				patch.resolution	= 1.0f;
				patch.bitmap		= ev.CurrentBitmap();
				patch.palette_id	= LyraPalette::FX_PALETTE;
				patch.color_table	= ev.ColorTable();
				patch_visible = true;
			}

		}
		break;
		case 6: // halo
			
			if ( avatar.Teacher() || avatar.Apprentice())
			{
				const int NUM_HALO_COLORS = 8;
				static short halo_colors[NUM_HALO_COLORS][2] =
				{
					{ 1,1 },  //  0 - WILLPOWER, Yellow
					{ 3,3 },  //  1 - INSIGHT, Blue
					{ 2,2 },  //  2 - RESILIENCE, Green
					{ 0,0 },  //  3 - LUCIDITY, Red
					{ 4,0 },  //  4 - GMMT, Skittles
					{ 5,5 },  //  5 - APPRENTICE, Chalk
					{ 6,7 },  //  6 - DREAMSTRIKE, Blood
					{ 7,7 }   //  7 - EVIL, Abyss
				};

				patch_point *halo_pos = points+PP_HEAD;
				if ( halo_pos->row && halo_pos->col)
				{
					int halo_bitmap = actor->IsNeighbor() ?
											((cNeighbor *)actor)->HaloBitmap() : LyraBitmap::AVATAR_HALO;

					int bitmap_center =	effects->EffectBitmap(halo_bitmap)->h/2;

					patch.resolution	= 2.0f;
					patch.row			= halo_pos->row - 6;
					patch.col			= halo_pos->col - float2int(bitmap_center/patch.resolution);
					patch.bitmap		= halo_bitmap;
					patch.palette_id = LyraPalette::HALO_PALETTE;
					int halo_color;

					// GM DreamerStrike Master
					if (avatar.Dreamstrike()
						&& avatar.NPSymbol()
						//&& avatar.AccountType() == avatar.ACCT_ADMIN
						) {
							halo_color = 7;
					}
					// NPSymbol for GMMT
					else if (avatar.NPSymbol())
						halo_color = 4;
					// Teacher is third priority
					else if (avatar.Teacher())
						halo_color = min(avatar.Focus() - 1, NUM_HALO_COLORS - 2); // teacher's focus stat
					// Default to apprentice, if exists
					else //(avatar.Apprentice())
						halo_color = 5;

					patch.color_table	= &halo_colors[halo_color][0];
					patch_visible		= true;
				}
			}
		break;
//#if 0
			case 7: // double halo
			if ( avatar.MasterTeacher() ) // master teacher
			{
				const int NUM_HALO_COLORS = 8;
				static short halo_colors[NUM_HALO_COLORS][2] =
				{
					{ 1,1 },  //  0 - WILLPOWER, Yellow
					{ 3,3 },  //  1 - INSIGHT, Blue
					{ 2,2 },  //  2 - RESILIENCE, Green
					{ 0,0 },  //  3 - LUCIDITY, Red
					{ 4,0 },  //  4 - GMMT, Skittles
					{ 5,5 },  //  5 - APPRENTICE, Chalk
					{ 6,6 },  //  6 - DREAMSTRIKE, Blood
					{ 7,7 }   //  7 - EVIL, Abyss
				};

				patch_point *halo_pos = points+PP_HEAD;
				if ( halo_pos->row && halo_pos->col)
				{
					int halo_bitmap = actor->IsNeighbor() ?
											((cNeighbor *)actor)->HaloBitmap() : LyraBitmap::AVATAR_HALO;

					int bitmap_center =	effects->EffectBitmap(halo_bitmap)->h/2;
					int halo_color;

					// GM DreamerStrike Master
					if (avatar.Dreamstrike()
						&& avatar.NPSymbol()
						//&& avatar.AccountType() == avatar.ACCT_ADMIN
						) 
					{
						patch.resolution = 3.0f;
						halo_color = 6;
					}
					// NPSymbol takes priority					
					else if (avatar.NPSymbol()) {
						patch.resolution = 4.0f;
						halo_color = 4;
					}
					else {
						patch.resolution = 4.0f;
						halo_color = min(avatar.Focus() - 1, NUM_HALO_COLORS - 1); // teacher's focus stat
					}

					patch.row = halo_pos->row - 6;
					patch.col = halo_pos->col - float2int(bitmap_center / patch.resolution);
					patch.bitmap = halo_bitmap;
					patch.palette_id = LyraPalette::HALO_PALETTE;
					patch.color_table	= &halo_colors[halo_color][0];
					patch_visible		= true;
				}
			}
//#endif
	break;
	}

	return patch_visible;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


static bool render_RLE_bitmap(BITMAPINFO_4DX *bitmap, int top_row, int left_col,
																			float zDistance, PIXEL *palette,short *color_table)
{
	bool visible	= false;
	int  bm_width	= bitmap->h;
	int  bm_height = bitmap->w;
	UCHAR *bm_address = /*opt_on? rle2_addr :*/ bitmap->address;
	short *starts		= (short *)bm_address;	 // array of column start offsets in RLE bitmap

	if (palette == NULL)
	{
		GAME_ERROR(IDS_NULL_PALETTE);
		return visible;
	}

	if (bm_address == NULL)
	{
		GAME_ERROR(IDS_NULL_TEXTURE);
		return visible;
	}

	// determine rendering scale
	int	scale 	 = float2int(ScaleRatio[float2int(zDistance)]*65536.0f);
	int	scale_inv = DeltaV[float2int(zDistance)];

	// determine screen bottom,right
	int bottom_row = top_row  + ((scale * bm_height)>>16);
	int right_col	= left_col + ((scale * bm_width)>>16);

	// clip left and right
	int screen_left_col = MAX(0,left_col);
	right_col			  = MIN(view_width,right_col);

	int bitmap_column = (screen_left_col - left_col)*scale_inv;
	for(int i = screen_left_col; i < right_col; i++, bitmap_column += scale_inv)
	{
		int usedf = wall_cols[i].usedf;
		int usedc = wall_cols[i].usedc;

		short start = starts[bitmap_column>>16];

		if ( usedf==usedc || !start)	// if the screen col. is full or texture column is all xparent
			continue;

		int itop 	= top_row;
		int ibottom = bottom_row;

		if(itop < usedf && ibottom > usedc)
		{
			int v = 0;

			// check top of column for clipping
			if ( itop < usedc )
			{
				v=(usedc-itop);	// v : offset in bitmap
				itop = usedc;				  // clip the top
			}

			//check bottom of column for clipping
			if ( ibottom > usedf )
				ibottom	= usedf; 	 // clip the bottom

			// clip against other actors
			int iitop = itop;
			column_clipper.test_span_and_clip(i,iitop, ibottom);

			if (iitop > itop)
				v += (iitop-itop);
			itop = iitop;

			int column_length = ibottom - itop;
/* 		if ( == CONTAINS)
			{
				//if our span contains the clipper span then we need to split the column into 2 sub columns
				int span_start 	= column_clipper.span_start(i);
				int span_length	= column_clipper.span_length(i);
				int first_length	= span_start-itop;
				int second_length = ibottom - span_start + span_length;

				if (span_length > 16)
				{

				}
			}
*/
			if ( column_length > 0 )
			{
				visible = true;
			// v *=scale_inv;

				UCHAR *column_ptr =	bm_address + start;

				UCHAR clip_span_start =  *column_ptr++ +1; // +1 ensures we don't clip too high
				UCHAR clip_span_end	 =  *column_ptr++ ;

				PIXEL *screencol	= ScreenRows[itop]+i;
				add_to_postdraw_list(column_ptr,v,column_length,scale,screencol,palette,color_table);

				int span_start = (clip_span_start*scale)>>16;// must scale the same way the blitter does
				int span_end	= (clip_span_end	*scale)>>16;//
				span_start = MAX(span_start + top_row ,0);
				span_end   = MIN(span_end + top_row ,view_height);
				column_clipper.add_span(i,span_start, span_end );
			}
		}
	}

/* if (visible)  // add horizontal clipping spans to row clipper
	{
		int bitmap_row=0;
		if (top_row < 0)
		{
			bitmap_row = -top_row*scale_inv;
			top_row = 0;
		}
		bottom_row = min(bottom_row,view_height);

		UCHAR *row_spans = bm_address + bm_width * sizeof(short);
		for (int i = top_row; i < bottom_row; i++, bitmap_row += scale_inv)
		{
			UCHAR *row_span = &row_spans[(bitmap_row>>16)*2];
			if (row_span[0] == row_span[1])
				continue;

			int span_start = (row_span[0]*scale)>>16;
			int span_end	= (row_span[1]*scale)>>16;

			span_start = MAX(span_start + left_col ,0);
			span_end   = MIN(span_end + left_col ,view_width);

			row_clipper.insert_span(i,span_start,span_end);
		}
	}*/
	return visible;
}


static void render_name_tag(cNameTag *tag, float rx, float ry, int actor_top_row, int actor_length,
														PIXEL *palette)
{
  BITMAPINFO_4DX *tag_bitmap = tag->Bitmap(0);
	float diff = viewing_distance - rx;
	float ratio = 1.0f + diff * 0.0015f;

	ratio = MAX(ratio,1.0f);
	float tag_rx = rx*ratio;

	if (tag_rx > viewing_distance)
	//if (ratio < 1.0f)
	{
		float bitmap_ratio = (float)tag->Bitmap(1)->h/tag_bitmap->h;
		tag_bitmap = tag->Bitmap(1);	// use lower resolution bitmap;
		tag_rx *= bitmap_ratio;
		if (tag_rx > viewing_distance)
				tag_rx = viewing_distance;
	}

	int half_width	= tag_bitmap->h/2;
	float ry1		= ry + half_width;
	float ry2		= ry - half_width;
	int length		= calc_screen_column(tag_rx,ry2) - calc_screen_column(tag_rx,ry1);
	int center_col	= calc_screen_column(rx,ry);
	int left_col	= center_col - length/2;
	int right_col	= center_col + length/2;

	if (length > actor_length*2)
		return;

	float bitmap_height = tag_bitmap->w * ScaleRatio[float2int(tag_rx)];
	int top_row = actor_top_row - int(bitmap_height);

	if (left_col <= view_width && right_col >= 0)
		render_RLE_bitmap(tag_bitmap, top_row, left_col,tag_rx,palette);
}


static bool render_neighbor(cNeighbor *neighbor)
{

	linedef *line						= neighbor->l;
	float neighbor_height			= neighbor->z - player_height;
	struct sector		 *sec 		= level->Sectors[line->sector];
	BITMAPINFO_4DX *main_bitmap	= effects->EffectBitmap(line->bitmap);
	PIXEL *nametag_palette = NULL;

	for (int i = NUM_PATCHES-1; i >= 0 ; i--)  // Get patches, from top patch	to bottom patch
	{
		avatar_patch patch;

		if (get_avatar_patch(neighbor,i,patch))
		{
			float zDist = line->rx1*patch.resolution;
			if (zDist >= MAX_DIST)
				continue;

			int patch_top_row = calc_screen_row(line->rx1,neighbor_height - patch.row);
			int patch_left_col = calc_screen_column(line->rx1,line->ry1 - patch.col);
			BITMAPINFO_4DX *bitmap = effects->EffectBitmap(patch.bitmap);
			PIXEL *palette = shader->GetPalette(patch.palette_id, float2int(line->rx1), sec->lightlevel);

// 		rle2_addr = rle2[patch.bitmap-400];

			bool visible = render_RLE_bitmap(bitmap,patch_top_row,patch_left_col,
																			zDist,palette,patch.color_table);

			if (patch.bitmap == line->bitmap) // if patch is main bitmap
			{
				neighbor->SetVisible(visible);
				nametag_palette = palette;
			}
		}
	}


	//  check if a name tag is required
// Jared 6-05-00
// PMares cannot see nametags
#ifndef PMARE
	if (!neighbor->IsMonster() && options.nametags )
	{
		cNameTag *name_tag = neighbor->NameTag();

		if (name_tag)
		{
			// calculate left and right screen columns for actor bitmap
			int left_col	= calc_screen_column(line->rx1,line->ry1);
			int right_col	= calc_screen_column(line->rx2,line->ry2);
			int top_row		= calc_screen_row(line->rx1,neighbor_height);

			float ry = line->ry1 - main_bitmap->h/2;	// ry of center point

			render_name_tag(name_tag, line->rx1,ry, top_row, right_col-left_col,nametag_palette );
		}
	}
#endif

	return neighbor->Visible();
}

bool render_actor(cActor *actor)
{

	linedef *line = actor->l;
	int zDistance	= float2int(line->rx1);

	if ( zDistance >= MAX_DIST ||  zDistance < 0  )
		return false;

	if (actor->IsNeighbor()) //&& !((cNeighbor *)actor)->IsMonster())
		return render_neighbor((cNeighbor *)actor);

	struct sector	*actor_sec = level->Sectors[line->sector];

	if (actor->IsItem() && ((cItem *)actor)->Draggable() && (actor_sec->flags & SECTOR_ITEMS_INVISIBLE))
		return true; // don't render but make it known that it is there by retuning true

	BITMAPINFO_4DX  *actor_bitmap = effects->EffectBitmap(line->bitmap);

	// get actor's palette
	short palette_id = actor->Palette();
	PIXEL *palette  = shader->GetPalette(palette_id, zDistance, actor_sec->lightlevel);

	// calculate left and right screen columns for actor bitmap
	int left_col  = calc_screen_column(line->rx1,line->ry1);
	int right_col = calc_screen_column(line->rx2,line->ry2);

	// determine actor height relative to the player
	float actor_height;
	if (actor->flags & ACTOR_CEILHANG)
		actor_height = actor_sec->CeilHt(actor->x,actor->y);
	else
		actor_height = actor->z;
	actor_height -= player_height;

	// use this to calculate top and bottom screen rows for actor bitmap
	int bitmap_height = actor_bitmap->w;
	int top_row 	= calc_screen_row(line->rx1,actor_height);

	// render the actor's bitmap at the given screen location, and scale, returns visiblity of actor
	bool visible = 	render_RLE_bitmap(actor_bitmap,top_row,left_col,line->rx1,palette,actor->ColorRegions());
	//if (actor->IsNeighbor() && ((cNeighbor *)actor)->IsMonster())
	// ((cNeighbor *)actor)->SetVisible(visible);

	return visible;

}

bool point_in_actor(cActor *actor, int row, int column)
{
	linedef *line = actor->l;

	if (line == NULL)
		return false;

	if (line->rx1 <= 0.0f)
		return false;

	BITMAPINFO_4DX  *actor_bitmap = effects->EffectBitmap(line->bitmap);

	float actor_height = actor->z - player_height;

	int left_col	= calc_screen_column(line->rx1,line->ry1);
	int right_col	= calc_screen_column(line->rx2,line->ry2);
	int top_row 	= calc_screen_row(line->rx1,actor_height);
	int bottom_row = calc_screen_row(line->rx1,actor_height - actor_bitmap->w);

	if (row >= top_row  && row < bottom_row
		 && column >= left_col && column < right_col)
	{
		// transform given point to bitmap co-ords
		int scale_inv = DeltaV[float2int(line->rx1)];
		int bm_row = ((row - top_row) * scale_inv) >> 16;
		int bm_col = ((column - left_col)* scale_inv) >> 16;

		short *starts	= (short *)actor_bitmap->address;	// array of column start offsets in RLE bitmap
		return starts[bm_col] != 0 ;	// actor visible if not on transparent column.
/* 	UCHAR *run_length_ptr = actor_bitmap->address + starts[bm_col];
		int h = 0;
		while (h < actor_bitmap->w)
		{
			run_length_ptr += 2;
		  h += *run_length_ptr;
			if (bm_row <= h)
			{
				run_length_ptr++;
				return (( *run_length_ptr & 0x1F) != 0 );
			}
		}*/

	}
  return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
static void RLE_color_table_blit(void);
static void RLE_blit(void);

static int	  scale;
static int	  rle_remainder;
static int	  height;
static short *color_table;
static PIXEL *RLE_palette;
static int	  repeat;
static PIXEL *dest;
static UCHAR *src;


static void blit_RLE_column(PostDraw *pdraw)
{
	color_table	 = pdraw->color_table;
	scale 		 = pdraw->scale;
	RLE_palette	 = pdraw->palette;
	height		 = pdraw->destheight;
	repeat		 = pdraw->repeat+1;
	rle_remainder	 = 0;

	dest= pdraw->screencol;
	src = pdraw->ColumnPtr;
	int v_offset =0;
	int v = pdraw->v << 16;
	while (v_offset < v )
	{
		v_offset += *src * scale ;
		if (v_offset >= v)
		{
			rle_remainder = v_offset- v - *src * scale;
			break;
		}
		src += 2;
	}

	if (color_table)
		RLE_color_table_blit();
	else
		RLE_blit();

	pdraw->repeat = 0;
}

static void RLE2_color_table_blit();

static void blit_RLE2_column(PostDraw *pdraw)
{
	color_table  = pdraw->color_table;
	scale 		 = pdraw->scale;
	RLE_palette  = pdraw->palette;
	height		 = pdraw->destheight;
	repeat		 = pdraw->repeat+1;
	rle_remainder	 = 0;

	dest= pdraw->screencol;
	src = pdraw->ColumnPtr;


	if (color_table)
	{
		int v_offset =0;
		int v = pdraw->v>>16;
		int same_table_length= 0;
		while (v_offset < v )
		{
			if (!same_table_length)
			{
				src++;
				same_table_length = (*src & 0x1F);
			}
			int length = (*src >> 5);
			v_offset += length;
			if (v_offset >= v)
			{
				rle_remainder = (v_offset- v - length) * scale;
				break;
			}
			src++;
		}
		RLE2_color_table_blit();
	}
	else
	{
		int v_offset =0;
		int v = pdraw->v>>16;
		while (v_offset < v )
		{
			v_offset += *src ;
			if (v_offset >= v)
			{
				rle_remainder = (v_offset- v - *src) * scale;
				break;
			}
		src += 2;
		}

	 RLE_blit();
	}

	pdraw->repeat = 0;
}



static void RLE2_color_table_blit()
{
	static byte same_table_count;
	static word region_start;
	_asm {
			mov	edx,src
			mov	edi,dest
			push	ebp
			dec	edx

main_loop:
			inc	edx						// move to next byte in bitmap column
			xor	ecx,ecx 					// clear out registers
			mov	cl,byte ptr [edx]		// get color table index:count byte (3:5 split)
			mov	eax,ecx 					// save it in eax
			and	cl,0x1F 					// mask off count (of the consecutive pixels with the same color table index
			shr	al,5 						// make al color table index
			mov	esi,color_table		// pointer to color table
			mov	same_table_count,cl	// save count
			mov	bx,[esi][eax*2]		// get region number from color table
			shl	ebx,5						// multipy by 32 to get start of region
			mov	region_start,bx		// save it
			mov	esi,RLE_palette		// point esi to palette

same_table_loop:
			inc	edx						// move to next byte in bitmap column
			xor	ecx,ecx					// clear out reg.
			mov	cl,byte ptr [edx]		// get encoded pixel (run length:color_table_offset) (3:5 split)
			mov	al,cl						// save in al
			shr	cl,5						// make cl run length
			imul	ecx,scale 				// multiply by scale (16:16 fixed point) to get screen run length
			add	ecx,rle_remainder			// add remainder to ensure accuracy
			mov	ebx,ecx					// move length	into ebx (top 16)
			and	ecx,0xFFFF 				// mask off integer part to get new remainder
			shr	ebx,16					// shift top 16 to get actual length
			mov	rle_remainder,ecx 			// save remainder
			cmp	ebx,0						// check if run has length
			jbe	same_table_check		//
			and	al,0x1F					// mask off offset
			jz		transparent_pixel		// jump if pixel is transparent

			add	ax,region_start 		// add region start to offset to get actual pixel
			mov	PIX_REG_A,[esi][eax*PIXEL_SIZE]	// get color from palette[pixel]

			cmp	ebx,height				// is run length <= than remaining column height
			jbe	no_run_len_adjust		// yes: don't adjust
			mov	ebx,height				// no : set run length to height

no_run_len_adjust:
			sub	height,ebx

			// blit pixel to destination, a  [screen run length] by [column repeat] block
			mov	ebp,repeat
			mov	esi,view_pitch

run_len_loop:								// BEGIN outer run length loop
			mov	ecx,ebp

col_loop:									// BEGIN inner column repeat loop

			dec	ecx
			mov	[edi+ecx*PIXEL_SIZE],PIX_REG_A
			jnz	col_loop					// END column repeat loop

			add	edi,esi
			dec	ebx
			jnz	run_len_loop			// END run length loop

			cmp	height,0					// check if all of column is done
			jbe	exit1						// exit if so.

same_table_check:
			dec	same_table_count		// else check if we are still using the same color table index
			jnz	same_table_loop		// yes:
			jmp	main_loop 				// no : jump to top of main loop

transparent_pixel:
			mov	eax,ebx					// set eax to	screen run length
			imul	eax,view_pitch			// mult. by pitch to get new destination offset
			add	edi,eax					// add to edi to set new dest
			sub	height,ebx 				// decriment height by run len.
			cmp	height,0					// check if all of column is done
			jbe	exit1						// exit if so.

			dec	same_table_count		// else check if we are still using the same color table index
			jnz	same_table_loop		// yes:
			jmp	main_loop 				// no : jump to top of main loop

exit1:
			pop	ebp
	}
}

static void RLE_color_table_blit()
{
	_asm {
			mov	edi,dest
			push	ebp
			mov	edx,src
mloop:

			xor	ecx,ecx
			mov	cl,byte ptr [edx]
			add	edx,2
			imul	ecx,scale
			add	ecx,rle_remainder
			xor	eax,eax
			mov	ebx,ecx
			and	ecx,0xFFFF
			shr	ebx,16
			mov	rle_remainder,ecx
			cmp	ebx,0
			jbe	mloop

		// color region code
			mov	al,byte ptr [edx-1]

			mov	ecx,eax 					// save orginal pixel in eax
			and	eax,0x1F					// now modulo original pixel by 32 to get offset
			mov	esi,color_table		// pointer to color table
			jz		bypass

			shr	ecx,5						// divide pixel by 32 to get region
			mov	cx,[esi][ecx*2];		// get region number from region array into cx
			shl	ecx,5						// multiply by 32 to get start of region

			mov	esi,RLE_palette
			add	eax,ecx;					// add offset
			mov	PIX_REG_A,[esi][eax*PIXEL_SIZE]

			cmp	ebx,height				// is run length <= than remaining column height
			jbe	no_run_len_adjust		// yes: don't adjust
			mov	ebx,height				// no : set run length to height

no_run_len_adjust:

			sub	height,ebx

			mov	ebp,repeat
			mov	esi,view_pitch

run_len_loop:								// BEGIN outer run length loop
			mov	ecx,ebp

col_loop:									// BEGIN inner column repeat loop
			dec	ecx
			mov	[edi+ecx*PIXEL_SIZE],PIX_REG_A
			jnz	col_loop					// END column repeat loop

			add	edi,esi
			dec	ebx
			jnz	run_len_loop			// END run length loop

			cmp	height,0
			jg		mloop
			jmp	exit1

bypass:
			mov	eax,ebx
			imul	eax,view_pitch
			add	edi,eax

			sub	height,ebx
			cmp	height,0
			jg		mloop

exit1:
			pop	ebp
	}
}




void RLE_blit(void)
{
	_asm {
			mov	edi,dest
			push	ebp
			mov	edx,src

	mloop:
			xor	ecx,ecx
			mov	cl,byte ptr [edx]
			add	edx,2
			imul	ecx,scale
			add	ecx,rle_remainder
			xor	eax,eax
			mov	ebx,ecx
			and	ecx,0xFFFF
			shr	ebx,16
			mov	rle_remainder,ecx
			cmp	ebx,0
			jbe	mloop

			mov	al,byte ptr [edx-1]
			mov	esi,RLE_palette
			cmp	al,0
			je		bypass

			mov	PIX_REG_A,[esi][eax*PIXEL_SIZE]

			cmp	ebx,height				// is run length <= than remaining column height
			jbe	no_run_len_adjust		// yes: don't adjust
			mov	ebx,height				// no : set run length to height

no_run_len_adjust:
			sub	height,ebx

												// BEGIN outer run length loop
			mov	ebp,repeat
			mov	esi,view_pitch

run_len_loop:
												// BEGIN inner column repeat loop
			mov	ecx,ebp

col_loop:
			dec	ecx
			mov	[edi+ecx*PIXEL_SIZE],PIX_REG_A
			jnz	col_loop
												// END loop

			add	edi,esi
			dec	ebx
			jnz	run_len_loop
												// END loop

			cmp	height,0
			jg		mloop
			jmp	exit1

bypass:
			mov	eax,ebx
			imul	eax,view_pitch
			add	edi,eax

			sub	height,ebx
			cmp	height,0
			jg		mloop
exit1:
			pop	ebp
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// PostDraw list: used to draw objects with transparent pixels

static void add_to_postdraw_list(unsigned char *ColumnPtr,int v,int destheight,int deltav, PIXEL *screencol,
															PIXEL *palette, short *color_table)
{
	if (color_table && PostCount)
	{
		PostDraw *prev = &PostDrawList[PostCount-1];

		if ((ColumnPtr == prev->ColumnPtr) && (v == prev->v) && (destheight == prev->destheight))
		{
			prev->repeat++;
			return;
		}
	}

	if (PostCount >= MAX_POST_COUNT)
		return;

	PostDraw *pdraw = &PostDrawList[PostCount++];

	pdraw->ColumnPtr		= ColumnPtr;
	pdraw->v 				= v;
	pdraw->destheight		= destheight;
	pdraw->scale			= deltav;
	pdraw->screencol		= screencol;
	pdraw->palette 		= palette;
	pdraw->color_table	= color_table;
}


static void draw_postdraw_list(void)
{
  // Draw the PostDraw List.
  for (int i = PostCount-1; i>=0;i-- )
  {
		PostDraw *pdraw = &PostDrawList[i];
		//if (opt_on)
			//blit_RLE2_column(pdraw);
		//else
			blit_RLE_column(pdraw);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void cColumnClipper::init(int view_width)
{
	spans = new column_span[view_width];
}

cColumnClipper::~cColumnClipper()
{
 delete [] spans;
 spans  =  0;
}

void cColumnClipper::frame_reset(void)
{
	memset(spans,0, sizeof spans[0]*view_width);
}


void cColumnClipper::add_span(int col,int start,int end)
{
	eClipTestResult result = test_span(col,start,end);
	switch (result)
	{
		case DISJOINT:
			if ((spans[col].end - spans[col].start) < (end-start)) // if new span is longer than current
			{
				spans[col].start = start; // make it the current span
				spans[col].end	 = end;
			}
		//update_scanline_spans( col, start, end);
		break;

		case CONTAINED:			// do nothing
		break;

		case CONTAINS: 			//set span to new span
			spans[col].start = start;
			spans[col].end	 = end;
			//update_scanline_spans(col,	start,  end);
		break;

		case START_OVERLAP:		//join span to new span
			//update_scanline_spans( col,  start, spans[col].start );
			spans[col].start = start;
		break;

		case END_OVERLAP: //join new span to span
			//update_scanline_spans( col,spans[col].end , end);
			spans[col].end = end;
		break;
	}
}

// tests a span against the span in the column clipper.
// returns	the relationship of the test span to the clipper span.
eClipTestResult cColumnClipper::test_span(int col,int start,int end)
{
	int span_start = spans[col].start;
	int span_end	= spans[col].end;

	if (start < span_start)
	{
		if (end >= span_end)
			return CONTAINS;
		if (end >= span_start)
			return START_OVERLAP;
		return DISJOINT;
	}
	else
	{
		if (start > span_end)
			return DISJOINT;
		if (end <= span_end)
			return CONTAINED;
		return END_OVERLAP;
	}
}


eClipTestResult cColumnClipper::test_span_and_clip(int col,int &start,int &end)
{
  eClipTestResult result = test_span(col,start,end);
	switch (result)
	{
		case DISJOINT:// do nothing
		break;

		case CONTAINED:// clip totally
			start = end = 0;
		break;

		case CONTAINS: 	// do nothing
		break;

		case START_OVERLAP: //clip end
			end = spans[col].start;
		break;

		case END_OVERLAP:// clip start
			start = spans[col].end;
		break;
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

eClipTestResult cRowClipper::test_span(span &s,int start,int end)
{
	int span_start = s.start;
	int span_end	= s.end;

	if (start < span_start)
	{
		if (end >= span_end)
			return CONTAINS;
		if (end >= span_start)
			return START_OVERLAP;
		return DISJOINT;
	}
	else
	{
		if (start > span_end)
			return DISJOINT;
		if (end <= span_end)
			return CONTAINED;
		return END_OVERLAP;
	}
}


void cRowClipper::delete_span(int row, int i)
{
	span_row &sr = span_rows[row];
	memmove(&sr[i],&sr[i+1], (num_spans[row] - i)* sizeof span);
	num_spans[row]--;
}


void cRowClipper::add_span(int row, int i, int span_start, int span_end)
{
	span_row &sr = span_rows[row];
	memmove(&sr[i+1],&sr[i], (num_spans[row] - i)* sizeof span);
	sr[i].start = span_start;
	sr[i].end	= span_end;
	num_spans[row]++;
}


void cRowClipper::insert_span(int row, int span_start, int span_end)
{
	span_row &sr = span_rows[row];
	int i = 0;
	for (i = 0; i < num_spans[row]; i++)
	{
		span &s = sr[i];
		if (span_start > s.end )
			continue;

		eClipTestResult result = test_span(sr[i],span_start,span_end);
		switch (result)
		{
			case DISJOINT:
				add_span(row,i,span_start,span_end);
				return;			 // add new span, disjoint with the rest of the spans in row so return

			case CONTAINED:
				return;		  // fully contained in existing span, no need to check others

			case CONTAINS:
				delete_span(row,i); i--;
				continue;

			case START_OVERLAP:
				sr[i].start = span_start;
				return;

			case END_OVERLAP:
				span_start = sr[i].start;
				delete_span(row,i); i--;
			break;
		}
	}
	add_span(row,i,span_start,span_end);
}



// tests a span ( a floor/wall segment), clips it if required and returns if it is still visible (true)
bool cRowClipper::test_span_and_clip(int row, short &span_start, short &span_end)
{
	span_row &sr = span_rows[row];
	for (int i = 0; i < num_spans[row]; i++)
	{
		span &s = sr[i];
		if (span_start > s.end )
			continue;

		eClipTestResult result = test_span(sr[i],span_start,span_end);
		switch (result)
		{
			case DISJOINT:
				return true;			 // disjoint with the rest of the spans in row so return

			case CONTAINED:
				return false;		  // fully contained in existing span, no need to check others , not visible

			case CONTAINS:
				continue;

			case START_OVERLAP:
				span_end = s.start;
				return true;

			case END_OVERLAP:
				span_start = s.end;
			  continue;
		}
	}
	return true;
}

void cRowClipper::frame_reset(void)
{
	memset(num_spans,0, sizeof num_spans);
}

/*
bool test_span_and_clip(int row, short &start, short &end)
{
 return row_clipper.test_span_and_clip(row, start, end);
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

void RenderMap(unsigned char *viewBuffer, bool show_players_position)
{
	const PIXEL IN_ROOM_COLOR = 159;

	int current_room = player->Room();
	//int level = 0;

	BITMAPINFO_4DX *map_bitmap = effects->EffectBitmap(160);
	PIXEL *palette = shader->GetUnshadedPalette(map_bitmap->palette);
	PIXEL *dest = (PIXEL *)viewBuffer  ;
	UCHAR *src = map_bitmap->address;

	static int frames = 0;
	bool show_dot = (show_players_position) && (frames++ & 16);  // flashes dot

	for (int row = 0; row < view_height; row++ )
	{
		PIXEL color=0;
		for (int col = 0; col < view_width; col++)
		{
			if ((col <map_bitmap->h) && (row<map_bitmap->w))
			{
				UCHAR  pixel	= *src++;
				if (pixel < 32)
				{
					if (show_dot && pixel == current_room)
						color = 	palette[IN_ROOM_COLOR];
					// else
						// pixel = NOT_IN_ROOM_COLOR;
				}
				else
					color = palette[pixel];
				*dest++ = color;
			} else 
			{
				*dest++ = 0;
			}
		}
	}
}


int FindRoomOnMap(int x,int y)
{
	struct
	{
	  int row;
	  int col;
	}
	room_points[32];

	memset( room_points, 0 , sizeof room_points);

// First get map bitmap positions of  all the rooms
	BITMAPINFO_4DX *map_bitmap = effects->EffectBitmap(160);
	UCHAR *src =	map_bitmap->address;
	for (int row = 0; row < map_bitmap->w; row++ )
	{
		for (int col = 0; col <map_bitmap->h; col++)
		{
			UCHAR  pixel = *src++;
			if (pixel < 32)
			{
				room_points[pixel].row = row;
				room_points[pixel].col = col;
			}
		}
	}

	// now see which room point is closest to given point
	unsigned min_distance = 30*30;
	int closest_room = 0;
	for (int r = 1; r < level->NumRooms(); r++)
	{
		if (!room_points[r].row)
			continue;
		int ydiff = room_points[r].row - y;
		int xdiff = room_points[r].col - x;
		unsigned distance = (xdiff*xdiff + ydiff*ydiff);
		if ( distance	< min_distance)
		{
			min_distance = distance;
			closest_room = r;
		}
	}

  return closest_room;
}
