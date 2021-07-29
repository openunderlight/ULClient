#pragma once
#include ".\Mouse\MouseClass.h"

//mouse event helper
class MeHelper {
public:
    static MouseEvent GetME(); //returns the event currently held
    static void SetME(MouseEvent MEIN); //sets event
private:
   static inline MouseEvent me; // holds event static, and inline:)
};