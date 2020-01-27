// Header file for cOutput class 

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef COUTPUT_H
#define COUTPUT_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////
// Constants

//////////////////////////////////////////////////////////////////
// Class Definition
class cOutput
{

public:

private:
	FILE* fh;
	TCHAR filename[DEFAULT_MESSAGE_SIZE];
	TCHAR szPath[MAX_PATH];
	TCHAR szPath_backup[MAX_PATH];;
	bool append;
	bool m_fForceFlush; // true = flush after each write. Use with care.

public:
	cOutput(TCHAR* fn, bool append_to_file, bool fForceFlush = false);
	void ReInit(void); // close & reopen
	void Write(TCHAR* data, bool long_date = false);
	void WriteStamp(bool long_date = false);
	~cOutput(void);


	inline FILE *FileHandle(void) { return fh; };

private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cOutput(const cOutput& x);
	cOutput& operator=(const cOutput& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif