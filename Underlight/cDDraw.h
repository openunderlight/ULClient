#ifndef DDRAW_H
#define DDRAW_H

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "Central.h"
#include <ddraw.h>

//////////////////////////////////////////////////////////////////
// Macros for proper error handling for DirectDraw

#define TRY_DD(exp){{HRESULT rval = exp;if (rval != DD_OK){this->TraceErrorDD(rval, __LINE__);return;}}}


//////////////////////////////////////////////////////////////////
// Constants

const int MAX_RESOLUTIONS=3; // 0 = 640x480, 1 = 800x600, 2 = 1024x768

enum surface_type {
		PRIMARY = 0,
		BACK_BUFFER = 1
};

//////////////////////////////////////////////////////////////////
// Strucures

struct window_pos_t 
{ 
	int x, y, width, height; 
};

struct dialog_pos_t 
{ 
	int x, y; 
};


//////////////////////////////////////////////////////////////////
// Class Definition
enum ePixelFormat { PIXEL_FORMAT_555, PIXEL_FORMAT_565 };

class cDDraw
{ 
   public:	// member variables

   private:

	  int viewx;			// width of rendered view area
	  int viewy;			// height of rendered view area
	  int width;
      int height;
	  int winwidth;		// total area of window
      int winheight;	// total area of window
      int bpp;
      int pitch;
	  int res;				// 0=640x480, 1=800x600, 2=1024x768
	  int error_count;
	  bool windowed;	// true if running in a window
	  ePixelFormat   pixel_format;

      HWND hwnd_main;
      HRESULT status;
      LPDIRECTDRAW7            lpDD;           // DirectDraw object
	  LPDIRECTDRAWCLIPPER     lpDDClipper;
      LPDIRECTDRAWSURFACE7     lpDDSPrimary;   // DirectDraw primary surface
      LPDIRECTDRAWSURFACE7     lpDDSOffscreen;  // DirectDraw back buffer

// methods
   public:
	  cDDraw(	TCHAR *name, TCHAR *title, HINSTANCE hInstance,
		      	WNDPROC wproc, LPCTSTR applicon = NULL, LPCTSTR applcursor = IDC_ARROW, int resolution = 0,
					int x = 0,  int y = 0); 

	  cDDraw(void* window_handle);
	  void InitDDraw();
	  void DestroyDDraw();
      ~cDDraw();

	  // Selectors for parameters
		inline int   Status   (void)  { return status; };
	    inline int   Width    (void)  { return width;  };
		inline int   Height   (void)  { return height; };
		inline int	 Pitch    (void)  { return pitch;  };
		inline int	 ViewX	  (void)  { return viewx;  };
		inline int	 ViewY    (void)  { return viewy;  };
		void		 ViewRect (RECT *rect);
		inline int	 Res      (void)  { return res;	   };
		inline BOOL	 Windowed (void)  { return windowed;};
		int		     ScaletoRes (int value);
		int			 DlgPosX  (HWND hDlg);
		int			 DlgPosY  (HWND hDlg);
		int			 XOffset  (void);
		int			 YOffset  (void);
	 
		inline	ePixelFormat PixelFormat(void) {return pixel_format; };

	  void TraceErrorDD(HRESULT hErr, int nLine);

	  HWND    Hwnd_Main(void);

	  // Video memory operations
	  bool			 EraseSurface(int id);
	  unsigned char* GetSurface(int id);
      void           ReleaseSurface(int id);
	  bool           BlitOffScreenSurface(void);
	  bool		     ShowSplashScreen(void);
	  bool			 ShowIntroBitmap(void);

	  // Other Windows operations
	  void Show(void);

   private:


#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cDDraw(const cDDraw& x);
	cDDraw& operator=(const cDDraw& x);
   };


#endif
