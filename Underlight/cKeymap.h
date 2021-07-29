// Header file for cKeymap

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CKEYMAP_H
#define CKEYMAP_H

#include "cArts.h"
#include "resource.h"


//////////////////////////////////////////////////////////////////
// Constants

const int NUMBER_MAPPABLE_FUNCTIONS = 70; // update with LyraKeyboard
struct LyraKeyboard {
	enum { 
		FOCAL_FLAME = 0,
		FOCAL_BLADE,
		RESERVED_11,
		TOGGLE_NAMES,
		DROP_ITEM,
		EMOTE,
		OPEN_GOAL_BOOK,
		LEAVE_PARTY,
		SELECT_NEXT,
		RESET_EYELEVEL,
		TOGGLE_SOUND,
		TALK,
		WHO_NEARBY,
		ACTIVE_EFFECTS, 
		SHOW_XP,
		LOOK_UP,
		LOOK_DOWN,
		MOVE_FORWARD,
		MOVE_BACKWARD,
		TURN_LEFT,
		TURN_RIGHT,
		RUN,
		JUMP,
		USE,
		TRIP,
		STRAFE,
		SIDESTEP_LEFT,
		SIDESTEP_RIGHT,
		MOUSE_LOOK,
		SCROLL_UP,
		SCROLL_DOWN,
		PICK_INVENTORY,
		PICK_NEIGHBORS,
		PICK_ARTS,
		SELECT_FROM_LIST,
		AVATAR_CUSTOMIZATION,
		SELECT_PREV,  
		SHOW_NEXT,  // scroll listview down by 1 
		SHOW_PREV,   // scroll listview up by 1
		SHOW_RANKS, 
		SHOW_FOCUS, 
		WAVE,
		SHOW_SHIELD,
		IGNORE_LIST,
		SHOW_LEARNABLE_ARTS,
		TOGGLE_AUTORUN,
        TOGGLE_ADULT_FILTER,
		TIME_ONLINE,
		INSPECT,
		DESELECT,
		SHOW_DST_TIME,
		TOGGLE_MUSIC,
		SHOW_PARTY,
		RESERVED_12,
		RESERVED_13,
		RESERVED_14,
		RESERVED_15,
		RESERVED_16,
		RESERVED_17,
		RESERVED_18,
		RESERVED_19,
		RESERVED_20,
		RESERVED_21,
		RESERVED_22,
		RESERVED_23,
		RESERVED_24,
		RESERVED_25,
		ART // art _MUST_ always be the last function, lest we break the keymap dialog...
			 // Also remember to modify NUMBER_MAPPABLE_FUNCTIONS, above...
	};
};

struct keymap_name_t
{
	UINT name;
};

const keymap_name_t keymap_names[NUMBER_MAPPABLE_FUNCTIONS] =
{
	{ IDS_FOCAL_FLAME },
	{ IDS_FOCAL_BLADE },
	{IDS_RESERVED},
	{IDS_TOGGLE_NAMES}, // pmares do not see nametags
	{IDS_DROP_ITEM},
	{IDS_PERFORM_ACTION},
	{IDS_OPEN_GOAL_BOOK},
	{IDS_LEAVE_PARTY},
	{IDS_SELECT_NEXT},
	{IDS_RESET_EYE_LEVEL},
	{IDS_TOGGLE_SOUND},
	{IDS_TALK},
	{IDS_WHO_NEARBY},
	{IDS_SHOW_ACTIVE_FFX},
	{IDS_SHOW_XP},
	{IDS_LOOK_UP},
	{IDS_LOOK_DOWN},
	{IDS_MOVE_FORWARDS},
	{IDS_MOVE_BACKWARDS},
	{IDS_TURN_LEFT},
	{IDS_TURN_RIGHT},
	{IDS_RUN},
	{IDS_JUMP},
	{IDS_USE},
	{IDS_TRIGGER_SWITCHES},
	{IDS_STRAFE},
	{IDS_SIDESTEP_LEFT},
	{IDS_SIDESTEP_RIGHT},
	{IDS_TOGGLE_MOUSE_LOOK},
	{IDS_LIST_SCROLL_UP},
	{IDS_LIST_SCROLL_DOWN},
	{IDS_VIEW_INVENTORY},
	{IDS_VIEW_NEIGHBORS},
	{IDS_VIEW_ARTS},
	{IDS_SELECT_FROM_LIST},
	{IDS_CUSTOMIZE_AVATAR},
	{IDS_SELECT_PREVIOUS},
	{IDS_SHOW_NEXT},
	{IDS_SHOW_PREV},
	{IDS_SHOW_RANK},
	{IDS_SHOW_FOCUS_STAT},
	{IDS_WAVE},
	{IDS_SHOW_SHIELD_KEY},
	{IDS_IGNORE_LIST},
	{IDS_LEARNABLE_ARTS_KEY},
	{IDS_TOGGLE_AUTORUN},
	{IDS_FILTER},
	{IDS_TIME_ONLINE_KEY},
	{IDS_INSPECT_ITEM},
	{IDS_DESELECT},
	{IDS_DST},  // was {"Break Telepathy"}, // Jared 2-26-00 (Not yet implemented)
	{IDS_BACKGROUNDMUSIC},
	{IDS_SHOW_PARTY},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED},
	{IDS_RESERVED}
};


