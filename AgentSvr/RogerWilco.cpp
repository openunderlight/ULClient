//
// RogerWilco.cpp - agent stubs
//
// This module handles the integration with the Roger Wilco 
// voice library. 
//

#include "Resource.h"
#include "cChat.h"
#include "Options.h"
#include "RogerWilco.h"

void  HandleRWMessage(int message_id, RWNET_CHANNELDESC *cd, RWNET_MEMBERDESC* md) {}

void  __cdecl StartRogerWilco( HWND mainWindow ) {}

bool  JoinChannel( HWND notifyWnd, const char* host, const char* channelName, 
				  const char* password, const char* callsign) {	return true; }

void  LeaveChannel() {}

void  __cdecl ShutdownRogerWilco() {}

void  Record( bool start ) {}
