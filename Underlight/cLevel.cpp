// cLevel: Class for game level files.
//
// Copyright Lyra LLC, 1996. All rights reserved.
//

#define STRICT

#include "Central.h"
#include <stdio.h>
#include <math.h>
#include "cDDraw.h"
#include "cPlayer.h"
#include "cEffects.h"
#include "cActorList.h"
#include "cLevel.h"
#include "cGameServer.h"
#include "Resource.h"
#include "Move.h"
#include "Realm.h"
#include "Options.h"
#include "cOrnament.h"
#include "zlib.h"

#define texture_file_name "textures.rlm"

//#define DEMONSTRATION

///////////////////////////////////////////////////////////////////
// Global Variables

///////////////////////////////////////////////////////////////////
// External Global Variables

extern cPlayer *player;
extern options_t options;
extern cDDraw *cDD;
extern cEffects *effects;
extern cActorList *actors;
extern int numanimsecs;
extern bool leveleditor;
extern bool actor_delete_ok;
extern cGameServer *gs;
extern HINSTANCE hInstance;

long g_lLevelFileCheckSum = 0;


#ifdef UL3D
// temp!
level_header_t saved_headers[MAX_LEVELS];
#endif

#ifdef UL_DEBUG
#define FILE_ERROR GAME_ERROR
#else
#define FILE_ERROR(msg) GAME_ERROR(IDS_ERR_LOAD_LEVEL)
#endif

///////////////////////////////////////////////////////////

#ifdef OTHERLANGUAGENAMEHERE
#include "LevelRoomNames.cpp"
#else // English or other single byte language version
TCHAR Level_Names[MAX_LEVELS][LEVEL_NAME_SIZE];
TCHAR Room_Names[MAX_ROOMS][ROOM_NAME_SIZE];
#endif



////////////////////////////////////////////////////////////////

// performs a normal fread and does a checksum on the data read in
static size_t checksum_fread( void *buffer, size_t size, size_t count, FILE *stream )
{
	size_t result = fread(buffer,size, count, stream);

	for (size_t i = 0; i < size*count; i++)
		g_lLevelFileCheckSum += ((byte *)buffer)[i];
	return result;
}

static void decrypt(void *ptr, int size)
{
#pragma message ("Room Decryption Code")
#ifndef GAME_LYR
	byte *src = (byte *)ptr;
	unsigned char c = (src[0] >> 6);
	unsigned char* s = src;

	for (s= src; s < src+size-1 ; s++)
	{
		*s <<=2;
		*(s) |= *(s+1) >> 6;
	}

	*s <<= 2;
	*s |= c;
#endif
}



/////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
cLevel::cLevel(TCHAR *filename)
{
	level_headers = NULL;
	texture_headers = NULL;
	flat_headers= NULL;
	texture_bytes = max_texture_bytes = 0;
	memset(&level_data, 0, sizeof(level_data));

	level_id = NO_LEVEL; // initially, no level is loaded

	for (int i=0; i<MAX_TEXTURES; i++)
	{
		memset(&(texture_info[i]), 0, sizeof(BITMAPINFO_4DX));
		TextureLoadList[i].load_status = UNUSED;
	}

	for (int i=0; i<MAX_FLATS; i++)
	{
		memset(&(flat_info[i]), 0, sizeof(BITMAPINFO_4DX));
		FlatLoadList[i].load_status = UNUSED;
		flat_info[i].address = flat_bits;
	}
#ifdef _DEBUG
	for (int i=0; i<MAX_LEVELS; i++)
		for (int j=0; j<Avatars::MAX_AVATAR_TYPE+1; j++)
			GeneratorsUsed[i][j] = 0;
#endif

	_tcscpy(level_file_name, filename);

	if (!leveleditor) // in editor mode, open/close for each load
		this->OpenFiles();

#ifdef UL3D
	write_data = false;
#endif

	return;
}


// Destructor
cLevel::~cLevel(void)
{
	//_tprintf("max texture bytes used: %d\n",max_texture_bytes);

#ifndef AGENT
	for (int i=0; i<MAX_TEXTURES; i++)
		if (TextureLoadList[i].load_status == USED)
		{
			delete [] texture_info[i].address;
			texture_info[i].address = NULL;
			effects->FreeEffectPalette(texture_info[i].palette);
		}
	for (int i=0; i<MAX_FLATS; i++)
		if (FlatLoadList[i].load_status == USED)
		{	 // don't delete flat bits because all flats are interleaved into
			// one statically allocated chunk of memory
			flat_info[i].address = flat_bits;
			effects->FreeEffectPalette(flat_info[i].palette);
		}
#endif !AGENT

	if (!leveleditor) // in editor mode, open/close for each load
		this->CloseFiles();

	CleanSectors();

#ifdef AGENT
	TlsSetValue(tlsLevel, NULL);
#endif AGENT

	return;
}

// opens & reads headers for the texture and game files
void cLevel::OpenFiles(void)
{
	texture_file_header header;

	level_fh =_tfopen(level_file_name , _T("rb"));
	if (level_fh == NULL)
	{
		GAME_ERROR(IDS_NO_OPEN_LVL_FILE);
		return;
	}

	// Load the version
	if( fread(&level_file_header,sizeof(level_file_header),1,level_fh) != 1)
	{
		GAME_ERROR(IDS_ERR_LOAD_LVL_FILE_HDR);
		return;
	}

#ifndef DEMONSTRATION
	if (level_file_header.version != CURRENT_VERSION)
	{
		GAME_ERROR(IDS_LVL_FILE_INCOR_VERS_NUM);
		return;
	}
#endif

	num_levels = level_file_header.num_levels;

	level_headers = new level_header_t[num_levels];

	// Load the level headers
	if( (int)fread(level_headers,sizeof(level_header_t),num_levels,level_fh) != num_levels)
	{
		GAME_ERROR(IDS_ERR_LOAD_LVL_HDR);
		return;
	}

//	GAME_ERROR1(IDS_INVALID_LEVEL, 13);

	// decrypt level names
	for(int i = 0; i < num_levels; i++)
	{
#ifdef UL3D
		memcpy(&saved_headers[i], (const void*)&level_headers[i], sizeof(level_header_t));
#endif

		decrypt(level_headers[i].name,sizeof(level_headers[i].name));
		if ((level_headers[i].level_id < 0) ||
			 (level_headers[i].level_id >= MAX_LEVELS))
		{
			GAME_ERROR1(IDS_INVALID_LEVEL, level_headers[i].level_id);
			return;
		}

#ifndef _UNICODE
		// note - do NOT change to _t - editor files are always single byte!!!
		// for single byte languages, we copy file names from the editor
		strcpy(Level_Names[i], level_headers[i].name);
#endif
		level_loaded[level_headers[i].level_id] = true;
	}

	// open texture file
	texture_fh =_tfopen(_T(texture_file_name), _T("rb+"));
	if (texture_fh == NULL)
	{
		GAME_ERROR(IDS_NO_OPEN_TEXTURE);
		return;
	}

	// read texture file header
	if ((fread(&header, sizeof(header), 1, texture_fh)) != 1)
	{
		GAME_ERROR(IDS_NO_READ_TEXTURE_FILE_HDR);
		return;
	}

	num_textures = header.num_textures;
	num_flats = header.num_flats;

	// read texture headers
	texture_headers = new texture_header_t[num_textures];
	if (fread(texture_headers, sizeof(texture_header_t), (size_t)num_textures, texture_fh) != (UINT)num_textures)
	{
		//_tprintf("num textures: %d\n",num_textures);
		GAME_ERROR(IDS_NO_READ_TEXTURE_HDR);
		return;
	}

	// seek to flat headers
	if (fseek(texture_fh, texture_headers[num_textures-1].file_position+texture_headers[num_textures-1].height*texture_headers[num_textures-1].width, SEEK_SET))
	{
		GAME_ERROR(IDS_NO_SEEK_READ_FLAT_HDR);
		return;
	}

	// read flat headers
	flat_headers = new flat_header_t[num_flats];
	if (fread(flat_headers, sizeof(flat_header_t), (size_t)num_flats, texture_fh) != (UINT)num_flats)
	{
		//_tprintf("num flats: %d\n",num_flats);
		GAME_ERROR(IDS_NO_READ_FLAT_HDR);
		return;
	}

	return;
}

#ifdef UL3D
// helper to remove multi hull sectors
void cLevel::ProcessFiles(void)
{
	offset = 0;
	new_fh =_tfopen("test2.new" , _T("wb"));
	if (new_fh == NULL)
	{
		GAME_ERROR(IDS_NO_OPEN_LVL_FILE);
		return;
	}

	// Load the version
	if( fwrite(&level_file_header,sizeof(level_file_header),1,new_fh) != 1)
	{
		GAME_ERROR(IDS_ERR_LOAD_LVL_FILE_HDR);
		return;
	}

	offset += sizeof(level_file_header);

	// Load the level headers
	if( (int)fwrite(saved_headers,sizeof(level_header_t),num_levels,new_fh) != num_levels)
	{
		GAME_ERROR(IDS_ERR_LOAD_LVL_HDR);
		return;
	}

	offset += sizeof(level_header_t)*num_levels;

	write_data = true;
	for (int i=0; i<num_levels; i++)
	{	
		int level_id = level_headers[i].level_id;
		//int level_id = 41;
		int old = saved_headers[i].file_position;
		saved_headers[i].file_position = offset;
		if (old != offset) {
			int zz = 0;
		}
		this->Load(level_id);
		actors->Purge();
	}

	fseek(new_fh, sizeof(level_file_header), SEEK_SET);

	// Load the level headers
	if( (int)fwrite(saved_headers,sizeof(level_header_t),num_levels,new_fh) != num_levels)
	{
		GAME_ERROR(IDS_ERR_LOAD_LVL_HDR);
		return;
	}

	fclose(new_fh);
	return;
}
#endif


