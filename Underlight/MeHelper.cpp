#include "MeHelper.h"

// returns the mouse event saved here.
    MouseEvent MeHelper::GetME() {
        return me;
    }

    //sets mouseevent
    void  MeHelper::SetME(MouseEvent MEIN)
    {
        //check for movements above or equal 3
        if (MEIN.GetPosX() >= 3 || MEIN.GetPosX() <= 3 || MEIN.GetPosY() >= 3 || MEIN.GetPosY() <= 3)
        {
            //yay passed, so save it.
            me = MEIN;
        }
        else
        {
            //reset to zero
            me.SetPosX(0);
            me.SetPosY(0);
        }

    }



