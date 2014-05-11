// Copyright Lyra LLC, 1997. All rights reserved. 

///////////////////////////////////////////////////////////
// CWAVE.H: Header file for the WAVE class.
///////////////////////////////////////////////////////////

#ifndef CWAVE_H
#define CWAVE_H

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

///////////////////////////////////////////////////////////////////
// Constants

///////////////////////////////////////////////////////////////////
// Structures

//////////////////////////////////////////////////////////////////
// External Function Prototypes

//////////////////////////////////////////////////////////////////
// Class Definition

class cWave 
{
protected:
    DWORD m_waveSize;
    BOOL m_waveOK;
    char* m_pWave;
    WAVEFORMATEX m_waveFormatEx;

public:
    cWave(char* fileName);
    ~cWave();

    DWORD GetWaveSize();
    LPWAVEFORMATEX GetWaveFormatPtr();
    char* GetWaveDataPtr();
    BOOL WaveOK();

protected:
    BOOL LoadWaveFile(char* fileName);

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cWave(const cWave& x);
	cWave& operator=(const cWave& x);

};

#endif

