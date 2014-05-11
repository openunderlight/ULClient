// Header file for Room Server class

// Copyright Lyra LLC, 1996. All rights reserved. 
// Last Modified: Brent Phillips, 4/8/96

#ifndef UNIXINC
#define UNIXINC

#define _USE_32BIT_TIME_T
#ifdef assert
#undef assert
#endif

#define assert(x) /* nothing */
#define err_print(x,y) /* nothing*/

typedef unsigned short ushort;
typedef int	pid_t;
#endif
 