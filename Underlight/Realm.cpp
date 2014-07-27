
// Realm: master program file

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <locale.h>
#include <commctrl.h>
#include <new.h>
#include <TLHelp32.h>

#include "Agent.h"
#include "4dx.h"
#include "cActorList.h"
#include "cDDraw.h"
#include "cChat.h"
#include "cControlPanel.h"
//#include "cBanner.h"
#include "LmItemDefs.h"
#include "cDSound.h"
#include "cDSound.h"
#include "cWave.h"
#include "Utils.h" 
#include "Interface.h"
#include "cGameServer.h"
#include "cPalettes.h"
#include "cLevel.h"
#include "Move.h"
#include "cPlayer.h"
#include "Resource.h"
#include "Dialogs.h"
#include "cOrnament.h"
#include "Options.h"
#include "cParty.h"
#include "cSending.h"
#include "cOutput.h"
#include "cEffects.h"
#include "cGoalPosting.h"
#include "cReadGoal.h"
#include "cReportGoal.h"
#include "cPostGoal.h"
#include "cReviewGoals.h"
#include "cGoalBook.h"
#include "cReadReport.h"
#include "cDetailGoal.h"
#include "cQuestBuilder.h"
#include "cPostQuest.h"
#include "cDetailQuest.h"
#include "cReadQuest.h"
#include "cAgentBox.h"
#include "cAgentServer.h"
#include "cArts.h"
#include "Mouse.h"
#include "LmItem.h"
#include "LyraDefs.h"
#include "Main.h"
#include "LoginOptions.h"
#include "cKeymap.h"
#include "Realm.h"
#include "Secure.h"
#include "keyboard.h"
//#include "RogerWilco.h"
#include "IconDefs.h"
#include <crtdbg.h>


#ifdef AGENT
#include "cAI.h"
#include "cAgentDaemon.h"
#include <direct.h>
// must undef macros to use global pointers
#undef player
#undef gs
#undef level
#undef output
#undef timing
#undef actors
#undef cDD
#endif


/////////////////////////////////////////////////
// Constants


/////////////////////////////////////////////////
// Global Variables

// Globals specific to each agent thread
cActorList *actors = NULL; // List of actors
cGameServer *gs = NULL; // game server object
cPlayer *player = NULL; // player object
timing_t *timing = NULL; // timer values
cDDraw *cDD = NULL; // Direct Draw object - need separate windows & window procs
cLevel *level = NULL; // level object 
cOutput *output = NULL; // debugging output object

int MAX_LV_ITEMS; // used to be a constant; now must be flexible

// Globals common to all threads

HINSTANCE hInstance = NULL;
HINSTANCE RichEdLibrary = NULL; 
cDSound *cDS = NULL; // Direct Sound object
cChat *display = NULL;  // Chat box rich text control
cControlPanel *cp = NULL; // Control Panel
//cBanner *banner = NULL; // Ad banner object
cEffects *effects = NULL; // effects controller
cPaletteManager *shader; // shader & shading object
cArts *arts = NULL; // arts object
cKeymap *keymap = NULL; // keymap object
cTimedEffects *timed_effects = NULL; // timed effects object
ppoint_t pp; // personality points use tracker

#ifdef GAMEMASTER
cAgentBox *agentbox = NULL; // agent controller, game masters only
cAgentServer *as = NULL; // agent controller networking, game masters only
#endif
#ifdef AGENT
unsigned short agent_gs_port = 0;
char agent_gs_ip_address[16];
int num_logins = 0;
#endif
mouse_move_t mouse_move; // mouse data
mouse_look_t mouse_look; 
bool killed = false; // only set to true after a dreamstrike
bool mouselooking = false; // used to toggle mouselooking on the fly
bool show_training_messages = false; // supplements options.welcome_ai
DWORD last_keystroke;
HFONT display_font[MAX_RESOLUTIONS] = {NULL, NULL, NULL};
HFONT bold_font[MAX_RESOLUTIONS] = {NULL, NULL, NULL}; 
float scale_x = 1.0f; // x scale factor for dialog boxes
float scale_y = 1.0f; // y scale factor for dialog boxes
long time_offset = 0; // used to work around Windows bug that returns negative system time on ME
unsigned long exit_time; 

