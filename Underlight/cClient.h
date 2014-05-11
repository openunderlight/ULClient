#ifndef CLIENT_H
#define CLIENT_H

// Copyright Lyra LLC, 1997. All rights reserved. 

#include <winsock2.h>
#include "shared/amsg.h"

//////////////////////////////////////////////////////////////////
// Constants

const int BUILD=1;


///////////////////////////////////////////////////////////////////
// Additional Windows Messages

#define WM_AGENT_SERVER_DATA		 (WM_USER + 1)


///////////////////////////////////////////////////////////////////
// Structures

//////////////////////////////////////////////////////////////////
// Function Prototypes


//////////////////////////////////////////////////////////////////
// Class Definition


class cClient
   {
   public:

   private:
	   SOCKET		sd_agent; // main listening socket
	   int header_bytes_read, body_bytes_read;
	   BOOL reading_header;
	   BOOL logged_in;
	   TCHAR username[Lyra::PLAYERNAME_MAX];
	   TCHAR	password[Lyra::PASSWORD_MAX];
	   void (*msg_callback)(amsg_t &amsg); 

   public:
      cClient(HWND hWindow, void (*amsg_callback)(amsg_t &amsg));
	  ~cClient(void);
	  void Connect(char *server, TCHAR *agent_username, TCHAR *agent_password);
	  BOOL Login(void);
	  BOOL Info(realmid_t id);
	  BOOL Start(realmid_t id);
	  BOOL Stop(realmid_t id);
	  void OnServerUpdate(void *hWindow, WPARAM wParam, LPARAM lParam);
	  inline BOOL LoggedIn(void) { return logged_in;};

   private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cClient(const cClient& x);
	cClient& operator=(const cClient& x);

   };
#endif
