 // An Agent Daemon Server Class

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "cDDraw.h"
#include "cAI.h"
#include "Realm.h"
#include "cGameServer.h"
#include "cLevel.h"
#include "cOutput.h"
#include "Resource.h"
#include "cAgentDaemon.h"

#undef output // so we can write to the daemon output

const unsigned int CLIENT_UPDATE_INTERVAL = 5000; 
const int BUILD=1;
const int HEADER_SIZE = 4;


//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cDDraw *cDD;
extern cAgentDaemon *daemon; 
extern cOutput *output;

// This helper must exist because you can't use a member function
// as a callback. :(			

void CALLBACK AgentUpdateTimerCallback (HWND hWindow, UINT uMSG, UINT idEvent, DWORD dwTime)
{
	if (daemon)
		daemon->UpdateClients(); 
}


/////////////////////////////////////////////////////////////////
// Class Defintion

// The game server exists to communicate with the agent daemons.

// Constructor

cAgentDaemon::cAgentDaemon(void) 
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		sd_clients[i] = NULL;
		addresses[i].sin_addr.s_addr = INADDR_NONE;
	}

	num_clients = 0;

	// set up timer for sending out stat update
	if (!SetTimer(cDD->Hwnd_Main(), TIMER_CLIENT_UPDATE, CLIENT_UPDATE_INTERVAL, 
		(TIMERPROC)AgentUpdateTimerCallback))
		WINDOWS_ERROR();

#ifdef DEBUG
	Debug_CheckInvariants(0);
#endif

}

// set up listening sockets, etc.
bool cAgentDaemon::Init(void)
{
	struct sockaddr_in saddr;
	int iRc;
	char 			szIPAddr[255];
	LPHOSTENT		lphp;
	in_addr			localIP; 

	// find local IP address
	iRc = gethostname( szIPAddr,255 );
	if (iRc == SOCKET_ERROR)
	{
	_tprintf(_T("Couldn't get local host name!\n"));
		LoadString (hInstance, IDS_NETWORK_INIT_ERROR, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return false;
	}
	lphp = gethostbyname( szIPAddr );
	if (lphp == NULL)
	{
	_tprintf(_T("Couldn't get local host name!\n"));
		LoadString (hInstance, IDS_NETWORK_INIT_ERROR, disp_message, sizeof(disp_message));
		GAME_ERROR(disp_message);
		return false;
	}

	localIP = *(struct in_addr *) (lphp->h_addr);

	sd_listen = socket(PF_INET, SOCK_STREAM, 0);
	if (sd_listen == INVALID_SOCKET)
	{
	_tprintf(_T("Couldn't create socket!\n"));
		SOCKETS_ERROR(0);
		return false;
	}

	saddr.sin_family = PF_INET;
	saddr.sin_port = htons( AMsg::AGENT_DAEMON_PORT );
	saddr.sin_addr = localIP;

	iRc = bind( sd_listen, (struct sockaddr*) &saddr,
						 sizeof(saddr) );
	if (iRc == SOCKET_ERROR) 
	{
	_tprintf(_T("Couldn't bind to socket!\n"));
		SOCKETS_ERROR(0); // couldn't create input socket
		return false;
	}
	
	// listen for new connections...
	iRc = listen(sd_listen,5); 
	if (iRc == SOCKET_ERROR)
	{
	_tprintf(_T("Couldn't listen at socket!\n"));
		SOCKETS_ERROR(0);
		return false;
	}

	// go async...
	iRc = WSAAsyncSelect( sd_listen, cDD->Hwnd_Main(), WM_DAEMON_LISTEN_DATA, FD_ACCEPT); 
	if (iRc == SOCKET_ERROR)
	{
	_tprintf(_T("Couldn't go async!\n"));
		SOCKETS_ERROR(0);
		return false;
	}

_tprintf(_T("Agent daemon initialized correctly!\n"));
	return true;
}

// Called whenever new data appears for the listening socket
void cAgentDaemon::OnListenUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam) 
{
	int iRc,i,len; 

	if (wParam != sd_listen) // wrong/invalid socket
	{ 
		return;
	}
	if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT) 
	{        		
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)  // error
		{	
		_tprintf(_T("Error %d on receipt of attempt message\n"),iRc);
			return;
		}
		i = FindUnusedSocket();
		if (i == -1) // too many clients
		{
		_tprintf(_T("Too many connections - client refused\n"));
			return;
		}

		len = sizeof(addresses[i]);
		
		sd_clients[i] = accept(sd_listen, (struct sockaddr *)&(addresses[i]), &len);

		if (sd_clients[i] == INVALID_SOCKET)
		{
		_tprintf(_T("Error %d on attempted accept\n"),iRc);
			return;
		}

		iRc = WSAAsyncSelect( sd_clients[i], cDD->Hwnd_Main(), WM_DAEMON_CLIENT_DATA,
								FD_READ | FD_CLOSE); 
		if (iRc == SOCKET_ERROR)
		{
		_tprintf(_T("Error %d on attempt to go async for client %s\n"),iRc,inet_ntoa(addresses[i].sin_addr));
			this->DisconnectClient(0);
			return;
		}

		reading_header[i] = client_active[i] = true;
		body_bytes_read[i] = header_bytes_read[i] = 0;

		num_clients++;
	_tprintf(_T("Got connection from %s!\n"),inet_ntoa(addresses[i].sin_addr));
		return;
	}	

	return;
}