unsigned char keyboard[num_keystates]; // keyboard
TCHAR message[DEFAULT_MESSAGE_SIZE]; // generic string for output messages
TCHAR disp_message[DEFAULT_MESSAGE_SIZE]; // generic string for output messages
TCHAR temp_message[DEFAULT_MESSAGE_SIZE]; // generic string for output messages
TCHAR errbuf[DEFAULT_MESSAGE_SIZE];// generic string for error messages
TCHAR duration_message[DEFAULT_MESSAGE_SIZE];
TCHAR modifier_message[DEFAULT_MESSAGE_SIZE];
TCHAR guild_name_message[DEFAULT_MESSAGE_SIZE];
TCHAR guild_rank_message[DEFAULT_MESSAGE_SIZE];
TCHAR guild_goal_message[DEFAULT_MESSAGE_SIZE];
TCHAR color_message[DEFAULT_MESSAGE_SIZE];
TCHAR monster_color_message[DEFAULT_MESSAGE_SIZE];
TCHAR token_message[DEFAULT_MESSAGE_SIZE];
TCHAR nightmare_message[DEFAULT_MESSAGE_SIZE];
TCHAR dreamweapon_message[DEFAULT_MESSAGE_SIZE];
TCHAR talisman_message[DEFAULT_MESSAGE_SIZE];


bool showing_map,map_shows_current_level;

// Goal Posting Objects
cGoalPosting *goals = NULL; // goal posting object
cReadGoal *readgoal = NULL; // goal viewing object
cReportGoal *reportgoal = NULL; // goal reporting object
cGoalBook *goalbook = NULL; // goal book object
cPostGoal *postgoal = NULL; // goal posting object
cReviewGoals *reviewgoals = NULL; // goal reviewing object
cReadReport *readreport = NULL; // report viewing object
cDetailGoal *detailgoal = NULL; // goal detail viewing object

// Quest Builder Objects
cQuestBuilder *quests = NULL; // main quest object
cReadQuest *readquest = NULL; // quest viewing object
cPostQuest *postquest = NULL; // quest posting object
cDetailQuest *detailquest = NULL; // quest detail viewing object

// Goal/Quest Shared Objects

HBITMAP *hGoalCheckButtons = NULL;
WNDPROC lpfnGoalPushButtonProc = NULL;
WNDPROC lpfnGoalStateButtonProc = NULL;
WNDPROC lpfnGoalEditProc = NULL;
WNDPROC lpfnGoalComboBoxProc = NULL;

// Modeless dialog box indicators
bool talkdlg = false; // true when talk dialog box is up
bool pmare_talkdlg = false; // true when pmare talk dialog box is up
bool helpdlg = false; // true when help dialog box is up
bool creditsdlg = false; // true when credits dialog box is up
bool metadlg = false; // true when meta dialog box is up
bool itemdlg = false; // true when item dialog box is up
bool gm_itemhelpdlg = false; // true when gm item creator help dialog is up
bool player_itemhelpdlg = false; // true when player item creator help dialog is up
bool quest_itemhelpdlg = false; // true when player item creator help dialog is up
bool quest_helpdlg = false; // true when displaying help for quests
bool keyboarddlg = false; // true when keyboard config dialog box is up
bool chooseguilddlg = false; // true when choose guild dialog is up
bool entervaluedlg = false; // true when enter value dialog is up
bool writescrolldlg = false; // true when enter value dialog is up
bool ignorelistdlg = false; // true when enter value dialog is up
bool locateavatardlg = false; // true when locate avatar dialog is up
bool exiting = false; // true when quiting the game
bool optiondlg = false; // true when options dialog box is up
bool loginoptiondlg = false; // true when login options dialog box is up
bool agentdlg = false; // true when agent controller dialog is up
bool createplayerdlg = false; // true when create player dialog is up
bool avatardlg = false; // true when avatar customization dialog is up
bool avatarmfdlg = false; // true when m/f avatar selection dialog is up
bool acceptrejectdlg = false; // true when accept/reject dialog is up
HWND hwnd_acceptreject = NULL; // dialog window handle used for automatic rejects
bool gmteleportdlg = false; // true when gm teleport dialog is up
bool warningyesnodlg = false; // true when warning yes/no dialog is up
bool useppointdlg = false; // true when meta dialog box is up
bool grantppointdlg = false; // true when meta dialog box is up
bool ppoint_helpdlg = false; // true when meta dialog box is up

