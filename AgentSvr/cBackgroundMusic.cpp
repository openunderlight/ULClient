// cDS: Direct Sound Class Implementation

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT
#include "Central.h"
#include "cBackgroundMusic.h"

//////////////////////////////////////////////////////////////////
// External Global Variables


//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// Macros 

//////////////////////////////////////////////////////////////////
// Constants


void CALLBACK cBMInitRetryTimerProc( HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime) {}

/////////////////////////////////////////////////////////////////
// Class Defintion


cBackgroundMusic::cBackgroundMusic(BOOL enabled) {}
cBackgroundMusic::~cBackgroundMusic() {}
int  cBackgroundMusic::PlayMusic(void) { return 0;}
int  cBackgroundMusic::StopMusic(void) { return 0;}
void cBackgroundMusic::ToggleMusic(bool message) {}
void cBackgroundMusic::ReleaseBuffer(void) {}
int	 cBackgroundMusic::SetFormat(DWORD frequency, WORD bits_per_sample, WORD channels) { return 0;}
int  cBackgroundMusic::CreatePlayingBuffer(void) { return 0;}
int  cBackgroundMusic::FillPlayingBuffer(int data_size, const void *data) { return 0;}
int cBackgroundMusic::OpenVorbis(void) { return 0;} 
int cBackgroundMusic::CloseVorbis(void) { return 0;}
int  cBackgroundMusic::CheckVorbis(void) { return 0;}
int  cBackgroundMusic::StartMusic(void) { return 0;}
int	cBackgroundMusic::SetVolume(void) { return 0;}
void cBackgroundMusic::MusicError(void) {}
void cBackgroundMusic::TraceErrorBM(HRESULT hErr, int nLine) {}
int cBackgroundMusic::InitBM() { return 0;}
void cBackgroundMusic::DeInit(void) {}
int cBackgroundMusic::ReadVorbis(void) { return 0;}

#ifdef CHECK_INVARIANTS
void cBackgroundMusic::CheckInvariants(int line, TCHAR *file) {}
#endif
