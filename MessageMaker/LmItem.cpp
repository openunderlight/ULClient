// LmItem.cpp  -*- C++ -*-
// $Id: LmItem.cpp,v 1.30 1997-12-12 17:02:23-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmItem.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock2.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <string.h>

#include "LmItem.h"
#include "LyraDefs.h"
#include "LmItemDefs.h"

#ifndef USE_INLINE
#include "LmItem.i"
#endif

// default instance
const LmItem LmItem::DEFAULT_INSTANCE;

////
// constructor
////

LmItem::LmItem()
{
  Init(LmItemHdr::DEFAULT_INSTANCE, _T("unnamed"), 0, 0, 0);
}

////
// Init: initialize item header, state
////

void LmItem::Init(const LmItemHdr& hdr, const TCHAR* name, int s1, int s2, int s3)
{
  SetHeader(hdr);
  SetName(name);
  SetState(s1, s2, s3);
}

void LmItem::Init(int serial, int header_info, int h2, const TCHAR* name, int s1, int s2, int s3)
{
  hdr_.Init(header_info, h2, serial);
  SetName(name);
  SetState(s1, s2, s3);
}

////
// NumFields: loop through fields & check if valid, return total
////

int LmItem::NumFields() const
{
  int num_fields = 0;
  for (int i = 0; i < 3; ++i) {
    if (FieldIsValid(i)) {
      ++num_fields;
    }
  }
  return num_fields;
}


////
// StateField: return pointer to internal state field
////

void* LmItem::StateField(int field_num)
{
  return state_field(field_num);
}

const void* LmItem::StateField(int field_num) const
{
  return state_field(field_num);
}

////
// SetStateField: update internal state field; return 0 if successful, -1 if error
////

int LmItem::SetStateField(int field_num, const void* data, int data_length)
{
  int offset = LyraItem::FieldOffset(hdr_.StateFormat(), field_num);
  if (offset < 0) {
    return -1;
  }
  int len = LyraItem::FieldLength(hdr_.StateFormat(), field_num);
  if ((len < 0) || (data_length != len)) {
    return -1;
  }
  void* target = &(state_.b[1]) + offset;  // b[0] is # of charges
  //_tprintf(_T("target=%p, offset=%d, len=%d\n"), target, offset, len);
  memcpy(target, data, len);
  return 0;
}

////
// ConvertToNetwork
////

void LmItem::ConvertToNetwork()
{
  // convert state fields before header, so numfields is correct
  int nf = NumFields();
  for (int i = 0; i < nf; ++i) {
    LyraItem::ConvertStateToNetwork(StateField(i));
  }
  hdr_.ConvertToNetwork();
  // not converted: name
  // NOTE: state bytes are not used for conversion, since their re-ordering
  // hoses the state fields they span; they are used only to store item state
  // on the server
}

////
// ConvertToHost
////

void LmItem::ConvertToHost()
{
  // convert header before state fields, so numfields is correct
  hdr_.ConvertToHost();
  // convert state fields
  int nf = NumFields();
  for (int i = 0; i < nf; ++i) {
    LyraItem::ConvertStateToHost(StateField(i));
  }
  // not converted: name
  // NOTE: state bytes are not used for conversion, since their re-ordering
  // hoses the state fields they span; they are used only to store item state
  // on the server
}

////
// Parse: read from string; return 0 if successful, -1 otherwise
//   format: header state
////

int LmItem::Parse(const TCHAR* str)
{
  TCHAR str_hdr[80];
  TCHAR str_name[NAME_LENGTH * 2];
  unsigned s1, s2, s3;
  int rc;
  if ((rc = _stscanf(str, _T("%[^_]_%u.%u.%u_%s"), str_hdr, &s1, &s2, &s3, str_name)) != 5) {
    //_tprintf(_T("LmItem::Parse: '%s' -> (%d), '%s' %u %u %u\n"), str, rc, str_hdr, s1, s2, s3);
    return -1;
  }
  if (hdr_.Parse(str_hdr) < 0) {
    return -1;
  }
  SetName(str_name);
  SetState(s1, s2, s3);
  return 0;
}

////
// UnParse: write to string
////

void LmItem::UnParse(TCHAR* str, int /* strlen */) const
{
  TCHAR str_hdr[80];
  TCHAR str_name[NAME_LENGTH * 2];
  name_encode(str_name);
  hdr_.UnParse(str_hdr, sizeof(str_hdr));
 _stprintf(str, _T("%s_%u.%u.%u_%s"), str_hdr, State1(), State2(), State3(), str_name);
}

////
// SetName
////

void LmItem::SetName(const TCHAR* name)
{
  if (name) {
    name_decode(name);
  }
}

////
// Dump: print to FILE stream, on an output line of its own
////

