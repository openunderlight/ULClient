// LmItemDefs.cpp  -*- C++ -*-
// $Id: LmItemDefs.cpp,v 1.35 1998/05/17 21:49:21 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmItemDefs.h"
#endif

#include <limits.h>
#include <string.h>

#include "LmItemDefs.h"
#ifdef WIN32
#include "../Underlight/SharedConstants.h"
#else
#include "SharedConstants.h"
#endif
#include "LmItemNames.h"
#include "LmItem.h"

#ifndef USE_INLINE
#include "LmItemDefs.i"
#endif

// definition of the item state format table

const LyraItem::item_format_entry_t LyraItem::item_formats[] = {

  // format  field lengths (must sum to at most 11, use 0 for unused fields,
  //                        lengths should be packed to the left and in decreasing size)

  // single use items
  {  0,     {  2, 0, 0 } },  // 2
  {  1,     {  3, 0, 0 } },  // 3
  {  2,     {  4, 0, 0 } },  // 4 
  {  3,     {  5, 0, 0 } },  // 5
  {  4,     {  6, 0, 0 } },  // 6
  {  5,     {  7, 0, 0 } },  // 7
  {  6,     {  8, 0, 0 } },  // 8
  {  7,	    {  9, 0, 0 } },  // 9
  {  8,	    { 10, 0, 0 } },  // 10
  {  9,	    { 11, 0, 0 } },  // 11

  // double use items
  { 10,     {  2, 2, 0 } },  // 4
  { 11,     {  3, 2, 0 } },  // 5
  { 12,     {  3, 3, 0 } },  // 6
  { 13,     {  4, 2, 0 } },  // 6
  { 14,     {  4, 3, 0 } },  // 7
  { 15,     {  4, 4, 0 } },  // 8
  { 16,     {  5, 2, 0 } },  // 7
  { 17,     {  5, 3, 0 } },  // 8
  { 18,     {  5, 4, 0 } },  // 9
  { 19,     {  5, 5, 0 } },  // 10
  { 20,     {  6, 2, 0 } },  // 8
  { 21,     {  6, 3, 0 } },  // 9
  { 22,     {  6, 4, 0 } },  // 10
  { 23,     {  6, 5, 0 } },  // 11
  { 24,     {  7, 2, 0 } },  // 9
  { 25,     {  7, 3, 0 } },  // 10
  { 26,     {  7, 4, 0 } },  // 11
  { 27,     {  8, 2, 0 } },  // 10
  { 28,     {  8, 3, 0 } },  // 11
  { 29,     {  9, 2, 0 } },  // 11

  // triple use items
  { 30,     {  2, 2, 2 } },  // 6
  { 31,     {  3, 2, 2 } },  // 7
  { 32,     {  3, 3, 2 } },  // 8
  { 33,     {  3, 3, 3 } },  // 9
  { 34,     {  4, 2, 2 } },  // 8
  { 35,     {  4, 3, 2 } },  // 9
  { 36,     {  4, 3, 3 } },  // 10
  { 37,     {  4, 4, 2 } },  // 10
  { 38,     {  4, 4, 3 } },  // 11
  { 39,     {  5, 2, 2 } },  // 9
  { 40,     {  5, 3, 2 } },  // 10
  { 41,     {  5, 3, 3 } },  // 11
  { 42,     {  5, 4, 2 } },  // 11
  { 43,     {  6, 2, 2 } },  // 10
  { 44,     {  6, 3, 2 } },  // 11
  { 45,     {  7, 2, 2 } },  // 11
};

const int LyraItem::item_formats_num =
  sizeof(LyraItem::item_formats) / sizeof(LyraItem::item_format_entry_t);

// shorthand
#define NO_FIELD { _T(""), 0, 0, false, 0, 0 }