bool ready = false;   // true when initilization finished
bool framerate = false; // display frame rate
bool leveleditor = false; // true if launched by level editor
bool lost_server = false; // true when server connection is lost
bool agents_in = false; // used for agent login/logout timer
bool exit_switch_task = false; // did they switch task to exit?
int nonfataldlg = 0; // count of simple, stateless message dialogs that are up
long g_lExeFileCheckSum = 0;

// roger wilco real-time voice
//void* voicePipeline = NULL;
//void* channel = RWNET_INVALIDCHANNEL;

// for windows version info
//extern unsigned int _osver; // only _winmajor is used here
//extern unsigned int _winmajor;
//extern unsigned int _winminor;
//extern unsigned int _winver;


// stores player mare information - mare names, charges
pmare_t pmare_info[3] = {	
	{BOG_PRICE, IDS_BOGROM, IDS_LP}, 
	{AGO_PRICE, IDS_AGOKNIGHT, IDS_MEDIUM},
	{SHAM_PRICE, IDS_SHAMBLIX, IDS_MP}};
// cArts methods for the dialog box procedures to call

art_dlg_callback_t acceptreject_callback = NULL;
art_dlg_callback_t entervalue_callback = NULL;
art_dlg_callback_t grantpp_callback = NULL;
art_dlg_callback_t chooseguild_callback = NULL;

// Command line arguments & game options
LPTSTR argv; 
int argc;
options_t options;

// memory profiling
#ifdef _DEBUG
#include <CRTDBG.H>
_CrtMemState memstuff;
#endif

// define this to get agents to login and logout 
#define LOGIN_LOGOUT

// storage for original windows colors (never needed by AI)

#ifndef AGENT
#if !(defined (UL_DEBUG) || defined (GAMEMASTER))
unsigned long *origcolors = NULL;
#endif
#endif


// if GAME_LYR is defined, use game.lyr, unless we are overriding by setting GAME_CLI
#ifdef GAME_LYR
TCHAR gamefile[256] = _T("game.lyr");
#else // use game.cli
TCHAR gamefile[256] = _T("game.cli");
#endif

// debugging variables
cNeighbor *test_avatar;
cOrnament *test_object;

/////////////////////////////////////////////////
// Constants

const unsigned long lyra_colors[11] =
	{BLUE, LTBLUE, DKBLUE, LTBLUE, ORANGE, BLUE, 
	 ORANGE, BLACK, ORANGE, ORANGE, BLUE};

const int syscolors[11] = 
	{COLOR_BTNFACE, COLOR_BTNHIGHLIGHT, 
	 COLOR_BTNSHADOW, COLOR_3DLIGHT, COLOR_BTNTEXT, COLOR_HIGHLIGHT,
	 COLOR_HIGHLIGHTTEXT, COLOR_WINDOW, COLOR_WINDOWTEXT, COLOR_GRAYTEXT,
	 COLOR_INFOBK};

/////////////////////////////////////////////////
// Functions

#ifdef _UNICODE
#define _tpgmptr _wpgmptr
#else
#define _tpgmptr _pgmptr
#endif


static long EXECheckSum()
{
	long result = 0;
	char buffer[4096*4];
	size_t bytes;

	TCHAR pgmname[_MAX_PATH];
	LPTSTR lpFilePart;

	if (!GetFullPathName(_tpgmptr, _MAX_PATH, pgmname, &lpFilePart))
		return 0L;

	FILE *fh =_tfopen(pgmname, _T("rb"));

	while ((bytes = fread(buffer, 1, 4096*4, fh)))
		while (bytes)
			result += ((BYTE*)buffer)[--bytes];

	result += _tcslen(_T("Tampering is a violation of the first law of Underlight. Michael & Renee Ketcham"));
	fclose(fh);
	return ((result >> (result & 0x3)) ^ 0x10010);
}


#define MemoryCheck(label)

#if 0 // this is left in - it might be useful sometime
// Check whether game has been launched from gizmo or not
static bool ParentProcessIsGizmo()
{
#ifdef GAMEMASTER
	return true;
#else
	PROCESSENTRY32 pe;
	DWORD ParentProcessID = 0;
	DWORD GizmoProcessID  = -1;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	pe.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnap,&pe) )
		do 
		{
			if (pe.th32ProcessID == GetCurrentProcessId())
				ParentProcessID = pe.th32ParentProcessID;
			if (_tcsstr(_tcslwr(pe.szExeFile),_T("mic.exe")))
				GizmoProcessID = pe.th32ProcessID;

		} while (Process32Next(hSnap,&pe));

	return (GizmoProcessID == ParentProcessID);
