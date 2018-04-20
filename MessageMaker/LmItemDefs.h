// LmItemDefs.h  -*- C++ -*-
// $Id: LmItemDefs.h,v 1.35 1998-05-11 11:00:29-07 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item type/state definitions

#ifndef INCLUDED_LmItemDefs
#define INCLUDED_LmItemDefs

#ifdef __GNUC__
#pragma interface
#endif

#ifdef WIN32
#include "unix.h"
#include <winsock2.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include "LyraDefs.h"

// item definitions class

class LyraItem {

public:

  // constants
  enum {
    // item header flags (up to 16)
    FLAG_NOREAP         = 0x01,  // item is not be reaped by server
    FLAG_SENDSTATE      = 0x02,  // item must have state sent always (wards)
    FLAG_IMMUTABLE      = 0x04,  // item state cannot be changed
    FLAG_CHANGE_CHARGES = 0x08,  // item state cannot be changed, except for # of charges
    FLAG_ALWAYS_DROP    = 0x10,  // item is auto-dropped upon player logout
    FLAG_HASDESCRIPTION = 0x20,  // item has text description
	FLAG_ALWAYSREAP = 0x40, // ALWAYS reap this item, even in vaults
	FLAG_NOPICKUP = 0x80, // Players can't drag/pickup this item
	FLAG_ISCOMBINED = 0x100,
    MAX_FIELDS_PER_FUNCTION = 7, // max # of subfields per function

    // item functions
    NO_FUNCTION = 0,		
    WARD_FUNCTION,           // 10 bytes
    CHANGE_STAT_FUNCTION,    // 3 bytes
    MISSILE_FUNCTION,        // 6 bytes
    EFFECT_PLAYER_FUNCTION,  // 3 bytes
    AMULET_FUNCTION,         // 6 bytes
    AREA_EFFECT_FUNCTION,    // 9 bytes
    ESSENCE_FUNCTION,	     // 8 bytes
    ARMOR_FUNCTION,			 // 4 bytes
    SUPPORT_FUNCTION,        // 10 bytes
    META_ESSENCE_FUNCTION,   // 9 bytes
    SCROLL_FUNCTION,         // 10 bytes
    NOTHING_FUNCTION,        // 10 bytes
    COLLECTION_FUNCTION,     // 10 bytes
    MAP_FUNCTION,            // 10 bytes
    TELEPORTER_FUNCTION,     // 10 bytes
	SUPPORT_TRAIN_FUNCTION,  // 10 bytes
	GRATITUDE_FUNCTION,		 // 10 bytes
	META_ESSENCE_NEXUS_FUNCTION, // 9 bytes
	PORTKEY_FUNCTION, // 7 bytes
    
    // translation types for item effect fields
    TRANSLATION_NONE = 0,
    TRANSLATION_MODIFIER,
    TRANSLATION_DURATION,
    TRANSLATION_EFFECT,
    TRANSLATION_STAT,
    TRANSLATION_GUILD,
    TRANSLATION_ART,
    TRANSLATION_MISSILE_BITMAP,
    TRANSLATION_VELOCITY,
    TRANSLATION_POS_MODIFIER,
    TRANSLATION_DURABILITY,
    TRANSLATION_ABSORPTION,
    TRANSLATION_NIGHTMARE,
    TRANSLATION_GUILDTOKEN,
    TRANSLATION_LEVEL_ID,
    TRANSLATION_TPORT_DEST,
	TRANSLATION_DISTANCE
  };
  
  // descriptor table selectors
  static int FASTCALL NumItemFormats();
  static int FASTCALL FieldOffset(int format, int fieldnum);
  static int FASTCALL FieldLength(int format, int fieldnum);
  static int FASTCALL FormatType(int size1 = 0, int size2 = 0, int size3 = 0);