struct keymap_t {
	UINT key;
	UINT func;
	int art;
};

const int KEY_INDEX_OUT_OF_BOUNDS = 9999;
const int FUNC_INDEX_OUT_OF_BOUNDS = -1;
const int NULL_MAPPING = 9999;
const int MAPPING_NOT_FOUND = -1;

const int default_num_keys = 57;
const keymap_t default_keymap[default_num_keys] = 
{
	{'1', LyraKeyboard::ART, Arts::SENSE_DREAMERS},
	{'2', LyraKeyboard::WAVE, Arts::NONE},
	{'I', LyraKeyboard::TOGGLE_NAMES, Arts::NONE},
	{'D', LyraKeyboard::SIDESTEP_RIGHT, Arts::NONE},
	{'E', LyraKeyboard::EMOTE, Arts::NONE},
	{'G', LyraKeyboard::OPEN_GOAL_BOOK, Arts::NONE},
	{'L', LyraKeyboard::ART, Arts::LOCATE_AVATAR},
	{'M', LyraKeyboard::SELECT_NEXT, Arts::NONE},
	{'N', LyraKeyboard::SELECT_PREV, Arts::NONE},
	{',', LyraKeyboard::SHOW_NEXT, Arts::NONE},
	{'.', LyraKeyboard::SHOW_PREV, Arts::NONE},
	{'R', LyraKeyboard::RESET_EYELEVEL, Arts::NONE},

	{'=', LyraKeyboard::TOGGLE_SOUND, Arts::NONE},
	{'T', LyraKeyboard::TALK, Arts::NONE},
	{222, LyraKeyboard::LEAVE_PARTY, Arts::NONE}, // (apostrophe)
	{VK_UP, LyraKeyboard::WHO_NEARBY, Arts::NONE},

	{'C', LyraKeyboard::ACTIVE_EFFECTS, Arts::NONE},
	{'X', LyraKeyboard::SHOW_XP, Arts::NONE},
	{VK_HOME, LyraKeyboard::LOOK_DOWN, Arts::NONE},
	{VK_END, LyraKeyboard::LOOK_UP, Arts::NONE},
	{'W', LyraKeyboard::MOVE_FORWARD, Arts::NONE},
	{'S', LyraKeyboard::MOVE_BACKWARD, Arts::NONE},

	{VK_LEFT, LyraKeyboard::TURN_LEFT, Arts::NONE},
	{VK_RIGHT, LyraKeyboard::TURN_RIGHT, Arts::NONE},
	{VK_CONTROL, LyraKeyboard::USE, Arts::NONE},
	{VK_SPACE, LyraKeyboard::TRIP, Arts::NONE},
	{ VK_MENU, LyraKeyboard::JUMP, Arts::NONE },
	{ VK_SHIFT, LyraKeyboard::RUN, Arts::NONE},
	{VK_LBUTTON, LyraKeyboard::USE, Arts::NONE},
	{VK_MBUTTON, LyraKeyboard::MOUSE_LOOK, Arts::NONE},
	{VK_RBUTTON, LyraKeyboard::JUMP, Arts::NONE},
	{0xdc, LyraKeyboard::MOUSE_LOOK, Arts::NONE}, // (backslash)

	{'A', LyraKeyboard::SIDESTEP_LEFT, Arts::NONE},

	{'K', LyraKeyboard::SHOW_RANKS, Arts::NONE},
	{VK_PRIOR, LyraKeyboard::SCROLL_UP, Arts::NONE},
	{VK_NEXT, LyraKeyboard::SCROLL_DOWN, Arts::NONE},
	{VK_DIVIDE, LyraKeyboard::PICK_INVENTORY, Arts::NONE},
	{VK_MULTIPLY, LyraKeyboard::PICK_NEIGHBORS, Arts::NONE},
	{VK_SUBTRACT, LyraKeyboard::PICK_ARTS, Arts::NONE},
	{VK_ADD, LyraKeyboard::SELECT_FROM_LIST, Arts::NONE},
	{'F', LyraKeyboard::FOCAL_FLAME, Arts::NONE},
	//{'H', LyraKeyboard::WAVE, Arts::NONE},
	{'O', LyraKeyboard::SHOW_SHIELD, Arts::NONE},
	{'P', LyraKeyboard::ART, Arts::JOIN_PARTY},
	{'Q', LyraKeyboard::ART, Arts::RANDOM},
	{'Y', LyraKeyboard::ART, Arts::MEDITATION},
	{'U', LyraKeyboard::ART, Arts::TRAIL},
	{'V', LyraKeyboard::ART, Arts::GIVE},
	{'B', LyraKeyboard::FOCAL_BLADE, Arts::NONE},
	{0xbf, LyraKeyboard::ART, Arts::KNOW}, // (is also the ? key!)
	{'Z', LyraKeyboard::SHOW_LEARNABLE_ARTS, Arts::NONE},
	{0xc0, LyraKeyboard::TOGGLE_AUTORUN, Arts::NONE}, // Jared 2-26-00
	{'-', LyraKeyboard::TOGGLE_ADULT_FILTER, Arts::NONE}, 
	{'[', LyraKeyboard::TIME_ONLINE, Arts::NONE}, 	
	{']', LyraKeyboard::SHOW_DST_TIME, Arts::NONE}, 	
//	{'-', LyraKeyboard::BREAK_TELEPATHY, Arts::NONE}, // Jared 2-26-00 (Not yet implemented)
};

