// cDS: Direct Sound Class Implementation

// This file is NOT shared with the main project!

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "cGameServer.h"
#include "cDSound.h"

extern cGameServer *gs;


/////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
cDSound::cDSound(BOOL enabled, BOOL reverse) : 
			reverse_stereo (reverse) {}

cDSound::~cDSound() {}

// we need to copy the sound propagation code here...
int cDSound::PlaySound(int id, float x, float y, bool propagate) {
	// we want to propagate sounds even if we can't hear it locally
	if (propagate && gs && gs->LoggedIntoLevel())
		gs->SetLastSound(id);//gs->SendPlayerMessage(0, RMsg_PlayerMsg::TRIGGER_SOUND, id, 0);

	return 0;

}

int cDSound::StopSound(int bufferNum) {return 0;}

void cDSound::CheckSounds(void) { return; }

void cDSound::ToggleSound(void) {return;}

void cDSound::ReleaseBuffer(int buffer) { return; }

int cDSound::CreateSoundBuffer(int id, WAVEFORMATEX &wave_format,int data_size, const void *data)
{
	return 0;
}

int cDSound::SetFormat(DWORD frequency, WORD bits_per_sample, WORD channels) { return 0;}

#ifdef DEBUG

void cDSound::Debug_CheckInvariants(int caller) {}

#endif