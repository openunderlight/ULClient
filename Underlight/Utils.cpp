// Utils.cpp: Utility Functions

// Copyright Lyra LLC, 1996. All rights reserved. 

#define STRICT

#include "Central.h"
#include <windows.h>
#include <stdio.h>
#include <math.h> // needed for fabs
#include "Realm.h"
#include "cLevel.h"
#include "cChat.h"
#include "cDDraw.h"
#include "cDSound.h"
#include "cEffects.h"
#include "cPlayer.h"
#include "cArts.h"
#include "cItem.h"
#include "Options.h"
#include "cKeymap.h"
#include "Resource.h"
#include "LmItemDefs.h"
#include "LoginOptions.h"
#include "Interface.h"
#include "Dialogs.h"
#include "Utils.h"

/////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cPlayer *player;
extern cArts *arts;
extern cDDraw *cDD;
extern cDSound *cDS;
extern timing_t *timing;
extern cTimedEffects *timed_effects;
extern cKeymap *keymap;
extern cChat *display;
extern options_t options;
extern long time_offset;
//extern 
// memory profiling
#ifdef _DEBUG
#include <CRTDBG.H>
_CrtMemState extern memstuff;
#endif


/////////////////////////////////////////////////////////////////
// Constants 

const unsigned int MS_PER_TICK = 53; // ms per IBM PC clock tick 
const unsigned int TALISMAN_INDEX_CUTOFF = 250; // anything above this incurs a linear search

// Return a Reg key indexed by charater name
TCHAR* RegPlayerKey(bool fBase /* = false */)
{
	fBase = true; // force common

	if (player == NULL || fBase)
		return REGISTRY_DATA_KEY;

	static TCHAR reg_player_key [sizeof(REGISTRY_DATA_KEY)+Lyra::PLAYERNAME_MAX+2];
	_stprintf(reg_player_key,_T("%s\\%s"),REGISTRY_DATA_KEY,player->Name());
	return reg_player_key;
}

// Get time player has been online
void __cdecl TimeOnline(int* minutes, int* seconds)
{
	unsigned int cur_time = LyraTime();
	unsigned int msec_diff = cur_time - timing->begin_time;
	unsigned int num_secs = (unsigned int)(msec_diff/1000);
	(*minutes) = (int)(num_secs/60);
	(*seconds) = (int)(num_secs%60);
	return;
}

// are t1 and t2 within 48 hours of one another?
bool Within48Hours(SYSTEMTIME t1, SYSTEMTIME t2)
{
  double dt1, dt2;
  SystemTimeToVariantTime (&t1, &dt1);
  SystemTimeToVariantTime (&t2, &dt2);

  if (dt1 < 0.0 ||
      dt2 < 0.0)
      return false;
  return (fabs (dt1 - dt2) <= 2.0);
}
  /*********
   * This is the code for Within24Hours, the predecessor of Within48Hours. :)
   *********
	int num_hours = t2.wHour + (24 - t1.wHour);

	if (t1.wYear != t2.wYear)
	{
		if ((t1.wYear + 1) != t2.wYear)
			return false;
		if ((1 != t2.wMonth) || (1 != t2.wDay))
			return false;
		if (num_hours > 24)
			return false;
		return true;
	}
	if (t1.wMonth != t2.wMonth)
	{
		if ((t1.wMonth + 1) != t2.wMonth)
			return false;
		if (1 != t2.wDay)
			return false;
		if (num_hours > 24)
			return false;
		return true;
	}
	if (t1.wDay != t2.wDay)
	{
		if ((1 + t1.wDay) != t2.wDay)
			return false;
		if (num_hours > 24)
			return false;
	}
	return true;
  */


// Load in game options from the registry; return false if
// registry can not be accessed
bool __cdecl LoadGameOptions(void)
{
	HKEY reg_key = NULL;
	unsigned long result;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0, 
				NULL, 0, KEY_ALL_ACCESS, NULL, &reg_key, &result)
					!= ERROR_SUCCESS)
	{
		GAME_ERROR(IDS_NO_ACCESS_REGISTRY);
		return false;
	}
	LoadInGameRegistryOptionValues(reg_key, false);
	LoadOutOfGameRegistryOptionValues(reg_key, false);

	RegCloseKey(reg_key);

	return true;
}


// check game options to handle on entry; this is here because
// realm.cpp must have __cdecl 
void __cdecl CheckOptions(void)
{

#ifdef AGENT
		
	return;

#else AGENT

	TCHAR start_message[DEFAULT_MESSAGE_SIZE];

#ifdef PMARE

	LoadString (hInstance, IDS_PMARE_WELCOME, start_message, sizeof(start_message));

#else

	if (options.welcome_ai) 
		LoadString (hInstance, IDS_TRAIN_WELCOME, start_message, sizeof(start_message));
	else
		LoadString (hInstance, IDS_NOTRAIN_WELCOME, start_message, sizeof(start_message));

#endif

	   display->DisplayMessage (start_message);

#endif !AGENT
}


// *rough* formula for min use skill: (max - 5) + 2*base

const modifier_t modifier_types[NUM_MODIFIERS] =
 //  die    num    num    min use   min create descrip
{// base    type   dice    skill     skill
                                                       // constants

    {0,      0,     0,       0,        0,      IDS_0       },     
    {1,      0,     0,       0,        2,      IDS_1       },  
    {2,      0,     0,       3,        6,      IDS_2       },  
    {10,     0,     0,       20,       100,    IDS_10      },  
    {15,     0,     0,       30,       100,    IDS_15      },  
    {20,     0,     0,       40,       100,    IDS_20      },  
    {25,     0,     0,       50,       100,    IDS_25      }, 
    {30,     0,     0,       60,       100,    IDS_30      }, 
    {35,     0,     0,       70,       100,    IDS_35      },  
    {40,     0,     0,       80,       100,    IDS_40      },  
    {45,     0,     0,       90,       100,    IDS_45      },  // 10
    {50,     0,     0,       99,       100,    IDS_50      }, 
                                     														// 1d values

    {0,      2,     1,       0,        4,      IDS_1_TO_2    },  
    {0,      3,     1,       0,        6,      IDS_1_TO_3    },  
    {0,      4,     1,       0,         8,     IDS_1_TO_4    }, 
    {3,      4,     1,       9,        18,     IDS_4_TO_7    }, 
    {0,      6,     1,       6,        12,     IDS_1_TO_6    }, 
    {3,      6,     1,       11,        22,    IDS_4_TO_9    },  
    {6,      6,     1,       15,       30,     IDS_7_TO_12   },  
    {0,       8,     1,       8,        16,    IDS_1_TO_8    },    


 //         die    num    min use  min create  descrip
 // base    type   dice    skill     skill
    {4,      8,     1,       14,       28,      IDS_5_TO_12    },  // 20
    {8,      8,     1,       20,       40,      IDS_9_TO_16    },  
    {0,      10,    1,       10,       20,      IDS_1_TO_10    },
    {5,      10,    1,       18,       36,      IDS_6_TO_15    },  
    {0,      12,    1,       12,       24,      IDS_1_TO_12    },  
    {6,      12,    1,       21,       42,      IDS_7_TO_18    },  
    {0,      20,    1,       20,       40,      IDS_1_TO_20    },  
    {0,      30,    1,       30,       60,      IDS_1_TO_30    },  
    {0,      40,    1,       40,       80,      IDS_1_TO_40    },  
    {0,      50,    1,       50,       100,     IDS_1_TO_50    },
                                     // 2d values
    {0,      3,     2,       7,        21,      IDS_2_TO_6      },  // 30
    {2,      4,     2,       12,       24,      IDS_4_TO_10     },  
    {3,      6,     2,       15,       30,      IDS_5_TO_13     }, 
    {8,      6,     2,       27,       54,      IDS_10_TO_22    },  
    {0,      8,     2,       17,       34,      IDS_2_TO_16     },  
    {7,      8,     2,       27,       54,      IDS_9_TO_23     },  
    {0,      10,    2,       21,       42,      IDS_2_TO_20     },  
    {1,      12,    2,       25,       50,      IDS_2_TO_24     },  
    {10,     15,    2,       46,       92,      IDS_12_TO_40    },     
    {0,      20,    2,       41,       82,      IDS_2_TO_40     },
//         die    num    min use  min create  descrip
// base    type   dice    skill     skill
                                     // 3d values
    {0,      2,     3,       8,        17,     IDS_3_TO_6      },  // 40
    {0,      3,     3,       11,       23,     IDS_3_TO_9      },  
    {2,      4,     3,       20,       40,     IDS_5_TO_14     },
    {0,      6,     3,       23,       46,     IDS_3_TO_18     },  
    {4,      6,     3,       31,       62,     IDS_7_TO_22     },  
    {0,      8,     3,       31,       62,     IDS_3_TO_24     },  
    {0,      10,    3,       38,       77,     IDS_3_TO_30     },  
                                     // 4d values
    {0,      2,     4,       11,       22,      IDS_4_TO_8     },  
    {0,      3,     4,       15,       30,      IDS_4_TO_12    },  
    {0,      4,     4,       21,       42,      IDS_4_TO_16    },    
    {1,      5,     4,       27,       54,      IDS_5_TO_21    },  // 50
    {1,      6,     4,       33,       66,      IDS_5_TO_25    },  
                                     // 5+d values
    {0,      2,     5,       14,       28,      IDS_5_TO_10       },  
    {0,      3,     5,       20,       40,      IDS_5_TO_15     },  
    {0,      2,     6,       16,       33,      IDS_6_TO_12     },  
    {0,      3,     6,       24,       48,      IDS_6_TO_18    },  
    {0,      2,     7,       19,       39,      IDS_7_TO_14     },  
    {0,      2,     8,       22,       44,      IDS_8_TO_16     },  
    {0,      2,     9,       25,       50,      IDS_9_TO_18     },  
    {0,      2,    10,       27,       54,      IDS_10_TO_20    },     
    {0,      3,    10,       40,       80,      IDS_10_TO_30    },     // 60


                                     // gm only
    {0,      100,   1,       99,      100,     IDS_1_TO_99    },  
    {0,      10,    10,       99,      100,    IDS_10_TO_99    },  
    {0,     0,     0,       40,      100,     IDS_0      },  

};

