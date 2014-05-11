// cDS: Direct Sound Class Implementation
// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT
#include "Central.h"
#include "cBackgroundMusic.h"
#include "cDDraw.h"
#include "cChat.h"
#include "cEffects.h"
#include "cPlayer.h"
#include "Options.h"
#include "cGameServer.h"
#include "4dx.h"
#include "Dialogs.h"
#include "Interface.h"
#include "cLevel.h"
#include "LoginOptions.h"
#include "Realm.h"
#include "Move.h"
#include "math.h"
#include "utils.h"
#include "resource.h"

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <math.h>   
#include <fcntl.h>
#include <process.h>


//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cEffects *effects;
extern options_t options;
extern cGameServer *gs;
extern cLevel *level;
extern cDDraw *cDD;
extern cChat *display;

static HANDLE hCheckEvent; // can't be a class member since the thread function needs to use it


//////////////////////////////////////////////////////////////////
// Constants

const int OGG_FRAME_SIZE = 4096;
const int OGG_BUFFER_SIZE = OGG_FRAME_SIZE*32;
const int OGG_NUM_TRACKS = 22;

char pcmmusic[OGG_BUFFER_SIZE]; 

//////////////////////////////////////////////////////////////////
// Macros 

// For proper error handling for DirectSound 
#define TRY_BM(exp) { { HRESULT rval = exp; if (rval != DS_OK) { this->TraceErrorBM(rval, __LINE__); return rval; } } }

//////////////////////////////////////////////////////////////////
// Constants

const float ZERO_DISTANCE = 1500.0f;	// max distance until dB_RANGE attenuation
const float PAN_DISTANCE	= 100.0f;
const int dB_RANGE = -1000;			    	// max attenuation

static cBackgroundMusic *timerBM;
static int timerID;
static int retry_attempts;

