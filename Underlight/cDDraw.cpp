// Direct Draw Class

// Copyright Lyra LLC, 1996. All rights reserved.


#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "cChat.h"
#include "cControlPanel.h"
#include "cGameServer.h"
//#include "cBanner.h"
#include "cPlayer.h"
#include "cEffects.h"
#include "Options.h"
#include "Utils.h"

#include "4dx.h"
#include "resource.h"
#include "cDDraw.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern options_t options;
extern cPlayer *player;
extern cChat *display;
extern cGameServer *gs;
extern cControlPanel *cp;
extern unsigned long exit_time;
//extern cBanner *banner;
extern cEffects *effects;
extern cDDraw *cDD;
extern bool leveleditor;
extern int MAX_LV_ITEMS;
extern HINSTANCE hInstance;
extern bool exiting;


/////////////////////////////////////////////////////////////////
// Constants

const int ERROR_THRESHHOLD = 500; // exit after 500 nonfatal errors


/////////////////////////////////////////////////////////////////
// Macros

// Initializes a Direct Draw struct that is about to be used in a call.
// Ensures the struct is zeroed and it's size member is set
#define INIT_DD_STRUCT(s) { memset(&s,0, sizeof(s)); s.dwSize = sizeof(s); }

/////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor
// normal(stand-alone) constructor
cDDraw::cDDraw(TCHAR *name, TCHAR *title, HINSTANCE hInstance, WNDPROC wproc,
				LPCTSTR applicon, LPCTSTR applcursor,	int resolution, int x, int y)


{
	WNDCLASS wc;
	bpp = BITS_PER_PIXEL;
	DWORD style,type;

	// convert resolution from (640,800,1024) to (0,1,2)
	switch (resolution) {
		case 800:
			res = 1;
			break;
		case 1024:
			res = 2;
			break;
		default:
			res = 0;
			break;
	}

	windowed = TRUE;
	lpDDSOffscreen = lpDDSPrimary = NULL;
	lpDD = NULL;

	switch (res)
	{
	case 0: // 640x480
		
		if (leveleditor)
		{
			width = 480 + 2*GetSystemMetrics(SM_CXFIXEDFRAME);
			height = 300 + 2*GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION);
			windowed = TRUE;

		}
		else
		{
			width = 640; height = 480; 
			//windowed=TRUE;
		}
		MAX_LV_ITEMS = 15;
		viewx = 480; viewy = 300;
		break;

	case 1: // 800x600
		
		width = 800; height = 600 + GetSystemMetrics(SM_CYCAPTION);
		viewx = 600; viewy = 375;
		MAX_LV_ITEMS = 17;

		break;

	case 2: // 1024x768
		
		if (leveleditor) 
			windowed = TRUE;

		width = 1024; height = 768 + GetSystemMetrics(SM_CYCAPTION);
		MAX_LV_ITEMS = 20;


		if (x && y)
		{
			viewx = x; viewy = y;
		}
		else
		{
			viewx = 768; viewy = 480;
		}

		break;

	default:
		status = NULL;
		return;
	}

	// set up and register window class
	wc.style 		  = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
	wc.lpfnWndProc   = wproc;
	wc.cbClsExtra	  = 0;
	wc.cbWndExtra	  = 0;
	wc.hInstance	  = hInstance;
	wc.hIcon 		  = LoadIcon( hInstance, applicon);
	wc.hCursor		  = NULL; //LoadCursor( NULL, applcursor );
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = name;

	RegisterClass( &wc );

	style = 0;
	/*
	if (windowed)
		type = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	else
		type = WS_POPUP;
	*/
	type = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	hwnd_main = CreateWindowEx(
										style,
										name,
										title,
										type,
										0, 0,
										width,
										height,
										NULL,
										NULL,
										hInstance,
										NULL );

	
	  error_count = 0;

	return;
}


// Selector for window handle

HWND cDDraw::Hwnd_Main(void)
{
	return hwnd_main;
}

void cDDraw::Show()
{
	ShowWindow( hwnd_main, SW_SHOWNORMAL );
	if (leveleditor)
		return; // show rendering window only!
	
	cp->Show();
	display->Show();
	UpdateWindow( hwnd_main ); // crashes the activex control...
	SendMessage(hwnd_main, WM_ACTIVATE, (WPARAM) WA_CLICKACTIVE, (LPARAM) hwnd_main);

	return;
}


