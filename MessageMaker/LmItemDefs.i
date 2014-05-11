// LmItemDefs.i  -*- C++ -*-
// $Id: LmItemDefs.i,v 1.5 1997-08-29 18:04:21-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

INLINE int FASTCALL LyraItem::NumItemFormats()
{
  return item_formats_num;
}

INLINE int FASTCALL LyraItem::NumItemFunctions()
{
  return item_functions_num;
}

INLINE int FASTCALL LyraItem::StateFunction(const void* ssptr)
{
  // first byte is function
  return ssptr ? *((char*) ssptr) : NO_FUNCTION;
}
