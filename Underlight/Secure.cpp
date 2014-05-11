#define STRICT

#include "Central.h"
#include <windows.h>
#include <lzexpand.h>
#include <windowsx.h>
#include "Resource.h"
#include "Interface.h"
#include "Secure.h"

//******************************************************************
// EXTERNAL FUNCTION PROTOTYPES
//******************************************************************
BOOL StartGame( BOOL bNewGame, BOOL bSinglePlayer ); 

//******************************************************************
// INTERNAL DEFINITIONS
//******************************************************************

#define BITMAPRESOURCE  IDB_TARGET

#ifdef UL_DEBUG
#define WSCAN_FREQ		3000		// WINDOW SCANNING FREQUENCY (ms)
#else
#define WSCAN_FREQ		500			// WINDOW SCANNING FREQUENCY (ms)
#endif //UL_DEBUG

//******************************************************************
// GLOBALS
//******************************************************************

HINSTANCE	ghInst			= NULL;
HANDLE		ghAudit			= NULL;
HANDLE		ghevAuditInit	= NULL;
HANDLE		ghevAuditDone	= NULL;

//******************************************************************
// INTERNAL FUNCTION PROTOTYPES
//******************************************************************

DWORD _stdcall Audit( LPVOID pParam );
BOOL CALLBACK WndScan( HWND hWnd, LPARAM lParam );

//******************************************************************
//
// SECURE IMPLEMENTATION
//
//******************************************************************

//******************************************************************
//******************************************************************
BOOL CALLBACK WndScan( HWND hWnd, LPARAM lParam )
{
	HANDLE hProc;
	int	   i, length;
	HWND   hWndExplorer;
	TCHAR   szTitle[MAX_PATH];
	DWORD  dwPid, dwPidExplorer;

	// THE WIN95 EXPLORER IS A SINGLE PROCESS - TERMINATION OF THIS
	// PROCESS RESULTS IN TERMINATION OF THE USER SHELL AND HANGS
	// THE SYSTEM UPON EXIT

	hWndExplorer = FindWindow( _T("CabinetWClass"), NULL );
		
	GetWindowThreadProcessId( hWnd, &dwPid );
	GetWindowThreadProcessId( hWndExplorer, &dwPidExplorer );

	if( dwPid == dwPidExplorer )
		return TRUE;

	if( dwPid == GetCurrentProcessId() )
		return TRUE;

	if( GetWindowText( hWnd, szTitle, sizeof( szTitle ) ) == 0 )
		return TRUE;

	if( ( length = _tcslen( szTitle ) ) < 6 )
		return TRUE;

	_tcslwr( szTitle );

	for( i=0; i<length-5; i++ )
	{
		if( szTitle[i] == 'd' )
		{
			if( memcmp( &(szTitle[i]), "underlight", 6 ) == 0 )
			{
				hProc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dwPid );

				if( hProc == NULL )
					return TRUE;

#ifdef UL_DEBUG
				CloseHandle( hProc );
				return TRUE;
#else
				TerminateProcess( hProc, 0 );
				CloseHandle( hProc );
				return FALSE;
#endif //UL_DEBUG

			}
		}
	}

	return TRUE;
}


//******************************************************************
// CALL DURING PROGRAM STARTUP - DO NOT RUN IF THIS FAILS
//******************************************************************
BOOL MpStartup( HINSTANCE hInst )
{
	DWORD dwId;

	ghInst = hInst;
	if( !ghInst )
		return FALSE;

	// CREATE AUDIT SHUTDOWN EVENT

	ghevAuditDone = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( ghevAuditDone == NULL )
	{
		return FALSE;
	}

	// CREATE AUDIT INIT EVENT

	ghevAuditInit = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( ghevAuditInit == NULL )
	{
		MpShutdown();
		return FALSE;
	}

	// START AUDIT THREAD

	ghAudit = CreateThread( NULL, 0, Audit, NULL, 0, &dwId );
	if( ghAudit == NULL )
	{
		MpShutdown();
		return FALSE;
	}

	SetThreadPriority( ghAudit, THREAD_PRIORITY_TIME_CRITICAL );

	// WAIT FOR AUDIT INITIALIZATION TO COMPLETE

	if( WaitForSingleObjectEx( ghevAuditInit, INFINITE, FALSE ) != WAIT_OBJECT_0 )
	{
		MpShutdown();
		return FALSE;
	}

	// WAIT FOR AUDIT THREAD TO TERMINATE

	if( WaitForSingleObjectEx( ghAudit, 0, FALSE ) != WAIT_TIMEOUT )
	{
		MpShutdown();
		return FALSE;
	}

	return TRUE;
}

//******************************************************************
// CALL DURING PROGRAM EXIT - IF STARTUP SUCCEEDS YOU *MUST* CALL SHUTDOWN
//******************************************************************
void MpShutdown( void )
{
	// SIGNAL AUDIT SHUTDOWN EVENT

	if( ghevAuditDone )
		SetEvent( ghevAuditDone );

	// WAIT FOR THREAD TO EXIT - TERMINATE W/TIMEOUT OF 3000ms

	if( ghAudit )
	{
		if( WaitForSingleObjectEx( ghAudit, 3000, FALSE ) == WAIT_TIMEOUT )
			TerminateThread( ghAudit, 0 );

		ghAudit = NULL;
	}

	// CLOSE AUDIT INIT EVENT

	if( ghevAuditInit )
	{
		CloseHandle( ghevAuditInit );
		ghevAuditInit = NULL;
	}

	// CLOSE AUDIT SHUTDOWN EVENT

	if( ghevAuditDone )
	{
		CloseHandle( ghevAuditDone );
		ghevAuditDone = NULL;
	}
}

