// Handles the player's goal book

// Copyright Lyra LLC, 1997. All rights reserved.

#define STRICT

#include <winsock2.h>
#include <limits.h>
#include "cDDraw.h"
#include "cChat.h"
#include "Resource.h"
#include "Move.h"
#include "Dialogs.h"
#include "cLevel.h"
#include "cPlayer.h"
#include "Interface.h"
#include "cAgentServer.h"
#include "Utils.h"
#include "cAgentBox.h"
#include "Options.h"
#include "cGameServer.h"

#ifdef GAMEMASTER

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cAgentBox *agentbox;
extern cChat *display;
extern cDDraw *cDD;
extern cAgentServer *as;
extern cLevel *level;
extern cPlayer *player;
extern cGameServer *gs;
extern options_t options;
extern bool agentdlg;
extern bool exiting;

//////////////////////////////////////////////////////////////////
// Constants

const int POSESSION_DELAY = 5000;
const int BUTTON_HEIGHT = 20;
const int BUTTON_WIDTH =  55;

// columns for the listview control
struct COLUMN_DESC
{
	UINT name;
	short fmt;
	short width;
} column_desc[] = 
{
	{ IDS_NAME,	LVCFMT_LEFT,	 70 },
	{ IDS_TYPE,	LVCFMT_LEFT,	 60 },
	{ IDS_LEVEL,	LVCFMT_LEFT,	120 },
	{ IDS_ROOM,	LVCFMT_LEFT,	120 },
	{ IDS_STATUS,	LVCFMT_CENTER,	 45 },
	{ IDS_ALONE,	LVCFMT_CENTER,	 20 },
	{ IDS_DEATHS,	LVCFMT_CENTER,	 25 },
	{ IDS_KILLS,	LVCFMT_CENTER,	 25 },
	{ IDS_BUSY,	LVCFMT_CENTER,	 25 },
	{ IDS_X,		LVCFMT_CENTER,	 45 },
	{ IDS_Y,		LVCFMT_CENTER,	 45 },
	{ IDS_LEVELID,LVCFMT_CENTER,	 25 },
	{ IDS_ROOMID,	LVCFMT_CENTER,	 25 },
	{ IDS_FRMRATE,LVCFMT_CENTER,	 25 }, 
};
const int NUM_COLUMNS = sizeof(column_desc)/sizeof column_desc[0];


//static TCHAR *COLUMN_NAMES[] =
//	{ "Name", "Type", "Lvl", "Room", "Status", "Alone", "Deaths", "Kills", "% Busy", "X", "Y", "LvlID", "RoomID", "FR" };
//const NUM_COLUMNS = sizeof(COLUMN_NAMES)/sizeof COLUMN_NAMES[0];

enum {
	NAME_COLUMN = 0,
	TYPE_COLUMN,
	LEVEL_COLUMN,
	ROOM_COLUMN,
	STATUS_COLUMN,
	ALONE_COLUMN,
	DEATHS_COLUMN,
	KILLS_COLUMN,
	BUSY_COLUMN,
	X_COLUMN, // these are hidden & just used to store state
	Y_COLUMN,
	LEVELID_COLUMN,
	ROOMID_COLUMN,
	FRAMERATE_COLUMN,
};


// position for agent controller window
const struct window_pos_t agentBoxPos[MAX_RESOLUTIONS]=
{
	{4,  305, 634, 165}, // 640x480
	{4,  385, 634, 165}, // 800x600
	{4,  485, 634, 165}, // 1024x768
};

// position for listview on agent controller window
const struct window_pos_t listviewPos=
	{0, 25, 627, 135};

// position for start button on agent controller window
const struct window_pos_t startPos=
	{5, 2, BUTTON_WIDTH, BUTTON_HEIGHT};

// position for start all button on agent controller window
const struct window_pos_t startAllPos=
	{70, 2, BUTTON_WIDTH, BUTTON_HEIGHT};

// position for stop button on agent controller window
const struct window_pos_t stopPos=
	{135, 2, BUTTON_WIDTH, BUTTON_HEIGHT};

// position for stop all button on agent controller window
const struct window_pos_t stopAllPos=
	{200, 2, BUTTON_WIDTH, BUTTON_HEIGHT};

