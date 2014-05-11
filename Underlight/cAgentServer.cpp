 // A Game Server Class

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include <winsock2.h>
#include "cDDraw.h"
#include "Realm.h"
#include "Dialogs.h"
#include "Resource.h"
#include "cAgentBox.h"
#include "Interface.h"
#include "cAgentServer.h"

#ifdef GAMEMASTER

const int				BUILD=1;
const int				HEADER_SIZE = 4;

const unsigned short AGENT_SERVER_PORT=9900;

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cDDraw *cDD;
extern cAgentBox *agentbox;
extern cAgentServer *as; 


/////////////////////////////////////////////////////////////////
// Class Defintion

// The game server exists to communicate with the agent daemons.

// Constructor

cAgentServer::cAgentServer(void) 
{
	logged_into_daemon = false;
	sd_agents = NULL;

#ifdef AGENT
	return;
#endif

	version = 0; // just in case it's referenced before it's set

#ifdef DEBUG
	Debug_CheckInvariants(0);
#endif

}


// Called whenever new data appears coming from the game server
void cAgentServer::OnServerUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam) 
{
	int iRc; 

	// for sending login message
	AMsg_Login login_msg;
	
	if (wParam != sd_agents) // wrong/invalid socket
	{ 
		return;
	}
	if (WSAGETSELECTEVENT(lParam) == FD_READ) 
	{                                                                    
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)  // error
		{	
			WSASetLastError(iRc);
			SOCKETS_ERROR(iRc);
			return;
		}
	    if (reading_header)
		{
			iRc = recv(sd_agents, (char*)((char*)(msgheader.HeaderAddress())+header_bytes_read), 
				(HEADER_SIZE-header_bytes_read), 0);
			if (iRc == SOCKET_ERROR)
			{
				if ((iRc=WSAGetLastError()) && (iRc != WSAEWOULDBLOCK))
					SOCKETS_ERROR(iRc);
				return;
			}
		 	header_bytes_read += iRc;
			if (header_bytes_read < HEADER_SIZE)
				return; // bail if we're not done reading the header
			msgheader.SetByteOrder(ByteOrder::NETWORK);
			if (msgheader.MessageSize() > Lyra::MSG_MAX_SIZE) 
			{
				GAME_ERROR(IDS_MESSAGE_BEYOND_MAX);
				return;
			}
			header_bytes_read = 0; 
			reading_header = FALSE; 
			msgbuf.ReadHeader(msgheader);
		} 

		//_tprintf("Got Game Server message type %d length %d\n", gmsg.AMsg_type, gmsg.AMsg_length);
		iRc=recv(sd_agents, (char*)((char*)(msgbuf.MessageAddress())+body_bytes_read), (msgbuf.MessageSize() - body_bytes_read), 0);
		if (iRc == SOCKET_ERROR)
		{
			if ((iRc=WSAGetLastError()) && (iRc != WSAEWOULDBLOCK))
				SOCKETS_ERROR(iRc);
			return;
		}
		body_bytes_read+=iRc;
		if (body_bytes_read < msgheader.MessageSize())
			return; // we have a partial read, so bail
		body_bytes_read = 0;
		reading_header = TRUE;
		this->HandleMessage();
		return;
	}				 
	else if (WSAGETSELECTEVENT(lParam) == FD_CONNECT)
	{	 // connected - now log in
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)  // error
		{	
			LoadString (hInstance, IDS_AGENT_LOGIN_ERROR, message, sizeof(message));
			CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
						cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
			agentbox->Hide();
			return;
		}

		login_msg.Init(BUILD, username, password);
		sendbuf.ReadMessage(login_msg);
		send (sd_agents, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
		return;
	}
	else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
	{
		//if ((iRc = WSAGETSELECTERROR(lParam)) != 0)  // error
		//{	
		//	WSASetLastError(iRc);
		//	SOCKETS_ERROR(iRc);
		//	return;
		//}
		if (logged_into_daemon)
		{  
			this->Logout();
			LoadString (hInstance, IDS_DAEMON_DISCONNECT, message, sizeof(message));
			CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
						cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
		}
	}

	return;
}

