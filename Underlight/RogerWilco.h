 // Header file for the Actor class

// Copyright Lyra LLC, 1999. All rights reserved. 

#ifndef ROGERWILCO_H
#define ROGERWILCO_H

#include "Central.h"
#include "rwvoice.h"	/* Roger Wilco Voice stuff */
#include "rwnet.h"	/* and Roger Wilco Networking stuf */

void __cdecl StartRogerWilco( HWND mainWindow );
void __cdecl ShutdownRogerWilco();
bool  JoinChannel( HWND notifyWnd, const char* host, const char* channelName, const char* password, const char* callsign);
void  LeaveChannel();
void  Record( bool start );
void  HandleRWMessage(int message_id, RWNET_CHANNELDESC *cd, RWNET_MEMBERDESC* md);
#endif