// position for goto button on agent controller window
const struct window_pos_t gotoPos=
	{265, 2, BUTTON_WIDTH, BUTTON_HEIGHT};

// position for posess button on agent controller window
const struct window_pos_t posessPos=
	{330, 2, BUTTON_WIDTH, BUTTON_HEIGHT};

// position for hide button on agent controller window
const struct window_pos_t hidePos=
	{395, 2, BUTTON_WIDTH, BUTTON_HEIGHT};

// position for close button on agent controller window
const struct window_pos_t closePos=
	{460, 2, BUTTON_WIDTH, BUTTON_HEIGHT};

#ifdef UL_DEBUG
// position for kill button on agent controller window
const struct window_pos_t killPos=
	{525, 2, BUTTON_WIDTH, BUTTON_HEIGHT};
#endif

// Constructor
cAgentBox::cAgentBox(void)
{
	int i;
	LV_COLUMN lvc;

	hwnd_agent_box = hwnd_listview = hwnd_start = hwnd_start_all =
	hwnd_stop = hwnd_stop_all = hwnd_goto = hwnd_posess = hwnd_hide =
	hwnd_close = 0;
#ifdef UL_DEBUG
	hwnd_kill = 0;
#endif

	num_agents =0;
	old_x = old_y = 0L;
	old_level_id = 0;
	posess_x = posess_y = posess_level_id = posess_time = 0;
	posession_pending = posession_in_progress = exorcism_in_progress = ready_to_posess = false;
	possessed_id = 0;

	memset (&game_server_addr, 0, sizeof(game_server_addr));
	memset (old_username, 0, sizeof(old_username));
	memset (old_password, 0, sizeof(old_password));

#ifdef AGENT
	return;
#endif

	num_agents = 0;
	WNDCLASS wc;

	// set up and register window class
	wc.style 			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc 	= AgentBoxWProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon 			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= (HBRUSH)(GetStockObject(GRAY_BRUSH));
	wc.lpszMenuName	= NULL;
	wc.lpszClassName	= _T("AgentBox");

	RegisterClass( &wc );

	// create main agent box window
	hwnd_agent_box =	CreateWindowEx(0, _T("AgentBox"), _T(""),
		WS_POPUP | WS_CHILD | WS_DLGFRAME,
		agentBoxPos[cDD->Res()].x, agentBoxPos[cDD->Res()].y,
		agentBoxPos[cDD->Res()].width, agentBoxPos[cDD->Res()].height,
		display->Hwnd(), NULL, hInstance, NULL );

	 // Create the list view window.
	hwnd_listview = CreateWindowEx(0, WC_LISTVIEW, _T(""),
		WS_VISIBLE | WS_VSCROLL | WS_CHILD |
		LVS_REPORT | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS,
		listviewPos.x, listviewPos.y,
		listviewPos.width, listviewPos.height,
		hwnd_agent_box, NULL, hInstance, NULL);

	// add the columns to the list view

	// Initialize the LV_COLUMN structure.
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	// Add the columns.
	for (i = 0; i < NUM_COLUMNS; i++)
	{
		lvc.iSubItem = i;
		LoadString(hInstance, IDS_NAME + i, message, sizeof(message));
		lvc.pszText = message;
		lvc.cx = column_desc[i].width;
		if (i == 0)
			lvc.fmt = LVCFMT_LEFT;	// leftmost must be left
		else
			lvc.fmt = column_desc[i].fmt;

		ListView_InsertColumn(hwnd_listview, i, &lvc);
	}

	// Create the buttons that go along the top
	hwnd_start = CreateWindow(_T("button"), _T("Start"), WS_CHILD | BS_PUSHBUTTON,
						startPos.x, startPos.y,
						startPos.width, startPos.height,
						hwnd_agent_box, NULL, hInstance, NULL);

	hwnd_start_all = CreateWindow(_T("button"), _T("Start All"), WS_CHILD | BS_PUSHBUTTON,
						startAllPos.x, startAllPos.y,
						startAllPos.width, startAllPos.height,
						hwnd_agent_box, NULL, hInstance, NULL);

	hwnd_stop = CreateWindow(_T("button"), _T("Stop"),	WS_CHILD | BS_PUSHBUTTON,
						stopPos.x, stopPos.y,
						stopPos.width, stopPos.height,
						hwnd_agent_box, NULL, hInstance, NULL);

	hwnd_stop_all = CreateWindow(_T("button"), _T("Stop All"), WS_CHILD | BS_PUSHBUTTON,
						stopAllPos.x, stopAllPos.y,
						stopAllPos.width, stopAllPos.height,
						hwnd_agent_box, NULL, hInstance, NULL);

	hwnd_goto = CreateWindow(_T("button"), _T("Go To"),	WS_CHILD | BS_PUSHBUTTON,
						gotoPos.x, gotoPos.y,
						gotoPos.width, gotoPos.height,
						hwnd_agent_box, NULL, hInstance, NULL);

	hwnd_posess = CreateWindow(_T("button"), _T("Posess"), WS_CHILD | BS_PUSHBUTTON,
						posessPos.x, posessPos.y,
						posessPos.width, posessPos.height,
						hwnd_agent_box, NULL, hInstance, NULL);

	hwnd_hide = CreateWindow(_T("button"), _T("Hide"),	WS_CHILD | BS_PUSHBUTTON,
						hidePos.x, hidePos.y,
						hidePos.width, hidePos.height,
						hwnd_agent_box, NULL, hInstance, NULL);

	hwnd_close = CreateWindow(_T("button"), _T("Close"),	WS_CHILD | BS_PUSHBUTTON,
						closePos.x, closePos.y,
						closePos.width, closePos.height,
						hwnd_agent_box, NULL, hInstance, NULL);

#ifdef UL_DEBUG
	hwnd_kill = CreateWindow(_T("button"), _T("Kill"),	WS_CHILD | BS_PUSHBUTTON,
						killPos.x, killPos.y,
						killPos.width, killPos.height,
						hwnd_agent_box, NULL, hInstance, NULL);
#endif

	this->SetButtonState();

	this->CheckInvariants(__LINE__, _T(__FILE__));

	return;
}