  // function type table selectors
  static int FASTCALL NumItemFunctions();
  static const TCHAR* FASTCALL FunctionName(int function);
  static int FASTCALL FunctionSize(int function);
  static int FASTCALL FunctionEntries(int function);
  static bool FASTCALL FunctionCreateByArt(int function);
  static bool FASTCALL FunctionCreateByForge(int function);
  static bool FASTCALL FunctionCreateByGM(int function);
  static bool FASTCALL FunctionAlwaysSendState(int function);
  static bool FASTCALL FunctionImmutable(int function);
  static bool FASTCALL FunctionChangeCharges(int function);
  static bool FASTCALL FunctionLosable(int function);
  static const TCHAR* FASTCALL EntryName(int function, int entry);
  static int FASTCALL EntrySize(int function, int entry);
  static int FASTCALL EntryTranslation(int function, int entry);
  static bool FASTCALL EntryIdentifiable(int function, int entry);
  static int FASTCALL EntryMinValue(int function, int entry);
  static int FASTCALL EntryMaxValue(int function, int entry);

  // return function of given state structure
  static int FASTCALL StateFunction(const void* ssptr);

  // state structure conversion
  static void FASTCALL ConvertStateToNetwork(void* ssptr);
  static void FASTCALL ConvertStateToHost(void* ssptr);

public: // logically private

  // Note: these types must be public, otherwise Sun's C++ compiler complains for some reason

  // item state descriptor format table entry
  struct item_format_entry_t {
    int format;  // descriptor value
    int len[3];  // field lengths
  };

  // each field within an item function struct has one of these associated with it
  struct item_function_field_t {
    TCHAR name[20]; // name of field
    short size;	   // size in bytes
    short translation_index;	// which translation table to use
    bool identifiable;		// does field show up on identify?
    int min;
    int max;
  };

  // used to translate item functions into human readable interfaces
  struct item_function_entry_t {
    TCHAR name[20];
    short size;
    int	num_valid_fields;
    bool create_by_art; // can they be created by arts?
    bool create_by_forge; // can they be created by forge talisman?
    bool create_by_gm; // can they be created by a gm terminal?
    bool always_send_state; // do items with the funtion always send state?
    bool immutable; // is the function always immutable?
    bool change_charges; // is the function always immutable except # of charges?
    bool losable; // is item a candiate for dropping at logout?
    item_function_field_t fields[MAX_FIELDS_PER_FUNCTION];
  };

private:

  // not implemented
  LyraItem(const LyraItem&);
  //operator=(const LyraItem&);

  // descriptor table
  static const item_format_entry_t item_formats[];
  static const int item_formats_num;
  static const item_function_entry_t item_functions[];
  static const int item_functions_num;

  // helper methods
  static bool FASTCALL function_ok(int function);
  static bool FASTCALL entry_ok(int function, int entry);
  static bool FASTCALL field_ok(int fieldnum);
  static int FASTCALL format_index(int format);

protected:

  // if this is ever called, a linker error will result, since it is never
  // defined; this is as it should be, since this class is not instantiable
  LyraItem();

};

////
// Item State Structures
////

// Note: be careful of structure alignment and padding!
// *ALWAYS* double check the size at run time & ensure the table is compatible!

// Note: structures with fields other than chars will need hton() and ntoh(), plus
//   ConvertStateToNetwork/Host will need to be updated.

// wards need to have player id hacked into two parts so that
// they will fit into a 10 byte packet :(
struct lyra_item_ward_t {  // 10 bytes -> descriptor 8
  unsigned char type;       // WARD_FUNCTION
  unsigned char strength;   // strength of ward
  short         from_vert;  // linedef for ward
  short         to_vert;
  unsigned short id_hi_bits;	// top bits of id of ward placer
  unsigned short id_lo_bits;	// bottom bits of id of ward placer
  // aux methods
  inline unsigned int player_id() {
    return ((id_hi_bits<<16) | id_lo_bits);
  };
  inline void set_player_id(unsigned int id) {
    id_hi_bits = (id >> 16 );
    id_lo_bits = (id & 0x0000ffff);
  };
  // conversion methods
  inline void hton() {
    HTONS(from_vert);
    HTONS(to_vert);
    HTONS(id_hi_bits);
    HTONS(id_lo_bits);
  }
  inline void ntoh() {
    NTOHS(from_vert);
    NTOHS(to_vert);
    NTOHS(id_hi_bits);
    NTOHS(id_lo_bits);
  }
};

struct lyra_item_change_stat_t {  // 3 bytes
  unsigned char type;      // CHANGE_STAT_FUNCTION
  unsigned char stat;      // which stat to effect
  char          modifier;  // index into modifer table; neg = negate
  // no hton() or ntoh() needed, all chars
};