// closes & deletes headers for the texture and game files
void cLevel::CloseFiles(void)
{
	if (texture_fh)
		fclose(texture_fh);
	if (level_fh)
		fclose(level_fh);

	if (texture_headers)
		delete [] texture_headers;
	if (flat_headers)
		delete [] flat_headers;
	if (level_headers)
		delete [] level_headers;

	level_headers = NULL;
	texture_headers = NULL;
	flat_headers= NULL;

	return;
}

TCHAR* cLevel::Name(int id)
{
	if (this->IsValidLevel(id)) {

		return Level_Names[id-1];
		//return level_headers[id-1].name;  // sub 1 since levels start at 1
	}
	else
	{
		LoadString(hInstance, IDS_LEVEL_ID , temp_message, sizeof(temp_message));
		_stprintf(errbuf, temp_message, id);
		NONFATAL_ERROR(errbuf);
		return _T("");
	}
}

TCHAR* cLevel::RoomName(int id)
{
	return Room_Names[id];
}

bool cLevel::FlatIsLoaded(int flat_id)
{
	if (flat_id < MAX_FLATS)
		return (FlatLoadList[flat_id].load_status == USED);
	else
	{
		LoadString(hInstance, IDS_FLAT_ID , temp_message, sizeof(temp_message));
		_stprintf(errbuf, temp_message, flat_id);
		NONFATAL_ERROR(errbuf);
		return false;
	}
}


void cLevel::Load(int id)
{
	level_id = id;
	cActor *a;
	int texture_ram = 0;
	int num_palettes = 0;
#ifndef AGENT
	bool palettes_used[MAX_PALETTES];
#endif !AGENT

	// reject any outstanding party queries

	// clean out old level data
	CleanSectors();
	texture_bytes = 0;

	// delete actors that don't survive level changes
	for (a = actors->IterateActors(INIT); a != NO_ACTOR; a = actors->IterateActors(NEXT))
		if (!(a->SurviveLevelChange()))
			a->SetTerminate();
		else
			a->sector = DEAD_SECTOR;
	actors->IterateActors(DONE);

	// mark all used textures/flats for removal, and load new level data;
	// this will set reused textures to USED and mark the textures
	// that must be loaded.

	for (int i=0; i<MAX_TEXTURES; i++)
		if (TextureLoadList[i].load_status == USED)
			TextureLoadList[i].load_status = REMOVE;

	// all flats must be nuked & reloaded for each level
	for (int i=0; i<MAX_FLATS; i++)
	{
		if (FlatLoadList[i].load_status == USED)
		{
			flat_info[i].address = flat_bits;
			effects->FreeEffectPalette(flat_info[i].palette);
		}
		FlatLoadList[i].load_status = UNUSED;
	}

	if (leveleditor)
		this->OpenFiles();

	player->sector = DEAD_SECTOR;

	LoadLevelData();

#ifndef AGENT
	// now remove the textures not used by the new level
	for (int i=0; i<MAX_TEXTURES; i++)
		if (TextureLoadList[i].load_status == REMOVE)
		{
			delete [] texture_info[i].address;
			texture_info[i].address = NULL;
			effects->FreeEffectPalette(texture_info[i].palette);
			TextureLoadList[i].load_status = UNUSED;
		}

	// now load in the new textures and flats needed by the new level

	LoadBitmaps();

	// now ensure that all bitmaps needed are loaded
	for (int i=0; i<MAX_TEXTURES; i++)
	{
		if (TextureLoadList[i].load_status == ADD)
		{
			LoadString(hInstance, IDS_TEXTURE_ID , temp_message, sizeof(temp_message));
			_stprintf(errbuf, temp_message, i);
			NONFATAL_ERROR(errbuf);
		}
		if (TextureLoadList[i].load_status == USED)
		{
			texture_ram += texture_info[i].w*texture_info[i].h;
			if (!palettes_used[texture_info[i].palette])
			{
				num_palettes++;
				palettes_used[texture_info[i].palette] = true;
			}
#ifdef _DEBUG
			TexturesUsed[i] = 1;
#endif _DEBUG
		}
	}

	for (int i=0; i<MAX_FLATS; i++)
	{
		if (FlatLoadList[i].load_status == ADD)
		{
			LoadString(hInstance, IDS_FLATID , temp_message, sizeof(temp_message));
			_stprintf(errbuf, temp_message, i);
			NONFATAL_ERROR(errbuf);
		}
		if (FlatLoadList[i].load_status == USED)
		{
#ifdef _DEBUG
			FlatsUsed[i] = 1;
#endif _DEBUG
			if (!palettes_used[flat_info[i].palette])
			{
				num_palettes++;
				palettes_used[flat_info[i].palette] = true;
			}
		}
	}

#endif !AGENT

	if (leveleditor)
	{
		this->CloseFiles();
	_stprintf(errbuf, _T("Level %d Loaded; Texture RAM Usage: %d # of Palettes Used: %d"), level_id, texture_ram, num_palettes);
		INFO(errbuf);
	}

	return;
}


void cLevel::LoadBitmaps(void)
{
	int i,id,next_flat=0;
	// next flat references the next open spot in flat_bits,
	// the space allocated for 32 flats

	//_tprintf("num textures: %d\n",num_textures);

	for (i=0; i<num_textures; i++)
	{
		id = texture_headers[i].id;
		if (TextureLoadList[id].load_status != ADD)
			continue;
		else
			TextureLoadList[id].load_status = USED;

		texture_info[id].w = texture_headers[i].height;
		texture_info[id].h = texture_headers[i].width;
		texture_info[id].palette = texture_headers[i].palette;

		effects->LoadEffectPalette(texture_info[id].palette);
		//_tprintf("Texture %d, h = %d, w = %d, pal = %d at pos %d\n",id,
		// texture_info[id].h, texture_info[id].w, texture_info[id].palette, texture_headers[i].file_position);

		// allocate memory

#ifndef AGENT
		texture_info[id].address = new unsigned char[texture_info[id].w * texture_info[id].h]; // 1 bpp for textures
		texture_bytes += texture_info[id].w * texture_info[id].h;
#endif !AGENT

		if (fseek(texture_fh, texture_headers[i].file_position, SEEK_SET))
		{
				GAME_ERROR(IDS_NO_SEEK_FIND_TEXTURE);
			return;
		}

		//_tprintf("reading visual effect %d\n",i);
		if ((fread((texture_info[id].address), (texture_info[id].w * texture_info[id].h), 1, texture_fh)) != 1)
		{
				GAME_ERROR(IDS_NO_READ_TEXTURE_BIT);
			return;
		}
	}
///_tprintf("texture bytes for level %d: %d\n",level_id, texture_bytes);
	if (texture_bytes > max_texture_bytes)
		max_texture_bytes = texture_bytes;

	for (i=0; i<num_flats; i++)
	{
		id = flat_headers[i].id;
		if (FlatLoadList[id].load_status != ADD)
			continue;
		else
			FlatLoadList[id].load_status = USED;
		flat_info[id].w = flat_headers[i].height;
		flat_info[id].h = flat_headers[i].width;
		flat_info[id].palette = flat_headers[i].palette;
		effects->LoadEffectPalette(flat_info[id].palette);

		// set address pointer correctly
		flat_info[id].address = flat_bits + 4096*next_flat;
		next_flat++;
		if (next_flat > MAX_SIMUL_FLATS)
		{
			GAME_ERROR(IDS_TOO_MANY_FLATS);
			return;
		}


		if (fseek(texture_fh, flat_headers[i].file_position, SEEK_SET))
		{
			GAME_ERROR(IDS_NO_SEEK_FIND_FLAT);
			return;
		}

		if ((fread(flat_info[id].address, flat_info[id].w*flat_info[id].h, 1, texture_fh)) != 1)
		{
			GAME_ERROR(IDS_NO_READ_FLAT_BIT);
			return;
		}
	}

	// Now arrange flats data	such that each line of a flat is 256  bytes away from the previous
	// line. (required by graphics engine).
	// To save space, interleave flats in groups of 4 (256/64). i.e in a 64*64*4 block,
	// first 256 bytes contain  line 0 of flats 0..3,	next 256 bytes contain line 1 of flats 0...3.
	// In the next 4096 block, repeat for flats 4..7, and so on.
	{
		UCHAR buffer[4096*4];

		for (int i = 0; i < MAX_SIMUL_FLATS; i+=4) // groups of 4
		{
			UCHAR *b = buffer;
			for(int line = 0; line < 64; line++)
			{
				for (int j = 0; j < 4; j++)	  // for each flat in group of 4
				{
					//memcpy(b,flat_info[i+j].address+line*64,64); //interleave,data into temp buffer
					memcpy(b,flat_bits+4096*(i+j)+line*64,64); //interleave,data into temp buffer
					b += 64;
				}
			}

			memcpy(flat_bits+4096*i,buffer,sizeof(buffer)); // copy interleaved data back
			for (int j = 0; j < MAX_FLATS; j++)
			{
				if (flat_info[j].address == flat_bits + 4096*(i+1))
					flat_info[j].address = flat_bits + 4096*i + 64;
				else if (flat_info[j].address == flat_bits + 4096*(i+2))
					flat_info[j].address = flat_bits + 4096*i + 64*2;
				else if (flat_info[j].address == flat_bits + 4096*(i+3))
					flat_info[j].address = flat_bits + 4096*i + 64*3;
				//flat_info[i+j].address = flat_info[i].address+j*64; // adjust pointers
			}
		}
	}
	// Craig - put prerotate stuff here!!! ***
}


void cLevel::CleanSectors(void)
{
	int	i;
	sector  *pSec;
	linedef *l,*temp;

	for (i = 0; i < level_data.num_sectors; i++)
		{
		pSec = Sectors[i];
		if ( pSec )
			{
			l	  = pSec->firstline;
			if ( l )
				{
				while(1)
					{
					temp = l->nextline;
					delete l;
					if(!temp) break;
					l = temp;
					}
				}
			delete pSec;
			}
		Sectors[i]=NULL;
		}
	for(;i<MAX_SECTORS;i++)
		{
		Sectors[i]=NULL;
		}
	level_data.num_sectors=0;
	numanimsecs=0;
}


