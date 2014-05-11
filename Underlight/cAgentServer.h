#ifndef AGENTSERVER_H
#define AGENTSERVER_H

// Copyright Lyra LLC, 1996. All rights reserved. 

#include "Central.h"
#include <winsock2.h>
#include "LmMessage.h"
#include "AMsg_All.h"

//////////////////////////////////////////////////////////////////
// Constants

///////////////////////////////////////////////////////////////////
// Additional Windows Messages

#define WM_AGENT_SERVER_DATA		 WM_USER + AS_MAGIC + 1

///////////////////////////////////////////////////////////////////
// Structures

//////////////////////////////////////////////////////////////////
// Function Prototypes

//////////////////////////////////////////////////////////////////
// Class Definition


class cAgentServer
   {
   public:

   private:
	   SOCKET		sd_agents;
	   in_addr		localIP; // local IP address
	   bool			logged_into_daemon; // true after login_ack
	   int			version; // version # of server software
	   int			header_bytes_read, body_bytes_read;
	   bool			reading_header;
	   LmMesgBuf	msgbuf; // message buffer used to read in network messages
	   LmMesgBuf	sendbuf; // message buffer used to send messages
	   LmMesgHdr	msgheader; // message header

	   TCHAR		username[Lyra::PLAYERNAME_MAX];
	   TCHAR		password[Lyra::PASSWORD_MAX];
	   char			server_ip[20];

   public:
      cAgentServer(void);
	  ~cAgentServer(void);

	  // received input from server - tcp and udp
	  void OnServerUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam);
	  void HandleMessage(void);

	  // commands to daemon
	  void SendControlMessage(lyra_id_t id, int command);

	  // login/logout methods
	  void Login(TCHAR *uname, TCHAR *passwd, TCHAR *server);
	  void Logout(void);


	  // Selectors
	  inline bool LoggedIn(void) { return logged_into_daemon; };
	  inline int Version(void) { return version;};
	  inline in_addr LocalIP()   { return localIP;   };

   private:

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cAgentServer(const cAgentServer& x);
	cAgentServer& operator=(const cAgentServer& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif


   };
#endif
