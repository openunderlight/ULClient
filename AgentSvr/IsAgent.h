// Copyright Lyra LLC, 1996. All rights reserved.

#ifndef ISAGENT
#define ISAGENT

#ifndef AGENT
#define AGENT
#endif

// agents should always have gm defined
#ifndef GAMEMASTER
#define GAMEMASTER
#endif

/////////////////////////////////////////////////
// Agent-Only Constants

const int MAX_GAMEMASTERS = 128;
const int MAX_AGENTS = 256; // total # of agents available to this daemon
const int MAX_CONCURRENT_AGENTS = 256; // max agent threads per process


// substitution & replacement of globals


/////////////////////////////////////////////////
// Agent-Only External Global Variables

extern int tlsLevel;
extern int tlsPlayer;
extern int tlsGS;
extern int tlsActors;
extern int tlsDD;
extern int tlsOutput;
extern int tlsTiming;
extern int tlsPrimary;
extern int tlsIndex;
extern int num_agents;
extern agent_info_t agent_info[MAX_AGENTS];
extern HWND hwnd_daemon;
extern gamemaster_info_t gamemaster_info[MAX_GAMEMASTERS];
extern bool primary_thread;
extern cAgentDaemon *daemon; 


/////////////////////////////////////////////////
// Agent-Only Functions

int LookUpAgent(lyra_id_t agent_id);
bool InitAgents(void);
void ExitAgents(void);
int  StartAgent(lyra_id_t id, int delay=0);
void StopAgent(int index);
void DeInitAgent(int index);
void KillAgent(int index, int status);
void __cdecl RunAgent( void *dummy );

// Inline methods & substitution macros for thread-specific globals

inline bool PrimaryThread(void) { return *((bool*)TlsGetValue(tlsPrimary)); };

inline cPlayer *GetPlayer(void) { return ((cPlayer*)TlsGetValue(tlsPlayer)); };
#define player GetPlayer()

inline cLevel *GetLevel(void) { return ((cLevel*)(TlsGetValue(tlsLevel))); };
#define level GetLevel()

inline cGameServer *GetGS(void) { return  ((cGameServer*)(TlsGetValue(tlsGS))); };
#define gs GetGS()

inline cDDraw *GetDD(void) { return ((cDDraw*)(TlsGetValue(tlsDD))); };
#define cDD GetDD()

inline timing_t *GetTiming(void) { return  ((timing_t*)(TlsGetValue(tlsTiming))); };
#define timing GetTiming()

inline cActorList *GetAL(void) { return ((cActorList*)(TlsGetValue(tlsActors))); };
#define actors GetAL()

inline cOutput *GetOutput(void) { return ((cOutput*)(TlsGetValue(tlsOutput))); };
#define output GetOutput()

inline int AgentIndex(void) { return *((int*)TlsGetValue(tlsIndex)); };

#endif