BOOL cLevel::LoadLevelData(void)
{
	vertex  TempVert, *v=&TempVert;
	int i,lines,num_ornaments;
	cOrnament *ornament;

// Ul3d note: this method modified to write out level data as it is loaded!

	g_lLevelFileCheckSum = 0;
	num_ornaments = 0;

	// find level id
	for (i=0; i< num_levels; i++)
		if (level_headers[i].level_id == level_id)
			break;

	if (i == num_levels)
	{
		FILE_ERROR(IDS_CANT_MATCH_ID);
		return FALSE;
	}

	if (fseek(level_fh, level_headers[i].file_position, SEEK_SET))
	{
		FILE_ERROR(IDS_SEEK_ERR);
		return FALSE;
	}

	// Load the control record
	if( fread(&level_data,sizeof(level_data),1,level_fh) != 1)
	{
		FILE_ERROR(IDS_CONTROL_RECORD_ERR);
		return FALSE;
	}


	if (level_data.num_sectors > MAX_SECTORS)
	{
		FILE_ERROR(IDS_SECTORS_ERR);
		return FALSE;
	}

	if (level_data.num_vertices > MAX_VERTICES)
	{
		FILE_ERROR(IDS_VERTICES_ERR);
		return FALSE;
	}

	if (level_data.num_rooms > MAX_ROOMS)
	{
		FILE_ERROR(IDS_ROOMS_ERR);
		return FALSE;
	}

	unsigned file_checksum = level_data.checksum2 | (level_headers[i].checksum1 << 16);



	// Load the Sectors
	for (i=0; i<level_data.num_sectors; i++ )
	{
		file_sector fs;
		if( checksum_fread(&fs,sizeof(file_sector),1,level_fh) != 1)
		{
			FILE_ERROR(IDS_LOADING_SECTORS_ERR);
			return FALSE;
		}
#ifdef UL3D
		memcpy(&all_fs[i], &fs, sizeof(file_sector));
#endif

		bool animated = ((fs.flags & SECTOR_SELFILLUMINATING) ||  fs.xanim_top || fs.xanim_bottom
								|| fs.yanim_top || fs.yanim_bottom );
		Sectors[fs.id] = animated	? new cAnimatedSector(fs) :  new sector(fs);
		if (animated)
			Sectors[fs.id]->flags = Sectors[fs.id]->flags | SECTOR_ANIMATING;

#ifdef _DEBUG

		FlatsUsed[fs.ceiling_bitmap] = 1;
		FlatsUsed[fs.floor_bitmap] = 1;

#endif _DEBUG

	///_tprintf("adding sector: %d room: %d\n",fs.SecNo, fs.tag);

		if (FlatLoadList[fs.ceiling_bitmap].load_status == UNUSED)
			FlatLoadList[fs.ceiling_bitmap].load_status = ADD;

		if (FlatLoadList[fs.floor_bitmap].load_status == UNUSED)
			FlatLoadList[fs.floor_bitmap].load_status = ADD;

		//_tprintf("top: %d bottom: %d\n",fs.ceiling_bitmap,fs.floor_bitmap);

	}

	Sectors[DEAD_SECTOR]->room = 0; // make sure sector 0 = room 0
	//_tprintf("loaded sectors.\n");

	// Load the Vertices
	if ((checksum_fread(&FileVertices[0],sizeof(file_vertex),level_data.num_vertices,level_fh))
		!= (UINT)level_data.num_vertices)
	{
		FILE_ERROR(IDS_LOADING_VERTICES_ERR);
		return FALSE;
	}


	//_tprintf("loading vertices.\n");

	// now loop through and create the vertices
	for (i=0; i<level_data.num_vertices; i++)
	{
		Vertices[i].x = (float)FileVertices[i].x;
		Vertices[i].y = (float)FileVertices[i].y;
	}


	//_tprintf("loaded vertices!\n");

	// Load the lines
	//_tprintf("loading lines...\n");
	file_linedef file_line, *l= &file_line;
	lines = level_data.num_lines;
	level_data.num_lines=0;



	for (i=0; i< lines; i++)
	{
		if( checksum_fread(&file_line,sizeof(file_line),1,level_fh) != 1)
		{
			FILE_ERROR(IDS_LOADING_LINES_ERR);
			return FALSE;
		}

#ifdef UL3D
		memcpy(&all_lines[i], &file_line, sizeof(file_line));
#endif

		if (i == 91) {
			int qbd = 0;
		}

		AddLine(file_line);


	// *****
	//if ((l->fwall_bitmap == 1504) || (l->cwall_bitmap == 1504) || (l->animation_end == 1504))
		//_tprintf("line with vertices %d, %d in sector %d in level %d references texture 741\n", l->from, l->to, l->sector, level_id);

#ifdef _DEBUG

		TexturesUsed[l->fwall_bitmap] = 1;
		TexturesUsed[l->cwall_bitmap] = 1;

#endif

		// add bitmaps to loadList
		if (TextureLoadList[l->fwall_bitmap].load_status == UNUSED)
			TextureLoadList[l->fwall_bitmap].load_status = ADD;
		else if (TextureLoadList[l->fwall_bitmap].load_status == REMOVE)
			TextureLoadList[l->fwall_bitmap].load_status = USED;

		if (TextureLoadList[l->cwall_bitmap].load_status == UNUSED)
			TextureLoadList[l->cwall_bitmap].load_status = ADD;
		else if (TextureLoadList[l->cwall_bitmap].load_status == REMOVE)
			TextureLoadList[l->cwall_bitmap].load_status = USED;

		if (TextureLoadList[l->animation_end].load_status == UNUSED)
			TextureLoadList[l->animation_end].load_status = ADD;
		else if (TextureLoadList[l->animation_end].load_status == REMOVE)
			TextureLoadList[l->animation_end].load_status = USED;

		//if (TextureLoadList[l->unused].load_status == UNUSED)
		// TextureLoadList[l->unused].load_status = ADD;
		//else if (TextureLoadList[l->unused].load_status == REMOVE)
		// TextureLoadList[l->unused].load_status = USED;

		// add bitmaps for animated linedefs to loadlist
		if (l->flags & LINE_ANIMATED)
				for (int i = l->fwall_bitmap+1; i < l->animation_end;i++)
				if (TextureLoadList[i].load_status == UNUSED)
					TextureLoadList[i].load_status = ADD;
				else if (TextureLoadList[i].load_status == REMOVE)
					TextureLoadList[i].load_status = USED;
	}


	 // Once all the lines are loaded, allow sectors to do any initialization,
	 // that requires the sectors' lines to be setup.
	for(i=0;i<level_data.num_sectors;i++)
	{
		Sectors[i]->PostLineLoadInit();
	}

#ifdef UL3D

	// first, combine vertices
	for (int m=0; m< level_data.num_vertices; m++)
	{
		//printf("M: %d\n", m);
		for (i=m+1; i< level_data.num_vertices; i++)
		{
			if ((FileVertices[i].x == FileVertices[m].x) &&
				(FileVertices[i].y == FileVertices[m].y)) 
			{ // replace occurences of u with m
				//printf("Replacing vertex %d with %d\n", i, m);
				for (int q=0; q<lines; q++) 				
				{
					if (all_lines[q].from == i) 
						all_lines[q].from = m;
					if (all_lines[q].to == i) 
						all_lines[q].to = m;
				}
			}
		}
	}
	
	// process to break up multi hull sectors
	int old_num_sectors = level_data.num_sectors;
	//num_floors = num_ceilings = 0;
	level_data.num_sectors = BreakupSectors(&FileVertices[0], level_data.num_sectors, level_data.num_vertices, lines);

	level_data.num_lines = lines;
	if (write_data) {
		int retval = 0;
	if((retval = fwrite(&level_data,sizeof(level_data),1,new_fh)) != 1)
	{
		FILE_ERROR(IDS_CONTROL_RECORD_ERR);
		return FALSE;
	}

	offset +=sizeof(level_data);

	}

	for (i=0; i<level_data.num_sectors; i++ ) {
		if (write_data) {
			if( fwrite(&all_fs[i],sizeof(file_sector),1,new_fh) != 1)
			{
				FILE_ERROR(IDS_LOADING_SECTORS_ERR);
				return FALSE;
			}
		}
	}
	

	offset += level_data.num_sectors * sizeof(file_sector);

	// copy across vertices to 3D
	for (i=0; i<level_data.num_vertices; i++) 
	{
		FileVertices3D[i].x = FileVertices[i].x;
		FileVertices3D[i].y = FileVertices[i].y;
	}

	if (write_data) {
		if ((fwrite(&FileVertices[0],sizeof(file_vertex),level_data.num_vertices,new_fh))
			!= (UINT)level_data.num_vertices)
		//if ((fwrite(&FileVertices3D[0],sizeof(file_vertex3d),level_data.num_vertices,new_fh))
			//!= (UINT)level_data.num_vertices)
		{
			FILE_ERROR(IDS_LOADING_SECTORS_ERR);
			return FALSE;
		}
	}
	//offset += level_data.num_vertices * sizeof(file_vertex3d);
	offset += level_data.num_vertices * sizeof(file_vertex);

	for (i=0; i< lines; i++)
	{
// set unused2 to sector height of teleport
// set unused3 to sector of teleport
// unused4 shows the original sector the line was in
		if (all_lines[i].TripFlags & TRIP_TELEPORT) {
			short x = all_lines[i].trip1;
			short y = all_lines[i].trip2;
			int sector_id = FindSector(x, y, DEAD_SECTOR, true);
			all_lines[i].unused2 = Sectors[sector_id]->HtOffset + Sectors[sector_id]->FloorHt(x, y);
			all_lines[i].unused3 = sector_id;
		}	

	}
	
	if (write_data) {
		for (i=0; i< lines; i++) {
			
			if( fwrite(&all_lines[i],sizeof(file_line),1,new_fh) != 1)
			{
				FILE_ERROR(IDS_LOADING_LINES_ERR);
				return FALSE;
			}
		}
	}
	offset += lines * sizeof(file_line);
	level_data.num_sectors = old_num_sectors;

#endif


	// Load the Actors
	actor temp_struct;
// _tprintf("loading actors...\n");

	bool found_recall_point = false;

	for (i=0; i<level_data.num_ornaments; i++)
	{
		if (checksum_fread(&temp_struct, sizeof(struct actor), 1, level_fh) != 1)
		{
			FILE_ERROR(IDS_LOADING_ACTOR_ERR);
			return FALSE;
		}

#ifdef UL3D
		actor3d actor2;
		if (write_data) {
			actor2.angle = temp_struct.angle;
			actor2.finfo = temp_struct.finfo;
			actor2.flags = temp_struct.flags;
			actor2.x = temp_struct.x;
			actor2.y = temp_struct.y;
			int sector_id = FindSector(temp_struct.x, temp_struct.y, DEAD_SECTOR, true);
			actor2.height = Sectors[sector_id]->HtOffset + Sectors[sector_id]->FloorHt(temp_struct.x, temp_struct.y);
							
		if (fwrite(&actor2, sizeof(struct actor3d), 1, new_fh) != 1)
		{
			FILE_ERROR(IDS_LOADING_ACTOR_ERR);
			return FALSE;
		}

		}
		offset += sizeof(struct actor3d);

#endif


		if (temp_struct.flags & ACTOR_ENTRYPOINT)
		{
			if (temp_struct.finfo == options.entrypoint)
				player->PlaceActor((float)temp_struct.x, (float)temp_struct.y, 0, temp_struct.angle, SET_XHEIGHT, true);
		}
		else if (temp_struct.flags & ACTOR_RECALLPOINT)
		{
			found_recall_point = true;
			//player->SetRecall((float)temp_struct.x, (float)temp_struct.y, temp_struct.angle);
		}
		else if ((temp_struct.flags & ACTOR_ITEMGENERATOR) ||
					(temp_struct.flags & ACTOR_AGENTGENERATOR))
		{ 

// show generator points only in gm builds
#if (defined (UL_DEBUG) || defined (GAMEMASTER) || defined (AGENT)) && (!defined LIVE_DEBUG)

			if (temp_struct.flags & ACTOR_ITEMGENERATOR)
			{
				VERIFY_XY(temp_struct.x, temp_struct.y);

				ornament = new cOrnament((float)temp_struct.x, (float)temp_struct.y, 0, temp_struct.angle,
													temp_struct.flags, LyraBitmap::ITEM_GENERATOR);
				ornament->SetData(temp_struct.angle);
			}
			else
			{
				VERIFY_XY(temp_struct.x, temp_struct.y);

				ornament = new cOrnament((float)temp_struct.x, (float)temp_struct.y, 0, temp_struct.angle,
													temp_struct.flags, LyraBitmap::AGENT_GENERATOR);
				int avatar_type = temp_struct.angle;

				if (avatar_type == 1) // fix Level Editor bug which  generated emphants with value 1
					avatar_type = Avatars::EMPHANT;

				avatar_type = MIN(avatar_type,Avatars::MAX_AVATAR_TYPE); // bound avatar_type
#ifdef _DEBUG
				GeneratorsUsed[this->ID()][avatar_type]++;
#endif _DEBUG
				ornament->SetData(avatar_type);
			}
#endif (defined (UL_DEBUG) || defined (GAMEMASTER) || defined (AGENT)) && (!defined LIVE_DEBUG)
		}
		else // generic decorative actor
		{
			VERIFY_XY(temp_struct.x, temp_struct.y);

			ornament = new cOrnament((float)temp_struct.x, (float)temp_struct.y, 0, temp_struct.angle,
												temp_struct.flags, temp_struct.finfo);
			num_ornaments++;
		}
	}

	if (!found_recall_point && (level_id != START_LEVEL_ID))
	{
		FILE_ERROR(IDS_NO_RECALL);
		return FALSE;
	}


	// Load the Rooms

	for (i=0; i<MAX_ROOMS; i++)
		Rooms[i].id = -1; // non-existant room

	room temp_room;
	for (i=0; i<level_data.num_rooms; i++)
	{
		if (checksum_fread(&temp_room, sizeof(room), 1, level_fh) != 1)
		{
			FILE_ERROR(IDS_LOADING_ROOMS_ERR);
			return FALSE;
		}

#ifdef UL3D
		if (write_data) {
		if (fwrite(&temp_room, sizeof(room), 1, new_fh) != 1)
		{
			FILE_ERROR(IDS_LOADING_ROOMS_ERR);
			return FALSE;
		}
		offset += sizeof(room);
		}
#endif


		decrypt(temp_room.name, sizeof (temp_room.name));
		memcpy(&Rooms[temp_room.id], &temp_room, sizeof(temp_room));

#ifndef UNICODE
		// note - do NOT use _t here - editor files are single byte only!
		strcpy(Room_Names[temp_room.id], temp_room.name);

#endif !UNICODE

		//_tprintf("backgrd: %d\n",Rooms[i].background);
		//_tprintf("loaded room %d; id = %d name = %s\n",i,Rooms[i].id, Rooms[i].name);
	}

	//_tprintf("set rooms; about to check sectors\n");

#if (!defined (AGENT) && defined (UL_DEBUG))
	
	this->Verify();

#endif (!defined (AGENT) && defined (UL_DEBUG))

	return(1);

}

