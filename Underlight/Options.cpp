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
extern macro_t chat_macros[MAX_MACROS];

//////////////////////////////////////////////////////////////////
// Constants

const short min_turnrate = 1;
const short max_turnrate = 20;

const int default_speech_color = 3;
const int default_whisper_color = 3;
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
				ListBox_AddString(GetDlgItem(hDlg, IDC_WHISPCOLOR), ChatColorName(i));
				ListBox_AddString(GetDlgItem(hDlg, IDC_BGCOLOR), ChatColorName(i));
			}
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_SPEECHCOLOR), options.speech_color);
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_MESSAGECOLOR), options.message_color);
			ListBox_SetCurSel(GetDlgItem(hDlg, IDC_WHISPCOLOR), options.whisper_color);
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
				options.whisper_color = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_WHISPCOLOR));
				options.bg_color = ListBox_GetCurSel(GetDlgItem(hDlg, IDC_BGCOLOR));

				display->SetSpeechFormat(options.speech_color);
				display->SetMessageFormat(options.message_color);
				display->SetWhisperFormat(options.whisper_color);
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
#ifndef AGENT
	SaveInGameRegistryOptionValues();
#endif
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
	ADDNUM(bind_local_tcp);
	ADDNUM(bind_local_udp);
#ifdef UL_DEV
	ADDNUM(dev_server);
	cJSON_AddStringToObject(obj, "custom_ip", options.custom_ip);
	ADDNUM(debug);
	ADDNUM(network);
#endif
	cJSON_AddStringToObject(obj, "last_username", options.username[options.account_index]);
	return obj;
}

cJSON* __cdecl WriteJSONOptionValues()
{
	cJSON* obj = cJSON_CreateObject();
	ADDNUM(welcome_ai);
	ADDNUM(sound);
	ADDNUM(extra_scroll);
	ADDNUM(resolution);
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
	ADDNUM(whisper_color);
	ADDNUM(message_color);
	ADDNUM(bg_color);
	ADDNUM(fullscreen);
	ADDNUM(classic_chat);
	ADDNUM(show_effects_hud);
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
		ADDNUM(num_buddies);
		cJSON* friends = cJSON_CreateArray();
		for (int f = 0; f < options.num_buddies; f++)
			cJSON_AddItemToArray(friends, cJSON_CreateString(options.buddies[f].name));
		cJSON_AddItemToObject(obj, "buddies", friends);
		cJSON* macros = cJSON_CreateArray();
		for (int m = 0; m < MAX_MACROS; m++) {
			if (_tcslen(chat_macros[m]) > 0)
				cJSON_AddItemToArray(macros, cJSON_CreateString(chat_macros[m]));
			else
				cJSON_AddItemToArray(macros, cJSON_CreateNull());
		}
		cJSON_AddItemToObject(obj, "macros", macros);
	}
			
#ifdef PMARE
	ADDNUM(pmare_type);
	ADDNUM(pmare_start_type);
	ADDNUM(pmare_price);
	size_t output_length = 0;
	char* pmareSesh = (char*)base64_encode((unsigned char *)&(options.pmare_session_start), sizeof(options.pmare_session_start), &output_length);
	cJSON_AddStringToObject(obj, "pmare_session_start", pmareSesh);
	free(pmareSesh);
#endif

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
			free(string);
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
#ifndef AGENT
	if (!player)
	{
		cJSON* globals = WriteGlobalJSONOptionValues();
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
#endif
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
	if(local)
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
	GETNUM(fullscreen);
	GETNUM(classic_chat);
	GETNUM(log_chat);
	cJSON* tr = cJSON_GetObjectItem(obj, "turnrate"); 
	if (tr && cJSON_IsNumber(tr)) options.turnrate = tr->valuedouble;

	GETNUM(effects_volume);
	GETNUM(music_volume);
	GETNUM(speech_color);
	GETNUM(whisper_color);
	GETNUM(message_color);
	GETNUM(bg_color);
	GETNUM(reverse);
	GETNUM(autorun);
	GETNUM(show_effects_hud);
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

	cJSON* f; cJSON* friends = cJSON_GetObjectItem(obj, "buddies");
	int fIdx = 0;
	cJSON_ArrayForEach(f, friends) {
		if (igIdx < GMsg_LocateAvatar::MAX_PLAYERS && f && cJSON_IsString(f))
		{
			_tcscpy(options.buddies[fIdx++].name, cJSON_GetStringValue(f));
		}
	}
	options.num_buddies = fIdx;

	cJSON* m; cJSON* macros = cJSON_GetObjectItem(obj, "macros");
	int mIdx = 0;
	cJSON_ArrayForEach(m, macros) {
		if (mIdx < MAX_MACROS && m)
		{
			if (cJSON_IsNull(m))
				_tcscpy(chat_macros[mIdx], _T(""));
			else if (cJSON_IsString(m))
				_tcscpy(chat_macros[mIdx], cJSON_GetStringValue(m));
			mIdx++;
		}
	}

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
#ifndef AGENT
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
	options.fullscreen = FALSE;
	options.classic_chat = FALSE;
	options.footsteps = TRUE;
	options.art_prompts = TRUE;
	options.mouselook = FALSE;
	options.invertmouse = FALSE;
	options.log_chat = TRUE;
	options.show_effects_hud = FALSE;
	options.turnrate = default_turnrate;
	options.effects_volume = default_volume;
	options.music_volume = default_volume;
	options.speech_color = default_speech_color;
	options.whisper_color = default_whisper_color;
	options.message_color = default_message_color;
	options.bg_color = default_bg_color;
	options.reverse = FALSE;
	options.autorun = TRUE;
	options.adult_filter = TRUE;
	options.pmare_type = 0;
	options.pmare_start_type = 0;
	options.pmare_price = 0;
	options.tcp_only = TRUE;
	options.pmare_session_start.wYear = 1970;
	memset(&options.avatar, 0, sizeof(options.avatar));
	options.num_bungholes = 0;
	for (int i = 0; i < MAX_IGNORELIST; i++)
		_tcscpy(options.bungholes[i].name, _T(""));
	options.num_buddies = 0;
	for (int i = 0; i < GMsg_LocateAvatar::MAX_PLAYERS; i++)
		_tcscpy(options.buddies[i].name, _T(""));

	for (int i = 0; i < MAX_MACROS; i++)
		_tcscpy(chat_macros[i], _T(""));

	keymap->SetDefaultKeymap(0);
#endif // AGENT
}

void SmartLoadJSON()
{
	static bool alreadyLoadedGlobal = false;
	static bool alreadyLoadedLocal = false;

	if (!player && !alreadyLoadedGlobal) {
		LoadJSONOptionValues(NULL);
		if (strlen(options.username[options.account_index])) {
			LoadJSONOptionValues(options.username[options.account_index]);
		}
		alreadyLoadedGlobal = true;
	}

	if (!alreadyLoadedLocal && player)
	{
		LoadJSONOptionValues(player->UpperName());
		alreadyLoadedLocal = alreadyLoadedGlobal = true;
	}
}