// Initialize all the DD stuff
void cDDraw::InitDDraw()
{
	DDSURFACEDESC2 ddsd;
	int curr_depth, curr_width, curr_height;

	// initialize the pointers so that we can unload them in the
	// destructors.
	lpDD			 = NULL;
	lpDDSPrimary = NULL;

	// get default device
	TRY_DD(DirectDrawCreateEx(NULL, (LPVOID* )&lpDD, IID_IDirectDraw7, NULL));

	// Examine current pixel depth
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags = DDSD_ALL ;
	TRY_DD(lpDD->GetDisplayMode(&ddsd));
	curr_depth	= ddsd.ddpfPixelFormat.dwRGBBitCount;
	curr_width	= ddsd.dwWidth;
	curr_height = ddsd.dwHeight;

	bpp = curr_depth;
	TRY_DD(lpDD->SetCooperativeLevel( hwnd_main, DDSCL_NORMAL));
	if (curr_depth > bpp)
	{
		GAME_ERROR(IDS_SURFACE_COLOR_DEPTH_TOO_HIGH);
		return;
	}

	DDPIXELFORMAT ddpf;
	ZeroMemory(&ddpf, sizeof(ddpf));
	ddpf.dwSize = sizeof(ddpf);
	ddpf.dwRBitMask = 0x00F800;
	ddpf.dwGBitMask = 0x0007E0;
	ddpf.dwBBitMask = 0x00001F;
	ddpf.dwRGBBitCount = 16;
	ddpf.dwFlags = DDPF_RGB;

	// Allocate the surface
	// For now, just use one primary surface on the vid card
	memset(&ddsd, 0, sizeof( ddsd));
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags			  = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps	  = DDSCAPS_PRIMARYSURFACE;
	TRY_DD(lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ));

	// create & attach the clipper if we're windowed
	if (windowed)
	{
		TRY_DD(lpDD->CreateClipper(0, &lpDDClipper, NULL));
		TRY_DD(lpDDClipper->SetHWnd(0, hwnd_main));
		TRY_DD(lpDDSPrimary->SetClipper(lpDDClipper));
	}

	// create offscreen surface in system memory
	memset(&ddsd, 0, sizeof( ddsd));
	ddsd.dwSize 			  = sizeof( ddsd );
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	ddsd.ddpfPixelFormat = ddpf;
	ddsd.dwHeight = viewy;
	ddsd.dwWidth = viewx;
	ddsd.ddsCaps.dwCaps	  = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	TRY_DD(lpDD->CreateSurface( &ddsd, &lpDDSOffscreen, NULL ));

	// Determine surface pixel format 555 or 565
	DDPIXELFORMAT format;
	INIT_DD_STRUCT(format);
	TRY_DD(lpDDSOffscreen->GetPixelFormat(&format));
	switch (format.dwGBitMask) // Use green mask to determine format
	{
		case 0x000007E0:	pixel_format = PIXEL_FORMAT_565; break;
		case 0x000003E0:	pixel_format = PIXEL_FORMAT_555; break;
		//case 0x0000FF00:	pixel_format = PIXEL_FORMAT_32; break;  //Alex added to ID 32-bit color depth?
		default: 			GAME_ERROR(IDS_UNK_PIX_FORMAT);
	}
}


unsigned char *cDDraw::GetSurface(int id)
{
	DDSURFACEDESC2		  DDSDesc;
	LPDIRECTDRAWSURFACE7	 lpDDSSurface;

	if (id == PRIMARY)
		lpDDSSurface = lpDDSPrimary;
	else if (id == BACK_BUFFER)
		lpDDSSurface = lpDDSOffscreen;
	else
		return NULL;

	memset(&DDSDesc,0,sizeof(DDSURFACEDESC2));
	DDSDesc.dwSize = sizeof(DDSURFACEDESC2);
	while( 1 )
	{
		status = lpDDSSurface->Lock(NULL,&DDSDesc,0,NULL);

		if( status == DD_OK )
			break;

		if( status == DDERR_SURFACELOST)
		{
			status = lpDDSSurface->Restore();
			if (status == DD_OK)
				continue;
		}
		this->TraceErrorDD(status, __LINE__);

		return NULL;
	}
	pitch = DDSDesc.lPitch;
	return (unsigned char *)DDSDesc.lpSurface;
}


// We're done with the surface pointer
void cDDraw::ReleaseSurface(int id)
{
	LPDIRECTDRAWSURFACE7	 lpDDSSurface;

	if (id == PRIMARY)
		lpDDSSurface = lpDDSPrimary;
	else if (id == BACK_BUFFER)
		lpDDSSurface = lpDDSOffscreen;
	else
		return;

	lpDDSSurface->Unlock(NULL);
}