void cLevel::Verify()
{
#ifdef UL_DEBUG
	int i,secnum;
	sector *s;
	linedef *l;
	char badsec[MAX_SECTORS];

	//  _tprintf("Checking Sectors...\n");
	for(i=0;i<MAX_SECTORS;i++) badsec[i]=1;

	for(i=0;i<level_data.num_sectors;i++)
	{
		s=Sectors[i];
		if (!s)
		{
		_stprintf(errbuf, _T("Sector %d is NULL!"),i);
			NONFATAL_ERROR(errbuf);
		}
		else if (s->firstline)
			badsec[i]=0;
	}

	for(i=0;i<level_data.num_sectors;i++)
	{
		if (Rooms[Sectors[i]->room].id == -1)
		{
		_stprintf(message, _T("Sector %d references non-existant room %d\n"),Sectors[i]->SecNo, Sectors[i]->room);
			GAME_ERROR(message);
			return;
		}
	}

	for(i=0;i<level_data.num_sectors;i++)
	{
		if ( !badsec[i] )
		{
			s=Sectors[i];
			l=s->firstline;
			if (l) do
			{ // check the lines and see if they point to a bad sector...
				if ( badsec[l->facingsector] )
				{
				_stprintf(errbuf, _T("Line in sector %d points at badsector %d!"),i,l->facingsector);
					NONFATAL_ERROR(errbuf);
					l->facingsector =l->sector;
					l->flags = BOUND | LINE_S_IMPASS;
				}
				// ensure teleporters go to legal areas
				if (l->TripFlags & TRIP_TELEPORT)
				{
					secnum = FindSector(l->trip1,l->trip2, DEAD_SECTOR, true);
					if ((secnum == DEAD_SECTOR) || (Sectors[secnum]->room <= 0))
					{
					_stprintf(errbuf, _T("Bad teleporter to sec %d for line b/tw verts %d,%d at (%.f,%.f) and (%.f,%.f) on level %d!"),
								secnum,	l->from, l->to, Vertices[l->from].x, Vertices[l->from].y,
								Vertices[l->to].x,Vertices[l->to].y, this->ID());
						NONFATAL_ERROR(errbuf);
					}
				}
			} while(l=l->nextline);
		}
	}
#endif
}

// Checks if a sector is 'open' i.e one of its vertices has only one linedef in the sector
bool cLevel::SectorIsOpen(sector *s)
{
	char vertex_found[MAX_VERTICES];
	memset(vertex_found,0, sizeof vertex_found);
	for(linedef *l = s->firstline;l != NULL; l = l->nextline)
	{
		vertex_found[l->from]++;
		vertex_found[l->to]++;
	}
	return memchr(vertex_found,1,sizeof vertex_found) != NULL;
}

#ifdef _DEBUG
// print out which textures are used and unused for levels
// default param, or level = -1, counts for ALL levels
void cLevel::FindUsedTextures(int target_level)
{
	int i; 

	for (i=0; i<MAX_TEXTURES; i++)
		TexturesUsed[i] = 0;
	for (i=0; i<MAX_FLATS; i++)
		FlatsUsed[i] = 0;

	if (target_level != -1)
		this->Load(target_level);
	else
	{
		for (i=0; i<num_levels; i++)
		{
		_stprintf(errbuf, _T("Loading level %d...\n"), level_headers[i].level_id);
			INFO(errbuf);
			this->Load(level_headers[i].level_id);
			actors->Purge();
		}
	}

	for (i=0; i<MAX_TEXTURES; i++)
	{
	_stprintf(errbuf, _T("Texture %d used: %d\n"), i, TexturesUsed[i]);
		INFO(errbuf);
	}
	for (i=0; i<MAX_FLATS; i++)
	{
	_stprintf(errbuf, _T("Flat %d used: %d\n"), i, FlatsUsed[i]);
		INFO(errbuf);
	}

	return;
}

