// cDS: Direct Sound Class Implementation

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT
#include "Central.h"
#include "cDSound.h"
#include "cDDraw.h"
#include "cChat.h"
#include "cEffects.h"
#include "cPlayer.h"
#include "Options.h"
#include "cGameServer.h"
#include "4dx.h"
#include "cLevel.h"
#include "LoginOptions.h"
#include "Realm.h"
#include "Move.h"
#include "math.h"
#include "resource.h"


//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cEffects *effects;
extern options_t options;
extern cGameServer *gs;
extern cLevel *level;
extern cDDraw *cDD;
extern cPlayer *player;
extern cChat *display;

//////////////////////////////////////////////////////////////////
// Constants

const int MAX_SIMUL_INSTANCES = 8;

//////////////////////////////////////////////////////////////////
// Macros 

// For proper error handling for DirectSound 
#define TRY_DS(exp) { { HRESULT rval = exp; if (rval != DS_OK) { this->TraceErrorDS(rval, __LINE__); return rval; } } }

//////////////////////////////////////////////////////////////////
// Constants

const float ZERO_DISTANCE = 1500.0f;	// max distance until dB_RANGE attenuation
const float PAN_DISTANCE	= 100.0f;
const int dB_RANGE = -1000;			    	// max attenuation
const int PAN_dB_RANGE = -400;



/////////////////////////////////////////////////////////////////
// Class Defintion


static cDSound *timerDS;
static int timerID;
static int retry_attempts;

void CALLBACK InitRetryTimerProc( HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (timerDS)
	{
		if (display  && retry_attempts== 0) // if this is the first attempt at initialization
		{
			LoadString (hInstance, IDS_SOUND_FAILED_INIT, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message, false); // do this here rather than when the timer is started
		}												  // since display is not created at that time		

		int result = timerDS->InitDS(); // try re-initiailize
		if (result == DS_OK)   // initialzed OK 
			KillTimer(NULL,timerID); // stop timer.
		else if ( ++retry_attempts >= 10) // could not init after 10 times, give up
		{
			LoadString (hInstance, IDS_SOUND_INIT_ABORTED, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message, false); // tell user
		    KillTimer(NULL,timerID);
		}
	}
}




// Constructor
cDSound::cDSound(BOOL enabled, BOOL reverse) : 
			reverse_stereo (reverse)
{
    // Initialize class data members.
	pDirectSound = NULL;
	primary      = NULL;
	initialized  = FALSE;

	for (int i = 0; i < MAX_DUPLICATES; i++)
		duplicates[i] = NULL;

	sound_error_displayed = false;

	for (int i = 0; i < MAX_SOUNDS; i++)
	{
		sounds[i] = NULL;
		looping[i] = false;
		num_duplicates_playing[i] = 0; // # of these currently being played
	}

	for (int i = 0; i < MAX_DUPLICATES; i++)
	{
			duplicate_id[i] = 0;
			duplicates[i] = NULL;
	}


	// set looping sounds here
	looping[LyraSound::SCARE_LOOP] = true;
	looping[LyraSound::BLIND_LOOP] = true;
	looping[LyraSound::DEAFEN_LOOP] = true;

	if (enabled)
	{
		int result = this->InitDS();
		if (result != DS_OK) // intialization failed, set up timer to retry in a second or so
		{
			 timerDS = this;
			 timerID = SetTimer( NULL,0,1500,InitRetryTimerProc);
		}
	}

	for (int i=0; i<LyraSound::MAX_SOUND_ID; i++)
		propagate_sound[i] = true; // default is to propagate all sounds

	// here, list the sounds we do NOT want to propagate_sound, EVER :)
	propagate_sound[(int)LyraSound::EXPLODE] = false;
	propagate_sound[(int)LyraSound::LAUNCH] = false;
	propagate_sound[(int)LyraSound::ENTRY] = false;
	propagate_sound[(int)LyraSound::MESSAGE] = false;
	propagate_sound[(int)LyraSound::INTRO] = false;
	propagate_sound[(int)LyraSound::EXIT] = false;
	propagate_sound[(int)LyraSound::OTHER_STEP_1] = false;
	propagate_sound[(int)LyraSound::OTHER_STEP_2] = false;
	propagate_sound[(int)LyraSound::PLAYER_STEP_1] = false;
	propagate_sound[(int)LyraSound::PLAYER_STEP_2] = false;
	propagate_sound[(int)LyraSound::MENU_CLICK] = false;
	propagate_sound[(int)LyraSound::FIREBALL_HIT_WALL] = false;
	propagate_sound[(int)LyraSound::JUMPLANDING] = false;
	propagate_sound[(int)LyraSound::MESSAGE_SENT] = false;
	propagate_sound[(int)LyraSound::MESSAGE_ALERT] = false;
	propagate_sound[(int)LyraSound::REJECTED] = false;
	propagate_sound[(int)LyraSound::WATER_STEP_1] = false;
	propagate_sound[(int)LyraSound::WATER_STEP_2] = false;
//	propagate_sound[(int)LyraSound::] = false;

}