bool cDDraw::EraseSurface(int id)
{
	 DDBLTFX 	 ddbltfx;
	LPDIRECTDRAWSURFACE7	 lpDDSSurface;
	RECT rect;
	POINT wndpt;
	int xoff=0,yoff=0; // offsets for running in a window

	if (id == PRIMARY)
		lpDDSSurface = lpDDSPrimary;
	else if (id == BACK_BUFFER)
		lpDDSSurface = lpDDSOffscreen;
	else
		return false;

	// Erase the surface
	ddbltfx.dwSize = sizeof( ddbltfx );
	ddbltfx.dwFillColor = 0;
	while( 1 )
	{
		status = lpDDSSurface->Blt( NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
		if( status == DD_OK )
			  return true;

		if( status == DDERR_SURFACELOST)
		{
			status = lpDDSSurface->Restore();
			if (status == DD_OK)
				continue;
		}

		this->TraceErrorDD(status, __LINE__);

		return false;
	}
}


bool cDDraw::BlitOffScreenSurface(void)
{
	RECT srcRect,dstRect;
	int xoff,yoff;

	int len = viewx;
	int width = viewy;

	srcRect.left	= (viewx - len)/2;
	srcRect.top	= (viewy - width)/2;
	srcRect.right = srcRect.left+len;
	srcRect.bottom= srcRect.top +width;

	while( 1 )
	{
		if (windowed)
		{
			xoff = this->XOffset();
			yoff = this->YOffset();
			dstRect.left  = (viewx - len)/2 + xoff;
			dstRect.top   = (viewy - width)/2 + yoff;
			dstRect.right = srcRect.left+len + xoff;
			dstRect.bottom= srcRect.top +width + yoff;

			status = lpDDSPrimary->Blt(&dstRect, lpDDSOffscreen, &srcRect, DDBLTFAST_NOCOLORKEY|DDBLT_WAIT, NULL);
		}
		else
// REG 6/11/00: Following keeps cDD from redrawing while minimized.
//#if defined (UL_DEBUG) || defined (GAMEMASTER)
//		{
			if( !IsIconic(cDD->Hwnd_Main()) )
				status = lpDDSPrimary->BltFast(0,0,lpDDSOffscreen, &srcRect, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
//		}
//#else
//			status = lpDDSPrimary->BltFast(0,0,lpDDSOffscreen, &srcRect, DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
//#endif
		if( status == DD_OK )
			  return true;

		if( status == DDERR_SURFACELOST)
		{
			status = lpDDSPrimary->Restore();
			if (status == DD_OK)
				continue;
		}

		this->TraceErrorDD(status, __LINE__);

		return false;
	}
}


// fills in the rectangle for the current viewport
void cDDraw::ViewRect (RECT *rect)
{
	rect->left = this->XOffset();
	rect->top = this->YOffset();
	rect->right = this->XOffset() + viewx - 1;
	rect->bottom = this->YOffset() + viewy - 1;
	return;
}


// scale a coordinate up according to the resolution
int cDDraw::ScaletoRes (int value)
{
	float temp;
	switch (res) {
		case 1:
		{
			temp = value*1.25;
			return (int)temp;
		}	
		case 2:
		{
			temp = value*1.6;
			return (int)temp;
		}	
		case 0:
		default:
			return value;
	}
	return value;
}

// Find the center position for dialog boxes
int	cDDraw::DlgPosX(HWND hDlg)
{
	RECT rect;
	int w;

	GetWindowRect(hDlg, &rect);
	w = rect.right - rect.left;

	if (w < viewx) // put over only the chat box
		return (width - w)/2 + this->XOffset() - (width - viewx)/2;
	else
		return (width - w)/2 + this->XOffset();
}

// Find the top y position for dialog boxes
int	cDDraw::DlgPosY(HWND hDlg)
{
	RECT rect;
	int h;

	GetWindowRect(hDlg, &rect);
	h = rect.bottom - rect.top;

	return (height - viewy - h)/2 + viewy + this->YOffset();
}


// returns the x offset of the upper left window coord
int cDDraw::XOffset(void)
{
	POINT wndpt;

	if (windowed)
	{
		wndpt.x = 0;
		wndpt.y = 0;
		ClientToScreen(hwnd_main, &wndpt);
		return wndpt.x;
	} else
		return 0;
}


// returns the y offset of the upper left window coord
int cDDraw::YOffset(void)
{
	POINT wndpt;

	if (windowed)
	{
		wndpt.x = 0;
		wndpt.y = 0;
		ClientToScreen(hwnd_main, &wndpt);
		return wndpt.y;
	} else
		return 0;
}

// show game intro splash screen
bool cDDraw::ShowSplashScreen(void)
{
#ifdef UL_DEBUG
	return true;
#endif
	unsigned char *dst = this->GetSurface(PRIMARY);

	if (dst == NULL)
		return false;

	effects->LoadEffectBitmaps(LyraBitmap::SPLASH, 6);
	unsigned char *src = effects->EffectBitmap(LyraBitmap::SPLASH)->address;
	int splash_height = effects->EffectHeight(LyraBitmap::SPLASH);
	int splash_width = effects->EffectWidth(LyraBitmap::SPLASH);

	for (int i=0; i<splash_height; i++)
	{
		memcpy(dst, src, splash_width*BYTES_PER_PIXEL);
		src += splash_width*BYTES_PER_PIXEL;
		dst += pitch;
	}

	this->ReleaseSurface(PRIMARY);

	effects->FreeEffectBitmaps(LyraBitmap::SPLASH);

	return true;
}

// show message in empty viewport while entering levels
bool cDDraw::ShowIntroBitmap(void)
{
	if (!this->EraseSurface(BACK_BUFFER))
		return false;

	if (exiting)
		return false;

	int bitmap = LyraBitmap::INTRO;

	// if we're not entering the game, show the changing planes bitmap instead
	if (gs->LoggedIntoGame())
		bitmap = LyraBitmap::CHANGING_PLANES;

	unsigned char *dst = this->GetSurface(BACK_BUFFER);

	if (dst == NULL)
		return false;

	unsigned char *src = effects->EffectBitmap(bitmap)->address;
	int intro_height = effects->EffectHeight(bitmap);
	int intro_width = effects->EffectWidth(bitmap);

	// offset to center
	dst += pitch*(int)((viewy - intro_height)/2);
	dst += BYTES_PER_PIXEL*(int)((viewx - intro_width)/2);

	for (int i=0; i<intro_height; i++)
	{
		memcpy(dst, src, intro_width*BYTES_PER_PIXEL);
		src += intro_width*BYTES_PER_PIXEL;
		dst += pitch;
	}

	this->ReleaseSurface(BACK_BUFFER);
	return true;
}

// End the Direct Draw Session
void cDDraw::DestroyDDraw()
{
	if( lpDDSOffscreen != NULL )
	{
		lpDDSOffscreen->Release();
		lpDDSOffscreen = NULL;
	}
	if( lpDDSPrimary != NULL )
	{
		lpDDSPrimary->Release();
		lpDDSPrimary = NULL;
	}

	if (lpDD != NULL)
	{
#ifndef UL_DEBUG
	if (!windowed && options.exclusive)
		lpDD->RestoreDisplayMode();
#endif
		lpDD->Release();
		lpDD = NULL;
	}
}

// Destructor
cDDraw::~cDDraw()
{
	if (hwnd_main)
		DestroyWindow(hwnd_main);
	return;

}


#define DDERR(str) LoadString(hInstance, str, ddErr, sizeof(ddErr))

// checks on status for a failed surface operation, and either ignores
// it or forms an error based on where it was called from

void cDDraw::TraceErrorDD(HRESULT hErr, int nLine)
{
	TCHAR ddErr[DEFAULT_MESSAGE_SIZE];
	bool fatal = true; // default is fatal error

	switch (hErr)
	{
	case DD_OK:
		// The request completed successfully.
		DDERR( IDS_DD_OK);
		fatal = false;
		break;

	case DDERR_ALREADYINITIALIZED:
		// The object has already been initialized.
		// example for SH: DDERR(IDS_DDERR_ALREADYINITIALIZED);
		DDERR( IDS_DDERR_ALREADYINITIALIZED);
		fatal = false;
		break;

	case DDERR_BLTFASTCANTCLIP:
		// A DirectDrawClipper object is attached to a source surface
		// that has passed into a call to the IDirectDrawSurface2::BltFast method.
		DDERR( IDS_DDERR_BLTFASTCANTCLIP);
		break;

	case DDERR_CANNOTATTACHSURFACE:
		// A surface cannot be attached to another requested surface.
		DDERR( IDS_DDERR_CANNOTATTACHSURFACE);
		break;

	case DDERR_CANNOTDETACHSURFACE:
		// A surface cannot be detached from another requested surface.
		DDERR( IDS_DDERR_CANNOTDETACHSURFACE);
		break;

	case DDERR_CANTCREATEDC:
		// Windows cannot create any more device contexts (DCs).
		DDERR( IDS_DDERR_CANTCREATEDC);
		break;

	case DDERR_CANTDUPLICATE:
		// Primary and 3D surfaces, or surfaces that are
		// implicitly created, cannot be duplicated.
		DDERR( IDS_DDERR_CANTDUPLICATE);
		break;

	case DDERR_CANTLOCKSURFACE:
		// Access to this surface is refused because an
		// attempt was made to lock the primary surface without DCI support.
		DDERR( IDS_DDERR_CANTLOCKSURFACE);
		break;

	case DDERR_CANTPAGELOCK:
		// An attempt to page lock a surface failed.
		// Page lock will not work on a display-memory
		// surface or an emulated primary surface.
		DDERR( IDS_DDERR_CANTPAGELOCK);
		break;

	case DDERR_CANTPAGEUNLOCK:
		// An attempt to page unlock a surface failed.
		// Page unlock will not work on a display-memory
		// surface or an emulated primary surface.
		DDERR( IDS_DDERR_CANTPAGEUNLOCK);
		break;

	case DDERR_CLIPPERISUSINGHWND:
		// An attempt was made to set a clip list for a DirectDrawClipper
		// object that is already monitoring a window handle.
		DDERR( IDS_DDERR_CLIPPERISUSINGHWND);
		break;

	case DDERR_COLORKEYNOTSET:
		// No source color key is specified for this operation
		DDERR( IDS_DDERR_COLORKEYNOTSET);
		break;

	case DDERR_CURRENTLYNOTAVAIL:
		// No support is currently available.
		DDERR( IDS_DDERR_CURRENTLYNOTAVAIL);
		break;

	case DDERR_DCALREADYCREATED:
		// A device context (DC) has already been returned for this surface.
		// Only one DC can be retrieved for each surface.
		DDERR( IDS_DDERR_DCALREADYCREATED);
		break;

	case DDERR_DIRECTDRAWALREADYCREATED:
		// A DirectDraw object representing this driver
		// has already been created for this process.
		DDERR( IDS_DDERR_DIRECTDRAWALREADYCREATED);
		break;

	case DDERR_EXCEPTION:
		// An exception was encountered while
		// performing the requested operation.
		DDERR( IDS_DDERR_EXCEPTION);
		break;

	case DDERR_EXCLUSIVEMODEALREADYSET:
		// An attempt was made to set the cooperative
		// level when it was already set to exclusive.
		DDERR( IDS_DDERR_EXCLUSIVEMODEALREADYSET);
		fatal = false;
		break;

	case DDERR_GENERIC:
		// There is an undefined error condition.
		DDERR( IDS_DDERR_GENERIC);
		break;

	case DDERR_HEIGHTALIGN:
		// The height of the provided rectangle
		// is not a multiple of the required alignment.
		DDERR( IDS_DDERR_HEIGHTALIGN);
		break;

	case DDERR_HWNDALREADYSET:
		// The DirectDraw cooperative level window
		// handle has already been set. It cannot
		// be reset while the process has surfaces or palettes created.
		DDERR( IDS_DDERR_HWNDALREADYSET);
		break;

	case DDERR_HWNDSUBCLASSED:
		// DirectDraw is prevented from restoring state because the
		// DirectDraw cooperative level window handle has been subclassed.
		DDERR( IDS_DDERR_HWNDSUBCLASSED);
		break;

	case DDERR_IMPLICITLYCREATED:
		// The surface cannot be restored because
		// it is an implicitly created surface.
		DDERR( IDS_DDERR_IMPLICITLYCREATED);
		break;

	case DDERR_INCOMPATIBLEPRIMARY:
		// The primary surface creation request
		// does not match with the existing primary surface.
		DDERR( IDS_DDERR_INCOMPATIBLEPRIMARY);
		break;

	case DDERR_INVALIDCAPS:
		// One or more of the capability bits
		// passed to the callback function are incorrect.
		DDERR( IDS_DDERR_INVALIDCAPS);
		break;

	case DDERR_INVALIDCLIPLIST:
		// DirectDraw does not support the provided clip list.
		DDERR( IDS_DDERR_INVALIDCLIPLIST);
		break;

	case DDERR_INVALIDDIRECTDRAWGUID:
		// The globally unique identifier (GUID) passed to the
		// DirectDrawCreate function is not a valid DirectDraw driver identifier.
		DDERR( IDS_DDERR_INVALIDDIRECTDRAWGUID);
		break;

	case DDERR_INVALIDMODE:
		// DirectDraw does not support the requested mode.
		DDERR( IDS_DDERR_INVALIDMODE);
		break;

	case DDERR_INVALIDOBJECT:
		// DirectDraw received a pointer that was an invalid DirectDraw object.
		DDERR( IDS_DDERR_INVALIDOBJECT);
		break;

	case DDERR_INVALIDPARAMS:
		// One or more of the parameters passed to the method are incorrect.
		DDERR( IDS_DDERR_INVALIDPARAMS);
		break;

	case DDERR_INVALIDPIXELFORMAT:
		// The pixel format was invalid as specified.
		DDERR( IDS_DDERR_INVALIDPIXELFORMAT);
		break;

	case DDERR_INVALIDPOSITION:
		// The position of the overlay on the destination is no longer legal.
		DDERR( IDS_DDERR_INVALIDPOSITION);
		break;

	case DDERR_INVALIDRECT:
		// The provided rectangle was invalid.
		DDERR( IDS_DDERR_INVALIDRECT);
		break;

	case DDERR_INVALIDSURFACETYPE:
		// The requested operation could not be performed
		// because the surface was of the wrong type.
		DDERR( IDS_DDERR_INVALIDSURFACETYPE);
		break;

	case DDERR_LOCKEDSURFACES:
		// One or more surfaces are locked,
		// causing the failure of the requested operation.
		DDERR( IDS_DDERR_LOCKEDSURFACES);
		break;

	case DDERR_MOREDATA:
		// There is more data available than the specified
		// buffer size could hold.
		DDERR( IDS_DDERR_MOREDATA);
		break;

	case DDERR_NO3D:
		// No 3D hardware or emulation is present.
		DDERR( IDS_DDERR_NO3D);
		break;

	case DDERR_NOALPHAHW:
		// No alpha acceleration hardware is present or available,
		// causing the failure of the requested operation.
		DDERR( IDS_DDERR_NOALPHAHW);
		break;

	case DDERR_NOBLTHW:
		// No blitter hardware is present.
		DDERR( IDS_DDERR_NOBLTHW);
		break;

	case DDERR_NOCLIPLIST:
		// No clip list is available.
		DDERR( IDS_DDERR_NOCLIPLIST);
		break;

	case DDERR_NOCLIPPERATTACHED:
		// No DirectDrawClipper object is attached to the surface object.
		DDERR( IDS_DDERR_NOCLIPPERATTACHED);
		break;

	case DDERR_NOCOLORCONVHW:
		// The operation cannot be carried out because
		// no color-conversion hardware is present or available.
		DDERR( IDS_DDERR_NOCOLORCONVHW);
		break;

	case DDERR_NOCOLORKEY:
		// The surface does not currently have a color key.
		DDERR( IDS_DDERR_NOCOLORKEY);
		break;

	case DDERR_NOCOLORKEYHW:
		// The operation cannot be carried out because there
		// is no hardware support for the destination color key.
		DDERR( IDS_DDERR_NOCOLORKEYHW);
		break;

	case DDERR_NOCOOPERATIVELEVELSET:
		// A create function is called without the
		// IDirectDraw2::SetCooperativeLevel method being called.
		DDERR( IDS_DDERR_NOCOOPERATIVELEVELSET);
		break;

	case DDERR_NODC:
		// No DC has ever been created for this surface.
		DDERR( IDS_DDERR_NODC);
		break;

	case DDERR_NODDROPSHW:
		// No DirectDraw raster operation (ROP) hardware is available.
		DDERR( IDS_DDERR_NODDROPSHW);
		break;

	case DDERR_NODIRECTDRAWHW:
		// Hardware-only DirectDraw object creation is not possible;
		// the driver does not support any hardware.
		DDERR( IDS_DDERR_NODIRECTDRAWHW);
		break;

	case DDERR_NODIRECTDRAWSUPPORT:
		// DirectDraw support is not possible with the current display driver.
		DDERR( IDS_DDERR_NODIRECTDRAWSUPPORT);
		break;

	case DDERR_NOEMULATION:
		// Software emulation is not available.
		DDERR( IDS_DDERR_NOEMULATION);
		break;

	case DDERR_NOEXCLUSIVEMODE:
		// The operation requires the application to have
		// exclusive mode, but the application does not have exclusive mode.
		DDERR( IDS_DDERR_NOEXCLUSIVEMODE);
		break;

	case DDERR_NOFLIPHW:
		// Flipping visible surfaces is not supported.
		DDERR( IDS_DDERR_NOFLIPHW);
		break;

	case DDERR_NOGDI:
		// No GDI is present.
		DDERR( IDS_DDERR_NOGDI);
		break;

	case DDERR_NOHWND:
		// Clipper notification requires a window handle,
		// or no window handle has been previously set
		// as the cooperative level window handle.
		DDERR( IDS_DDERR_NOHWND);
		break;

	case DDERR_NOMIPMAPHW:
		// The operation cannot be carried out because no
		// mipmap texture mapping hardware is present or available.
		DDERR( IDS_DDERR_NOMIPMAPHW);
		break;

	case DDERR_NOMIRRORHW:
		// The operation cannot be carried out because
		// no mirroring hardware is present or available.
		DDERR( IDS_DDERR_NOMIRRORHW);
		break;

	case DDERR_NONONLOCALVIDMEM:
		// An attempt was made to allocate non-local video memory
		// from a device that does not support non-local video memory.
		DDERR( IDS_DDERR_NONONLOCALVIDMEM);
		break;

	case DDERR_NOOVERLAYDEST:
		// The IDirectDrawSurface2::GetOverlayPosition method
		// is called on an overlay that the IDirectDrawSurface2::UpdateOverlay
		// method has not been called on to establish a destination.
		DDERR( IDS_DDERR_NOOVERLAYDEST);
		break;

	case DDERR_NOOVERLAYHW:
		// The operation cannot be carried out because
		// no overlay hardware is present or available.
		DDERR( IDS_DDERR_NOOVERLAYHW);
		break;

	case DDERR_NOPALETTEATTACHED:
		// No palette object is attached to this surface.
		DDERR( IDS_DDERR_NOPALETTEATTACHED);
		break;

	case DDERR_NOPALETTEHW:
		// There is no hardware support for 16- or 256-color palettes.
		DDERR( IDS_DDERR_NOPALETTEHW);
		break;

	case DDERR_NORASTEROPHW:
		// The operation cannot be carried out because
		// no appropriate raster operation hardware is present or available.
		DDERR( IDS_DDERR_NORASTEROPHW);
		break;

	case DDERR_NOROTATIONHW:
		// The operation cannot be carried out because
		// no rotation hardware is present or available.
		DDERR( IDS_DDERR_NOROTATIONHW);
		break;

	case DDERR_NOSTRETCHHW:
		// The operation cannot be carried out because
		// there is no hardware support for stretching.
		DDERR( IDS_DDERR_NOSTRETCHHW);
		break;

	case DDERR_NOT4BITCOLOR:
		// The DirectDrawSurface object is not using a
		// 4-bit color palette and the requested operation
		// requires a 4-bit color palette.
		DDERR( IDS_DDERR_NOT4BITCOLOR);
		break;

	case DDERR_NOT4BITCOLORINDEX:
		// The DirectDrawSurface object is not using a 4-bit
		// color index palette and the requested operation
		// requires a 4-bit color index palette.
		DDERR( IDS_DDERR_NOT4BITCOLORINDEX);
		break;

	case DDERR_NOT8BITCOLOR:
		// The DirectDrawSurface object is not using an 8-bit
		// color palette and the requested operation requires
		// an 8-bit color palette.
		DDERR( IDS_DDERR_NOT8BITCOLOR);
		break;

	case DDERR_NOTAOVERLAYSURFACE:
		// An overlay component is called for a non-overlay surface.
		DDERR( IDS_DDERR_NOTAOVERLAYSURFACE);
		break;

	case DDERR_NOTEXTUREHW:
		// The operation cannot be carried out because no
		// texture-mapping hardware is present or available.
		DDERR( IDS_DDERR_NOTEXTUREHW);
		break;

	case DDERR_NOTFLIPPABLE:
		// An attempt has been made to flip a surface that cannot be flipped.
		DDERR( IDS_DDERR_NOTFLIPPABLE);
		break;

	case DDERR_NOTFOUND:
		// The requested item was not found.
		DDERR( IDS_DDERR_NOTFOUND);
		break;

	case DDERR_NOTINITIALIZED:
		// An attempt was made to call an interface method of a DirectDraw object
		// created by CoCreateInstance before the object was initialized.
		DDERR( IDS_DDERR_NOTINITIALIZED);
		break;

	case DDERR_NOTLOCKED:
		// An attempt is made to unlock a surface that was not locked.
		DDERR( IDS_DDERR_NOTLOCKED);
		fatal = false;
		break;

	case DDERR_NOTPAGELOCKED:
		// An attempt is made to page unlock a surface
		// with no outstanding page locks.
		DDERR( IDS_DDERR_NOTPAGELOCKED);
		break;

	case DDERR_NOTPALETTIZED:
		// The surface being used is not a palette-based surface.
		DDERR( IDS_DDERR_NOTPALETTIZED);
		break;

	case DDERR_NOVSYNCHW:
		// The operation cannot be carried out because
		// there is no hardware support for vertical blank synchronized operations.
		DDERR( IDS_DDERR_NOVSYNCHW);
		break;

	case DDERR_NOZBUFFERHW:
		// The operation to create a z-buffer in display memory
		// or to perform a blit using a z-buffer cannot be carried
		// out because there is no hardware support for z-buffers.
		DDERR( IDS_DDERR_NOZBUFFERHW);
		break;

	case DDERR_NOZOVERLAYHW:
		// The overlay surfaces cannot be z-layered based
		// on the z-order because the hardware does not
		// support z-ordering of overlays.
		DDERR( IDS_DDERR_NOZOVERLAYHW);
		break;

	case DDERR_OUTOFCAPS:
		// The hardware needed for the requested operation has already been allocated.
		DDERR( IDS_DDERR_OUTOFCAPS);
		break;

	case DDERR_OUTOFMEMORY:
		// DirectDraw does not have enough memory to perform the operation.
		DDERR( IDS_DDERR_OUTOFMEMORY);
		break;

	case DDERR_OUTOFVIDEOMEMORY:
		// DirectDraw does not have enough display memory to perform the operation.
		DDERR( IDS_DDERR_OUTOFVIDEOMEMORY);
		break;

	case DDERR_OVERLAYCANTCLIP:
		// The hardware does not support clipped overlays.
		DDERR( IDS_DDERR_OVERLAYCANTCLIP);
		break;

	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
		// An attempt was made to have more than one color key active on an overlay.
		DDERR( IDS_DDERR_OVERLAYCOLORKEYONLYONEACTIVE);
		break;

	case DDERR_OVERLAYNOTVISIBLE:
		// The IDirectDrawSurface2::GetOverlayPosition method is called on a hidden overlay.
		DDERR( IDS_DDERR_OVERLAYNOTVISIBLE);
		break;

	case DDERR_PALETTEBUSY:
		// Access to this palette is refused
		// because the palette is locked by another thread.
		DDERR( IDS_DDERR_PALETTEBUSY);
		break;

	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		// This process has already created a primary surface.
		DDERR( IDS_DDERR_PRIMARYSURFACEALREADYEXISTS);
		break;

	case DDERR_REGIONTOOSMALL:
		// The region passed to the
		// IDirectDrawClipper::GetClipList method is too small.
		DDERR( IDS_DDERR_REGIONTOOSMALL);
		break;

	case DDERR_SURFACEALREADYATTACHED:
		// An attempt was made to attach a surface to
		// another surface to which it is already attached.
		DDERR( IDS_DDERR_SURFACEALREADYATTACHED);
		break;

	case DDERR_SURFACEALREADYDEPENDENT:
		// An attempt was made to make a surface a dependency
		// of another surface to which it is already dependent.
		DDERR( IDS_DDERR_SURFACEALREADYDEPENDENT);
		break;

	case DDERR_SURFACEBUSY:
		// Access to the surface is refused because the
		// surface is locked by another thread.
		DDERR( IDS_DDERR_SURFACEBUSY);
		fatal = false;
		break;

	case DDERR_SURFACEISOBSCURED:
		// Access to the surface is refused
		// because the surface is obscured.
		DDERR( IDS_DDERR_SURFACEISOBSCURED);
		fatal = false;
		break;

	case DDERR_SURFACELOST:
		// Access to the surface is refused because the
		// surface memory is gone. The DirectDrawSurface
		// object representing this surface should have
		// the IDirectDrawSurface2::Restore method called on it.
		DDERR( IDS_DDERR_SURFACELOST);
		fatal = false;
		break;

	case DDERR_SURFACENOTATTACHED:
		// The requested surface is not attached.
		DDERR( IDS_DDERR_SURFACENOTATTACHED);
		break;

	case DDERR_TOOBIGHEIGHT:
		// The height requested by DirectDraw is too large.
		DDERR( IDS_DDERR_TOOBIGHEIGHT);
		break;

	case DDERR_TOOBIGSIZE:
		// The size requested by DirectDraw is too large.
		// However, the individual height and width are OK.
		DDERR( IDS_DDERR_TOOBIGSIZE);
		break;

	case DDERR_TOOBIGWIDTH:
		// The width requested by DirectDraw is too large.
		DDERR( IDS_DDERR_TOOBIGWIDTH);
		break;

	case DDERR_UNSUPPORTED:
		// The operation is not supported.
		DDERR( IDS_DDERR_UNSUPPORTED);
		break;

	case DDERR_UNSUPPORTEDFORMAT:
		// The FourCC format requested is not supported by DirectDraw.
		DDERR( IDS_DDERR_UNSUPPORTEDFORMAT);
		break;

	case DDERR_UNSUPPORTEDMASK:
		// The bitmask in the pixel format requested is not supported by DirectDraw.
		DDERR( IDS_DDERR_UNSUPPORTEDMASK);
		break;

	case DDERR_UNSUPPORTEDMODE:
		// The display is currently in an unsupported mode.
		DDERR( IDS_DDERR_UNSUPPORTEDMODE);
		break;

	case DDERR_VERTICALBLANKINPROGRESS:
		// A vertical blank is in progress.
		DDERR( IDS_DDERR_VERTICALBLANKINPROGRESS);
		break;

	case DDERR_WASSTILLDRAWING:
		// The previous blit operation that is transferring
		// information to or from this surface is incomplete.
		DDERR( IDS_DDERR_WASSTILLDRAWING);
		fatal = false;
		break;

	case DDERR_WRONGMODE:
//	if (exit_time == UINT_MAX)
//			exit_time = LyraTime() + 5000;
		// This surface cannot be restored because it was created in a different mode.
		//DDERR( IDS_DDERR_WRONGMODE);
		return;

	case DDERR_XALIGN:
		// The provided rectangle was not horizontally aligned on a required boundary.
		DDERR( IDS_DDERR_XALIGN);
		break;

	case DDERR_VIDEONOTACTIVE:
		// The video port is not active
		DDERR( IDS_DDERR_VIDEONOTACTIVE);
		break;

	default:
		DDERR( IDS_UNK_DD_ERROR);
		break;
	}

	error_count++;

	if (fatal || (error_count == ERROR_THRESHHOLD))
#if defined (UL_DEBUG) || defined (GAMEMASTER)
		ErrAndExit(GENERIC_ERR, ddErr, nLine, _T(__FILE__));
#else
		ErrAndExit(GENERIC_ERR, ddErr, nLine, _T(""));
#endif
	//else // uncomment to enable bug logging of warnings
	// Warn(ddErr, nLine, sFile);

	return;
}


#ifdef CHECK_INVARIANTS
void cDDraw::CheckInvariants(int line, TCHAR *file)
{

}
#endif