void cAgentBox::Show(void)
{ // log in to daemon, if not already
	if (!as->LoggedIn())
	{
		if (!agentdlg)
			CreateDialog(hInstance, MAKEINTRESOURCE(IDD_AGENT_LOGIN),
				cDD->Hwnd_Main(), (DLGPROC)AgentLoginDlgProc);
	}
	else
	{
		ShowWindow(hwnd_agent_box, SW_SHOWNORMAL);
		ShowWindow(hwnd_listview, SW_SHOWNORMAL);
		ShowWindow(hwnd_start, SW_SHOWNORMAL);
		ShowWindow(hwnd_start_all, SW_SHOWNORMAL);
		ShowWindow(hwnd_stop, SW_SHOWNORMAL);
		ShowWindow(hwnd_stop_all, SW_SHOWNORMAL);
		ShowWindow(hwnd_goto, SW_SHOWNORMAL);
		ShowWindow(hwnd_posess, SW_SHOWNORMAL);
		ShowWindow(hwnd_hide, SW_SHOWNORMAL);
		ShowWindow(hwnd_close, SW_SHOWNORMAL);
#ifdef UL_DEBUG
		ShowWindow(hwnd_kill, SW_SHOWNORMAL);
#endif
		if (as && as->LoggedIn()) // re-enable updates
			as->SendControlMessage(0, AMsg_ControlAgent::COMMAND_ENABLE);
	}

	return;
}

void cAgentBox::Hide(void)
{
	ShowWindow(hwnd_agent_box, SW_HIDE);
	if (as && as->LoggedIn()) // disable updates
		as->SendControlMessage(0, AMsg_ControlAgent::COMMAND_DISABLE);

	SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());

	return;
}

// returns the listview index of the given agent, or NO_AGENT if
// it can not be found
int cAgentBox::LookUpAgentIndex(lyra_id_t id)
{
	LV_FINDINFO stats;

	stats.flags = LVFI_PARAM; stats.lParam = (LPARAM)id;

	return ListView_FindItem(hwnd_listview, NO_AGENT, &stats);

}