struct lyra_item_missile_t {  // 6 bytes 
  unsigned char type;       // MISSILE_FUNCTION
  char          velocity;   // negative for bouncing missiles!
  unsigned char effect;     // timed effect constant activate on hit
  char          damage;     // index into modifier table for damage
  short         bitmap_id;  // bitmap for missile to use
  // conversion methods
  inline void hton() {
    HTONS(bitmap_id);
  }
  inline void ntoh() {
    NTOHS(bitmap_id);
  }
};

struct lyra_item_effect_player_t {  // 3 bytes
  unsigned char type;      // EFFECT_PLAYER_FUNCTION
  unsigned char effect;    // timed effect constant activate on hit
  unsigned char duration;  // index into duration table for effect
  // no hton() or ntoh() needed, all chars
};

struct lyra_item_amulet_t {  // 8 bytes 
  unsigned char type;        // AMULET_FUNCTION
  unsigned char strength;    // skill of caster
  short unused;
  unsigned int	player_id;	 // id of caster
  inline bool IsKey() {
	  return strength > 99;
  }
  // conversion methods
  inline void hton() {
    HTONL(player_id);
  }
  inline void ntoh() {
    NTOHL(player_id);
  }
};

struct lyra_item_area_effect_t {  // 10
  unsigned char type;      // AREA_EFFECT_FUNCTION
  unsigned char effect;    // timed effect constant activate on hit, hibit is effect self and party. 0 means to effect.
  unsigned char duration;
  unsigned char stat;
  char          damage;    // index into modifier table for damage
  unsigned char distance;
  unsigned short id_hi_bits;	// top bits of id of ward placer
  unsigned short id_lo_bits;	// bottom bits of id of ward placer
								// aux methods
  inline unsigned int player_id() {
	  return ((id_hi_bits << 16) | id_lo_bits);
  };
  inline void set_player_id(unsigned int id) {
	  id_hi_bits = (id >> 16);
	  id_lo_bits = (id & 0x0000ffff);
  };
  // conversion methods
  inline void hton() {
    HTONS(id_hi_bits);
	HTONS(id_lo_bits);
  }
  inline void ntoh() {
    NTOHS(id_hi_bits);
	NTOHS(id_lo_bits);
  }
  inline void set_effects_party_and_self(bool effect_party)
  {
	  if (!effect_party)
		  effect |= (1 << 7);
	  else
		  effect &= 127; // 0111111
  }
  inline bool effects_party_and_self()
  {
	  // returns true if effects false if not.
	  return !(effect & (1 << 7));
  }

  inline void set_razorwind(bool is_rw)
  {
	  if (is_rw)
		  distance |= (1 << 7);
	  else
		  distance &= 127;
  }

  inline bool is_razorwind()
  {
	  return (distance & (1 << 7));
  }

  inline unsigned char get_distance() {
	  return distance & 127;
  }

  inline unsigned char get_effect() {
	  return effect & 127;
  }
};

struct lyra_item_portkey_t {
	unsigned char type;      // PORTKEY_FUNCTION
	unsigned char unused;
	unsigned char distance;
	unsigned char	level_id;
	short x;
	short y; 

	inline void hton() {
		HTONS(x);
		HTONS(y);
	}
	inline void ntoh() {
		NTOHS(x);
		NTOHS(y);		
	}
};

struct lyra_item_essence_t {  // 8 bytes 
  unsigned char type;        // ESSENCE_FUNCTION
  unsigned char mare_type;   // type of mare enslaved
  unsigned char strength;    // amount of essence left in talisman
  unsigned char weapon_type; // type of weapon that took 'em down
  lyra_id_t     slaver_id;   // id of player who enslaved the mare
  // conversion methods
  inline void hton() {
    HTONL(slaver_id);
  }
  inline void ntoh() {
    NTOHL(slaver_id);
  }
};

struct lyra_item_armor_t {  // 4 bytes 
  unsigned char type;        // ARMOR_FUNCTION
  unsigned char curr_durability;   // how much more damage it can take
  unsigned char max_durability;  // max durability
  unsigned char absorption;	  // what % of damage it absorbs
  // no hton() or ntoh() needed, all chars
};