// calculate modifier based on table above
int CalculateModifier(int modifier)
{
	int value = 0, index = modifier;
	bool negate = FALSE;

	if (index < 0)
	{
		index = -index;
		negate = true;
	}
	for (int i=0; i<modifier_types[index].num_dice; i++)
		if (modifier_types[index].die_type)
			value += (rand()%(modifier_types[index].die_type)+1);
	if (negate)
		return (-(value + modifier_types[index].base));
	else
		return (value + modifier_types[index].base);
}

// calculate max modifier possible for category, based on table above
int CalculateModifierMax(int modifier)
{
	return (modifier_types[modifier].base + 
		(modifier_types[modifier].die_type -  1) * modifier_types[modifier].num_dice);
}

// calculate inx modifier possible for category, based on table above
int CalculateModifierMin(int modifier)
{
	return modifier_types[modifier].base;
}

// table of duration types
const duration_t duration_types[NUM_DURATIONS] = 
 //			 	     min create	   
{// base    random     skill       descrip 
				 // constants
    {0,       0,       0,          IDS_0_SEC      }, 
    {1,       0,       0,          IDS_1_SEC     },
    {2,       0,       1,          IDS_2_SEC    },
    {3,       0,       2,          IDS_3_SEC    },
    {4,       0,       3,          IDS_4_SEC    },
    {5,       0,       5,          IDS_5_SEC    },
    {10,      0,       8,          IDS_10_SEC   },
    {15,      0,       10,         IDS_15_SEC   },
    {20,      0,       12,         IDS_20_SEC   },
    {30,      0,       15,         IDS_30_SEC   },
    {40,      0,       17,         IDS_40_SEC   }, // 10
    {45,      0,       20,         IDS_45_SEC   }, 
    {50,      0,       22,         IDS_50_SEC   },
    {60,      0,       25,         IDS_1_MIN    },
    {90,      0,       27,         IDS_1_MIN_5_SEC  },
    {120,     0,       30,         IDS_2_MIN      },
    {180,     0,       100,         IDS_3_MIN      },
    {240,     0,       100,         IDS_4_MIN      },
    {300,     0,       100,         IDS_5_MIN      },
    {360,     0,       100,         IDS_6_MIN      },
    {420,     0,       100,         IDS_7_MIN      }, // 20
//               min create    
// base    random     skill       descrip
    {480,     0,       100,         IDS_8_MIN  },
    {540,     0,       100,         IDS_9_MIN  },
    {600,     0,       100,         IDS_10_MIN },
    {900,     0,       100,         IDS_15_MIN },
    {1200,    0,       100,         IDS_20_MIN },
    {1800,    0,       100,         IDS_30_MIN },
    {2700,    0,       100,         IDS_45_MIN },
    {3600,    0,       100,         IDS_1_HR     },
    {7200,    0,       100,         IDS_2_HR    },
                        // variable - seconds
    {1,       11,      1,         IDS_1_TO_10_SEC        }, // 30
    {1,       16,      3,         IDS_1_TO_15_SEC       },
    {1,       31,      6,         IDS_1_TO_30_SEC       },
    {1,       46,      9,         IDS_1_TO_45_SEC        },
    {1,       61,      12,        IDS_1_TO_60_SEC          },
    {10,      11,      15,         IDS_10_TO_20_SEC        },
    {15,      16,      18,         IDS_15_TO_30_SEC        },
    {20,      21,      19,         IDS_20_TO_40_SEC        },
    {30,      31,      22,         IDS_30_TO_60_SEC       },
    {30,      61,      25,         IDS_30_TO_90_SEC        },
    {30,      66,      28,         IDS_30_TO_95_SEC        }, // 40
//               min create    
// base    random     skill       descrip
                        // variable - minutes
    {60,       60,      31,         IDS_1_TO_2_MIN          },
    {60,       120,     34,         IDS_1_TO_3_MIN        },
    {60,       180,     37,         IDS_1_TO_4_MIN        },
    {60,       240,     40,         IDS_1_TO_5_MIN        },
    {60,       540,     48,         IDS_1_TO_10_MIN      },
    {60,       840,     57,         IDS_1_TO_15_MIN      },
    {60,       1740,    66,         IDS_1_TO_30_MIN      },
    {60,       3540,    75,         IDS_1_TO_60_MIN      },
    {180,      180,     45,         IDS_3_TO_6_MIN       },
    {240,      300,     51,         IDS_4_TO_9_MIN       }, // 50
    {300,      360,     54,         IDS_5_TO_10_MIN      },
    {300,      600,     60,         IDS_5_TO_15_MIN      },
    {600,      600,     63,         IDS_10_TO_20_MIN       },
    {600,      1200,    69,         IDS_10_TO_30_MIN       },
    {1200,     1200,    62,         IDS_20_TO_40_MIN        },
    {600,      3000,    80,         IDS_10_TO_60_MIN        },
    {1800,     1800,    84,         IDS_30_TO_60_MIN        },
    {1800,     3600,    88,         IDS_30_TO_90_MIN        },
                           // variable - hours
    {3600,     3600,    90,         IDS_1_TO_2_HR        },
    {3600,     7200,    95,         IDS_1_TO_3_HR        }, // 60
           // gm only
    {14400,    0,      100,         IDS_4_HR      },
    {21600,    0,      100,         IDS_6_HR      },
    {28800,    0,      100,         IDS_8_HR      }, // 63
};


