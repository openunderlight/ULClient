// cBanner: The rich edit control that displays Banner and status messages.

// Copyright Lyra LLC, 1996. All rights reserved. 
/*

#define STRICT

#include <windowsx.h>
#include <memory.h>
#include "cDDraw.h"
#include "Realm.h"
#include "cEffects.h"
#include "cBanner.h"
#include "cGif.h"
#include "Options.h"
#include "cPalettes.h"
#include "cLevel.h"
#include "Mouse.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

extern cDDraw *cDD;
extern HINSTANCE hInstance;
extern cBanner *banner; // needed for window proc
extern cEffects *effects;
extern options_t options;
extern cPaletteManager *shader; // palettes & shading object
extern mouse_look_t mouse_look;
extern mouse_move_t mouse_move;

//////////////////////////////////////////////////////////////////
// Constants

const int NO_BANNER = -1;
const int UNDERLIGHT_BANNER = MAX_BANNERS;
const int ROTATION_INTERVAL = 5000; // rotate every 30 seconds
const int MAX_ROTATIONS = 10; // # of impressions per load a new ad every 20 rotations (10 min)

#define SACRED_URL "http://www.jamisongold.com/~brent/banners.cgi"
#define BANNER_DISABLED

// position for ad banner
const struct window_pos_t bannerPos[MAX_RESOLUTIONS] = 
{
	{ 0, 419, BANNER_WIDTH, BANNER_HEIGHT}, // 640x480
	{ 86, 539, BANNER_WIDTH, BANNER_HEIGHT}, // 800x600
};


/////////////////////////////////////////////////////////////////
// Class Defintion

// This helper must exist because you can't use a member function
// as a callback. :(
void CALLBACK BannerTimerCallback (HWND hWindow, UINT uMSG, UINT idEvent, DWORD dwTime)
{
	banner->Rotate(); 
}

// Constructor
// The colors are palette indexes
cBanner::cBanner(void) 
{

	return; // NO BANNER CURRENTLY
   WNDCLASS wc;
   int		i;
   HKEY reg_key = NULL;
   unsigned long result,size;
   DWORD reg_type;		
   TCHAR keyname[64];
   unsigned char *buffer;
   HDC dc;

   opening_url = reading_url = getting_html = getting_gif = FALSE;
   curr_download = NO_BANNER; 
   curr_banner = UNDERLIGHT_BANNER;
   hBanner = NULL;
   for (i=0; i<MAX_BANNERS; i++)
   {
	   banner_status[i] = BANNER_NEEDS_URL;
	   banners[i] = NULL;
   }

   // Now set up DIB for Underlight banner
	
   bmheader.bmiHeader.biSize = sizeof(bmheader.bmiHeader); 
   bmheader.bmiHeader.biWidth = BANNER_WIDTH; 
   bmheader.bmiHeader.biHeight = -BANNER_HEIGHT; 
   bmheader.bmiHeader.biPlanes = 1; 
   bmheader.bmiHeader.biBitCount = 8; 
   bmheader.bmiHeader.biCompression = BI_RGB; 
   bmheader.bmiHeader.biSizeImage = BANNER_SIZE; 
   bmheader.bmiHeader.biXPelsPerMeter = 0; 
   bmheader.bmiHeader.biYPelsPerMeter = 0; 
   bmheader.bmiHeader.biClrUsed = 256; 
   bmheader.bmiHeader.biClrImportant = 0;

   dc = GetDC(cDD->Hwnd_Main());
   banners[curr_banner] = CreateDIBSection(dc, &bmheader, DIB_RGB_COLORS, 
							(void**)(&buffer), NULL, 0);
   effects->LoadEffectPalette(LyraPalette::BANNER_PALETTE);
   for (i=0; i<256; i++)
   {
		palettes[curr_banner][i].rgbRed = shader->Red(LyraPalette::BANNER_PALETTE,i);
		palettes[curr_banner][i].rgbGreen= shader->Green(LyraPalette::BANNER_PALETTE,i);
		palettes[curr_banner][i].rgbBlue = shader->Blue(LyraPalette::BANNER_PALETTE,i);
		palettes[curr_banner][i].rgbReserved = 0;
   }
   effects->FreeEffectPalette(LyraPalette::BANNER_PALETTE);

   effects->LoadEffectBitmaps(LyraBitmap::BANNER_EFFECT);
   memcpy(buffer, effects->EffectBitmap(LyraBitmap::BANNER_EFFECT)->address, BANNER_SIZE);
   effects->FreeEffectBitmaps(LyraBitmap::BANNER_EFFECT);

   ReleaseDC(cDD->Hwnd_Main(), dc);

   // set up and register window class
   wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
   wc.lpfnWndProc   = BannerWProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInstance;
   wc.hIcon         = NULL;
   wc.hCursor       = NULL;
   wc.hbrBackground = NULL;
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = "Banner";

   RegisterClass( &wc );

   hwnd_banner = CreateWindowEx(
 	    0,
		"Banner", "",
		WS_POPUP | WS_CHILD, 
		bannerPos[cDD->Res()].x, bannerPos[cDD->Res()].y, 		
		bannerPos[cDD->Res()].width, bannerPos[cDD->Res()].height,
		cDD->Hwnd_Main(), NULL, hInstance, NULL );

#ifndef BANNER_DISABLED

	if (options.network == INTERNET)
	{
   		// set up timer for rotating banners + pulling down new ones
		if (!SetTimer(hwnd_banner, BANNER_TIMER, ROTATION_INTERVAL,
			(TIMERPROC)BannerTimerCallback))
			WINDOWS_ERROR();

		hInternet = InternetOpen(NAME, NULL, NULL, NULL, INTERNET_FLAG_ASYNC);
		InternetSetStatusCallback(hInternet, ::InetStatusCallback);
    	
		// read registry to find URL's for banner URL's & impressions
		if (RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_DATA_KEY,0, 
						NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result)
						== ERROR_SUCCESS)
		{	
			for (i=0; i<MAX_BANNERS; i++)
			{
				size = sizeof(banner_urls[i]);
			_stprintf(keyname, "banner_url%d",i);
				//_tprintf("keyname: %s\n",keyname);
				RegQueryValueEx(reg_key, keyname, NULL, &reg_type,
					(unsigned char *)banner_urls[i], &size);
				size = sizeof(impressions[i]);
			_stprintf(keyname, "impressions%d",i);
				//_tprintf("keyname: %s\n",keyname);
				RegQueryValueEx(reg_key, keyname, NULL, &reg_type,
					(unsigned char *)&(impressions[i]), &size);
				//if impressions & URL seem valid, load gif...
				if ((impressions[i] >=0) && (impressions[i] < MAX_ROTATIONS)
					&& (_tcsstr(banner_urls[i], "http") != NULL))
					this->OpenNewGif(i);
				else
					this->OpenNewPage(i);
			}
			RegCloseKey(reg_key);
		}
		else
		{	// can't access registry
			GAME_ERROR("Cannot access registry");
			return;
		}
	}
#endif

	return;

}

// begin operation to load HTML to get a new banner
void cBanner::OpenNewPage(int index)
{
	impressions[index] = -1;
	_tcscpy(banner_urls[index], "");
	if (curr_download != NO_BANNER)
	{	// load already in progres...
		banner_status[index] = BANNER_NEEDS_URL;
//	_tprintf("load url on %d blocked, already in progres...\n",index);
		return;
	}
	//_tprintf("opening new page for index %d...\n",index);

    getting_html = opening_url = TRUE;

    //InternetOpenUrl(hInternet, SACRED_URL, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 1);
    InternetOpenUrl(hInternet, SACRED_URL, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT, 1);

    curr_download = index;
    banner_status[curr_download] == BANNER_LOADING_URL;
}


// we have opened the HTML URL, so now start to read the HTML
void cBanner::ReadNewPage(void)
{
	//_tprintf("reading new page for index %d...\n",curr_download);

    reading_url = TRUE;
    opening_url = FALSE;
   
    InternetReadFile(hBanner, address, MAX_HTML_SIZE, &bytes_read);
}


// we have finished reading the HTML; now parse address out & get the 
// URL for the .gif 
void cBanner::ParseAddress(void)
{
	TCHAR *start_pos, *end_pos;
	int index;

	index = curr_download;
    address[bytes_read] = '\0';
    start_pos = _tcsstr((TCHAR*)address, "http://");
    end_pos = _tcsstr((TCHAR*)address, ".gif");

	if ((start_pos == NULL) || (end_pos == NULL))
	{
		curr_download = NO_BANNER;
	_tprintf("WARNING: got error on loading page...\n");
		this->OpenNewPage(index);
		return;
	}
    address[end_pos - (TCHAR*)address + 4] = '\0';
	_tcscpy(banner_urls[curr_download], start_pos);
	impressions[curr_download] = 0;
	//_tprintf("url: %s\n", banner_urls[curr_download]);
	curr_download = NO_BANNER;
	this->OpenNewGif(index);

}


// note that this may be called either following a page read, or directly
// without the page read if we have the URL but not the image (i.e. registry)

void cBanner::OpenNewGif(int index)
{
	if (curr_download != NO_BANNER)
	{	// another load already in progres...
		banner_status[index] = BANNER_NEEDS_IMAGE;
//	_tprintf("load gif on %d blocked, already in progres...\n",index);
		return;
	}

	curr_download = index;
	banner_status[curr_download] == BANNER_LOADING_IMAGE;

	//_tprintf("opening new gif for index %d...\n",index);
	reading_url = getting_html = FALSE;
	opening_url = getting_gif = TRUE;

    InternetOpenUrl(hInternet, banner_urls[curr_download], NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 1);
}

// pull down the gif specified by URL

void cBanner::ReadNewGif(void)
{
	//_tprintf("reading new gif for index %d...\n",curr_download);
	reading_url = TRUE;
	opening_url = FALSE;
    
	InternetReadFile(hBanner, gif_buffer, GIF_BUFFER_SIZE, &bytes_read);
}

// we have the gif bits; now process
void cBanner::ProcessGif(void)
{
	int i;
    unsigned TCHAR *buffer;
	HDC dc;
	RGB *gif_palette;
	//_tprintf("processing new gif for index %d...\n",curr_download);

	reading_url = getting_gif = FALSE;

	cGif *gif = new cGif(gif_buffer, bytes_read);

	if (gif->Ht() == 0) // error - download a new banner
	{
	_tprintf("got error on loading banner...\n");
		delete gif;
		i = curr_download;
		curr_download = NO_BANNER;
		this->OpenNewPage(i);
		return;
	}
	if (banners[curr_download])
		DeleteObject(banners[curr_download]);

    dc = GetDC(cDD->Hwnd_Main());
    banners[curr_download] = CreateDIBSection(dc, &bmheader, DIB_RGB_COLORS, 
							(void**)(&buffer), NULL, 0);
    gif_palette = gif->Colors();
    for (i=0; i<256; i++)
    {
		palettes[curr_download][i].rgbRed = gif_palette[i].red;
		palettes[curr_download][i].rgbGreen = gif_palette[i].green;
		palettes[curr_download][i].rgbBlue = gif_palette[i].blue;
		palettes[curr_download][i].rgbReserved = 0;
    }

    memcpy(buffer, gif->Address(), BANNER_SIZE);

    ReleaseDC(cDD->Hwnd_Main(), dc);

	delete gif;

	banner_status[curr_download] = BANNER_LOADED;
	curr_download = NO_BANNER;

	// now search the other banners to see if 
	for (i=0; i<MAX_BANNERS; i++)
	{
		if (banner_status[i] == BANNER_NEEDS_URL)
			this->OpenNewPage(i);
		else if (banner_status[i] == BANNER_NEEDS_IMAGE)
			this->OpenNewGif(i);
	}
}


void cBanner::Rotate()
{
//#ifndef _DEBUG
	return;
//#endif
	int banners_checked=0;

	if (curr_banner != UNDERLIGHT_BANNER)
	{  // switch banners if we've shown it enough times
		//_tprintf("banner: %d imp: %d\n",curr_banner, impressions[curr_banner]);
		if (++impressions[curr_banner] >= MAX_ROTATIONS)
			this->OpenNewPage(curr_banner);
	}

	if (curr_banner == UNDERLIGHT_BANNER)
		curr_banner = -1; 

	while (banners_checked < MAX_BANNERS)  
	{	// so find thenext loaded banner and display it
		curr_banner++;
	
		if (curr_banner == MAX_BANNERS)
			curr_banner = 0;
		if (banner_status[curr_banner] == BANNER_LOADED)
			break;
		banners_checked++;
	}

	// if no banner is loaded, use the Underlight banner
	if (banners_checked == MAX_BANNERS)
		curr_banner = UNDERLIGHT_BANNER;

	//_tprintf("rotated to banner %d!\n",curr_banner);
	InvalidateRect(hwnd_banner, NULL, FALSE);

}

void cBanner::Blit(HDC hdc)
{	
	HGDIOBJ old_object;
	HDC bitmap_dc;
	
	bitmap_dc = CreateCompatibleDC(hdc);

	// will select the underlight banner if curr_banner = underlight banner
	old_object = SelectObject(bitmap_dc, banners[curr_banner] );

	SetDIBColorTable(bitmap_dc, 0, 256, palettes[curr_banner]);

	BitBlt(hdc, 0, 0, BANNER_WIDTH, BANNER_HEIGHT, bitmap_dc, 0, 0, SRCCOPY);

	SelectObject(bitmap_dc, old_object);

	DeleteDC(bitmap_dc);

	return;
}

// Destructor
cBanner::~cBanner(void)
{
	return; // NO BANNER CURRENTLY

   HKEY reg_key = NULL;
   unsigned long result;	
   TCHAR keyname[64];
   int i;

   // delete bitmap for Underlight banner
   DeleteObject(banners[MAX_BANNERS]);

#ifndef BANNER_DISABLED

	if (options.network == INTERNET)
	{
		KillTimer( hwnd_banner, BANNER_TIMER );
		if (hBanner) { InternetCloseHandle(hBanner); hBanner = NULL; }
		if (hInternet) { InternetCloseHandle(hInternet); hInternet = NULL; }

		// write out URL names and # of impressions to registry
		RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_DATA_KEY,0, 
						NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
		for (i=0; i<MAX_BANNERS; i++)
		{
			_stprintf(keyname, "banner_url%d",i);
				RegSetValueEx(reg_key, keyname, 0, REG_SZ, 
					(unsigned char *)banner_urls[i], sizeof(banner_urls[i]));
			_stprintf(keyname, "impressions%d",i);
				RegSetValueEx(reg_key, keyname, 0, REG_DWORD,  
					(unsigned char *)&(impressions[i]), sizeof(impressions[i]));
				if (banners[i])
					DeleteObject(banners[i]);
		}
		RegCloseKey(reg_key);
	}
#endif

	return;
}

#ifdef CHECK_INVARIANTS
void cBanner::CheckInvariants(int line, char *file)
{

}
#endif


//////////////////////////////////////////////////////////////////////////
// Helper Functions and Friends

// callback function for WinInet operations
// if any part of the banner loading fails, go back to the
// beginning and start again
void __stdcall InetStatusCallback(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, 
						LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	int i;
	if (!banner) // banner deleted...
		return;
	if (dwInternetStatus == INTERNET_STATUS_REQUEST_COMPLETE)
	{
		LPINTERNET_ASYNC_RESULT results = (LPINTERNET_ASYNC_RESULT)lpvStatusInformation;
		if (banner->getting_html) // pulling down html page
		{
			if (banner->opening_url) // call to OpenURL is finishing
			{
				if (results->dwResult == NULL)
				{	 // error trying to open URL; try again
					i = banner->curr_download;
					banner->curr_download = NO_BANNER;
					banner->OpenNewPage(i);
					return;
				} 
				banner->hBanner = (HINTERNET)results->dwResult;
				banner->ReadNewPage(); // now we can read the page...
			}
			else if (banner->reading_url)
			{
				InternetCloseHandle(banner->hBanner); 
				banner->hBanner = NULL; 
				if (results->dwResult == FALSE)
				{	    
					i = banner->curr_download;
					banner->curr_download = NO_BANNER;
					banner->OpenNewPage(i);
					return;
				} 
				banner->ParseAddress(); // now parse the HTML 
			}
		}
		else if (banner->getting_gif) // pulling down gif
		{
			if (banner->opening_url) // call to OpenURL is finishing
			{
				if (results->dwResult == NULL)
				{	    
					i = banner->curr_download;
					banner->curr_download = NO_BANNER;
					banner->OpenNewPage(i);
					return;
				} 
				banner->hBanner = (HINTERNET)results->dwResult;
				banner->ReadNewGif(); // now we can read the gif
			}
			else if (banner->reading_url)
			{
				InternetCloseHandle(banner->hBanner); 
				banner->hBanner = NULL; 
				if (results->dwResult == FALSE)
				{	    
					i = banner->curr_download;
					banner->curr_download = NO_BANNER;
					banner->OpenNewPage(i);
					return;
				} 
				banner->ProcessGif(); // gif read finished
			}
		}
	}

}

// Subclassed window procedure for the rich edit control
LRESULT WINAPI BannerWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HDC hdc;
	PAINTSTRUCT paint;

	switch(message)
	{
		case WM_KEYDOWN: // send the key to the main window
			SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			SendMessage(cDD->Hwnd_Main(), message,
				(WPARAM) wParam, (LPARAM) lParam);
			return (LRESULT)0;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			return (LRESULT) 0;
		case WM_MOUSEMOVE:
			if (mouse_look.looking || mouse_move.moving)
			{
				StopMouseMove();
				StopMouseLook();
			}
			break;
		case WM_PAINT:
			if (hwnd == banner->Hwnd())
			{
				hdc = BeginPaint(hwnd, &paint);
				banner->Blit(hdc);
				EndPaint(hwnd, &paint);
				return 0L;
			}
			break;
		break;
	}  

	return DefWindowProc(hwnd, message, wParam, lParam);
} 



*/