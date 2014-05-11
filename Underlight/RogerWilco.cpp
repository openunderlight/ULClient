//
// RogerWilco.cpp
//
// This module handles the integration with the Roger Wilco 
// voice library. 
//

#include "Resource.h"
#include "cChat.h"
#include "Options.h"
#include "RogerWilco.h"

extern void* voicePipeline;
extern void* channel;
extern options_t options;
extern cChat *display;
extern HINSTANCE hInstance;

#ifndef RW_ENABLED //UL_DEBUG

void  HandleRWMessage(int message_id, RWNET_CHANNELDESC *cd, RWNET_MEMBERDESC* md) {}
void  __cdecl StartRogerWilco( HWND mainWindow ) {}
bool  JoinChannel( HWND notifyWnd, const char* host, const char* channelName, 
				  const char* password, const char* callsign) {	return true; }
void  LeaveChannel() {}
void  __cdecl ShutdownRogerWilco() {}
void  Record( bool start ) {}

#else

void  HandleRWMessage(int message_id, RWNET_CHANNELDESC *cd, RWNET_MEMBERDESC* md)
{
	switch (message_id)
	{
	case RWNETMSG_NEWMEMBER: // Sent when a member joins a channel. 
		LoadString(hInstance, IDS_RWJOIN, message, sizeof(message));
		//_stprintf(message, disp_message, md->callsign);
		display->DisplayMessage(message, true);
		break;
	case RWNETMSG_REMOVEMEMBER: // Sent when a member leaves a channel. The member idSent with the message should be used only to clean up any per-member structures you might be maintaining. 
		LoadString(hInstance, IDS_RWLEAVE, message, sizeof(message));
		//_stprintf(message, disp_message, md->callsign);
		display->DisplayMessage(message, true);
		break;

	case RWNETMSG_NOSUCHCHANNEL: 
		// Sent if you are unable to join a channel because you specified an invalid host or channel name in the channel descriptor passed to RWNet_JoinChannel(). 
	case RWNETMSG_BADPASSWORD: 
		// Sent if you are unable to join a channel because you specified an incorrect channel password. 
	case RWNETMSG_CHANNELFULL: 
		// Sent if you are unable to join a channel because the maximum number of users is already on the channel. 
		LoadString(hInstance, IDS_RWERROR, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message, true);

		break;

		// unhandled messages
	case RWNETMSG_REMOVECHANNEL:
		// Sent when the local host leaves a channel. The channel identifier passed with this message is invalid, and must not be passed to any RWNet_ functions (you should use it only to clean up any structures you might have associated with the old channel). 
	case RWNETMSG_CHANGECALLSIGN:
		// Sent when a member's callsign changes. 
	case RWNETMSG_CHANGEFLAGS: 
		// Sent when a member's flags change. 
	case RWNETMSG_CHANGEPUBFLAGS: 
		// Sent when a member's public_flags change. 
	case RWNETMSG_KICKED: 
		// Sent when a member is kicked off by the channel host. 
		
	default:
		break;
	}
}


void __cdecl StartRogerWilco( HWND mainWindow )
{
	// Initialize Voice pipeline:
	int err = RWVoice_Init();

	// Configure, if necessary:
	if (err == RWVOICE_NO_CONFIG) {
		RWVoice_Configure(NULL, NULL);
	}

	RWVoice_PipelineDesc pd;
	memset( &pd, 0, sizeof(pd) );
	pd.size = sizeof(RWVoice_PipelineDesc);
	voicePipeline = RWVoice_CreatePipeline( &pd );

	switch (RWVoice_GetLastError()) {	// Possible errors:
	case RWVOICE_OK:
		break;
	case RWVOICE_INVALID_STRUCT:
	case RWVOICE_NO_INIT:
		abort();	// Programming error
		break;
	case RWVOICE_DSOUND_INIT:
		MessageBox(NULL, "Playback (DirectSound) initialization failed for Roger Wilco", "Roger Wilco Error", MB_OK);
		break;
	case RWVOICE_WAV_RECORD_INIT:
		MessageBox(NULL, "Recording (WAV) initialization failed for Roger Wilco", "Roger Wilco Error", MB_OK);
		break;
	case RWVOICE_DSCAPTURE_INIT:
		MessageBox(NULL, "Recording (DirectSoundCapture) initialization failed for Roger Wilco", "Roger Wilco Error", MB_OK);
		break;
	default:
		break;
	}


	if (voicePipeline == NULL) {
		RWVoice_Shutdown();
		LoadString(hInstance, IDS_NO_ROGER_WILCO, message, sizeof(message));
		display->DisplayMessage(message, false);
		options.rw = FALSE;
	}

	// Initialize network layer:
	RWNet_Init( 3783, 3783, 0 );	// default upd/tcp ports
}

