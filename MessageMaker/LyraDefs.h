
// LyraDefs.h  -*- C++ -*-
// $Id: LyraDefs.h,v 1.59 1998-04-17 16:38:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Global Lyra Definitions: base datatypes, constants, enumerations, macros, etc.

#ifndef INCLUDED_LyraDefs
#define INCLUDED_LyraDefs

////
//// Compiler/Language Workarounds
////

////
//// Logging New/Delete Operators (server only)
//// Pth included here for server to enable soft system call
//// mapping for use with user-level pth threads
////

#define MALLOC_CHECK 2

#include "tchar.h" // wide character support
#undef TCHAR
#ifdef _UNICODE
#define TCHAR wchar_t
#define TUCHAR wchar_t
#else
#define TCHAR char
#define TUCHAR unsigned char
#endif

#if !defined(WIN32)
// server
#include "LmNew.h"
#else
// client
#ifdef _DEBUG
#define USE_DEBUG // this is the server's constant to indicate debug mode
#endif
#define LmNEW(x)          new x
#define LmDELETE(x)       delete x
#define LmDELETEARRAY(x)  delete [] x
#ifdef UL_SERVER_WIN32
#include "LmNew.h"
#endif
#endif


////
//// Constants and Enumerations
////

// byte order (refer to them as ByteOrder::HOST, ByteOrder::NETWORK)
// (their values make them more meaningful when printed)
struct ByteOrder {
  enum {
    HOST = 'H',
    NETWORK = 'N'
  };
};

// global constants that don't belong anywhere else
struct Lyra {
  enum {
    GAME_VERSION = 68698,      // build / protocol version number - MUST be 5 digits!
    GAME_SUBVERSION = GAME_VERSION,

    ID_UNKNOWN = 0,         // invalid id
    PORT_UNKNOWN = 0,       // invalid port
    SOCK_UNKNOWN = -1,      // invalid socket
    AVATAR_UNKNOWN = 0,     // invalid avatar
	NUM_HOUSES = 8,

	CHALLENGE_LENGTH = 96, // length of challenge sent when a login is requested

    GM_DELTA = 2000000000,  // added to protocol version in GM build
    PMARE_DELTA = 1000000000,  // added to protocol version in GM build
    HIDDEN_DELTA = 100000,  // added to levelid for "hidden" players (not locatable)

    SERVERNAME_MAX = 16,    // maximum server name length (multiple of 4)
    PLAYERNAME_MAX = 16,    // maximum player name length (multiple of 4)
    PASSWORD_MAX = 12,      // maximum password length (multiple of 4)
    EMAIL_MAX = 40,         // maximum email address length (multiple of 4)
    INVENTORY_MAX = 50,     // maximum number of items in inventory
    MAX_XP = 1000000000,    // maximum XP for a player (1 billion)

    KNIGHT_XP_POOL = 30000, // amount of XP in knight's XP pool
    RULER_XP_POOL = 40000,  // amount of XP in ruler's XP pool
	QUEST_XP_POOL = 5000,	// amount of XP in teacher/knight/ruler's Quest XP pool
	PP_PER_DAY		  = 4,  // base personality points to grant per day for everyone
	PP_PER_DAY_TEACHER= 6,  // base personality points to grant per day for teachers
    PP_PER_DAY_ADMIN = 99, // base personality points to grant per day for GMs

    MAX_SPEECHLEN = 512,    // maximum length of speech text
    MAX_ITEMDESC = 512,     // maximum length of an item's description
    MAX_AVATARDESC = 512,   // max length of avatar description

	MAX_LEVELS = 64,        // max # of levels
    MAX_LEVELROOMS = 64,   // maximum number of rooms in a level
    ROOMPEOPLE_MAX = 100,   // maximum number of people in a room
    MAX_ROOMITEMS = 64,     // maximum number of items in a room
    MAX_LEVELPEOPLE = 1000, // abs max number of people in a level

    MAX_GROUP = 6,          // maximum number of people in a local group
    MAX_OTHERPEOPLE = 12,   // maximum number of other neighbors
    MAX_PARTYSIZE = 4,      // maximum number of people in a party (< MAX_GROUP)

    ROOMNAME_MAX = 32,      // maximum room name length
    LEVELNAME_MAX = 64,     // maximum level name length
	ROOMDESC_MAX = 1024,    // maximum room description length

	MAX_CLOSING_GAMEDS = 1,  // number of gamed's that can be closing at one time
	IP_LENGTH		  = 64,

	MAX_DB_QUERY = 4096,

