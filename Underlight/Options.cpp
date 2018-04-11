// Options: pre- and post-login configuration routines

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "Utils.h"
#include "Resource.h"
#include "cGameServer.h"
#include "cControlPanel.h"
#include "LyraDefs.h"
#include "cChat.h"
#include "cParty.h"
#include "cDSound.h"
#include "Mouse.h"
#include "Options.h"
#include "cAgentServer.h"
#include "SharedConstants.h"
#include "cActorList.h"
#include "cPlayer.h"
#include "cDDraw.h"
#include "Realm.h"
#include "Dialogs.h"
#include "cJSON.h"
#include "LmItem.h"
#include "LmItemDefs.h"
#include "LmItemHdr.h"
#include "cOutput.h"
#include "cKeymap.h"
#include "Mouse.h"
#include "Interface.h"
#include "cArts.h"
#include "cEffects.h"
#include "LoginOptions.h"
#include "base64.h"
/////////////////////////////////////////////////
// External Global Variables

extern cGameServer *gs;
extern cAgentServer *as; 
extern HINSTANCE hInstance;
extern cActorList *actors;
extern cChat *display;
extern cControlPanel *cp;
extern cPlayer *player;
extern cDDraw *cDD;
extern cDSound *cDS;
extern cArts *arts;
extern cEffects *effects;
extern options_t options;
extern cKeymap *keymap;
extern bool optiondlg;
extern bool loginoptiondlg;
extern bool ready;
extern bool keyboarddlg;
extern bool mouselooking;

//////////////////////////////////////////////////////////////////
// Constants

const short min_turnrate = 1;
const short max_turnrate = 20;

const int default_speech_color = 3;
const int default_message_color = 2;
const int default_bg_color = 0;
const float default_turnrate = 13.0f;
const int default_volume = 10;
int numJsonFiles = 0;
cJSON** jsonFiles;

//////////////////////////////////////////////////////////////////
// Functions


BOOL CALLBACK OptionsDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int i, turnrate;

	if (HBRUSH brush = SetControlColors(hDlg, Message, wParam, lParam))
		return (LRESULT)brush; 

	switch(Message)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_DESTROY:
			optiondlg = false;
			break;

		case WM_INITDIALOG:
			optiondlg = true;

			SetWindowPos(hDlg, TopMost(), cDD->DlgPosX(hDlg), cDD->DlgPosY(hDlg), 0, 0, SWP_NOSIZE);
			if (!options.sound) // hide if sound drivers not loaded
			{
				ShowWindow(GetDlgItem(hDlg, IDC_SOUND_ACTIVE), SW_HIDE);
				ShowWindow(GetDlgItem(hDlg, IDC_BGMUSIC_ACTIVE), SW_HIDE);
			}
		
// PMares cannot see nametags
#ifdef PMARE
			ShowWindow(GetDlgItem(hDlg, IDC_NAMETAGS), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_AUTOREJECT), SW_HIDE);
			ShowWindow(GetDlgItem(hDlg, IDC_AUTOREJOIN), SW_HIDE);