bool  JoinChannel( HWND notifyWnd, const char* host, const char* channelName, const char* password, const char* callsign)
{
	RWNET_CHANNELDESC cd;
	memset(&cd, 0, sizeof(cd)); cd.size = sizeof(cd);
	if (host != NULL) {
		_tcsnccpy(&cd.address[0], host, sizeof(cd.address)); cd.valid_fields |= RWNETCHAN_ADDRESS;
	}
	_tcsnccpy(&cd.name[0], channelName, sizeof(cd.name)); cd.valid_fields |= RWNETCHAN_NAME;
	if (password != NULL && _tcslen(password)>0) {
		_tcsnccpy(&cd.password[0], password, sizeof(cd.password)); cd.valid_fields |= RWNETCHAN_PASSWORD;
	}

	cd.recv_proc = RWVoice_Receive; cd.valid_fields |= RWNETCHAN_RECVPROC;
	cd.recv_arg = voicePipeline; cd.valid_fields |= RWNETCHAN_RECVARG;
	cd.notify_arg = notifyWnd; cd.valid_fields |= RWNETCHAN_NOTIFYARG;

	RWNET_MEMBERDESC md;
	memset(&md, 0, sizeof(md)); md.size = sizeof(md);
	_tcsnccpy(&md.callsign[0], callsign, sizeof(md.callsign)); md.valid_fields |= RWNETMEM_CALLSIGN;
	md.public_flags = 0; md.valid_fields |= RWNETMEM_PUBFLAGS;
	md.relay = 2; md.valid_fields |= RWNETMEM_RELAY;

	channel = RWNet_JoinChannel(&cd, &md);

	if (channel == RWNET_INVALIDCHANNEL) {
		LoadString(hInstance, IDS_RWJOIN_ERROR, message, sizeof(message));
		display->DisplayMessage(message, true);
		return false;
	}

	// Tell voice layer to send data through the net layer:
	RWVoice_SetNetSendProc( voicePipeline, RWNet_Send, channel );
	return true;
}

void  LeaveChannel()
{
	if (channel != RWNET_INVALIDCHANNEL) {
		// First: make sure voice layer won't send any data:
		RWVoice_SetNetSendProc( voicePipeline, NULL, NULL );

		RWNet_LeaveChannel( channel );
		channel = RWNET_INVALIDCHANNEL;
		LoadString(hInstance, IDS_BREAK_TELEPATHY, message,sizeof(message));
		display->DisplayMessage(message, false);
	}
}

void __cdecl ShutdownRogerWilco()
{
	LeaveChannel();
	RWNet_Shutdown();

	if (voicePipeline != NULL) {
		// NOTE! You must leave the channel before calling
		// DestroyPipeline/Voice_Shutdown, otherwise the network
		// layer may try to send a packet to a non-existent pipeline!
		RWVoice_DestroyPipeline( voicePipeline );
		RWVoice_Shutdown();
	}
}

void  Record( bool start )
{
	if (channel == RWNET_INVALIDCHANNEL) 
		return;

	if ( start ) 
		RWVoice_StartRecording(voicePipeline, FALSE);
	else 
		RWVoice_StopRecording(voicePipeline);
}

#endif