// Called whenever new data appears coming from the game server
void cAgentDaemon::OnClientUpdate(HWND hWindow, WPARAM wParam, LPARAM lParam) 
{
	int client,iRc; 

	client = LookUpSocket((SOCKET)wParam);

	if (client == -1) // wrong/invalid socket
		return;
	
	if (WSAGETSELECTEVENT(lParam) == FD_READ) 
	{                                                                    
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)  // error
		{	
		_tprintf(_T("Error %d on receipt of read message on client %s\n"),iRc,inet_ntoa(addresses[client].sin_addr));
			this->DisconnectClient(client);
			return;
		}
	    if (reading_header[client])
		{
			iRc = recv(sd_clients[client], (char*)((char*)(msgheader[client].HeaderAddress())+header_bytes_read[client]), 
				(HEADER_SIZE-header_bytes_read[client]), 0);
			if (iRc == SOCKET_ERROR)
			{
				if ((iRc = WSAGetLastError()) && (iRc != WSAEWOULDBLOCK))
				{
				_tprintf(_T("Error %d on attept to read header on client %s\n"),iRc,inet_ntoa(addresses[client].sin_addr));
					this->DisconnectClient(client);
				}
				return;
			}

		 	header_bytes_read[client] += iRc;
			if (header_bytes_read[client] < HEADER_SIZE)
				return; // bail if we're not done reading the header
			msgheader[client].SetByteOrder(ByteOrder::NETWORK);
			
			if (msgheader[client].MessageSize() > Lyra::MSG_MAX_SIZE) 
			{
			_tprintf(_T("Message of beyond max length received from client %s\n"),inet_ntoa(addresses[client].sin_addr));
				this->DisconnectClient(client);
				return;
			}
			header_bytes_read[client] = 0; 
			reading_header[client] = false; 
			msgbuf[client].ReadHeader(msgheader[client]);
		} 

		//_tprintf(_T("Got Agent Daemon message size %d type %d\n"), msgheader[client].MessageSize(),msgheader[client].MessageType());
		iRc=recv(sd_clients[client], (char*)((char*)(msgbuf[client].MessageAddress())+body_bytes_read[client]), (msgbuf[client].MessageSize() - body_bytes_read[client]), 0);
		if (iRc == SOCKET_ERROR)
		{
			if ((iRc = WSAGetLastError()) && (iRc != WSAEWOULDBLOCK))
			{
			_tprintf(_T("Error %d on attept to read body on client %s\n"),iRc,inet_ntoa(addresses[client].sin_addr));
				this->DisconnectClient(client);
			}
			return;
		}
		body_bytes_read[client]+=iRc;
		if (body_bytes_read[client] < msgheader[client].MessageSize())
			return; // we have a partial read, so bail
		body_bytes_read[client] = 0;
		reading_header[client] = true;
		this->HandleMessage(client);
		//_tprintf(_T("Handled Agent Daemon message size %d type %d\n"), msgheader[client].MessageSize(),msgheader[client].MessageType());
		return;
	}				 
	else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
	{
	_tprintf(_T("Lost connection to %s\n"),inet_ntoa(addresses[client].sin_addr));
		if ((iRc = WSAGETSELECTERROR(lParam)) != 0)  // error
		_tprintf(_T("Error %d on receipt of close message on client %s\n"),iRc,inet_ntoa(addresses[client].sin_addr));
		this->DisconnectClient(client);
		return;
	}

	return;
}