///////////////////////////////////////////////////////////
// cDSound::~cDSound()
//
// Destructor.
///////////////////////////////////////////////////////////
cDSound::~cDSound()
{
	this->DeInit();
}

void cDSound::DeInit(void)
{
	// release duplicates
	for (int i = 0; i < MAX_DUPLICATES; i++)
		if (duplicates[i]) 
		{
			duplicates[i]->Release();
			duplicates[i] = NULL;
		}

	for (int i = 0; i < MAX_SOUNDS; i++)
		if (sounds[i])
		{
			sounds[i]->Release();
			sounds[i] = NULL;
		}

	// release primary buffer
	if (primary)
  {
		primary->Release();
		primary = NULL;
	}

  // release the DirectSound object.
  if (pDirectSound)
  {
    pDirectSound->Release();
		pDirectSound = NULL;
	}
}

// Create DS object, primary buffer, and init array
// of secondary buffers and duplicate buffers
int cDSound::InitDS()
{
    DSBUFFERDESC dsBufferDesc;

    // Create the main DirectSound object.
  	TRY_DS(DirectSoundCreate(NULL, &pDirectSound, NULL));

    // Set the priority level
	TRY_DS(pDirectSound->SetCooperativeLevel(cDD->Hwnd_Main(), DSSCL_PRIORITY));
	//if (options.exclusive)
	//{
	//	TRY_DS(pDirectSound->SetCooperativeLevel(cDD->Hwnd_Main(), DSSCL_EXCLUSIVE));
	//}
	//else
	//{
	//	TRY_DS(pDirectSound->SetCooperativeLevel(cDD->Hwnd_Main(), DSSCL_PRIORITY));
	//}

		// Create the primary buffer
	memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
	dsBufferDesc.dwSize         =  sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags        =  DSBCAPS_PRIMARYBUFFER;
	dsBufferDesc.dwBufferBytes  =  0;
	dsBufferDesc.lpwfxFormat    =  NULL;
	TRY_DS(pDirectSound->CreateSoundBuffer(&dsBufferDesc,&primary, NULL));

	this->SetFormat(11025,8,1);
	initialized = TRUE;

	return DS_OK;
}

// set primary buffer format
int	cDSound::SetFormat(DWORD frequency, WORD bits_per_sample, WORD channels)
{
	WAVEFORMATEX format;

	if (!primary)
		return 0;

	// Set to proper format
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = channels;
    format.nSamplesPerSec = frequency;
    format.nAvgBytesPerSec = frequency*channels;
    format.nBlockAlign = (bits_per_sample/8)*channels;
    format.wBitsPerSample = bits_per_sample;
    format.cbSize = 0;

	TRY_DS(primary->SetFormat(&format));

	// compact while we're at it
	TRY_DS(pDirectSound->Compact());

	return 1;
}

// called whenever sounds might have just been deactivated;
// if so, loop through and stop all active sounds
void cDSound::CheckSounds(void)
{
	if (!options.sound_active)
		for (int i = 0; i < MAX_SOUNDS; i++)
			this->StopSound(i);

	return;
}

void cDSound::ToggleSound(void)
{
	// check for valid object
	if ((pDirectSound == NULL) || (!options.sound))
        return;

	options.sound_active = !options.sound_active;

	if (!options.sound_active) 
	{
		LoadString (hInstance, IDS_SOUND_OFF, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message, false);
		this->CheckSounds();
	}
	else 
	{
		LoadString (hInstance, IDS_SOUND_ON, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message, false);
	}
	SaveInGameRegistryOptionValues();
}

// Release a buffer
void cDSound::ReleaseBuffer(int id)
{
	if (pDirectSound == NULL)
		return;

	if (sounds[id])
		sounds[id]->Release();	

	sounds[id] = NULL;
}



///////////////////////////////////////////////////////////
// cDSound::PlaySound()
//
//  plays the sound for the given sound effect id 
///////////////////////////////////////////////////////////
int cDSound::PlaySound(int id, float x, float y, bool propagate) //, int volume)
{
	LPDIRECTSOUNDBUFFER pSoundBuffer;
	DWORD	bufferStatus;
	float dx, dy;
	long pan, distance_attenuation = -1;
	float m1, m2, xi, yi, b1, b2, distance;
	float px; 
	float py; 
	int   pa, flags, sec;
	int options_attenuation = 1;
	int volume_mod;

	// we want to propagate sounds even if we can't hear it locally
	if (propagate && gs && gs->LoggedIntoLevel() && 
		!player->Avatar().Hidden() && propagate_sound[id])
		gs->SetLastSound(id);
		//gs->SendPlayerMessage(0, RMsg_PlayerMsg::TRIGGER_SOUND, id, 0);

	// check for valid object
	if ((pDirectSound == NULL) || (!options.sound_active) || (!options.sound) || 
		((player && (player->flags & ACTOR_DEAFENED)) && (id !=LyraSound::DEAFEN_LOOP)))
	    return 0;

    if (CleanupDuplicates() != 0)   // could be called in a timer callback,task etc.
		return 0;

	if ((x != -100000.0f) && gs && gs->LoggedIntoLevel())
	{ // ensure sound is in the same room as us
		sec = FindSector(x, y, player->sector, false);
		if ((sec == DEAD_SECTOR) || ((unsigned int)level->Sectors[sec]->room != player->Room()))
			return 0; // in another room, ignore
		if ((x == DEAD_X) && (y == DEAD_Y))
			return 0; // dead position, don't play sound
	}

	if (!sounds[id]) // sound effect not loaded
	{
		effects->LoadSoundEffect(id);
		if (!sounds[id]) // problem loading, or unknown sound
			return 0;
	}

    // Get a pointer to the requested buffer.
    pSoundBuffer = sounds[id];
	
	// check if currently playing
	TRY_DS(pSoundBuffer->GetStatus(&bufferStatus));

        //  if we lost the buffer, get it back
    if( bufferStatus & DSBSTATUS_BUFFERLOST ) {
        TRY_DS(pSoundBuffer->Restore());
    }


	if (bufferStatus & DSBSTATUS_PLAYING) // if this sound is already playing
	{
		if (looping[id])
			return 0; // do nothing if a looping sound is already playing
		// create its duplicate and use that to play
		if (num_duplicates_playing[id] == MAX_SIMUL_INSTANCES)
			return 0; // already too many of this sound playing
		int i = 0;
		for (i=0; i< MAX_DUPLICATES; i++)   
		{
			if (!duplicates[i])  //if duplicate buffer is available
			{
				TRY_DS(pDirectSound->DuplicateSoundBuffer(pSoundBuffer, &duplicates[i]));
			    pSoundBuffer = duplicates[i];
				num_duplicates_playing[id]++;
				duplicate_id[i] = id;
				//_tprintf("using duplicate sound buf %d\n",i);
				break;						
			}
		}
		if ( i == MAX_DUPLICATES)  // if  no more duplicate buffers available.
			return 0;                // simply do not play it
	}
	else


	// distance_attenuation
	if (x != -100000.0f) {
			
		// calculations
		px = player->x;
	    py = player->y;
	    pa = player->angle;
		
		//_tprintf("x, y, angle, TANTABLE %f, %f, %d, %f\n", px, py, pa, TanTable[pa]);
		m1 = TanTable[pa];
		if (m1 == 0)	m1 = 0.000001f;
		m2 = -1/m1;
		b1 = py - m1*px;
		b2 = y  - m2*x;
		xi = (b2-b1)/(m1-m2);
		yi = m1*xi + b1;
		
		// set volume
		dx = (px - x);
		dy = (py - y);
		distance = (float)sqrt((dx*dx + dy*dy))/ZERO_DISTANCE;
		if (distance > 1)	distance = 1.0f;				// max distance_attenuation
		if (distance < 0)	distance = 0.0f;				// full sound
		distance_attenuation = (long)(dB_RANGE*distance);				// distance_attenuation rate:  linear

		// adding in volume is not mathematically correct, but close enough
				
		// set pan
		dx = x - xi;
		dy = y - yi;
		distance = (float)sqrt(dx*dx + dy*dy)/PAN_DISTANCE;
		if (distance > 1)	distance = 1.0f;
		if (distance < 0)	distance = 0.0f;
		pan = (long)(PAN_dB_RANGE*distance);						// pan
		if (m2 < 0) {		// determine side to attenuate: check right conditions
			if ( x<xi && y>yi && pa<256 )
				pan *= -1;
			else if ( x>xi && y<yi && pa>511)
				pan *= -1;
		}
		else { // m2>0
			if ( x>xi && y>yi && pa>767 )
				pan *= -1;
			else if ( x<xi && y<yi && pa<512)
				pan *= -1;
		}
		if (reverse_stereo)
		{
			TRY_DS(pSoundBuffer->SetPan(-pan));
		}
		else
		{
			TRY_DS(pSoundBuffer->SetPan(pan));
		}
	}

	options_attenuation = (max_volume - options.effects_volume)*(-DSBVOLUME_MIN/50); // attentuation factor
	if (options_attenuation == 0)
		options_attenuation = 1;
	if (distance_attenuation == 0)
		distance_attenuation = -1;

	volume_mod = distance_attenuation*options_attenuation;
	if (volume_mod < DSBVOLUME_MIN)
		volume_mod = DSBVOLUME_MIN;

	TRY_DS(pSoundBuffer->SetVolume(volume_mod));

	// Make sure the buffer is set to play from the beginning.
	TRY_DS(pSoundBuffer->SetCurrentPosition(0));
	
  // Play the sound. 
	if (looping[id])
		flags = DSBPLAY_LOOPING;
	else
		flags = 0;
	TRY_DS(pSoundBuffer->Play(0, 0, flags));

  return 0;
}