const LyraItem::item_function_entry_t LyraItem::item_functions[] = {
  // there is no entry zero - fill in dummy
  { _T(""), 0, 0, false, false, false, false, false, false, true, 
    { NO_FIELD, NO_FIELD, NO_FIELD, NO_FIELD, NO_FIELD, NO_FIELD } },

  // NOTE: these MUST be in the same order as the function constants are defined!

  /*
  *		SUPER IMPORTANT NOTE!!!!!
  *
  *		Items can have up to 6 function entries but forge can only forge up to 5!!!!
  *		Don't bother trying to change forge: that way lies madness. Simply accept the fact and move on!
  *			- MDA, 2014
  *
  */

  // WARD_FUNCTION
  { ITEM_WARD, 10, 5, true, false, false, true, true, false, true, {
    { ITEM_STR, 1, TRANSLATION_NONE, true, Stats::SKILL_MIN, Stats::SKILL_MAX},
    { ITEM_FRV, 2, TRANSLATION_NONE, false, INT_MIN, INT_MAX},
    { ITEM_TOV, 2, TRANSLATION_NONE, false, INT_MIN, INT_MAX},
    { ITEM_HIBIT, 2, TRANSLATION_NONE, false, INT_MIN, INT_MAX},
    { ITEM_LOBIT, 2, TRANSLATION_NONE, false, INT_MIN, INT_MAX},
    NO_FIELD},
  },

  // CHANGE_STAT_FUNCTION
  { ITEM_CHANGESTAT, 3, 2, false, true, true, false, false, true, true, {
    { ITEM_STAT, 1, TRANSLATION_STAT, true, 0, NUM_PLAYER_STATS-1},
    { ITEM_MOD, 1, TRANSLATION_MODIFIER, true, -NUM_MODIFIERS + 1, NUM_MODIFIERS - 1},
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

  // MISSILE_FUNCTION
  { ITEM_MISSILE, 6, 3, false, true, true, false, false, true, true, {
    {ITEM_VELOCITY, 1, TRANSLATION_VELOCITY, true, MIN_VELOCITY, MAX_VELOCITY},
    {ITEM_EFFECT, 1, TRANSLATION_EFFECT, true, 0, LyraEffect::MAX_ITEM_EFFECT},
    {ITEM_DAMAGE, 1, TRANSLATION_POS_MODIFIER, true, 0, NUM_MODIFIERS - 1},
    //{ _T("Bitmap ID"), 2, TRANSLATION_MISSILE_BITMAP, false, LyraBitmap::MINIMUM_MISSILE_BITMAP, LyraBitmap::MAXIMUM_MISSILE_BITMAP},
	NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

  // EFFECT_PLAYER_FUNCTION
  { ITEM_PEF, 3, 2, false, true, true, false, false, true, true, {
    { ITEM_EFF, 1, TRANSLATION_EFFECT, true, 0, NUM_TIMED_EFFECTS},
    { ITEM_DUR, 1, TRANSLATION_DURATION, true, 0, NUM_DURATIONS - 1},
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

  // AMULET_FUNCTION
  { ITEM_AMULET, 8, 3, true, false, false, false, true, false, true, {
    { ITEM_STR, 1, TRANSLATION_NONE, true, Stats::SKILL_MIN, Stats::SKILL_MAX},
    { ITEM_UNUSED, 2, TRANSLATION_NONE, false, INT_MIN, INT_MAX},
    { ITEM_CODE, 4, TRANSLATION_NONE, true, 0, UINT_MAX}, 
    NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

  // AREA_EFFECT_FUNCTION
  { ITEM_AE, 10, 6, false, false, true, true, true, false, true, {
	{ ITEM_EFFECT, 1, TRANSLATION_EFFECT, true, 0, NUM_TIMED_EFFECTS},
	{ ITEM_DUR, 1, TRANSLATION_DURATION, true, 0, NUM_DURATIONS - 1 },
	{ ITEM_STAT, 1, TRANSLATION_STAT, true, 0, NUM_PLAYER_STATS - 1 },
	{ ITEM_MOD, 1, TRANSLATION_MODIFIER, true, -NUM_MODIFIERS + 1, NUM_MODIFIERS - 1},
	{ ITEM_DISTANCE, 1, TRANSLATION_DISTANCE, true, 0, NUM_DISTANCES - 1},
	{ ITEM_CASTER, 4, TRANSLATION_NONE, false, 0, INT_MAX},
  } },

  // MARE_ESSENSE_FUNCTION
  { ITEM_ESSENCE, 8, 4, true, false, false, false, false, false, true, {
    { ITEM_ESSTYPE, 1, TRANSLATION_NIGHTMARE, true, 0, Avatars::MAX_AVATAR_TYPE},
    { ITEM_STR, 1, TRANSLATION_NONE, true, Stats::SKILL_MIN, Stats::SKILL_MAX},
    { ITEM_WPNTYPE, 1, TRANSLATION_NONE, false, 0, 1},
    { ITEM_MAKER, 4, TRANSLATION_NONE, false, 0, UINT_MAX},
    NO_FIELD,
    NO_FIELD},
  },

  // ARMOR_FUNCTION
  { ITEM_ARMOR, 4, 3, false, true, true, false, false, false, true, {
    { ITEM_CURRDURR, 1, TRANSLATION_DURABILITY, true, 1, 99},
    { ITEM_MAXDURR, 1, TRANSLATION_DURABILITY, true, 1, 99},
    { ITEM_ABSORB, 1, TRANSLATION_ABSORPTION, true, 1, 100},
    NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

   // SUPPORT_FUNCTION
  { ITEM_RS, 10, 5, true, false, false, false, true, false, false, {
    { ITEM_GU, 1, TRANSLATION_GUILDTOKEN, true, INT_MIN, INT_MAX},
    { ITEM_TARGETHI, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_TARGETLO, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_CREATORLO, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_CREATORHI, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    NO_FIELD},
  },

  // META_ESSENCE_FUNCTION
  { ITEM_PRIME, 10, 5, false, false, true, true, false, false, true, {
    { ITEM_GID, 1, TRANSLATION_GUILD, true, 0, NUM_HOUSES-1},
    { ITEM_STRHI, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_STRLO, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_TRAPHI, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_TRAPLO, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    NO_FIELD},
  },

  // SCROLL_FUNCTION
  { ITEM_CODEX, 10, 4, true, false, false, true, false, false, false, {
    { ITEM_ARTID, 1, TRANSLATION_ART, false, 0, UCHAR_MAX},
    { ITEM_TARGETHI, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_MAKER, 4, TRANSLATION_NONE, false, 0, UINT_MAX},
    { ITEM_TARGETLO, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    NO_FIELD,
    NO_FIELD},
  },

  // NOTHING_FUNCTION
  { ITEM_NOTHING, 2, 0, false, true, true, false, true, false, false, {
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

  // COLLECTION_FUNCTION
  { ITEM_COLLECTION, 3, 0, false, false, false, false, false, false, false, {
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

  // MAP_FUNCTION
    { ITEM_MAP, 2, 1, false, false, true, false, false, true, true, {
    { ITEM_LEVELID, 1, TRANSLATION_LEVEL_ID, true, 1, 255},
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

  // TELEPORTER_FUNCTION
  { ITEM_TPORT, 6, 3, false, false, true, false, true, false, true, {
    { ITEM_LEVELID, 1, TRANSLATION_LEVEL_ID, true, 1, 100},
    { ITEM_X, 2, TRANSLATION_TPORT_DEST, false, SHRT_MIN, SHRT_MAX},
    { ITEM_Y, 2, TRANSLATION_TPORT_DEST, false, SHRT_MIN, SHRT_MAX},
    NO_FIELD,
    NO_FIELD,
    NO_FIELD},
  },

   // SUPPORT_TRAIN_FUNCTION
  { ITEM_SUPPORT, 10, 6, true, false, false, false, true, false, false, {
    { ITEM_SUPPORTID, 1, TRANSLATION_NONE, true, INT_MIN, INT_MAX},
    { ITEM_ARTLVL, 1, TRANSLATION_NONE, true, INT_MIN, INT_MAX},
    { ITEM_UNUSED, 1, TRANSLATION_NONE, false, 0, 0},
    { ITEM_TARGETHI, 2, TRANSLATION_NONE, true, 0, USHRT_MAX},
    { ITEM_TARGETLO, 2, TRANSLATION_NONE, true, 0, USHRT_MAX},
    { ITEM_CREATORLO, 2, TRANSLATION_NONE, true, 0, UINT_MAX}},
	},

   // GRATITUDE_FUNCTION
  { ITEM_GRATITUDE, 10, 5, false, false, false, false, false, false, false, {
    { ITEM_UNUSED, 1, TRANSLATION_NONE, false, 0, 0},
    { ITEM_MATURITY, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_CREATORHI, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_CREATORLO, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    { ITEM_TARGETLO, 2, TRANSLATION_NONE, false, 0, USHRT_MAX},
    NO_FIELD},
  },

  // META_ESSENCE_NEXUS_FUNCTION
  { ITEM_NEXUS, 10, 4, true, false, true, true, false, false, false, {
	  { ITEM_UNUSED, 1, TRANSLATION_NONE, false, 0, 0 }, // might eventually be the type, like Imprison, Banish, etc.
	  { ITEM_STR, 2, TRANSLATION_NONE, false, 0, USHRT_MAX },
	  { ITEM_ESSENCE, 2, TRANSLATION_NONE, false, 0, USHRT_MAX },
	  { ITEM_STRCAP, 2, TRANSLATION_NONE, false, 0, USHRT_MAX },
	  { ITEM_ESSCAP, 2, TRANSLATION_NONE, false, 0, USHRT_MAX },
	  NO_FIELD }, },

	  // AREA_EFFECT_FUNCTION
{ ITEM_PORTKEY, 7, 6, false, false, true, true, true, false, true, {
	{ ITEM_DISTANCE, 1, TRANSLATION_DISTANCE, true, 0, NUM_DISTANCES - 1 },
	{ ITEM_LEVELID, 1, TRANSLATION_LEVEL_ID, true, 1, 100 },
	{ ITEM_X, 2, TRANSLATION_TPORT_DEST, false, SHRT_MIN, SHRT_MAX },
	{ ITEM_Y, 2, TRANSLATION_TPORT_DEST, false, SHRT_MIN, SHRT_MAX },
	NO_FIELD,
	NO_FIELD },
}
};

const int LyraItem::item_functions_num =
  sizeof(LyraItem::item_functions) / sizeof(LyraItem::item_function_entry_t);

////
// FormatType: return item format from item format table that has the given field sizes,
//   or -1 if none can be found
////

int FASTCALL LyraItem::FormatType(int size1, int size2, int size3)
{
  // check sum of sizes
  int sum = size1 + size2 + size3;
  if (sum > 11) {
    return -1;
  }
  // linear search
  for (int i = 0; i < item_formats_num; ++i) {
    if ((item_formats[i].len[0] == size1) &&
	(item_formats[i].len[1] == size2) &&
	(item_formats[i].len[2] == size3)) {
      return item_formats[i].format;
    }
  }
  // not found
  return -1;
}

////
// FieldOffset
////

int FASTCALL LyraItem::FieldOffset(int format, int field_num)
{
  if (!field_ok(field_num)) {
    return -1;
  }
  int i = format_index(format);
  if (i < 0) {
    return -1;
  }
  // add up offsets
  int offset = 0;
  for (int j = 0; j < field_num; ++j) {
    int flen = item_formats[i].len[j];
    // if length is zero, then the field is invalid
    if (!flen) {
      return -1;
    }
    offset += flen;
  }
  return offset;
}

////
// FieldLength: return length of given field in the given format, or -1 if the field/format
//   are invalid
////

int FASTCALL LyraItem::FieldLength(int format, int field_num)
{
  if (!field_ok(field_num)) {
    return -1;
  }
  int i = format_index(format);
  if (i < 0) {
    return -1;
  }
  int flen = item_formats[i].len[field_num];
  // if length is zero, then the field is invalid
  if (!flen) {
    return -1;
  }
  return flen;
}

////
// field_ok: return true if state field is within valid range
////

bool FASTCALL LyraItem::field_ok(int field_num)
{
  if ((field_num < 0) || (field_num > 2)) {
    return false;
  }
  return true;
}

////
// format_index: return index into item format table of given format, or -1 if not found
////

int FASTCALL LyraItem::format_index(int format)
{
  // linear search
  for (int i = 0; i < item_formats_num; ++i) {
    if (item_formats[i].format == format) {
      return i;
    }
  }
  return -1;
}

////
// FunctionName: return name of function type, or NULL if not valid
////

const TCHAR* FASTCALL LyraItem::FunctionName(int function)
{
  if (!function_ok(function)) {
    return NULL;
  }
  return item_functions[function].name;
}

////
// FunctionSize: return size of function state data, or -1 if not valid
////

int FASTCALL LyraItem::FunctionSize(int function)
{
  if (!function_ok(function)) {
    return -1;
  }
  return item_functions[function].size;
}

////
// FunctionEntries: return # of valid entries for function, or -1 if not valid
////

int FASTCALL LyraItem::FunctionEntries(int function)
{
  if (!function_ok(function)) {
    return -1;
  }
  return item_functions[function].num_valid_fields;
}

////
// FunctionCreateByArt:can they be created by using certain arts
////

bool FASTCALL LyraItem::FunctionCreateByArt(int function)
{
  if (!function_ok(function)) {
    return false;
  }
  return item_functions[function].create_by_art;
}

////
// FunctionCreateByForge:can they be created by using forge talisman
////

bool FASTCALL LyraItem::FunctionCreateByForge(int function)
{
  if (!function_ok(function)) {
    return false;
  }
  return item_functions[function].create_by_forge;
}
  
////
// FunctionCreateByGM:can they be created by using a gm terminal
////

bool FASTCALL LyraItem::FunctionCreateByGM(int function)
{
  if (!function_ok(function)) {
    return false;
  }
  return item_functions[function].create_by_gm;
}

////
// FunctionAlwaysSendState:does item with this function always send its state
////

bool FASTCALL LyraItem::FunctionAlwaysSendState(int function)
{
  if (!function_ok(function)) {
    return false;
  }
  return item_functions[function].always_send_state;
}

////
// FunctionImmutable:do properties never change once created
////

bool FASTCALL LyraItem::FunctionImmutable(int function)
{
  if (!function_ok(function)) {
    return false;
  }
  return item_functions[function].immutable;
}

////
// FunctionChangeCharges: do properties other than charges never change
////

bool FASTCALL LyraItem::FunctionChangeCharges(int function)
{
  if (!function_ok(function)) {
    return false;
  }
  return item_functions[function].change_charges;
}

////
// FunctionLosable: can be dropped at dissolution time?
////

bool FASTCALL LyraItem::FunctionLosable(int function)
{
  if (!function_ok(function)) {
    return false;
  }
  return item_functions[function].losable;
}


////
//EntryName: return name of entry, or -1 if not valid
////

const TCHAR* FASTCALL LyraItem::EntryName(int function, int entry)
{
  if (!entry_ok(function, entry)) {
    return NULL;
  }
  return item_functions[function].fields[entry].name;
}

////
// EntrySize: return size (in bytes) of entry, or -1 if not valid
////

int FASTCALL LyraItem::EntrySize(int function, int entry)
{
  if (!entry_ok(function, entry)) {
    return -1;
  }
  return item_functions[function].fields[entry].size;
}

////
// EntryTranslation: return translation code for entry, or -1 if not valid
////

int FASTCALL LyraItem::EntryTranslation(int function, int entry)
{
  if (!entry_ok(function, entry)) {
    return -1;
  }
  return item_functions[function].fields[entry].translation_index;
}

////
// EntryIdentifiable: true if entry should be identifiable
////

bool FASTCALL LyraItem::EntryIdentifiable(int function, int entry)
{
  if (!entry_ok(function, entry)) {
    return false;
  }
  return item_functions[function].fields[entry].identifiable;
}

////
// EntryMinValue: minimum legal value
////

int FASTCALL LyraItem::EntryMinValue(int function, int entry)
{
  if (!entry_ok(function, entry)) {
    return -1;
  }
  return item_functions[function].fields[entry].min;
}

////
// EntryMaxValue: maximum legal value
////

int FASTCALL LyraItem::EntryMaxValue(int function, int entry)
{
  if (!entry_ok(function, entry)) {
    return -1;
  }
  return item_functions[function].fields[entry].max;
}

////
// function_ok: return true if given function is valid (within the table's range)
////

bool FASTCALL LyraItem::function_ok(int function)
{
  if ((function <= 0) || (function >= item_functions_num)) {
    return false;
  }
  return true;
}

////
// entry_ok: return true if given function and entry are valid
////

bool FASTCALL LyraItem::entry_ok(int function, int entry)
{
  if (!function_ok(function)) {
    return false;
  }
  if ((entry < 0) || (entry >= item_functions[function].num_valid_fields)) {
    return false;
  }
  return true;
}

////
// ConvertStateToNetwork - convert an arbitrary state structure
////

// conversion function -- to avoid memory access errors due to structures not being
// aligned correctly, copy the state data into a temporary structure, convert,
// and then copy back

template<class T>
void convert_hton(T x, void* ptr) {
  memcpy(&x, ptr, sizeof(T));
  x.hton();
  memcpy(ptr, &x, sizeof(T));
}

void FASTCALL LyraItem::ConvertStateToNetwork(void* ssptr)
{
  // switch on function, convert if necessary
  switch (StateFunction(ssptr)) {
  case WARD_FUNCTION:
    lyra_item_ward_t ward;
    convert_hton(ward, ssptr);
    break;
  case MISSILE_FUNCTION:
    lyra_item_missile_t missile;
    convert_hton(missile, ssptr);
    break;
  case AMULET_FUNCTION:
    lyra_item_amulet_t amulet;
    convert_hton(amulet, ssptr);
    break;
  case AREA_EFFECT_FUNCTION:
    lyra_item_area_effect_t area_effect;
    convert_hton(area_effect, ssptr);
    break;
  case ESSENCE_FUNCTION:
    lyra_item_essence_t essence;
    convert_hton(essence, ssptr);
    break;
  case SUPPORT_FUNCTION:
    lyra_item_support_t support;
    convert_hton(support, ssptr);
    break;
  case META_ESSENCE_FUNCTION:
    lyra_item_meta_essence_t meta_essence;
    convert_hton(meta_essence, ssptr);
    break;
  case SCROLL_FUNCTION:
    lyra_item_scroll_t scroll;
    convert_hton(scroll, ssptr);
    break;
  case TELEPORTER_FUNCTION:
    lyra_item_teleporter_t teleporter;
    convert_hton(teleporter, ssptr);
    break;
  case SUPPORT_TRAIN_FUNCTION:
    lyra_item_train_support_t train_support;
    convert_hton(train_support, ssptr);
    break;
  case GRATITUDE_FUNCTION:
	lyra_item_gratitude_t gratitude;
	convert_hton(gratitude, ssptr);
	break;
  case META_ESSENCE_NEXUS_FUNCTION:
	lyra_item_meta_essence_nexus_t nexus;
	convert_hton(nexus, ssptr);
	break;
  case PORTKEY_FUNCTION:
	  lyra_item_portkey_t portkey;
	  convert_hton(portkey, ssptr);
	  break;
  case NO_FUNCTION: // should be an error
  case EFFECT_PLAYER_FUNCTION:
  case CHANGE_STAT_FUNCTION:
  case ARMOR_FUNCTION:
  case NOTHING_FUNCTION:
  case COLLECTION_FUNCTION:
  case MAP_FUNCTION:
    // no conversion function for these types
    break;
  default:
    // error, unknown type
    //_tprintf(_T("LyraItem::ConvertToNetwork: Doh!\n"));
    break;
  }
}

////
// ConvertStateToHost - convert an arbitrary state structure
////

// conversion function -- to avoid memory access errors due to structures not being
// aligned correctly, copy the state data into a temporary structure, convert,
// and then copy back

template<class T>
void convert_ntoh(T x, void* ptr) {
  memcpy(&x, ptr, sizeof(T));
  x.ntoh();
  memcpy(ptr, &x, sizeof(T));
}

void FASTCALL LyraItem::ConvertStateToHost(void* ssptr)
{
  // switch on function, convert if necessary
  switch (StateFunction(ssptr)) {
  case WARD_FUNCTION:
    lyra_item_ward_t ward;
    convert_ntoh(ward, ssptr);
    break;
  case MISSILE_FUNCTION:
    lyra_item_missile_t missile;
    convert_ntoh(missile, ssptr);
    break;
  case AMULET_FUNCTION:
    lyra_item_amulet_t amulet;
    convert_ntoh(amulet, ssptr);
    break;
  case AREA_EFFECT_FUNCTION:
    lyra_item_area_effect_t area_effect;
    convert_ntoh(area_effect, ssptr);
    break;
  case ESSENCE_FUNCTION:
    lyra_item_essence_t essence;
    convert_ntoh(essence, ssptr);
    break;
  case SUPPORT_FUNCTION:
    lyra_item_support_t support;
    convert_ntoh(support, ssptr);
    break;
  case META_ESSENCE_FUNCTION:
    lyra_item_meta_essence_t meta_essence;
    convert_ntoh(meta_essence, ssptr);
    break;
  case SCROLL_FUNCTION:
    lyra_item_scroll_t scroll;
    convert_ntoh(scroll, ssptr);
    break;
  case TELEPORTER_FUNCTION:
    lyra_item_teleporter_t teleporter;
    convert_ntoh(teleporter, ssptr);
    break;
  case SUPPORT_TRAIN_FUNCTION:
    lyra_item_train_support_t train_support;
    convert_ntoh(train_support, ssptr);
    break;
  case GRATITUDE_FUNCTION:
	lyra_item_gratitude_t gratitude;
	convert_ntoh(gratitude, ssptr);
	break;
  case META_ESSENCE_NEXUS_FUNCTION:
	lyra_item_meta_essence_nexus_t nexus;
	convert_ntoh(nexus, ssptr);
	break;
  case PORTKEY_FUNCTION:
	  lyra_item_portkey_t portkey;
	  convert_ntoh(portkey, ssptr);
	  break;
  case NO_FUNCTION: // should be an error
  case EFFECT_PLAYER_FUNCTION:
  case CHANGE_STAT_FUNCTION:
  case ARMOR_FUNCTION:
  case NOTHING_FUNCTION:
  case COLLECTION_FUNCTION:
  case MAP_FUNCTION:
    // no conversion function for these types
    break;
  default:
    // error, unknown type
    //_tprintf(_T("LyraItem::ConvertToHost: Doh!\n"));
    break;
  }
}