struct lyra_item_support_t {  // 10 bytes 
  unsigned char type;        // SUPPORT_FUNCTION
  unsigned char guild_token; // support for which guild AND
                             // token purpose (membership, ascend, demote...)
  unsigned short target_hi_bits; // id of player for whom it was created
  unsigned short target_lo_bits; // id of player for whom it was created
  unsigned short creator_lo_bits;  // id of player who created the token
  unsigned short creator_hi_bits;  // id of player who created the token
  // aux methods
  inline unsigned char guild_id() {
    return ((guild_token >> 4) & 0x0f);
  };
  inline unsigned char token_type() {
    return (guild_token & 0x0f);
  };
  inline void set_guild_token(unsigned char guild, unsigned char token) {
    guild_token = (((guild & 0x0f) << 4) | (token & 0x0f));
  };
  inline unsigned int target_id() {
    return ((target_hi_bits<<16) | target_lo_bits);
  };
  inline void set_target_id(unsigned int id) {
    target_hi_bits = (id >> 16 );
    target_lo_bits = (id & 0x0000ffff);
  };
  inline unsigned int creator_id() {
    return ((creator_hi_bits<<16) | creator_lo_bits);
  };
  inline void set_creator_id(unsigned int id) {
    creator_hi_bits = (id >> 16 );
    creator_lo_bits = (id & 0x0000ffff);
  };
  // conversion methods
  inline void hton() {
    HTONS(target_hi_bits);
    HTONS(creator_hi_bits);
    HTONS(target_lo_bits);
    HTONS(creator_lo_bits);
  }
  inline void ntoh() {
    NTOHS(target_hi_bits);
    NTOHS(creator_hi_bits);
    NTOHS(target_lo_bits);
    NTOHS(creator_lo_bits);
  }
};

struct lyra_item_meta_essence_nexus_t { // 9 bytes
	unsigned char type;			// META_ESSENCE_NEXUS_FUNCTION
	unsigned char unused;
	unsigned short strength;
	unsigned short essences;
	unsigned short strength_cap;
	unsigned short essence_cap;

	inline void hton() {
		HTONS(strength);
		HTONS(essences);
		HTONS(essence_cap);
		HTONS(strength_cap);
	}

	inline void ntoh() {
		NTOHS(strength);
		NTOHS(essences);
		NTOHS(essence_cap);
		NTOHS(strength_cap);
	}
};

struct lyra_item_meta_essence_t {  // 10 bytes 
  unsigned char type;        // META_ESSENCE_FUNCTION
  unsigned char guild_id;  // talisman for which guild 
  unsigned short strength_hi_bits;  // current amount of essence
  unsigned short strength_lo_bits;  // current amount of essence
  unsigned short num_mares_hi_bits; // # of mares that have been absorbed into it
  unsigned short num_mares_lo_bits; // # of mares that have been absorbed into it
  // aux methods
  inline unsigned int strength() {
    return ((strength_hi_bits<<16) | strength_lo_bits);
  };
  inline void set_strength(unsigned int value) {
    strength_hi_bits = (value >> 16 );
    strength_lo_bits = (value & 0x0000ffff);
  };
  inline unsigned int num_mares() {
    return ((num_mares_hi_bits<<16) | num_mares_lo_bits);
  };
  inline void set_num_mares(unsigned int value) {
    num_mares_hi_bits = (value >> 16 );
    num_mares_lo_bits = (value & 0x0000ffff);
  };
  // conversion methods
  inline void hton() {
    HTONS(strength_hi_bits);
    HTONS(strength_lo_bits);
    HTONS(num_mares_hi_bits);
    HTONS(num_mares_lo_bits);
  }
  inline void ntoh() {
    NTOHS(strength_hi_bits);
    NTOHS(strength_lo_bits);
    NTOHS(num_mares_hi_bits);
    NTOHS(num_mares_lo_bits);
  }
};

struct lyra_item_scroll_t { // 10 bytes
  unsigned char type;  // SCROLL_FUNCTION
  unsigned char art_id;  // holds (art_id + 1) was reserved
  unsigned short target_hi;          // was reserved (pad2)
  unsigned short creator_hi; // creator id (must be split, argh)
  unsigned short creator_lo;
  unsigned short target_lo;          // was reserved (pad3)
  // aux methods
  inline unsigned int creatorid() {
    return ((creator_hi<<16) | creator_lo);
  };
  inline void set_creatorid(unsigned int value) {
    creator_hi = (value >> 16 );
    creator_lo = (value & 0x0000ffff);
  };
  inline unsigned int targetid() {
    return ((target_hi<<16) | target_lo);
  };
  inline void set_targetid(unsigned int value) {
    target_hi = (value >> 16 );
    target_lo = (value & 0x0000ffff);
  };