// returns the agent id associated with a given listview index
lyra_id_t cAgentBox::LookUpAgent(int index)
{
	LV_ITEM stats;
	stats.mask = LVIF_PARAM; stats.iItem = index; stats.iSubItem = 0;
	ListView_GetItem(hwnd_listview, &stats);
	return (lyra_id_t)stats.lParam;
}

void cAgentBox::SortAgents(void)
{	// sort by level, with current level on top
	SendMessage(hwnd_listview, LVM_SORTITEMS, (WPARAM) 0, (LPARAM) ::CompareAgents);
}


void cAgentBox::AddAgent(lyra_id_t id, const TCHAR *name, lyra_id_t level_id,
						 int type)
{
	int index;
	TCHAR temp[DEFAULT_MESSAGE_SIZE];

	if (LookUpAgentIndex(id) != NO_AGENT)
		return; // agent already present

	LV_ITEM stats;

	 stats.mask = LVIF_PARAM;
	 stats.iItem = num_agents;
	 stats.iSubItem = 0;
	 stats.lParam = id; // use id as 32 bit user data

	index = ListView_InsertItem(hwnd_listview, &stats);

	if (index != num_agents)
		GAME_ERROR(IDS_INCORRECT_INDEX);

	// first four columns (name,type,level,room) are constant, so set now

	_tcscpy(temp, name); // needed b/c not const
	ListView_SetItemText(hwnd_listview, index, NAME_COLUMN, temp);

	switch (type)
	{
		case Avatars::EMPHANT:
			LoadString(hInstance, IDS_EMPHANT, message, sizeof(message));
			ListView_SetItemText(hwnd_listview, index, TYPE_COLUMN, message);
			break;
		case Avatars::BOGROM:
			LoadString(hInstance, IDS_BOGROM, message, sizeof(message));
			ListView_SetItemText(hwnd_listview, index, TYPE_COLUMN, message);
			break;
		case Avatars::AGOKNIGHT:
			LoadString(hInstance, IDS_AGOKNIGHT, message, sizeof(message));
			ListView_SetItemText(hwnd_listview, index, TYPE_COLUMN, message);
			break;
		case Avatars::SHAMBLIX:
			LoadString(hInstance, IDS_SHAMBLIX, message, sizeof(message));
			ListView_SetItemText(hwnd_listview, index, TYPE_COLUMN, message);
			break;
		case Avatars::HORRON:
			LoadString(hInstance, IDS_HORRON, message, sizeof(message));
			ListView_SetItemText(hwnd_listview, index, TYPE_COLUMN, message);
			break;
		default:
			LoadString(hInstance, IDS_UNKNOWN, message, sizeof(message));
			ListView_SetItemText(hwnd_listview, index, TYPE_COLUMN, message);
	}

	_stprintf(message, _T("%s"), level->Name(level_id));
	ListView_SetItemText(hwnd_listview, index, LEVEL_COLUMN, message);

	for (int i=STATUS_COLUMN; i<=Y_COLUMN; i++)
		ListView_SetItemText(hwnd_listview, index, i, _T(""));

	_stprintf(message, _T("%d"), level_id);
	ListView_SetItemText(hwnd_listview, index, LEVELID_COLUMN, message);

	this->SortAgents();

	num_agents++;
	return;
}

void cAgentBox::RemoveAgent(lyra_id_t id)
{
	int index = LookUpAgentIndex(id);

	if (index == NO_AGENT)
		return;

	ListView_DeleteItem(hwnd_listview, index);
	ListView_Arrange(hwnd_listview, LVA_ALIGNTOP);
	num_agents--;

	return;
}

void cAgentBox::RemoveAllAgents(void)
{
	for (int i=0; i<num_agents; i++)
		ListView_DeleteItem(hwnd_listview, 0);

	num_agents = 0;
	return;
}