#endif
}
#endif 0


const int MIN_FRAME_TIMER = WM_USER + 9234;
const int AGENT_LOGIN_LOGOUT_TIMER = WM_USER + 9235	;

// WARNING  The calling order of functions here is very important, as some depend on others for initialization
// BE VERY CAREFUL IN CHANGEING THE ORDER !!!

bool __cdecl Init_Game(void)
{
	int iRc, start_level_id = START_LEVEL_ID;
	WSADATA wsadata;
	WORD wVer;

	long testtime = timeGetTime();
	if (testtime < 0)
		time_offset = abs(testtime);

	last_keystroke = LyraTime();
	if (timeBeginPeriod(1) != TIMERR_NOERROR)
	{
		LoadString(hInstance, IDS_TIMER_PROBLEM, message, sizeof(message));
		LoadString(hInstance, IDS_TIMER_PROBLEM_CAPTION, disp_message, sizeof(disp_message));
		MessageBox(NULL, message, disp_message, MB_OK);
		return false;
	}

	int my_x = 0;
	int my_y = 0;
	exit_time = UINT_MAX;

//#include "../Launcher/Version.h"
//	sprintf(message, "%d", LAUNCHER_CURRENT_VERSION);
//	MessageBox(NULL, message, message, MB_OK);


	// compute and verify this files checksum with known values

	_tsetlocale(LC_ALL, _T("C"));

	g_lExeFileCheckSum = EXECheckSum();


#ifdef AGENT
	// load agent working directory from registry
	HKEY reg_key = NULL;
	unsigned long result, reg_type, size;
	TCHAR buffer[_MAX_PATH];

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, AGENT_REGISTRY_KEY, 0, KEY_ALL_ACCESS, &reg_key )
		!= ERROR_SUCCESS)
		return false;
	size = _MAX_PATH*sizeof(TCHAR);
	result = RegQueryValueEx(reg_key, _T("agent_working_directory"), NULL, &reg_type,
								(unsigned char *)buffer, &size);
	RegCloseKey(reg_key);

	if (result != ERROR_SUCCESS)
		return false;

	if (!SetCurrentDirectory(buffer))
		return false;

	LoadString(hInstance, IDS_DAEMON_LOG, temp_message, sizeof(temp_message));
	output = new cOutput(temp_message, false, true);		

	TlsSetValue(tlsOutput, output);

	// jack up priority for daemon thread
	if (!InitAgents())
		return false;
	
#else
	LoadString(hInstance, IDS_DEBUG, temp_message, sizeof(temp_message));
	output = new cOutput(temp_message, false, true);	// create new, force flush
#ifdef UL_DEBUG

	// look for game.cli if game.lyr isnt present
	//if (GetFileAttributes(gamefile) == -1 && GetLastError() == ERROR_FILE_NOT_FOUND)
	//	_tcscpy(gamefile,gamefile_alt);

#ifndef UNICODE // command line arguments need to be fixed for UNICODE builds
	leveleditor = (bool)_tcslen(argv);
	//MessageBox(NULL, argv, "Args", MB_OK);
	if (leveleditor)
		_stscanf(argv, _T("%s %d %d %d %d %d\n"),gamefile, &start_level_id);
#endif

#endif
#endif
	
	// Set up error handlers for new and malloc, set up timing struct
	_set_new_handler(OutOfMemory);
	_set_new_mode(1);
	timing = new timing_t();

	// randomize 
	srand(LyraTime());

	// initialize Sockets & WinINet
	wVer = MAKEWORD(1,1);
	iRc = WSAStartup(wVer,&wsadata);
	if (iRc) 
	{
		SOCKETS_ERROR(0); // winsock error
		return FALSE;
	}

	if (LOBYTE( wsadata.wVersion) !=1 ||
		 HIBYTE( wsadata.wVersion) !=1)
	{
		SOCKETS_ERROR(0); // version doesn't match
		return FALSE;
	}

#ifdef INIT_DEBUG
	MemoryCheck(_T("Initialized socket"));
#endif