	MAX_DB_QUERY_ACK = 4096,

    GOAL_SUMMARY_LENGTH = 64,
	QUEST_KEYWORDS_LENGTH = 64,
    MAX_GOAL_LENGTH = 2048,
    MAX_REPORT_LENGTH  = 1024,
    MAX_SIMUL_GOALS = 512,
    MAX_ACCEPTS = 200,      // abs max # of people who can accept a goal
    MAX_GOAL_LIFE = 30,	    // max life of goal in days
	MAX_QUEST_LIFE = 30,	// max life of quest in days
    MAX_ACTIVE_GOALS = 8,   // max # of goals a player can have as active
    MAX_AWARDXP = 100000,   // maximum amount of XP that can be awarded in a report

    MSG_NONE = 0,           // "unknown" message type
    MSG_MIN = 0,            // minimum message number
    RMSG_MIN = 1000,        // start of room server messages
    RMSG_MAX = 1999,
    GMSG_MIN = 2000,        // start of game server messages
    GMSG_MAX = 2999,
    SMSG_MIN = 3000,        // start of server-only messages
    SMSG_MAX = 3999,
    AMSG_MIN = 4000,        // start of agent server messages
    AMSG_MAX = 4999,
    MSG_MAX = 5000,         // maximum message number

    MSG_MAX_SIZE = 16384,   // maximum allowable message size

    EXIT_OK = 0,            // normal exit status on shutdown
	EXIT_CLOSED = 1,		// normal exit due to closing from # of logins
    EXIT_ARGS = 100,        // status: args incorrect
    EXIT_INIT = 101,        // status: could not initialize
    EXIT_EXEC = 102         // status: could not execute
  };
};


const int BOG_PRICE = (int)1;
const int AGO_PRICE = (int)2;
const int SHAM_PRICE = (int)3;

////
//// Base Data Types
////

// generic id type
typedef unsigned int lyra_id_t;
typedef unsigned int realmid_t;  // backwards compatibility

////
//// Macros
////

// everyone uses these, right? (should use C++ template functions)
#ifndef MIN
#define MIN(x,y)  ((x)>(y)?(y):(x))
#endif
#ifndef MAX
#define MAX(x,y)  ((x)>(y)?(x):(y))
#endif
#ifndef SQUARE
#define SQUARE(x) ((x)*(x))
#endif

// truncate a string of a given size (ie. make sure it has a trailing \0)
// (ensure that the string buffer is large enough, or bad things will happen)
#ifndef TRUNC
#define TRUNC(str, len)  ((str)[(len)-1] = '\0')
#endif

// network/host byte conversion
#ifndef NTOHS
#define NTOHS(x)  (x) = ntohs((x))
#define HTONS(x)  (x) = htons((x))
#define NTOHL(x)  (x) = ntohl((x))
#define HTONL(x)  (x) = htonl((x))
#endif

// used in Dump() functions to print indentation level
#ifndef INDENT
#define INDENT(n,f)  { if ((n) > 0) for (int XiY = 0; XiY < 2*(n); ++XiY)_fputtc(' ', f); }
#endif

// __fastcall macro for Win32
#ifdef WIN32
#define FASTCALL __fastcall
#else
#define FASTCALL /* nothing */
#endif

// define INLINE macro depending on if USE_INLINE is defined or not
#ifdef USE_INLINE
#define INLINE inline
#else
#define INLINE /* nothing */
#endif

// check if a given integer is inside a given range, [min,max] (inclusive)
#ifndef RANGE_OK
#define RANGE_OK(x,min,max) (((x) >= (min)) && ((x) <= (max)))
#endif

// check an array index, index in [min,max)
#ifndef INDEX_OK
#define INDEX_OK(index,min,max) RANGE_OK(index,min,(max-1))
#endif

// define a variable called "method" with the current method name
#ifndef DEFMETHOD
#define DEFMETHOD(Class,Method) static const TCHAR* method = #Class "::" #Method ;
#endif

// on Linux, include pth.h to use soft system call mapping for user-level threads
// on Win32, alias all of pth_ types and calls to use pthread_ 
#if !defined(WIN32)
#include "pth.h"
#else
#ifdef UL_SERVER_WIN32
typedef int pid_t;
#define UL_SERVER_REGISTRY_KEY "Software\\Lyra\\Server"
#define SERVER_SHUTDOWN_STRING "SHUTDOWN UNDERLIGHT SERVERS"
#include "pthread.h"
#include "Windows.h"
#endif
#endif


////
//// Inline Functions
////

#endif /* INCLUDED_LyraDefs */