  // conversion methods
  inline void hton() {
    HTONS(target_hi);
    HTONS(target_lo);
    HTONS(creator_hi);
    HTONS(creator_lo);
  }
  inline void ntoh() {
    NTOHS(target_hi);
    NTOHS(target_lo);
    NTOHS(creator_hi);
    NTOHS(creator_lo);
  }
};

struct lyra_item_nothing_t { // 2 bytes
  unsigned char type; // NOTHING_FUNCTION
  char padding;
};

struct lyra_item_collection_t { // 3 bytes
  unsigned char type; // COLLECTION_FUNCTION
  unsigned char coll_type;  // type of collection
  unsigned char coll_num;   // collection number ("id")
};

struct lyra_item_map_t { // 2 bytes
  unsigned char type; // MAP_FUNCTION
  unsigned char level_id;
};

struct lyra_item_teleporter_t { // 6 bytes
  unsigned char type; // TELEPORTER_FUNCTION
  unsigned char	level_id;
  short x;
  short y;
  // conversion methods
  inline void hton() {
    HTONS(x);
    HTONS(y);
  }
  inline void ntoh() {
    NTOHS(x);
    NTOHS(y);
  }
};


// player self training
// we only store the low bits of the creators ID so we have room to 
// store both the art number & skill level

struct lyra_item_train_support_t {  // 10 bytes 
  unsigned char type;        // SUPPORT_TRAIN_FUNCTION
  unsigned char art_id; // id of art, or 255 for sphere
  unsigned char art_level; // max level of art or sphere supported
  unsigned char padding;
  unsigned short target_hi_bits; // id of player for whom it was created
  unsigned short target_lo_bits; // id of player for whom it was created
  unsigned short creator_lo_bits;  // id of player who created the token

  // aux methods
  inline unsigned int target_id() {
    return ((target_hi_bits<<16) | target_lo_bits);
  };
  inline void set_target_id(unsigned int id) {
    target_hi_bits = (id >> 16 );
    target_lo_bits = (id & 0x0000ffff);
  };
  inline unsigned int creator_id() {
    return ((unsigned int)creator_lo_bits);
  };
  inline void set_creator_id(unsigned int id) {
    //creator_hi_bits = (id >> 16 );
    creator_lo_bits = (id & 0x0000ffff);
  };
  // conversion methods
  inline void hton() {
    HTONS(target_hi_bits);
    HTONS(target_lo_bits);
    HTONS(creator_lo_bits);
  }
  inline void ntoh() {
    NTOHS(target_hi_bits);
    NTOHS(target_lo_bits);
    NTOHS(creator_lo_bits);
  }
};

struct lyra_item_gratitude_t { // 10 bytes
	// the structure is carefully aligned to make it easy to create these
	// sorts of items in Perl via the billing system
	unsigned char type; // GRATITUDE_FUNCTION
	unsigned char unused; 
	unsigned short maturity_date; // # of days after year 2000 that it matures
	unsigned short creator_hi; // creator id (must be split, argh)
	unsigned short creator_lo;
	unsigned short target; // we can only store the low 2 bytes of the target ID
	
  // aux methods
  inline unsigned int creatorid() {
    return ((creator_hi<<16) | creator_lo);
  };
  inline void set_creatorid(unsigned int value) {
    creator_hi = (value >> 16 );
    creator_lo = (value & 0x0000ffff);
  };
  // conversion methods
  inline void hton() {
    HTONS(creator_hi);
    HTONS(creator_lo);
	HTONS(maturity_date);
	HTONS(target);
  }
  inline void ntoh() {
    NTOHS(creator_hi);
    NTOHS(creator_lo);
	NTOHS(maturity_date);
	NTOHS(target);
  }
};

#ifdef USE_INLINE
#include "LmItemDefs.i"
#endif

#endif /* INCLUDED_LmItemDefs */