void cAgentBox::UpdateAgent(AMsg_AgentInfo& info)
{
	int index;
	lyra_id_t level_id;
	// LV_ITEM stats;
	TCHAR buffer[64];

	index = LookUpAgentIndex(info.AgentID());

	if (index == NO_AGENT)
	{
		LoadString(hInstance, IDS_GOT_UPDATE_FOR_UNK_AGENT, message, sizeof(message));
		_stprintf(errbuf, message,info.AgentID());
		NONFATAL_ERROR(errbuf);
	}

	ListView_GetItemText(hwnd_listview, index, NAME_COLUMN, buffer, sizeof(buffer));

	this->SetSubItem(index, STATUS_COLUMN, info.Status());
	this->SetSubItem(index, FRAMERATE_COLUMN, info.FrameRate());
	this->SetSubItem(index, DEATHS_COLUMN, info.Deaths());
	this->SetSubItem(index, KILLS_COLUMN, info.Kills());
	if (info.Busy() > 0)
	{
		this->SetSubItem(index, BUSY_COLUMN, info.Busy());
		LoadString(hInstance, IDS_NO, message, sizeof(message));
		ListView_SetItemText(hwnd_listview, index, ALONE_COLUMN, message);
	}
	else
	{
		this->SetSubItem(index, BUSY_COLUMN, -info.Busy());
		LoadString(hInstance, IDS_YES, message, sizeof(message));
		ListView_SetItemText(hwnd_listview, index, ALONE_COLUMN, message);
	}
	// x/y values are hidden from view
	this->SetSubItem(index, X_COLUMN, info.X());
	this->SetSubItem(index, Y_COLUMN, info.Y());

	// set room name
	ListView_GetItemText(hwnd_listview, index, LEVELID_COLUMN, buffer, sizeof(buffer));
	level_id = _ttoi(buffer);

	if ((info.Status() != AMsg_AgentInfo::STATUS_RUNNING) || (info.RoomID() == 0))
	{
		LoadString(hInstance, IDS_NONE, message, sizeof(message));
		ListView_SetItemText(hwnd_listview, index, ROOM_COLUMN, message);
	}
	else if (level_id == (lyra_id_t)level->ID())
	{
		ListView_SetItemText(hwnd_listview, index, ROOM_COLUMN, level->RoomName(info.RoomID()));
	}
	else
	{
		LoadString(hInstance, IDS_UNKNOWN, message, sizeof(message));
		ListView_SetItemText(hwnd_listview, index, ROOM_COLUMN, message);
	}

	_stprintf(message, _T("%d"), info.RoomID());
	ListView_SetItemText(hwnd_listview, index, ROOMID_COLUMN, message);

	this->SetButtonState();

	return;
}

// set button state based on state of currently active agent
void cAgentBox::SetButtonState(void)
{
	TCHAR buffer[32];
	TCHAR name[32];
	int	room_id;

	int selected = this->SelectedIndex();

	if (selected == NO_AGENT)
	{
		EnableWindow(hwnd_start, FALSE);
		EnableWindow(hwnd_stop, FALSE);
		EnableWindow(hwnd_goto, FALSE);
		EnableWindow(hwnd_posess, FALSE);
	}
	else
	{
		ListView_GetItemText(hwnd_listview, selected, ROOMID_COLUMN, buffer, sizeof(buffer));
		room_id = _ttoi(buffer);
		ListView_GetItemText(hwnd_listview, selected, STATUS_COLUMN, buffer, sizeof(buffer));
		ListView_GetItemText(hwnd_listview, selected, NAME_COLUMN, name, sizeof(name));
		if (0 == _tcscmp(name, player->Name()))
		{	// posessed agent is selected
			EnableWindow(hwnd_start, FALSE);
			EnableWindow(hwnd_stop, FALSE);
			EnableWindow(hwnd_goto, FALSE);
			EnableWindow(hwnd_posess, TRUE);
		}
		else if (0 == _tcscmp(buffer, _T("Running")) && (room_id > 0))
		{ // agent is running
			EnableWindow(hwnd_start, FALSE);
			EnableWindow(hwnd_stop, TRUE);
			EnableWindow(hwnd_goto, TRUE);
			if (posession_pending || ready_to_posess || posession_in_progress)
				EnableWindow(hwnd_posess, FALSE);
			else
				EnableWindow(hwnd_posess, TRUE);
		}
		else // agent is not running or not logged in yet
		{
			EnableWindow(hwnd_start, TRUE);
			EnableWindow(hwnd_stop, FALSE);
			EnableWindow(hwnd_goto, FALSE);
			EnableWindow(hwnd_posess, FALSE);
		}
	}
	return;
}