#ifndef AGENT
	// init common controls (for rich edit)
	InitCommonControls();
	RichEdLibrary = LoadLibrary(_T("Riched32.DLL"));
	if (NULL == RichEdLibrary)
		LoadLibrary(_T("Riched20.DLL"));

	// set up game font
	LOGFONT logFont;

	memset(&logFont, 0, sizeof(LOGFONT));

	logFont.lfHeight = 15;
	logFont.lfWidth = 0;
	logFont.lfWeight = 500;
	logFont.lfEscapement = 0;
	logFont.lfItalic = 0;
	logFont.lfUnderline = 0;
	logFont.lfOutPrecision = OUT_TT_ONLY_PRECIS;
	logFont.lfClipPrecision = CLIP_STROKE_PRECIS;
	logFont.lfQuality = DEFAULT_QUALITY;
	_tcscpy(logFont.lfFaceName, FONT_NAME);

	// normal font
	display_font[0] = CreateFontIndirect(&logFont);

	// 800x600
	logFont.lfHeight = 18;
	display_font[1] = CreateFontIndirect(&logFont);

	// 1024x768
	logFont.lfHeight = 20;
	display_font[2] = CreateFontIndirect(&logFont);

	// bolder, thicker, bigger font
	logFont.lfHeight = 16;
	logFont.lfWeight = 800;
	bold_font[0] = CreateFontIndirect(&logFont);

	// 800x600 stat font
	logFont.lfHeight = 20;
	bold_font[1] = CreateFontIndirect(&logFont);

	// 1024x768 stat font
	logFont.lfHeight = 25;
	bold_font[2] = CreateFontIndirect(&logFont);

	HWND hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DUMMY), NULL, (DLGPROC)DummyDlgProc);

	// size of dummy is 200x200 with normal fonts; resize accordingly
	RECT rect; // magic dialog size is 129x108
	GetWindowRect(hDlg, &rect);
	scale_x = 200.0f/(float)rect.right;
	scale_y = 200.0f/(float)rect.bottom;

	DestroyWindow(hDlg);
#endif

	MemoryCheck(_T("Initialized common controls"));

#ifdef ANNOUNCE_LEVEL_FILE
	MessageBox(NULL, gamefile, _T("game file name"), MB_OK);
#endif

	// Create level object
	level = new cLevel(gamefile);
#ifdef AGENT
	TlsSetValue(tlsLevel, level);
#endif
	// Create keymap, must be present before game options accesses registry
	keymap = new cKeymap();
	InitKeyboard();

	// Load game options
	if (!LoadGameOptions())
		return false;

   if (leveleditor)
   {
		options.network = NO_NETWORK;
		options.entrypoint = 0;
   }
   else
   {
#ifndef AGENT
		if (!DialogBox(hInstance, MAKEINTRESOURCE(IDD_LOGIN), NULL, (DLGPROC)LoginDlgProc))
			return FALSE;
		MemoryCheck(_T("Dialog box input received"));
#ifdef PMARE
		LoadString (hInstance, IDS_AGREEMENT_TEXT5, message, sizeof(message));
		if (!DialogBox(hInstance, MAKEINTRESOURCE(IDD_PMARE_CONFIRM), 
					NULL, (DLGPROC)PMareDlgProc))
			return FALSE;

#endif
#endif
		options.entrypoint = options.welcome_ai;
   }

    // initialize main window
#ifdef AGENT
	cDD = new cDDraw(NAME, _T("Agent Daemon"), hInstance, AgentWindowProc, 
						  MAKEINTRESOURCE(IDI_PMARE), IDC_ARROW, 0);
	TlsSetValue(tlsDD, cDD);

#else
#ifdef PMARE
	cDD = new cDDraw(NAME, TITLE, hInstance, WindowProc, 
					MAKEINTRESOURCE(IDI_PMARE), IDC_ARROW, options.resolution, my_x, my_y);
#else
	cDD = new cDDraw(NAME, TITLE, hInstance, WindowProc, 
					MAKEINTRESOURCE(IDI_LYRA), IDC_ARROW, options.resolution, my_x, my_y);
#endif
#endif

	// Initialize Direct Draw
	cDD->InitDDraw();
	MemoryCheck(_T("Direct Draw initialized"));

	// Initialize Direct Sound
	cDS = new cDSound(options.sound,options.reverse);

	MemoryCheck(_T("Initialized Direct Sound") );

