#ifndef AGENTDAEMON_H
#define AGENTDAEMON_H

// Copyright Lyra LLC, 1997. All rights reserved. 

#include "Central.h"
#include <winsock2.h>
#include "LmMessage.h"
#include "AMsg_All.h"

//////////////////////////////////////////////////////////////////
// Constants

const int MAX_CLIENTS = 64;

///////////////////////////////////////////////////////////////////
// Additional Windows Messages

#define WM_DAEMON_LISTEN_DATA		 WM_USER + AGENT_DAEMON_MAGIC + 1
#define WM_DAEMON_CLIENT_DATA		 WM_USER + AGENT_DAEMON_MAGIC + 2
#define TIMER_CLIENT_UPDATE			 WM_USER + AGENT_DAEMON_MAGIC + 3

///////////////////////////////////////////////////////////////////
// Structures

//////////////////////////////////////////////////////////////////
// Function Prototypes

void CALLBACK AgentUpdateTimerCallback (HWND hWindow, UINT uMSG, UINT idEvent, DWORD dwTime);

//////////////////////////////////////////////////////////////////
// Class Definition


class cAgentDaemon
   {
   public:

   private:
	   SOCKET		sd_listen; // main listening socket
	   SOCKET		sd_clients[MAX_CLIENTS]; // per-client sockets
	   sockaddr_in 	addresses[MAX_CLIENTS]; // per-client addresses
	   int			num_clients;
	   int			header_bytes_read[MAX_CLIENTS], body_bytes_read[MAX_CLIENTS];
	   bool			reading_header[MAX_CLIENTS];
	   bool			client_active[MAX_CLIENTS]; // does client want updates
	   LmMesgBuf	msgbuf[MAX_CLIENTS]; // message buffer used to read in network messages
	   LmMesgBuf	sendbuf[MAX_CLIENTS]; // message buffer used to send messages
	   LmMesgHdr	msgheader[MAX_CLIENTS]; // message header

   public:
      cAgentDaemon(void);
	  ~cAgentDaemon(void);

	  // received input from server 
	  void OnListenUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam);
	  void OnClientUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam);

	  void HandleMessage(int client);

	  // send out updates
	  void UpdateClients(void);

	  // login/logout methods
	  bool Init(void);

	  // Selectors

   private:

	  int FindUnusedSocket(void);
	  int LookUpSocket(SOCKET& sock);
	  void DisconnectClient(int client);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cAgentDaemon(const cAgentDaemon& x);
	cAgentDaemon& operator=(const cAgentDaemon& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif


   };
#endif