// an incoming message has been received - handle it!
void cAgentDaemon::HandleMessage(int client)
{
	int i;
	agent_id_t id;

	// incoming messages
	AMsg_Login			login_msg;
	AMsg_ControlAgent	control_msg;
	AMsg_Logout			logout_msg;

	// outgoing messages
	AMsg_LoginAck loginack_msg;
	AMsg_ControlAgentAck controlack_msg;
	AMsg_PosessInfo posess_msg;

	// for receiving as messages
	//_tprintf(_T("handling message of type %d...\n"), msgheader[client].MessageType());
	switch (msgheader[client].MessageType())
	{
		case AMsg::LOGIN:
			if (login_msg.Read(msgbuf[client]) < 0) { GAME_ERROR(IDS_ERROR_READ_GS_LOGIN); return; }
			// authenticate
			for (i=0; i<MAX_GAMEMASTERS; i++)
			{
				if (   (_tcscmp(gamemaster_info[i].name, login_msg.PlayerName())) == 0)
					break;
			}
			if (i == MAX_GAMEMASTERS)
			{   // user not found
				loginack_msg.Init(BUILD, AMsg_LoginAck::LOGIN_USERNOTFOUND, 0);
				sendbuf[client].ReadMessage(loginack_msg);
				send (sd_clients[client], (char *) sendbuf[client].BufferAddress(), sendbuf[client].BufferSize(), 0);
				this->DisconnectClient(client);
				return;
			}
			if (   (_tcscmp(gamemaster_info[i].passwd, login_msg.Password())) != 0)
			{   // incorrect password
				loginack_msg.Init(BUILD, AMsg_LoginAck::LOGIN_BADPASSWORD, 0);
				sendbuf[client].ReadMessage(loginack_msg);
				send (sd_clients[client], (char *) sendbuf[client].BufferAddress(), sendbuf[client].BufferSize(), 0);
				this->DisconnectClient(client);
				return;
			}

			// boot 'em if already logged in
			//if (gamemaster_info[i].logged_in)
			//	this->DisconnectClient(gamemaster_info[i].socket_index);
			
			gamemaster_info[i].socket_index = client;
			gamemaster_info[i].logged_in = true;

			loginack_msg.Init(BUILD, AMsg_LoginAck::LOGIN_OK, num_agents);
			for (i = 0; i<num_agents; i++)
			{
				_tcscpy(id.name, agent_info[i].name);
				id.id = agent_info[i].id;
				id.level_id = agent_info[i].level_id;
				id.type = agent_info[i].type;
				loginack_msg.SetID(i, id);
			}
			sendbuf[client].ReadMessage(loginack_msg);
			send (sd_clients[client], (char *) sendbuf[client].BufferAddress(), sendbuf[client].BufferSize(), 0);
			break;

		case AMsg::CONTROLAGENT:
			if (control_msg.Read(msgbuf[client]) < 0) { GAME_ERROR(IDS_ERROR_READ_GS_LOGOUT); return; }
			switch (control_msg.Command())
			{
				case AMsg_ControlAgent::COMMAND_ENABLE:
					client_active[client] = true;
					break;

				case AMsg_ControlAgent::COMMAND_DISABLE:
					client_active[client] = false;
					break;

				case AMsg_ControlAgent::COMMAND_START:
					StartAgent(control_msg.AgentID());
					break;

				case AMsg_ControlAgent::COMMAND_START_ALL:
					for (i=0; i<num_agents; i++)
						if (agent_info[i].status == AMsg_AgentInfo::STATUS_READY)
							StartAgent(agent_info[i].id, i*10000);
					break;

				case AMsg_ControlAgent::COMMAND_STOP:
					i = LookUpAgent(control_msg.AgentID());
					if (i != -1)
					{
						if (agent_info[i].status == AMsg_AgentInfo::STATUS_POSESSED)
							agent_info[i].status = AMsg_AgentInfo::STATUS_READY;
						else 
							StopAgent(i);
					}
					break;

				case AMsg_ControlAgent::COMMAND_KILL_DAEMON:
					for (i=0; i<num_agents; i++)
						StopAgent(i);
					Exit();
					return;

				case AMsg_ControlAgent::COMMAND_POSESS:
					i = LookUpAgent(control_msg.AgentID());
					if (i != -1)
					{
						StopAgent(i);
						Sleep(5000);
						agent_info[i].status = AMsg_AgentInfo::STATUS_POSESSED;
						posess_msg.Init(AMsg_PosessInfo::OK);
						posess_msg.SetName(agent_info[i].name);
						posess_msg.SetPassword(agent_info[i].passwd);
					}
					else
						posess_msg.Init(AMsg_PosessInfo::FAILED);

					sendbuf[client].ReadMessage(posess_msg);
					send (sd_clients[client], (char *) sendbuf[client].BufferAddress(), sendbuf[client].BufferSize(), 0);
					break;

				default:
					break;
			}
			break;

		case AMsg::LOGOUT: // login ack, check version...
			if (logout_msg.Read(msgbuf[client]) < 0) { GAME_ERROR(IDS_ERROR_READ_GS_LOGOUT); return; }
			this->DisconnectClient(client);
			break;
		default:
			break;
	}
	return;
}