#ifndef AGENT
//	if (options.rw)
//		StartRogerWilco(cDD->Hwnd_Main());
#endif

	// load visual effects controller
	shader = new cPaletteManager();  // create before effects
	shader->SetDistances(1000,6000); // set minimum and maximum shading distances

	effects = new cEffects(); // cDS must be created first
	MemoryCheck(_T("Created effects controller"));

	// Removed Lyra Splash Screen for Beta.
	// cDS->PlaySound(LyraSound::INTRO);
	// cDD->ShowSplashScreen();

	// Create Actor List - must be done before player is created
	actors = new cActorList();

#ifdef AGENT
	TlsSetValue(tlsActors, actors);
	// create player and set to -1 if networked, 0 otherwise
	player = new cAI(0.0f, 0.0f, 0, 0);
	TlsSetValue(tlsPlayer, player);
#else
	// create player and set to -1 if networked, 0 otherwise
	player = new cPlayer(cDD->ViewY());
#endif

	player->InitPlayer();

	// Initialize 
	Init4DX(cDD->ViewX(), cDD->ViewY());  
	MemoryCheck(_T("4DX initialized"));

	// Clean out the keyboard[]
	memset(keyboard, 0, num_keystates);

	// profile level info
#ifdef UL_DEBUG
//	level->DumpMonsterGens();
//	level->FindUsedTextures(); 
//	level->CheckInterLevelPortals();
//	level->FindAllOpenSectors();
//	level->CheckInterLevelTeleports();
	//level->ProcessFiles();
#endif

	// Load the Level 
	level->Load(start_level_id);

	MemoryCheck(_T("Game loaded"));

	// Create UI Components

	display			= new cChat(options.speech_color, options.message_color, options.bg_color);
	cp					= new cControlPanel();
//	banner			= new cBanner();
	goals				= new cGoalPosting();
	readgoal			= new cReadGoal();
	reportgoal		= new cReportGoal();
	goalbook			= new cGoalBook();
	postgoal			= new cPostGoal();
	reviewgoals		= new cReviewGoals();
	readreport		= new cReadReport();
	detailgoal		= new cDetailGoal();
	quests			= new cQuestBuilder();
	readquest		= new cReadQuest();
	postquest		= new cPostQuest();
	detailquest		= new cDetailQuest();
	arts				= new cArts(); // must be created after cp
	timed_effects	= new cTimedEffects();

//	cp->AddAvatar();

#ifndef AGENT
	
	InitLyraDialogController(); // must be after effects creation
	MemoryCheck(_T("UI components created"));

	// Create options.networking Classes
	if (options.network)
	{
		gs = new cGameServer();
		gs->Login(LOGIN_NORMAL);
		// freed at level login ack 
		effects->LoadEffectBitmaps(LyraBitmap::INTRO);
	}

	InvalidateRect(display->Hwnd(), NULL, TRUE);
	InvalidateRect(cp->Hwnd_CP(), NULL, TRUE);   
	MemoryCheck(_T("Networking classes created"));
#endif

	cDD->Show();

	cDS->StopSound(LyraSound::INTRO);
	cDS->ReleaseBuffer(LyraSound::INTRO);
	cDS->PlaySound(LyraSound::ENTRY);
	timing->begin_time = timing->t_start = LyraTime();

#ifndef GAMEMASTER
#ifndef AGENT
#ifndef UL_DEBUG
	origcolors = new unsigned long[11];
	// screw with the windows colors
	{
		int i;
		for (i=0; i<11; i++)
			origcolors[i] = GetSysColor(syscolors[i]);
	}

	SetSysColors (11, syscolors, lyra_colors);
#endif
#endif
#endif

#ifdef GAMEMASTER // set up agent controller
	as = new cAgentServer();
	agentbox = new cAgentBox();
	MemoryCheck(_T("Gamemaster objects initialized"));
#endif


#ifdef UL_DEBUG
	if (options.network == NO_NETWORK)
	{// put in test_avatar neighbor
#include "RmRemotePlayer.h"
		LmPeerUpdate update;
		RmRemotePlayer info;
 	    LmAvatar avatar = player->Avatar();

		avatar.SetAvatarType(Avatars::FEMALE);
		update.Init(1, 0, (short)player->x, (short)player->y-50 , 0, 0 );
		update.SetAngle(player->angle+Angle_180);
		info.Init(update, avatar, _T("Test Avatar"), 0, 0);
		test_avatar = new cNeighbor(info);
		test_object = new cOrnament((float)((short)player->x+200), (float)(short)player->y-400, 0, 0, 0, 100);
	}