// returns currently selected index, or NO_AGENT if none selected
int cAgentBox::SelectedIndex(void)
{
	for (int i=0; i<num_agents; i++)
		if (ListView_GetItemState(hwnd_listview, i, LVIS_SELECTED))
			return i;

	return NO_AGENT;

}


void cAgentBox::SetSubItem(int index, int subitem, int data)
{
	TCHAR text[80];

	_stprintf(text, _T("%d"), data);
	if (subitem == STATUS_COLUMN)
		switch (data)
		{
			case AMsg_AgentInfo::STATUS_READY:
			LoadString(hInstance, IDS_WAITING, message, sizeof(message));
			_stprintf(text, _T("%s"), message);
				break;
			case AMsg_AgentInfo::STATUS_RUNNING:
			LoadString(hInstance, IDS_RUNNING, message, sizeof(message));
			_stprintf(text, _T("%s"), message);
				break;
			case AMsg_AgentInfo::STATUS_ABORTED:
			LoadString(hInstance, IDS_ABORTED, message, sizeof(message));
			_stprintf(text, _T("%s"), message);
				break;
			case AMsg_AgentInfo::STATUS_CRASHED:
			LoadString(hInstance, IDS_CRASHED, message, sizeof(message));
			_stprintf(text, _T("%s"), message);
				break;
			case AMsg_AgentInfo::STATUS_POSESSED:
			LoadString(hInstance, IDS_POSESSED, message, sizeof(message));
			_stprintf(text, _T("%s"), message);
				break;
			case AMsg_AgentInfo::STATUS_NONE:
			default:
			LoadString(hInstance, IDS_UNKNOWN, message, sizeof(message));
			_stprintf(text, _T("%s"), message);
		}

	ListView_SetItemText(hwnd_listview,  index, subitem, text);

	return;
}

// called every frame to facilitate posession
void cAgentBox::Update(void)
{
	if (ready_to_posess && (posess_time <= (int)LyraTime()))
	{
		//player->SetTimedEffect(LyraEffect::PLAYER_TRANSFORMED, INT_MAX, player->ID());
//		ASSERT_XY(posess_x < _I16_MAX && posess_x > _I16_MIN);
	   //ASSERT_XY(posess_y < _I16_MAX && posess_y > _I16_MIN);
		player->Teleport( (float)posess_x, (float)posess_y, 0, posess_level_id);
		gs->Login(LOGIN_AGENT);
		gs->SetAutoLevelLogin(true);
		posession_in_progress = true;
		ready_to_posess = false;
		posess_time = 0;
	}
}

// got back posession ack from agent
void cAgentBox::Posess(AMsg_PosessInfo& info)
{
	if (!posession_pending)
		return; // do nothing, probably got canceled

	if (info.Status() != AMsg_PosessInfo::OK)
	{
		LoadString (hInstance, IDS_POSESS_ERROR, message, sizeof(message));
		CreateLyraDialog(hInstance, IDD_NONFATAL_ERROR,
						cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);
		return;
	}

	// go to the agent's last marked location
	// log into game again as the agent
	_tcscpy(options.username[options.account_index], info.Name());
	_tcscpy(options.password[options.account_index], info.Password());
	posession_pending = false;
	ready_to_posess = true;
	posess_time = LyraTime() + POSESSION_DELAY;

	return;
}



// Destructor
cAgentBox::~cAgentBox(void)
{
}