// send updated agent info to all clients
void cAgentDaemon::UpdateClients(void)
{	// loop & send info about every agent to every client
	AMsg_AgentInfo info_msg;
	cAI *ai;
	short kills,deaths,x,y;
	unsigned char busy, room;

	if (num_clients == 0)
		return;

	for (int i=0; i<num_agents; i++)
	{
		ai = (cAI*)agent_info[i].player_ptr;
		kills = deaths = x = y = 0;
		busy = room = 0;
		if (agent_info[i].status == AMsg_AgentInfo::STATUS_RUNNING)
		{
			kills = ai->Kills();
			deaths = ai->Deaths();
			busy = ai->PercentBusy();
//			VERIFY_XY(1);
			x = (short)ai->x;
			y = (short)ai->y;
			if (agent_info[i].gs_ptr && agent_info[i].gs_ptr->LoggedIntoLevel())
				room = ai->Room();
		}
		info_msg.Init(agent_info[i].id, kills, deaths, agent_info[i].framerate, 
			 agent_info[i].status, busy, room, x, y);

		for (int j=0; j<MAX_CLIENTS; j++)
			if (sd_clients[j] && client_active[j])
			{
				sendbuf[j].ReadMessage(info_msg);
				send (sd_clients[j], (char *) sendbuf[j].BufferAddress(), sendbuf[j].BufferSize(), 0);
			}
	}
	
	return;
}


// used when it is necessary to boot a client
void cAgentDaemon::DisconnectClient(int client)
{
	if (sd_clients[client])
	{
		closesocket(sd_clients[client]);
		sd_clients[client] = NULL;
		client_active[client] = false;
		num_clients--; // only decrement when socket is closed + nulled
		for (int i=0; i<MAX_GAMEMASTERS; i++)
			if (gamemaster_info[i].socket_index == client)
			{	// remove gamemaster info
				gamemaster_info[i].socket_index = -1;
				gamemaster_info[i].logged_in = false;
				break;
			}
	_tprintf(_T("disconnected client at %s\n"),inet_ntoa(addresses[client].sin_addr));
	}

	addresses[client].sin_addr.s_addr = INADDR_NONE;
	return;
}

// Used to find an unused socket for accept; returns -1 if all are used.
int cAgentDaemon::FindUnusedSocket(void)
{
	for (int i=0; i<MAX_CLIENTS; i++)
		if (sd_clients[i] == NULL)
			return i;
	return -1;
}

// Used to look up a socket; returns -1 if not found.
int cAgentDaemon::LookUpSocket(SOCKET& sock)
{
	for (int i=0; i<MAX_CLIENTS; i++)
		if (sd_clients[i] == sock)
			return i;
	return -1;
}

// Destructor
cAgentDaemon::~cAgentDaemon()
{
	if (sd_listen)
		closesocket(sd_listen);

	for (int i=0; i<MAX_CLIENTS; i++)
		if (sd_clients[i])
			closesocket(sd_clients[i]);

	if (cDD && cDD->Hwnd_Main())
		KillTimer(cDD->Hwnd_Main(), TIMER_CLIENT_UPDATE );
}


#ifdef CHECK_INVARIANTS
void cAgentDaemon::CheckInvariants(int line, TCHAR *file)
{

}
#endif