#endif

  
#ifdef AGENT // delete objects not needed by main agent thread
	delete actors; actors = NULL; player = NULL;    
	delete level; level = NULL; 
	delete timing; timing = NULL; 
//	cDD->Show();
	hwnd_daemon = cDD->Hwnd_Main();
	daemon = new cAgentDaemon();
	if (!daemon->Init())
		return false;
	for (int i=0; i<num_agents; i++)
	{	// we need a long delay so that the rooms don't get overcrowded
		StartAgent(agent_info[i].id, i*10000);
	}
#endif

	if (!SetTimer(cDD->Hwnd_Main(), MIN_FRAME_TIMER, 100, NULL))
		return false;
#ifdef AGENT
#ifdef _DEBUG
#ifdef LOGIN_LOGOUT
	if (!SetTimer(cDD->Hwnd_Main(), AGENT_LOGIN_LOGOUT_TIMER, 90000, NULL))
		return false;
#endif
#endif
#endif

#ifdef UL_DEBUG
//	LoadString (hInstance, IDS_WINVER, message, sizeof(message));
//	_stprintf(disp_message, message, _osver, _winmajor, _winminor, _winver);
//	display->DisplayMessage(disp_message);
#endif

	ready = true;

	CheckOptions(); // defined in utils
	MemoryCheck(_T("Underlight initialized!"));

	return TRUE;
}

void __cdecl EstimatePmareBilling(void)
{
	// show message calculating time & cost
	int minutes, billing_minutes, seconds;
	TimeOnline(&minutes, &seconds);
	if ((minutes > 1500) || (minutes < 0))// avoid weird numbers on failed logins
		minutes = 0; 
	else if (minutes == 0)
		minutes = 1;
	billing_minutes = minutes + options.pmare_session_minutes;
	// timer is per-minute, with a 15 minute minimum
	if (billing_minutes < 15) { billing_minutes = 15; seconds = 0; }
		
	int num_blocks = (int)(billing_minutes/15);
	int block_rate = pmare_info[options.pmare_start_type-3].charge;
	int charge = num_blocks * block_rate;
	float minute_rate = block_rate/15;
	int num_extra_minutes = billing_minutes%15;
	charge += num_extra_minutes*minute_rate;

	if (!gs || !gs->HasLoggedIn() || ((minutes == 0) && (seconds < 10)))
	{
		if (cDD != NULL) // If they haven't logged in, we might not have a window yet
		{
			LoadString (hInstance, IDS_NO_CHARGE_PMARE, message, sizeof(message));
			LyraDialogBox(hInstance, (IDD_FATAL_ERROR), 
					cDD->Hwnd_Main(), (DLGPROC)FatalErrorDlgProc);
		}
	}
	else
	{
		LoadString (hInstance, IDS_CHARGE_PMARE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, minutes, options.pmare_session_minutes, 
			minutes + options.pmare_session_minutes, NightmareName(options.pmare_start_type), 
			pmare_info[options.pmare_start_type-3].charge, charge);
		LyraDialogBox(hInstance, (IDD_FATAL_ERROR), 
					cDD->Hwnd_Main(), (DLGPROC)FatalErrorDlgProc);
	}
}

void __cdecl CancelExit(void)
{
	if (exit_time != UINT_MAX)
	{
		LoadString (hInstance, IDS_EXIT_CANCELLED, message, sizeof(message));
		if (display)
			display->DisplayMessage(message);
	}
	exit_time = UINT_MAX;	
}

// this function is called when we're about to start the exit process,
// which is delayed by 5 seconds
void __cdecl StartExit(void)
{
#ifndef AGENT
#ifndef GAMEMASTER
#ifndef UL_DEBUG
	// if we're logged into a level, give a 5 second delay
	if (gs && gs->LoggedIntoGame() && gs->LoggedIntoLevel())
	{
//		LoadString (hInstance, IDS_FADING, message, sizeof(message));
		//gs->Talk(message, RMsg_Speech::EMOTE, Lyra::ID_UNKNOWN, false, false);
		if (exit_time == UINT_MAX)
		{
			exit_time = LyraTime() + 5000;
			LoadString (hInstance, IDS_EXITING, message, sizeof(message));
			display->DisplayMessage(message);
			//CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
			//	cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
		}
	}
	else // not logged into a level, exit immediately
#endif
#endif
#endif
	{
		Exit();
		exit(-1);
	}

}


