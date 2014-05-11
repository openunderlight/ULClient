// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef INCL_BOX
#define INCL_BOX
//***********************************************************************
//* Box Class                                                           *
//***********************************************************************
class BOX
   {
   public:
      BOX();
      BOX(short bx,short by,short bw,short bh);
     ~BOX();
      void Union(BOX &bx1,BOX &bx2);
      void SetBox(short bx,short by,short bw,short bh);
      short Contains(short x,short y);
      short x,y,w,h,x2,y2;
      // later -- Union. Returns back a box that is the union of two boxes.
   };
#endif