// an incoming message has been received - handle it!
void cAgentServer::HandleMessage(void)
{
	int i;
	AMsg_LoginAck loginack_msg;
	AMsg_AgentInfo info_msg;
	AMsg_PosessInfo posess_msg;

	// for receiving as messages
	switch (msgheader.MessageType())
	{
		case AMsg::LOGINACK: // login ack, check version...
			if (loginack_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERROR_READING_LOGINACK); return; }
			version = loginack_msg.Version();
			if (BUILD < version)
			{
				LoadString (hInstance, IDS_AGENT_VERSION_EXPIRED, message, sizeof(message));
				CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
				this->Logout();
				return;
			}
			switch (loginack_msg.Status())
			{
				case AMsg_LoginAck::LOGIN_OK:
					break;
				case AMsg_LoginAck::LOGIN_USERNOTFOUND:
					LoadString (hInstance, IDS_LOGIN_UNKNOWN_USER, message, sizeof(message));
					CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					this->Logout();
					return;
				case AMsg_LoginAck::LOGIN_BADPASSWORD:
					LoadString (hInstance, IDS_LOGIN_BADPASSWORD, message, sizeof(message));
					CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					this->Logout();
					return;
				default:
					LoadString (hInstance, IDS_LOGIN_UNKNOWN, message, sizeof(message));
					CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR), 
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
					this->Logout();
					return;
			}
			// if we get to here, login was OK....
			logged_into_daemon = true;
			// add agents
			for (i=0; i<loginack_msg.NumAgents(); i++)
				agentbox->AddAgent(loginack_msg.ID(i).id, loginack_msg.ID(i).name, 
						loginack_msg.ID(i).level_id, loginack_msg.ID(i).type);
			agentbox->Show();
			break;

		case AMsg::AGENTINFO: 
			if (info_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERROR_READ_AGENT_INFO_MSG); return; }
			agentbox->UpdateAgent(info_msg);
			break;

		case AMsg::POSESSINFO: 
			if (posess_msg.Read(msgbuf) < 0) { GAME_ERROR(IDS_ERROR_READ_AGENT_POSSES_MSG); return; }
			agentbox->Posess(posess_msg);
			break;

		default:
			break;
	}
}




// Log into agent daemon

void cAgentServer::Login(TCHAR *uname, TCHAR *passwd, TCHAR *server)
{
	struct sockaddr_in saddr;
	int				iRc;
	char 			szIPAddr[255];
	LPHOSTENT		lphp;

	if (sd_agents != NULL)
		this->Logout();

	_tcscpy(username, uname);
	_tcscpy(password, passwd);
#ifdef UNICODE
	wcstombs(server_ip, server, strlen(server_ip));
#else
	strcpy(server_ip, server);
#endif

	header_bytes_read = body_bytes_read = 0;
	reading_header = true;

	// find local IP address
	iRc = gethostname( szIPAddr,255 );
	if (iRc == SOCKET_ERROR)
	{
		LoadString (hInstance, IDS_NETWORK_INIT_ERROR, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return;
	}
	lphp = gethostbyname( szIPAddr );
	if (lphp == NULL)
	{
		LoadString (hInstance, IDS_NETWORK_INIT_ERROR, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return;
	}

	localIP = *(struct in_addr *) (lphp->h_addr);

	// set up connection to agent daemon

	if (sd_agents != NULL)
		closesocket(sd_agents);

	sd_agents = socket(PF_INET, SOCK_STREAM, 0);
	if (sd_agents == INVALID_SOCKET)
	{
		SOCKETS_ERROR(0);
		return;
	}

	// go async...
	iRc = WSAAsyncSelect( sd_agents, cDD->Hwnd_Main(), WM_AGENT_SERVER_DATA,
							FD_READ | FD_CLOSE | FD_CONNECT); 
	if (iRc == SOCKET_ERROR)
	{
		SOCKETS_ERROR(0);
		return;
	}

	saddr.sin_family = PF_INET;
	saddr.sin_port=htons( AGENT_SERVER_PORT );
	saddr.sin_addr.s_addr = inet_addr ( server_ip );

	iRc = connect( sd_agents, (struct sockaddr *) &saddr,
					sizeof(saddr) );
	// will yield a blocking error...
	if (iRc == SOCKET_ERROR)
		if ((iRc=WSAGetLastError()) && (iRc != WSAEWOULDBLOCK))
			SOCKETS_ERROR(iRc);
	return;
}

void cAgentServer::SendControlMessage(lyra_id_t id, int command)
{
	AMsg_ControlAgent control_msg;
	
	if (logged_into_daemon)
	{
		control_msg.Init(id, command);
		sendbuf.ReadMessage(control_msg);
		send (sd_agents, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);
	}

	return;
}


void cAgentServer::Logout(void)
{
	AMsg_Logout logout_msg;

	if (logged_into_daemon)
	{ 	// send logout message
		logout_msg.Init();
		sendbuf.ReadMessage(logout_msg);
		send (sd_agents, (char *) sendbuf.BufferAddress(), sendbuf.BufferSize(), 0);

		// disable async for close to avoid unnecessary close message
		WSAAsyncSelect( sd_agents, cDD->Hwnd_Main(), WM_AGENT_SERVER_DATA, 0); 
		closesocket(sd_agents);
		sd_agents = NULL;
	}

	logged_into_daemon = false;
	if (agentbox)
	{
		agentbox->RemoveAllAgents();
		agentbox->Hide();
	}

	return;
}

// Destructor
cAgentServer::~cAgentServer()
{
#ifdef AGENT
	return;
#endif

	this->Logout();

	if (sd_agents)
		closesocket(sd_agents);
}


#ifdef CHECK_INVARIANTS
void cAgentServer::CheckInvariants(int line, TCHAR *file)
{

}
#endif


#endif