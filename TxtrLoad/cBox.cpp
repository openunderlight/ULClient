// Copyright Lyra LLC, 1996. All rights reserved. 

//************************************************************************
//** BOX                                                                **
//************************************************************************
#include "wbox.h"
#define  MAX(a,b)   		((a)>(b)?(a):(b))
#define  MIN(a,b)   		((a)<(b)?(a):(b))

BOX::BOX()
   {
   SetBox(0,0,0,0);
   }

BOX::BOX(short bx, short by, short bw, short bh)
   {
   SetBox(bx,by,bw,bh);
	}

BOX::~BOX()
   {
   return;   // merely for your convenience.
   }

void
BOX::SetBox(short bx, short by, short bw, short bh)
   {
   x = bx;
   y = by;
   w = bw;
   h = bh;
   x2 = x+w;
   y2 = y+h;
   }

short
BOX::Contains(short px, short py)
   {
   if ((px >= x) && (px <= w+x) && (py >= y) &&(py <= h+y)) return(1);
	return(0);
   }

void
BOX::Union(BOX &bx1,BOX &bx2)
   {
   x=MAX(bx1.x,bx2.x);
   y=MAX(bx1.y,bx2.y);

	w = MIN(bx1.x2,bx2.x2)-x;
	h = MIN(bx1.y2,bx2.y2)-y;

	if ( w > 0 && h > 0 ) SetBox(x,y,w,h);
	else                  SetBox(0,0,0,0);
   }