// calculates a duration based on the duration table above
int CalculateDuration(int index) 
{ 
	int base = 1000*(duration_types[index].base);
	int random = 0;
	if (duration_types[index].random)
		random = 1000*((rand()%(duration_types[index].random))+1); 
	return (base + random);
};

const velocity_t velocity_types[] =
{// min skill
 // to create   descrip
	{0,       IDS_MELEE_WEAPON},
	{0,       IDS_MINIMUM},
	{5,       IDS_VERY_SLOW},
	{15,      IDS_SLOW},
	{30,      IDS_MODERATE},
	{50,      IDS_FAST},
	{70,      IDS_VERY_FAST},
	{90,      IDS_FASTEST},
};

// Translate a value into a human readable string and sticks it into
// the "message" global variable

void TranslateValue(int type, int value)
{
	switch (type)
	{
		case LyraItem::TRANSLATION_MODIFIER:
			if (value >= 0)
			{
				LoadString(hInstance, modifier_types[value].descrip, disp_message, sizeof(disp_message));
				_stprintf(message, _T("+%s"), disp_message);
			}
			else
			{	
				LoadString(hInstance, modifier_types[-value].descrip, disp_message, sizeof(disp_message));
				_stprintf(message, _T("-%s"), disp_message);
			}
			break;

		case LyraItem::TRANSLATION_DURATION:
			{
				LoadString(hInstance, duration_types[value].descrip, disp_message, sizeof(disp_message));
				_stprintf(message, _T("%s"), disp_message);
			}
			break;

		case LyraItem::TRANSLATION_EFFECT:
			{
				//LoadString(hInstance, timed_effects->name[value], disp_message, sizeof(disp_message));
				//_stprintf(message, _T("%s"), disp_message);
				_stprintf(message, _T("%s"), timed_effects->name[value]);
			}
			break;

		case LyraItem::TRANSLATION_STAT:
			{
				_stprintf(message, _T("%s"), player->StatName(value));
			}
			break;

		case LyraItem::TRANSLATION_GUILD:
			{
				_stprintf(message, _T("%s"), GuildName(value));
			}
			break;

		case LyraItem::TRANSLATION_GUILDTOKEN:
			{
				_stprintf(message, _T("%s %s"), GuildName((value>>4) & 0x0f), TokenName(value & 0x0f));
			}
			break;

		case LyraItem::TRANSLATION_ART: // note we need to subtract 1 to get the "real" art value
			{
				_stprintf(message, _T("%s"), arts->Descrip(value-1));
			}
			break;

		case LyraItem::TRANSLATION_MISSILE_BITMAP:
			if (value == LyraBitmap::FIREBALL_MISSILE)
				LoadString(hInstance, IDS_FIREBALL, message, sizeof(message));
			else if ((value == LyraBitmap::DREAMBLADE_MISSILE) ||
					 (value == LyraBitmap::PUSH_MISSILE))
				LoadString(hInstance, IDS_MELEE, message, sizeof(message));
			break;

		case LyraItem::TRANSLATION_VELOCITY:
			if (value >= 0)
			{
				LoadString(hInstance, velocity_types[value].descrip, message, sizeof(message));
			}
			else
			{
				TCHAR buffer[DEFAULT_MESSAGE_SIZE];
				LoadString(hInstance, velocity_types[-value].descrip, disp_message, sizeof(disp_message));
				LoadString(hInstance, IDS_BOUNCING, buffer, sizeof(buffer));
				_stprintf(message, buffer, disp_message);
			}
			break;

		case LyraItem::TRANSLATION_POS_MODIFIER:
			{
				LoadString(hInstance, modifier_types[value].descrip, message, sizeof(message));
			}
			break;

		case LyraItem::TRANSLATION_ABSORPTION:
			{
				_stprintf(message, _T("%d%s"), value,_T("%"));
			}
			break;
		
		case LyraItem::TRANSLATION_NIGHTMARE:
			{
				if (value == Avatars::MALE)
					LoadString(hInstance, IDS_MALE_AVATAR, message, sizeof(message));
				else if (value == Avatars::FEMALE)
					LoadString(hInstance, IDS_FEMALE_AVATAR, message, sizeof(message));
				else
					_stprintf(message, _T("%s"), NightmareName(value));
			}
			break;
	
		case LyraItem::TRANSLATION_LEVEL_ID:
		case LyraItem::TRANSLATION_DURABILITY:
		case LyraItem::TRANSLATION_TPORT_DEST:
		case LyraItem::TRANSLATION_NONE:
		default:
			{
				_stprintf(message, _T("%d"), value);
			}
			break;
	}
	return;
}

// returns true if the player has this value as an option for
// translation type when using forge talisman; for example, a player
// must have a certain skill to create weapons with high damage
// categories, and must have access to the related arts to create
// items that cause timed effects
bool CanPlayerForgeValue(int type, int value, bool usePowerToken)
{
	int forge_skill = arts->EffectiveForgeSkill(player->Skill(Arts::FORGE_TALISMAN), usePowerToken);
	switch (type)
	{
		case LyraItem::TRANSLATION_MODIFIER:
			if (value >= 0)
				return (forge_skill >= modifier_types[value].min_skill_to_create);
			else
				return (forge_skill >= modifier_types[-value].min_skill_to_create);
		case LyraItem::TRANSLATION_DURATION:
			return (forge_skill >= duration_types[value].min_skill_to_create);
		case LyraItem::TRANSLATION_EFFECT:
			if (value > LyraEffect::MAX_ITEM_EFFECT)
				return false;
			else if (value == LyraEffect::NONE)
				return true;
			else if (value >= LyraEffect::PLAYER_RECALL)
				return false;
			else if (value == LyraEffect::PLAYER_INVISIBLE)
				return false;
			else if (value == LyraEffect::PLAYER_BLEED)
				return false;
			else
				return (player->Skill(timed_effects->related_art[value]) > 0);
		case LyraItem::TRANSLATION_STAT:
			return true; // no check
		case LyraItem::TRANSLATION_GUILD:
			return true; // no check
		case LyraItem::TRANSLATION_GUILDTOKEN:
			return true; // no check
		case LyraItem::TRANSLATION_ART:
			return true; // no check
		case LyraItem::TRANSLATION_MISSILE_BITMAP:
			return true; // no check
		case LyraItem::TRANSLATION_VELOCITY:
			if (value >= 0)
				return (forge_skill >= velocity_types[value].min_skill_to_create);
			else
				return (forge_skill >= velocity_types[-value].min_skill_to_create);
		case LyraItem::TRANSLATION_POS_MODIFIER:
			return (forge_skill >= modifier_types[value].min_skill_to_create);
		case LyraItem::TRANSLATION_ABSORPTION: 
			return (forge_skill >= value);
		case LyraItem::TRANSLATION_DURABILITY: 
			return (forge_skill >= value);
		case LyraItem::TRANSLATION_NIGHTMARE:
			return true;
		case LyraItem::TRANSLATION_NONE:
		default:
			return true; // no check
	}
}

// returns min skill needed to use a weapon
int	MinModifierSkill(int value)
{
	return modifier_types[value].min_skill_to_use;
}

int NumberTranslations(int type)
{
	switch (type)
	{
		case LyraItem::TRANSLATION_MODIFIER:
			return 2*NUM_MODIFIERS-1; // to count the negative values
		case LyraItem::TRANSLATION_DURATION:
			return NUM_DURATIONS;
		case LyraItem::TRANSLATION_EFFECT:
			return LyraEffect::MAX_ITEM_EFFECT+1;
		case LyraItem::TRANSLATION_STAT:
			return NUM_PLAYER_STATS;
		case LyraItem::TRANSLATION_GUILD:
			return NUM_GUILDS;
		case LyraItem::TRANSLATION_GUILDTOKEN:
			return 0; // guildtoken translations aren't enum'd -- never use in loops!
		case LyraItem::TRANSLATION_ART:
			return NUM_ARTS+1;
		case LyraItem::TRANSLATION_MISSILE_BITMAP:
			return NUM_MISSILE_BITMAPS;
		case LyraItem::TRANSLATION_VELOCITY:
			return (abs(MIN_VELOCITY) + MAX_VELOCITY +1);
		case LyraItem::TRANSLATION_POS_MODIFIER:
			return NUM_MODIFIERS;
		case LyraItem::TRANSLATION_ABSORPTION:
		case LyraItem::TRANSLATION_DURABILITY:
			return 99;
		case LyraItem::TRANSLATION_NIGHTMARE:
			return Avatars::MAX_AVATAR_TYPE+1;
		case LyraItem::TRANSLATION_LEVEL_ID:
		    return MAX_LEVELS;
		default:
		case LyraItem::TRANSLATION_NONE:
			return 0;
	}
}