void cLevel::FindAllOpenSectors()
{
	for (int li = 0; li < num_levels; li++)
	{
	_stprintf(errbuf, _T("Loading level %d...\n"), level_headers[li].level_id);
		INFO(errbuf);

		this->Load(level_headers[li].level_id);
		actors->Purge();

		for(int i=0;i<level_data.num_sectors;i++)
		{
			sector  *s=Sectors[i];
			//if (this->SectorIsOpen(s=Sectors[i]))

			char vertex_found[MAX_VERTICES];
			memset(vertex_found,0, sizeof vertex_found);
			for(linedef *l = s->firstline;l != NULL; l = l->nextline)
			{
					vertex_found[l->from]++;
					vertex_found[l->to]++;
			}
			if ( memchr(vertex_found,1,sizeof vertex_found) != NULL)
			{
			_stprintf(errbuf, _T("Sector %d is open. Room %d , level %d  vertices : "),i,s->room,ID());
				INFO(errbuf);

				char *open_pos = vertex_found;
				while ((open_pos = (char *)memchr(open_pos,1,  sizeof vertex_found- (open_pos-vertex_found))) != NULL)
				{
				_stprintf(errbuf, _T("%d "),open_pos - vertex_found );
					INFO(errbuf);
					open_pos++;
				}
			}
		}
	}
}

void cLevel::CheckInterLevelPortals()
{
	struct portal_info
	{
		int vertex_id;
		float src_x,src_y ;
		int level_id ;
		short dest_x,dest_y;
	};

	portal_info portals[MAX_LEVELS][64];
	int counts[MAX_LEVELS];

	// first, load each level and extract out the level portal changes
	for (int li = 0; li < num_levels; li++)
	{
	_stprintf(errbuf, _T("Loading level %d..."), level_headers[li].level_id);
		INFO(errbuf);

		this->Load(level_headers[li].level_id);
		actors->Purge();

		for(int i=0; i<level_data.num_sectors; i++)
		{
			sector *s=Sectors[i];
			linedef *l=s->firstline;
			if (l) do
			{
				if (l->TripFlags & TRIP_LEVELCHANGE)
				{
					int level_id = l->trip4 & 0xff;

					portal_info *pi =  &portals[level_id][counts[level_id]++];
					pi->vertex_id = l->from;
					pi->src_x	  = Vertices[l->from].x;
					pi->src_y	  = Vertices[l->from].y;
					pi->level_id  = this->ID();
					pi->dest_x	  = l->trip1;
					pi->dest_y	  = l->trip2;
				}
			} while (l = l->nextline);
		}
	}

  // now re-load the levels and see if the portal destinations are valid
	for (int level_id = 1; level_id <= num_levels; level_id++)
	{
		if (!counts[level_id])
			continue;

		this->Load(level_id);
		actors->Purge();

	for (int c = 0; c < counts[level_id]; c++)
		{
			portal_info *pi =  &portals[level_id][c];
			int secnum = FindSector(pi->dest_x,pi->dest_y, DEAD_SECTOR, true);
			if ((secnum == DEAD_SECTOR) || (Sectors[secnum]->room <= 0))
			{
			_stprintf(errbuf, _T("Bad Level Teleport from level %d, vertex %d (%f,%f) to level %d, (%d,%d)"),
								pi->level_id, pi->vertex_id, pi->src_x, pi->src_y, level_id, pi->dest_x, pi->dest_y);
				NONFATAL_ERROR(errbuf);
			}
		}
	}

}

void cLevel::CheckInterLevelTeleports()
{
	struct portal_info
	{
		float src_x,src_y ;
		int	src_level_id;
		short dest_x,dest_y;
		int	dest_level_id ;
	};
	portal_info portals[MAX_LEVELS*16];
	int pcount = 0;

	for (int li = 25; li < num_levels; li++)
	{
	_stprintf(errbuf, _T("Loading level %d..."), level_headers[li].level_id);
		INFO(errbuf);

		this->Load(level_headers[li].level_id);
		actors->Purge();

		for(int i=0; i<level_data.num_sectors; i++)
		{
			sector *s=Sectors[i];
			linedef *l=s->firstline;
			if (l) do
			{
				if (l->TripFlags & TRIP_LEVELCHANGE)
				{
					portal_info *pi =  &portals[pcount++];

					pi->src_x			= Vertices[l->from].x;
					pi->src_y			= Vertices[l->from].y;
					pi->src_level_id	= this->ID();

					pi->dest_x			= l->trip1;
					pi->dest_y			= l->trip2;
					pi->dest_level_id = (l->trip4 & 0xff);
				}
			} while (l = l->nextline);
		}
	}

	for(portal_info *pi =  portals; pcount > 0 ; pi++, pcount-- )
	{
	_stprintf(errbuf, _T("Teleporting back to source level %d at (%f,%f)"),
				pi->src_level_id,pi->src_x, pi->src_y );
		INFO(errbuf);

		player->Teleport( pi->src_x, pi->src_y, 0/*angle*/, pi->src_level_id);
		actors->Purge();

	_stprintf(errbuf, _T("Teleporting to teleport destination level %d at (%d,%d)"),
				pi->dest_level_id, pi->dest_x, pi->dest_y );
		INFO(errbuf);

		player->Teleport( pi->dest_x, pi->dest_y, 0/*angle*/, pi->dest_level_id);
		actors->Purge();
	}
}


void cLevel::DumpMonsterGens()
{
	int num_gens = 0;

	for (int i=0; i < MAX_LEVELS; i++)
		for (int j=0; j < 7; j++)
			GeneratorsUsed[i][j] = 0;

	FILE* fh =_tfopen(_T("monster_gens.txt"), _T("w"));

	// first, load each level and extract out the level portal changes
	for (int li = 0; li < num_levels; li++)
	{
		num_gens = 0;
		this->Load(level_headers[li].level_id);
		actors->Purge();
		// delete actors	that don't survive level changes
	_ftprintf(fh, _T("%u : %u : %u : %u : %u : %u\n"), li, GeneratorsUsed[li][2], 
			GeneratorsUsed[li][3], GeneratorsUsed[li][4], 
			GeneratorsUsed[li][5], GeneratorsUsed[li][6]);
	}
	fclose(fh);
}

#endif

#ifdef UL3D
bool verbose = false;

