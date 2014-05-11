// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef ACTOR_INC
#define ACTOR_INC

#define MAX_BOX 256

#include "cActor.h"
#include "wbox.h"

class ActorBox
    {
    private:

      int BoxNos;
      BOX boxes[MAX_BOX];
	  cActor *acts[MAX_BOX];

    public:
      inline ActorBox() {BoxNos = 0;};

      inline void AddBox(int x, int y, int x2, int y2, cActor *actor)
         {
         boxes[BoxNos].SetBox(x,y,x2-x,y2-y);
         acts[BoxNos] = actor;
         BoxNos++;
         };

      inline cActor* FindBox(int x, int y)
         {
         int i;
         for ( i=0;i<BoxNos;i++ )
            {
            if (boxes[i].Contains(x,y)) return acts[i];
            }
         return NO_ACTOR;
         };

      inline void ClearBoxes(void) {BoxNos = 0;};
      inline void DumpBoxes(void) {};
			
		inline int GetBoxNos(void)	{ return BoxNos; };
		inline BOX* GetBoxes(void)  { return boxes; };
		inline cActor* GetActor(int i) { return acts[i]; };

    };
#endif