DWORD __cdecl LyraTime(void)
{
	//LARGE_INTEGER curtime;
	//QueryPerformanceCounter(&curtime);
	//return (DWORD)(curtime.LowPart + time_offset);
	unsigned int time = (unsigned int)timeGetTime();
	//_tprintf("TGT: %u QPC: %u\n", time, curtime.LowPart);
	return time + time_offset;
}


void SetCounter(void)
{
   // nmsecs is the exact ms count since the last update.
   // nticks is the exact (float) number of 53ms ticks that 
   //     have occurred since the last update
   // sync_ticks is the number of integer ticks that have
   //     occured since the last update; this is used for
   //     timing that must remain synchronized among players,
   //     such as for gravity and event updates

	if (!timing)
		return; // safety check for mares
   
   timing->t_end = LyraTime();
   timing->nmsecs = timing->t_end - timing->t_start;
   timing->t_start = timing->t_end;
   timing->nticks = (float((timing->nmsecs & 0xfff)/16));
   timing->sync_msecs += timing->nmsecs; 
   timing->sync_ticks = 0;
   while (timing->sync_msecs > MS_PER_TICK)
   {
	   timing->sync_ticks++;
	   timing->sync_msecs -= MS_PER_TICK;
   }
   return;
}

struct guild_name_t
{
	UINT			name; // pointer to string table
	int				flag;
	unsigned char	levelid; // level of guild's house plane
};

guild_name_t guild_names[NUM_GUILDS+1] =
{
	{IDS_OOSM, Guild::MOON_FLAG, 25},
	{IDS_AOE, Guild::ECLIPSE_FLAG, 22},
	{IDS_KOES, Guild::SHADOW_FLAG, 24},
	{IDS_UOC, Guild::COVENANT_FLAG, 26},
	{IDS_POR, Guild::RADIANCE_FLAG, 21},
	{IDS_HC, Guild::CALENTURE_FLAG, 17},
	{IDS_GOE, Guild::ENTRANCED_FLAG, 23},
	{IDS_DOL, Guild::LIGHT_FLAG, 18},		
	{IDS_ANY_HOUSE, 0, 0},

};

TCHAR* GuildName(int guild_id)
{

	if ((guild_id < 0) || (guild_id >= NUM_GUILDS))
		LoadString(hInstance, guild_names[NUM_GUILDS].name, guild_name_message, sizeof(guild_name_message));
	else
		LoadString(hInstance, guild_names[guild_id].name, guild_name_message, sizeof(guild_name_message));
	return guild_name_message;
}

// translates a guild id into a guild flag
int GuildFlag(int guild_id)
{
	for (int i=0; i<NUM_GUILDS; i++)
		if (i == guild_id)
			return guild_names[i].flag;
	return 0;
}

// translates a guild flag into a guild id
int	GuildID(int guild_flag)
{
	for (int i=0; i<NUM_GUILDS; i++)
		if (guild_flag == guild_names[i].flag)
			return i;
	return Guild::NO_GUILD;
}

// translates a guild id into a level id
int GuildLevel(int guild_id)
{
	for (int i = 0; i<NUM_GUILDS; i++)
		if (i == guild_id)
			return guild_names[i].levelid;
	return 0;
}

// translates a level id into a guild id
int LevelGuild(int level_id)
{
	for (int i = 0; i<NUM_GUILDS; i++)
		if (level_id == guild_names[i].levelid)
			return i;
	return Guild::NO_GUILD;
}

struct rank_name_t
{
	UINT			name; // string table pointer
	UINT			goalname; // string table pointer
};

rank_name_t rank_names[NUM_RANKS+1] =
{
	{IDS_NONE, IDS_GOAL},
	{IDS_INITIATE, IDS_MISSION},
	{IDS_GUARDIAN, IDS_GOAL},
	{IDS_RULER, IDS_GOAL},
};

TCHAR* RankName(int rank)
{
	if ((rank < 0) || (rank > NUM_RANKS))
		LoadString(hInstance, rank_names[0].name, guild_rank_message, sizeof(guild_rank_message));
	else
		LoadString(hInstance, rank_names[rank].name, guild_rank_message, sizeof(guild_rank_message));
	return guild_rank_message;
}

TCHAR* RankGoalName(int rank)
{
	if ((rank < 0) || (rank >= NUM_RANKS))
		LoadString(hInstance, rank_names[0].goalname, guild_goal_message, sizeof(guild_goal_message));
	else
		LoadString(hInstance, rank_names[rank].goalname, guild_goal_message, sizeof(guild_goal_message));
	return guild_goal_message;
}

struct color_name_t
{
	UINT			name; // pointer to string table
};
/*
color_name_t color_mares[NUM_ACTOR_COLORS] =
{
	{_T("Earth")},
	{_T("Earth (Lt)")},
	{_T("Life")},
	{_T("Life (Lt)")},
	{_T("Water")},
	{_T("Water (Lt)")},
	{_T("Battle")},
	{_T("Battle (Lt)")},
	{_T("Nothing")},	{_T("Nothing")},	{_T("Nothing")},	{_T("Nothing")},
	{_T("Nothing")},	{_T("Nothing")},	{_T("Nothing")},	{_T("Nothing")},
};
*/
// add one for ANY_COLOR
color_name_t color_names[NUM_ACTOR_COLORS+1] =
{
  {IDS_CHALK},
  {IDS_BLOOD},
  {IDS_FIRE},
  {IDS_GOLD},

  {IDS_JADE},
  {IDS_TEAL},
  {IDS_CYAN},
  {IDS_NIGHT},

  {IDS_AZURE},
  {IDS_PLUM},
  {IDS_BERRY},
  {IDS_SAND},

  {IDS_BEIGE},
  {IDS_TAN},
  {IDS_EARTH},
  {IDS_ABYSS},
  {IDS_ANY_COLOR}

};

// Jared 6-16-00
// Add some meaningful names in the near future
color_name_t monster_color_names[NUM_MONSTER_COLORS] =
{
	{IDS_BIRCH},
	{IDS_MUD},
	{IDS_FADED_BIRCH},
	{IDS_MIDNIGHT},
	{IDS_SNOW},
	{IDS_NIGHT},
	{IDS_LIGHT_RED},
	{IDS_BLOOD},
	{IDS_CHARCOAL},
	{IDS_IVORY},
	{IDS_LEMON},
	{IDS_LIME},
	{IDS_AQUA},
	{IDS_GRAPE},
	{IDS_EMBER},
	{IDS_EERIE_GLOW},
};

TCHAR* MonsterColorName(int color)
{
	LoadString(hInstance, monster_color_names[color].name, monster_color_message, sizeof(monster_color_message));
	return monster_color_message;
}


TCHAR* ColorName(int color)
{
	LoadString(hInstance, color_names[color].name, color_message, sizeof(color_message));
	return color_message;
}

void ColorName(int color, TCHAR* buffer, int bufsize)
{
	LoadString(hInstance, color_names[color].name, buffer, bufsize);
}


struct token_name_t
{
	UINT	name; // pointer to string table
};