// Window procedure for the agent box
LRESULT WINAPI AgentBoxWProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, level_id;
	short x,y;
	TCHAR buffer[32];
	LPNMHDR notify;
	NM_LISTVIEW *stats;

	if (HBRUSH brush = SetControlColors(hwnd, message, wParam, lParam))
		return (LRESULT)brush;

	switch(message)
	{
		case WM_NOTIFY:
			notify = (LPNMHDR) lParam;
			switch (notify->code)
			{
				case LVN_ITEMCHANGED:
					stats = (NM_LISTVIEW*) lParam;
					if ((stats->uChanged & LVIF_STATE) && !exiting)
					{	// new agent has been selected
						if ((stats->uNewState & LVIS_SELECTED) && !(stats->uOldState & LVIS_SELECTED))
							agentbox->SetButtonState();
					}
					return (LRESULT)0;
			};
			break;

		case WM_COMMAND:
			if ((HWND)lParam == agentbox->hwnd_hide)
				agentbox->Hide();
			else if ((HWND)lParam == agentbox->hwnd_close)
				as->Logout();
#ifdef UL_DEBUG
			else if ((HWND)lParam == agentbox->hwnd_kill)
			{
				as->SendControlMessage(0, AMsg_ControlAgent::COMMAND_KILL_DAEMON);
				as->Logout();
			}
#endif
			else if ((HWND)lParam == agentbox->hwnd_start)
			{
				i = agentbox->SelectedIndex();
				if (i != NO_AGENT)
					as->SendControlMessage(agentbox->LookUpAgent(i), AMsg_ControlAgent::COMMAND_START);
			}
			else if ((HWND)lParam == agentbox->hwnd_start_all)
			{ // start all requires its own message
				as->SendControlMessage(0, AMsg_ControlAgent::COMMAND_START_ALL);
			}
			else if ((HWND)lParam == agentbox->hwnd_stop)
			{
				i = agentbox->SelectedIndex();
				if (i != NO_AGENT)
					as->SendControlMessage(agentbox->LookUpAgent(i), AMsg_ControlAgent::COMMAND_STOP);
			}
			else if ((HWND)lParam == agentbox->hwnd_stop_all)
			{
				for (i=0; i<agentbox->num_agents; i++)
					as->SendControlMessage(agentbox->LookUpAgent(i), AMsg_ControlAgent::COMMAND_STOP);
			}
			else if ((HWND)lParam == agentbox->hwnd_goto)
			{
				i = agentbox->SelectedIndex();
				if (i != NO_AGENT)
				{
					ListView_GetItemText(agentbox->hwnd_listview, i, STATUS_COLUMN, buffer, sizeof(buffer));
					if (0 == _tcscmp(buffer, _T("Running")))
					{
						ListView_GetItemText(agentbox->hwnd_listview, i, LEVELID_COLUMN, buffer, sizeof(buffer));
						level_id = _ttoi(buffer);
						ListView_GetItemText(agentbox->hwnd_listview, i, X_COLUMN, buffer, sizeof(buffer));
						x = _ttoi(buffer);
						ListView_GetItemText(agentbox->hwnd_listview, i, Y_COLUMN, buffer, sizeof(buffer));
						y = _ttoi(buffer);
						player->Teleport(x,y,0,level_id);
						break;
					}
				}
			}
			else if ((HWND)lParam == agentbox->hwnd_posess)
			{
				if (agentbox->posession_pending || agentbox->posession_in_progress || agentbox->ready_to_posess) // cancel posession
				{
					if (agentbox->posession_in_progress)
					{
						gs->LevelLogout(RMsg_Logout::LOGOUT);
						gs->Logout(GMsg_Logout::LOGOUT_POSSESS, false);
						agentbox->posession_in_progress = false;
					}
					agentbox->exorcism_in_progress = true;
					lyra_id_t mare_id = player->ID();
					as->SendControlMessage(player->ID(), AMsg_ControlAgent::COMMAND_STOP);
					agentbox->posession_pending = agentbox->ready_to_posess = false;
					_tcscpy(options.username[options.account_index], agentbox->old_username);
					_tcscpy(options.password[options.account_index], agentbox->old_password);
					player->Teleport(agentbox->old_x, agentbox->old_y, 0, agentbox->old_level_id);
					for (i=0; i<NUM_TIMED_EFFECTS; i++)
						player->RemoveTimedEffect(i);
					gs->SetServerAddress(agentbox->ServerAddress());
					Sleep(5000);
					gs->Login(LOGIN_NORMAL);
					gs->SetAutoLevelLogin(true);
					TCHAR *label = new TCHAR[32];
					_tcscpy(label, _T("Posess"));
					SendMessage(agentbox->hwnd_posess, WM_SETTEXT, 0, (LPARAM)label);
					delete label;
					as->SendControlMessage(mare_id, AMsg_ControlAgent::COMMAND_START);

				}
				else // begin posession sequence
				{
					if (!gs->LoggedIntoLevel())
					{
						LoadString (hInstance, IDS_CANT_POSESS, disp_message, sizeof(disp_message));
						display->DisplayMessage(disp_message);
						break;
					}
					agentbox->SetServerAddress(gs->ServerAddress());
					SOCKADDR_IN  agent_server_addr = gs->ServerAddress(); // IP address/port of agents game server

					gs->SetServerAddress(agent_server_addr);

					i = agentbox->SelectedIndex();
					if (i != NO_AGENT)
					{
						ListView_GetItemText(agentbox->hwnd_listview, i, STATUS_COLUMN, buffer, sizeof(buffer));
						if (0 == _tcscmp(buffer, _T("Running")))
						{
							ListView_GetItemText(agentbox->hwnd_listview, i, LEVELID_COLUMN, buffer, sizeof(buffer));
							agentbox->posess_level_id = _ttoi(buffer);
							ListView_GetItemText(agentbox->hwnd_listview, i, X_COLUMN, buffer, sizeof(buffer));
							agentbox->posess_x = _ttoi(buffer);
							ListView_GetItemText(agentbox->hwnd_listview, i, Y_COLUMN, buffer, sizeof(buffer));
							agentbox->posess_y = _ttoi(buffer);
							// save current name & password
							_tcscpy(agentbox->old_username, player->Name());
							_tcscpy(agentbox->old_password, player->Password());
							agentbox->old_x = player->x;
							agentbox->old_y = player->y;
							agentbox->old_level_id = level->ID();
							as->SendControlMessage(agentbox->LookUpAgent(i), AMsg_ControlAgent::COMMAND_POSESS);
							gs->LevelLogout(RMsg_Logout::LOGOUT);
							gs->Logout(GMsg_Logout::LOGOUT_POSSESS, false);
							agentbox->posession_pending = true;
							TCHAR *label = new TCHAR[32];
							_tcscpy(label, _T("Exorcise"));
							SendMessage(agentbox->hwnd_posess, WM_SETTEXT, 0, (LPARAM)label);
							delete label;
							break;
						}
					}
				}
			}
			break;

		case WM_KEYUP:
			if ((UINT)(wParam) == VK_ESCAPE)
			{
				agentbox->Hide();
				return 0;
			}
		case WM_KEYDOWN: // send the character + the focus back to the main window
		case WM_CHAR:
			SendMessage(cDD->Hwnd_Main(), WM_ACTIVATE,
				(WPARAM) WA_CLICKACTIVE, (LPARAM) cDD->Hwnd_Main());
			SendMessage(cDD->Hwnd_Main(), message,
				(WPARAM) wParam, (LPARAM) lParam);
		case WM_PAINT:
			if (hwnd == agentbox->hwnd_agent_box)
				TileBackground(hwnd);
			break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

// routine to sort agents; sort by level id, then by name
int CALLBACK CompareAgents(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	lyra_id_t level1, level2, index1, index2;
	TCHAR name1[64], name2[64], buffer[64];

	// agent id's are stored as the user data, so look up index & retrieve level id
	index1 = agentbox->LookUpAgentIndex(lParam1);
	ListView_GetItemText(agentbox->hwnd_listview, index1, LEVELID_COLUMN, buffer, sizeof(buffer));
	level1 = _ttoi(buffer);

	index2 = agentbox->LookUpAgentIndex(lParam2);
	ListView_GetItemText(agentbox->hwnd_listview, index2, LEVELID_COLUMN, buffer, sizeof(buffer));
	level2 = _ttoi(buffer);

	if ((level1 == (lyra_id_t)level->ID()) && (level2 != (lyra_id_t)level->ID()))
		return -1;
	else if ((level1 != (lyra_id_t)level->ID()) && (level2 == (lyra_id_t)level->ID()))
		return 1;
	else
	{	// both on same level - sort by name
		ListView_GetItemText(agentbox->hwnd_listview, index1, NAME_COLUMN, name1, sizeof(name1));
		ListView_GetItemText(agentbox->hwnd_listview, index2, NAME_COLUMN, name2, sizeof(name2));
		return _tcscmp(name1, name2);
	}
}



// Check invariants

#ifdef CHECK_INVARIANTS

void cAgentBox::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif

#endif