#endif
				
			Button_SetCheck(GetDlgItem(hDlg, IDC_REVERSESTEREO), options.reverse);
			Button_SetCheck(GetDlgItem(hDlg, IDC_AUTOREJECT),    options.autoreject);
			Button_SetCheck(GetDlgItem(hDlg, IDC_AUTOREJOIN),    options.autorejoin);
			Button_SetCheck(GetDlgItem(hDlg, IDC_NAMETAGS),      options.nametags);
			Button_SetCheck(GetDlgItem(hDlg, IDC_MULTILINE),     options.multiline);
			Button_SetCheck(GetDlgItem(hDlg, IDC_FOOTSTEPS),     options.footsteps);
			Button_SetCheck(GetDlgItem(hDlg, IDC_ART_PROMPTS),   options.art_prompts);
			Button_SetCheck(GetDlgItem(hDlg, IDC_MOUSELOOK),     options.mouselook);
			Button_SetCheck(GetDlgItem(hDlg, IDC_INVERTMOUSE),   options.invertmouse);
			Button_SetCheck(GetDlgItem(hDlg, IDC_CHATLOG),       options.log_chat);
			Button_SetCheck(GetDlgItem(hDlg, IDC_CHATLOG),       options.log_chat);
			Button_SetCheck(GetDlgItem(hDlg, IDC_SOUND_ACTIVE),  options.sound_active);
			Button_SetCheck(GetDlgItem(hDlg, IDC_BGMUSIC_ACTIVE),  options.music_active);
			Button_SetCheck(GetDlgItem(hDlg, IDC_PROFANITY_FILTER),  options.adult_filter);

			SendMessage(GetDlgItem(hDlg, IDC_TURNSPEED), TBM_SETRANGE, 
						(WPARAM) FALSE, (LPARAM) MAKELONG(min_turnrate, max_turnrate));

			turnrate = (int)options.turnrate;
			SendMessage(GetDlgItem(hDlg, IDC_TURNSPEED), TBM_SETPOS, 
						(WPARAM) TRUE, (LPARAM) turnrate);

			SendMessage(GetDlgItem(hDlg, IDC_EFFECTS_VOLUME), TBM_SETRANGE, 
						(WPARAM) FALSE, (LPARAM) MAKELONG(min_volume, max_volume));

			SendMessage(GetDlgItem(hDlg, IDC_EFFECTS_VOLUME), TBM_SETPOS, 
						(WPARAM) TRUE, (LPARAM) options.effects_volume);

			SendMessage(GetDlgItem(hDlg, IDC_MUSIC_VOLUME), TBM_SETRANGE, 
						(WPARAM) FALSE, (LPARAM) MAKELONG(min_volume, max_volume));

			SendMessage(GetDlgItem(hDlg, IDC_MUSIC_VOLUME), TBM_SETPOS, 
						(WPARAM) TRUE, (LPARAM) options.music_volume);



			for (i=0; i<NUM_CHAT_COLORS; i++)
			{
				ListBox_AddString(GetDlgItem(hDlg, IDC_SPEECHCOLOR), ChatColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_MESSAGECOLOR), ChatColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_BGCOLOR), ChatColorName(i));
			}
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_SPEECHCOLOR), options.speech_color);
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_MESSAGECOLOR), options.message_color);
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_BGCOLOR), options.bg_color);

			ResizeButton(GetDlgItem(hDlg, IDC_OK), effects->EffectWidth(IDC_OK), effects->EffectHeight(IDC_OK));
			ResizeButton(GetDlgItem(hDlg, IDC_CANCEL), effects->EffectWidth(IDC_CANCEL), effects->EffectHeight(IDC_CANCEL));
			ResizeButton(GetDlgItem(hDlg, IDC_KEYBOARD), effects->EffectWidth(IDC_KEYBOARD), effects->EffectHeight(IDC_KEYBOARD));

			return TRUE;

		case WM_PAINT:
			if (TileBackground(hDlg))
				return (LRESULT)0;
			break;


		case WM_KEYUP:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE:
				case VK_RETURN:
					PostMessage(hDlg, WM_COMMAND, (WPARAM) IDC_OK, 0);
					break;
				break;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{

			case IDC_KEYBOARD:
				if (!keyboarddlg)
				{
					keyboarddlg = true;
					CreateLyraDialog(hInstance, IDD_KEYBOARD_CONFIG,  
						cDD->Hwnd_Main(), (DLGPROC)KeyboardConfigDlgProc);
				}
	 			break;

			case IDC_OK:
			{
				if (Button_GetCheck(GetDlgItem(hDlg, IDC_REVERSESTEREO))) 
					options.reverse=true; 
				else 
					options.reverse=false;

				cDS->SetReverse(options.reverse);

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_AUTOREJECT))) 
					options.autoreject=true; 
				else 
					options.autoreject=false;

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_AUTOREJOIN))) 
					options.autorejoin=true; 
				else 
				{
					options.autorejoin=false;
					player->SetLastLeaderID(Lyra::ID_UNKNOWN);
				}

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_NAMETAGS))) 
					options.nametags=true; 
				else 
					options.nametags=false;

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_MULTILINE))) 
					options.multiline=true; 
				else 
					options.multiline=false;

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_FOOTSTEPS))) 
					options.footsteps=true; 
				else 
					options.footsteps=false;

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_ART_PROMPTS))) 
					options.art_prompts=true; 
				else 
					options.art_prompts=false;

				BOOL old_adult_filter = options.adult_filter;

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_PROFANITY_FILTER))) 
					options.adult_filter=TRUE; 
				else 
					options.adult_filter=FALSE;
				
				if (old_adult_filter != options.adult_filter)
				{
					if (options.adult_filter)
						LoadString (hInstance, IDS_ADULT_FILTER_TOGGLE_ON, message, sizeof(message));
					else
						LoadString (hInstance, IDS_ADULT_FILTER_TOGGLE_OFF, message, sizeof(message));
					display->DisplayMessage (message, false);
					
				}
				
				
				if (Button_GetCheck(GetDlgItem(hDlg, IDC_MOUSELOOK))) 
				{
					options.mouselook=true; 
					mouselooking = true;
				}
				else 
				{
					StopMouseLook();
					options.mouselook=false;
					mouselooking = false;
				}

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_INVERTMOUSE))) 
					options.invertmouse=true; 
				else 
					options.invertmouse=false;

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_CHATLOG))) 
					options.log_chat=true; 
				else 
					options.log_chat=false;

				if (Button_GetCheck(GetDlgItem(hDlg, IDC_SOUND_ACTIVE))) 
					options.sound_active = true; 
				else 
					options.sound_active = false;

				cDS->CheckSounds();

				options.speech_color = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_SPEECHCOLOR));
				options.message_color = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_MESSAGECOLOR));
				options.bg_color = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_BGCOLOR));

				display->SetSpeechFormat(options.speech_color);
				display->SetMessageFormat(options.message_color);
				display->SetBGColor(options.bg_color);

				// need intermediate var b/c turnrate is a float
				turnrate = SendMessage(GetDlgItem(hDlg, IDC_TURNSPEED), TBM_GETPOS, (WPARAM) 0, (LPARAM) 0);
				options.turnrate = (float)turnrate;

				options.effects_volume = SendMessage(GetDlgItem(hDlg, IDC_EFFECTS_VOLUME), TBM_GETPOS, (WPARAM) 0, (LPARAM) 0);

				SaveInGameRegistryOptionValues();
				DestroyWindow(hDlg);
				return TRUE;
			}
			case  IDC_CANCEL:
				DestroyWindow(hDlg);
				return FALSE;
			}
			break;
	}
	return FALSE;		
} 