token_name_t token_names[NUM_TOKENS+1] =
{
	{IDS_NONE},
	{IDS_MEMBERSHIP},
	{IDS_ASCENSION_TO_RULER},
	{IDS_DEMOTION},
	{IDS_POWER_TOKEN}
};

TCHAR* TokenName(int token_type)
{
	LoadString(hInstance, token_names[token_type].name, token_message, sizeof(token_message));
	return token_message;
}


struct nightmare_name_t
{
	UINT	name; // pointer to string table
};

nightmare_name_t nightmare_names[Avatars::MAX_AVATAR_TYPE + 1] =
{
	{IDS_MALE_REVENANT},
	{IDS_FEMALE_REVENANT},
//	{IDS_MALE_AVATAR},
//	{IDS_FEMALE_AVATAR},
	{IDS_EMPHANT},
	{IDS_BOGROM},
	{IDS_AGOKNIGHT},
	{IDS_SHAMBLIX},
	{IDS_HORRON},
};

TCHAR* __cdecl NightmareName(int id)
{
	LoadString(hInstance, nightmare_names[id].name, nightmare_message, sizeof(nightmare_message));
	return nightmare_message;
}

struct dreamweapon_name_t
{
	UINT	name; // string table pointer
	int		color;
};

dreamweapon_name_t dreamweapon_names[] =
{
	{IDS_NO_WEAPON, -1},
	{IDS_GATESMASHER, 0},
	{IDS_DREAMBLADE, 1},
	{IDS_SOULREAPER, 2},
	{IDS_FATESLAYER, 3}
};

TCHAR *DreamweaponName(int color)
{
#ifdef PMARE // Just hard-code the name for pmares
	LoadString(hInstance, IDS_BLADE, dreamweapon_message, sizeof(dreamweapon_message));
#else

	//LoadString(hInstance, dreamweapon_names[color-1].name, dreamweapon_message, sizeof(dreamweapon_message));
	for (int i=0; i<(sizeof(dreamweapon_names)/sizeof(dreamweapon_name_t)); i++)
		if (dreamweapon_names[i].color == (int)(color/4))
			LoadString(hInstance, dreamweapon_names[i].name, dreamweapon_message, sizeof(dreamweapon_message));
#endif

	return dreamweapon_message;

}

struct talisman_name_t
{
	bool    forgable;
	UINT	name; // string table pointer
	int		bitmap_id;
};

talisman_name_t talisman_names[] =
{
	{false,IDS_UNKNOWN, LyraBitmap::NONE},
	{false,IDS_BLADE, LyraBitmap::DREAMBLADE},
	{false,IDS_WARD, LyraBitmap::WARD},
	{false,IDS_WARD_PASS_AMULET, LyraBitmap::AMULET},
	{false,IDS_EMPH_ESS_TALISMAN, LyraBitmap::EMPHANT_ESSENCE},
	{false,IDS_BOG_ESS_TALISMAN, LyraBitmap::BOGROM_ESSENCE},
	{false,IDS_AGO_ESS_TALISMAN, LyraBitmap::AGOKNIGHT_ESSENCE},
	{false,IDS_SHAM_ESS_TALISMAN, LyraBitmap::SHAMBLIX_ESSENCE},
	{false,IDS_MALE_AV_ESS_TOKEN, LyraBitmap::M_AVATAR_ESSENCE},
	{false,IDS_FEMALE_AVT_ESS_TOKEN, LyraBitmap::F_AVATAR_ESSENCE},
	{false,IDS_ASCENSION_TOKEN, LyraBitmap::GUILD_ASCENSION_TOKEN},
	{false,IDS_DEMOTE_TOKEN, LyraBitmap::GUILD_DEMOTION_TOKEN},
	{false,IDS_OOSM_MEMBER_TOKEN, LyraBitmap::GUILD_MEMBER_TOKEN_BASE},
	{false,IDS_AOE_MEMBER_TOKEN, LyraBitmap::GUILD_MEMBER_TOKEN_BASE + 1},
	{false,IDS_KOES_MEMBER_TOKEN, LyraBitmap::GUILD_MEMBER_TOKEN_BASE + 2},
	{false,IDS_UOC_MEMBER_TOKEN, LyraBitmap::GUILD_MEMBER_TOKEN_BASE + 3},
	{false,IDS_POR_MEMBER_TOKEN, LyraBitmap::GUILD_MEMBER_TOKEN_BASE + 4},
	{false,IDS_HC_MEMBER_TOKEN, LyraBitmap::GUILD_MEMBER_TOKEN_BASE + 5},
	{false,IDS_GOE_MEMBER_TOKEN, LyraBitmap::GUILD_MEMBER_TOKEN_BASE + 6},
	{false,IDS_DOL_MEMBER_TOKEN, LyraBitmap::GUILD_MEMBER_TOKEN_BASE + 7},
	{false,IDS_CLEANSED_NIGHTMARE_TOKEN, LyraBitmap::CLEANSED_MARE},
	{false,IDS_BANISHED_NIGHTMARE_TOKEN, LyraBitmap::BANISHED_MARE},
	{false,IDS_IMPRISONED_NIGHTMARE_TOKEN, LyraBitmap::ENSLAVED_MARE},
	{false,IDS_GUILD_ESS_TALISMAN, LyraBitmap::META_ESSENCE},
	{false,IDS_SOUL_ESS_TALISMAN, LyraBitmap::SOUL_ESSENCE},
	{true,IDS_CODEX, LyraBitmap::TALISMAN0}, // RRR was Artifax
	{true,IDS_ELEMEN, LyraBitmap::TALISMAN1}, 
	{true,IDS_CHARM, LyraBitmap::TALISMAN2}, 
	{true,IDS_CHAKRAM, LyraBitmap::TALISMAN3}, 
	{true,IDS_ALTEROR, LyraBitmap::TALISMAN4}, 
	{true,IDS_SHIELD, LyraBitmap::TALISMAN5}, 
	{true,IDS_TALIS, LyraBitmap::TALISMAN6}, 
	{true,IDS_COMPENDIUM, LyraBitmap::TALISMAN7}, // RRR was Artifax
	{true,IDS_ARTIFAX, LyraBitmap::TALISMAN8}, 
	{true, IDS_SCROLL, LyraBitmap::SCROLL},
	{true, IDS_FLOWER, LyraBitmap::FLOWER},
	{false, IDS_CHAOS_WELL, LyraBitmap::BOX},
	{true, IDS_STAFF, LyraBitmap::STAFF},
	{true, IDS_GIFT, LyraBitmap::GIFT},
	{true, IDS_RING, LyraBitmap::RING},
	{true, IDS_EGG, LyraBitmap::EGG},
	{true, IDS_FEATHER, LyraBitmap::FEATHER},
	{false,IDS_CODEX,   LyraBitmap::CODEX},

	/// RRR Names Clarified
	{false,IDS_FLAG_DOL , 63},
	{false,IDS_FLAG_HC , 64},
	{false,IDS_FLAG_AOE , 65},
	{false,IDS_FLAG_DOL2 , 67},
	{false,IDS_FLAG_GOE , 68},
	{false,IDS_FLAG_HC2 , 69},
	{false,IDS_FLAG_KOES , 70},
	{false,IDS_FLAG_OOSM , 71},
	{false,IDS_FLAG_POR , 72},
	{false,IDS_FLAG_UOC , 73},
	{false,IDS_SCROLL_POLE, 74},
	{false,IDS_CREST_AOE , 77},
	{false,IDS_CREST_HC , 78},
	{false,IDS_CREST_UOC , 79},
	{false,IDS_CREST_DOL , 80},
	{false,IDS_CREST_GOE , 81},
	{false,IDS_CREST_KOES , 82},
	{false,IDS_CREST_POR , 83},
	{false,IDS_CREST_OOSM , 84},
	{false,IDS_BANISHMENT , 88},
	{false,IDS_DIAMOND , 89},
	{false,IDS_INVIS , 90},
	{false,IDS_PEARL , 91},
	{false,IDS_STAR , 92},
	{false,IDS_CTF_PLUM , 93},
	{false,IDS_CTF_BLOOD , 94},
	{false,IDS_CTF_CYAN , 95},
	{false,IDS_CTF_CHALK , 96},
	{false,IDS_CTF_JADE , 97},
	{ false, IDS_TORCH, 794 },
	{ false, IDS_LUMITWIST, 802 },
	{ false, IDS_RAIN, 805 },
	{ false, IDS_HELI, 811 },
	{ false, IDS_STATUE_1, 822 },
	{ false, IDS_STATUE_2, 823 },
	{ false, IDS_FEM_STATUE_1, 824 },
	{ false, IDS_FEM_STATUE_2, 825 },
	{ false, IDS_FEM_STATUE_3, 826 },
	{ false, IDS_FEM_STATUE_4, 827 },
	{ false, IDS_ST_L, 830 },
	{ false, IDS_ST_I, 831 },
	{ false, IDS_ST_W, 832 },
	{ false, IDS_WEP_A, 833 },
	{ false, IDS_DRIP, 834 },
	{ false, IDS_FI_SM, 857 },
	{ false, IDS_FI_MD, 863 },
	{ false, IDS_FIRE, 874 },
	{ false, IDS_TORCH_1, 885 },
	{ false, IDS_TORCH_2, 896 },
	{ false, IDS_EYES_1, 903 },
	{ false, IDS_EYES_2, 912 },
	{ false, IDS_W_HOLE, 921 },
	{ false, IDS_S_ALT2, 930 },
	{ false, IDS_ALTAR1, 937 },
	{ false, IDS_DTORCH, 938 },
	{ false, IDS_LIGHTNING, 947 },
	{ false, IDS_LIGHTNING_SM, 951 },
	{ false, IDS_AOE_ORNAMENT, 957 },
	{ false, IDS_TELESCOPE, 958 },
	{ false, IDS_VINE1, 959 },
	{ false, IDS_VINE2, 960 },
	{ false, IDS_VINE3, 961 },
	{ false, IDS_VINE4, 962 },
	{ false, IDS_DOL_CL_BNR, 963 },
	{ false, IDS_DOL_FL_BNR, 964 },
	{ false, IDS_HC_CL_BNR, 965 },
	{ false, IDS_HC_CL_PCE, 966 },
	{ false, IDS_HC_CL_SYM, 967 },
	{ false, IDS_HC_FL_BNR, 968 },
	{ false, IDS_HC_RAIL, 969 },
	{ false, IDS_HC_TETS, 970 },
	{ false, IDS_TREE_HAND, 971 },
	{ false, IDS_TREE_MILO, 972 },
	{ false, IDS_RVINE1, 973 },
	{ false, IDS_RVINE2, 974 },
	{ false, IDS_TORCH, 975 },
	{ false, IDS_DOL_CHAN, 980 },
	{ false, IDS_DOL_DECO, 981 },
	{ false, IDS_TORCH, 982 },
	{ false, IDS_TOMBSTONE, 987 },
	{ false, IDS_COPPER_STATUE, 988 },
	{ false, IDS_COPPER_STATUE, 989 },
	{ false, IDS_AOE_FLAG, 990 },
	{ false, IDS_DOL_FLAG, 991 },
	{ false, IDS_GOE_FLAG, 992 },
	{ false, IDS_HC_FLAG, 993 },
	{ false, IDS_KOES_FLAG, 994 },
	{ false, IDS_OOSM_FLAG, 995 },
	{ false, IDS_POR_FLAG, 996 },
	{ false, IDS_UOC_FLAG, 997 },
	{ false, IDS_AOE_SYM, 998 },
	{ false, IDS_HC_SYM, 999 },
	{ false, IDS_UOC_SYM, 1000 },
	{ false, IDS_DOL_SYM, 1001 },
	{ false, IDS_GOE_SYM, 1002 },
	{ false, IDS_KOES_SYM, 1003 },
	{ false, IDS_POR_SYM, 1004 },
	{ false, IDS_OOSM_SYM, 1005 },
	{ false, IDS_BANISH, 1006 },
	{ false, IDS_DIAMOND, 1007 },
	{ false, IDS_INVIS, 1008 },
	{ false, IDS_PEARL, 1009 },
	{ false, IDS_STAR, 1010 },

/*	
	{false,_T("dol_flag") , 63},
	{false,_T("hc_flag") , 64},
	{false,_T("aoeflag") , 65},
	{false,_T("dolflag") , 67},
	{false,_T("geflag") , 68},
	{false,_T("hcflag") , 69},
	{false,_T("kesflag") , 70},
	{false,_T("osmflag") , 71},
	{false,_T("ptrflag") , 72},
	{false,_T("ucflag") , 73},
	{false,_T("alliance") , 77},
	{false,_T("calenture") , 78},
	{false,_T("covenant") , 79},
	{false,_T("dreamers") , 80},
	{false,_T("gathering") , 81},
	{false,_T("keepers") , 82},
	{false,_T("protectors") , 83},
	{false,_T("sable_moon") , 84},
	{false,_T("banish") , 88},
	{false,_T("diamond") , 89},
	{false,_T("invis") , 90},
	{false,_T("pearl") , 91},
	{false,_T("star") , 92},
*/
};

