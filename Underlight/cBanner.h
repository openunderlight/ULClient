// Header file for cBanner

// Copyright Lyra LLC, 1997. All rights reserved. 
/*

#ifndef BANNER_H
#define BANNER_H

#define STRICT

#include "Central.h"
#include <windows.h>
#include <wininet.h>

//////////////////////////////////////////////////////////////////
// Constants

const int BANNER_HEIGHT = 60;
const int BANNER_WIDTH = 480;
const int GIF_BUFFER_SIZE = 12000; // .gifs can be at most 12K
const int MAX_BANNERS = 4;
const int BANNER_TIMER = WM_USER + BANNER_MAGIC + 50;
const int MAX_HTML_SIZE = 512; // max size of HTML page loaded & parsed
const int BANNER_SIZE = BANNER_WIDTH * BANNER_HEIGHT;
const int URL_MAX = 256; // max size of URL for banner

//////////////////////////////////////////////////////////////////
// Enumerations

enum banner_status_t {
	BANNER_LOADED = 1, // banner successfully loaded
	BANNER_NEEDS_URL = 2, // banner needs to retrieve a new URL
	BANNER_LOADING_URL = 3, // in process of getting a new URL
	BANNER_NEEDS_IMAGE = 4, // banner needs to retrieve gif from URL
	BANNER_LOADING_IMAGE = 5, // in process of loading the gif
};


//////////////////////////////////////////////////////////////////
// Friends

LRESULT WINAPI BannerWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void __stdcall InetStatusCallback(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, 
						LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);


//////////////////////////////////////////////////////////////////
// Class Definition


class cBanner
{
public: 

private:
	HINTERNET hInternet; // main handle for WinInet stuff
	HINTERNET hBanner; // handle for downloading HTML pages & gifs
	unsigned char gif_buffer[GIF_BUFFER_SIZE]; // for downloading gifs into
	unsigned char address[MAX_HTML_SIZE]; // for downloading html into
	HBITMAP banners[MAX_BANNERS+1]; // banner bits; extra is for the Underlight banner
	int banner_status[MAX_BANNERS]; // see above enumeration
	int	impressions[MAX_BANNERS]; // # of impressions made by this banner
	char banner_urls[MAX_BANNERS][URL_MAX]; // URL's for current banners
	BITMAPINFO bmheader; // used for creating new bitmaps
	RGBQUAD palettes[MAX_BANNERS+1][256]; // color tables for banners
	
	unsigned long bytes_read;	// number of bytes read by last read file operation

	BOOL getting_html; // true when pulling down HTML
	BOOL getting_gif;  // true when pulling down a .gif
	BOOL opening_url; // true when opening a URL
	BOOL reading_url; // true when reading a URL
	int curr_download; // index of banner for current read/load operation

	HWND hwnd_banner; // handle to banner dummy window
	WNDPROC	lpfn_banner; // pointer to window procedure
	int curr_banner;  // index of currently displayed banner
	
public:
    cBanner(void);
    ~cBanner();
	inline void Show(void) { return; };//ShowWindow( hwnd_banner, SW_SHOWNORMAL );};
	inline HWND Hwnd(void) { return hwnd_banner;};
	void Rotate(); // called by timer to rotate ads

private:
	void Blit(HDC hdc);
	void OpenNewPage(int index);
	void ReadNewPage(void);
	void ParseAddress(void);
	void OpenNewGif(int index);
	void ReadNewGif(void);
	void ProcessGif(void);

	void LoadNewBanner(int index); // load a new ad into this slot
	void LoadGif(char *URL, int index); // load .gif at this URL into index slot

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cBanner(const cBanner& x);
	cBanner& operator=(const cBanner& x);


	// The Window Proc and WinInet callback must be friends...
	friend LRESULT WINAPI BannerWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend void __stdcall InetStatusCallback(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, 
							LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);

#ifdef CHECK_INVARIANTS
		void CheckInvariants(int line, char *file);
#else
		inline void CheckInvariants(int line, char *file) {};
#endif


};

#endif
  */