// cKeymap: User keyboard customization class & keyboard handlers

// Copyright Lyra LLC, 1997. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windows.h>
#include <windowsx.h>
#include "LoginOptions.h"
#include "cKeymap.h"

//////////////////////////////////////////////////////////////////
// External Global Variables

//////////////////////////////////////////////////////////////////
// Class Defintion

// Constructor 
cKeymap::cKeymap(void)
{
	mappings = (keymap_t *)malloc((default_num_keys * sizeof(keymap_t)));
	memcpy (mappings, default_keymap, (default_num_keys * sizeof(keymap_t)));
	num_keys_used = default_num_keys;
}


// Destructor
cKeymap::~cKeymap(void)
{
	free (mappings);
}


void cKeymap::GetMap(keymap_t *map)
{
	memcpy (map, mappings, (num_keys_used * sizeof(keymap_t)));
	return;
}


UINT cKeymap::GetKey(int index)
{
	if ((index < num_keys_used) && (index >= 0))
		return mappings[index].key;
	else return KEY_INDEX_OUT_OF_BOUNDS;
}


int cKeymap::GetFunc(int index)
{
	if ((index < num_keys_used) && (index >= 0))
		return mappings[index].func;
	else return FUNC_INDEX_OUT_OF_BOUNDS;
}


int cKeymap::GetArt(int index)
{
	if ((index < num_keys_used) && (index >= 0))
		return mappings[index].art;
	else return FUNC_INDEX_OUT_OF_BOUNDS;
}


void cKeymap::Init(int initnum_keys, keymap_t *initmap)
{
	mappings = (keymap_t *)realloc(mappings, initnum_keys*(sizeof(keymap_t)));
	for (int counter = 0; counter < initnum_keys; counter++)
	{
		mappings[counter].key = initmap[counter].key;
		mappings[counter].func = initmap[counter].func;
		mappings[counter].art = initmap[counter].art;
	}
	num_keys_used = initnum_keys;
}


void cKeymap::AddMapping (UINT vk, int key_func, int art)
{ // add a new key mapping -- will overwrite a mapping using the same key
	
	// could check key_func for validity for extra safety

	int	found_key = -1;

	// search for key mapping
	for (int counter = 0; counter < num_keys_used; counter++)
		if (vk == mappings[counter].key)
		{
			found_key = counter;
			break;
		}
					
	if (found_key > -1)
	{ // mapping a key that is mapped elsewhere, overwrite other mapping
			mappings[found_key].key = vk;
			mappings[found_key].func = key_func;
			mappings[found_key].art = art;
			return;
	}
	else 
	{ // creating a new mapping, search for null records to put into first
		for (int counter = 0; counter < num_keys_used; counter++)
			if (mappings[counter].key == NULL_MAPPING)
			{
				mappings[counter].key = vk;
				mappings[counter].func = key_func;
				mappings[counter].art = art;
				return;
			}

		// no null keys, make new mapping at end
		num_keys_used++;
		mappings = (keymap_t *)realloc(mappings, num_keys_used * sizeof(keymap_t));
		mappings[(num_keys_used-1)].key = vk;
		mappings[(num_keys_used-1)].func = key_func;
		mappings[(num_keys_used-1)].art = art;
	}

	return;
}


int cKeymap::FindMapping (UINT vk)
{
	int	found_key = MAPPING_NOT_FOUND;

	// search for key mapping
	for (int counter = 0; counter < num_keys_used; counter++)
		if (vk == mappings[counter].key)
		{
			found_key = counter;
			break;
		}

	if (found_key != MAPPING_NOT_FOUND)
		return mappings[found_key].func;
	else return MAPPING_NOT_FOUND;
}


int cKeymap::FindArt (UINT vk)
{
	int	found_key = MAPPING_NOT_FOUND;

	// search for key mapping
	for (int counter = 0; counter < num_keys_used; counter++)
		if (vk == mappings[counter].key)
		{
			found_key = counter;
			break;
		}

	if (found_key != MAPPING_NOT_FOUND)
		return mappings[found_key].art;
	else return MAPPING_NOT_FOUND;
}


int cKeymap::InvalidateMapping (UINT vk)
{
	int	found_key = MAPPING_NOT_FOUND;

	// search for and replace key mapping
	for (int counter = 0; counter < num_keys_used; counter++)
		if (vk == mappings[counter].key)
		{
			mappings[counter].key = NULL_MAPPING;
			found_key = counter;
			break;
		}

	return found_key;					
}


void cKeymap::SetDefaultKeymap(int default_keyboard)
{
	if (default_keyboard == 1)
	{
		mappings = (keymap_t *)realloc(mappings, mousemove_default_num_keys * sizeof(keymap_t));
		memcpy (mappings, mousemove_default_keymap, (mousemove_default_num_keys * sizeof(keymap_t)));
		num_keys_used = mousemove_default_num_keys;
	}
	else 
	{
		mappings = (keymap_t *)realloc(mappings, default_num_keys * sizeof(keymap_t));
		memcpy (mappings, default_keymap, (default_num_keys * sizeof(keymap_t)));
		num_keys_used = default_num_keys;
	}
}