#ifdef USE_DEBUG
void LmItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmItem[%p,%d] name='%s' state=(%u,%u,%u)=("), this, sizeof(LmItem),
	  Name(), State1(), State2(), State3());
 int i=0;
  for (i = 0; i < STATE_BYTES; ++i) {
   _ftprintf(f, _T("%d"), StateByte(i));
    if (i != (STATE_BYTES - 1)) {
     _ftprintf(f, _T(","));
    }
  }
 _ftprintf(f, _T(")>\n"));
  TCHAR str[80];
  UnParse(str, sizeof(str));
  INDENT(indent + 1, f);
 _ftprintf(f, _T("itemstr: %s\n"), str);
  // print out state in some more detail
  INDENT(indent + 1, f);
  int num_fields = NumFields();
 _ftprintf(f, _T("charges=%d artifact=%d fields=%d\n"), Charges(), IsArtifact(), num_fields);
  // loop through all state fields, dumping their contents
   
  for (i = 0; i < num_fields; ++i) {
    INDENT(indent + 1, f);
    int func = LyraItem::StateFunction(StateField(i));
   _ftprintf(f, _T("field %d: func(%d)="), i, func);
    switch (func) {
    case LyraItem::CHANGE_STAT_FUNCTION: {
      lyra_item_change_stat_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("CHANGE_STAT; stat=%d modifier=%d"), state.stat, state.modifier);
    }
    break;
    case LyraItem::ARMOR_FUNCTION: {
      lyra_item_armor_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("ARMOR; absorption=%d durability=%d/%d"),
	      state.absorption, state.curr_durability, state.max_durability);
    }
    break;
    case LyraItem::EFFECT_PLAYER_FUNCTION: {
      lyra_item_effect_player_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("EFFECT_PLAYER; effect=%d duration=%d"), state.effect, state.duration);
    }
    break;
    case LyraItem::MISSILE_FUNCTION: {
      lyra_item_missile_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("MISSILE; velocity=%d effect=%d damage=%d bitmap=%d"),
	      state.velocity, state.effect, state.damage, state.bitmap_id);
    }
    break;
    case LyraItem::WARD_FUNCTION: {
      lyra_item_ward_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("WARD; strength=%d from=%d to=%d playerid=%d"),
	      state.strength, state.from_vert, state.to_vert, state.player_id());
    }
    break;
    case LyraItem::AMULET_FUNCTION: {
      lyra_item_amulet_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("AMULET; strength=%d playerid=%d"), state.strength, state.player_id);
    }
    break;
    case LyraItem::ESSENCE_FUNCTION: {
      lyra_item_essence_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("ESSENCE; mare=%d strength=%d weapon=%d slaver=%d"),
	      state.mare_type, state.strength, state.weapon_type, state.slaver_id);
    }
    break;
    case LyraItem::SUPPORT_FUNCTION: {
      lyra_item_support_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("SUPPORT; token=%d guild=%d creator=%u target=%u"),
	      state.token_type(), state.guild_id(), state.creator_id(), state.target_id());
    }
    break;
    case LyraItem::META_ESSENCE_FUNCTION: {
      lyra_item_meta_essence_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("META_ESSENCE; guild=%d strength=%d mares=%d"),
	      state.guild_id, state.strength(), state.num_mares());
    }
    break;
    case LyraItem::SCROLL_FUNCTION: {
      lyra_item_scroll_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("SCROLL; creator=%u"),
	      state.creatorid());
    }
    break;
    case LyraItem::SUPPORT_TRAIN_FUNCTION: {
      lyra_item_train_support_t state;
      memcpy(&state, StateField(i), sizeof(state));
     _ftprintf(f, _T("SUPPORT TRAINING; art=%d level=%d creator=%u target=%u"),
	      state.art_id, state.art_level, state.creator_id(), state.target_id());
    }
    break;

    // TODO: (AREA_EFFECT_FUNCTION)
    default:
     _ftprintf(f, _T("(unknown)"));
      break;
    }
   _ftprintf(f, _T("\n"));
  }
  hdr_.Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// Dump1: print to FILE stream, in "shorthand" format, no trailing newline
////

#ifdef USE_DEBUG
void LmItem::Dump1(FILE* f) const
{
 _ftprintf(f, _T("["));
  hdr_.Dump1(f);
 _ftprintf(f, _T(", [%u.%u.%u]]"), State1(), State2(), State3());
}
#endif /* USE_DEBUG */

////
// name_encode: encode name into given string
////

void LmItem::name_encode(TCHAR* encoded_name) const
{
  // copy
 _tcsnccpy(encoded_name, name_, sizeof(name_));
  TRUNC(encoded_name, NAME_LENGTH);
  // encode
  for (int i = 0; i < NAME_LENGTH; ++i) {
    if (encoded_name[i] == ' ') {
      encoded_name[i] = SPACE_CHAR;
    }
  }
}

////
// name_decode: decode name, store in object's name field
////

void LmItem::name_decode(const TCHAR* encoded_name)
{
  // copy
 _tcsnccpy(name_, encoded_name, sizeof(name_));
  TRUNC(name_, NAME_LENGTH);
  // decode
  for (int i = 0; i < NAME_LENGTH; ++i) {
    if (name_[i] == SPACE_CHAR) {
      name_[i] = ' ';
    }
  }
}

////
// state_field
////

void* LmItem::state_field(int field_num) const
{
  int offset = LyraItem::FieldOffset(hdr_.StateFormat(), field_num);
  if (offset < 0) {
    return 0;
  }
  // byte 0 is # of charges
  return (void*) (&(state_.b[1]) + offset);
}