unsigned int NumTalismans(void)
{
	return  sizeof(talisman_names)/sizeof(talisman_name_t);
}

TCHAR *TalismanName(int bitmap_id)
{
	LoadString(hInstance, talisman_names[0].name, talisman_message, sizeof(talisman_message));
	for (unsigned int i=0; i<NumTalismans(); i++)
		if (talisman_names[i].bitmap_id == bitmap_id)
			LoadString(hInstance, talisman_names[i].name, talisman_message, sizeof(talisman_message));
	return talisman_message;
}


TCHAR *TalismanNameAt(unsigned int index)
{
	LoadString(hInstance, talisman_names[0].name, talisman_message, sizeof(talisman_message));

	if (index < NumTalismans())
		LoadString(hInstance, talisman_names[index].name, talisman_message, sizeof(talisman_message));

	return talisman_message;

}

int TalismanBitmapAt(unsigned int index)
{
	if (index >= NumTalismans())
		return talisman_names[0].bitmap_id; // not found
	else
		return talisman_names[index].bitmap_id;
}

bool TalismanForgable(unsigned int index)
{
	if (index >= NumTalismans())
		return false;
	else
#ifdef GAMEMASTER
	return true;
#else
	return talisman_names[index].forgable;
#endif
}

//////////////////////////////////////////////////////////////////
// Timed Effects Class

