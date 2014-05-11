#ifndef CDSOUND_H
#define CDSOUND_H

#include <dsound.h>
#include "Effects.h"
#include "SharedConstants.h"

//////////////////////////////////////////////////////////////////
// Constants

const int MAX_DUPLICATES = 16;

//////////////////////////////////////////////////////////////////
// Class Definition

class cDSound
{
public:

private:
    LPDIRECTSOUND        pDirectSound;
	BOOL                 reverse_stereo;
	BOOL				 initialized;
    LPDIRECTSOUNDBUFFER  primary;
    LPDIRECTSOUNDBUFFER  duplicates[MAX_DUPLICATES];
	int					 duplicate_id[MAX_DUPLICATES];
	LPDIRECTSOUNDBUFFER  sounds[MAX_SOUNDS];
	int					 num_duplicates_playing[MAX_SOUNDS];
	bool				 looping[MAX_SOUNDS];
	bool				 sound_error_displayed;
	bool				 propagate_sound[LyraSound::MAX_SOUND_ID];

public:
    cDSound(BOOL enabled, BOOL reverse=FALSE); 
    ~cDSound();
    int  PlaySound(int id, float x=-100000.0, float y=0.0f, bool propagate = false);//, int volume=0);
    int  StopSound(int id);
  	void ToggleSound(void);
	void ReleaseBuffer(int id);
	int	 SetFormat(DWORD frequency, WORD bits_per_sample, WORD channels);
	inline BOOL Initialized(void) { return initialized; };
	int  CreateSoundBuffer(int id, WAVEFORMATEX &wave_format,int data_size, const void *data);
	inline void SetReverse(BOOL reverse) { reverse_stereo = reverse;};
	void CheckSounds(void);

private:
	void TraceErrorDS(HRESULT hErr, int nLine);
  int  CleanupDuplicates(void);
	int  InitDS();
	void DeInit(void);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cDSound(const cDSound& x);
	cDSound& operator=(const cDSound& x);

friend void CALLBACK InitRetryTimerProc( HWND hwnd,UINT uMsg, UINT idEvent, DWORD dwTime);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};


#endif