//******************************************************************
// AUDIT THREAD
//******************************************************************
DWORD _stdcall Audit( LPVOID pParam )
{
	// OPEN A TEMPFILE

	HANDLE hVXD;
	TCHAR   szVXDPath[MAX_PATH], szTempFile[MAX_PATH], szVXDFile[MAX_PATH];

	GetTempPath( sizeof( szVXDPath ), szVXDPath );
	GetTempFileName( szVXDPath, _T("DL"), 0, szTempFile );
	GetTempFileName( szVXDPath, _T("DL"), 0, szVXDFile );

	hVXD = CreateFile( szTempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
		               FILE_ATTRIBUTE_ARCHIVE, NULL );

	if( hVXD == INVALID_HANDLE_VALUE )
	{
		SetEvent( ghevAuditInit );
		return 0;
	}

	// GET A POINTER TO THE MPDEICE VXD RESOURCE

	char*   pVXD;
	HGLOBAL hgVXD;
	HRSRC   hrVXD;

	hrVXD = FindResourceEx( NULL, RT_BITMAP, MAKEINTRESOURCE( BITMAPRESOURCE ),
		                    MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ) );
  	
	if( hrVXD == NULL )
	{
		CloseHandle( hVXD );
		DeleteFile( szTempFile );
		SetEvent( ghevAuditInit );
		return 0;
	}

	hgVXD = LoadResource( ghInst, hrVXD );

	if( hgVXD == NULL )
	{
		CloseHandle( hVXD );
		DeleteFile( szTempFile );
		SetEvent( ghevAuditInit );
		return 0;
	}

	pVXD = (char*) LockResource( hgVXD );

	if( pVXD == NULL )
	{
		CloseHandle( hVXD );
		DeleteFile( szTempFile );
		SetEvent( ghevAuditInit );
		return 0;
	}

	// DECODE THE DEICE VXD
	// WRITE THE MPDEICE VXD TO THE TEMPFILE

	BITMAPINFO*	pbmi;
	BYTE		vxdVal;
	RGBQUAD*	prgbVal;
	DWORD		d, dwLength, dwBytesOut;

	pbmi = (BITMAPINFO*) pVXD;
	pVXD += sizeof( BITMAPINFO ) + 10;

	dwLength = pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biHeight;

	for( d=0; d<dwLength; d++ )
	{
		prgbVal = (RGBQUAD*) pVXD;

		if( prgbVal->rgbBlue & 0x04 )
		{
			vxdVal = ( ( prgbVal->rgbBlue  & 0x03 ) << 6 ) + 
				     ( ( prgbVal->rgbRed   & 0x07 ) << 3 ) +
				     ( ( prgbVal->rgbGreen & 0x07 ) );

			if( !WriteFile( hVXD, &vxdVal, 1, &dwBytesOut, NULL ) )
			{
				CloseHandle( hVXD );
				DeleteFile( szTempFile );
				SetEvent( ghevAuditInit );
				return 0;
			}
		}

		pVXD += 3;
	}

	CloseHandle( hVXD );

	// DECOMPRESS THE VXD

	OFSTRUCT	ofn;
	int			hzDst, hzSrc;

	hzSrc = LZOpenFile( szTempFile, &ofn, OF_READ );

	if( hzSrc < 0 )
	{
		DeleteFile( szTempFile );
		SetEvent( ghevAuditInit );
		return 0;
	}

	hzDst = LZOpenFile( szVXDFile, &ofn, OF_CREATE );

	if( hzDst < 0 )
	{
		LZClose( hzSrc );
		DeleteFile( szTempFile );
		SetEvent( ghevAuditInit );
		return 0;
	}

	if( LZCopy( hzSrc, hzDst ) < 0 )
	{
		LZClose( hzSrc );
		LZClose( hzDst );
		DeleteFile( szTempFile );
		SetEvent( ghevAuditInit );
		return 0;
	}

	LZClose( hzSrc );
	LZClose( hzDst );

	DeleteFile( szTempFile );
 
	// LOAD THE VXD

	_tcscpy( szVXDPath, _T("\\\\.\\") );
_tcscat( szVXDPath, szVXDFile );

	hVXD = CreateFile( szVXDPath, 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL );
	if( hVXD == INVALID_HANDLE_VALUE )
	{
		if( GetLastError() != 87 )
		{
			// USER IS RUNNING SOFTICE ON WIN95

			CloseHandle( hVXD );
			DeleteFile( szVXDFile );
			SetEvent( ghevAuditInit );

			ExitProcess(1);

			return 0;
		}

		// USER IS RUNNING WINNT (87) -OR-
		// USER IS RUNNING WIN95 AND SOFTICE IS NOT PRESENT
	}

	// WINICE NOT PRESENT

	SetEvent( ghevAuditInit );

	WaitForSingleObjectEx( ghevAuditDone, INFINITE, FALSE );

	if( hVXD != INVALID_HANDLE_VALUE )
		CloseHandle( hVXD );

	DeleteFile( szVXDFile );

	return 0;
}