// constructor
cTimedEffects::cTimedEffects(void)
{
	int i = 0;
	for (i=0; i<NUM_TIMED_EFFECTS; i++)
	{
		start_descrip[i] = NULL;
		more_descrip[i] = NULL;
		expire_descrip[i] = NULL;
		actor_flag[i] = 0;
		related_art[i] = Arts::NONE;
		default_duration[i] = 0;
		harmful[i] = false;
	}

	i = LyraEffect::NONE;
	LoadString(hInstance, IDS_NONE, name[i], sizeof(name[i]));
	
	// Now set up effect descriptions, expirations, and corresponding flags

	i = LyraEffect::PLAYER_INVISIBLE;
	LoadString (hInstance, IDS_PLAYER_INVISIBLE_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_INVISIBLE_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_INVISIBLE;
	related_art[i] = Arts::INVISIBILITY;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 3; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_CHAMELED;
	LoadString (hInstance, IDS_PLAYER_CHAMELE_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_CHAMELE_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_CHAMELED;
	related_art[i] = Arts::CHAMELE;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 13; // 1 min
	harmful[i] = false;


	i = LyraEffect::PLAYER_CURSED;
	LoadString (hInstance, IDS_PLAYER_CURSED_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_CURSED_MORE, disp_message, sizeof(disp_message));
	more_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_CURSED_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_CURSED;
	related_art[i] = Arts::CURSE;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 23; // 10 min
	harmful[i] = true;

	i = LyraEffect::PLAYER_BLIND;
	LoadString (hInstance, IDS_PLAYER_BLIND_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_BLIND_MORE, disp_message, sizeof(disp_message));
	more_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_BLIND_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_BLINDED;
	related_art[i] = Arts::BLIND;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 5;
	harmful[i] = true;

	i = LyraEffect::PLAYER_DEAF;
	LoadString (hInstance, IDS_PLAYER_DEAF_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_DEAF_MORE, disp_message, sizeof(disp_message));
	more_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_DEAF_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_DEAFENED;
	related_art[i] = Arts::DEAFEN;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 9; // 30 sec
	harmful[i] = true;


	i = LyraEffect::PLAYER_DRUNK;
	LoadString (hInstance, IDS_PLAYER_DRUNK_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_DRUNK_MORE, disp_message, sizeof(disp_message));
	more_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_NORMAL, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_DRUNK;  
	related_art[i] = Arts::STAGGER;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 3; 
	harmful[i] = true;

	i = LyraEffect::PLAYER_FEAR;
	LoadString (hInstance, IDS_PLAYER_FEAR_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_FEAR_MORE, disp_message, sizeof(disp_message));
	more_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_FEAR_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_SCARED; 
	related_art[i] = Arts::SCARE;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 7; // 15 sec
	harmful[i] = true;

	i = LyraEffect::PLAYER_PARALYZED;
	LoadString (hInstance, IDS_PLAYER_PARALYZE_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_PARALYZE_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_PARALYZED; 
	related_art[i] = Arts::PARALYZE;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 3; // 3 sec
	harmful[i] = true;

	i = LyraEffect::PLAYER_POISONED;
	LoadString (hInstance, IDS_PLAYER_POISON_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_POISON_MORE, disp_message, sizeof(disp_message));
	more_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_POISON_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_POISONED;
	related_art[i] = Arts::POISON;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 13; // 1 min
	harmful[i] = true;

	i = LyraEffect::PLAYER_DETECT_INVISIBLE;
	LoadString (hInstance, IDS_PLAYER_DINVIS_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_DINVIS_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_DETECT_INVIS;
	related_art[i] = Arts::VISION;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 18; // 5 min
	harmful[i] = false;

	i = LyraEffect::PLAYER_PROT_FEAR;
	LoadString (hInstance, IDS_PLAYER_PFEAR_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_PFEAR_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_PROT_FEAR;
	related_art[i] = Arts::RESIST_FEAR;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 18; // 5 min
	harmful[i] = false;

	i = LyraEffect::PLAYER_PROT_CURSE;
	LoadString (hInstance, IDS_PLAYER_PCURSE_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_PCURSE_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_PROT_CURSE;
	related_art[i] = Arts::PROTECTION;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 18; // 5 min
	harmful[i] = false;

	i = LyraEffect::PLAYER_PROT_PARALYSIS;
	LoadString (hInstance, IDS_PLAYER_PPARALYZE_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_PPARALYZE_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_FREE_ACTION;
	related_art[i] = Arts::FREE_ACTION;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 18; // 5 min
	harmful[i] = false;

	i = LyraEffect::PLAYER_MEDITATING;
	LoadString (hInstance, IDS_PLAYER_MEDITATE_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_MEDITATE_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_MEDITATING;	
	related_art[i] = Arts::MEDITATION;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=15; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_TRAIL;
	LoadString (hInstance, IDS_PLAYER_TRAIL_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_TRAIL_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_TRAILING;	
	related_art[i] = Arts::TRAIL;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=23; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_SOULEVOKE;
	LoadString (hInstance, IDS_PLAYER_SOULEVOKE_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_SOULEVOKE_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_SOULEVOKE;	
	related_art[i] = Arts::SOULEVOKE;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=23; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_BLENDED;
	LoadString (hInstance, IDS_PLAYER_BLENDED_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_BLENDED_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_BLENDED;	
	related_art[i] = Arts::BLEND;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=6; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_RECALL;
	LoadString (hInstance, IDS_PLAYER_RECALL_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_RECALL_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_RECALL;	
	related_art[i] = Arts::RECALL;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=25; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_RETURN;
	LoadString (hInstance, IDS_PLAYER_RETURN_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_RETURN_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_RETURN;	
	related_art[i] = Arts::RETURN;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=23; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_TRANSFORMED;
	LoadString (hInstance, IDS_TRANSFORM_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_TRANSFORM_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_TRANSFORMED;	
	related_art[i] = Arts::NIGHTMARE_FORM;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=6; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_MIND_BLANKED;
	LoadString (hInstance, IDS_MIND_BLANK_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_MIND_BLANK_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_MIND_BLANKED;	
	related_art[i] = Arts::MIND_BLANK;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=18; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_REGENERATING;
	LoadString (hInstance, IDS_REGENERATE_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_REGENERATE_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_REGENERATING;	
	related_art[i] = Arts::NONE;
	LoadString(hInstance, IDS_REGENERATE, name[i], sizeof(name[i]));
	default_duration[i]=9; 
	harmful[i] = false;


	i = LyraEffect::PLAYER_SOUL_SHIELD;
	LoadString (hInstance, IDS_SOUL_SHIELD_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_SOUL_SHIELD_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_SOUL_SHIELDED;	
	related_art[i] = Arts::SOUL_SHIELD;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=18; 
	harmful[i] = false;

	i = LyraEffect::PLAYER_REFLECT;
	LoadString (hInstance, IDS_REFLECT_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_REFLECT_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_REFLECT;
	related_art[i] = Arts::REFLECT;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=9; // 30 sec
	harmful[i] = false;

	i = LyraEffect::PLAYER_NO_POISON;
	LoadString (hInstance, IDS_NO_POISON_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_NO_POISON_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_NO_POISON;
	related_art[i] = Arts::SABLE_SHIELD;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=12; // 45 sec
	harmful[i] = false;


	i = LyraEffect::PLAYER_PEACE_AURA;
	LoadString (hInstance, IDS_PEACE_AURA_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PEACE_AURA_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_PEACE_AURA;
	related_art[i] = Arts::PEACE_AURA;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=9; // 30 sec
	harmful[i] = false;


	i = LyraEffect::PLAYER_NO_PARTY;
	LoadString (hInstance, IDS_NO_PARTY_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_NO_PARTY_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_NO_PARTY;
	related_art[i] = Arts::BREAK_COVENANT;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i]=9; // 30 sec
	harmful[i] = false;

  i = LyraEffect::PLAYER_SPIN;
	LoadString (hInstance, IDS_PLAYER_SPIN_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_SPIN_MORE, disp_message, sizeof(disp_message));
	more_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_SPIN_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_SPIN; 
	related_art[i] = Arts::CHAOTIC_VORTEX;
	_tcscpy(name[i], arts->Descrip(related_art[i]));
	default_duration[i] = 3; // 15 sec
	harmful[i] = true;

  i = LyraEffect::PLAYER_BLEED;
	LoadString (hInstance, IDS_PLAYER_BLEED_ON, disp_message, sizeof(disp_message));
	start_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_BLEED_MORE, disp_message, sizeof(disp_message));
	more_descrip[i] = _tcsdup(disp_message);
	LoadString (hInstance, IDS_PLAYER_BLEED_OFF, disp_message, sizeof(disp_message));
	expire_descrip[i] = _tcsdup(disp_message);
	actor_flag[i] = ACTOR_BLEED;
	related_art[i] = Arts::RAZORWIND;
	LoadString(hInstance, IDS_BLEED, name[i], sizeof(name[i]));
	default_duration[i] = 7; // 15 seconds
	harmful[i] = true;

	return;
}

// destructor
cTimedEffects::~cTimedEffects(void)
{
	for (int i=0; i<NUM_TIMED_EFFECTS; i++)
	{
		if (start_descrip[i])
			free(start_descrip[i]);
		if (more_descrip[i])
			free(more_descrip[i]);
		if (expire_descrip[i])
			free(expire_descrip[i]);
	}
}


bool LegalRoom(int roomnum)
{
	if ((roomnum > 0) && (roomnum <= Lyra::MAX_LEVELROOMS))
		return TRUE;
	else
		return FALSE;
}


int WhichMonsterName(TCHAR *name)
{
	TCHAR temp_buffer[Avatars::MAX_AVATAR_TYPE+1][DEFAULT_MESSAGE_SIZE];
	LoadString(hInstance, IDS_EMPHANT, temp_buffer[Avatars::EMPHANT], DEFAULT_MESSAGE_SIZE);
	LoadString(hInstance, IDS_BOGROM , temp_buffer[Avatars::BOGROM], DEFAULT_MESSAGE_SIZE);
	LoadString(hInstance, IDS_AGOKNIGHT, temp_buffer[Avatars::AGOKNIGHT], DEFAULT_MESSAGE_SIZE);
	LoadString(hInstance, IDS_SHAMBLIX, temp_buffer[Avatars::SHAMBLIX], DEFAULT_MESSAGE_SIZE);
	LoadString(hInstance, IDS_HORRON, temp_buffer[Avatars::HORRON],DEFAULT_MESSAGE_SIZE);

	for (int i=Avatars::MIN_NIGHTMARE_TYPE; i<=Avatars::MAX_AVATAR_TYPE; i++)
	{
		if (_tcsicmp(name, temp_buffer[i]) == 0) 
			return i;
	}
	
	return FALSE;

}
// Called when new and malloc fail...

int __cdecl OutOfMemory(size_t size)
{
	TCHAR buffer[128];
	LoadString(hInstance, IDS_NO_MEMORY, temp_message, DEFAULT_MESSAGE_SIZE);
	_stprintf(buffer, temp_message, size);
	GAME_ERROR(buffer);
	return 0;
}


// note: update these yearly!
const int DT_START_MONTH = 3;
const int DT_END_MONTH = 11;
const int DT_START_DAY = 13;
const int DT_END_DAY = 1;
const int DT_START_HOUR = 2;
const int DT_END_HOUR = 2;

void GetDSTTime(LPSYSTEMTIME dsttime)
{

		SYSTEMTIME uttime;
	//	TIME_ZONE_INFORMATION curr_zone;
		ULARGE_INTEGER filetime_one_hour;
		filetime_one_hour.QuadPart = 10000000; // filetime is in 100's of ns
		filetime_one_hour.QuadPart = filetime_one_hour.QuadPart*3600;
		GetSystemTime(&uttime);

		FILETIME filetime;
		SystemTimeToFileTime(&uttime, &filetime);
		ULARGE_INTEGER currfiletime;
		currfiletime.LowPart =  filetime.dwLowDateTime;
		currfiletime.HighPart = filetime.dwHighDateTime;

		// DST is 8 hours behind UTC
		currfiletime.QuadPart -= 8*filetime_one_hour.QuadPart; 

		// if we're in daylight savings, add an hour back
		bool daylight_savings = false;
		// may-september is always daylight savings
		if ((uttime.wMonth > DT_START_MONTH) && (uttime.wMonth < DT_END_MONTH))
			daylight_savings = true;
		else if (DT_END_MONTH == uttime.wMonth)
		{
			if (uttime.wDay > DT_END_DAY) 
				daylight_savings = false;
			else if (uttime.wDay < DT_END_DAY)
				daylight_savings = true;
			else if (uttime.wHour < DT_END_HOUR)
				daylight_savings = true;
		}
		else if (DT_START_MONTH == uttime.wMonth)
		{
			if (uttime.wDay > DT_START_DAY) 
				daylight_savings = true;
			else if (uttime.wDay < DT_START_DAY)
				daylight_savings = false;
			else if (uttime.wHour > DT_START_HOUR)
				daylight_savings = true;
		}

		if (daylight_savings)
			currfiletime.QuadPart += filetime_one_hour.QuadPart; 

		filetime.dwHighDateTime = currfiletime.HighPart;
		filetime.dwLowDateTime = currfiletime.LowPart;
		FileTimeToSystemTime(&filetime, dsttime);
									
		return;

}

void Scream(int avatar_type, cActor* actor, bool propagate)
{
	int r;
	switch (avatar_type) {
	case Avatars::FEMALE:
		r = rand()%4;
		if (r == 0)
			cDS->PlaySound(LyraSound::FEMALE_SCREAM1, actor->x, actor->y, propagate);
		else if (r == 1)
			cDS->PlaySound(LyraSound::FEMALE_SCREAM2, actor->x, actor->y, propagate);
		else if (r == 2)
			cDS->PlaySound(LyraSound::FEMALE_SCREAM3, actor->x, actor->y, propagate);
		else
			cDS->PlaySound(LyraSound::FEMALE_SCREAM4, actor->x, actor->y, propagate);
		break;
	case Avatars::MALE:
		r = rand()%3;
		if (r == 0)
			cDS->PlaySound(LyraSound::MALE_SCREAM1, actor->x, actor->y, propagate);
		else if (r == 1)
			cDS->PlaySound(LyraSound::MALE_SCREAM2, actor->x, actor->y, propagate);
		else
			cDS->PlaySound(LyraSound::MALE_SCREAM3, actor->x, actor->y, propagate);
		break;
		
	case Avatars::EMPHANT:
	case Avatars::BOGROM:
	case Avatars::AGOKNIGHT:
		cDS->PlaySound(LyraSound::EMPHANT_ROAR, actor->x, actor->y, propagate);
		break;
	case Avatars::SHAMBLIX:
	case Avatars::HORRON:
		cDS->PlaySound(LyraSound::MONSTER_ROAR, actor->x, actor->y, propagate);
		break;
	}
	
}

void PrepareSrcRect(RECT* src, RECT* region, int stretch)
 {
	if ((stretch == NOSTRETCH) || (cDD->Res() == 0))
	{
		src->left = region->left;
		src->right = region->right;
		src->bottom = region->bottom;
		src->top = region->top;
	} 
	else if (cDD->Res() == 1) 
	{
		src->left = (int)(region->left*4/5);
		src->right = (int)(region->right*4/5);
		src->bottom = (int)(region->bottom*4/5);
		src->top = (int)(region->top*4/5);
	} 
	else if (cDD->Res() == 2)
	{
		src->left = (int)(region->left*5/8);
		src->right = (int)(region->right*5/8);
		src->bottom = (int)(region->bottom*5/8);
		src->top = (int)(region->top*5/8);
	}
	return;
}


int MaxSkill(int art_id)
{
  // find entry for art
  // initial max is same as player's orbit
  int the_max = player->Orbit();

  // if the art has no focus stat, then the limit is 99 for anyone
  if ((arts->Stat(art_id) == Stats::NO_STAT) ||
      (arts->Stat(art_id) == Stats::DREAMSOUL)) {
    the_max = MIN(the_max, 99);
    return the_max;
  }

  // if player's focus stat matches the art's focus stat, no limit (99)
  if (player->FocusStat() == arts->Stat(art_id)) {
    the_max = MIN(the_max, 99);
    return the_max;
  }

  // otherwise, focus stats do not match, so limit is below 3rd plateau (29),
  // and for non-focus arts can be learned only to 2 spheres less than
  // the player's sphere

  // player's orbit high enough?  (must be 2 spheres above art's sphere)
  int sphere = player->Sphere();
  int min_sphere = arts->MinOrbit(art_id)/10;
  min_sphere += 2;
  int sphere_diff = sphere - min_sphere;
  if (sphere_diff < 0)
    sphere_diff = 0;
  int art_max = sphere_diff*10 + 9;

  the_max = MIN(the_max, art_max);

  the_max = MIN(the_max, 29);

  return the_max;
}