///////////////////////////////////////////////////////////
// cDSound::StopSound()
//
// This member function stops the sound stored in the given
// buffer, and any duplicates that are playing the same sound
///////////////////////////////////////////////////////////
int cDSound::StopSound(int id)
{
	LPDIRECTSOUNDBUFFER pSoundBuffer = NULL;
	DWORD status;
	bool  clean_up = false;

	if ((pDirectSound == NULL) || (!sounds[id]))
	    return 0;

	TRY_DS(sounds[id]->GetStatus(&status));
	if (status & DSBSTATUS_PLAYING)  // if sound is playing...
	{
		TRY_DS(sounds[id]->Stop());
	}


	for (int i=0; i< MAX_DUPLICATES; i++)
	{ // stop all duplicates of this sound
		if ((duplicate_id[i] == id) && duplicates[i])
		{
			TRY_DS(duplicates[i]->GetStatus(&status));
			if (status & DSBSTATUS_PLAYING)
			{
				TRY_DS(duplicates[i]->Stop());
				clean_up = true;
			}
		}
	}
	if (clean_up)
		this->CleanupDuplicates();

	return 0;
}


///////////////////////////////////////////////////////////
// cDSound::CreateSoundBuffer(...)
// Creates a direct sound buffer for the given id using the given wave format and sound data.