int cLevel::BreakupSectors(file_vertex* verts, int num_sectors, int num_verts, int num_lines)
{
	// go through and break up mult-hull sectors
	int next_free_sector = num_sectors;
	int sector_index;
	int line_num;
	int hull_num;
	int floor_base;
	int ceiling_base;
	bool floor_facing;
	bool ceiling_facing;

	int num_sector_lines[MAX_SECTORS];



	for (sector_index=0; sector_index < MAX_SECTORS; sector_index++) {
		all_fs[sector_index].tag = -1;
		int sector_num = all_fs[sector_index].id;
		// count lines
		num_sector_lines[sector_index] = 0;
		for (line_num=0; line_num < num_lines; line_num++) {
			if ((all_lines[line_num].sector == sector_num) ||
				(all_lines[line_num].facingsector == sector_num)) {
				num_sector_lines[sector_index]++;
			}
		}
	}

	for (sector_index=0; sector_index < num_sectors; sector_index++)
	{	// first, classify all the lines in this sector into different hulls by
		// looking for shared vertices
		int sector_num = all_fs[sector_index].id;

		if (all_fs[sector_index].tag != -1)
			continue;

		printf("Processing sector %d in level %d\n", sector_num, level_id);

#if 0
		bool bounding = true;

		for (unsigned int i = 0; i < num_lines; i++) 
		{
			int sec = all_lines[i].sector;
			int face = all_lines[i].facingsector;
			if ((sec == sector_num) ||
				(face == sector_num)) {
				if (!all_lines[i].flags & BOUND) {
					bounding = false;
					break;
				}
			}
		}
 
		if (bounding)
			continue; // don't worry about splitting up bounding sectors
#endif

		int num_hulls = 0;
		// init hull data
		for (hull_num=0; hull_num<128; hull_num++) {
			hulls[hull_num].num_lines = 0;
			for (int i=0; i<512; i++) {
				hulls[hull_num].facing[i] = false;
				hulls[hull_num].assigned[i] = false;
			}
		}

		for (line_num=0; line_num < num_lines; line_num++) 
		{
			all_lines[line_num].unused4 = all_lines[line_num].sector;
			all_lines[line_num].unused5 = all_lines[line_num].facingsector;
			if ((level_id == 41) && (line_num == 1975)) {
				all_lines[line_num].facingsector = 490;
			}
			if ((level_id == 41) && (line_num == 1994)) {
				all_lines[line_num].facingsector = 490;
			}
			if ((level_id == 41) && (line_num == 1995)) {
				all_lines[line_num].facingsector = 490;
			}
		}


		for (line_num=0; line_num < 10000; line_num++) 
			line_assigned[line_num] = false;

		if (all_fs[sector_index].flags & SECTOR_SKY) {
			all_fs[sector_index].xanim_top = 0;
			all_fs[sector_index].yanim_top = 0;
		}


		floor_base = -1;	
		ceiling_base = -1;

		if (all_fs[sector_index].floor_slope_angle != 0) 
		{
			for (line_num=0; line_num < num_lines; line_num++)
				if ((all_lines[line_num].sector == sector_num) &&
					(all_lines[line_num].flags & LINE_SECTOR_FLOOR_BASE)) {
					floor_base = line_num;
					floor_facing = false;
					break;
				}
				if 	((all_lines[line_num].facingsector == sector_num) &&
					 (all_lines[line_num].flags & LINE_FACING_FLOOR_BASE))	{
					floor_base = line_num;
					floor_facing = true;
					break;
				}
		}

		if (all_fs[sector_index].ceiling_slope_angle != 0) 
		{
			for (line_num=0; line_num < num_lines; line_num++)
				if ((all_lines[line_num].sector == sector_num) &&
					(all_lines[line_num].flags & LINE_SECTOR_CEILING_BASE)) {
					ceiling_base = line_num;
					ceiling_facing = false;
					break;

				}
				if  ((all_lines[line_num].facingsector == sector_num) &&
					(all_lines[line_num].flags & LINE_FACING_CEILING_BASE)) {
					ceiling_base = line_num;
					ceiling_facing = true;
					break;
				}
		}

		if (sector_num == 644) {
			int qqq =0;
		}

		for (line_num=0; line_num < num_lines; line_num++)
		{	// find all the lines in this sector; we only need to assign each line
			// to a hull once
			if (((all_lines[line_num].sector == sector_num) ||
				 (all_lines[line_num].facingsector == sector_num)) &&
				!line_assigned[line_num])
			{	// it's a line in this sector! find the hull...
				CreateHull(sector_num, line_num, num_hulls, num_lines);
				num_hulls++;
			}
		}

		// at this point we have split the sector into hulls
		// let's calculate the z heights of all the vertices now...
/*
		for (hull_num=0; hull_num < num_hulls; hull_num++) {
				for (line_num=0; line_num < hulls[hull_num].num_lines; line_num++)
				{	
					int l = hulls[hull_num].lines[line_num];
					int v = all_lines[l].from;
					float x = FileVertices[v].x;
					float y = FileVertices[v].y;
					FileVertices3D[v].floor = Sectors[sector_num]->FloorHt(x,y);
					FileVertices3D[v].ceil = Sectors[sector_num]->CeilHt(x,y);
					v = all_lines[l].to;
					x = FileVertices[v].x;
					y = FileVertices[v].y;
					FileVertices3D[v].floor = Sectors[sector_num]->FloorHt(x,y);
					FileVertices3D[v].ceil = Sectors[sector_num]->CeilHt(x,y);

				}

		}
		*/

		short vertcount[MAX_VERTICES];
		int num_dupverts = 0;
		bool recurse = true;
		while (recurse) {
			recurse = false;
			for (hull_num=0; hull_num < num_hulls; hull_num++) {
				if (recurse)
					break;
				// now look for shared points in the hull we have
				memset(&vertcount[0], 0, MAX_VERTICES*sizeof(short));
				num_dupverts = 0;
				for (line_num=0; line_num < hulls[hull_num].num_lines; line_num++)
				{	// find all the lines in this sector; we only need to assign each line
					int l = hulls[hull_num].lines[line_num];
					int v0 = all_lines[l].to;
					int v1 = all_lines[l].from;
					vertcount[v0]++;
					vertcount[v1]++;
				}
				for (int s=0; s<MAX_VERTICES;s++) {
					if (vertcount[s] == 3) {
						//DebugOut("Vertex %d has ref count 3 in sector %d\n", s, sector_num);
					}
					if (vertcount[s] >= 3) {
						if (vertcount[s] > 3) { // don't split sectors if there's just 3 line shared verts {
							recurse = true;
							//DebugOut("Sector %d, hull %d needs to be split! Vert %d has ref count %d\n",
							//	sector_num, hull_num, s, vertcount[s]);
						}
						dupverts[num_dupverts] = s;
						num_dupverts++;
						// now we need to split up this sector so that we have multiple enclosed
						// contours that share just this point
					}
				}
				if (recurse) {
					//if ((sector_num == 237) && (level_id == 26)) {
					//	verbose = true;
					//	int qqq = 0;
					//}
					
					int old_num_hulls = num_hulls;
					int dupvert = -1;
					for (s=0; s<num_dupverts; s++) {
						if (vertcount[dupverts[s]] > 3) {
							dupvert = dupverts[s];
							break;
						}
					}

					if (s == num_dupverts)
						continue;
 
					num_hulls = this->SplitHull(dupvert, hull_num, num_hulls, num_dupverts);
					if (old_num_hulls == num_hulls)
						recurse = false;
					//if (recurse)
						//DebugOut("Split sector %d!\n", sector_num);
				}
				if (recurse)
					break;
			}
		}

		// at this point we've created all the separate hull for this sector
		//if (num_hulls == 0)
		//{
			//FILE_ERROR(IDS_SECTORS_ERR);
			//return -1;
		//}

		if (sector_num == 491) {
			int max_lines = 0;
			int hull = -1;
			for (int qq=0; qq<num_hulls; qq++)  {
				if (hulls[qq].num_lines > max_lines) {
					max_lines = hulls[qq].num_lines;
					hull = qq;
				}
				for (int zz=0; zz<hulls[qq].num_lines; zz++)
					if (hulls[qq].lines[zz] == 1980) {
						int qb = 0;
					}
			}
			int ff=0;
		}

		if (num_hulls > 1)
		{	// we need to split this sector into multiple hulls!
			if (next_free_sector == MAX_SECTORS)
			{
				FILE_ERROR(IDS_SECTORS_ERR);
				return -1;
			}

			
			hulls[0].sector_index = sector_index;
			hulls[0].sector_id = sector_num;

			int check_sector_lines[10000];

			for (hull_num=1; hull_num<num_hulls; hull_num++)
			{
				int duplicate = -1; // set to sector # if we duplicated a sector

				bool stop = false;
				if ((sector_num == 491) && (hull_num == 17)) {
					stop = true;
				}

				memcpy(&all_fs[next_free_sector], &all_fs[sector_num], sizeof(file_sector));
				all_fs[next_free_sector].id = next_free_sector;
				hulls[hull_num].sector_index = next_free_sector;
				hulls[hull_num].sector_id = next_free_sector;

				// set sector or facing sector properly for this line
				for (int i=0; i<hulls[hull_num].num_lines; i++)
				{
					int hull_line = hulls[hull_num].lines[i];
					if (all_lines[hull_line].sector == sector_num) {
						//if (all_lines[hull_line].sector < all_lines[hull_line].facingsector)
						all_lines[hull_line].sector = next_free_sector;
						//printf("SECTOR MATCH sector: %d facing: %d\n", all_lines[hull_line].sector, all_lines[hull_line].facingsector);

					}
					if (all_lines[hull_line].facingsector == sector_num) {
						//if (all_lines[hull_line].facingsector < all_lines[hull_line].sector)
						all_lines[hull_line].facingsector = next_free_sector;
						//printf("FACING MATCH sector: %d facing: %d\n", all_lines[hull_line].sector, all_lines[hull_line].facingsector);
					}
				}

									// set tags appropriately for child sectors 
				//for (int i=0; i<num_sectors; i++) {

				//}



				next_free_sector++;
				//printf("NFS: %d\n", next_free_sector);
				if ((level_id == 1) && (next_free_sector == 582)) {
					int debughere=0;
				}


			}

			num_hulls = CombineHulls(num_hulls, verts, sector_num, level_data.num_lines);

			if (sector_num == 29) {
				int qbv = 0;
			}

			// now fix up the base lines for sloping sectors
			if (num_hulls > 1) {
				if ((all_fs[sector_index].ceiling_slope_angle != 0) && (ceiling_base > -1))
					FixBaseLines(num_hulls, verts, sector_num, false, ceiling_base, ceiling_facing); // for sloping ceilings

				if ((all_fs[sector_index].floor_slope_angle != 0) && (floor_base > -1))
					FixBaseLines(num_hulls, verts, sector_num, true, floor_base, floor_facing); // for sloping ceilings
			}


		}
	}
	return next_free_sector;

}

struct bbox_t {
	float minx;
	float miny;
	float maxx;
	float maxy;
};

int cLevel::CombineHulls(int num_hulls, file_vertex* verts, int sector_num, int num_lines) 
{
	int i,t;
	int hull_line, v0, v1;
	int hull_count = num_hulls;
	// i is the outer looping variable
	//bool recurse = true;
		
	if ((sector_num == 3) && (level_id == 1)) {
			int qqq = 0;
	}



	//while (recurse) {
	//	recurse = false;
	for (i=0; i < hull_count; i++)
	{
		// create a bounding box for hull #i
		bbox_t bbox;
		bbox.maxx = bbox.maxy =  -9999999.0f;
		bbox.minx = bbox.miny = 9999999.0f;
		int sec_i_index = hulls[i].sector_index;
		int sec_i_id = hulls[i].sector_id;

		for (int j=0; j<hulls[i].num_lines; j++)
		{
			hull_line = hulls[i].lines[j];
			v0 = all_lines[hull_line].from;
			v1 = all_lines[hull_line].to;
			if (verts[v0].x < bbox.minx)
				bbox.minx = verts[v0].x;
			if (verts[v0].y < bbox.miny)
				bbox.miny = verts[v0].y;
			if (verts[v1].x < bbox.minx)
				bbox.minx = verts[v1].x;
			if (verts[v1].y < bbox.miny)
				bbox.miny = verts[v1].y;
			
			if (verts[v0].x > bbox.maxx)
				bbox.maxx = verts[v0].x;
			if (verts[v0].y > bbox.maxy)
				bbox.maxy = verts[v0].y;
			if (verts[v1].x > bbox.maxx)
				bbox.maxx = verts[v1].x;
			if (verts[v1].y > bbox.maxy)
				bbox.maxy = verts[v1].y;
		}
		
		for (j=0; j< hull_count; j++)
		{
			if (j == i)
				continue;
			int sec_j_index = hulls[j].sector_index;
			int sec_j_id = hulls[j].sector_id;
			
			// check to see if j is entirely inside i
			bool inside = true;
			for (int s=0; s<hulls[j].num_lines; s++)
			{
				hull_line = hulls[j].lines[s];
				v0 = all_lines[hull_line].from;
				v1 = all_lines[hull_line].to;
				if ((verts[v0].x > bbox.maxx) ||
					(verts[v0].y > bbox.maxy) ||
					(verts[v0].x < bbox.minx) ||
					(verts[v0].y < bbox.miny) ||
					(verts[v1].x > bbox.maxx) ||
					(verts[v1].y > bbox.maxy) ||
					(verts[v1].x < bbox.minx) ||
					(verts[v1].y < bbox.miny))
				{
					inside=false;
					break;
				}
			}
			if (inside) {

				all_fs[sec_j_index].tag = sec_i_id;
				//printf("Found hull %d inside hull %d in sector %d, level %d!\n",j,i,sector_num,level_id);
				//recurse=true;
				
				//for (t=0; t<hulls[j].num_lines; t++)
				//{ // combine together hulls i,j
				//hulls[i].lines[hulls[i].num_lines] = hulls[j].lines[t];
				//hulls[i].num_lines++;	
				//}
				//hull_count--;
				//if (j != hull_count) { // copy across top hull to one we got rid of
				//memcpy(&hulls[j], &hulls[hull_count], sizeof(hull_t));
				//j--; // don't skip the one we just copied in!
				//}
			}
		}
	}
	//}
	//if (hull_count < num_hulls)
	//	printf("Reduced hull count %d to %d\n", num_hulls, hull_count);
	return hull_count;
}