void __cdecl Exit(void)
{
	ShowCursor(TRUE);

	if (player && gs && player->Gamesite() == GMsg_LoginAck::GAMESITE_MULTIPLAYER)
		gs->MPGPLogTime(100);

	if (cDD != NULL) // We might not have a window yet
	{
		KillTimer( cDD->Hwnd_Main(), MIN_FRAME_TIMER );
#ifdef AGENT
#ifdef _DEBUG
#ifdef LOGIN_LOGOUT
		KillTimer( cDD->Hwnd_Main(), AGENT_LOGIN_LOGOUT_TIMER );
#endif
#endif
#endif
	}

	timeEndPeriod(1);

	if (keymap) 
		SaveInGameRegistryOptionValues();

#ifdef PMARE 
	EstimatePmareBilling();
#endif

#ifdef AGENT // kill all running agent threads
	ExitAgents(); // important - must be BEFORE exiting = true
#endif

	exiting = true;

#ifndef GAMEMASTER
#ifndef AGENT
#ifndef UL_DEBUG
	if (origcolors)
		SetSysColors (11, syscolors, origcolors);
#endif
#endif
#endif

	if (keymap) { delete keymap; keymap = NULL; }
		DeInitKeyboard();

	DeInit4DX();
#ifndef AGENT
	DeInitLyraDialogController();
#endif

	// anything with timers must be destroyed before cDD
	if (gs) { delete gs; gs = NULL; }

#ifdef GAMEMASTER
	if (as) { delete as; as = NULL; }
	if (agentbox) { delete agentbox; agentbox = NULL; }
#endif

#ifndef AGENT
//	if (options.rw)
//		ShutdownRogerWilco();
#endif
	//if (banner)		{ delete banner; banner = NULL; }
	if (actors)			{ delete actors; actors = NULL; player = NULL;}
	if (level)			{ delete level; level = NULL; }
	if (cDD)				{ cDD->DestroyDDraw(); delete cDD; cDD = NULL; }
	if (cDS)				{ delete cDS; cDS = NULL; }
	if (cp)				{ delete cp; cp = NULL; }
	if (goals)			{ delete goals; goals = NULL; }
	if (readgoal)		{ delete readgoal; readgoal = NULL; }
	if (reportgoal)	{ delete reportgoal; reportgoal = NULL; }
	if (goalbook)		{ delete goalbook; goalbook = NULL; }
	if (postgoal)		{ delete postgoal; postgoal = NULL; }
	if (reviewgoals)	{ delete reviewgoals; reviewgoals = NULL; }
	if (readreport)	{ delete readreport; readreport = NULL; }
	if (detailgoal)	{ delete detailgoal; detailgoal = NULL; };
	if (quests)		{ delete quests; quests = NULL; }
	if (readquest)	{ delete readquest; readquest = NULL; }
	if (postquest)	{ delete postquest; postquest = NULL; }
	if (detailquest)	{ delete detailquest; detailquest = NULL; }
	if (arts)			{ delete arts; arts = NULL; }
	if (shader)			{ delete shader; shader = NULL; };
	if (effects)		{ delete effects; effects = NULL; }
	if (timed_effects){ delete timed_effects; timed_effects = NULL; }
	if (display)		{ delete display; display = NULL; }
	if (timing)			{ delete timing; timing = NULL; }
	if (output)			{ delete output; output = NULL; }
	for (int i=0; i<MAX_RESOLUTIONS; i++) 
	{
		if (bold_font[i])		{ DeleteObject(bold_font[i]); bold_font[i] = NULL; }
		if (display_font[i])		{ DeleteObject(display_font[i]); display_font[i] = NULL; }
	}


	WSACleanup();  

	if (RichEdLibrary) { FreeLibrary(RichEdLibrary); RichEdLibrary = NULL; }

#ifndef GAMEMASTER
#ifndef AGENT
#ifndef UL_DEBUG
	if (origcolors)
	{
		SetSysColors(11, syscolors, origcolors);
		delete origcolors;
		origcolors = NULL;
	}
#endif
#endif
#endif

	if (killed)
	{ // disp_message comes loaded with the dreamstrike message with the killer's name
		LoadString (hInstance, IDS_DREAMSTRUCK, message, sizeof(message));
		MessageBox(NULL, disp_message, message, MB_OK);
	}
	
	PostQuitMessage( 0 ); 
}