void CALLBACK cBMInitRetryTimerProc( HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (timerBM)
	{
		if (display  && retry_attempts== 0) // if this is the first attempt at initialization
		{
			LoadString (hInstance, IDS_SOUND_FAILED_INIT, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message, false); // do this here rather than when the timer is started
		}												  // since display is not created at that time		

		int result = timerBM->InitBM(); // try re-initiailize
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


/////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
cBackgroundMusic::cBackgroundMusic(BOOL enabled) 
{
    // Initialize class data members.
	pDirectSound = NULL;
	primary      = NULL;
	pPlayingBuffer = NULL;
	initialized  = FALSE;
	vorbis_initialized = FALSE;
	playing_first_half = true;
	hCheckEvent = CreateEvent(NULL, FALSE, FALSE, _T("Check Music"));
    track_number = 0;
	thread_id = 0;
	playing_music = false;
	//hVorbisFile = NULL;

	pPlayingBuffer = NULL;

	if (enabled)
	{
		//try_initialize_again = true;
		int result = this->InitBM();
		//if ((result != DS_OK) && try_initialize_again) 
		if (result != DS_OK) 
		// intialization failed, set up timer to retry in a second or so
		{
			 timerBM = this;
			 timerID = SetTimer( NULL,0,1500,cBMInitRetryTimerProc);
		}
	}


}

///////////////////////////////////////////////////////////
// cBackgroundMusic::~cBackgroundMusic()
//
// Destructor.
///////////////////////////////////////////////////////////
cBackgroundMusic::~cBackgroundMusic()
{
	this->DeInit();
}

void cBackgroundMusic::DeInit(void)
{

  if (pPlayingBuffer)
	{
		pPlayingBuffer->Release();
		pPlayingBuffer = NULL;
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

   // clean up Ogg Vorbis
  if (vorbis_initialized) 
  {
	  this->CloseVorbis();
  }

//  if (hVorbisFile)
//  {
	  //FreeLibrary(hVorbisFile);
	  //hVorbisFile = NULL;
  //}

}

// Create DS object & primary buffer
int cBackgroundMusic::InitBM()
{ 
#if 0 // this code tries to dynamically link in the Vorbis DLL's, but it doesn't work...
	// first see if the Ogg Vorbis DLL's are there
	TCHAR dll_filename[256];
	
	LoadString (hInstance, IDS_VORBIS_DLL, dll_filename, sizeof(dll_filename));

	// first we try to open it, just to see if it's there, so we can put up
	// our own error message
	FILE* dll_fd = fopen(dll_filename, _T("rb"));

	if (dll_fd <= 0)
	{
		try_initialize_again = false;
		this->MusicError();
		return DSERR_GENERIC;
	}

	fclose(dll_fd);

	hVorbisFile = LoadLibrary(dll_filename);
	hOvOpen = (OVOPEN)GetProcAddress(hVorbisFile, _T("ov_open"));
	hOvClear = (OVCLEAR)GetProcAddress(hVorbisFile, _T("ov_clear"));
	hOvRead = (OVREAD)GetProcAddress(hVorbisFile, _T("ov_read"));

	if (!hVorbisFile || !hOvOpen || !hOvClear || !hOvRead)
	{
		try_initialize_again = false;
		this->MusicError();
		return DSERR_GENERIC;
	}

#endif 

	if (!options.sound)
		return DS_OK;

    DSBUFFERDESC dsBufferDesc;

    // Create the main DirectSound object.
  	TRY_BM(DirectSoundCreate(NULL, &pDirectSound, NULL));

    // Set the priority level
	TRY_BM(pDirectSound->SetCooperativeLevel(cDD->Hwnd_Main(), DSSCL_PRIORITY));

		// Create the primary buffer
	memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
	dsBufferDesc.dwSize         =  sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags        =  DSBCAPS_PRIMARYBUFFER;
	dsBufferDesc.dwBufferBytes  =  0;
	dsBufferDesc.lpwfxFormat    =  NULL;
	TRY_BM(pDirectSound->CreateSoundBuffer(&dsBufferDesc,&primary, NULL));

	this->SetFormat(44100,16,2);
     
	initialized = TRUE;

	return DS_OK;
}

// set primary buffer format
int	cBackgroundMusic::SetFormat(DWORD frequency, WORD bits_per_sample, WORD channels)
{
	WAVEFORMATEX format;

	if (!primary || !options.sound)
		return DS_OK;

	// Set to proper format
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = channels;
    format.nSamplesPerSec = frequency;
    format.nAvgBytesPerSec = frequency*channels*2;
    format.nBlockAlign = (bits_per_sample/8)*channels;
    format.wBitsPerSample = bits_per_sample;
    format.cbSize = 0;

	TRY_BM(primary->SetFormat(&format));

	// compact while we're at it
	TRY_BM(pDirectSound->Compact());

	return DS_OK;
}


void cBackgroundMusic::ToggleMusic(bool message)
{
	// check for valid object
	if ((pDirectSound == NULL) || (!options.sound))
        return;

	options.music_active = !options.music_active;

	SaveInGameRegistryOptionValues();

	if (options.music_active)
		this->StartMusic();
	else
		this->StopMusic();

	if (!message)
		return;

	if (!options.music_active) 
	{
		LoadString (hInstance, IDS_BACKGROUND_MUSIC_OFF, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message, false);
	}
	else 
	{
		LoadString (hInstance, IDS_BACKGROUND_MUSIC_ON, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message, false);
	}
}

// Release a buffer
void cBackgroundMusic::ReleaseBuffer()
{
	if ((pDirectSound == NULL) || (!options.sound))
		return;

	if (pPlayingBuffer)
		pPlayingBuffer->Release();	

	pPlayingBuffer = NULL;
}

int cBackgroundMusic::SetVolume()
{

	// check for valid object
	if ((pDirectSound == NULL) || (!options.music_active) || (!options.sound) || 
		(!playing_music) || (!pPlayingBuffer))
	    return DS_OK;

	int options_attenuation = 1;
	int volume_mod;

	// we don't multiply by -1 because we don't have a distance attenuation factor
	// like we do with sound effects
	options_attenuation = (max_volume - options.music_volume)*(DSBVOLUME_MIN/50); // attentuation factor
	if (options_attenuation == 0)
		options_attenuation = -1;

	volume_mod = options_attenuation;
	if (volume_mod < DSBVOLUME_MIN)
		volume_mod = DSBVOLUME_MIN;

	TRY_BM(pPlayingBuffer->SetVolume(volume_mod));

	return DS_OK;
}

///////////////////////////////////////////////////////////
// cBackgroundMusic::PlayMusic()
//
//  plays the next chunk of background music
///////////////////////////////////////////////////////////
int cBackgroundMusic::PlayMusic()
{
	DWORD	bufferStatus;
	int   flags;

	// check for valid object
	if ((pDirectSound == NULL) || (!options.music_active) || (!options.sound))
	    return DS_OK;

	if (!pPlayingBuffer)
		return DS_OK;
	
	// check if currently playing
	TRY_BM(pPlayingBuffer->GetStatus(&bufferStatus));

        //  if we lost the buffer, get it back
    if( bufferStatus & DSBSTATUS_BUFFERLOST ) {
        TRY_BM(pPlayingBuffer->Restore());
    }

	playing_music = true;

	this->SetVolume();
	
	// Make sure the buffer is set to play from the beginning.
	TRY_BM(pPlayingBuffer->SetCurrentPosition(0));
	
	flags = DSBPLAY_LOOPING;

	TRY_BM(pPlayingBuffer->Play(0, 0, flags));

	//printf("Playing buffer %d\n", pPlayingBuffer);

	return 0;
}

///////////////////////////////////////////////////////////
// cBackgroundMusic::StopMusic()
//
// This member function stops the sound stored in the given
// buffer
///////////////////////////////////////////////////////////
int cBackgroundMusic::StopMusic()
{
	if (!options.sound)
		return 0;

	DWORD status;

	playing_music = false;

	if (hCheckEvent)
		SetEvent(hCheckEvent);

	if ((pDirectSound == NULL) || (!pPlayingBuffer))
	    return 0;

	TRY_BM(pPlayingBuffer->GetStatus(&status));
	if (status & DSBSTATUS_PLAYING)  // if sound is playing...
	{
		TRY_BM(pPlayingBuffer->Stop());
	}

	this->ReleaseBuffer();

	this->CloseVorbis();

	track_number = 0;

	return 0;
}


///////////////////////////////////////////////////////////
// cBackgroundMusic::CreateSoundBuffer(...)
// Creates a direct sound buffer for the given id using the given wave format and sound data.

int cBackgroundMusic::CreatePlayingBuffer(void)
{
    DSBUFFERDESC dsBufferDesc;  

	if (!options.sound)
		return 0;

    if (pPlayingBuffer) // buffer is already loaded
		return 0;
	
	WAVEFORMATEX wave_format;
	
	wave_format.cbSize = 0;
	wave_format.nBlockAlign = 4;
	wave_format.nChannels = 2;
	wave_format.nSamplesPerSec = 44100;
	wave_format.wBitsPerSample = 16;
	wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec*wave_format.nChannels*wave_format.wBitsPerSample/8;
	wave_format.wFormatTag = WAVE_FORMAT_PCM ;
	
	// Initialize the DSBUFFERDESC structure.
	memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
	dsBufferDesc.dwSize         =  sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags        =  DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsBufferDesc.dwBufferBytes  =  OGG_BUFFER_SIZE;
	dsBufferDesc.lpwfxFormat    =  &wave_format;

	// Create the secondary sound buffer.
	TRY_BM(pDirectSound->CreateSoundBuffer(&dsBufferDesc,&(pPlayingBuffer), NULL));
	  
	return DS_OK;
}


///////////////////////////////////////////////////////////
// cBackgroundMusic::FillSoundBuffer(...)

int cBackgroundMusic::FillPlayingBuffer(int data_size, const void *data)
{
    LPVOID pSoundBlock1;
    DWORD bytesSoundBlock1;

	if (!options.sound)
		return DS_OK;

	if (!pPlayingBuffer)
		return DS_OK;

	DWORD start_pos = 0;
	if (playing_first_half)
		start_pos = OGG_BUFFER_SIZE/2;

	TRY_BM(pPlayingBuffer->Lock(start_pos, OGG_BUFFER_SIZE/2,	&pSoundBlock1, &bytesSoundBlock1,	NULL, NULL, 0));

	// Copy the data into the buffer.
	memcpy((void*)pSoundBlock1, data, data_size);

	// Unlock the buffer.
	TRY_BM(pPlayingBuffer->Unlock(pSoundBlock1, bytesSoundBlock1, NULL, 0));
  
	return DS_OK;
}

void cBackgroundMusic::MusicError(void)
{
	options.music_active = 0;
	SaveInGameRegistryOptionValues();

	LoadString (hInstance, IDS_NO_BACKGROUND_MUSIC, message, sizeof(message));
	if (display)
		display->DisplayMessage(message, false);
	else
		CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR, 
			cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
}

// opens the appropriate track 
int cBackgroundMusic::OpenVorbis(void)
{

	if (!options.sound)
		return DS_OK;

	// now set up Ogg Vorbis for background music playback
	char filename[512];

	bool ready = false;
	int num_tries = 0;

	while (!ready && (num_tries < OGG_NUM_TRACKS))
	{
		sprintf(filename, "Track%02d.rlm", track_number);
		vorbis_fd = fopen(filename, _T("rb"));
		if (vorbis_fd <= 0)
		{
			num_tries++;
			track_number++;
			if (track_number > OGG_NUM_TRACKS)
			   track_number = 1;
		}
		else 
		{
			ready = true;
		}
	}

	if (!ready) 
	{
		this->MusicError();
		this->DeInit();
		return DSERR_GENERIC;
	}

	_setmode( _fileno( vorbis_fd ), _O_BINARY );

    if (ov_open(vorbis_fd, &vf, NULL, 0) < 0) {
		fclose(vorbis_fd);
		this->MusicError();
		this->DeInit();
		return DSERR_GENERIC;
	}
	vorbis_initialized = TRUE;
	return DS_OK;
}	

int cBackgroundMusic::CloseVorbis(void)
{
	if (!vorbis_initialized || !options.sound)
		return DS_OK;

	vorbis_initialized = FALSE;
	ov_clear(&vf);

	return DS_OK;

}

void __cdecl StartMusicThreadProc(void *param)
{
	if (!options.sound)
		return;

	cBackgroundMusic* cBM = ((cBackgroundMusic*)param);
	int result = cBM->StartMusicHelper();
	if (result != DS_OK)
		cBM->MusicError();

	// now run check_vorbis at the proper intervals until we stop the music
	while (1)
	{
		WaitForSingleObject(hCheckEvent, INFINITE);
		if (cBM->PlayingMusic())
			cBM->CheckVorbisHelper();
		else 
			break;
	}

}

int cBackgroundMusic::StartMusicHelper(void)
{
	if (!options.sound)
		return DS_OK;

    this->CreatePlayingBuffer();
	track_number = rand()%OGG_NUM_TRACKS;
	if (track_number < 1)
		track_number = 1;
	if (track_number > OGG_NUM_TRACKS)
		track_number = OGG_NUM_TRACKS;

	int result = this->OpenVorbis();
	if (result != DS_OK)
		return result;

	playing_first_half = false;
	result = this->ReadVorbis();
	if (result != DS_OK)
		return result;

	playing_first_half = true;
	result = this->PlayMusic();
	if (result != DS_OK)
		return result;

	result = this->ReadVorbis();
	if (result != DS_OK)
		return result;

	return DS_OK;
}

int cBackgroundMusic::StartMusic(void)
{
	// spawn a new thread to handle playing the music, to avoid interruptions
	if (playing_music || !options.sound)
		return DS_OK;

	unsigned long thread_id = _beginthread(StartMusicThreadProc, 0, (void*)this);

	if (thread_id == -1)
	{
		return DSERR_GENERIC;
	}


	return DS_OK;
}

int cBackgroundMusic::CheckVorbisHelper(void)
{
	DWORD	bufferStatus;
	DWORD play_pos;
	DWORD write_pos;

	if (!options.sound)
		return DS_OK;

	// check if currently playing
	TRY_BM(pPlayingBuffer->GetStatus(&bufferStatus));

        //  if we lost the buffer, get it back
    if( bufferStatus & DSBSTATUS_BUFFERLOST ) {
        TRY_BM(pPlayingBuffer->Restore());
    }

	//printf("Getting position for buffer %d\n", pPlayingBuffer);

	TRY_BM(pPlayingBuffer->GetCurrentPosition(&play_pos, &write_pos));

	if (playing_first_half && (play_pos > (OGG_BUFFER_SIZE/2)))
	{
		//printf("In second half...\n");
		playing_first_half = false;
		this->ReadVorbis();
	} else if (!playing_first_half && (play_pos < (OGG_BUFFER_SIZE/2)))
	{
		//printf("In first half...\n");
		playing_first_half = true;
		this->ReadVorbis();
	}

	return DS_OK;
}
 
// checks to see if the current buffer being played has finished; if so, start playing the 
// next buffer,  and start to read in the one ahead of that
int cBackgroundMusic::CheckVorbis(void)
{
	// check for valid object
	if ((pDirectSound == NULL) || (!options.music_active) || (!options.sound) || (!vorbis_initialized))
	    return 0;

	if (!pPlayingBuffer)
		return 0;

	SetEvent(hCheckEvent);

	return DS_OK;
}


// we only read in half a buffer full of music at any one time; so, 
// if playing_first_half is true, we load in the second half of the 
// buffer, and vice versa
int cBackgroundMusic::ReadVorbis(void) 
{
	if (!options.sound)
		return DS_OK;

  char* pbuffer = (char*)pcmmusic;
  if (playing_first_half)
	pbuffer = (char*)(pcmmusic + OGG_BUFFER_SIZE/2);
  unsigned int read_so_far = 0;
  bool eof_vorbis  = false;
  int result = 0;

  while(!eof_vorbis && (read_so_far < OGG_BUFFER_SIZE/2)) {
    long num_bytes=ov_read(&vf,pbuffer,OGG_FRAME_SIZE,0,2,1,&vorbis_current_section);
	if (OGG_FRAME_SIZE == num_bytes)
	{
		read_so_far += num_bytes;
		pbuffer += num_bytes;
	} 
	else if (0 == num_bytes) 
      eof_vorbis = true;
	else if (num_bytes < 0) 
	{ // streaming error 
		this->MusicError();
		this->DeInit();
		return DSERR_GENERIC;
    } 
  }

  if (eof_vorbis) // finished playing a track; prepare to open the next one
  {
	  this->CloseVorbis();

	  track_number++;
	  if (track_number > OGG_NUM_TRACKS)
		   track_number = 1;
	  result = this->OpenVorbis();
	  if (result != DS_OK)
	  {
			this->MusicError();
			this->DeInit();
			return result;
	  }
  }

	// zero out the rest not filled with music
  if (read_so_far < OGG_BUFFER_SIZE/2)
  {
	memset((void*)pbuffer, 0, 
		( OGG_BUFFER_SIZE/2 - read_so_far));
  }

  if (playing_first_half)
	result = this->FillPlayingBuffer(OGG_BUFFER_SIZE/2, (char*)(pcmmusic + OGG_BUFFER_SIZE/2));
  else
	result = this->FillPlayingBuffer(OGG_BUFFER_SIZE/2, (char*)pcmmusic);

  if (result != DS_OK)
  {
		this->MusicError();
		this->DeInit();
		return result;
  }
    
  return DS_OK;

}


void cBackgroundMusic::TraceErrorBM(HRESULT hErr, int nLine)
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

	this->DeInit();
}


// Check invariants

#ifdef CHECK_INVARIANTS
void cBackgroundMusic::CheckInvariants(int line, TCHAR *file)
{

}
#endif