int cLevel::CreateHull(int sector_num, int line_num, int hull_num, int num_lines)
{
	// add starting line to new hull
	hulls[hull_num].lines[hulls[hull_num].num_lines] = line_num;

	if (all_lines[line_num].facingsector == sector_num)
		hulls[hull_num].facing[hulls[hull_num].num_lines] = 1;
	else
		hulls[hull_num].facing[hulls[hull_num].num_lines] = 0;

	hulls[hull_num].num_lines++;
	line_assigned[line_num] = true;

	if (sector_num == 1) {
		int qsdf = 0;
	}
	
	// now, loop through all the linedefs and add any to this hull that share
	// any vertices with the linedefs already in the hull. we need to keep
	// iterating until we add nothing new

	int i,j;
	bool done = false;
	while (!done) {
		int lines_added = 0;
		for (j=0; j< num_lines; j++)
		{
			if (((all_lines[j].sector == sector_num) ||
				 (all_lines[j].facingsector == sector_num))
				&&	!line_assigned[j]) // it's a unassigned line in this sector
			{
				if ((j == 222) || (j == -1)) {
					int asdf  = 0;
				}
				for (i=0; i < hulls[hull_num].num_lines; i++)
				{	// check to see if this line shares any vertices
					int hull_line = hulls[hull_num].lines[i];
					bool do_split = true;
					//if ((level_id == 1) && (sector_num == 303))
						//int qqq=0;
					//elsedo_split = false;


					if ((all_lines[hull_line].from == all_lines[j].from) ||
						(all_lines[hull_line].from == all_lines[j].to) || 
						(all_lines[hull_line].to == all_lines[j].from) || 
						(all_lines[hull_line].to == all_lines[j].to) ||
						!do_split)
					{
						hulls[hull_num].lines[hulls[hull_num].num_lines] = j;
						hulls[hull_num].num_lines++;
						line_assigned[j] = true;
						lines_added++;
						break;
					}
				}
			}

		}

		if (lines_added == 0)
			done = true;

	}
	return hulls[hull_num].num_lines;
}

// for the hull base_hull, compare the z coords of each vertex not on the base line,
// and return the # of verts with z values 

static int VertAssigned[MAX_VERTICES];

int cLevel::CompareZValues(int v0, int v1, int base_z, int hull_num, bool floor)
{
	int above = 0;
	memset(&VertAssigned[0], 0, sizeof(int)*MAX_VERTICES);
	for (int j=0; j< hulls[hull_num].num_lines; j++)
	{
		int l = hulls[hull_num].lines[j];
		int v3 = all_lines[l].from;
		int v4 = all_lines[l].from;
		int sector_num = hulls[0].sector_id;
		if ((v3 != v0) && (v3 != v1) && !VertAssigned[v3]) 
		{
			int x0 = FileVertices[v3].x;
			int y0 = FileVertices[v3].y;
			int vert_z;
			if (floor) 
				vert_z = Sectors[sector_num]->FloorHt(x0,y0);
			else
				vert_z = Sectors[sector_num]->CeilHt(x0,y0);

			if (vert_z > base_z) {
				above++;
				VertAssigned[v3]=1;
			}

		}
		if ((v4 != v0) && (v4 != v1) && !VertAssigned[v4]) 
		{
			int x0 = FileVertices[v4].x;
			int y0 = FileVertices[v4].y;
			int vert_z;
			if (floor) 
				vert_z = Sectors[sector_num]->FloorHt(x0,y0);
			else
				vert_z = Sectors[sector_num]->CeilHt(x0,y0);

			if (vert_z > base_z) {
				above++;
				VertAssigned[v4]=1;
			}
		}
	}
	
	return above;
}


int cLevel::FixBaseLines(int num_hulls, file_vertex* verts, int sector_num, bool floor, int line, bool facing_flag)
{	
	int flag;
	int i,j;
	int base_hull=-1;
	bool facing = false;
	for (i=0; i< num_hulls; i++) {
		for (j=0; j< hulls[i].num_lines; j++)
			if (hulls[i].lines[j] == line) {
				base_hull = i;
				facing = hulls[i].facing[j];
				break;
			}
	}
	if (base_hull == -1)
	{
		FILE_ERROR(IDS_SECTORS_ERR);
	}

	int v0,v1;
	if (facing) {
		v0 = all_lines[line].to;
		v1 = all_lines[line].from;
	}
	else {
		v0 = all_lines[line].from;
		v1 = all_lines[line].to;
	}

	float x1,x0,y1,y0;
	
	x0 = verts[v0].x;
	x1 = verts[v1].x;
	y0 = verts[v0].y;
	y1 = verts[v1].y;

	// determine if base line is at "top" or "bottom"
	int base_z = 0;
	if (floor) 
		base_z = Sectors[sector_num]->FloorHt(x0,y0);
	else
		base_z = Sectors[sector_num]->CeilHt(x0,y0);

	int base_above = CompareZValues(v0, v1, base_z, base_hull, floor);


	//float length =  fdist(x0,y0,x1,y1);

	// set the sine and cosine of the line
	//float basecos	= (x1 - x0)/length;
	//float basesine	= (y1 - y0)/length;
	//float cos = 0.0f;
	//float sine = 0.0f;
	float xdiff = x1-x0;
	float ydiff = y1-y0;
	bool noy = false;
	float baseangle;
	if ((abs(y1 - y0)) < 0.001f) {
		noy = true;
		baseangle = 999999999.0f;
	} else
		baseangle = xdiff/ydiff;

	for (i=0; i< num_hulls; i++) {
		if (i == base_hull) // this hull has the old baseline in it
			continue;
		int sector_index = hulls[i].sector_index;
		int sector_id = all_fs[sector_index].id;

		float min_diff = 999999999.0f;		
		int k=0;
		// now we need to pick the linedef that's closest in orientation
		for (j=0; j<hulls[i].num_lines; j++)
		{
			int v4,v5;
			int thisline = hulls[i].lines[j];
			if (facing) {
				v4 = all_lines[thisline].to;
				v5 = all_lines[thisline].from;
			}
			else {
				v4 = all_lines[thisline].from;
				v5 = all_lines[thisline].to;
			}
			
			float x1,x0,y1,y0;
			
			x0 = verts[v4].x;
			x1 = verts[v5].x;
			y0 = verts[v4].y;
			y1 = verts[v5].y;
			
			//float length =  fdist(x0,y0,x1,y1);
			
			// set the sine and cosine of the line
			//float lcos	= (x1 - x0)/length;
			//float lsine	= (y1 - y0)/length;

			//float diffcos = lcos - basecos;
			//diffcos = diffcos*diffcos;
			//float diffsine = lsine - basesine;
			//diffsine = diffsine*diffsine;

			//float diff = diffcos + diffsine;

			float xdiff = x1-x0;
			float ydiff = y1-y0;
			float thisangle = 0.0f;
			if (noy) {
				if ((abs(y1 - y0)) < 0.001f)
					thisangle = 999999999.0f;
				else
					thisangle = xdiff/ydiff;
			} else {
				if ((abs(y1 - y0)) < 0.001f)
					continue;
				thisangle = xdiff/ydiff;
			}


			float diff = abs(thisangle - baseangle);

			if (diff <= min_diff) {
				k = hulls[i].lines[j];
				min_diff = diff;
			}
			
		}

		//int k = hulls[i].lines[0];
		int v2;		
		if (hulls[i].facing[k]) {
			v2 = all_lines[k].to;
		} else {
			v2 = all_lines[k].from;
		}

		float x2,y2;
		
		x2 = verts[v2].x;
		y2 = verts[v2].y;

		if (hulls[i].sector_id == 582) {
			int qbw = 0;
		}

		int newbase_z = 0;
		if (floor) 
			newbase_z = Sectors[sector_num]->FloorHt(x2,y2);
		else
			newbase_z = Sectors[sector_num]->CeilHt(x2,y2);


		int newbase_above = CompareZValues(all_lines[k].to, all_lines[k].from, newbase_z, i, floor);

		bool invert_slope = false;
		if ((newbase_above <= hulls[i].num_lines/2) &&
			(base_above >= hulls[base_hull].num_lines/2))
			invert_slope = true;
		if ((newbase_above >= hulls[i].num_lines/2) &&
			(base_above <= hulls[base_hull].num_lines/2))
			invert_slope = true;

		//printf("Level: %d Sector: %d Base Sector: %d Old z: %d New z: %d Old Above: %d/%d New Above: %d/%d Old Base Line: %d New Base Line: %d Invert: %d\n",
			//level_id, sector_id, hulls[0].sector_id, base_z, newbase_z, base_above,hulls[base_hull].num_lines,  newbase_above,  hulls[i].num_lines, line, k, invert_slope);

		if (invert_slope) 
		{
			if (floor)
			{
				all_fs[sector_index].floor_slope_angle = -all_fs[sector_index].floor_slope_angle;
				all_fs[sector_index].floor_slope_frac = -all_fs[sector_index].floor_slope_frac;
			} 
			else
			{
				all_fs[sector_index].ceiling_slope_angle = -all_fs[sector_index].ceiling_slope_angle;
				all_fs[sector_index].ceiling_slope_frac = -all_fs[sector_index].ceiling_slope_frac;
			}
		} 

		if (floor) {
			
			all_fs[sector_index].floor_height = Sectors[sector_num]->FloorHt(x2,y2);

			if (hulls[i].facing[k])
				all_lines[k].flags  |= LINE_FACING_FLOOR_BASE;
			else
				all_lines[k].flags  |= LINE_SECTOR_FLOOR_BASE;

			//num_floors++;
		}
		else {
			all_fs[sector_index].ceiling_height = Sectors[sector_num]->CeilHt(x2,y2);
			if (hulls[i].facing[k])
				all_lines[k].flags  |= LINE_FACING_CEILING_BASE;
			else
				all_lines[k].flags  |= LINE_SECTOR_CEILING_BASE;

//			num_ceilings++;
		}
	}

	return 0;
}




