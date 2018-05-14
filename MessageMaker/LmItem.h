// LmItem.h  -*- C++ -*-
// $Id: LmItem.h,v 1.36 1997-09-04 02:29:16-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item class

#ifndef INCLUDED_LmItem
#define INCLUDED_LmItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include "LyraDefs.h"
#include "LmItemHdr.h"
#include "LmItemDefs.h"

// forward decls

// item state structure

union lyra_itemstate_t {
  // this is needed to store the state values in database files; server-only use!
  struct {
    unsigned s1, s2, s3;
  } s;
  // this has the state bytes split into single-byte values
  // by convention: byte0 = # of charges, bytes 1-11 are format-specific
  unsigned char b[12];
};

// the class

class LmItem {

public:

  // constants
  enum {
    STATE_BYTES = 12,  // number of state bytes (same as above!)
    NAME_LENGTH = 20,  // length of item names
    SPACE_CHAR = '`',  // character to replace the space in item names

    DEFAULT_STATE = 0
  };

  // default instance
  static const LmItem DEFAULT_INSTANCE;

public:

  LmItem();

  // initializers
  void Init(const LmItemHdr& hdr, const TCHAR* name = _T(""), int s1 = 0, int s2 = 0, int s3 = 0);
  void Init(int serial, int header_info, int h2, const TCHAR* name = _T(""), int s1 = 0, int s2 = 0, int s3 = 0);

  // selectors
  const LmItemHdr& Header() const;
  int Charges() const;
  const TCHAR* Name() const;
  int NumFields() const;
  bool FieldIsValid(int field_num) const;
  const void* StateField(int field_num) const;
  int FieldLength(int field_num) const;
  bool IsArtifact() const;

  // low-level state information selectors
  int State1() const;
  int State2() const;
  int State3() const;
  int StateByte(int byte_num) const;

  // selectors returning non-const
  LmItemHdr& Header();
  TCHAR* Name();
  void* StateField(int field_num);

  // convenience selectors for the header object
  int Serial() const;
  int Flags() const;
  int Graphic() const;
  int Color1() const;
  int Color2() const;
  int StateFormat() const;
  bool FlagSet(int flag) const;

  // mutators
  void SetHeader(const LmItemHdr& hdr);
  void SetName(const TCHAR* name);
  void SetState(int state1, int state2, int state3);
  int SetStateField(int field_num, const void* data, int data_length);
  void SetCharges(int charges);

  // comparison
  bool Equals(const LmItem& item) const;

  // byteorder conversion
  void ConvertToNetwork();
  void ConvertToHost();

  // read from string, write to string
  int Parse(const TCHAR* str);
  void UnParse(TCHAR* str, int strlen) const;

  // print contents for debugging
  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  void* state_field(int fieldnum) const;
  void name_encode(TCHAR* encoded_name) const;
  void name_decode(const TCHAR* encoded_name);

  LmItemHdr hdr_;
  lyra_itemstate_t state_;
  TCHAR name_[NAME_LENGTH];

};

#ifdef USE_INLINE
#include "LmItem.i"
#endif

// inline functions

inline int operator==(const LmItem& left, const LmItem& right)
{
  // items are equivalent if their headers are equal
  return (left.Header() == right.Header());
}

#endif /* INCLUDED_LmItem */