int cDSound::CreateSoundBuffer(int id, WAVEFORMATEX &wave_format, int data_size, const void *data)
{
    DSBUFFERDESC dsBufferDesc;
    LPVOID pSoundBlock1;
    DWORD bytesSoundBlock1;
  

    if (sounds[id]) // buffer is already loaded
		return 0;

	// Initialize the DSBUFFERDESC structure.
	memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
	dsBufferDesc.dwSize         =  sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags        =  DSBCAPS_STATIC | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsBufferDesc.dwBufferBytes  =  data_size;
	dsBufferDesc.lpwfxFormat    =  &wave_format;

	// Create the secondary sound buffer.
	TRY_DS(pDirectSound->CreateSoundBuffer(&dsBufferDesc,&(sounds[id]), NULL));
	
	// Lock the buffer.
	TRY_DS(sounds[id]->Lock(0, data_size,	&pSoundBlock1, &bytesSoundBlock1,	NULL, NULL, 0));

	// Copy the data into the buffer.
	memcpy((void*)pSoundBlock1, data, data_size);

	// Unlock the buffer.
	TRY_DS(sounds[id]->Unlock(pSoundBlock1, bytesSoundBlock1, NULL, 0));
  
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Frees up duplicate sound buffers that have stopped playing for re-use;

int cDSound::CleanupDuplicates(void)
{
	for (int i=0; i< MAX_DUPLICATES; i++)
	{
		if (duplicates[i])
		{
			DWORD status;
			TRY_DS(duplicates[i]->GetStatus(&status));
			if (!(status & DSBSTATUS_PLAYING))  // if sound has stopped playing
			{
				duplicates[i]->Release();
				duplicates[i] = NULL;
				num_duplicates_playing[duplicate_id[i]]--;
				duplicate_id[i] = 0;
				//_tprintf("free dup sound buffer %d\n",i);
			}
		}
	}
	return 0;
}


void cDSound::TraceErrorDS(HRESULT hErr, int nLine)
{       
	TCHAR dserr[DEFAULT_MESSAGE_SIZE];

	switch (hErr)
	{
		case DSERR_ALLOCATED :LoadString (hInstance, IDS_DSERR_ALLOCATED, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_CONTROLUNAVAIL :LoadString (hInstance, IDS_DSERR_CONTROLUNAVAIL, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_INVALIDPARAM :LoadString (hInstance, IDS_DSERR_INVALIDPARAM, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_INVALIDCALL :LoadString (hInstance, IDS_DSERR_INVALIDCALL, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_GENERIC :LoadString (hInstance, IDS_DSERR_GENERIC, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_PRIOLEVELNEEDED :LoadString (hInstance, IDS_DSERR_PRIOLEVELNEEDED, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_OUTOFMEMORY :LoadString (hInstance, IDS_DSERR_OUTOFMEMORY, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_BADFORMAT :LoadString (hInstance, IDS_DSERR_BADFORMAT, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_UNSUPPORTED :LoadString (hInstance, IDS_DSERR_UNSUPPORTED, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_NODRIVER :LoadString (hInstance, IDS_DSERR_NODRIVER, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_ALREADYINITIALIZED :LoadString (hInstance, IDS_DSERR_ALREADYINITIALIZED, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_NOAGGREGATION :LoadString (hInstance, IDS_DSERR_NOAGGREGATION, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_BUFFERLOST :LoadString (hInstance, IDS_DSERR_BUFFERLOST, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_OTHERAPPHASPRIO :LoadString (hInstance, IDS_DSERR_OTHERAPPHASPRIO, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
		case DSERR_UNINITIALIZED :LoadString (hInstance, IDS_DSERR_UNINITIALIZED, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;

		 default :LoadString (hInstance, IDS_UNKNOWN_ERROR, temp_message, sizeof(temp_message)); _stprintf(dserr, temp_message); break;
	}
	//ErrAndExit(GENERIC_ERR, dserr, nLine, sFile);
#if defined (UL_DEBUG) || defined (GAMEMASTER)
	Warn(dserr,nLine,_T(__FILE__));
#endif

	if (display && !sound_error_displayed)
	{
		LoadString (hInstance, IDS_SOUND_ERROR, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		sound_error_displayed = true;
	}
	this->DeInit();
}


// Check invariants

#ifdef CHECK_INVARIANTS
void cDSound::CheckInvariants(int line, TCHAR *file)
{

}
#endif

