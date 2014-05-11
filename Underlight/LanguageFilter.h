// Header file for LanguageFilter.cpp

// Copyright Lyra LLC, 2001. All rights reserved. 

#ifndef INCL_LANGFILTER
#define INCL_LANGFILTER

#include "Central.h"
#include "SharedConstants.h"



//////////////////////////////////////////////////////////////////
// Function Prototypes

TCHAR* MareSpeak(TCHAR *in_buffer, int speechType);
TCHAR* AdultFilter(TCHAR *in_buffer);
TCHAR* BabbleFilter(TCHAR *in_buffer);
TCHAR* GruntFilter(TCHAR *in_buffer);

#endif