int cLevel::SplitHull(int shared_vert, int hull_num, int num_hulls, int num_dupverts)
{	// first, find a line that has this vertex as an anchor
	// index 0 = start line
	// index 1-3 = other lines that share the same vertex

	//if (verbose)
		//DebugOut("Splitting hull for sector 910\n");
	int vert_lines[8];
	int num_vert_lines = 0;
	for (int s=0; s<hulls[hull_num].num_lines; s++) {
		int l = hulls[hull_num].lines[s];
		if ((all_lines[l].from == shared_vert) ||
			(all_lines[l].to == shared_vert)) {
			vert_lines[num_vert_lines] = l;
			num_vert_lines++;
		}
	}

	if (num_vert_lines != 4) {
		return num_hulls;
	}

		if ((hulls[hull_num].sector_id  == 910) && (level_id == 4)) {
			int qqq = 0;
		}


	hull_t test_hulls[16];
	memset(&test_hulls[0], 0, sizeof(hull_t)*12);
	for (int t=0; t<16; t++) {
		test_hulls[t].num_lines = 99999;
	}

	// now we wind around the sector starting with the start line until we come back
	// to where we started. we compare the # of lines in each winding, and pick the one
	// with the fewest.

	for (t=0; t<4; t++) {
		// loop through and try each line as a starting line
		for (s=0; s<4; s++) {
			if (s == t)
				continue;
			if (verbose) {
				_tprintf("Making hull piece %d Splitting hull for sector 910\n", s);
			}

			MakeHullPiece(hulls[hull_num], test_hulls[t*4+s], vert_lines[t], vert_lines[s], 
				shared_vert, num_dupverts);
		}
	}

	int best_hull = -1;
	int min_distance = 99999999.0f;
	for (s=0; s<16; s++) {
		if ((test_hulls[s].num_lines > 1000) ||
			(test_hulls[s].num_lines < 3))
		{
			test_hulls[s].distance = 99999999.0f;
			continue;
		}
		test_hulls[s].distance = 0.0f;
		for (int t=0; t<test_hulls[s].num_lines; t++) {
			int l = test_hulls[s].lines[t];
			int v0 = all_lines[l].from;
			int v1 = all_lines[l].to;
			float xdiff = FileVertices[v0].x - FileVertices[v1].x;
			float ydiff = FileVertices[v0].y - FileVertices[v1].y;
			test_hulls[s].distance += xdiff*xdiff + ydiff*ydiff;
		}

		if (test_hulls[s].distance < min_distance) {
			best_hull = s;
			min_distance = test_hulls[s].distance;
		}
	}
	
	// we have now picked the best hull of the bunch; split it out from the rest
	memcpy(&hulls[num_hulls], &test_hulls[best_hull], sizeof(hull_t));
	// remove every line in the new hull from the original
	for (s=0; s<hulls[num_hulls].num_lines; s++) {
		int l = hulls[num_hulls].lines[s];
		for (int t=0; t<hulls[hull_num].num_lines; t++) {
			int q = hulls[hull_num].lines[t];
			if (q == l) { // replace this with the last line in the hull
				hulls[hull_num].num_lines--;
				hulls[hull_num].lines[t] = hulls[hull_num].lines[hulls[hull_num].num_lines];
			}
		}
	}

	return num_hulls+1;
}

void cLevel::MakeHullPiece(hull_t& old_hull, hull_t& new_hull, int start_line, int follow_line, 
						   int shared_vert, int num_dupverts)
{
	// start with start_line; go around and add lines until we get back to the start
	for (int t=0; t<512; t++) {
		old_hull.assigned[t] = false;
		if (old_hull.lines[t] == start_line)
			old_hull.assigned[t] = true;
		if (old_hull.lines[t] == follow_line)
			old_hull.assigned[t] = true;
	}
	new_hull.num_lines = 2;
	new_hull.lines[0] = start_line;
	new_hull.lines[1] = follow_line;

	int next_vert = -1;
	int end_vert = -1;

	if (all_lines[start_line].from == shared_vert)
		end_vert = all_lines[start_line].to;
	else
		end_vert = all_lines[start_line].from;


	if (all_lines[follow_line].from == shared_vert)
		next_vert = all_lines[follow_line].to;
	else
		next_vert = all_lines[follow_line].from;

	if ((old_hull.sector_id == 910) && (level_id == 4)) {
		int qzf = 0;
	}

	if (verbose) {
		_tprintf("Top level add lines to hull; next vert = %d, end vert = %d\n", next_vert, end_vert);
	}


	bool result = AddLinesToHull(old_hull, new_hull, next_vert, end_vert, num_dupverts);
	if (!result)
		new_hull.num_lines = 99999;

	return;
}

// returns true if we get back to the start vert, false otherwise
bool cLevel::AddLinesToHull(hull_t& old_hull, hull_t& new_hull, int start_vert, 
							int end_vert, int num_dupverts) //, file_vertex* verts)
{
	hull_t local_old_hull;
	hull_t local_new_hull[8];
	int num_tries = 0;
	// copy the old hull so we can mark the lines assigned
	memcpy(&local_old_hull, &old_hull, sizeof(hull_t));
	int next_vert = start_vert;
	int t=0;
	if (verbose) {
		_tprintf("Adding lines to hull; start vert = %d, end vert = %d\n", start_vert, end_vert);
	}


	bool stuck = false;
	while (next_vert != end_vert) {
		if (stuck) {
			if (num_tries == 0)
				return false;
			else
				break;
		}
		stuck = true;
		for (t=0; t<local_old_hull.num_lines; t++) {
			if (local_old_hull.assigned[t])
				continue;
			int l = local_old_hull.lines[t];
			bool add = false;
			bool to = false;
			if (all_lines[l].from == next_vert) 
				add = true;
			if (all_lines[l].to == next_vert) {
				add = true;
				to = true;
			}
			int last_next_vert = next_vert;
			if (add) {
				local_old_hull.assigned[t] = true;
				bool ignore = false;
				int temp_vert = -1;
				if (to)
					temp_vert = all_lines[l].from;
				else
					temp_vert = all_lines[l].to;

				for (int s=0; s<new_hull.num_lines; s++) {
					int l = new_hull.lines[s];
					if (temp_vert == end_vert)
						continue;
					if ((all_lines[l].from == temp_vert) ||
						(all_lines[l].to == temp_vert)) {
						ignore = true;
						if (verbose) {
							_tprintf("Ignoring line %d due to loop\n", l);
						}
						break;
					}	
				}
				if (ignore)
					continue;

				next_vert = temp_vert;

				bool dup = false;
				for (int q=0; q<num_dupverts; q++) {
					if (last_next_vert == dupverts[q])
						dup = true;
				}

				// if it's a shared vertex, we need to split & try twice
				new_hull.lines[new_hull.num_lines] = l;
				new_hull.num_lines++;
				if (verbose) {
					_tprintf("Adding line %d; dup = %d\n", l, dup);
				}

				if (!dup) {
					stuck = false;
					break;
				}

				memcpy(&local_new_hull[num_tries], &new_hull, sizeof(hull_t));

				bool result = true;
				int old_num_lines = new_hull.num_lines;
				if (next_vert != end_vert) {
					// past here we're dealing with a duplicate
					if (verbose) {
						_tprintf("Making recursive call for dup vertex %d\n", next_vert);
					}

					result = AddLinesToHull(local_old_hull, local_new_hull[num_tries], next_vert, end_vert, 
						num_dupverts);
				}
				if (verbose) {
					_tprintf("Returned from recursive call for dup vertex %d; result = %d\n", next_vert, result);
				}
				new_hull.num_lines--; // back up & undo so we can try all possibilities
				next_vert = last_next_vert;

				if (result) {
					num_tries++;	
					stuck = false;
				}

			}
		}
		if ((t == old_hull.num_lines) && (num_tries == 0)) {
			if (verbose) {
				_tprintf("Returning false from add lines\n");
			}

			return false; // can't continue this way
		}
	}
	if (num_tries > 0) {
		// pick the best one...
		if (verbose) {
			_tprintf("Choice of %d hulls...\n", num_tries);
		}
		
		int best_hull = -1;
		int min_distance = 9999999999.0f;
		for (int s=0; s<num_tries; s++) {
			if (local_new_hull[s].num_lines > 1000) {
				local_new_hull[s].distance = 99999999.0f;
				continue;
			}
			local_new_hull[s].distance = 0.0f;
			for (int t=0; t<local_new_hull[s].num_lines; t++) {
				int l = local_new_hull[s].lines[t];
				int v0 = all_lines[l].from;
				int v1 = all_lines[l].to;
				float xdiff = FileVertices[v0].x - FileVertices[v1].x;
				float ydiff = FileVertices[0].y - FileVertices[v1].y;
				local_new_hull[s].distance += sqrt(xdiff*xdiff + ydiff*ydiff);
			}
			
			if (local_new_hull[s].distance < min_distance) {
				best_hull = s;
				min_distance = local_new_hull[s].distance;
			}
		}
		
		memcpy(&new_hull, &local_new_hull[best_hull], sizeof(hull_t));
	}

	if (verbose) {
		_tprintf("Returning true from add lines!\n", num_tries);
	}

	return true;
}



#endif

