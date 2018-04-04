// LmItemDefs.h  -*- C++ -*-
// $Id: LmItemDefs.h,v 1.35 1998-05-11 11:00:29-07 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item type/state definitions

#ifndef INCLUDED_LmItemNames
#define INCLUDED_LmItemNames

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

// item name definitions
// these are done as #define's instead of const's so the compiler won't complain

#define ITEM_WARD _T("Ward")
#define ITEM_STR _T("Strength")
#define ITEM_FRV  _T("From Vertex")
#define ITEM_TOV  _T("To Vertex")
#define ITEM_HIBIT _T("ID (Hi Bits)")
#define ITEM_LOBIT _T("ID (Lo Bits)")

#define ITEM_CHANGESTAT _T("Change Stat")
#define ITEM_STAT _T("Stat")
#define ITEM_MOD _T("Modifier")

#define ITEM_MISSILE _T("Missile")
#define ITEM_VELOCITY  _T("Velocity") 
#define ITEM_EFFECT  _T("Effect")
#define ITEM_DAMAGE _T("Damage")

#define ITEM_PEF _T("Player Effect")
#define ITEM_EFF _T("Effect")
#define ITEM_DUR _T("Duration")

#define ITEM_AMULET _T("Amulet")
#define ITEM_UNUSED _T("Unused")
#define ITEM_CODE _T("Code")

#define ITEM_AE _T("Area Effect")
#define ITEM_TIME _T("Time to Detonate")
#define ITEM_CASTER _T("Caster ID")
#define ITEM_DISTANCE _T("Distance")

#define ITEM_ESSENCE _T("Essence")
#define ITEM_ESSTYPE _T("Essence Type")
#define ITEM_WPNTYPE _T("Weapon Type")
#define ITEM_MAKER _T("Creator ID")

#define ITEM_ARMOR _T("Armor")
#define ITEM_CURRDURR _T("Curr Durability")
#define ITEM_MAXDURR _T("Max Durability")
#define ITEM_ABSORB _T("Absorption")

#define ITEM_RS _T("Ruler Support")

#define ITEM_GU _T("Guild use")
#define ITEM_TARGETHI _T("Target ID (hi)")
#define ITEM_TARGETLO _T("Target ID (low)")
#define ITEM_CREATORLO _T("Creator ID (low)")
#define ITEM_CREATORHI _T("Creator ID (hi)")

#define ITEM_PRIME _T("House Prime")
#define ITEM_NEXUS _T("Chaos Well")
#define ITEM_GID _T("Guild ID")
#define ITEM_STRHI _T("Strength (hi)")
#define ITEM_STRLO _T("Strength (low)")
#define ITEM_TRAPHI _T("Trapped Mares (hi)")
#define ITEM_TRAPLO _T("Trapped Mares (low)")
#define ITEM_STRCAP _T("Strength (cap)")
#define ITEM_ESSCAP _T("Essence (cap)")

#define ITEM_CODEX _T("Codex")
#define ITEM_ARTID _T("Art ID")
#define ITEM_NOTHING _T("Do Nothing")

#define ITEM_COLLECTION _T("Collection")
#define ITEM_MAP _T("Map")
#define ITEM_LEVELID _T("Level ID")

#define ITEM_TPORT _T("Teleporter")
#define ITEM_X _T("X")
#define ITEM_Y _T("Y")


#define ITEM_SUPPORT _T("Support Train")
#define ITEM_SUPPORTID _T("Art (Sphere=255)")
#define ITEM_ARTLVL _T("Art Level")

#define ITEM_GRATITUDE _T("Gratitude")
#define ITEM_MATURITY _T("Maturity Date")

#endif /* INCLUDED_LmItemNames */