/////////////////////////////////////////////////
// Functions

void __cdecl SaveCharacterRegistryOptionValues(HKEY reg_key)
{
	SaveInGameRegistryOptionValues();
}

#define ADDNUM(Field) { cJSON_AddNumberToObject(obj, #Field, options.##Field); }
#define GETNUM(Field) \
	do { \
		if(cJSON_HasObjectItem(obj, #Field)) \
		{ \
			cJSON* node = cJSON_GetObjectItem(obj, #Field); \
			if (node && cJSON_IsNumber(node)) options.##Field = node->valueint;  \
		} \
	} while (0)

cJSON* __cdecl WriteGlobalJSONOptionValues()
{
	cJSON* obj = cJSON_CreateObject();
	ADDNUM(account_index);
	ADDNUM(welcome_ai);
	ADDNUM(sound);
	ADDNUM(extra_scroll);
	ADDNUM(bind_local_tcp);
	ADDNUM(bind_local_udp);
	ADDNUM(resolution);
#ifdef UL_DEV
	ADDNUM(dev_server);
	cJSON_AddStringToObject(obj, "custom_server_ip", options.custom_ip);
	ADDNUM(debug);
	ADDNUM(network);
#endif
	cJSON_AddStringToObject(obj, "last_username", options.username[options.account_index]);
	ADDNUM(pmare_type);
	ADDNUM(pmare_start_type);
	ADDNUM(pmare_price);
	size_t output_length = 0;
	char* pmareSesh = (char*)base64_encode((unsigned char *)&(options.pmare_session_start), sizeof(options.pmare_session_start), &output_length);
	cJSON_AddStringToObject(obj, "pmare_session_start", pmareSesh);
	free(pmareSesh);
	return obj;
}

cJSON* __cdecl WriteJSONOptionValues()
{
	cJSON* obj = cJSON_CreateObject();
	ADDNUM(sound_active);
	ADDNUM(music_active);
	ADDNUM(reverse);
	ADDNUM(autoreject);
	ADDNUM(autorejoin);
	ADDNUM(nametags);
	ADDNUM(multiline);
	ADDNUM(footsteps);
	ADDNUM(art_prompts);
	ADDNUM(mouselook);
	ADDNUM(invertmouse);
	ADDNUM(log_chat);
	ADDNUM(effects_volume);
	ADDNUM(turnrate);
	ADDNUM(music_volume);
	ADDNUM(speech_color);
	ADDNUM(message_color);
	ADDNUM(bg_color);
	ADDNUM(autorun);
	ADDNUM(adult_filter);

	if (player)
	{
		cJSON_AddStringToObject(obj, "name", player->Name());
		cJSON_AddStringToObject(obj, "password", player->Password());
		size_t output_length = 0;
		char* av = (char*)base64_encode((unsigned char *)&(options.avatar), sizeof(options.avatar), &output_length);
		cJSON_AddStringToObject(obj, "avatar", av);
		free(av);
		ADDNUM(num_bungholes);
		cJSON* ignores = cJSON_CreateArray();
		for (int ig = 0; ig < options.num_bungholes; ig++)
			cJSON_AddItemToArray(ignores, cJSON_CreateString(options.bungholes[ig].name));
		cJSON_AddItemToObject(obj, "ignore_list", ignores);
		// save keyboard layout
		int num_keys;
		keymap_t *map;
		num_keys = keymap->num_keys();
		map = new keymap_t[num_keys];
		keymap->GetMap(map);
		cJSON_AddNumberToObject(obj, "number_keys_mapped", num_keys);
		char* keys = (char*)base64_encode((unsigned char*)map, (num_keys * sizeof(keymap_t)), &output_length);
		cJSON_AddStringToObject(obj, "key_mappings", keys);
		free(keys);
		delete map;
	}

	return obj;
}

cJSON** LoadJSONFiles()
{
	// first count em, then load em, parse em and return em.
	HANDLE hFind;
	WIN32_FIND_DATA data;
	int fcount = 0;
	hFind = FindFirstFile("*.json", &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			fcount++;
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	numJsonFiles = fcount;

	if (numJsonFiles > MAX_STORED_ACCOUNTS)
	{
		GAME_ERROR("Too many JSON files - delete some!");
		return NULL;
	}

	jsonFiles = new cJSON*[fcount];
	hFind = FindFirstFile("*.json", &data);
	int fidx = 1;
	int u = 0;
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			FILE* f = fopen(data.cFileName, "r");
			fseek(f, 0, SEEK_END);
			long fsize = ftell(f);
			fseek(f, 0, SEEK_SET);  //same as rewind(f);

			char *string = (char*)malloc(fsize + 1);
			fread(string, fsize, 1, f);
			fclose(f);

			cJSON* parsed = cJSON_Parse(string);
			cJSON* name = cJSON_GetObjectItem(parsed, "name");
			cJSON* pass = cJSON_GetObjectItem(parsed, "password");
			if (parsed && name && cJSON_IsString(name)) {
				jsonFiles[fidx++] = parsed;
				_tcscpy(options.username[u], cJSON_GetStringValue(name));
				if (cJSON_IsString(pass))
					_tcscpy(options.password[u], cJSON_GetStringValue(pass));
				u++;
			}
			else if (parsed) {
				jsonFiles[0] = parsed; // assume global
			}
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	
	return jsonFiles;
}

void __cdecl WriteJSONFile(cJSON* json, char* file)
{
	FILE* f = fopen(file, "w+t");
	char* jsonString = cJSON_Print(json);
	if (jsonString) {
		fwrite(jsonString, sizeof(char), strlen(jsonString), f);
		fclose(f);
		free(jsonString);
	}
}

void __cdecl SaveInGameRegistryOptionValues(void)
{
	if (!player)
	{
		cJSON* globals = WriteGlobalJSONOptionValues();
		cJSON* localGlobals = WriteJSONOptionValues();
		cJSON* cur = NULL;
		cJSON_ArrayForEach(cur, localGlobals)
		{
			char* k = cur->string;
			if (k) {
				cJSON_AddItemToObject(globals, k, cur);
			}
		}

		WriteJSONFile(globals, "globals.json");
		cJSON_Delete(globals);
	}
	else {
		cJSON* locals = WriteJSONOptionValues();
		char filename[Lyra::PLAYERNAME_MAX + 5] = { NULL };
		sprintf(filename, "%s.json", player->UpperName());
		WriteJSONFile(locals, filename);
		cJSON_Delete(locals);
	}
}


static bool ColorOutOfRange(int color)
{
	return color < 0 || color >= NUM_CHAT_COLORS;
}

void LoadJSONOptionValues(char* charName)
{
	int fidx = 0;
	cJSON *global = NULL, *local = NULL;
	if (numJsonFiles)
		global = jsonFiles[0];

	if (charName)
	{
		for (int i = 1; i < numJsonFiles; i++)
		{
			cJSON* nameNode = cJSON_GetObjectItem(jsonFiles[i], "name");
			if (!nameNode)
				continue;
			if (stricmp(nameNode->valuestring, charName) == 0) {
				local = jsonFiles[i];
				break;
			}
		}
	}
	LoadParsedJSONOptions(global);
	LoadParsedJSONOptions(local);
}

void __cdecl CleanupLoadedJSONFiles()
{
	for (int i = 0; i < numJsonFiles; i++)
	{
		cJSON* json = jsonFiles[i];
		cJSON_Delete(json);
	}
	numJsonFiles = 0;
	jsonFiles = NULL;
}

void LoadParsedJSONOptions(cJSON* json)
{
	if (!json)
		return;
	// OOG option vals
	cJSON* obj = json;
	// go go macro hackery
	GETNUM(account_index);
	GETNUM(rw);
	GETNUM(sound);
	GETNUM(extra_scroll);
	GETNUM(bind_local_tcp);
	GETNUM(bind_local_udp);
	GETNUM(restart_last_location);
	GETNUM(resolution);
	GETNUM(network);
	GETNUM(debug);
	GETNUM(welcome_ai);
	GETNUM(sound_active);
	GETNUM(music_active);
	GETNUM(autoreject);
	GETNUM(autorejoin);
	GETNUM(nametags);
	GETNUM(multiline);
	GETNUM(footsteps);
	GETNUM(art_prompts);
	GETNUM(mouselook);
	GETNUM(invertmouse);
	GETNUM(log_chat);
	cJSON* tr = cJSON_GetObjectItem(obj, "turnrate"); 
	if (tr && cJSON_IsNumber(tr)) options.turnrate = tr->valuedouble;

	GETNUM(effects_volume);
	GETNUM(music_volume);
	GETNUM(speech_color);
	GETNUM(message_color);
	GETNUM(bg_color);
	GETNUM(reverse);
	GETNUM(autorun);
	GETNUM(adult_filter);
	GETNUM(pmare_type);
	GETNUM(pmare_start_type);
	GETNUM(pmare_price);
#ifdef UL_DEV
	GETNUM(dev_server);
	cJSON* customIpNode = cJSON_GetObjectItem(obj, "custom_ip");
	if(customIpNode && cJSON_IsString(customIpNode))
		_tcscpy(options.custom_ip, customIpNode->valuestring);
	cJSON* gsNode = cJSON_GetObjectItem(obj, "game_server");
	if (gsNode && cJSON_IsString(gsNode))
		_tcscpy(options.game_server, gsNode->valuestring);
#endif

	cJSON* last = cJSON_GetObjectItem(obj, "last_username");
	if (last && cJSON_IsString(last))
	{
		char* lastVal = cJSON_GetStringValue(last);
		for (int i = 0; i < MAX_STORED_ACCOUNTS; i++) {
			if (stricmp(options.username[i], lastVal) == 0) {
				options.account_index = i;
				break;
			}
		}
	}
#ifndef UL_DEV
	LoadString(hInstance, IDS_LIVE_GAME_SERVER_IP, options.game_server, sizeof(options.game_server));
#endif
	cJSON* ig; cJSON* ignoreList = cJSON_GetObjectItem(obj, "bungholes");
	int igIdx = 0;
	cJSON_ArrayForEach(ig, ignoreList) {
		if (igIdx < MAX_IGNORELIST && ig && cJSON_IsString(ig))
		{
			_tcscpy(options.bungholes[igIdx++].name, cJSON_GetStringValue(ig));
		}
	}

	options.num_bungholes = igIdx;
	cJSON* pmareStart = cJSON_GetObjectItem(obj, "pmare_session_start");
	if (pmareStart && cJSON_IsString(pmareStart))
	{
		size_t out;
		unsigned char* sesh = base64_decode((const unsigned char*)cJSON_GetStringValue(pmareStart), strlen(cJSON_GetStringValue(pmareStart)), &out);
		if (sesh)
			memcpy(&options.pmare_session_start, sesh, out);
	}

	cJSON* av = cJSON_GetObjectItem(obj, "avatar");
	if (av && cJSON_IsString(av))
	{
		size_t out;
		unsigned char* avbytes = base64_decode((const unsigned char*)cJSON_GetStringValue(av), strlen(cJSON_GetStringValue(av)), &out);
		if (avbytes)
			memcpy(&options.avatar, avbytes, out);
	}
	cJSON* numKeys = cJSON_GetObjectItem(obj, "number_keys_mapped");
	keymap_t* map;
	if (numKeys && cJSON_IsNumber(numKeys))
	{
		map = new keymap_t[numKeys->valueint];
		cJSON* km = cJSON_GetObjectItem(obj, "key_mappings");
		if (km && cJSON_IsString(km))
		{
			size_t out;
			unsigned char* kmbytes = base64_decode((const unsigned char*)cJSON_GetStringValue(km), strlen(cJSON_GetStringValue(km)), &out);
			if (kmbytes)
				memcpy(map, kmbytes, out);
			keymap->Init(numKeys->valueint, map);
		}
	}
}

void LoadDefaultOptionValues()
{
	// OOG option vals
	options.account_index = 0;
	for (int i = 0; i < MAX_STORED_ACCOUNTS; i++) {
		_tcscpy(options.username[i], _T(""));
		_tcscpy(options.password[i], _T(""));
	}
	options.rw = TRUE;
	options.sound = TRUE;
	options.extra_scroll = TRUE;
	options.bind_local_tcp = 0;
	options.bind_local_udp = DEFAULT_UDP_PORT;
	options.restart_last_location = FALSE;
	options.resolution = 1024;
#ifdef UL_DEV
	options.dev_server = 1;
	_tcscpy(options.custom_ip, "127.0.0.1");
#endif
	options.network = INTERNET;
	options.debug = FALSE;
	LoadString(hInstance, IDS_LIVE_GAME_SERVER_IP, options.game_server, sizeof(options.game_server));
	options.welcome_ai = TRUE;
	options.sound_active = TRUE;
	options.music_active = TRUE;
	options.autoreject = FALSE;
	options.autorejoin = TRUE;
	options.nametags = TRUE;
	options.multiline = FALSE;
	options.footsteps = TRUE;
	options.art_prompts = TRUE;
	options.mouselook = FALSE;
	options.invertmouse = FALSE;
	options.log_chat = TRUE;
	options.turnrate = default_turnrate;
	options.effects_volume = default_volume;
	options.music_volume = default_volume;
	options.speech_color = default_speech_color;
	options.message_color = default_message_color;
	options.bg_color = default_bg_color;
	options.reverse = FALSE;
	options.autorun = TRUE;
	options.adult_filter = TRUE;
	options.pmare_type = 0;
	options.pmare_start_type = 0;
	options.pmare_price = 0;
	options.pmare_session_start.wYear = 1970;
	memset(&options.avatar, 0, sizeof(options.avatar));
	options.num_bungholes = 0;
	for (int i = 0; i < MAX_IGNORELIST; i++)
		_tcscpy(options.bungholes[i].name, _T(""));
	keymap->SetDefaultKeymap(0);
}

void SmartLoadJSON()
{
	static bool alreadyLoadedGlobal = false;
	static bool alreadyLoadedLocal = true;

	if (!player && !alreadyLoadedGlobal) {
		LoadJSONOptionValues(NULL);
		alreadyLoadedGlobal = true;
	}
	else if (!alreadyLoadedLocal)
	{
		LoadJSONOptionValues(player->UpperName());
		alreadyLoadedLocal = alreadyLoadedGlobal = true;
	}
}

void LoadInGameRegistryOptionValues(HKEY reg_key, bool force)
{
	if (numJsonFiles) {
		SmartLoadJSON();
		return;
	}

	DWORD keyresult, size, reg_type;

	size = sizeof(options.sound_active);
	keyresult = RegQueryValueEx(reg_key, _T("sound_active"), NULL, &reg_type,
		(unsigned char *)&(options.sound_active), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.sound_active = TRUE;

	size = sizeof(options.music_active);
	keyresult = RegQueryValueEx(reg_key, _T("music_active"), NULL, &reg_type,
		(unsigned char *)&(options.music_active), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.music_active = TRUE;

	size = sizeof(options.autoreject);
	keyresult = RegQueryValueEx(reg_key, _T("autoreject"), NULL, &reg_type,
		(unsigned char *)&(options.autoreject), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.autoreject = FALSE;

	size = sizeof(options.autorejoin);
	keyresult = RegQueryValueEx(reg_key, _T("autorejoin"), NULL, &reg_type,
		(unsigned char *)&(options.autorejoin), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.autorejoin = TRUE;

	size = sizeof(options.nametags);
	keyresult = RegQueryValueEx(reg_key, _T("nametags"), NULL, &reg_type,
		(unsigned char *)&(options.nametags), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.nametags = TRUE;

	size = sizeof(options.multiline);
	keyresult = RegQueryValueEx(reg_key, _T("multiline"), NULL, &reg_type,
		(unsigned char *)&(options.multiline), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.multiline = FALSE;

	size = sizeof(options.footsteps);
	keyresult = RegQueryValueEx(reg_key, _T("footsteps"), NULL, &reg_type,
		(unsigned char *)&(options.footsteps), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.footsteps = TRUE;

	size = sizeof(options.art_prompts);
	keyresult = RegQueryValueEx(reg_key, _T("art_prompts"), NULL, &reg_type,
		(unsigned char *)&(options.art_prompts), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.art_prompts = TRUE;

	size = sizeof(options.mouselook);
	keyresult = RegQueryValueEx(reg_key, _T("mouselook"), NULL, &reg_type,
		(unsigned char *)&(options.mouselook), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.mouselook = FALSE;

	size = sizeof(options.invertmouse);
	keyresult = RegQueryValueEx(reg_key, _T("invertmouse"), NULL, &reg_type,
		(unsigned char *)&(options.invertmouse), &size);
	
	if ((keyresult != ERROR_SUCCESS) || force)
		options.invertmouse = FALSE;
	
	size = sizeof(options.log_chat);
	keyresult = RegQueryValueEx(reg_key, _T("log_chat"), NULL, &reg_type,
		(unsigned char *)&(options.log_chat), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.log_chat = TRUE;

	int turnrate;
	size = sizeof(turnrate);
	keyresult = RegQueryValueEx(reg_key, _T("turnrate"), NULL, &reg_type,
		(unsigned char *)&(turnrate), &size);
	if ((keyresult != ERROR_SUCCESS) || force || 
		(turnrate < min_turnrate) || (turnrate > max_turnrate))
		options.turnrate = default_turnrate;
	else
		options.turnrate = (float)turnrate; // need intermediate var b/c turnrate is a float
		
	size = sizeof(options.effects_volume);
	keyresult = RegQueryValueEx(reg_key, _T("volume"), NULL, &reg_type,
		(unsigned char *)&(options.effects_volume), &size);
	if ((keyresult != ERROR_SUCCESS) || force || 
		(options.effects_volume < min_volume) || (options.effects_volume > max_volume))
		options.effects_volume = default_volume;

	size = sizeof(options.music_volume);
	keyresult = RegQueryValueEx(reg_key, _T("music_volume"), NULL, &reg_type,
		(unsigned char *)&(options.music_volume), &size);
	if ((keyresult != ERROR_SUCCESS) || force || 
		(options.music_volume < min_volume) || (options.music_volume > max_volume))
		options.music_volume = default_volume;

	size = sizeof(options.speech_color);
	keyresult = RegQueryValueEx(reg_key, _T("speech_color"), NULL, &reg_type,
		(unsigned char *)&(options.speech_color), &size);
	if ((keyresult != ERROR_SUCCESS) || force || ColorOutOfRange(options.speech_color))
		options.speech_color = default_speech_color;

	size = sizeof(options.message_color);
	keyresult = RegQueryValueEx(reg_key, _T("message_color"), NULL, &reg_type,
		(unsigned char *)&(options.message_color), &size);
	if ((keyresult != ERROR_SUCCESS) || force || ColorOutOfRange(options.message_color))
		options.message_color = default_message_color;
	
	size = sizeof(options.bg_color);
	keyresult = RegQueryValueEx(reg_key, _T("bg_color"), NULL, &reg_type,
		(unsigned char *)&(options.bg_color), &size);
	if ((keyresult != ERROR_SUCCESS) || force|| ColorOutOfRange(options.bg_color))
		options.bg_color = default_bg_color;

	size = sizeof(options.reverse);
	keyresult = RegQueryValueEx(reg_key, _T("reverse"), NULL, &reg_type,
		(unsigned char *)&(options.reverse), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.reverse = FALSE;

	size = sizeof(options.autorun);
	keyresult = RegQueryValueEx(reg_key, _T("autorun"), NULL, &reg_type,
		(unsigned char *)&(options.autorun), &size);
	
	if ((keyresult != ERROR_SUCCESS) || force)
		options.autorun = TRUE;

	size = sizeof(options.adult_filter);
	keyresult = RegQueryValueEx(reg_key, _T("adult_filter"), NULL, &reg_type,
		(unsigned char *)&(options.adult_filter), &size);
	
	if ((keyresult != ERROR_SUCCESS) || force)
		options.adult_filter = TRUE;

	size = sizeof(options.pmare_type);
	keyresult = RegQueryValueEx(reg_key, _T("pmare_type"), NULL, &reg_type,
		(unsigned char *)&(options.pmare_type), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.pmare_type = 0;

	size = sizeof(options.pmare_start_type);
	keyresult = RegQueryValueEx(reg_key, _T("pmare_start_type"), NULL, &reg_type,
		(unsigned char *)&(options.pmare_start_type), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.pmare_start_type = 0;


	size = sizeof(options.pmare_price);
	keyresult = RegQueryValueEx(reg_key, _T("pmare_price"), NULL, &reg_type,
		(unsigned char *)&(options.pmare_price), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.pmare_price = 0;


	size = sizeof(options.pmare_session_start);
	keyresult = RegQueryValueEx(reg_key, _T("pmare_session_start"), NULL, &reg_type,
		(unsigned char *)&(options.pmare_session_start), &size);
	if ((keyresult != ERROR_SUCCESS) || force)
		options.pmare_session_start.wYear = 1970;

	// save them back, in case a default was set
	SaveInGameRegistryOptionValues();
}

void LoadCharacterRegistryOptionValues(bool force)
{
	// write out new value to registry
	HKEY reg_key = NULL;
	unsigned long result;
	
	RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(false), 0,
		NULL, 0, KEY_ALL_ACCESS, NULL, &reg_key, &result);

	LoadCharacterRegistryOptionValues(reg_key, force);
	RegCloseKey(reg_key);
}

void LoadCharacterRegistryOptionValues(HKEY reg_key, bool force)
{
	if (numJsonFiles) {
		LoadJSONOptionValues(player->UpperName());
		return;
	}

	int num_keys;
	DWORD keyresult, size, reg_type;
	keymap_t *map;

	size = sizeof(options.avatar);
	keyresult = RegQueryValueEx(reg_key, _T("avatar"), NULL, &reg_type,
		(unsigned char *)&(options.avatar), &size);
	if ((keyresult != ERROR_SUCCESS) || force) // default avatar
		memset(&options.avatar, 0, sizeof(options.avatar));

	size = sizeof(options.num_bungholes);
	keyresult = RegQueryValueEx(reg_key, _T("num_ignores"), NULL, &reg_type,
		(unsigned char *)&(options.num_bungholes), &size);
	if ((keyresult != ERROR_SUCCESS) ||
		(options.num_bungholes > MAX_IGNORELIST))
		options.num_bungholes = 0;

	size = MAX_IGNORELIST * sizeof(other_t);
	keyresult = RegQueryValueEx(reg_key, _T("ignore_list"), NULL, &reg_type,
		(unsigned char *)options.bungholes, &size);
	if ((keyresult != ERROR_SUCCESS) || (!options.num_bungholes))
	{
		for (int i = 0; i < MAX_IGNORELIST; i++)
			_tcscpy(options.bungholes[i].name, _T(""));
		options.num_bungholes = 0;
	}

	// keymapping
	num_keys = keymap->num_keys();
	map = new keymap_t[num_keys];
	keymap->GetMap(map);
	size = sizeof(num_keys);
	keyresult = RegQueryValueEx(reg_key, _T("number_keys_mapped"), NULL, &reg_type,
		(unsigned char *)&(num_keys), &size);
	if (keyresult == ERROR_SUCCESS)
	{
		size = num_keys * sizeof(keymap_t);
		delete map;
		map = new keymap_t[num_keys];
		RegQueryValueEx(reg_key, _T("key_mappings"), NULL, &reg_type,
			(unsigned char *)map, &size);
		keymap->Init(num_keys, map);
		delete map;
	}
	else
	{ // may have version without keymap registry entries -- look in main registry first and then pull defaults from code

		HKEY main_key = NULL;
		unsigned long mresult;
		RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(true), 0,
			NULL, 0, KEY_ALL_ACCESS, NULL, &main_key, &mresult);

		keyresult = RegQueryValueEx(main_key, _T("number_keys_mapped"), NULL, &reg_type,
			(unsigned char *)&(num_keys), &size);

		if (keyresult == ERROR_SUCCESS)
		{
			size = num_keys * sizeof(keymap_t);
			delete map;
			map = new keymap_t[num_keys];
			RegQueryValueEx(main_key, _T("key_mappings"), NULL, &reg_type,
				(unsigned char *)map, &size);
			keymap->Init(num_keys, map);
			delete map;
		}
		else
		{
			// okay, we really don't have keys set, use defaults
			keymap->SetDefaultKeymap(0);
			num_keys = keymap->num_keys();
			delete map;
			map = new keymap_t[num_keys];
			keymap->GetMap(map);
			RegSetValueEx(reg_key, _T("number_keys_mapped"), 0, REG_DWORD,
				(unsigned char *)&(num_keys), sizeof(num_keys));
			RegSetValueEx(reg_key, _T("key_mappings"), 0, REG_BINARY,
				(unsigned char *)map, (num_keys * sizeof(keymap_t)));
			delete map;
		}

		RegCloseKey(main_key);
	}

	// save it incase something changed
	SaveCharacterRegistryOptionValues(reg_key);
}