const int mousemove_default_num_keys = 60;
const keymap_t mousemove_default_keymap[mousemove_default_num_keys] = 
{
	{ '1', LyraKeyboard::ART, Arts::SENSE_DREAMERS },
	{ '2', LyraKeyboard::WAVE, Arts::NONE },
	{'A', LyraKeyboard::SIDESTEP_LEFT, Arts::NONE},
	{'D', LyraKeyboard::SIDESTEP_RIGHT, Arts::NONE},
	{'W', LyraKeyboard::MOVE_FORWARD, Arts::NONE},
	{'X', LyraKeyboard::MOVE_BACKWARD, Arts::NONE},
	{'E', LyraKeyboard::EMOTE, Arts::NONE},
	{'G', LyraKeyboard::OPEN_GOAL_BOOK, Arts::NONE},
	{'L', LyraKeyboard::LEAVE_PARTY, Arts::NONE},
	{'N', LyraKeyboard::SELECT_NEXT, Arts::NONE},
	{'M', LyraKeyboard::SELECT_PREV, Arts::NONE},
	{',', LyraKeyboard::SHOW_NEXT, Arts::NONE},
	{'.', LyraKeyboard::SHOW_PREV, Arts::NONE},
	{'R', LyraKeyboard::RESET_EYELEVEL, Arts::NONE},
	{'S', LyraKeyboard::TOGGLE_SOUND, Arts::NONE},
	{'T', LyraKeyboard::TALK, Arts::NONE},
	{'Z', LyraKeyboard::AVATAR_CUSTOMIZATION, Arts::NONE},
	{222, LyraKeyboard::WHO_NEARBY, Arts::NONE}, // (apostrophe)
	{'C', LyraKeyboard::ACTIVE_EFFECTS, Arts::NONE},
	{0xba, LyraKeyboard::SHOW_XP, Arts::NONE}, // (semicolon)
	{0xbe, LyraKeyboard::TOGGLE_NAMES, Arts::NONE}, // (period)
	{0xbc, LyraKeyboard::DROP_ITEM, Arts::NONE}, // (comma)
	{VK_HOME, LyraKeyboard::LOOK_DOWN, Arts::NONE},
	{VK_END, LyraKeyboard::LOOK_UP, Arts::NONE},
	{VK_UP, LyraKeyboard::MOVE_FORWARD, Arts::NONE},
	{VK_DOWN, LyraKeyboard::MOVE_BACKWARD, Arts::NONE},
	{VK_LEFT, LyraKeyboard::TURN_LEFT, Arts::NONE},
	{VK_RIGHT, LyraKeyboard::TURN_RIGHT, Arts::NONE},
	{VK_CONTROL, LyraKeyboard::USE, Arts::NONE},
	{VK_SPACE, LyraKeyboard::TRIP, Arts::NONE},
	{'J', LyraKeyboard::JUMP, Arts::NONE},
	{VK_SHIFT, LyraKeyboard::RUN, Arts::NONE},
	{VK_MENU, LyraKeyboard::MOUSE_LOOK, Arts::NONE},
	{VK_LBUTTON, LyraKeyboard::USE, Arts::NONE},
	{VK_MBUTTON, LyraKeyboard::MOUSE_LOOK, Arts::NONE},
	{VK_RBUTTON, LyraKeyboard::JUMP, Arts::NONE},
	{0xdc, LyraKeyboard::STRAFE, Arts::NONE}, // (backslash)
	{'K', LyraKeyboard::SHOW_RANKS, Arts::NONE},
	{VK_PRIOR, LyraKeyboard::SCROLL_UP, Arts::NONE},
	{VK_NEXT, LyraKeyboard::SCROLL_DOWN, Arts::NONE},
	{VK_DIVIDE, LyraKeyboard::PICK_INVENTORY, Arts::NONE},
	{VK_MULTIPLY, LyraKeyboard::PICK_NEIGHBORS, Arts::NONE},
	{VK_SUBTRACT, LyraKeyboard::PICK_ARTS, Arts::NONE},
	{VK_ADD, LyraKeyboard::SELECT_FROM_LIST, Arts::NONE},
	{'F', LyraKeyboard::SHOW_FOCUS, Arts::NONE},
	{'H', LyraKeyboard::WAVE, Arts::NONE},
	{'O', LyraKeyboard::SHOW_SHIELD, Arts::NONE},
	{'P', LyraKeyboard::ART, Arts::JOIN_PARTY},
	{'Q', LyraKeyboard::ART, Arts::RANDOM},
	{'Y', LyraKeyboard::ART, Arts::MEDITATION},
	{'U', LyraKeyboard::ART, Arts::TRAIL},
	{'V', LyraKeyboard::ART, Arts::GIVE},
	{'B', LyraKeyboard::ART, Arts::LOCATE_AVATAR},
	{0xbf, LyraKeyboard::ART, Arts::KNOW}, // (is also the ? key!)
	{'Z', LyraKeyboard::SHOW_LEARNABLE_ARTS, Arts::NONE},
	{0xc0, LyraKeyboard::TOGGLE_AUTORUN, Arts::NONE}, // Jared 2-26-00
	{'-', LyraKeyboard::TOGGLE_ADULT_FILTER, Arts::NONE},
	{'[', LyraKeyboard::TIME_ONLINE, Arts::NONE},
	{']', LyraKeyboard::SHOW_DST_TIME, Arts::NONE}, 	
//	{'-', LyraKeyboard::BREAK_TELEPATHY, Arts::NONE}, // Jared 2-26-00 (Not yet implemented)
};




//////////////////////////////////////////////////////////////////
// Class Definition

class cKeymap
{
   private:
	   int num_keys_used;
	   keymap_t *mappings;

   public:
	   cKeymap(void); 
	  ~cKeymap(void);

	  // selectors
	  inline int num_keys(void) { return num_keys_used; };
	  void GetMap(keymap_t *map);
	  UINT GetKey(int index);
	  int GetFunc(int index);
	  int GetArt(int index);

	  // mutators
	  void Init(int initnum_keys, keymap_t *initmap);
	  void AddMapping (UINT vk, int key_func, int art);
	  int FindMapping (UINT vk);
	  int FindArt (UINT vk);
	  int InvalidateMapping (UINT vk);
	  void SetDefaultKeymap(int);

	private:
	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cKeymap(const cKeymap& x);
	cKeymap& operator=(const cKeymap& x);
	
};

#endif
