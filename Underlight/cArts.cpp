// Handles the use of Arts and Glamours

// Copyright Lyra LLC, 1997. All rights reserved.

#define STRICT

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "4dx.h"
#include "cDDraw.h"
#include "cChat.h"
#include "cDSound.h"
#include "cNeighbor.h"
#include "cEffects.h"
#include "cPlayer.h"
#include "cGameServer.h"
#include "cParty.h"
#include "cLevel.h"
#include "cActorList.h"
#include "Dialogs.h"
#include "Interface.h"
#include "Move.h"
#include "Math.h"
#include "cControlPanel.h"
#include "Options.h"
#include "Resource.h"
#include "Realm.h"
#include "Utils.h"
#include "RmRemotePlayer.h"
#include "cArts.h"
//#include "RogerWilco.h"
#include "interface.h"
#ifdef AGENT
#include "cAI.h"
#endif

//////////////////////////////////////////////////////////////////
// External Global Variables

extern HINSTANCE hInstance;
extern cPlayer *player;
extern cControlPanel *cp;
extern cEffects *effects;
extern cDDraw *cDD;
extern cChat *display;
extern cDSound *cDS;
extern cActorList *actors;
extern cGameServer *gs;
extern cTimedEffects *timed_effects;
extern options_t options;
extern bool chooseguilddlg;
extern bool entervaluedlg;
extern bool acceptrejectdlg;
extern bool locateavatardlg;
extern bool writescrolldlg;
extern bool itemdlg;
extern bool ready;
extern bool useppointdlg;
extern bool grantppointdlg;

extern bool exiting;
extern bool killed;
extern cArts *arts;
extern art_dlg_callback_t acceptreject_callback;
extern art_dlg_callback_t entervalue_callback;
extern art_dlg_callback_t chooseguild_callback;
extern art_dlg_callback_t grantpp_callback;

extern ppoint_t pp; // personality points use tracker


extern void* voicePipeline;
extern void* channel;

//////////////////////////////////////////////////////////////////
// Constants

const int CHANCE_SPELL_FAILURE = 1; // % chance of art failure

const float PUSH_DISTANCE = 75.0f;

#if defined (UL_DEBUG) && !defined (LIVE_DEBUG)
const int CHANCE_SKILL_INCREASE = 66;
#else
const int CHANCE_SKILL_INCREASE = 15; // % chance of skill increase
#endif

const int CASTING_TIME_MULTIPLIER = 150; // milliseconds per unit of casting time
const int MIN_DS_SOULEVOKE = 10;

unsigned long art_chksum[NUM_ARTS] =
{
0x0970, // Join Party 
0x2D10, // GateKeeper 
0x504E, // DreamSeer 
0x7390, // SoulMaster 
0x9FE9, // FateSender 
0xC2B4, // Random 
0xD931, // Meditation 
0x05DB, // Resist Fear 
0x2216, // Protection 
0x4E90, // Free Action 
0x6E00, // Ward 
0x91A8, // Amulet
#ifndef PMARE
0xBABF, // Shatter 
#else
0xB5C0, // Shatter - pmare 
#endif
0xDEFA, // Return 
0x00E9, // Know 
0x26CD, // Judgment 
0x4999, // Identify 
0x6B91, // Identify Curse 
0x9125, // Chamele 
0xB581, // Vision 
0xDDA6, // Blast 
0x0106, // Blend 
0x263E, // Forge Talisman 
0x4937, // Recharge 
0x6C73, // Restore 
0x919D, // Reweave 
0xB501, // Purify 
0xDFAC, // Drain Self 
0x0281, // Abjure 
0x274B, // Poison 
0x4A46, // Antidote 
0x6C08, // Curse 
0x953C, // Drain Essence 
0xB0F4, // Banish Nightmare 
0xDC59, // Imprison Nightmare 
0xFAE1, // Trap Nightmare 
0x2191, // Dreamblade 
0x48FC, // Trail 
0x6C9B, // Scare 
0x90F5, // Stagger 
0xB40F, // Deafen 
0xDAC5, // Blind 
0xFC86, // Darkness 
0x20A3, // Paralyze 
0x4B10, // Firestorm 
0x6ED5, // Razorwind 
0x8E9A, // Recall 
0xB0A3, // Push 
0xD632, // Soul Evoke 
0xFDCE, // Dream Strike 
0x1BE2, // Nightmare Form 
0x479C, // Locate Dreamer 
0x6E50, // Train 
0x979A, // Initiate 
0xBA93, // Knight 
0xDBBF, // Support Ascension 
0x04AB, // Ascend to Ruler 
0x2395, // Instant Collapse 
0x48C6, // Grant XP 
0x6DF2, // Terminate 
0x9097, // Sphere 
0xB1D9, // Support Demotion 
0xD980, // Demote 
0xFECE, // Invisibility 
0x248B, // Give 
0x4501, // GateSmasher 
0x6B4E, // FateSlayer 
0x8825, // SoulReaper 
0xAD00, // FlameShaft 
0xD006, // TranceFlame 
0xFADD, // FlameSear 
0x1C95, // FlameRuin 
0x3C59, // Inscribe 
0x65AA, // Destroy Talisman 
0x8BEF, // Mind Blank 
0xB179, // Show Talisman 
0xDB0B, // Awaken 
0xFF24, // UnTrain 
0x21A8, // Grant RP XP 
0x3BDB, // Dreamquake 
0x6222, // Hypnotic Weave 
0x8E57, // Vampiric Draw 
0xADC8, // Terror 
0xD7EE, // Healing Aura 
0xFF5F, // Telepathy 
0x24FE, // Dreamsmith Mark 
0x435B, // Support Train 
0x6F37, // Support Sphere 
0x8D20, // Train Self 
0xAE98, // Soul Shield 
0xD56D, // Summon 
0xF999, // Suspend 
0x1BC3, // Reflect 
0x3DFD, // Sacrifice 
0x659F, // Cleanse Nightmare 
0x8149, // Create ID Token 
0xAC49, // Sense Dreamers 
0xCCD3, // Expel 
0xF6A4, // Newly Awakened 
0x1A4A, // Combine 
0x3F28, // Power Token 
0x6D27, // Show Gratitude 
0x8E44, // Quest 
0xB230, // Bequeath 
0xC88A, // Radiant Blaze 
0xF445, // Poison Cloud 
0x106E, // Break Covenant 
0x3A08, // Peace Aura 
0x5A62, // Sable Shield 
0x8684, // Entrancement 
0xA2E4, // Shadow Step 
0xC950, // Dazzle 
0xFD03, // Guild House 
0x131D, // Corrupt Essence 
0x3E30, // Tehthu's Oblivion 
0x58A2, // Chaos Purge 
0x8C82, // Wordsmith Mark 
0xAA1F, // Cup Summons 
0xCD70, // House Members 
0xEB02, // Freesoul Blade 
0x14D8, // Illuminated Blade 
0x332E, // Summon Prime 
0x665F, // Reward 
0x803A, // Scan 
0xA5E2, // Passlock 
0xCAFE, // Heal 
0xED56, // Sanctify 
0x1282, // Lock 
0x3614, // Key 
0x586F, // Break Lock 
0x8380, // Repair 
0xA29F, // Remove Curse 
0xCFED, // Hold Avatar 
0xE842, // Sanctuary 
0x1237, // Shove 
0x37E2, // Inscribe 
0x664D, // Forge Master 
0x8544, // Merge Talisman 
0xA9D8, // NP Symbol 
0xCC58, // Sense Datoken 
0xED06, // Tempest 
0x123A, // Kinesis 
0x3066, // Misdirection 
0x5D3E, // Chaotic Vortex 
0x7EE1, // Chaos Well 
0xA08F, // Rally 
};

art_t art_info[NUM_ARTS] = // 		  			    Evoke
{// CKS  Name				Stat			    Orb Drn Dur Time PP 	Flags
{IDS_JP,					Stats::NO_STAT,		0,  0,  0,	0,	-1, SANCT|NEIGH|LEARN},
{IDS_GK, 					Stats::NO_STAT,		0,  0,  0,	0, 	-1, FOCUS},
{IDS_DREAMSEER,				Stats::NO_STAT,		0,  0,  0,	0,	-1, FOCUS},
{IDS_SM, 					Stats::NO_STAT,		0,  0,  0,	0,	-1, FOCUS},
{IDS_FS, 					Stats::NO_STAT,		0,  0,  0,	0, 	-1, FOCUS},
{IDS_RANDOM,				Stats::NO_STAT,		0,  0,  0,	2, 	-1, SANCT|LEARN},
{IDS_MEDITATION, 			Stats::WILLPOWER,	0,  2,  15, 1,	1, SANCT|LEARN},
{IDS_RF,					Stats::WILLPOWER,	10, 5,  13, 2, 	2, SANCT|LEARN},
{IDS_PROTECTION, 			Stats::WILLPOWER,	15, 5,  13, 2, 	2, SANCT|LEARN},
{IDS_FA,					Stats::WILLPOWER,	5,  5,  13, 2, 	2, SANCT|LEARN},
{IDS_WARD, 					Stats::WILLPOWER,	20, 20, 0,	5, 	3, MAKE_ITEM|FOCUS|LEARN},
{IDS_AMULET,				Stats::WILLPOWER,	20, 1,  0,	2, 	1, SANCT|MAKE_ITEM|FOCUS|LEARN},
#ifndef PMARE
{IDS_SHATTER, 				Stats::WILLPOWER,	40, 40, 0,	8, 	4, SANCT|FOCUS|LEARN},
#else
{IDS_SHATTER, 				Stats::DREAMSOUL,	0, 15, 0,	8, 	4, SANCT|LEARN},
#endif
{IDS_RETURN,				Stats::WILLPOWER, 50, 20, 23,   2,  2, SANCT|FOCUS|LEARN},
{IDS_KNOW, 					Stats::INSIGHT,		0,  0,  0,	1, 	-1, SANCT|LEARN},
{IDS_JUDGEMENT_ART_NAME,	Stats::INSIGHT,		10, 2,  0,	1, 	1, SANCT|NEIGH|LEARN},
{IDS_ID,					Stats::INSIGHT,		15, 6,  0,	5, 	1, SANCT|NEED_ITEM|LEARN},
{IDS_ID_CURSE,				Stats::INSIGHT,		20, 2,  0,	1, 	1, SANCT|NEIGH|LEARN},
{IDS_CHAMELE, 				Stats::INSIGHT,		20, 20, 16, 5, 	2, SANCT|FOCUS|LEARN},
{IDS_VISION,				Stats::INSIGHT,		20, 5,  13, 2, 	2, SANCT|FOCUS|LEARN},
{IDS_BLAST,					Stats::INSIGHT,		30, 2,  0,	1, 	2, NEIGH|FOCUS|LEARN},
{IDS_BLEND,					Stats::INSIGHT,		50, 30, 6,	5, 	3, SANCT|FOCUS|LEARN},
{IDS_FORGE,					Stats::DREAMSOUL,	50, 50, 0,	8, 	-1, SANCT|MAKE_ITEM|LEARN},
{IDS_RECHARGE,				Stats::INSIGHT,		40, 15, 0,	8, 	2, SANCT|FOCUS|LEARN|NEED_ITEM},
{IDS_RESTORE, 				Stats::RESILIENCE,	10, 5,  0,	1, 	1, SANCT|LEARN},
{IDS_REWEAVE, 				Stats::RESILIENCE,	15, 10, 0,	4, 	1, SANCT|NEED_ITEM|LEARN},
{IDS_PURIFY,				Stats::RESILIENCE,	5,  15, 0,	2, 	1, SANCT|LEARN},
{IDS_DRAIN_SELF, 			Stats::RESILIENCE,	20, 5,  0,	2, 	2, SANCT|NEIGH|FOCUS|LEARN},
{IDS_ABJURE,				Stats::RESILIENCE,	50, 30, 0,	3, 	4, FOCUS|LEARN},
{IDS_POISON,				Stats::RESILIENCE,	30, 15, 13, 3, 	2, NEIGH|FOCUS|LEARN},
{IDS_ANTIDOTE,				Stats::RESILIENCE,	30, 10, 0,	2, 	3, SANCT|FOCUS|LEARN},
{IDS_CURSE,					Stats::RESILIENCE,	40, 10, 13, 3, 	3, NEIGH|FOCUS|LEARN},
{IDS_DRAIN_ESSENCE,			Stats::RESILIENCE,	0,  0,  0,	1, 	1, SANCT|NEED_ITEM|LEARN},
{IDS_BANISH_MARE,			Stats::RESILIENCE,	50, 5,  0,	1, 	1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS|LEARN},
{IDS_IMPRISON_MARE,			Stats::RESILIENCE,	50, 5,  0,	1, 	1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS|LEARN},
{IDS_TRAP_MARE,				Stats::RESILIENCE,	50, 10, 0,	3, 	3, SANCT|NEIGH|FOCUS|LEARN},
{IDS_DREAMBLADE, 			Stats::INSIGHT,		0,  5,  23, 1, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_TRAIL,					Stats::LUCIDITY,	0,  2,  25, 1, 	-1, SANCT|LEARN},
{IDS_SCARE,					Stats::LUCIDITY,	10, 5,  4,	2, 	2, NEIGH|LEARN},
{IDS_STAGGER, 				Stats::LUCIDITY,	40, 10, 3,	2, 	3, NEIGH|FOCUS|LEARN},
{IDS_DEAFEN,				Stats::LUCIDITY,	5,  15, 4,	2, 	2, NEIGH|LEARN},
{IDS_BLIND,					Stats::LUCIDITY,	45, 15, 3,	3, 	3, NEIGH|FOCUS|LEARN},
{IDS_DARKNESS_ART_NAME,		Stats::LUCIDITY,	50, 25, 4,	5, 	3, NEIGH|FOCUS|LEARN},
{IDS_PARALYZE,				Stats::LUCIDITY,	30, 20, 2,	3, 	3, NEIGH|FOCUS|LEARN},  
{IDS_FIRESTORM,				Stats::LUCIDITY,	50, 25, 0,	7, 	3, FOCUS|LEARN},
{IDS_RAZORWIND,				Stats::LUCIDITY,	70, 40, 6,	9, 	4, FOCUS|LEARN},
{IDS_RECALL_ART_NAME,		Stats::DREAMSOUL,	25, 1,  25, 1, 	1, SANCT|LEARN},
{IDS_PUSH, 					Stats::DREAMSOUL,	0,  0,  0,	1, 	1, NEIGH|LEARN},
{IDS_SOUL_EVOKE, 			Stats::DREAMSOUL,	15, 1,  23, 1, 	1, SANCT|LEARN},
{IDS_DREAM_STRIKE,			Stats::DREAMSOUL,	10, 30,  0,	5, 	-1, NEIGH},
{IDS_NMF,					Stats::DREAMSOUL,	40, 10, 13, 5, 	1, SANCT|LEARN},
{IDS_LOCATE,				Stats::DREAMSOUL,	0,  1,  0,	1, 	-1, SANCT|LEARN},
{IDS_TRAIN,					Stats::NO_STAT,		10, 0,  0,	2, 	-1, SANCT|NEIGH},
{IDS_INITIATE,				Stats::NO_STAT,		0,  0,  0,	10,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_KNIGHT,				Stats::NO_STAT,		0,  0,  0,	10,	-1, SANCT|NEIGH},
{IDS_SUPPORT_ASCENSION,		Stats::NO_STAT,		0,  0,  0,	10,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_ASCEND_RULER,			Stats::NO_STAT,		0,  0,  0,	10,	-1, SANCT|NEED_ITEM},
{IDS_INSTA_COLLAPSE,		Stats::NO_STAT,		0,  0,  0,	1, 	-1, SANCT|NEIGH},		// gm only
{IDS_GRANT_XP_ART_NAME,		Stats::NO_STAT,		0,  0,  0,	1, 	-1, SANCT},				// gm only
{IDS_TERMINATE,				Stats::NO_STAT,		0,  0,  0,	0, 	-1, SANCT},		
{IDS_SPHERE,				Stats::NO_STAT,		20, 0,  0,	1, 	-1, SANCT|NEIGH},
{IDS_SUPPORT_DEMOTION,		Stats::NO_STAT,		0,  0,  0,	3,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_DEMOTE,				Stats::NO_STAT,		0,  0,  0,	3,	-1, SANCT|NEIGH|NEED_ITEM},
{IDS_INVISIBILITY,			Stats::INSIGHT,		40, 20, 6,	3, 	3, SANCT|FOCUS|LEARN},
{IDS_GIVE, 					Stats::NO_STAT,		0,  0,  0,	0, 	-1, SANCT|NEIGH|NEED_ITEM|LEARN},
{IDS_GATESMASHER,			Stats::WILLPOWER,	0,  5,  23, 1, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_FATESLAYER, 			Stats::LUCIDITY,	0,  5,  23, 1, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_SOULREAPER, 			Stats::RESILIENCE,	0,  5,  23, 1, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_FLAMESHAFT, 			Stats::WILLPOWER,	1,  2,  0,	1, 	2, FOCUS|LEARN},
{IDS_TRANCEFLAME,			Stats::INSIGHT,		1,  2,  0,	1, 	2, FOCUS|LEARN},
{IDS_FLAMESEAR,				Stats::RESILIENCE,	1,  2,  0,	1, 	2, FOCUS|LEARN},
{IDS_FLAMERUIN,				Stats::LUCIDITY,	1,  2,  0,	1, 	2, FOCUS|LEARN},
{IDS_INSCRIBE,				Stats::DREAMSOUL,	20, 2,  0,	1, 	1, SANCT|MAKE_ITEM|LEARN},
{IDS_DT,					Stats::DREAMSOUL,	35, 5,	0,	2,	1, SANCT|NEED_ITEM|LEARN}, 
{IDS_MB,					Stats::DREAMSOUL,	30, 5, 18,	5,	2, SANCT|LEARN},
{IDS_SHOW,					Stats::NO_STAT, 	0,	0,	0,	0,	-1, SANCT|NEIGH|NEED_ITEM|LEARN},
{IDS_AWAKEN,				Stats::NO_STAT, 	0,	0,	0,	0,	-1, SANCT|NEIGH},		// gm only
{IDS_UNTRAIN_ART_NAME, 		Stats::NO_STAT, 	0,	0,	0,	2,	-1, SANCT|NEIGH},		// gm only
{IDS_GRANT_RP_XP, 			Stats::NO_STAT, 	0,	0,	0,	0,	-1, SANCT|NEIGH},
{IDS_DREAMQUAKE,			Stats::WILLPOWER,	60, 40, 4,	7,	3, FOCUS|LEARN}, 
{IDS_HYPNOTIC_WEAVE_ART_NAME,		Stats::LUCIDITY,	60, 40, 4,	7,	4, FOCUS|LEARN},
{IDS_VAMPRIC_DRAW,					Stats::RESILIENCE,	60, 5,	0,	5,	3, FOCUS|LEARN|NEIGH|NEED_ITEM},
{IDS_TERROR_ART_NAME,				Stats::LUCIDITY,	60, 40, 4,	1,	3, FOCUS|LEARN},
{IDS_HEAL_AURA,						Stats::RESILIENCE,	60, 40, 0,	7, 	-1, FOCUS|NEIGH|SANCT|LEARN},
{IDS_TELEPATHY,						Stats::NO_STAT,		75, 0,  0,	0, 	-1, SANCT|NEIGH},	// roger wilco voice
{IDS_DREAMSMITH_MARK_ART_NAME, 		Stats::NO_STAT,		50, 0,  0,	1, 	-1, SANCT},
{IDS_SUPPORT_TRAIN,					Stats::NO_STAT,		10, 0,  0,	10,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_SUPPORT_SPHERE,				Stats::NO_STAT,		20, 0,  0,	10,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_TRAIN_SELF,					Stats::NO_STAT,		40, 0,  0,	2, 	-1, SANCT},
{IDS_SOUL_SHIELD,					Stats::DREAMSOUL,	40, 20, 15, 5,  2, SANCT|LEARN},
{IDS_SUMMON_ART_NAME,				Stats::NO_STAT,		0,  0,  0,  0,  -1, SANCT},
{IDS_SUSPEND,						Stats::NO_STAT,		0,  0,  0,	0, 	-1, SANCT},
{IDS_REFLECT_ART_NAME,				Stats::WILLPOWER,   65, 40, 9,  3,	-1, SANCT|FOCUS|LEARN},
{IDS_SACRIFICE,						Stats::RESILIENCE,	10, 5,  0,	1, 	-1, SANCT|NEED_ITEM},
{IDS_CLEANSE_MARE,					Stats::RESILIENCE,	50, 5,  0,	1, 	1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS|LEARN},
{IDS_CREATE_ID_TOKEN,				Stats::DREAMSOUL,	0,  20, 0,	1, 	-1, SANCT|NEED_ITEM|MAKE_ITEM},
{IDS_SENSE,							Stats::DREAMSOUL,	0,  0,  0,	1,  -1, SANCT|LEARN},
{IDS_EXPEL_ART_NAME,				Stats::DREAMSOUL,	20, 0,  0,	1,  -1, SANCT|NEED_ITEM|NEIGH},
{IDS_NEWLY_AWAKENED,				Stats::INSIGHT,	    0,  1,  0,	1, 	-1, SANCT|LEARN},
{IDS_COMBINE,						Stats::INSIGHT,	    60, 40, 0,	2, 	-1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS|LEARN},
{IDS_POWER_TOKEN,					Stats::DREAMSOUL,	10,  0, 0,	10, -1, SANCT|NEED_ITEM|MAKE_ITEM},
{IDS_SHOW_GRATITUDE,				Stats::NO_STAT,		0,   0, 0,	10, -1, SANCT|NEED_ITEM|NEIGH},
{IDS_QUEST,							Stats::NO_STAT,		0,   0, 0,	3, -1, SANCT|NEIGH|MAKE_ITEM},
{IDS_BEQUEATH,						Stats::NO_STAT,		0,   0, 0,	10, -1, SANCT|NEED_ITEM|NEIGH},
{IDS_RADIANT_BLAZE,					Stats::DREAMSOUL,	20, 10, 9,	5,  -1, NEED_ITEM|NEIGH},
{IDS_POISON_CLOUD,					Stats::DREAMSOUL,	20, 10,15,	5,  -1, NEED_ITEM|NEIGH},
{IDS_BREAK_COVENANT,				Stats::DREAMSOUL,	20, 10, 9,	5,  -1, NEED_ITEM|NEIGH},
{IDS_PEACE_AURA_ART_NAME,			Stats::DREAMSOUL,	20, 10, 6,	5,  -1, NEED_ITEM|SANCT},
{IDS_SABLE_SHIELD,					Stats::DREAMSOUL,	20, 10,16,	5,  -1, NEED_ITEM|SANCT},
{IDS_ENTRANCEMENT,					Stats::DREAMSOUL,	20, 10,13,	5,  -1, NEED_ITEM|SANCT},
{IDS_SHADOW_STEP,					Stats::DREAMSOUL,	20, 10,10,	5,  -1, NEED_ITEM|SANCT},
{IDS_DAZZLE,						Stats::DREAMSOUL,	20, 10, 9,	5,  -1, NEED_ITEM|NEIGH},
{IDS_GUILD_HOUSE,					Stats::NO_STAT	,	50,  0, 0,  13, -1, SANCT},
{IDS_CORRUPT_ESSENCE,				Stats::RESILIENCE,	10,  5, 0,  1,  -1, NEED_ITEM|SANCT|MAKE_ITEM},
{IDS_TEHTHUS_OBLIVION,				Stats::DREAMSOUL,	10, 10, 0,  5,  -1, NEED_ITEM|SANCT},
{IDS_CHAOS_PURGE_ART_NAME,			Stats::DREAMSOUL,	 0, 20, 0,  5,  -1, NEIGH},
{IDS_WORDSMITH_MARK_ART_NAME, 		Stats::NO_STAT,		10,  0, 0,	2, 	-1, SANCT|NEIGH},
{IDS_CUP_SUMMONS_ART_NAME,			Stats::NO_STAT,		 0,  0, 0,  0,  -1, SANCT},
{IDS_HOUSE_MEMBERS,					Stats::DREAMSOUL,	 0,  1, 0,	1, 	-1, SANCT},
{IDS_FREESOUL_BLADE,				Stats::DREAMSOUL,	 0,  0, 0,	1, 	-1, SANCT|MAKE_ITEM|NEED_ITEM|NEIGH},
{IDS_ILLUMINATED_BLADE,				Stats::DREAMSOUL,	 0,  0, 0,	1, 	-1, SANCT|MAKE_ITEM|NEED_ITEM|NEIGH},
{IDS_SUMMON_PRIME,				    Stats::DREAMSOUL,	 0, 25, 0,	10,	-1, SANCT|NEED_ITEM|MAKE_ITEM},
{IDS_GRANT_PPOINT,				    Stats::NO_STAT,		 0, 0, 0,	0,	-1, SANCT|NEIGH},
{IDS_SCAN,							Stats::INSIGHT,		10, 2,  0,	1, 	-1, SANCT|NEIGH},
{IDS_PASSLOCK,						Stats::INSIGHT,		50, 30, 6,	5, 	-1, SANCT|FOCUS},
{IDS_HEAL, 							Stats::RESILIENCE,	10, 5,  0,	1, 	-1, SANCT},
{IDS_SANCTIFY, 						Stats::WILLPOWER,	15, 5,  13, 2, 	-1, SANCT},
{IDS_LOCK, 							Stats::WILLPOWER,	20, 20, 0,	5, 	-1, MAKE_ITEM|FOCUS},
{IDS_KEY,							Stats::WILLPOWER,	20, 1,  0,	2, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_BREAK_LOCK, 					Stats::WILLPOWER,	40, 40, 0,	8, 	-1, SANCT|FOCUS},
{IDS_REPAIR, 						Stats::RESILIENCE,	15, 10, 0,	4, 	-1, SANCT|NEED_ITEM},
{IDS_REMOVE_CURSE,					Stats::RESILIENCE,	5,  15, 0,	2, 	-1, SANCT},
{IDS_HOLD_AVATAR,					Stats::LUCIDITY,	30, 20, 1,	3, 	-1, NEIGH|FOCUS},  
{IDS_SANCTUARY,						Stats::DREAMSOUL,	25, 1,  25, 1, 	-1, SANCT},
{IDS_SHOVE, 						Stats::DREAMSOUL,	0,  0,  0,	1, 	-1, NEIGH},
{IDS_INSCRIBE,						Stats::DREAMSOUL,	20, 2,  0,	1, 	-1, SANCT|MAKE_ITEM},
{IDS_FORGE_MASTER,		 			Stats::NO_STAT,		50, 0,  0,	1, 	-1, SANCT},
{IDS_MERGE_TALISMAN,				Stats::INSIGHT,	    60, 40, 0,	2, 	-1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS},
{IDS_NP_SYMBOL_ART_NAME, 			Stats::NO_STAT,		10,  0, 0,	2, 	-1, SANCT},
{IDS_LOCATE_MARES,					Stats::INSIGHT,	    0,  1,  0,	1, 	-1, SANCT|LEARN},
{IDS_TEMPEST,				        Stats::LUCIDITY,	60, 40, 0,	7, 	-1, FOCUS|LEARN},
{IDS_KINESIS, 						Stats::WILLPOWER,	30, 5,  0,	1, 	-1, FOCUS|LEARN|NEIGH},
{IDS_MISDIRECTION,					Stats::DREAMSOUL,   60, 30, 0,  5,  -1, LEARN|NEIGH},
{IDS_CHAOTIC_VORTEX,				Stats::DREAMSOUL,   70, 40, 4,  5,  -1, NEIGH|NEED_ITEM},
{IDS_CHAOS_WELL,					Stats::DREAMSOUL,   30, 5,  0,  5,  -1, SANCT|MAKE_ITEM|LEARN},
{IDS_RALLY,							Stats::WILLPOWER,	60, 30, 0,  5,   4, SANCT|NEIGH|FOCUS},
};


//////////////////////////////////////////////////////////////////
// Helpers

// Constructor
cArts::cArts(void)
{

	waiting_for_sel = waiting_for_dlg = false;
	callback_method = NULL;
	giving_item = receiving_item = combining_item = gratitude_item = quest_item = NO_ACTOR;
	art_in_use = Arts::NONE;
	cp_mode = NO_TAB;
	dummy = NULL;
	active_dlg = NULL;
	art_completion_time = NOT_CASTING;
	rogerwilco_id = 0;
	LoadString(hInstance, IDS_NO_ART, no_art, sizeof(no_art));
  displayed_await_update_art = false;

#ifdef UL_DEBUG
	this->CheckInvariants(__LINE__, _T(__FILE__));
#endif

	return;
}


// Retrieve Art Description
TCHAR* cArts::Descrip(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
		LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
		_stprintf(errbuf, message, art_id);
		INFO(errbuf);
		LoadString(hInstance, IDS_NO_ART, art_name, sizeof(art_name));
		return (TCHAR*)art_name;
	}
	else
 
	//return art_info[art_id].descrip;
	LoadString(hInstance, art_info[art_id].descrip, art_name, sizeof(art_name));
	return art_name;

}

bool cArts::UseInSanctuary(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return false;
	}
	else
		return art_info[art_id].usable_in_sanctuary();
}

bool cArts::Restricted(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return false;
	}
	else
		return art_info[art_id].restricted();
}

short cArts::PPMultiplier(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
		LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
		_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return false;
	}
	else
		return art_info[art_id].pp_multiplier;;
}

int cArts::CanPlateauArt(int art_id) // Returns the next skill level if art can be plateaued
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return 0;
	}
	else
	{
		int skill = player->Skill(art_id);
		if (!skill)
			return 0;
		int the_max = 99;
		// if the art has no focus stat, or matches player's focus,
		// then the limit is 99 or player's orbit for anyone
		if ((art_info[art_id].stat == Stats::NO_STAT) ||
			(art_info[art_id].stat == Stats::DREAMSOUL) ||
			(art_info[art_id].stat == player->FocusStat())) {
			the_max = player->Orbit();
		}

		else {
		// otherwise, focus stats do not match, so limit is below 3rd plateau (29),
		// and for non-focus arts can be learned only to 2 spheres less than
		// the player's sphere

		// player's orbit high enough?  (must be 2 spheres above art's sphere)
		int sphere = player->Sphere();
		int min_sphere = art_info[art_id].min_orbit/10;
			min_sphere += 2;
		int sphere_diff = sphere - min_sphere;
		if (sphere_diff < 0)
			sphere_diff = 0;
		int art_max = sphere_diff*10 + 9;

		the_max = MIN(the_max, art_max);
		the_max = MIN(the_max, 29);
		}

		if ((the_max > skill) && ((skill % 10) == 9))
		{
			return (player->Skill(art_id))+1;
		}

		return 0;
	}
}

bool cArts::Learnable(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return false;
	}
	else if (player->Skill(art_id))
		return false;
	else
	{
		int stat = art_info[art_id].stat;
		if (stat == Stats::DREAMSOUL || stat == Stats::NO_STAT || stat == player->FocusStat())
		{
			// in focus
			return  player->Orbit() >= art_info[art_id].min_orbit;
		}
		else
		{
			// not in focus
			if (art_info[art_id].restricted())
				return false;
			else
				return (player->Orbit() - 20) >= art_info[art_id].min_orbit;
		}
	}
}

bool cArts::DisplayLearnable(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return false;
	}
	else
		return art_info[art_id].display_learnable();
}


// Retrieve Art Stat
int cArts::Stat(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return 0;
	}
	else
		return art_info[art_id].stat;
}

// Retrieve amount of stat drain
int cArts::Drain(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return 0;
	}
	else
		return art_info[art_id].drain;
}

// Retrieve Art Minimum Orbit
int cArts::MinOrbit(int art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return 0;
	}
	else
		return art_info[art_id].min_orbit;
}

// Calculate Duration for Art
int cArts::Duration(int art_id, int skill)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return 0;
	}

	int multiplier;
	if (skill > 0)
		multiplier = (skill/10)+1;
	else
		multiplier =0;

	return (multiplier*CalculateDuration(art_info[art_id].duration));
}



// Attempts skill trial, and if successful calls the method to launch the art
void cArts::BeginArt(int art_id, bool bypass)
{
	if (!this->CanUseArt(art_id, bypass))
		return;

	player->PerformedAction();

	if (player->flags & ACTOR_TRANSFORMED)
	{ // no arts use in nightmare form
		player->NightmareAttack();
		return;
	}

	if (player->flags & ACTOR_MEDITATING) // expire meditation on evoke
		player->RemoveTimedEffect(LyraEffect::PLAYER_MEDITATING);

	if (!bypass && (player->Skill(art_id) == 0) && (art_id != Arts::GRANT_PPOINT))	// no chance of success or improvement when skill=0
	{
		cp->UpdateArt(art_id);
		return;
	}

	if (!art_info[art_id].usable_in_sanctuary() && (player->flags & ACTOR_CHAMELED))
		player->RemoveTimedEffect(LyraEffect::PLAYER_CHAMELED);

	art_in_use = art_id;
	int duration = art_info[art_id].casting_time*CASTING_TIME_MULTIPLIER;
	art_completion_time = LyraTime() + duration*(10 - player->SkillSphere(art_id));

	if (duration)
	{	// no begin message for instantaneous arts
		if (options.art_prompts)
		{
			LoadString (hInstance, IDS_BEGIN_ART, disp_message, sizeof(disp_message));
			_stprintf(message,disp_message,this->Descrip(art_id));
			display->DisplayMessage (message, false);
		}
		player->EvokingFX().Activate(art_id, true);
	}
	else
		this->ApplyArt();

	return;
}

// note that we always write the error string to message
// so that other places that call here can get the error for 
// a failed evoke
bool cArts::CanUseArt(int art_id, bool bypass)
{
	int x = _tcslen(this->Descrip(art_id));
	unsigned long myval =0L;

	while (x--)
		myval += this->Descrip(art_id)[x];
	
#ifdef UL_DEBUG

//#define MAKE_CKSUM 
#ifdef  MAKE_CKSUM

	int art_num;
	unsigned long name_checksum;

	for (art_num = 0 ; art_num < NUM_ARTS ; art_num++ ) {

		int name_len = _tcslen(this->Descrip(art_num));

		name_checksum =0L;

		while (name_len--)
			name_checksum += this->Descrip(art_num)[name_len];

		LoadString(hInstance, IDS_CHECKSUM, message, sizeof(message));
		_stprintf(errbuf, message, art_info[art_num].my_checksum(art_num, name_checksum), this->Descrip(art_num));
		display->DisplayMessage(errbuf, false);
		INFO(errbuf);
	}
	return true;

#endif  MAKE_CKSUM

	LoadString(hInstance, IDS_CAST_ART_CHECKSUM, message, sizeof(message));
	_stprintf(errbuf, message, art_id, this->Descrip(art_id), 
		art_info[art_id].my_checksum(art_id, myval), art_chksum[art_id]);
	INFO(errbuf);

#endif

	unsigned int checksum1 = art_info[art_id].my_checksum(art_id, myval); // calculated
	unsigned int checksum2 = art_chksum[art_id]; // listed
	
	if (checksum1 != checksum2)
	//if (0) // checksums disabled temporarily
	{ // Cheating Bastard!!!!	
		LoadString (hInstance, IDS_DETECTED_CHEATER, message, sizeof(message));
		display->DisplayMessage(message);
		player->SetCurrStat(Stats::DREAMSOUL, 0, SET_ABSOLUTE, player->ID());
		return false;
	}
		
	if (!ready)
		return false;

	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
		LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
		_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return false;
	}

	// no arts until on-line
	if (!options.welcome_ai && gs && !gs->LoggedIntoLevel())
	{
		LoadString (hInstance, IDS_AWAIT_CONNECTION, message, sizeof(message));
		display->DisplayMessage(message);
		return false;
	}

#ifndef UL_DEBUG
#ifndef GAMEMASTER
	// if player has lost a sphere, don't let them use arts that require
	// their old orbit. this is ignored for dev builds
	if ((player->Orbit() < art_info[art_id].min_orbit) && !bypass)
	{
		LoadString (hInstance, IDS_NEED_MIN_ORBIT, message, sizeof(message));
	 	_stprintf(message, message, art_info[art_id].min_orbit);
		display->DisplayMessage(message);
		return false;
	}
#endif
#endif

	// some arts are only used automatically
	if (cp->Using() && (	(art_id == Arts::GATEKEEPER) ||
								(art_id == Arts::DREAMSEER) ||
								(art_id == Arts::SOULMASTER) ||
								(art_id == Arts::FATESENDER)))
	{
		LoadString (hInstance, IDS_ART_AUTOMATIC, message, sizeof(message));
		display->DisplayMessage(message);
		return false;
	}

	if ((player->flags & ACTOR_PEACE_AURA) && !art_info[art_id].usable_in_sanctuary())
	{
		LoadString (hInstance, IDS_PA_NO_ARTS, message, sizeof(message));
		display->DisplayMessage(message);
		return false;
	}


	// DreamSmith Mark and WordSmith Mark are unevokable (like focus arts above)
	// Special case included to utilize IDS_DREAMSMITH_MARK message
	if (cp->Using() && ((art_id == Arts::DREAMSMITH_MARK) || (art_id == Arts::FORGE_MASTER))) {
		LoadString (hInstance, IDS_DREAMSMITH_MARK, message, sizeof(message));
		display->DisplayMessage (message, false);
		return false;
	}
	if (cp->Using() && (art_id == Arts::WORDSMITH_MARK)){
		LoadString (hInstance, IDS_WORDSMITH_MARK, message, sizeof(message));
		display->DisplayMessage (message, false);
		return false;
	}

	if (cp->Using() && (art_id == Arts::NP_SYMBOL)){
		LoadString (hInstance, IDS_NP_SYMBOL, message, sizeof(message));
		display->DisplayMessage (message, false);
		return false;
	}


	if ((level->ID() == 46) && (player->Room() == 17) && !bypass)
	{
		LoadString (hInstance, IDS_NO_ARTS_DOCA_PIT, message, sizeof(message));
		display->DisplayMessage (message, false);
		return false;
	}

	//  Forbid dreamers from beginning evokation while paralyzed.
	if ((player->flags & (ACTOR_PARALYZED /*|ACTOR_DRUNK|ACTOR_SCARED*/)) && !bypass && !art_info[art_id].usable_in_sanctuary())
	{
		LoadString (hInstance, IDS_CANNOT_USE_THAT, disp_message, sizeof(disp_message));
		LoadString (hInstance, IDS_NO_ART_PARALYZED, temp_message, sizeof(temp_message));
		_stprintf(message, temp_message);
		display->DisplayMessage(message, false);
		return false;
	}

	if (art_in_use != Arts::NONE)
	{
		LoadString (hInstance, IDS_ART_IN_USE, message, sizeof(message));
		display->DisplayMessage(message);
		return false;
	}

	if (!(player->flags & ACTOR_SOULSPHERE) && (art_id == Arts::SOULEVOKE) )
	{
		LoadString (hInstance, IDS_SOULEVOKE_SOULSPHERE_ONLY, message, sizeof(message));
		display->DisplayMessage(message);
		return false;
	}

	if ((player->flags & ACTOR_SOULSPHERE) && !(player->flags & ACTOR_SOULEVOKE) && 
		(art_id != Arts::SOULEVOKE) && (art_id != Arts::GRANT_PPOINT) /*&& !pp.in_use*/)
	{
		LoadString (hInstance, IDS_SOULSPHERE_NO_ARTS, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(art_id));
		display->DisplayMessage(message);
		return false;
	}

	if (!art_info[art_id].usable_in_sanctuary() && (level->Rooms[player->Room()].flags & ROOM_SANCTUARY))
	{
		LoadString (hInstance, IDS_SANCUTARY_NOEVILARTS, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(art_id));
		display->DisplayMessage (message);
		return false;
	}

	if (art_info[art_id].requires_neighbor() && (0 == actors->NumNonHiddenNeighbors()))
	{
		LoadString (hInstance, IDS_NEED_NEIGHBOR, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(art_id));
		display->DisplayMessage (message);
		return false;
	}

	if (options.network && (!art_info[art_id].usable_in_sanctuary() || art_info[art_id].requires_neighbor ()) 
      && gs && gs->LoggedIntoLevel() && !gs->GotPeerUpdates())
	{
    if (!displayed_await_update_art) {
		  LoadString (hInstance, IDS_AWAIT_UPDATE_ART, disp_message, sizeof(disp_message));
		  display->DisplayMessage (disp_message);
      displayed_await_update_art = true;
    } else
      cDS->PlaySound(LyraSound::MESSAGE);
		return FALSE;
	}

	if ((art_info[art_id].creates_item()) && (cp->InventoryFull()))
	{
		LoadString (hInstance, IDS_INV_FULL, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(art_id));
		display->DisplayMessage(message);
		return false;
	}

	if ((art_info[art_id].requires_item()) && (cp->InventoryEmpty()))
	{
		LoadString (hInstance, IDS_INVENTORY_EMPTY, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(art_id));
		display->DisplayMessage(message);
		return false;
	}

	if ((art_info[art_id].stat != Stats::NO_STAT) && !bypass)
	{
		if (!((art_id == Arts::SOULEVOKE) && ((player->MaxStat(art_info[art_id].stat) > art_info[art_id].drain)))
			&&
			(((art_info[art_id].stat == Stats::DREAMSOUL) &&
			 (player->CurrStat(art_info[art_id].stat) <= art_info[art_id].drain))
			||
			((art_info[art_id].stat != Stats::DREAMSOUL) &&
			 (player->CurrStat(art_info[art_id].stat) < art_info[art_id].drain))))
		{
			LoadString (hInstance, IDS_STAT_TOO_LOW, disp_message, sizeof(disp_message));
			_stprintf(message,disp_message,this->Descrip(art_id));
			display->DisplayMessage (message);
			return false;
		}
	}
  if (displayed_await_update_art)
    displayed_await_update_art = false;
	return true;
}

// if we are evoking and the art requires a target, cancel if we're alone
void cArts::CheckTarget(void)
{
	if (art_in_use != Arts::NONE) 
	{
		if ((art_info[art_in_use].requires_neighbor()) && (actors->NumNonHiddenNeighbors() == 0))
		{
			LoadString (hInstance, IDS_NEED_NEIGHBOR, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, this->Descrip(art_in_use));
			display->DisplayMessage (message);
			this->ArtFinished(false);
			return;
		}
	}
}

// casting time has finished - apply the art
void cArts::ApplyArt(void)
{
// int skill = player->Skill(art_in_use);
// int skill_random = (rand()%100);

	if ((art_in_use < 0) || (art_in_use >= NUM_ARTS))
	{
		LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
		_stprintf(errbuf,message, art_in_use);
		NONFATAL_ERROR(errbuf);
		return;
	}

	if ((art_in_use >= Arts::FLAMESHAFT) &&
		(art_in_use <= Arts::FLAMERUIN) && 
		((LyraTime() - gs->LastAttackTime())<SHOT_INTERVAL))
	{ // increase evoke time on flame arts so it doesn't go faster than max attack rate
		art_completion_time += (gs->LastAttackTime() - LyraTime() + SHOT_INTERVAL);
		return;
	}

	int success_random = (rand()%100);

	art_completion_time = NOT_CASTING;
	if ((art_info[art_in_use].requires_neighbor()) && (actors->NumNonHiddenNeighbors() == 0))
	{
		LoadString (hInstance, IDS_NEED_NEIGHBOR, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(art_in_use));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}


	if (art_info[art_in_use].requires_item())
	{
		int owned_items = 0;
		for (cItem *item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			if (item->Status() == ITEM_OWNED)
				owned_items++;
		actors->IterateItems(DONE);

		if (owned_items == 0)
		{
			LoadString (hInstance, IDS_NEED_ITEM, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(art_in_use));
			display->DisplayMessage (message);
			this->ArtFinished(true);
			return;
		}
	}


	if ((player->flags & ACTOR_SOULSPHERE) && !(player->flags & ACTOR_SOULEVOKE) && 
		(art_in_use != Arts::SOULEVOKE) && (art_in_use != Arts::GRANT_PPOINT))
	{
		LoadString (hInstance, IDS_SOULSPHERE_NO_ARTS, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(art_in_use));
		display->DisplayMessage(message);
		this->ArtFinished(false);
		return;
	}

	//  No using arts while paralyzed, etc!
	if (player->flags & (ACTOR_PARALYZED /*|ACTOR_DRUNK|ACTOR_SCARED*/) && !art_info[art_in_use].usable_in_sanctuary())
	{
		LoadString (hInstance, IDS_CANNOT_USE_THAT, disp_message, sizeof(disp_message));
		LoadString (hInstance, IDS_NO_ART_PARALYZED, message, sizeof(message));
	_stprintf(disp_message, message);
		display->DisplayMessage(disp_message, false);
		// Z2 Change: Oops, this is VERY important, we MUST terminate the art evoke
		this->ArtFinished(false);
		return;
	}


	// fail chance increased for cursed players
	int fail_chance = CHANCE_SPELL_FAILURE+1; // add one for consistancy with old code


	//  Activating Curse code
	if (player->flags & ACTOR_CURSED) 	 // if we're cursed, increase failure chances
		fail_chance = player->curse_strength; 


	if (pp.in_use && (pp.cursel == GMsg_UsePPoint::USE_ART) &&
		(art_in_use == pp.art_id)) 
	{
		fail_chance = 0;
	}
	if ((art_info[art_in_use].stat != Stats::NO_STAT) &&
		(success_random < fail_chance)) // skill trial failed
	{
		LoadString (hInstance, IDS_SKILL_TRIAL_FAILED, disp_message, sizeof(disp_message));
	_stprintf(message,disp_message,this->Descrip(art_in_use));
		display->DisplayMessage (message, false);
		this->ArtFinished(true);
		return;
	}

	//art_method_t method = art_info[art_in_use].method;

//RRR 02/12/98
//Assign functions ot arts here..

	art_method_t method;
	switch (art_in_use) {
		case Arts::JOIN_PARTY: method = &cArts::StartJoin; break;
		case Arts::GATEKEEPER: method = &cArts::ApplyWeapon; break;
		case Arts::DREAMSEER: method = &cArts::ApplyWeapon; break;
		case Arts::SOULMASTER: method = &cArts::ApplyWeapon; break;
		case Arts::FATESENDER: method = &cArts::ApplyWeapon; break;
		case Arts::RANDOM: method = &cArts::Random; break;
		case Arts::MEDITATION: method = &cArts::Meditate; break;
		case Arts::RESIST_FEAR: method = &cArts::StartResistFear; break;
		case Arts::PROTECTION: method = &cArts::StartResistCurse; break;
		case Arts::FREE_ACTION: method = &cArts::StartResistParalysis; break;
		case Arts::WARD: method = &cArts::Ward; break;
		case Arts::AMULET: method = &cArts::Amulet; break;
		case Arts::SHATTER: method = &cArts::Shatter; break;
		case Arts::RETURN: method = &cArts::Return; break;
		case Arts::KNOW: method = &cArts::Know; break;
		case Arts::JUDGEMENT: method = &cArts::StartJudgement; break;
		case Arts::IDENTIFY: method = &cArts::StartIdentify; break;
		case Arts::IDENTIFY_CURSE: method = &cArts::StartIdentifyCurse; break;
		case Arts::CHAMELE: method = &cArts::Chamele; break;
		case Arts::VISION: method = &cArts::StartVision; break;
		case Arts::BLAST: method = &cArts::StartBlast; break;
		case Arts::BLEND: method = &cArts::Blend; break;
		case Arts::FORGE_TALISMAN: method = &cArts::StartForgeTalisman; break;
		case Arts::RECHARGE_TALISMAN: method = &cArts::StartRecharge; break;
		case Arts::RESTORE: method = &cArts::StartRestore; break;
		case Arts::REWEAVE: method = &cArts::StartReweave; break;
		case Arts::PURIFY: method = &cArts::StartPurify; break;
		case Arts::DRAIN_SELF: method = &cArts::StartDrainSelf; break;
		case Arts::ABJURE: method = &cArts::StartAbjure; break;
		case Arts::POISON: method = &cArts::StartPoison; break;
		case Arts::ANTIDOTE: method = &cArts::StartAntidote; break;
		case Arts::CURSE: method = &cArts::StartCurse; break;
		case Arts::DRAIN_NIGHTMARE: method = &cArts::StartDrainMare; break;
		case Arts::BANISH_NIGHTMARE: method = &cArts::StartBanishMare; break;
		case Arts::ENSLAVE_NIGHTMARE: method = &cArts::StartEnslaveMare; break;
		case Arts::TRAP_NIGHTMARE: method = &cArts::StartTrapMare; break;
		case Arts::DREAMBLADE: method = &cArts::Dreamblade; break;
		case Arts::TRAIL: method = &cArts::Trail; break;
		case Arts::SCARE: method = &cArts::StartScare; break;
		case Arts::STAGGER: method = &cArts::StartStagger; break;
		case Arts::DEAFEN: method = &cArts::StartDeafen; break;
		case Arts::BLIND: method = &cArts::StartBlind; break;
		case Arts::DARKNESS: method = &cArts::Darkness; break;
		case Arts::PARALYZE: method = &cArts::StartParalyze; break;
		case Arts::FIRESTORM: method = &cArts::Firestorm; break;
		case Arts::RAZORWIND: method = &cArts::Razorwind; break;
		case Arts::RECALL: method = &cArts::Recall; break;
		case Arts::PUSH: method = &cArts::Push; break;
		case Arts::SOULEVOKE: method = &cArts::SoulEvoke; break;
		case Arts::DREAMSTRIKE: method = &cArts::StartDreamStrike; break;
		case Arts::NIGHTMARE_FORM: method = &cArts::NightmareForm; break;
		case Arts::LOCATE_AVATAR: method = &cArts::StartLocate; break;
		case Arts::TRAIN: method = &cArts::StartTrain; break;
		case Arts::INITIATE: method = &cArts::StartInitiate; break;
		case Arts::KNIGHT: method = &cArts::StartKnight; break;
		case Arts::SUPPORT_ASCENSION: method = &cArts::StartSupportAscension; break;
		case Arts::ASCEND: method = &cArts::StartAscend; break;
		case Arts::FINGER_OF_DEATH: method = &cArts::StartDie; break;
		case Arts::GRANT_XP: method = &cArts::StartGrantXP; break;
		case Arts::TERMINATE: method = &cArts::StartTerminate; break;
		case Arts::LEVELTRAIN: method = &cArts::StartSphere; break;
		case Arts::SUPPORT_DEMOTION: method = &cArts::StartSupportDemotion; break;
		case Arts::DEMOTE: method = &cArts::StartDemote; break;
		case Arts::INVISIBILITY: method = &cArts::Invisibility; break;
		case Arts::GIVE: method = &cArts::StartGive; break;
		case Arts::GATESMASHER: method = &cArts::GateSmasher; break;
		case Arts::FATESLAYER: method = &cArts::FateSlayer; break;
		case Arts::SOULREAPER: method = &cArts::SoulReaper; break;
		case Arts::FLAMESHAFT: method = &cArts::FlameShaft; break;
		case Arts::TRANCEFLAME: method = &cArts::TranceFlame; break;
		case Arts::FLAMESEAR: method = &cArts::FlameSear; break;
		case Arts::FLAMERUIN: method = &cArts::FlameRuin; break;
		case Arts::WRITE_SCROLL: method = &cArts::StartWriteScroll; break;
		case Arts::DESTROY_ITEM: method = &cArts::StartDestroyItem; break;
		case Arts::MIND_BLANK: method = &cArts::StartMindBlank; break;
		case Arts::SHOW: method = &cArts::StartShow; break;
		case Arts::BOOT: method = &cArts::StartBoot; break;
		case Arts::UNTRAIN: method = &cArts::StartUnTrain; break;
		case Arts::GRANT_RP_XP: method = &cArts::StartGrantRPXP; break;
		case Arts::EARTHQUAKE: method = &cArts::Earthquake; break;
		case Arts::HYPNOTIC_WEAVE: method = &cArts::HypnoticWeave; break;
		case Arts::VAMPIRIC_DRAW: method = &cArts::StartVampiricDraw; break;
		case Arts::TERROR: method = &cArts::Terror; break;
		case Arts::HEALING_AURA: method = &cArts::HealingAura; break;
		case Arts::ROGER_WILCO: method = &cArts::StartRogerWilco; break;
//		case Arts::DREAMSMITH_MARK: method = &cArts::DreamsmithMark; break;
		case Arts::SUPPORT_TRAINING: method = &cArts::StartSupportTraining; break;
		case Arts::SUPPORT_SPHERING: method = &cArts::StartSupportSphering; break;
		case Arts::TRAIN_SELF: method = &cArts::StartTrainSelf; break;
		case Arts::SOUL_SHIELD: method = &cArts::StartSoulShield; break;
		case Arts::SUMMON: method = &cArts::StartSummon; break;
		case Arts::SUSPEND: method = &cArts::StartSuspend; break;
		case Arts::REFLECT: method = &cArts::Reflect; break;
		case Arts::SACRIFICE: method = &cArts::StartSacrifice; break;
		case Arts::CLEANSE_NIGHTMARE: method = &cArts::StartCleanseMare; break;
		case Arts::CREATE_ID_TOKEN: method = &cArts::StartCreateIDToken; break;
		case Arts::SENSE_DREAMERS: method = &cArts::StartSenseDreamers; break;
		case Arts::EXPEL: method = &cArts::StartExpel; break;
		case Arts::LOCATE_NEWLIES: method = &cArts::StartFindNewlies; break;
		case Arts::COMBINE: method = &cArts::StartCombine; break;
		case Arts::POWER_TOKEN: method = &cArts::StartPowerToken; break;
		case Arts::SHOW_GRATITUDE: method = &cArts::StartShowGratitude; break;
		case Arts::QUEST: method = &cArts::StartQuest; break;
		case Arts::EMPATHY: method = &cArts::StartEmpathy; break;
		case Arts::RADIANT_BLAZE: method = &cArts::RadiantBlaze; break;
		case Arts::POISON_CLOUD: method = &cArts::PoisonCloud; break;
		case Arts::BREAK_COVENANT: method = &cArts::StartBreakCovenant; break;
		case Arts::PEACE_AURA: method = &cArts::StartPeaceAura; break;
		case Arts::SABLE_SHIELD: method = &cArts::StartSableShield; break;
		case Arts::ENTRANCEMENT: method = &cArts::StartEntrancement; break;
		case Arts::SHADOW_STEP: method = &cArts::StartShadowStep; break;
		case Arts::DAZZLE: method = &cArts::Dazzle; break;
		case Arts::GUILDHOUSE: method = &cArts::GuildHouse; break;
		case Arts::CORRUPT_ESSENCE: method = &cArts::StartCorruptEssence; break;
		case Arts::TEHTHUS_OBLIVION: method = &cArts::TehthusOblivion; break;
		case Arts::CHAOS_PURGE: method = &cArts::StartChaosPurge; break;
		case Arts::CUP_SUMMONS: method = &cArts::StartCupSummons; break;
//		case Arts::WORDSMITH_MARK: method = &cArts::WordSmithMark; break;
		case Arts::HOUSE_MEMBERS: method = &cArts::StartHouseMembers; break;
		case Arts::FREESOUL_BLADE: method = &cArts::StartFreesoulBlade; break;
		case Arts::ILLUMINATED_BLADE: method = &cArts::StartIlluminatedBlade; break;
		case Arts::SUMMON_PRIME: method = &cArts::StartSummonPrime; break;
		case Arts::GRANT_PPOINT: method = &cArts::StartGrantPPoint; break;
		case Arts::SCAN: method = &cArts::StartJudgement; break; 
		case Arts::PASSLOCK: method = &cArts::Blend; break;
		case Arts::HEAL: method = &cArts::StartRestore; break;
		case Arts::SANCTIFY: method = &cArts::StartResistCurse; break;
		case Arts::LOCK: method = &cArts::Ward; break;
		case Arts::KEY: method = &cArts::Amulet; break;
		case Arts::BREAK_LOCK: method = &cArts::Shatter; break;
		case Arts::REPAIR: method = &cArts::StartReweave; break;
		case Arts::REMOVE_CURSE: method = &cArts::StartPurify; break;
		case Arts::HOLD_AVATAR: method = &cArts::StartParalyze; break;
		case Arts::SANCTUARY: method = &cArts::Recall; break;
		case Arts::SHOVE: method = &cArts::Push; break;
		case Arts::SCRIBE_NOT: method = &cArts::StartWriteScroll; break;
		//case Arts::FORGE_MASTER: method = &cArts::StartDreamsmithMark; break; 
		case Arts::MERGE_TALISMAN: method = &cArts::StartCombine; break; 
    case Arts::SENSE_MARE: method = &cArts::StartFindMares; break;
    case Arts::TEMPEST: method = &cArts::Tempest; break;
    case Arts::KINESIS: method = &cArts::StartKinesis; break;
    case Arts::MISDIRECTION: method = &cArts::Misdirection; break;
    case Arts::CHAOTIC_VORTEX: method = &cArts::ChaoticVortex; break;
	case Arts::CHAOS_WELL: method = &cArts::EssenceContainer; break;
	case Arts::RALLY: method = &cArts::StartRally; break;
//		case Arts::NP_SYMBOL: method = &cArts::W; break;

	}
	(this->*method)();

	return;
}

// cancel an art in progress, for whatever reason
// does nothing if no art is in progress
void cArts::CancelArt(void)
{
	if (art_in_use != Arts::NONE)
	{
		LoadString (hInstance, IDS_ART_CANCEL, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(art_in_use));
		display->DisplayMessage (message);
		this->ArtFinished(false);
	}
	return;
}

// called either when a skill trial fails or after an art has
// been used successfully; drains the appropriate stat from the player
void cArts::DrainStat(lyra_id_t art_id)
{
	if ((art_id < 0) || (art_id >= NUM_ARTS))
	{
	LoadString(hInstance, IDS_INVALID_ART, message, sizeof(message));
	_stprintf(errbuf,message, art_id);
		NONFATAL_ERROR(errbuf);
		return;
	}

	if (art_info[art_id].stat != Stats::NO_STAT)
	{
		if (art_id == Arts::SOULEVOKE)
			player->SetMaxStat(art_info[art_id].stat, player->MaxStat(art_info[art_id].stat)-art_info[art_id].drain, player->ID());
		else
			player->SetCurrStat(art_info[art_id].stat, -art_info[art_id].drain, SET_RELATIVE, player->ID());
	}
	return;
}

bool cArts::IncreaseSkill(int art_id, int chance_increase)
{
	int skill = player->Skill(art_id);
	int focus = this->Stat(art_id);

	if (0 == skill) // for pseudo arts, we don't want to ever increase
		return false;

	// non-focus stats are limited to 20 below the player's orbit
	if ((focus != Stats::NO_STAT) &&
		(focus != Stats::DREAMSOUL) && 
		(focus != player->FocusStat()) &&
		(this->Restricted(art_id)))
	{
		if (skill >= (player->Orbit() - 20))
			return false;
	}

	if	(	(skill < player->Orbit())	// cannot go above orbit
		&&	((skill%10) != 9)				// cannot platue
		&&	(skill < Stats::SKILL_MAX)	// cannot go above game max
		)
	{
		// only increase skills below max; don't increase if training required
		int skill_random = (rand()%100);

		if (player->flags & LyraEffect::PLAYER_CURSED)
			skill = 98;

		if ((skill_random < chance_increase) && ((rand()%100) > skill))
		{
			LoadString (hInstance, IDS_SKILL_INCREASE, disp_message, sizeof(disp_message));
		_stprintf(message,disp_message,this->Descrip(art_id));
			display->DisplayMessage(message);
			player->SetSkill(art_id, 1, SET_RELATIVE, player->ID());
			gs->UpdateServer();
			return true;
		}
		else
			return false;
	}
	return false;
}


// called after the player has cast an art; drain is true if the use
// was successful, or false if it was cancelled for some reason
void cArts::ArtFinished(bool drain, bool allow_skill_increase)
{
	player->EvokingFX().DeActivate();

	bool do_drain = drain;

	if (pp.in_use && (pp.cursel == GMsg_UsePPoint::USE_ART) &&
		(art_in_use == pp.art_id)) {
		pp.reset();
		do_drain = false;
	}

	waiting_for_sel = waiting_for_dlg = fDoingLocate = fDoingSense = fDoingNewlies = fDoingMares = false;
	callback_method = NULL;
	if (do_drain && (art_in_use != Arts::NONE))
	{
		this->DrainStat(art_in_use); // drain stat
#ifndef PMARE // no skill increases for pmares
		if (allow_skill_increase)
			this->IncreaseSkill(art_in_use,CHANCE_SKILL_INCREASE);
#endif
	}

	if (dummy)
		this->RemoveDummyNeighbor();
	if (cp_mode != NO_TAB)
		this->RestoreCP();
	art_in_use = Arts::NONE;
	art_completion_time = NOT_CASTING;
	combining_item = gratitude_item = NO_ACTOR;

	if (active_dlg)
	{	// if dialog is up, close it
		DestroyWindow(active_dlg);
		active_dlg = NULL;
	}
	return;
}

void cArts::CaptureCP(int new_mode, lyra_id_t art_id)
{	// set cp_mode if not already captured
	if (cp_mode == NO_TAB)
		cp_mode = cp->Mode();
	cp->SetMode(new_mode, true, false);
	if (new_mode == INVENTORY_TAB)
	{
		cp->SetSelectedItem(NO_ACTOR);
		if (options.art_prompts)
		{
			LoadString (hInstance, IDS_CHOOSE_ITEM, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
	}
	else if (new_mode == NEIGHBORS_TAB)
	{
		cp->SetSelectedNeighbor(NO_ACTOR);
		if (options.art_prompts)
		{
			LoadString (hInstance, IDS_CHOOSE_AVATAR, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
	}
	else if (new_mode == ARTS_TAB)
	{
		cp->SetSelectedArt(Arts::NONE);
		if (options.art_prompts)
		{
			LoadString (hInstance, IDS_CHOOSE_ART, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
		}
	}

	return;
}

void cArts::RestoreCP(void)
{ // restore cp mode
	if (cp_mode != NO_TAB)
	{
		cp->SetMode(cp_mode, false, true);
		cp_mode = NO_TAB;
	}
	return;
}

// Sets up to wait for user to choose target
void cArts::WaitForSelection(art_method_t callback, lyra_id_t art_id)
{
	waiting_for_sel = true;
	callback_method = callback;
	// set to false so we can check if the next click is a selection
	cp->SetSelectionMade(false);
	return;
}

void cArts::WaitForDialog(HWND hDlg, lyra_id_t art_id)
{
	active_dlg = hDlg;
	waiting_for_dlg = true;
	return;
}


void cArts::CheckForSelection(void)
{
	if (waiting_for_sel && cp->SelectionMade())
	{	// if selection made, activate; we can't munch callback_method
		// in case we're doing a multi-selection art (like train)
		waiting_for_sel = false;
		(this->*callback_method)();
	}

	return;
}

// to allow selection of player as a target
void cArts::AddDummyNeighbor(void)
{
	RmRemotePlayer info;
	LmPeerUpdate update;

	if (dummy)
	{
		LoadString (hInstance, IDS_DUMMY_NOT_GONE, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->RemoveDummyNeighbor();
	}

	update.Init(0, 0, (short)player->x, (short)player->y, 0, 0);
	update.SetAngle(player->angle);
	LmAvatar avatar = player->Avatar();
	avatar.SetHidden(0);
	info.Init(update, avatar, player->Name(), player->ID(), 0);
	dummy = new cNeighbor(info);
	// important! make no-collide or risk an invulnerability bug!
	// but don't do it in the constructor - ever call to SetAvatar resets
	// the NOCOLLIDE flag
	dummy->SetRoom(player->Room());
	dummy->flags = dummy->flags | ACTOR_NOCOLLIDE;

	return;
}

void cArts::RemoveDummyNeighbor(void)
{
	if (dummy)
	{
		dummy->SetTerminate();
		dummy = NULL;
	}
	return;
}

// look up neighbor based on ID; return NO_ACTOR if not found
cNeighbor* cArts::LookUpNeighbor(lyra_id_t id)
{
	cNeighbor *n;

	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
		if (n->ID() == id)
			break;
	actors->IterateNeighbors(DONE);
	return n;
}

// display the proper message for when we apply an art to ourselves
// or to others
void cArts::DisplayUsedByOther(cNeighbor *n, lyra_id_t art_id)
{
	if ((n != NO_ACTOR) && (n->ID() != player->ID()))
	{
		if (n->Avatar().Hidden()) 
		{
			LoadString (hInstance, IDS_ART_APPLIED_TO_SELF_BY_HIDDEN_GM, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, this->Descrip(art_id));
		}
		else
		{
			LoadString (hInstance, IDS_ART_APPLIED_TO_SELF, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, n->Name(), this->Descrip(art_id));
		}
		display->DisplayMessage (message);
	}
	return;
}

void cArts::DisplayUsedOnOther(cNeighbor *n, lyra_id_t art_id)
{
	if (n != NO_ACTOR)
	{
		if (options.art_prompts)
		{
			LoadString (hInstance, IDS_ART_APPLIED_TO_OTHER, disp_message, sizeof(disp_message));
			if (n->ID() == player->ID())
			{
			LoadString(hInstance, IDS_YOURSELF, temp_message, sizeof(temp_message));
			_stprintf(message, disp_message, this->Descrip(art_id), temp_message);
			}
			else
			_stprintf(message, disp_message, this->Descrip(art_id), n->Name());
			display->DisplayMessage (message, false);
		}
	}
	return;
}

// in case target has bailed
void cArts::DisplayNeighborBailed(lyra_id_t art_id)
{
	LoadString (hInstance, IDS_NEIGHBOR_BAILED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, this->Descrip(art_id));
	display->DisplayMessage (message);
	return;
}

// in case target has bailed
void cArts::DisplayItemBailed(lyra_id_t art_id)
{
	LoadString (hInstance, IDS_ITEM_BAILED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, this->Descrip(art_id));
	display->DisplayMessage (message);
	return;
}

// in case target has bailed
void cArts::DisplayArtBailed(lyra_id_t art_id)
{
	LoadString (hInstance, IDS_ART_BAILED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, this->Descrip(art_id));
	display->DisplayMessage (message);
	return;
}

// placeholder for arts not yet implemented
void cArts::NotImplemented(void)
{
	LoadString (hInstance, IDS_NOT_IMPLEMENTED, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	this->ArtFinished(false);
	return;
}

////////////////////////////////////////////////////////////////
// *** Arts that require no selection ***
////////////////////////////////////////////////////////////////


void cArts::ApplyWeapon(void)
{
	this->ArtFinished(true,false);
	return;
}


//////////////////////////////////////////////////////////////////
// Meditate

void cArts::Meditate(void)
{
	int duration = this->Duration(Arts::MEDITATION, player->Skill(Arts::MEDITATION));
	if (player->SetTimedEffect(LyraEffect::PLAYER_MEDITATING, duration, player->ID()))
			cDS->PlaySound(LyraSound::MEDITATION, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Chaos Well
void cArts::EssenceContainer(void)
{
	int capacity = 20 * ((player->Skill(Arts::CHAOS_WELL) / 10) + 1);
	lyra_item_meta_essence_nexus_t nexus = { LyraItem::META_ESSENCE_NEXUS_FUNCTION, 0, 0, 0, capacity, capacity };
	LmItem info;
	LmItemHdr header;
	cItem *item;

	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_SENDSTATE);
	header.SetGraphic(LyraBitmap::BOX);
	header.SetColor1(player->Avatar().Color2()); header.SetColor2(player->Avatar().Color3());
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::META_ESSENCE_NEXUS_FUNCTION), 0, 0));

	LoadString(hInstance, IDS_CHAOS_WELL, message, sizeof(message));
	info.Init(header, message, 0, 0, 0);
	info.SetStateField(0, &nexus, sizeof(nexus));
	info.SetCharges(1);
	item = CreateItem(player->x, player->y, player->angle, info, 0, false, GMsg_PutItem::DEFAULT_TTL);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	this->ArtFinished(true);
}

//////////////////////////////////////////////////////////////////
// Ward

void cArts::Ward(void)
{
	LmItem info;
	LmItemHdr header;
	cItem *item;
	lyra_item_ward_t ward = {LyraItem::WARD_FUNCTION, 0, 0, 0, 0};

	linedef *line; line = FindTeleportal(player);

	if (line == NULL)
	{	// no teleportal nearby
		LoadString (hInstance, IDS_NO_TELEPORTAL, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	if ((line->flags & LINE_NO_WARD) || (level->Rooms[player->Room()].flags & ROOM_NOREAP))
	{	// can't ward this teleportal; in no reap areas, wards would last forever!
		LoadString (hInstance, IDS_NO_WARDING, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	// see if there's already ward on the teleportal
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->ItemFunction(0) == LyraItem::WARD_FUNCTION) && (line == ((linedef*)item->Extra())))
		{
			actors->IterateItems(DONE);
			LoadString (hInstance, IDS_ALREADY_WARDED, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message);
			this->ArtFinished(false);
			return;
		}
	actors->IterateItems(DONE);

	// see if we can pass this teleportal

	int guild_id = GetTripGuild(line->flags);
	if (!CanPassPortal(line->trip3, guild_id, true))
	{
		LoadString (hInstance, IDS_CANT_WARD_IMPASSABLE, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	// uncomment to prevent warding of trip_cross teleportals
// if (!(line->flags & TRIP_ACTIVATE))
// {
// 	LoadString (hInstance, IDS_WARD_TRIP_ONLY, disp_message, sizeof(disp_message));
// 	display->DisplayMessage(disp_message);
// 	this->ArtFinished(false);
// 	return;
// }

	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_ALWAYS_DROP );
	header.SetGraphic(LyraBitmap::WARD);
	header.SetColor1(0); header.SetColor2(0);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::WARD_FUNCTION), 0, 0));

	ward.strength = player->Skill(art_in_use);
	ward.from_vert = (short)line->from;
	ward.to_vert = (short)line->to;
	ward.set_player_id(player->ID());

	LoadString(hInstance, IDS_WARD, message, sizeof(message));
	info.Init(header, message, 0, 0, 0);
	info.SetStateField(0, &ward, sizeof(ward));
	info.SetCharges(1);
	int ttl = 120*((player->SkillSphere(art_in_use))+1);
	item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}
	item->SetMarkedForDrop();
	cDS->PlaySound(LyraSound::WARD, player->x, player->y, true);

	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Amulet

void cArts::Amulet(void)
{
	TCHAR name[LmItem::NAME_LENGTH];
	LmItem info;
	LmItemHdr header;
	lyra_item_amulet_t amulet = {LyraItem::AMULET_FUNCTION, 0, 0};

	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_CHANGE_CHARGES);
	header.SetGraphic(LyraBitmap::AMULET);
	header.SetColor1(0); header.SetColor2(0);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::AMULET_FUNCTION),0,0));

	amulet.strength = (unsigned char)player->Skill(art_in_use);
	amulet.player_id = player->ID();

	// r->ErrorInfo()->RIf name is longer than ten, truncate it on the amulet name
	TCHAR myname[20];
_stprintf(myname, player->Name());
	if (_tcslen(myname) < 10) 
	{
	LoadString(hInstance, IDS_AMULET_OF, message, sizeof(message));
	_stprintf(name, message, myname);
	}
	else {
		int i;
		TCHAR myname10[10];
		for (i=0;i<10;i++)
			myname10[i]=myname[i];
	_stprintf(&myname10[9], _T("\0"));
	LoadString(hInstance, IDS_AMULET_OF, message, sizeof(message));
	_stprintf(name, message, myname10);
	}

	info.Init(header, name, 0, 0, 0);
	info.SetStateField(0, &amulet, sizeof(amulet));
	info.SetCharges(player->Skill(art_in_use));
	cItem* item = CreateItem(player->x, player->y, player->angle, info, 0, false);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Shatter

void cArts::Shatter(void)
{
	cItem *item = NO_ACTOR;
	lyra_item_ward_t ward = {LyraItem::WARD_FUNCTION, 0, 0, 0, 0};

	linedef *line; line = FindTeleportal(player);

	if (line != NULL)
	{	// see if there's a ward on the teleportal
		for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			if ((item->ItemFunction(0) == LyraItem::WARD_FUNCTION) && (line == ((linedef*)item->Extra())))
				break;
		actors->IterateItems(DONE);
	}

	if ((line == NULL) || (item == NO_ACTOR))
	{
		LoadString (hInstance, IDS_NO_WARD, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
	if (!item->Destroy())
		item->SetTerminate();
	LoadString (hInstance, IDS_WARD_SHATTERED, disp_message, sizeof(disp_message));
	display->DisplayMessage(disp_message, false);
	cDS->PlaySound(LyraSound::SHATTER, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Blend

void cArts::Blend(void)
{
	int duration = this->Duration(art_in_use, player->Skill(art_in_use));
	if (player->SetTimedEffect(LyraEffect::PLAYER_BLENDED, duration, player->ID()))
			cDS->PlaySound(LyraSound::BLEND, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}



struct know_t {
	int level;
	int room;
	UINT string;
};

#ifndef AGENT
//const int NUM_KNOW_STRINGS = 50; //11/19/02 - MDA -- array size changed to 50 to allow for further additions
static know_t know_strings[] = {
	{ 1, 3, IDS_KNOW0103},
	{ 2, 16, IDS_KNOW0216},
	{ 2, 31, IDS_KNOW0231},
	{ 3, 12, IDS_KNOW0312},
	{ 3, 26, IDS_KNOW0326},
	{ 4, 1, IDS_KNOW0401},
	{ 5, 28, IDS_KNOW0528},
	{ 6, 1, IDS_KNOW0601},
	{ 6, 2, IDS_KNOW0602},
	{ 6, 6, IDS_KNOW0606},
	{ 6, 12, IDS_KNOW0612},
	{ 6, 13, IDS_KNOW0613},
	{ 6, 17, IDS_KNOW0617},
	{ 6, 23, IDS_KNOW0623},
	{ 6, 27, IDS_KNOW0627},
	{ 6, 29, IDS_KNOW0629},
	{ 7, 1, IDS_KNOW0701},
	{ 7, 3, IDS_KNOW0703},
	{ 7, 4, IDS_KNOW0704},
	{ 7, 5, IDS_KNOW0705},
	{ 7, 6, IDS_KNOW0706},
	{ 7, 9, IDS_KNOW0709},
	{ 8, 25, IDS_KNOW0825},
	{ 9, 13, IDS_KNOW0913},
	{ 11, 16, IDS_KNOW1116},
	{ 12, 2, IDS_KNOW1202},
	{ 12, 32, IDS_KNOW1232},
	{ 13, 1, IDS_KNOW1301},
	{ 14, 4, IDS_KNOW1404},
	{ 15, 17, IDS_KNOW1517},
	{ 15, 22, IDS_KNOW1522},
	{ 16, 1, IDS_KNOW1601},
	{ 16, 4, IDS_KNOW1604},
	{ 16, 7, IDS_KNOW1607},
	{ 16, 8, IDS_KNOW1608},
	{ 20, 1, IDS_KNOW2001},
	{ 20, 3, IDS_KNOW2003},
	{ 20, 5, IDS_KNOW2005},
	{ 20, 7, IDS_KNOW2007},
	{ 20, 9, IDS_KNOW2009},
	{ 20, 11, IDS_KNOW2011},
	{ 20, 13, IDS_KNOW2013},
	{ 20, 15, IDS_KNOW2015},
	{ 22, 2, IDS_KNOW2202},
	{ 22, 3, IDS_KNOW2203},
	{ 22, 4, IDS_KNOW2204},
	{ 22, 6, IDS_KNOW2206},
	{ 22, 9, IDS_KNOW2209},
	{ 22, 16, IDS_KNOW2216},
	{ 22, 30, IDS_KNOW2230},
	{ 26, 2, IDS_KNOW2602},
	{ 26, 3, IDS_KNOW2603},
	{ 26, 4, IDS_KNOW2604},
	{ 26, 13, IDS_KNOW2613},
	{ 26, 14, IDS_KNOW2614},
	{ 26, 30, IDS_KNOW2630},
	// { 27, 1, IDS_KNOW2701},
	// { 29, 17, IDS_KNOW2917},
	{ 29, 21, IDS_KNOW2921},
	// { 30, 7, IDS_KNOW3007},
	{ 31, 17, IDS_KNOW3117},
	{ 32, 21, IDS_KNOW3221},
	// { 33, 18, IDS_KNOW3318},
	{ 34, 13, IDS_KNOW3413},
	{ 37, 1, IDS_KNOW3701},
	{ 37, 2, IDS_KNOW3702},
	{ 37, 3, IDS_KNOW3703},
	{ 37, 4, IDS_KNOW3704},
	{ 39, 7, IDS_KNOW3907},
	{ 39, 9, IDS_KNOW3909},
	{ 39, 13, IDS_KNOW3913},
	{ 39, 14, IDS_KNOW3914},
	{ 39, 15, IDS_KNOW3915},
	{ 39, 31, IDS_KNOW3931},
	{ 42, 3, IDS_KNOW4203},
};
//NUM_KNOW_STRINGS defined here to facilitate future additions - 12/1/2002
const int NUM_KNOW_STRINGS = sizeof (know_strings) / sizeof (know_t);
#endif


//////////////////////////////////////////////////////////////////
// Know

void cArts::Know(void)
{
	LoadString (hInstance, IDS_KNOW_AREA, disp_message, sizeof(disp_message));
	_stprintf(message,disp_message,level->RoomName(player->Room()), level->Name(level->ID()));
	display->DisplayMessage (message, false);
	this->ArtFinished(true);
	cDS->PlaySound(LyraSound::KNOW, player->x, player->y, true);
#ifndef AGENT
	// look for room-specific description
	for (int i=0;i<NUM_KNOW_STRINGS;i++)
		if ((know_strings[i].level == level->ID()) && (know_strings[i].room == player->Room()))
		{
			LoadString (hInstance, know_strings[i].string, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message, false);
		}
#endif
	return;
}

//////////////////////////////////////////////////////////////////
// Chamele

void cArts::Chamele(void)
{
	int duration = this->Duration(Arts::CHAMELE, player->Skill(Arts::CHAMELE));
	if (player->SetTimedEffect(LyraEffect::PLAYER_CHAMELED, duration, player->ID()))
		cDS->PlaySound(LyraSound::CHAMELE, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Invisibility

void cArts::Invisibility(void)
{
	int duration = this->Duration(Arts::INVISIBILITY, player->Skill(Arts::INVISIBILITY));
	if (player->SetTimedEffect(LyraEffect::PLAYER_INVISIBLE, duration, player->ID()))
		cDS->PlaySound(LyraSound::CHAMELE, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Random

void cArts::Random(void)
{
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::RANDOM, 0, 0);
	this->ArtFinished(true);
	return;
}

// The 8 prime weapon arts share the following code to create them;
// note that the constants have dreamblade names for no particular reason

// damage done is according to plaetau
// NOTE: if you change this, server code must also change!!!  this means
// you tell Jason what you did here (preferably, just send the table)
int weapon_damage_table[10] =
{
	14,
	30,
	52,
	18,
	25,
	44,
	46,
	60,
	39,
	38
};

// Jared 1-23-00 DreamSmith Mark is not an evokable art. Instead, it functions
// much like focus arts. It is only checked when attempting to train someone
// in Forge Talisman. The error message below is now printed *before* the art
// evoke FX are done.
/*
//////////////////////////////////////////////////////////////////
// Dreamsmith Mark

void cArts::DreamsmithMark(void)
{
	LoadString (hInstance, IDS_DREAMSMITH_MARK, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message, false);
	this->ArtFinished(true);
	return;
}
*/


//////////////////////////////////////////////////////////////////
// Dreamblade, etc.


void cArts::CreateLocalWeapon(int color)
{
	LmItem info;
	LmItemHdr header;
	lyra_item_missile_t missile = { LyraItem::MISSILE_FUNCTION, MELEE_VELOCITY,
		0, weapon_damage_table[player->SkillSphere(art_in_use)],
		LyraBitmap::DREAMBLADE_MISSILE};

	header.Init(0, LyraTime());
	header.SetFlags(0);
	header.SetGraphic(LyraBitmap::DREAMBLADE);
	header.SetColor1(color);
	header.SetColor2(0);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::MISSILE_FUNCTION), 0, 0));

	info.Init(header, this->Descrip(art_in_use), 0, 0, 0);
	info.SetStateField(0, &missile, sizeof(missile));
	info.SetCharges(INFINITE_CHARGES);

	int duration = this->Duration(art_in_use, player->Skill(art_in_use));

	cItem* item = CreateItem(player->x, player->y, player->angle, info, 0, true,
		LyraTime() + duration);

	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	cp->SetSelectedItem(item);

	LoadString (hInstance, IDS_DREAMWEAPON_ON, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, this->Descrip(art_in_use));
	display->DisplayMessage(message, false);
	cDS->PlaySound(LyraSound::DREAMBLADE, player->x, player->y, true);

	this->ArtFinished(true, true);
	return;
}

void cArts::Dreamblade(void)
{
	this->CreateLocalWeapon(4);
	return;
}

void cArts::GateSmasher(void)
{
	this->CreateLocalWeapon(0);
	return;
}

void cArts::FateSlayer(void)
{
	this->CreateLocalWeapon(12);
	return;
}

void cArts::SoulReaper(void)
{
	this->CreateLocalWeapon(8);
	return;
}

//////////////////////////////////////////////////////////////////
// Flameshaft, etc.

void cArts::LaunchFireball(void) // used by next 4 arts
{
	if (options.network)
		this->ArtFinished(gs->PlayerAttack(LyraBitmap::FIREBALL_MISSILE,ART_MISSILE_VELOCITY,
													  0, weapon_damage_table[player->SkillSphere(art_in_use)], NO_ACTOR,
													 art_in_use),false);
	else
		this->ArtFinished(false);
	return;
}


void cArts::FlameShaft(void)
{
	this->LaunchFireball();
	return;
}

void cArts::TranceFlame(void)
{
	this->LaunchFireball();
	return;
}

void cArts::FlameSear(void)
{
	this->LaunchFireball();
	return;
}

void cArts::FlameRuin(void)
{
	this->LaunchFireball();
	return;
}




//////////////////////////////////////////////////////////////////
// Trail

void cArts::Trail(void)
{
	if (player->flags & ACTOR_TRAILING)
	{
		player->RemoveTimedEffect(LyraEffect::PLAYER_TRAIL);
		this->ArtFinished(true);
	}
	else
	{
		int duration = this->Duration(Arts::TRAIL, player->Skill(Arts::TRAIL));
		player->SetTimedEffect(LyraEffect::PLAYER_TRAIL, duration, player->ID());
		cDS->PlaySound(LyraSound::TRAIL, player->x, player->y, true);
		this->ArtFinished(true);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Push

void cArts::Push()
{
	cDS->PlaySound(LyraSound::PUSH, player->x, player->y, true);
	if (options.network)
		this->ArtFinished(gs->PlayerAttack(LyraBitmap::PUSH_MISSILE, MELEE_VELOCITY, 0, 0));
	else
		this->ArtFinished(false);
	return;
}

//////////////////////////////////////////////////////////////////
// Soul Evoke

void cArts::SoulEvoke()
{
	int duration = this->Duration(Arts::SOULEVOKE, player->Skill(Arts::SOULEVOKE));
	// r->ErrorInfo()->RMoved this code into cPlayer to enable talismans
/*	if (!(player->flags & ACTOR_SOULSPHERE))
	{
		LoadString (hInstance, IDS_SOULEVOKE_SOULSPHERE_ONLY, disp_message, sizeof(disp_message));
	_stprintf(message,disp_message,this->Descrip(Arts::SOULEVOKE));
		display->DisplayMessage (message, false);
		this->ArtFinished(false);
		return;
	} */
	if (player->MaxStat(Stats::DREAMSOUL) < MIN_DS_SOULEVOKE)
	{
		LoadString (hInstance, IDS_SE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message, false);
		this->ArtFinished(false);
		return;

	}
	// Cause permanent dreamsoul loss!
	gs->SendPlayerMessage(player->ID(), RMsg_PlayerMsg::SOULEVOKE, 0, 0);	
	player->SetTimedEffect(LyraEffect::PLAYER_SOULEVOKE, duration, player->ID());
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Nightmare Form

void cArts::NightmareForm(void)
{
	int duration = this->Duration(Arts::NIGHTMARE_FORM, player->Skill(Arts::NIGHTMARE_FORM));
	player->SetTimedEffect(LyraEffect::PLAYER_TRANSFORMED, duration, player->ID());
	// r->ErrorInfo()->RMoved this code into cPlayer so NMF can occur through talismans
//	LmAvatar new_avatar;
	//new_avatar.Init((player->Skill(Arts::NIGHTMARE_FORM)/20 + 1), 0, 0, 0, 0, 0, Guild::NO_GUILD, 0);
//	new_avatar.Init(Avatars::EMPHANT, 0, 0, 0, 0, 0, Guild::NO_GUILD, 0, 0, 0, 0, 0, 0);
//	player->SetTransformedAvatar(new_avatar);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Recall

void cArts::Recall(void)
{
	int duration = this->Duration(art_in_use, player->Skill(art_in_use));
	player->SetTimedEffect(LyraEffect::PLAYER_RECALL, duration, player->ID());
	// r->ErrorInfo()->RMoved this code into cPlayer so Recall can occur through talismans
//	player->SetRecall(player->x, player->y, player->angle, level->ID());
//	gs->SendPlayerMessage(0, RMsg_PlayerMsg::RECALL, 0, 0);
//	LoadString (hInstance, IDS_RECALL, disp_message, sizeof(disp_message));
//	display->DisplayMessage(disp_message, false);
	cDS->PlaySound(LyraSound::RECALL, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Return

void cArts::Return(void)
{

	// r->ErrorInfo()->RMoved this code into cPlayer so Return can occur through talismans
//	if (player->flags & ACTOR_RETURN)
//	{ // 2nd activation - return
//		player->Teleport(player->ReturnX(), player->ReturnY(), player->ReturnAngle(), player->ReturnLevel());
//		player->RemoveTimedEffect(LyraEffect::PLAYER_RETURN);
//	}
//	else
//	{ // 1st activation - mark location
		int duration = this->Duration(Arts::RETURN, player->Skill(Arts::RETURN));
		player->SetTimedEffect(LyraEffect::PLAYER_RETURN, duration, player->ID());
//		 player->SetReturn(player->x, player->y, player->angle, level->ID());
//		gs->SendPlayerMessage(0, RMsg_PlayerMsg::RETURN, 0, 0);
//	}
	cDS->PlaySound(LyraSound::RETURN, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Reflect

void cArts::Reflect(void)
{

	int duration = this->Duration(Arts::REFLECT, player->Skill(Arts::REFLECT));
	player->SetTimedEffect(LyraEffect::PLAYER_REFLECT, duration, player->ID());

	cDS->PlaySound(LyraSound::REFLECT, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

void cArts::ApplyReflectedArt(int art_id, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	LoadString (hInstance, IDS_REFLECTED_ART, disp_message, sizeof(disp_message));
	if (n != NO_ACTOR) {
		_stprintf(message, disp_message, n->Name(), this->Descrip(art_id));
		display->DisplayMessage(message);
	}
	switch (art_id)
	{
	case Arts::RESIST_FEAR:
		ApplyResistFear(player->Skill(art_id),player->ID());
		break;
	case Arts::PROTECTION:
		ApplyResistCurse(Arts::PROTECTION, player->Skill(art_id),player->ID());
		break;
	case Arts::FREE_ACTION:
		ApplyResistParalysis(player->Skill(art_id),player->ID());
		break;
	case Arts::JUDGEMENT:
		ApplyJudgement(player->Skill(art_id),player->ID());
		break;
	case Arts::IDENTIFY_CURSE:
		ApplyIdentifyCurse(player->Skill(art_id),player->ID());
		break;
	case Arts::VISION:
		ApplyVision(player->Skill(art_id),player->ID());
		break;
	case Arts::BLAST:					
		ApplyBlast(player->Skill(art_id),REFLECT_ID);
		break;
	case Arts::RESTORE:
		ApplyRestore(Arts::RESTORE, player->Skill(art_id),player->ID());
		break;
	case Arts::PURIFY:
		ApplyPurify(Arts::PURIFY, player->Skill(art_id),player->ID());
		break;
	case Arts::ABJURE:
		ApplyAbjure(player->Skill(art_id), caster_id);
		break;
	case Arts::POISON:
		ApplyPoison(player->Skill(art_id),player->ID());
		break;
	case Arts::ANTIDOTE:
		ApplyAntidote(player->Skill(art_id),player->ID());
		break;
	case Arts::CURSE:
		ApplyCurse(player->Skill(art_id),player->ID());
		break;
	case Arts::SCARE:
		ApplyScare(player->Skill(art_id),player->ID());
		break;
	case Arts::STAGGER:
		ApplyStagger(player->Skill(art_id),player->ID());
		break;
	case Arts::DEAFEN:
		ApplyDeafen(player->Skill(art_id),player->ID());
		break;
	case Arts::BLIND:
		ApplyBlind(player->Skill(art_id),player->ID());
		break;
	case Arts::PARALYZE:
		ApplyParalyze(Arts::PARALYZE, player->Skill(art_id),player->ID());
		break;
	case Arts::MIND_BLANK:
		ApplyMindBlank(player->Skill(art_id),player->ID());
		break;
	case Arts::SOUL_SHIELD:
		ApplySoulShield(player->Skill(art_id),player->ID());
		break;
	case Arts::KINESIS:
		ApplyKinesis (player->Skill(art_id), REFLECT_ID, ((player->angle)+Angle_180)/4);
		break;
	case Arts::PEACE_AURA:
		ApplyPeaceAura(player->Skill(art_id), player->ID());
		break;
	case Arts::HEALING_AURA:
		ApplyHealingAura(player->Skill(art_id), player->ID());
		break;
	};
	
	return;
}

// Jared 6-05-00
// Pretty nasty, but it works if needed
/*
bool cArts::ReflectableArt(int art_id)
{
	if ((art_id == RESIST_FEAR) ||
		(art_id == PROTECTION) ||
		(art_id == FREE_ACTION) ||
		(art_id == JUDGEMENT) ||
		(art_id == IDENTIFY_CURSE) ||
		(art_id == VISION) ||
		(art_id == BLAST, ) ||
		(art_id == RESTORE) ||
		(art_id == PURIFY) ||
		(art_id == DRAIN_SELF) ||
		(art_id == ABJURE) ||
		(art_id == POISON) ||
		(art_id == ANTIDOTE) ||
		(art_id == CURSE) ||
		(art_id == SCARE) ||
		(art_id == STAGGER) ||
		(art_id == DEAFEN) ||
		(art_id == BLIND) ||
		(art_id == PARALYZE) ||
		(art_id == MIND_BLANK) ||
		(art_id == VAMPIRIC_DRAW) ||
		(art_id == SOUL_SHIELD))
		return true;
	else
		return false;
}
*/

//////////////////////////////////////////////////////////////////
// Firestorm

void cArts::Firestorm(void)
{	// blast away...
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::FIRESTORM,
			player->Skill(Arts::FIRESTORM), 0);
	this->ApplyFirestorm(player->Skill(Arts::FIRESTORM), player->ID());
	this->ArtFinished(true);
	return;
}

void cArts::ApplyFirestorm(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::FIRESTORM, false);
	cDS->PlaySound(LyraSound::FIRESTORM);
	if ((caster_id == player->ID()) || (gs && gs->Party() && gs->Party()->IsInParty(caster_id)))
	{ // by caster or caster's party - no damage
		LoadString (hInstance, IDS_AREA_EFFECT_PROTECTED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(Arts::FIRESTORM));
		display->DisplayMessage(message, false);
	}
	else // damage...
	{
#ifdef AGENT // Inform agents when they've been struck. Copied from cMissile
	((cAI*)player)->HasBeenStruck();
#endif
		LoadString (hInstance, IDS_AREA_EFFECT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::FIRESTORM));
		display->DisplayMessage(message, false);
		int damage = 8 + (((skill/10)+1) * (rand()%3));
		this->DamagePlayer(damage, caster_id);
	//	player->SetCurrStat(Stats::DREAMSOUL, -damage, SET_RELATIVE, caster_id);
	}
	return;
}

void cArts::Tempest (void)
{
  gs->SendPlayerMessage (0, RMsg_PlayerMsg::TEMPEST,
    player->Skill (Arts::TEMPEST), player->angle/4);
  this->ApplyTempest (player->Skill (Arts::TEMPEST), player->angle/4, player->ID ());
  this->ArtFinished (true);
  return;
}

void cArts::ApplyTempest (int skill, int angle, lyra_id_t caster_id)
{
	if ((caster_id == player->ID()) || (gs && gs->Party() && gs->Party()->IsInParty(caster_id)))
	{ // by caster or caster's party - no damage
		LoadString (hInstance, IDS_AREA_EFFECT_PROTECTED, disp_message, sizeof(disp_message));
	  _stprintf(message, disp_message, this->Descrip(Arts::TEMPEST));
		display->DisplayMessage(message, false);
	}
  else
  {
    MoveActor (player, angle*4, (PUSH_DISTANCE * ((1/3)*(skill/10)+1)), MOVE_NORMAL);
    LoadString (hInstance, IDS_TEMPEST_APPLIED, disp_message, sizeof(disp_message));
	  display->DisplayMessage (disp_message);
		player->PerformedAction();
  }
}

void cArts::Misdirection (void)
{
  gs->SendPlayerMessage (0, RMsg_PlayerMsg::MISDIRECTION,
    player->Skill (Arts::MISDIRECTION),0);
  this->ArtFinished (true);
  return;
}

void cArts::ApplyMisdirection (int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
  if (n == NO_ACTOR)
    return;
  else {
    // Only apply if I am facing the caster.
    // 0 - back
    // 1 - back left
    // 2 - front left
    // 3 - front
    // 4 - front right
    // 5 - back right
    int view = FixAngle((player->angle - n->angle)+32)/(Angle_360/Avatars::VIEWS);
    if (view < 2 || view > 4 || !n->Visible ()) {
      LoadString (hInstance, IDS_MISDIRECTION_FAILED, disp_message, sizeof(disp_message));
      _stprintf (message, disp_message, n->Name ());
      display->DisplayMessage (message);
      return;
    }
    player->angle = FixAngle (player->angle + Angle_180);
    LoadString (hInstance, IDS_MISDIRECTION_APPLIED, disp_message, sizeof(disp_message));
    _stprintf (message, disp_message, n->Name ());
    display->DisplayMessage (message);
  }
}

void cArts::ChaoticVortex (void)
{
  lyra_item_essence_t essence;
  bool hasEssence = false;
  cItem* item;
  for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION))
    {
      memcpy (&essence, item->Lmitem ().StateField (0), sizeof (essence));
      if ((essence.mare_type < Avatars::MIN_NIGHTMARE_TYPE) && (essence.strength > 0))
        hasEssence = true;
        break;
    }
  
  if (hasEssence) {
    item->Destroy ();
    gs->SendPlayerMessage (0, RMsg_PlayerMsg::CHAOTIC_VORTEX,
      player->Skill (Arts::CHAOTIC_VORTEX), 0);
    this->ApplyChaoticVortex (player->Skill (Arts::CHAOTIC_VORTEX), player->ID ());
  } else {
    LoadString (hInstance, IDS_CHAOTIC_VORTEX_NEED_DREAMER_ESSENCE, 
                message, sizeof(message));
    display->DisplayMessage (message);
  }

  this->ArtFinished (hasEssence);
}

void cArts::ApplyChaoticVortex (int skill, lyra_id_t caster_id)
{
  if ((caster_id == player->ID()) || (gs && gs->Party() && gs->Party()->IsInParty(caster_id)))
  {
    LoadString (hInstance, IDS_AREA_EFFECT_PROTECTED, disp_message, sizeof(disp_message));
	  _stprintf(message, disp_message, this->Descrip(Arts::CHAOTIC_VORTEX));
		display->DisplayMessage(message, false);
  } else {
	  player->EvokedFX().Activate(Arts::CHAOTIC_VORTEX, false);
	  cNeighbor *n = this->LookUpNeighbor(caster_id);
	  this->DisplayUsedByOther(n, Arts::CHAOTIC_VORTEX);
	  int duration = this->Duration(Arts::CHAOTIC_VORTEX, skill);
	  player->SetTimedEffect(LyraEffect::PLAYER_SPIN, duration, caster_id);
  }
	return;
}





//////////////////////////////////////////////////////////////////
// Razorwind

void cArts::Razorwind(void)
{
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::RAZORWIND,
			player->Skill(Arts::RAZORWIND), 0);
	this->ApplyRazorwind(player->Skill(Arts::RAZORWIND), player->ID());
	this->ArtFinished(true);
	return;
}

void cArts::ApplyRazorwind(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::RAZORWIND, false);
	cDS->PlaySound(LyraSound::RAZORWIND);

	if ((caster_id == player->ID()) || (gs && gs->Party() && gs->Party()->IsInParty(caster_id)))
	{ // by caster or caster's party - no damage
		LoadString (hInstance, IDS_AREA_EFFECT_PROTECTED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(Arts::RAZORWIND));
		display->DisplayMessage(message, false);
	}
	else
	{
		LoadString (hInstance, IDS_AREA_EFFECT, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(Arts::RAZORWIND));
		display->DisplayMessage(message, false);
		int damage = 12 + (((skill/10)+1) * (rand()%4));
		this->DamagePlayer(damage, caster_id);
		//player->SetCurrStat(Stats::DREAMSOUL, -damage, SET_RELATIVE, caster_id);
		int duration = this->Duration(Arts::RAZORWIND, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_BLEED, duration, caster_id);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Darkness

void cArts::Darkness(void)
{
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::DARKNESS,
			player->Skill(Arts::DARKNESS), 0);
	this->ApplyDarkness(player->Skill(Arts::DARKNESS), player->ID());
	this->ArtFinished(true);
	return;
}

void cArts::ApplyDarkness(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if ((caster_id == player->ID()) || (gs && gs->Party() &&
		gs->Party()->IsInParty(caster_id)) && (n != NO_ACTOR))
	{ // by caster or caster's party - no effect
		LoadString (hInstance, IDS_DARKNESS_PROTECTED, disp_message, sizeof(disp_message));
		if (caster_id != player->ID())
		_stprintf(message, disp_message, n->Name());
		else
		_stprintf(message, disp_message, player->Name());
		display->DisplayMessage(message);
	}
	else
	{
		player->EvokedFX().Activate(Arts::DARKNESS, false);
		if (n != NO_ACTOR)
		{
			LoadString (hInstance, IDS_DARKNESS, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
			display->DisplayMessage(message, false);
		}
		int duration = this->Duration(Arts::DARKNESS, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_BLIND, duration, caster_id);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Earthquake

void cArts::Earthquake(void)
{
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::EARTHQUAKE, player->Skill(Arts::EARTHQUAKE), 0);
	cDS->PlaySound(LyraSound::EARTHQUAKE, player->x, player->y, true);
	this->ApplyEarthquake(player->Skill(Arts::EARTHQUAKE), player->ID());
	this->ArtFinished(true);
	return;
}

void cArts::ApplyEarthquake(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if ((caster_id == player->ID()) || (gs && gs->Party() &&
		gs->Party()->IsInParty(caster_id)) && (n != NO_ACTOR))
	{ // by caster or caster's party - no effect
		LoadString (hInstance, IDS_EARTHQUAKE_PROTECTED, disp_message, sizeof(disp_message));
		if (caster_id != player->ID())
		_stprintf(message, disp_message, n->Name());
		else
		_stprintf(message, disp_message, player->Name());
		display->DisplayMessage(message);
	}
	else
	{
		player->EvokedFX().Activate(Arts::EARTHQUAKE, false);
		if (n != NO_ACTOR)
		{
			LoadString (hInstance, IDS_EARTHQUAKE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
			display->DisplayMessage(message, false);
		}
		int duration = this->Duration(Arts::EARTHQUAKE, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_DRUNK, duration, caster_id);
	}
	return;
}
//////////////////////////////////////////////////////////////////
// Hypnotic Weave

void cArts::HypnoticWeave(void)
{
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::HYPNOTIC_WEAVE,
			player->Skill(Arts::HYPNOTIC_WEAVE), 0);
	this->ApplyHypnoticWeave(player->Skill(Arts::HYPNOTIC_WEAVE), player->ID());
	this->ArtFinished(true);
	return;
}

void cArts::ApplyHypnoticWeave(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if ((caster_id == player->ID()) || ((n != NO_ACTOR) && !n->Visible()) ||
		(gs && gs->Party() && gs->Party()->IsInParty(caster_id)) && (n != NO_ACTOR))
	{ // by caster or caster's party - no effect
		LoadString (hInstance, IDS_HYPNOTIC_WEAVE_PROTECTED, disp_message, sizeof(disp_message));
		if (caster_id != player->ID())
		_stprintf(message, disp_message, n->Name());
		else
		_stprintf(message, disp_message, player->Name());
		display->DisplayMessage(message);
	}
	else
	{
		player->EvokedFX().Activate(Arts::HYPNOTIC_WEAVE, false);
		if (n != NO_ACTOR)
		{
			LoadString (hInstance, IDS_HYPNOTIC_WEAVE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
			display->DisplayMessage(message, false);
		}
		int duration = this->Duration(Arts::HYPNOTIC_WEAVE, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_PARALYZED, duration, caster_id);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Terror

void cArts::Terror(void)
{
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::TERROR, 
		player->Skill(Arts::TERROR), 0);
	this->ApplyTerror(player->Skill(Arts::TERROR), player->ID());
	this->ArtFinished(true);
	return;
}

void cArts::ApplyTerror(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	//if ((caster_id == player->ID()) || ((n != NO_ACTOR) && !n->Visible()) ||
	//	(gs && gs->Party() && gs->Party()->IsInParty(caster_id)) && (n != NO_ACTOR))
	bool terror = true;
	if (caster_id == player->ID())
		terror = false;

	if (n != NO_ACTOR) 
	{
		//if (!n->Visible()) terror = false;
		if ((gs && gs->Party() && gs->Party()->IsInParty(caster_id)))
			terror = false;
	}
	
	if (!terror)
	{ // by caster or caster's party - no effect
		LoadString (hInstance, IDS_TERROR_PROTECTED, disp_message, sizeof(disp_message));
		if ((caster_id != player->ID()) && (n != NO_ACTOR))
			_stprintf(message, disp_message, n->Name());
		else
			_stprintf(message, disp_message, player->Name());
		display->DisplayMessage(message);
	}
	else
	{
		player->EvokedFX().Activate(Arts::TERROR, false);
		if (n != NO_ACTOR)
		{
			LoadString (hInstance, IDS_TERROR, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, n->Name());
			display->DisplayMessage(message, false);
		}
		int duration = this->Duration(Arts::TERROR, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_FEAR, duration, caster_id);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// HealingAura

void cArts::HealingAura(void)
{
	// if we have party members, send the message
	if (gs && gs->Party() && gs->Party()->Members() > 0)
		gs->SendPlayerMessage(0, RMsg_PlayerMsg::HEALING_AURA,
			player->Skill(Arts::HEALING_AURA), 0);
	this->ApplyHealingAura(player->Skill(Arts::HEALING_AURA), player->ID());
	this->ArtFinished(true);
	return;
}

void cArts::ApplyHealingAura(int skill, lyra_id_t caster_id)
{
	int i = 1;
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	// applies only to caster or caster's party
	player->EvokedFX().Activate(Arts::HEALING_AURA, false);

	if (caster_id != player->ID() && n != NO_ACTOR) 
	{
		LoadString (hInstance, IDS_HEALING_AURA, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
	}
	else
	{
		LoadString (hInstance, IDS_HEALING_AURA_SELF, message, sizeof(disp_message));
	}
	display->DisplayMessage(message);

	// int healing = 8 + ((skill/10)+1)*(rand()%3);
	int healing = 4 + ((skill/10)+5)*(rand()%3+1);
	player->SetCurrStat(Stats::DREAMSOUL, healing, SET_RELATIVE, caster_id);

	return;
}

////////////////////////////////////////////////////////////////
// Locate Newlies (Newly Awakened)

void cArts::StartFindNewlies(void)
{
	if (fDoingNewlies)
		this->ArtFinished(false);

#if 0 // changed to be learnable by anyone
	if ((player->Skill(Arts::TRAIN) == 0) &&
		(!player->IsRuler(Guild::NO_GUILD)) &&
		(!player->IsKnight(Guild::NO_GUILD)))
	{
		LoadString (hInstance, IDS_TEACHER_OR_KNIGHT, message, sizeof(message));
		display->DisplayMessage(message);
		this->ArtFinished(false);
		return;
	}
#endif

	gs->SendPlayerMessage(0, RMsg_PlayerMsg::LOCATE_NEWLIES, 0, 0);

	fDoingNewlies = true;

	return;
}

void cArts::EndFindNewlies(void *value)
{
	fDoingNewlies = false;
	GMsg_LocateNewliesAck* newlies_msg = (GMsg_LocateNewliesAck*)value;

	if (newlies_msg->NumPlayers() == 0)
	{
		LoadString (hInstance, IDS_NO_NEWLIES, message, sizeof(message));
		display->DisplayMessage(message);
		this->ArtFinished(true);
		return;
	}
	LoadString (hInstance, IDS_NEWLIES, message, sizeof(message));
	display->DisplayMessage(message);

	for (int i=0; i<newlies_msg->NumPlayers(); i++) 
	{
		int level_id = newlies_msg->LevelID(i);
    if (0 == level_id)
			level_id = 1; // means they're in offline threshhold
    /**
     * Note -- this was removed because of innaccuracies reported by the PlayerDB
     *         The PlayerDB doesn't update room locations as often as leveld, and thus
     *         you can get false reporting if you output the room.
     *         This is also why "Locate All" doesn't contain any room information.
     **
		if (level_id == level->ID())
		{
			int room_id = newlies_msg->RoomID(i);

			LoadString (hInstance, IDS_NEWLY_SAMEPLANE, message, sizeof(message));
			_stprintf(disp_message, message, newlies_msg->PlayerName(i), 
				level->RoomName(room_id));		
		}
		else*/ if ((level_id > 0) && (level_id < MAX_LEVELS))
		{
			LoadString (hInstance, IDS_NEWLY, message, sizeof(message));
			_stprintf(disp_message, message, newlies_msg->PlayerName(i), 
				level->Name(level_id));		
		}
		else
		{
			LoadString (hInstance, IDS_NEWLY_PROTECTED, message, sizeof(message));
			_stprintf(disp_message, message, newlies_msg->PlayerName(i));

		}
		display->DisplayMessage(disp_message);
	}


	this->ArtFinished(true);

}

////////////////////////////////////////////////////////////////
// Locate Mares

void cArts::StartFindMares(void)
{
	if (fDoingMares)
		this->ArtFinished(false);

	gs->SendPlayerMessage(0, RMsg_PlayerMsg::LOCATE_MARES, 0, 0);

	fDoingMares = true;

	return;
}

void cArts::EndFindMares(void *value)
{
	fDoingMares = false;
	GMsg_LocateMaresAck* mares_msg = (GMsg_LocateMaresAck*)value;

	if (mares_msg->NumPlayers() == 0)
	{
		LoadString (hInstance, IDS_NO_MARES, message, sizeof(message));
		display->DisplayMessage(message);
		this->ArtFinished(true);
		return;
	}
	LoadString (hInstance, IDS_MARES, message, sizeof(message));
	display->DisplayMessage(message);

	for (int i=0; i<mares_msg->NumPlayers(); i++) 
	{
		int level_id = mares_msg->LevelID(i);
		if (0 == level_id)
			level_id = 1; // means they're in offline threshhold
		/**
     * Note -- this was removed because of innaccuracies reported by the PlayerDB
     *         The PlayerDB doesn't update room locations as often as leveld, and thus
     *         you can get false reporting if you output the room.
     *         This is also why "Locate All" doesn't contain any room information.
     **
      
    if (level_id == level->ID())
		{
			int room_id = mares_msg->RoomID(i);

			LoadString (hInstance, IDS_NEWLY_SAMEPLANE, message, sizeof(message));
			_stprintf(disp_message, message, mares_msg->PlayerName(i), 
				level->RoomName(room_id));		
		}
		else */if ((level_id > 0) && (level_id < MAX_LEVELS))
		{
			LoadString (hInstance, IDS_NEWLY, message, sizeof(message));
			_stprintf(disp_message, message, mares_msg->PlayerName(i), 
				level->Name(level_id));		
		}
		else
		{
			LoadString (hInstance, IDS_NEWLY_PROTECTED, message, sizeof(message));
			_stprintf(disp_message, message, mares_msg->PlayerName(i));

		}
		display->DisplayMessage(disp_message);
	}


	this->ArtFinished(true);

}

//////////////////////////////////////////////////////////////////
// Radiant Blaze

const int RADIANT_BLAZE_POWER_TOKENS = 1;
void cArts::RadiantBlaze(void)
{
	// only works for POR

	if (!(player->GuildRank(Guild::RADIANCE) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER, message, sizeof(message));
		_stprintf(disp_message, message, GuildName(Guild::RADIANCE), arts->Descrip(Arts::RADIANT_BLAZE));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::RADIANCE);

	if (num_tokens < RADIANT_BLAZE_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, RADIANT_BLAZE_POWER_TOKENS, GuildName(Guild::RADIANCE), arts->Descrip(Arts::RADIANT_BLAZE));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}
	
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::RADIANT_BLAZE,
			player->Skill(Arts::RADIANT_BLAZE), 0);
	this->ApplyRadiantBlaze(player->Skill(Arts::RADIANT_BLAZE), player->ID());
	
	for (int i=0; i<RADIANT_BLAZE_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
	return;

}

void cArts::ApplyRadiantBlaze(int skill, lyra_id_t caster_id)
{
	if (player->GuildRank(Guild::RADIANCE) >= Guild::INITIATE)
	{	// no damage to POR
		LoadString (hInstance, IDS_AREA_EFFECT_PROTECTED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::RADIANT_BLAZE));
		display->DisplayMessage(message, false);
	}
	else
	{
		LoadString (hInstance, IDS_AREA_EFFECT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::RADIANT_BLAZE));
		display->DisplayMessage(message, false);
		int damage = 1 + (rand()%6);
		this->DamagePlayer(damage, caster_id);
		int duration = this->Duration(Arts::RADIANT_BLAZE, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_BLIND, duration, caster_id);
	}
	return;

}


//////////////////////////////////////////////////////////////////
// Poison Cloud

const int POISON_CLOUD_POWER_TOKENS = 1;
void cArts::PoisonCloud(void)
{
	// only works for HC
	if (!(player->GuildRank(Guild::CALENTURE) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER, message, sizeof(message));
		_stprintf(disp_message, message, GuildName(Guild::CALENTURE), arts->Descrip(Arts::POISON_CLOUD));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::CALENTURE);

	if (num_tokens < POISON_CLOUD_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, POISON_CLOUD_POWER_TOKENS, GuildName(Guild::CALENTURE), arts->Descrip(Arts::POISON_CLOUD));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}
	
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::POISON_CLOUD,
			player->Skill(Arts::POISON_CLOUD), 0);
	this->ApplyPoisonCloud(player->Skill(Arts::POISON_CLOUD), player->ID());
	
	for (int i=0; i<POISON_CLOUD_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
	return;

}

void cArts::ApplyPoisonCloud(int skill, lyra_id_t caster_id)
{
	if (player->GuildRank(Guild::CALENTURE) >= Guild::INITIATE)
	{	// no damage to HC
		LoadString (hInstance, IDS_AREA_EFFECT_PROTECTED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::POISON_CLOUD));
		display->DisplayMessage(message, false);
	}
	else
	{
		LoadString (hInstance, IDS_AREA_EFFECT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::POISON_CLOUD));
		display->DisplayMessage(message, false);
		int damage = 1 + (rand()%6);
		this->DamagePlayer(damage, caster_id);
		int duration = this->Duration(Arts::POISON_CLOUD, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_POISONED, duration, caster_id);
	}
	return;

}


//////////////////////////////////////////////////////////////////
// Dazzle

const int DAZZLE_POWER_TOKENS = 1;
void cArts::Dazzle(void)
{
	// only works for HC
	if (!(player->GuildRank(Guild::LIGHT) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER, message, sizeof(message));
		_stprintf(disp_message, message, GuildName(Guild::LIGHT), arts->Descrip(Arts::DAZZLE));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::LIGHT);

	if (num_tokens < DAZZLE_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, DAZZLE_POWER_TOKENS, GuildName(Guild::LIGHT), arts->Descrip(Arts::DAZZLE));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}
	
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::DAZZLE,
			player->Skill(Arts::DAZZLE), 0);
	this->ApplyDazzle(player->Skill(Arts::DAZZLE), player->ID());
	
	for (int i=0; i<DAZZLE_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
	return;

}

void cArts::ApplyDazzle(int skill, lyra_id_t caster_id)
{
	if (player->GuildRank(Guild::LIGHT) >= Guild::INITIATE)
	{	// no damage to DOL
		LoadString (hInstance, IDS_AREA_EFFECT_PROTECTED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::DAZZLE));
		display->DisplayMessage(message, false);
	}
	else
	{
		LoadString (hInstance, IDS_AREA_EFFECT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::DAZZLE));
		display->DisplayMessage(message, false);
		int damage = 1 + (rand()%6);
		this->DamagePlayer(damage, caster_id);
		int duration = this->Duration(Arts::DAZZLE, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_DRUNK, duration, caster_id);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Guild House

void cArts::GuildHouse(void)
{  
	LoadString (hInstance, IDS_USE_GUILD_HOUSE, message, sizeof(message));
	display->DisplayMessage(message);

	switch (player->FocusStat())
	{
		case Stats::WILLPOWER:
			player->Teleport(-850,-3556,0,14);
			break;

		case Stats::INSIGHT:
			player->Teleport(-10566,4336,0,3);
			break;

		case Stats::RESILIENCE:
			player->Teleport(8177,8235,0,7);
			break;

		case Stats::LUCIDITY:
			player->Teleport(-1738,-1548,0,29);
			break;
	}
	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Tehthu's Oblivion

void cArts::TehthusOblivion(void)
{  
	cNeighbor *n;
	cItem *i;
	bool tehthu = false;

	// Tehthu must be here and in soulsphere form.
	for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
	{
		if ((n->ID() == 711) && (n->flags & ACTOR_SOULSPHERE))
			tehthu = true;
	}
	actors->IterateNeighbors(DONE);

	if (!tehthu)
	{
		LoadString (hInstance, IDS_NO_TEHTHU, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message, false);
		this->ArtFinished(false);
		return;
	}

	// must have appropriate elemental blade in pack
	bool blade = false;
	for (i = actors->IterateItems(INIT); i != NO_ACTOR; i = actors->IterateItems(NEXT))
	{
		if (i->Status() == ITEM_OWNED)
		{
			lyra_id_t id = i->ID().Serial(); //i->Lmitem().S;
			TCHAR* name = i->Name();
			if ((player->FocusStat() == Stats::WILLPOWER) &&
				(id == 7308306))
				blade = true;
			if ((player->FocusStat() == Stats::INSIGHT) &&
				(id ==  554114))
				blade = true;
			if ((player->FocusStat() == Stats::RESILIENCE) &&
				(id == 7308305))
				blade = true;
			if ((player->FocusStat() == Stats::LUCIDITY) &&
				(id == 7308307))
				blade = true;
		}
	}
	actors->IterateItems(DONE);

	if (!blade)
	{
		LoadString (hInstance, IDS_NO_BLADE, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message, false);
		this->ArtFinished(false);
		return;
	}


	gs->SendPlayerMessage(711, RMsg_PlayerMsg::TEHTHUS_OBLIVION, 0, 0);
	LoadString (hInstance, IDS_KILLED_TEHTHU, message, sizeof(message));
	display->DisplayMessage(message);

	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Forge Talisman

void cArts::StartForgeTalisman(void)
{
	if ((!itemdlg) && (options.network))
	{
		itemdlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CREATE_ITEM,  cDD->Hwnd_Main(), (DLGPROC)CreateItemDlgProc);
		this->WaitForDialog(hDlg, Arts::FORGE_TALISMAN);
		SendMessage(hDlg, WM_INIT_ITEMCREATOR, 0, (LPARAM)CreateItem::FORGE_ITEM);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
	}
	else
		this->CancelArt();

	return;
}

void cArts::EndForgeTalisman(void *value, bool usePT)
{
	int success = *((int*)value);
	if (success)
	{
		cDS->PlaySound(LyraSound::FORGE, player->x, player->y, true);
		if (usePT)
		{
			cItem* power_tokens[Lyra::INVENTORY_MAX];
			int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::NO_GUILD);
			if (num_tokens)
				power_tokens[0]->Destroy();
		}

		this->ArtFinished(true);
	}
	else
		this->ArtFinished(false);

	return;
}

////////////////////////////////////////////////////////////////
// Sense Dreamers

void cArts::StartSenseDreamers(void)
{
	if (fDoingSense)
		this->ArtFinished(false);

	gs->SendPlayerMessage(0, RMsg_PlayerMsg::SENSE_DREAMERS, 0, 0);

	fDoingSense = true;

	return;
}

void cArts::EndSenseDreamers(void *value)
{
	fDoingSense = false;
	GMsg_SenseDreamersAck* psense_msg = (GMsg_SenseDreamersAck*)value;

	if (psense_msg->LevelID(0) == 0)
	{
		LoadString (hInstance, IDS_EMPTY, message, sizeof(message));
		display->DisplayMessage(message);
		this->ArtFinished(true);
		return;
	}

	const UINT SenseDreamerIDs[] = {IDS_SENSE_DREAMERS1,IDS_SENSE_DREAMERS2,IDS_SENSE_DREAMERS3};
	memset(message, '\0', sizeof(message));
	//_stprintf(message, _T(""));

	// now manipulate zeroes 
	for (int i=0; i<PLANES_SENSED_COUNT-1; i++) {
		if (psense_msg->LevelID(i) != 0){
			LoadString (hInstance, SenseDreamerIDs[i], disp_message, sizeof(disp_message));
			_stprintf(temp_message, disp_message, level->Name(psense_msg->LevelID(i)));
			_tcscat(message, temp_message);
		//if (psense_msg->LevelID(i) == 0){
		//	psense_msg->SetLevelID(psense_msg->LevelID(i-1), i);
		}
	}
	display->DisplayMessage(message);

	this->ArtFinished(true);

}



////////////////////////////////////////////////////////////////
// *** Arts that require selecting a neighbor ***
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Join Party

void cArts::StartJoin()
{
	if (player->flags & ACTOR_NO_PARTY)
	{	
		LoadString (hInstance, IDS_NO_JP, message, sizeof(message));
		display->DisplayMessage(message); 
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::EndJoin, Arts::JOIN_PARTY);
	this->CaptureCP(NEIGHBORS_TAB, Arts::JOIN_PARTY);
	return;
}

// perform the join party action if a neighbor is still around
void cArts::EndJoin(void)
{
	cNeighbor *n;
	if (((n = cp->SelectedNeighbor()) != NO_ACTOR) && options.network &&
		gs->Party() && (n->ID() != Lyra::ID_UNKNOWN))
		{
			gs->Party()->JoinParty(n->ID(), n->Name());
			this->ArtFinished(true);
		}
	else
		this->ArtFinished(false);

	return;
}

//////////////////////////////////////////////////////////////////
// Resist Fear

void cArts::StartResistFear(void)
{
	this->WaitForSelection(&cArts::EndResistFear, Arts::RESIST_FEAR);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::RESIST_FEAR);
	return;
}

void cArts::ApplyResistFear(int skill, lyra_id_t caster_id)
{
	 player->EvokedFX().Activate(Arts::RESIST_FEAR, false);

	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::RESIST_FEAR);
	int duration = this->Duration(Arts::RESIST_FEAR, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_PROT_FEAR, duration, caster_id);
	return;
}


void cArts::EndResistFear(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::RESIST_FEAR);
		this->ArtFinished(false);
		return;
	}
	else if (n->ID() == player->ID())
		this->ApplyResistFear(player->Skill(Arts::RESIST_FEAR), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::RESIST_FEAR,
			player->Skill(Arts::RESIST_FEAR), 0);
	this->DisplayUsedOnOther(n, Arts::RESIST_FEAR);
	cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Protection (Resist Curse)

void cArts::StartResistCurse(void)
{
	this->WaitForSelection(&cArts::EndResistCurse, Arts::PROTECTION);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, art_in_use);
	return;
}

void cArts::ApplyResistCurse(int art_id, int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(art_id, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, art_id);
	int duration = this->Duration(art_id, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_PROT_CURSE, duration, caster_id);
	return;
}

void cArts::EndResistCurse(void)
{
	int playermsg_type = RMsg_PlayerMsg::RESIST_CURSE;
	if (art_in_use == Arts::SANCTIFY)
		playermsg_type = RMsg_PlayerMsg::SANCTIFY;
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(art_in_use);
		this->ArtFinished(false);
		return;
	}
	else if (n->ID() == player->ID())
		this->ApplyResistCurse(art_in_use, player->Skill(art_in_use), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), playermsg_type,
			player->Skill(art_in_use), 0);
	this->DisplayUsedOnOther(n, art_in_use);
	cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);

	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Free Action (Resist Paralysis)

void cArts::StartResistParalysis(void)
{
	this->WaitForSelection(&cArts::EndResistParalysis, Arts::FREE_ACTION);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::FREE_ACTION);
	return;
}

void cArts::ApplyResistParalysis(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::FREE_ACTION, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::FREE_ACTION);
	int duration = this->Duration(Arts::FREE_ACTION, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_PROT_PARALYSIS, duration, caster_id);

	return;
}

void cArts::EndResistParalysis(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::FREE_ACTION);
		this->ArtFinished(false);
		return;
	}
	else if (n->ID() == player->ID())
		this->ApplyResistParalysis(player->Skill(Arts::FREE_ACTION), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::RESIST_PARALYSIS,
			player->Skill(Arts::FREE_ACTION), 0);
	this->DisplayUsedOnOther(n, Arts::FREE_ACTION);
	cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Identify Curse

void cArts::StartIdentifyCurse(void)
{
	this->WaitForSelection(&cArts::EndIdentifyCurse, Arts::IDENTIFY_CURSE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::IDENTIFY_CURSE);
	return;
}

void cArts::ApplyIdentifyCurse(int skill, lyra_id_t caster_id)
{
	int random;

	 player->EvokedFX().Activate(Arts::IDENTIFY_CURSE, false);

	cNeighbor *n = this->LookUpNeighbor(caster_id);
	random = rand()%100;

	if (random > skill) // chance of detection = 100 - skill level
	{
		this->DisplayUsedByOther(n, Arts::IDENTIFY_CURSE);
	}

	if (player->flags & ACTOR_CURSED)
		LoadString (hInstance, IDS_CURSED, disp_message, sizeof(disp_message));
	else
		LoadString (hInstance, IDS_NOT_CURSED, disp_message, sizeof(disp_message));

	_stprintf(message, disp_message, player->Name());
	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);

	return;
}

void cArts::EndIdentifyCurse(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::IDENTIFY_CURSE);
		this->ArtFinished(false);
		return;
	}
	else
	{
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::IDENTIFY_CURSE,
			player->Skill(Arts::IDENTIFY_CURSE), 0);
	}
	cDS->PlaySound(LyraSound::ID_CURSE, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Vision (Detect Invisible)

void cArts::StartVision(void)
{
	this->WaitForSelection(&cArts::EndVision, Arts::VISION);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::VISION);
	return;
}

void cArts::ApplyVision(int skill, lyra_id_t caster_id)
{
	 player->EvokedFX().Activate(Arts::VISION, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);

	this->DisplayUsedByOther(n, Arts::VISION);
	int duration = this->Duration(Arts::VISION, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_DETECT_INVISIBLE, duration, caster_id);
	return;
}

void cArts::EndVision(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::VISION);
		this->ArtFinished(false);
		return;
	}

	else if (n->ID() == player->ID())
		this->ApplyVision(player->Skill(Arts::VISION), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::VISION,
			player->Skill(Arts::VISION), 0);
	this->DisplayUsedOnOther(n, Arts::VISION);
	cDS->PlaySound(LyraSound::VISION, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Purify

void cArts::StartPurify(void)
{
	this->WaitForSelection(&cArts::EndPurify, art_in_use);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, art_in_use);
	return;
}

void cArts::ApplyPurify(int art_id, int skill, lyra_id_t caster_id)
{
	 player->EvokedFX().Activate(art_id, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, art_id);
	if (player->flags & ACTOR_CURSED)
		player->RemoveTimedEffect(LyraEffect::PLAYER_CURSED);
	return;
}

void cArts::EndPurify(void)
{
	int playermsg_type = RMsg_PlayerMsg::PURIFY;
	if (art_in_use == Arts::REMOVE_CURSE)
		playermsg_type = RMsg_PlayerMsg::REMOVE_CURSE;

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(art_in_use);
		this->ArtFinished(false);
		return;
	}
	else if (n->ID() == player->ID())
		this->ApplyPurify(art_in_use, player->Skill(art_in_use), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), playermsg_type,
			player->Skill(art_in_use), 0);
	this->DisplayUsedOnOther(n, art_in_use);
	cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Antidote

void cArts::StartAntidote(void)
{
	this->WaitForSelection(&cArts::EndAntidote, Arts::ANTIDOTE);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::ANTIDOTE);
	return;
}

void cArts::ApplyAntidote(int skill, lyra_id_t caster_id)
{
	 player->EvokedFX().Activate(Arts::ANTIDOTE, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n,Arts::ANTIDOTE );
	if (player->flags & ACTOR_POISONED)
		player->RemoveTimedEffect(LyraEffect::PLAYER_POISONED);
	return;
}

void cArts::EndAntidote(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::ANTIDOTE);
		this->ArtFinished(false);
		return;
	}
	else if (n->ID() == player->ID())
		this->ApplyAntidote(player->Skill(Arts::ANTIDOTE), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::ANTIDOTE,
			player->Skill(Arts::ANTIDOTE), 0);
	cDS->PlaySound(LyraSound::PROTECT_AVATAR, player->x, player->y, true);
	this->DisplayUsedOnOther(n, Arts::ANTIDOTE);
	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Restore

void cArts::StartRestore(void)
{
	this->WaitForSelection(&cArts::EndRestore, art_in_use);
	this->AddDummyNeighbor();
	
	this->CaptureCP(NEIGHBORS_TAB, art_in_use);
	return;
}

void cArts::ApplyRestore(int art_id, int skill, lyra_id_t caster_id)
{
	restore_skill = skill;
	restore_art = art_id;
	int i = 1;
	if ((caster_id == player->ID()) || (player->CurrStat(Stats::DREAMSOUL)))
		this->GotRestored(&i);
	else if (!acceptrejectdlg)
	{
		cNeighbor *n = this->LookUpNeighbor(caster_id);
		if (n != NO_ACTOR)
		{
			restore_id = caster_id;
			LoadString (hInstance, IDS_QUERY_RESTORE, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, n->Name());
			HWND hDlg = CreateLyraDialog(hInstance, (IDD_ACCEPTREJECT),
							cDD->Hwnd_Main(), (DLGPROC)AcceptRejectDlgProc);
			acceptreject_callback = (&cArts::GotRestored);
			SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
			SendMessage(hDlg, WM_SET_AR_NEIGHBOR, 0, (LPARAM)n);
		}
	}
	return;
}

void cArts::GotRestored(void *value)
{
	int success = *((int*)value);
	if (success)
	{
		player->EvokedFX().Activate(restore_art, false);
		cNeighbor *n = this->LookUpNeighbor(restore_id);
		this->DisplayUsedByOther(n,restore_art);
		int healing = ((restore_skill/10)+1+(rand()%8));
		player->SetCurrStat(Stats::DREAMSOUL, healing, SET_RELATIVE, restore_id);
	}
	restore_id = 0;
	return;
}


void cArts::EndRestore(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	int playermsg_type = RMsg_PlayerMsg::RESTORE;
	if (art_in_use == Arts::HEAL)
		playermsg_type = RMsg_PlayerMsg::HEAL;
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(art_in_use);
		this->ArtFinished(false);
		return;
	}
	else if (n->ID() == player->ID()) {
		cDS->PlaySound(LyraSound::RESTORE, player->x, player->y, true);
		this->ApplyRestore(art_in_use, player->Skill(art_in_use), player->ID());
	}
	else
	{
		cDS->PlaySound(LyraSound::RESTORE, player->x, player->y, true);
		gs->SendPlayerMessage(n->ID(), playermsg_type,
			player->Skill(art_in_use), 0);
	}
	this->DisplayUsedOnOther(n, art_in_use);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Roger Wilco/Telepathy
//

void cArts::StartRogerWilco(void)
{
	if (!options.rw)
	{
		LoadString(hInstance, IDS_NOVOICE, message, sizeof(message));
		display->DisplayMessage(message, true);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::EndRogerWilco, Arts::ROGER_WILCO);
	this->CaptureCP(NEIGHBORS_TAB, Arts::ROGER_WILCO);
	return;
}

void cArts::QueryRogerWilco(lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (!acceptrejectdlg && options.rw) //&& (rogerwilco_id == 0))
	{
		if (n != NO_ACTOR)
		{
			rogerwilco_id = caster_id;
			LoadString (hInstance, IDS_QUERY_ROGER_WILCO, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
			HWND hDlg = CreateLyraDialog(hInstance, (IDD_ACCEPTREJECT),
							cDD->Hwnd_Main(), (DLGPROC)AcceptRejectDlgProc);
			acceptreject_callback = (&cArts::RogerWilco);
			SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
			SendMessage(hDlg, WM_SET_AR_NEIGHBOR, 0, (LPARAM)n);
		}
	}
	else // reject player
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::ROGER_WILCO,
			2, 2);
	return;
}

void cArts::RogerWilco(void *value)
{
#ifdef ROGER_WILCO
	int success = *((int*)value);
	cNeighbor *n = this->LookUpNeighbor(rogerwilco_id);
	rogerwilco_id = 0;
	TCHAR host[64];
	//_stprintf(host, _T("%s"), inet_ntoa(gs->LocalIP()));
	if (success)
	{	// we have accepted telepathy - set up a host if needed, and send an ack
		if (channel == RWNET_INVALIDCHANNEL)
		{ // we need to set up a host
			if (!JoinChannel(cDD->Hwnd_Main(), host, _T("\0"), _T(""), player->Name()))
			{
				gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::ROGER_WILCO,
					2, 2);
				LoadString(hInstance, IDS_RWHOST_ERROR, message, sizeof(message));
				display->DisplayMessage(message, true);
				this->ArtFinished(false);
				return;
			}
		}
		gs->Talk(host, RMsg_Speech::TELL_IP, n->ID(), false, false);
#ifndef AGENT //RW_ENABLED
		//RWVoice_SetNetSendProc( voicePipeline, RWNet_Send, channel );
#endif
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::ROGER_WILCO,
			2, 1);
		LoadString(hInstance, IDS_RWSUCCESS, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name());
		display->DisplayMessage(message, true);
		return;
	}
	else // reject player
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::ROGER_WILCO,
			2, 2);
#endif // ROGER_WILCO
	return;
}

void cArts::RogerWilcoAck(int success)
{
#ifdef ROGER_WILCO
	cNeighbor *n = this->LookUpNeighbor(rogerwilco_id);
	if ((success == 1) && JoinChannel(cDD->Hwnd_Main(), gs->RWHost(), _T("\0"), _T(""), player->Name())) // successfully joined channel
	{
		LoadString(hInstance, IDS_RWSUCCESS, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name());
		display->DisplayMessage(message, true);
	}
	else
	{
		LoadString(hInstance, IDS_RWREJECT, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name());
		display->DisplayMessage(message, true);
	}
#endif // ROGER_WILCO
}


void cArts::EndRogerWilco(void)
{		
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::ROGER_WILCO);
		this->ArtFinished(false);
		return;
	}
	else
	{	
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::ROGER_WILCO,
			1, 0);
		rogerwilco_id = n->ID();
	}
	this->ArtFinished(true);
	return;
}



//////////////////////////////////////////////////////////////////
// Drain Self

void cArts::StartDrainSelf(void)
{
	this->WaitForSelection(&cArts::EndDrainSelf, Arts::DRAIN_SELF);
	this->CaptureCP(NEIGHBORS_TAB, Arts::DRAIN_SELF);
	return;

}

void cArts::ApplyDrainSelf(int stat, int amount, lyra_id_t caster_id)
{
	 player->EvokedFX().Activate(Arts::DRAIN_SELF, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::DRAIN_SELF);
#ifndef AGENT
	if ( player->IsMare() && // dont allow drain-self DS if collapsed
		 (stat == 0 && player->CurrStat(stat) == 0) && (n!= NO_ACTOR))
	{
		LoadString (hInstance, IDS_REFLECTS_BACK, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, player->Name());
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::DRAIN_SELF, stat, amount);
		return;
	}
#endif
	player->SetCurrStat(stat, amount, SET_RELATIVE, caster_id);
	return;
}

void cArts::EndDrainSelf(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::DRAIN_SELF);
		this->ArtFinished(false);
		return;
	}
	else if ((n->flags & ACTOR_SOULSPHERE) && 
		     (player->SelectedStat() == Stats::DREAMSOUL))
	{ // no force restore
		LoadString (hInstance, IDS_NO_FORCE_RESTORE, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
	else
	{
		int amount = ((player->SkillSphere(Arts::DRAIN_SELF)))+1;
		if (amount > player->CurrStat(player->SelectedStat()))
			amount = player->CurrStat(player->SelectedStat());
		player->SetCurrStat(player->SelectedStat(), -amount, SET_RELATIVE, player->ID());
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::DRAIN_SELF,
			player->SelectedStat(), amount);
		this->DisplayUsedOnOther(n, Arts::DRAIN_SELF);
	}
	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Scare (Cause Fear)

void cArts::StartScare(void)
{
	this->WaitForSelection(&cArts::EndScare, Arts::SCARE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::SCARE);
	return;
}

void cArts::ApplyScare(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::SCARE, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::SCARE);
	int duration = this->Duration(Arts::SCARE, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_FEAR, duration, caster_id);
	return;
}

void cArts::EndScare(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SCARE);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::SCARE,
		player->Skill(Arts::SCARE), 0);
	this->DisplayUsedOnOther(n, Arts::SCARE);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Curse

void cArts::StartCurse(void)
{
	this->WaitForSelection(&cArts::EndCurse, Arts::CURSE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::CURSE);
	return;
}

void cArts::ApplyCurse(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate( Arts::CURSE, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::CURSE);
	int duration = this->Duration(Arts::CURSE, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_CURSED, duration, caster_id);
	return;
}

void cArts::EndCurse(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::CURSE);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::CURSE,
			player->Skill(Arts::CURSE), 0);
	this->DisplayUsedOnOther(n, Arts::CURSE);
	cDS->PlaySound(LyraSound::CURSE, player->x, player->y);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Paralyze

void cArts::StartParalyze(void)
{
	this->WaitForSelection(&cArts::EndParalyze, art_in_use);
	this->CaptureCP(NEIGHBORS_TAB, art_in_use);
	return;
}

void cArts::ApplyParalyze(int art_id, int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(art_id, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, art_id);
	int duration = this->Duration(art_id, skill);
	// Z2 Change: Balthiir requested this change to paralyze duration
	duration += 1000; // r->ErrorInfo()->RHack to make Para duration 2 seconds + 1 second/plat
	player->SetTimedEffect(LyraEffect::PLAYER_PARALYZED, duration, caster_id);
	return;
}

void cArts::EndParalyze(void)
{
	int playermsg_type = RMsg_PlayerMsg::PARALYZE;
	if (art_in_use == Arts::HOLD_AVATAR)
		playermsg_type = RMsg_PlayerMsg::HOLD_AVATAR;

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(art_in_use);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), playermsg_type,
		player->Skill(art_in_use), 0);
	this->DisplayUsedOnOther(n, art_in_use);
	cDS->PlaySound(LyraSound::PARALYZE, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Stagger

void cArts::StartStagger(void)
{
	this->WaitForSelection(&cArts::EndStagger, Arts::STAGGER);
	this->CaptureCP(NEIGHBORS_TAB, Arts::STAGGER);
	return;
}

void cArts::ApplyStagger(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::STAGGER, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::STAGGER);
	int duration = this->Duration(Arts::STAGGER, skill);
	if (player->SetTimedEffect(LyraEffect::PLAYER_DRUNK, duration, caster_id))
		cDS->PlaySound(LyraSound::STAGGER);
	return;
}

void cArts::EndStagger(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::STAGGER);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::STAGGER,
		player->Skill(Arts::STAGGER), 0);
	this->DisplayUsedOnOther(n, Arts::STAGGER);
	cDS->PlaySound(LyraSound::STAGGER, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Deafen

void cArts::StartDeafen(void)
{
	this->WaitForSelection(&cArts::EndDeafen, Arts::DEAFEN);
	this->CaptureCP(NEIGHBORS_TAB, Arts::DEAFEN);
	return;
}

// roar is true if deafness is caused by a monster's roar
void cArts::ApplyDeafen(int skill, lyra_id_t caster_id, bool roar)
{
	player->EvokedFX().Activate(Arts::DEAFEN, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (roar)
	{
		LoadString (hInstance, IDS_DEAFENING_ROAR, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message, false);
	}
	else
		this->DisplayUsedByOther(n, Arts::DEAFEN);

	int duration = this->Duration(Arts::DEAFEN, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_DEAF, duration, caster_id);
	return;
}

void cArts::EndDeafen(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::DEAFEN);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::DEAFEN,
		player->Skill(Arts::DEAFEN), 0);
	this->DisplayUsedOnOther(n, Arts::DEAFEN);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Blind

void cArts::StartBlind(void)
{
	this->WaitForSelection(&cArts::EndBlind, Arts::BLIND);
	this->CaptureCP(NEIGHBORS_TAB, Arts::BLIND);
	return;
}

void cArts::ApplyBlind(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::BLIND, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::BLIND);
	int duration = this->Duration(Arts::BLIND, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_BLIND, duration, caster_id);
	return;
}

void cArts::EndBlind(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::BLIND);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::BLIND,
		player->Skill(Arts::BLIND), 0);
	this->DisplayUsedOnOther(n, Arts::BLIND);
	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Poison

void cArts::StartPoison(void)
{
	this->WaitForSelection(&cArts::EndPoison, Arts::POISON);
	this->CaptureCP(NEIGHBORS_TAB, Arts::POISON);
	return;
}

void cArts::ApplyPoison(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::POISON, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::POISON);
	int duration = this->Duration(Arts::POISON, skill);
	//if (player->poison_strength < skill)
	//	player->poison_strength = skill;
	player->SetTimedEffect(LyraEffect::PLAYER_POISONED, duration, caster_id);
	return;
}

void cArts::EndPoison(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::POISON);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::POISON,
		player->Skill(Arts::POISON), 0);
	this->DisplayUsedOnOther(n, Arts::POISON);
	this->ArtFinished(true);
	cDS->PlaySound(LyraSound::POISON, player->x, player->y, true);
	return;
}


//////////////////////////////////////////////////////////////////
// Blast

void cArts::StartBlast(void)
{
	this->WaitForSelection(&cArts::EndBlast, Arts::BLAST);
	this->CaptureCP(NEIGHBORS_TAB, Arts::BLAST);
	return;
}

void cArts::ApplyBlast(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::BLAST, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (!n && (caster_id != REFLECT_ID))
		// no Neighbor and not Blasting self (ala reflect)
		return;

	// send blast ack to allow further blasts on this target
	if (caster_id != REFLECT_ID) 
		// no need to ack our own reflected blasts
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::BLAST_ACK, 0, 0);

	this->DisplayUsedByOther(n, Arts::BLAST);
	int damage = (((skill/10)+1) + (rand()%4));

#if (defined (PMARE) || defined (GAMEMASTER))
#if (defined (GAMEMASTER))
  if (player->GetAccountType() == LmAvatar::ACCT_DARKMARE)
#endif (defined (GAMEMASTER))
  { 
	  damage = (int)((float)damage * .85);
    int random;
    static bool blastlog = false;
    if (damage > 1)
    {
      if ((rand () % 100) > 75) { // 25% chance of weakning...
        random = (rand () % (damage/2)) + 1; // could weaken as much as half the blast
        damage -= random; 
      }
    } else if (damage == 1) { // *** WARNING: Very weak blast
      if ((rand () % 100) > 90)
      {
        random = rand () % damage;
        damage -= (rand () % damage) + 1;
      }
    }
  }
#endif (defined (PMARE) || defined (GAMEMASTER))

#ifdef AGENT // powerful monsters are immune to blast

	if (player->AvatarType() >= Avatars::SHAMBLIX)
		return;
#endif
//#endif 0	// MKET - Game designer choice

	this->DamagePlayer(damage, caster_id);

	return;
}

void cArts::DamagePlayer(int damage, lyra_id_t caster_id) 
{
	if (!(player->flags & ACTOR_SOULSPHERE))
	{
		if (player->flags & ACTOR_TRANSFORMED)
			Scream(player->GetTransformedMonsterType(), player, true);
		else
			Scream(player->Avatar().AvatarType(), player, true);

		player->SetInjured(true);
		player->SetCurrStat(Stats::DREAMSOUL, -damage, SET_RELATIVE, caster_id);
	}
}


void cArts::EndBlast(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::BLAST);
		this->ArtFinished(false);
		return;
	}
	else if (n->Blasting())
	{
		LoadString (hInstance, IDS_NO_CHAIN_BLAST, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name());
		display->DisplayMessage(message);
		//this->ArtFinished(false);
		this->StartBlast();
		return;
	}
	else
	{
		// must be visible to site of vulnerability to effect blast

// RRR 02/12/99
// If a player is blind, they cannot possibly use arts that require Line of Sight. 
// Added (ACTOR_BLINDED)
		if (n->IsVulnerable() && !(player->flags & ACTOR_BLINDED) && 
			(n->Room() == player->Room())) 
		{
			n->SetBlasting(true);
			gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::BLAST, player->Skill(Arts::BLAST), 0);
			this->DisplayUsedOnOther(n, Arts::BLAST);
			cDS->PlaySound(LyraSound::BLAST, player->x, player->y, true);
			this->ArtFinished(true);
		}
		else
		{ // display message
			LoadString (hInstance, IDS_NEED_VISIBLE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::BLAST));
			display->DisplayMessage(message);
			this->ArtFinished(true);
		}
	}
	return;
}

void cArts::StartKinesis (void)
{
	this->WaitForSelection(&cArts::EndKinesis, Arts::KINESIS);
	this->CaptureCP(NEIGHBORS_TAB, Arts::KINESIS);
	return;
}

void cArts::ApplyKinesis (int skill, lyra_id_t caster_id, int angle)
{
	player->EvokedFX().Activate(Arts::KINESIS, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
  if (n == NO_ACTOR && caster_id != REFLECT_ID)
    return;
  angle = angle*4;
	this->DisplayUsedByOther(n, Arts::KINESIS);
  MoveActor (player, angle, PUSH_DISTANCE, MOVE_NORMAL);
  if (caster_id != REFLECT_ID) { // No need to show Kinesis was reflected again
  LoadString (hInstance, IDS_KINESIS_APPLIED, disp_message, sizeof(disp_message));
  _stprintf (message, disp_message, n->Name ());
	display->DisplayMessage (message);
  }
	player->PerformedAction();
	return;
}



void cArts::EndKinesis (void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::KINESIS);
		this->ArtFinished(false);
		return;
  } else {
		// must be visible to site of vulnerability to effect blast

// RRR 02/12/99
// If a player is blind, they cannot possibly use arts that require Line of Sight. 
// Added (ACTOR_BLINDED)
		if (n->IsVulnerable() && !(player->flags & ACTOR_BLINDED) && 
			(n->Room() == player->Room())) 
		{
			gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::KINESIS, player->Skill(Arts::KINESIS), (player->angle)/4);
			this->DisplayUsedOnOther(n, Arts::KINESIS);
			this->ArtFinished(true);
		}
		else
		{ // display message
			LoadString (hInstance, IDS_NEED_VISIBLE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::KINESIS));
			display->DisplayMessage(message);
			this->ArtFinished(true);
		}
	}
	return;
}


//////////////////////////////////////////////////////////////////
// Trap Nightmare

void cArts::StartTrapMare(void)
{
	int num_mares = cp->NumMonsters();
	cNeighbor *n;

	if (num_mares == 0)
	{
		LoadString (hInstance, IDS_NO_AGENT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::TRAP_NIGHTMARE));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}
	else if (num_mares == 1)
	{
		for (n = actors->IterateNeighbors(INIT); n != NO_ACTOR; n = actors->IterateNeighbors(NEXT))
			if (n->IsMonster())
				break;

			actors->IterateNeighbors(DONE);
			cp->SetSelectedNeighbor(n);
			this->EndTrapMare();
			return;
	}
	this->WaitForSelection(&cArts::EndTrapMare, Arts::TRAP_NIGHTMARE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::TRAP_NIGHTMARE);
	return;
}


void cArts::ApplyTrapMare(int skill, int guild_flags, lyra_id_t caster_id)
{
	bool success = false; // always fails on players
	player->EvokedFX().Activate(Arts::TRAP_NIGHTMARE, false);
	
	int random = rand()%100;
	
	if (random < (skill + 25 - player->CurrStat(Stats::DREAMSOUL)))
		success = true;
	
#pragma message ("MKET - Game designer choice goes here when ready")
//#if 0		// MKET - Game designer choice

	if (player->AvatarType() == Avatars::HORRON)
		success = false;// horrons are immune to trapping

//#endif 0 // MKET - Game designer choice

#ifndef AGENT

	success = false; // only agents can be trapped

#endif
	
	if (success)
	{	// Trap logic here
		LoadString (hInstance, IDS_ART_APPLIED_TO_OTHER, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(Arts::TRAP_NIGHTMARE), player->Name());
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
		player->Dissolve(caster_id, player->CurrStat(Stats::DREAMSOUL));
	}
	else
	{
		LoadString (hInstance, IDS_TRAP_FAILED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, player->Name());
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
	}
	return;
}

void cArts::EndTrapMare(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::TRAP_NIGHTMARE);
		this->ArtFinished(false);
		return;
	}
	else if (!n->IsMonster())
	{
		LoadString (hInstance, IDS_NOT_MARE, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name());
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}
	else
	{ // must be visible to TrapMare

// RRR 02/12/99
// If a player is blind, they cannot possibly use arts that require Line of Sight. 
// Added (ACTOR_BLINDED)

		if (n->Visible() && !(player->flags & ACTOR_BLINDED)) 
		{
			gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::TRAP_NIGHTMARE, player->Skill(Arts::TRAP_NIGHTMARE), player->GuildFlags(Guild::RULER_PENDING));
			cDS->PlaySound(LyraSound::TRAP_NIGHTMARE, player->x, player->y, true);
			this->ArtFinished(true);
		}
		else // display message
		{
			LoadString (hInstance, IDS_NEED_VISIBLE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::TRAP_NIGHTMARE));
			display->DisplayMessage(message, false);
			this->ArtFinished(true);
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Judgement

void cArts::StartJudgement(void)
{
	this->WaitForSelection(&cArts::EndJudgement, art_in_use);
	this->CaptureCP(NEIGHBORS_TAB, art_in_use);
	return;
}

const UINT judgement_power_strings[10][64] =
{
	IDS_UNSPHERED,
	IDS_FIRST_SPHERE,
	IDS_SECOND_SPHERE,
	IDS_THIRD_SPHERE,
	IDS_FOURTH_SPHERE,
	IDS_FIFTH_SPHERE,
	IDS_SIXTH_SPHERE,
	IDS_SEVENTH_SPHERE,
	IDS_EIGHT_SPHERE,
	IDS_NINTH_SPHERE,
};

const UINT judgement_health_strings[10][64] =
{
	IDS_HANG_COHERENCE,
	IDS_EDGE_COHERENCE,
	IDS_NEAR_DISS,
	IDS_WAVER_NEAR_DISS,
	IDS_SEEN_BETTER,
	IDS_BEAT_UP,
	IDS_SHOW_WEAR,
	IDS_DAMAGED_BUT_STRONG,
	IDS_MOST_EXCELLENT,
	IDS_PERFECT,
};


void cArts::ApplyJudgement(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::JUDGEMENT, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);

#ifdef AGENT
	LoadString (hInstance, IDS_JUDGE_MARE, message, sizeof(message));
#else
	int random, sphere, health;
	random = rand()%100;
	if (random > skill) // chance of detection = 100 - skill level
		this->DisplayUsedByOther(n, Arts::JUDGEMENT);

	sphere = player->Sphere();
	float Curr = (float)player->CurrStat(Stats::DREAMSOUL);
	float Max = (float)player->MaxStat(Stats::DREAMSOUL);
	float Ratio = Curr/Max;
	health = (int)(Ratio*10);
	if (health >9)
		health=9;

	TCHAR temp_buffer[DEFAULT_MESSAGE_SIZE];

	LoadString (hInstance, IDS_UNSPHERED + sphere, temp_buffer, sizeof(temp_buffer));
	LoadString (hInstance, IDS_JUDGEMENT, disp_message, sizeof(disp_message));
	LoadString(hInstance, IDS_HANG_COHERENCE + health, temp_message, sizeof(temp_message));
	_stprintf(message, disp_message, player->Name(), temp_buffer, temp_message);
		
#endif

	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
	return;
}

void cArts::ApplyScan(int skill, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::SCAN, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);

#ifdef AGENT
	LoadString (hInstance, IDS_JUDGE_MARE, message, sizeof(message));
#else

	int random, sphere, health;
	random = rand()%100;
	if (random > skill) // chance of detection = 100 - skill level
		this->DisplayUsedByOther(n, Arts::SCAN);

	float Curr = (float)player->CurrStat(Stats::DREAMSOUL);
	float Max = (float)player->MaxStat(Stats::DREAMSOUL);
	float Ratio = Curr/Max;
	health = (int)(Ratio*10);
	if (health >9)
		health=9;

	TCHAR temp_buffer[DEFAULT_MESSAGE_SIZE];

	LoadString (hInstance, IDS_SCANNING, disp_message, sizeof(disp_message));
	LoadString(hInstance, IDS_HANG_COHERENCE + health, temp_message, sizeof(temp_message));
	_stprintf(message, disp_message, player->Name(), temp_message);
		
#endif

	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
	return;
}


void cArts::EndJudgement(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(art_in_use);
		this->ArtFinished(false);
		return;
	}
	int playermsg_type = RMsg_PlayerMsg::JUDGEMENT;
	if (art_in_use == Arts::SCAN)
		playermsg_type = RMsg_PlayerMsg::SCAN;

	gs->SendPlayerMessage(n->ID(), playermsg_type,
		player->Skill(art_in_use), 0);
	this->DisplayUsedOnOther(n, art_in_use);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Abjure

void cArts::StartAbjure(void)
{
	this->WaitForSelection(&cArts::EndAbjure, Arts::ABJURE);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::ABJURE);
	return;
}


void cArts::ApplyAbjure(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
  
  if (caster_id != player->ID () && n == NO_ACTOR)
    return; // MDA 3/18/2004 - bail if the neighbor became NULL between evoke and Apply

  this->DisplayUsedByOther(n, Arts::ABJURE);
	int num_effects_to_abjure = 1;
	int num_effects_active = 0;
	int num_effects_abjured = 0;
	int curr_skill = skill;
	int random,i,j;
	
	player->EvokedFX().Activate(Arts::ABJURE, false);

	for (i=0; i<NUM_TIMED_EFFECTS; i++)
		if (player->flags & timed_effects->actor_flag[i])
			num_effects_active++;

	// if no active effects, do nothing
	while (1)
	{ // determine # of effects abjured
		curr_skill = curr_skill/4;
		random = rand()%100;
		if (random > curr_skill)
			break;
		num_effects_to_abjure++;
	}


	while (num_effects_active && num_effects_to_abjure)
	{
		random = rand()%num_effects_active;
		j=0; // j = count of active effects skipped by loop
		for (i=0; i<NUM_TIMED_EFFECTS; i++)
			if (player->flags & timed_effects->actor_flag[i])
			{
				if (j == random) // abjure this effect
				{
					LoadString (hInstance, IDS_ABJURED_EFFECT, disp_message, sizeof(disp_message));
					if (caster_id == player->ID())
					{
					LoadString(hInstance, IDS_YOURSELF, temp_message, sizeof(temp_message));
					_stprintf(message, disp_message, timed_effects->name[i], temp_message);
						display->DisplayMessage (message);
					}
					else
					{
						_stprintf(message, disp_message, timed_effects->name[i], player->Name());
						gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
						LoadString (hInstance, IDS_ABJURED_EFFECT_OTHER, disp_message, sizeof(disp_message));
						_stprintf(message, disp_message, n->Name(), timed_effects->name[i]);
						display->DisplayMessage (message);
          }
					player->RemoveTimedEffect(i);
					break;
				}
				else
					j++;
			}
		num_effects_active--;
		num_effects_to_abjure--;
		num_effects_abjured++;
		if (i == NUM_TIMED_EFFECTS)
		{
			GAME_ERROR(IDS_ABJURE_FAIL);
			return;
		}
	}

	if (!num_effects_abjured && (caster_id == player->ID()))
	{
		LoadString (hInstance, IDS_NOTHING_HAPPENS, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
	}
	return;
}

void cArts::EndAbjure(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::ABJURE);
		this->ArtFinished(false);
		return;
	}
	if (n->ID() == player->ID())
		this->ApplyAbjure(player->Skill(Arts::ABJURE), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::ABJURE,
			player->Skill(Arts::ABJURE), 0);
	this->DisplayUsedOnOther(n, Arts::ABJURE);
	cDS->PlaySound(LyraSound::ABJURE, player->x, player->y, true);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Finger of Death

void cArts::StartDie(void)
{
#ifdef GAMEMASTER
	this->WaitForSelection(&cArts::EndDie, Arts::FINGER_OF_DEATH);
	this->CaptureCP(NEIGHBORS_TAB, Arts::FINGER_OF_DEATH);
#else
	LoadString (hInstance, IDS_GM_ONLY, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	this->ArtFinished(true);
#endif
	return;
}

void cArts::ApplyDie(lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::FINGER_OF_DEATH, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::FINGER_OF_DEATH);
	player->SetCurrStat(Stats::DREAMSOUL, 0, SET_ABSOLUTE, caster_id);
	return;
}

void cArts::EndDie(void)
{

#ifdef GAMEMASTER
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::FINGER_OF_DEATH);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::FINGER_OF_DEATH, 0, 0);
	this->DisplayUsedOnOther(n, Arts::FINGER_OF_DEATH);
#endif
	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Grant XP

void cArts::StartGrantXP(void)
{
#ifdef GAMEMASTER
	this->WaitForSelection(&cArts::MidGrantXP, Arts::GRANT_XP);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::GRANT_XP);
#else
	LoadString (hInstance, IDS_GM_ONLY, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	this->ArtFinished(true);
#endif
	return;
}

void cArts::MidGrantXP(void)
{
	if (entervaluedlg)
	{
		this->ArtFinished(false);
		return;
	}
	entervaluedlg = true;
	LoadString (hInstance, IDS_HOW_MUCH_XP, message, sizeof(message));
	HWND hDlg = CreateLyraDialog(hInstance, (IDD_ENTER_VALUE),
						cDD->Hwnd_Main(), (DLGPROC)EnterValueDlgProc);
	entervalue_callback = (&cArts::EndGrantXP);
	SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
	this->WaitForDialog(hDlg, Arts::GRANT_XP);

	return;
}

void cArts::ApplyGrantXP(int k, int c, lyra_id_t caster_id)
{
	 player->EvokedFX().Activate(Arts::GRANT_XP, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (n != NO_ACTOR)
	{
		int amount = (k*1000 + c*100);
		if (amount > 0)
			LoadString (hInstance, IDS_GRANT_XP, disp_message, sizeof(disp_message));
		else
			LoadString (hInstance, IDS_REMOVE_XP, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name());
		display->DisplayMessage (message);
	}
	return;
}

void cArts::EndGrantXP(void *value)
{

#ifdef GAMEMASTER
	bool negative = false;
	if (!value)
	{
		this->ArtFinished(false);
		return;
	}
	int amount = _ttoi(message);

	if (amount == 0)
	{
		this->ArtFinished(false);
		return;
	}
	else if (amount < 0)
	{
		negative = true;
		amount = -amount;
	}

	int k,c;
	k = amount/1000;
	amount = amount%1000;
	c = amount/100;
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::GRANT_XP);
		this->ArtFinished(false);
		return;
	}
	if (negative)
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::GRANT_XP_NEGATIVE, k, c);
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::GRANT_XP, k, c);
	this->DisplayUsedOnOther(n,Arts::GRANT_XP );
#endif
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Terminate

void cArts::StartTerminate(void)
{
#ifdef GAMEMASTER
	this->WaitForSelection(&cArts::EndTerminate, Arts::TERMINATE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::TERMINATE);
#else
	LoadString (hInstance, IDS_GM_ONLY, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	this->ArtFinished(true);
#endif
	return;
}

void cArts::ApplyTerminate(lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::TERMINATE, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::TERMINATE);
	LoadString (hInstance, IDS_PLAYER_TERMINATED, message, sizeof(message));
	LyraDialogBox(hInstance, (IDD_FATAL_ERROR), 
					cDD->Hwnd_Main(), (DLGPROC)FatalErrorDlgProc);
	Exit();
}

void cArts::EndTerminate(void)
{

#ifdef GAMEMASTER
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::TERMINATE);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::TERMINATE, 0, 0);
	this->DisplayUsedOnOther(n, Arts::TERMINATE);
#endif
	this->ArtFinished(true);
	return;
}



//////////////////////////////////////////////////////////////////
// Dream Strike

// constants for dream strike allowable levels in SharedConstants.h

void cArts::StartDreamStrike(void)
{
	
	for (int i=0; i<num_no_dreamstrike_levels; i++) 
		if (no_dreamstrike_levels[i] == level->ID())
		{
			LoadString (hInstance, IDS_NO_DREAM_STRIKE_LEVEL, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			this->ArtFinished(false);
			return;
		}

	if (player->CurrStat(Stats::DREAMSOUL) < 25)
	{
		LoadString (hInstance, IDS_NO_DREAM_STRIKE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
		return;
	}
	this->WaitForSelection(&cArts::EndDreamStrike, Arts::DREAMSTRIKE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::DREAMSTRIKE);
	return;
}

void cArts::ApplyDreamStrike(lyra_id_t caster_id, int success)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	 player->EvokedFX().Activate(Arts::DREAMSTRIKE, false);
	if (success)
	{	// send out death emote
		LoadString(hInstance, IDS_DREAMSTRIKEN, message, sizeof(message));
		if (n != NO_ACTOR) {
			_stprintf(disp_message, message, n->Name(), player->Name());
		} else {
			LoadString(hInstance, IDS_ANOTHER_DREAMER, temp_message, sizeof(temp_message));
			_stprintf(disp_message, message, temp_message, player->Name());
		}
		gs->Talk(disp_message, RMsg_Speech::EMOTE, Lyra::ID_UNKNOWN);

		LoadString (hInstance, IDS_DREAMSTRIKEN_SUCCESS, message, sizeof(message));
		if (n != NO_ACTOR)
		{
			LoadString(hInstance, IDS_ANOTHER_DREAMER, temp_message, sizeof(temp_message));
			_stprintf(disp_message, message, temp_message);
		}
		else
			_stprintf(disp_message, message, n->Name());
		//LyraDialogBox(hInstance, IDD_FATAL_ERROR, NULL, (DLGPROC)FatalErrorDlgProc);
		killed = true;
		Exit();
		return;
	}
	else
	{
		LoadString (hInstance, IDS_DREAMSTRIKEN_FAILED, disp_message, sizeof(disp_message));
		if (n != NO_ACTOR)
		_stprintf(message, disp_message, n->Name());
		else
		LoadString(hInstance, IDS_ANOTHER_AVATAR, temp_message, sizeof(temp_message));
		_stprintf(temp_message, disp_message, n->Name());
		display->DisplayMessage(message);
	}
	return;
}

void cArts::EndDreamStrike(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::DREAMSTRIKE);
		this->ArtFinished(false);
		return;
	}
	else if (!(n->flags & ACTOR_SOULSPHERE)) { // must be ss to target
		LoadString (hInstance, IDS_MUST_BE_SOULSPHERE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::DREAMSTRIKE));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::DREAMSTRIKE, 0, 0);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Sphere (level training)

void cArts::StartSphere(void)
{
	this->WaitForSelection(&cArts::EndSphere, Arts::LEVELTRAIN);
	this->CaptureCP(NEIGHBORS_TAB, Arts::LEVELTRAIN);
	return;
}

void cArts::ApplySphere(int success, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);

	if (n == NO_ACTOR)
		return;

	 player->EvokedFX().Activate(Arts::LEVELTRAIN, false);
	int num_tokens = 0;
	cItem* tokens[Lyra::INVENTORY_MAX];

	if (success)
	{	// iterate and destroy all support sphere tokens in inventory
		num_tokens = CountTrainSphereTokens(SPHERE_TOKEN_ART_ID, player->ID(), (cItem**)tokens, false);
		for (int i=0; i<num_tokens; i++) 
			tokens[i]->Destroy();
		LoadString (hInstance, IDS_LEVELTRAIN_SUCCEEDED, disp_message, sizeof(disp_message));
	}
	else
	{
		num_tokens = CountTrainSphereTokens(SPHERE_TOKEN_ART_ID, player->ID(), (cItem**)tokens, true);
		LoadString (hInstance, IDS_LEVELTRAIN_FAILED, disp_message, sizeof(disp_message));
	}
	_stprintf(message, disp_message, n->Name());
	display->DisplayMessage(message);

	if (success)
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::SPHERE_REPLY, 1, player->Sphere());
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::SPHERE_REPLY, 0, player->Sphere());

	return;
}

void cArts::ResponseSphere(lyra_id_t caster_id, int success, int sphere)
{
	int i,num_tokens = 0;
	cItem* tokens[Lyra::INVENTORY_MAX]; // holds token pointers
	cNeighbor *n = this->LookUpNeighbor(caster_id);


	if (success)
	{	// success!!!
 		LoadString (hInstance, IDS_LEVELTRAIN_OTHER_SUCCEEDED, disp_message, sizeof(disp_message));
		if (n != NO_ACTOR) 
			_stprintf(message, disp_message, n->Name());
		else
			_stprintf(message, disp_message, _T("Unknown avatar"));
		display->DisplayMessage (message);

		// look up the tokens required so we can delete them
		num_tokens = CountTrainSphereTokens(SPHERE_TOKEN_ART_ID, caster_id, (cItem**)tokens, false);

	// now iterate and destroy ALL sphere support tokens

		for (i=0; i<num_tokens; i++)
			tokens[i]->Destroy();

    // actors->IterateItems(DONE); // this is unnecessary... CountTrainSphereTokens resets the depth
                                   // doing this here causes a crash because then UpdateServer iterates
                                   // on an incorrect depth.

	// now destroy the sphere quest item

		if (actors->ValidItem(quest_item))
			quest_item->Destroy();

		quest_item = NULL;

		gs->UpdateServer();
	}
	else
	{	// failure!
		num_tokens = CountTrainSphereTokens(SPHERE_TOKEN_ART_ID, caster_id, (cItem**)tokens, true);
		LoadString (hInstance, IDS_SPHERE_FAILED, disp_message, sizeof(disp_message));
		if (n != NO_ACTOR) 
			_stprintf(message, disp_message, n->Name(), num_tokens);
		else
			_stprintf(message, disp_message, _T("Unknown avatar"), num_tokens);
		display->DisplayMessage (message);
	}

}


void cArts::EndSphere(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	lyra_id_t art_id = cp->SelectedArt();
	quest_item = NO_ITEM;

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::LEVELTRAIN);
		this->ArtFinished(false);
		return;
	}

	// in order to sphere, teacher must be holding a quest codex for the appropriate
	// player and art of sphere

	// necessity of Quest item below 
	if ((quest_item = HasQuestCodex(n->ID(), Arts::LEVELTRAIN)) == NO_ITEM)
	{
		LoadString (hInstance, IDS_NEED_SPHERE_QUEST, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	// we can't do a sphere token validation here because
	// we don't know what sphere the target player is
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::TRAIN_SPHERE, 0, 0);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Mind Blank

void cArts::StartMindBlank(void)
{
	this->WaitForSelection(&cArts::EndMindBlank, Arts::MIND_BLANK);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::MIND_BLANK);
	return;
}

void cArts::ApplyMindBlank(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if ((n == NO_ACTOR) && (caster_id != player->ID()))
		return;

	this->DisplayUsedByOther(n, Arts::MIND_BLANK);

	if (player->flags & ACTOR_SOUL_SHIELDED)
	{
		LoadString (hInstance, IDS_SS_MB_OTHER, disp_message, sizeof(disp_message));
		if (n != NO_ACTOR)
			_stprintf(temp_message, disp_message, n->Name());
		else
			_stprintf(temp_message, disp_message, _T("Unknown avatar"));
		display->DisplayMessage(temp_message);
		LoadString (hInstance, IDS_SS_MB_OTHER_ACK, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, player->Name());
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
		this->ArtFinished(false);
		return;
	} 

    player->EvokedFX().Activate(Arts::MIND_BLANK, false);

	int duration = this->Duration(Arts::MIND_BLANK, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_MIND_BLANKED, duration, player->ID());
	return;
}

void cArts::EndMindBlank(void)
{
	cNeighbor *n = cp->SelectedNeighbor();

	if (player->flags & ACTOR_SOUL_SHIELDED)
	{
		LoadString (hInstance, IDS_SS_MB, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
		return;
	}
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::MIND_BLANK);
		this->ArtFinished(false);
		return;
	}
	else
	{
		if (n->ID() == player->ID())
			this->ApplyMindBlank(player->Skill(Arts::MIND_BLANK), player->ID());
		else
			gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::MIND_BLANK_OTHER, player->Skill(Arts::MIND_BLANK), 0);
		this->DisplayUsedOnOther(n, Arts::MIND_BLANK);

		this->ArtFinished(true);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Boot

void cArts::StartBoot(void)
{
#ifdef GAMEMASTER
	this->WaitForSelection(&cArts::EndBoot, Arts::BOOT);
	this->CaptureCP(NEIGHBORS_TAB, Arts::BOOT);
#else
	LoadString (hInstance, IDS_GM_ONLY, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	this->ArtFinished(true);
#endif
	return;
}

void cArts::ApplyBoot(lyra_id_t caster_id)
{
	 player->EvokedFX().Activate(Arts::BOOT, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::BOOT);
	Exit();
	return;
}

void cArts::EndBoot(void)
{
#ifdef GAMEMASTER
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::BOOT);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::BOOT, 0, 0);
		this->DisplayUsedOnOther(n, Arts::BOOT);
#endif
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Grant Role Playing XP

void cArts::StartGrantRPXP(void)
{
	this->WaitForSelection(&cArts::MidGrantRPXP, Arts::GRANT_RP_XP);
	this->CaptureCP(NEIGHBORS_TAB, Arts::GRANT_RP_XP);
	return;
}

void cArts::MidGrantRPXP(void)
{
	if (entervaluedlg)
	{
		this->ArtFinished(false);
		return;
	}
	entervaluedlg = true;
	LoadString (hInstance, IDS_HOW_MUCH_XP, message, sizeof(message));
	HWND hDlg = CreateLyraDialog(hInstance, (IDD_ENTER_VALUE),
						cDD->Hwnd_Main(), (DLGPROC)EnterValueDlgProc);
	entervalue_callback = (&cArts::EndGrantRPXP);
	SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
	this->WaitForDialog(hDlg, Arts::GRANT_RP_XP);

	return;
}

void cArts::EndGrantRPXP(void *value)
{
	if (!value)
	{
		this->ArtFinished(false);
		return;
	}
	int amount = _ttoi(message);
	if (0 == amount)
	{
		this->ArtFinished(false);
		return;
	}
	int k,c;
	k = amount/1000;
	amount = amount%1000;
	c = amount/100;
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::GRANT_RP_XP);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::GRANT_RP_XP, k, c);
	this->DisplayUsedOnOther(n,Arts::GRANT_RP_XP );
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Soul Shield

void cArts::StartSoulShield(void)
{
	this->WaitForSelection(&cArts::EndSoulShield, Arts::SOUL_SHIELD);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::SOUL_SHIELD);
	return;
}

void cArts::ApplySoulShield(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);

	this->DisplayUsedByOther(n, Arts::SOUL_SHIELD);

	if ((n == NO_ACTOR) && (caster_id != player->ID()))
		return;

    player->EvokedFX().Activate(Arts::SOUL_SHIELD, false);

	int duration = this->Duration(Arts::SOUL_SHIELD, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_SOUL_SHIELD, duration, player->ID());


	return;
}

void cArts::EndSoulShield(void)
{
	cNeighbor *n = cp->SelectedNeighbor();

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SOUL_SHIELD);
		this->ArtFinished(false);
		return;
	}
	else
	{
		if (n->ID() == player->ID())
			this->ApplySoulShield(player->Skill(Arts::SOUL_SHIELD), player->ID());
		else
		{
			gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::SOUL_SHIELD, player->Skill(Arts::SOUL_SHIELD), 0);
			this->DisplayUsedOnOther(n, Arts::SOUL_SHIELD);
		}

		this->ArtFinished(true);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Summon

void cArts::StartSummon(void)
{
#ifdef GAMEMASTER
	this->WaitForSelection(&cArts::EndSummon, Arts::SUMMON);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::SUMMON);
#else
	LoadString (hInstance, IDS_GM_ONLY, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	this->ArtFinished(true);
#endif
	return;
}

void cArts::ApplySummon(lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::SUMMON);

	if (n == NO_ACTOR)
		return;

    player->EvokedFX().Activate(Arts::SUMMON, false);

	player->Teleport (-7839,12457,-90,43 );	// Unknown

	return;
}

void cArts::EndSummon(void)
{
	cNeighbor *n = cp->SelectedNeighbor();

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SUMMON);
		this->ArtFinished(false);
		return;
	} else if (n->ID() == player->ID())
	{
		player->Teleport (-7839,12457,-90,43);	// Unknown
	}

	else
	{
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::SUMMON, player->Skill(Arts::SUMMON), 0);
		this->DisplayUsedOnOther(n, Arts::SUMMON);
	}

	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Rally

void cArts::StartRally(void)
{
	for (int i=0; i<num_no_rally_levels; i++) 
		if (no_rally_levels[i] == level->ID()){ // Cannot use Rally in levels with sphere/house locks
			LoadString (hInstance, IDS_NO_RALLY_LEVEL, disp_message, sizeof(disp_message));
			display->DisplayMessage (disp_message);
			this->ArtFinished(false);
			return;
		}
	if (gs->Party()->Members() < 1){ // Must be in a party to use Rally
		LoadString (hInstance, IDS_RALLY_NOPARTY, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
	this->WaitForSelection(&cArts::EndRally, Arts::RALLY);
	this->CaptureCP(NEIGHBORS_TAB, Arts::RALLY);
	return;
}

void cArts::ApplyRally(lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (n == NO_ACTOR)
		return;
	rally_id = caster_id;
	this->DisplayUsedByOther(n,Arts::RALLY);
	if (player->flags & ACTOR_SOULSPHERE)
		return;
	if (!acceptrejectdlg)
		{
			LoadString (hInstance, IDS_QUERY_RALLY, disp_message, sizeof(disp_message));
				_stprintf(message, disp_message, level->RoomName(n->Room()), n->Name());
			HWND hDlg = CreateLyraDialog(hInstance, (IDD_ACCEPTREJECT),
							cDD->Hwnd_Main(), (DLGPROC)AcceptRejectDlgProc);
			acceptreject_callback = (&cArts::GotRallied);
			SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
			SendMessage(hDlg, WM_SET_AR_NEIGHBOR, 0, (LPARAM)n);
		}
	return;
}

void cArts::GotRallied(void *value)
{
	if (player->flags & ACTOR_SOULSPHERE)
		return;
	int success = *((int*)value);
	if (success){
		player->EvokedFX().Activate(Arts::RALLY, false);
		cNeighbor *n = this->LookUpNeighbor(rally_id);
		player->EvokedFX().Activate(Arts::RALLY, false);
		player->Teleport (n->x, n->y, n->angle, NO_LEVEL);
	}
	rally_id = 0;
	return;
}

void cArts::EndRally(void)
{
	cNeighbor *n = cp->SelectedNeighbor();

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n))){
		this->DisplayNeighborBailed(Arts::RALLY);
		this->ArtFinished(false);
		return;
	}

	if (!gs->Party()->IsInParty(n->ID())){ //target must be in player's party
		LoadString (hInstance, IDS_RALLY_NOTMEMBER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
/*
	if (n->flags & ACTOR_SOULSPHERE){
		LoadString (hInstance, IDS_RALLY_NO_SS, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
*/
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::RALLY, 0, 0);
	this->DisplayUsedOnOther(n, Arts::RALLY);

	this->ArtFinished(true);
	return;
}
//////////////////////////////////////////////////////////////////
// Suspend

void cArts::StartSuspend(void)
{
#ifdef GAMEMASTER
	this->WaitForSelection(&cArts::MidSuspend, Arts::SUSPEND);
	this->CaptureCP(NEIGHBORS_TAB, Arts::SUSPEND);
#else
	LoadString (hInstance, IDS_GM_ONLY, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	this->ArtFinished(true);
#endif
	return;
}

void cArts::MidSuspend(void)
{
	if (entervaluedlg)
	{
		this->ArtFinished(false);
		return;
	}
	entervaluedlg = true;
	LoadString (hInstance, IDS_HOW_MANY_DAYS, message, sizeof(message));
	HWND hDlg = CreateLyraDialog(hInstance, (IDD_ENTER_VALUE),
						cDD->Hwnd_Main(), (DLGPROC)EnterValueDlgProc);
	entervalue_callback = (&cArts::EndSuspend);
	SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
	this->WaitForDialog(hDlg, Arts::SUSPEND);

	return;
}

void cArts::ApplySuspend(int num_days, lyra_id_t caster_id)
{
	player->EvokedFX().Activate(Arts::SUSPEND, false);
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::SUSPEND);
	LoadString (hInstance, IDS_PLAYER_SUSPENDED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, num_days);
	exiting = true;
	LyraDialogBox(hInstance, (IDD_FATAL_ERROR), 
					cDD->Hwnd_Main(), (DLGPROC)FatalErrorDlgProc);
	Exit();
}

void cArts::EndSuspend(void* value)
{
#ifdef GAMEMASTER
	if (!value)
	{
		this->ArtFinished(false);
		return;
	}
	int num_days = _ttoi(message);

	if (num_days < 1)
		num_days = 1;

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SUSPEND);
		this->ArtFinished(false);
		return;
	}
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::SUSPEND, num_days, 0);
	this->DisplayUsedOnOther(n, Arts::SUSPEND);
#endif
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Expel

void cArts::StartExpel(void)
{
	if (!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_RULER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::EXPEL));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::EndExpel, Arts::EXPEL);
	this->CaptureCP(NEIGHBORS_TAB, Arts::EXPEL);
	return;
}

void cArts::ApplyExpel(int guild_id, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::EXPEL);

	if (n == NO_ACTOR)
		return;

    player->EvokedFX().Activate(Arts::EXPEL, false);

	// send to Boggen's lair = 7049;-4831;39 
	player->Teleport (7049, -4831, 0, 39);	

	return;
}

void cArts::EndExpel(void)
{
	cNeighbor *n = cp->SelectedNeighbor();

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::EXPEL);
		this->ArtFinished(false);
		return;
	} 
	else if (!(n->flags & ACTOR_SOULSPHERE)) { // must be ss to target
		LoadString (hInstance, IDS_MUST_BE_SOULSPHERE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::EXPEL));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}
	// now figure out which prime we have
	cItem* prime = NULL;
	for (int guild=0; guild<NUM_GUILDS; guild++) 
	{
		prime = FindPrime(guild, Arts::EXPEL_DRAIN);
		if (prime == NO_ITEM) 
		{
			continue;
		} 
		else if (player->IsRuler(guild))
		{
			prime->DrainMetaEssence(Arts::EXPEL_DRAIN);
			gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::EXPEL, player->Skill(Arts::EXPEL), guild);
			this->DisplayUsedOnOther(n, Arts::EXPEL);
			this->ArtFinished(true);
			return;
		}
	}

	LoadString (hInstance, IDS_EXPEL, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, Arts::EXPEL_DRAIN);
	display->DisplayMessage (message);
	this->ArtFinished(false);
	return;
}

////////////////////////////////////////////////////////////////
// Empathy

// helper method 
int cArts::CountPowerTokens(cItem** tokens, lyra_id_t guild_id)
{
	int num_tokens = 0;
	lyra_item_support_t power_tokens[Lyra::INVENTORY_MAX];	// holds token info
	const void* state;
	cItem* item;

	// check that the proper # of unique support tokens are carried
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::SUPPORT_FUNCTION))
		{ // potential candidate for power token; support is always item function zero
			state = item->Lmitem().StateField(0);
			memcpy(&power_tokens[num_tokens], state, sizeof(lyra_item_support_t));

			// power tokens use the soul essence graphic
			if (item->BitmapID() != LyraBitmap::SOUL_ESSENCE)
				continue; 
			
			if (power_tokens[num_tokens].token_type() != Tokens::POWER_TOKEN)
				continue;

			if (guild_id !=	Guild::NO_GUILD)
				if (guild_id != power_tokens[num_tokens].guild_id())
					continue;

			tokens[num_tokens] = item;
			num_tokens++; // valid token!!!
			
		}
	actors->IterateItems(DONE);
	return num_tokens;
}

// return the effective skill if we take power tokens into account
int cArts::EffectiveForgeSkill(int player_skill, bool usePowerToken)
{
	int num_tokens = 0;
	if (usePowerToken) {
		cItem* power_tokens[Lyra::INVENTORY_MAX];
		num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::NO_GUILD);
	}
	if(!num_tokens)
		return MAX(1,player_skill / 4);
	return player_skill;
}

void cArts::StartEmpathy(void)
{
	this->WaitForSelection(&cArts::MidEmpathy, Arts::EMPATHY);
	this->CaptureCP(NEIGHBORS_TAB, Arts::EMPATHY);
	return;
}

void cArts::MidEmpathy(void)
{
	if (entervaluedlg)
	{
		this->ArtFinished(false);
		return;
	}
	entervaluedlg = true;
	LoadString (hInstance, IDS_HOW_MUCH_BEQUEATH, message, sizeof(message));
	HWND hDlg = CreateLyraDialog(hInstance, (IDD_ENTER_VALUE),
						cDD->Hwnd_Main(), (DLGPROC)EnterValueDlgProc);
	entervalue_callback = (&cArts::EndEmpathy);
	SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
	this->WaitForDialog(hDlg, Arts::EMPATHY);
}


void cArts::ApplyEmpathy(int success, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);

	if (n == NO_ACTOR)
		return;

	player->EvokedFX().Activate(Arts::EMPATHY, false);

	if (success)
	{
		LoadString (hInstance, IDS_EMPATHY_SUCCEEDED, disp_message, sizeof(disp_message));
		if (n != NO_ACTOR)
			_stprintf(message, disp_message, n->Name(), arts->Descrip(Arts::EMPATHY));
		else
			_stprintf(message, disp_message, _T("Unknown avatar"), arts->Descrip(Arts::EMPATHY));
		display->DisplayMessage(message);
	}

	if (success)
	{
		LoadString (hInstance, IDS_EMPATHY_OTHER_SUCCEEDED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, arts->Descrip(Arts::EMPATHY), player->Name());
	}
	else
	{
		LoadString (hInstance, IDS_EMPATHY_OTHER_FAILED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, arts->Descrip(Arts::EMPATHY), player->Name());
	}
	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);

	return;

}

void cArts::EndEmpathy(void* value)
{
	if (!value)
	{
		this->ArtFinished(false);
		return;
	}
	int amount = _ttoi(message);
	if ((amount < 100) || (amount > 10000) || (amount*2 > player->XP()))
	{
		LoadString (hInstance, IDS_BEQUEATH_LIMITS, message, sizeof(message));
		display->DisplayMessage(message);

		this->ArtFinished(false);
		return;
	}

	int k,c;
	k = amount/1000;
	amount = amount%1000;
	c = amount/100;

	cNeighbor* n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::EMPATHY);
		this->ArtFinished(false);
		return;
	} 



	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::NO_GUILD);
	
	if (0 == num_tokens)
	{
		LoadString (hInstance, IDS_NEED_POWER_TOKEN, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, arts->Descrip(Arts::EMPATHY));
		display->DisplayMessage (message);

		this->ArtFinished(false);
		return;
	}

	power_tokens[0]->Destroy();
	
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::EMPATHY, k, c);

	this->ArtFinished(true);
}

// Grant Personality Point

void cArts::StartGrantPPoint(void)
{
	if ((player->PPPool() - player->GrantingPP()) < 1)
	{

		LoadString (hInstance, IDS_NEED_PP_POOL, message, sizeof(message));
		//display->DisplayMessage (disp_message);
		HWND hDlg = CreateLyraDialog(hInstance, (IDD_NONFATAL_ERROR),
							cDD->Hwnd_Main(), (DLGPROC)NonfatalErrorDlgProc);

		this->ArtFinished(false);
		return;

	}
		
	this->WaitForSelection(&cArts::MidGrantPPoint, Arts::GRANT_PPOINT);
	this->CaptureCP(NEIGHBORS_TAB, Arts::GRANT_PPOINT);
	return;
}

void cArts::MidGrantPPoint(void)
{
	if (grantppointdlg)
	{
		this->ArtFinished(false);
		return;
	}
	grantppointdlg = true;
	HWND hDlg = CreateLyraDialog(hInstance, (IDD_GRANT_PPOINT),
						cDD->Hwnd_Main(), (DLGPROC)GrantPPointDlgProc);
	grantpp_callback = (&cArts::EndGrantPPoint);
	SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
	this->WaitForDialog(hDlg, Arts::GRANT_PPOINT);
}



void cArts::EndGrantPPoint(void* value)
{
	if (!value)
	{
		this->ArtFinished(false);
		return;
	}

	TCHAR* why = (TCHAR*)value;

	cNeighbor* n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::GRANT_PPOINT);
		this->ArtFinished(false);
		return;
	}
	
	else if (n->IsMonster())
	{
		LoadString (hInstance, IDS_NO_PP_MONSTER, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	gs->GrantPPoint(n->ID(), why);
	
	this->ArtFinished(true);
}



//////////////////////////////////////////////////////////////////
// Break Covenant

const int BREAK_COVENANT_POWER_TOKENS = 1;

void cArts::StartBreakCovenant(void)
{
	this->WaitForSelection(&cArts::EndBreakCovenant, Arts::BREAK_COVENANT);
	this->CaptureCP(NEIGHBORS_TAB, Arts::BREAK_COVENANT);
	return;
}

void cArts::ApplyBreakCovenant(int skill, lyra_id_t caster_id)
{	
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (n == NO_ACTOR)
		return;

	player->EvokedFX().Activate(Arts::BREAK_COVENANT, false);
	this->DisplayUsedByOther(n, Arts::BREAK_COVENANT);

	if (player->GuildRank(Guild::COVENANT) >= Guild::INITIATE)
	{	
		LoadString (hInstance, IDS_CANT_BE_MEMBER_APPLY, message, sizeof(message));
		_stprintf(disp_message, message, arts->Descrip(Arts::BREAK_COVENANT), 
					GuildName(Guild::COVENANT));
		display->DisplayMessage(disp_message, false);
	}
	else 
	{
		if (options.network && gs && gs->Party() && 
			(gs->Party()->Members() > 0))
		{
			gs->Party()->LeaveParty();
			int damage = 1 + (rand()%6);
			this->DamagePlayer(damage, caster_id);
		}
		int duration = this->Duration(Arts::BREAK_COVENANT, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_NO_PARTY, duration, caster_id);
	}
	return;
}

void cArts::EndBreakCovenant(void)
{
	cNeighbor* n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::BREAK_COVENANT);
		this->ArtFinished(false);
		return;
	} 

	// only works for UCO
	if (!(player->GuildRank(Guild::COVENANT) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER, message, sizeof(message));
		_stprintf(disp_message, message, GuildName(Guild::COVENANT), arts->Descrip(Arts::BREAK_COVENANT));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::COVENANT);

	if (num_tokens < BREAK_COVENANT_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, BREAK_COVENANT_POWER_TOKENS, GuildName(Guild::COVENANT), arts->Descrip(Arts::BREAK_COVENANT));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}
	
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::BREAK_COVENANT,
			player->Skill(Arts::BREAK_COVENANT), 0);
	
	for (int i=0; i<BREAK_COVENANT_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Peace Aura

const int PEACE_AURA_POWER_TOKENS = 1;

void cArts::StartPeaceAura(void)
{
	this->WaitForSelection(&cArts::EndPeaceAura, Arts::PEACE_AURA);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::PEACE_AURA);
	return;
}

void cArts::ApplyPeaceAura(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if ((n == NO_ACTOR) && (caster_id != player->ID()))
		return;

	if (player->IsMonster())
	{
		return;
	}

	player->EvokedFX().Activate(Arts::PEACE_AURA, false);
	this->DisplayUsedByOther(n, Arts::PEACE_AURA);

	int duration = this->Duration(Arts::PEACE_AURA, skill);
	player->SetTimedEffect(LyraEffect::PLAYER_PEACE_AURA, duration, caster_id);
	
/*	else
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER_APPLY, disp_message, sizeof(disp_message));
      _stprintf(message, disp_message, GuildName(Guild::ECLIPSE), this->Descrip(Arts::PEACE_AURA));
		display->DisplayMessage(message, false);
	}
*/
	return;
}

void cArts::EndPeaceAura(void)
{
	cNeighbor* n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::PEACE_AURA);
		this->ArtFinished(false);
		return;
	} 

	// only works for AOE
	if (!(player->GuildRank(Guild::ECLIPSE) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER, message, sizeof(message));
		_stprintf(disp_message, message, GuildName(Guild::ECLIPSE), arts->Descrip(Arts::PEACE_AURA));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::ECLIPSE);

	if (num_tokens < PEACE_AURA_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, PEACE_AURA_POWER_TOKENS, GuildName(Guild::ECLIPSE), arts->Descrip(Arts::PEACE_AURA));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	if (n->ID() == player->ID())
		this->ApplyPeaceAura(player->Skill(Arts::PEACE_AURA), player->ID());
	else gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::PEACE_AURA,
			player->Skill(Arts::PEACE_AURA), 0);
	
	for (int i=0; i<PEACE_AURA_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Sable Shield

const int SABLE_SHIELD_POWER_TOKENS = 1;

void cArts::StartSableShield(void)
{
	this->WaitForSelection(&cArts::EndSableShield, Arts::SABLE_SHIELD);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::SABLE_SHIELD);
	return;
}

void cArts::ApplySableShield(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (n == NO_ACTOR)
		return;

	player->EvokedFX().Activate(Arts::SABLE_SHIELD, false);
	this->DisplayUsedByOther(n, Arts::SABLE_SHIELD);

	if (player->GuildRank(Guild::MOON) >= Guild::INITIATE)
	{	// only works on OSM
		int duration = this->Duration(Arts::SABLE_SHIELD, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_PROT_PARALYSIS, duration, caster_id);
		player->SetTimedEffect(LyraEffect::PLAYER_PROT_CURSE, duration, caster_id);
		player->SetTimedEffect(LyraEffect::PLAYER_DETECT_INVISIBLE, duration, caster_id);
		player->SetTimedEffect(LyraEffect::PLAYER_PROT_FEAR, duration, caster_id);
		player->SetTimedEffect(LyraEffect::PLAYER_NO_POISON, duration, caster_id);
	}
	else
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER_APPLY, disp_message, sizeof(disp_message));
        _stprintf(message, disp_message, GuildName(Guild::MOON), this->Descrip(Arts::SABLE_SHIELD));
		display->DisplayMessage(message, false);
	}
	return;
}

void cArts::EndSableShield(void)
{
	cNeighbor* n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SABLE_SHIELD);
		this->ArtFinished(false);
		return;
	} 

	// only works for OSM
	if (!(player->GuildRank(Guild::MOON) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER, message, sizeof(message));
		_stprintf(disp_message, message, GuildName(Guild::MOON), arts->Descrip(Arts::SABLE_SHIELD));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::MOON);

	if (num_tokens < SABLE_SHIELD_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, SABLE_SHIELD_POWER_TOKENS, GuildName(Guild::MOON), arts->Descrip(Arts::SABLE_SHIELD));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}
	
	if (n->ID() == player->ID())
		this->ApplySableShield(player->Skill(Arts::SABLE_SHIELD), player->ID());
	else gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::SABLE_SHIELD,
			player->Skill(Arts::SABLE_SHIELD), 0);
	
	for (int i=0; i<SABLE_SHIELD_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Entrancement

const int ENTRANCEMENT_POWER_TOKENS = 1;

void cArts::StartEntrancement(void)
{
	this->WaitForSelection(&cArts::EndEntrancement, Arts::ENTRANCEMENT);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::ENTRANCEMENT);
	return;
}

void cArts::ApplyEntrancement(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (n == NO_ACTOR)
		return;

	player->EvokedFX().Activate(Arts::ENTRANCEMENT, false);
	this->DisplayUsedByOther(n, Arts::ENTRANCEMENT);

	if (player->GuildRank(Guild::ENTRANCED) >= Guild::INITIATE)
	{	// only works on  GOE 
		int duration = this->Duration(Arts::ENTRANCEMENT, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_REGENERATING, duration, caster_id);
	}
	else
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER_APPLY, disp_message, sizeof(disp_message));
        _stprintf(message, disp_message, GuildName(Guild::ENTRANCED), this->Descrip(Arts::ENTRANCEMENT));
		display->DisplayMessage(message, false);
	}
	return;
}

void cArts::EndEntrancement(void)
{
	cNeighbor* n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::ENTRANCEMENT);
		this->ArtFinished(false);
		return;
	} 

	// only works for KOES
	if (!(player->GuildRank(Guild::ENTRANCED) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER, message, sizeof(message));
		_stprintf(disp_message, message, GuildName(Guild::ENTRANCED), arts->Descrip(Arts::ENTRANCEMENT));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::ENTRANCED);

	if (num_tokens < ENTRANCEMENT_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, ENTRANCEMENT_POWER_TOKENS, GuildName(Guild::ENTRANCED), arts->Descrip(Arts::ENTRANCEMENT));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}
	
	if (n->ID() == player->ID())
		this->ApplyEntrancement(player->Skill(Arts::ENTRANCEMENT), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::ENTRANCEMENT,
			player->Skill(Arts::ENTRANCEMENT), 0);
	
	for (int i=0; i<ENTRANCEMENT_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Shadow Step

const int SHADOW_STEP_POWER_TOKENS = 1;

void cArts::StartShadowStep(void)
{
	this->WaitForSelection(&cArts::EndShadowStep, Arts::SHADOW_STEP);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::SHADOW_STEP);
	return;
}

void cArts::ApplyShadowStep(int skill, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (n == NO_ACTOR)
		return;

	player->EvokedFX().Activate(Arts::SHADOW_STEP, false);
	this->DisplayUsedByOther(n, Arts::SHADOW_STEP);

	if (player->GuildRank(Guild::SHADOW) >= Guild::INITIATE)
	{	// only works on KOES
		int duration = this->Duration(Arts::SHADOW_STEP, skill);
		player->SetTimedEffect(LyraEffect::PLAYER_INVISIBLE, duration, caster_id);
	}
	else
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER_APPLY, disp_message, sizeof(disp_message));
        _stprintf(message, disp_message, GuildName(Guild::SHADOW), this->Descrip(Arts::SHADOW_STEP));
		display->DisplayMessage(message, false);
	}
	return;
}

void cArts::EndShadowStep(void)
{
	cNeighbor* n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SHADOW_STEP);
		this->ArtFinished(false);
		return;
	} 

	// only works for KOES
	if (!(player->GuildRank(Guild::SHADOW) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_MEMBER, message, sizeof(message));
		_stprintf(disp_message, message, GuildName(Guild::SHADOW), arts->Descrip(Arts::SHADOW_STEP));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::SHADOW);

	if (num_tokens < SHADOW_STEP_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, SHADOW_STEP_POWER_TOKENS, GuildName(Guild::SHADOW), arts->Descrip(Arts::SHADOW_STEP));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}
	
	if (n->ID() == player->ID())
		this->ApplyShadowStep(player->Skill(Arts::SHADOW_STEP), player->ID());
	else
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::SHADOW_STEP,
			player->Skill(Arts::SHADOW_STEP), 0);
	
	
	for (int i=0; i<SHADOW_STEP_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// ChaosPurge

void cArts::StartChaosPurge(void)
{
	int bitmap = player->Avatar().AvatarType();
#ifdef GAMEMASTER
	if (bitmap < Avatars::EMPHANT)
#endif
	{
		LoadString (hInstance, IDS_MUST_BE_MARE, message, sizeof(message));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::EndChaosPurge, Arts::CHAOS_PURGE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::CHAOS_PURGE);
	return;
}

void cArts::ApplyChaosPurge(lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::CHAOS_PURGE);

	if (n == NO_ACTOR)
		return;

	LoadString (hInstance, IDS_CHAOS_PURGED, message, sizeof(message));
	display->DisplayMessage (message);

    player->EvokedFX().Activate(Arts::CHAOS_PURGE, false);

	// send to 885;3898;42 
	player->Teleport (885, 3898, 0, 42);	

	return;
}

void cArts::EndChaosPurge(void)
{
	cNeighbor *n = cp->SelectedNeighbor();

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::CHAOS_PURGE);
		this->ArtFinished(false);
		return;
	} 
	else if (!(n->flags & ACTOR_SOULSPHERE)) { // must be ss to target
		LoadString (hInstance, IDS_MUST_BE_SOULSPHERE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::CHAOS_PURGE));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}
			
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::CHAOS_PURGE, player->Skill(Arts::CHAOS_PURGE), 0);
	this->DisplayUsedOnOther(n, Arts::CHAOS_PURGE);
	this->ArtFinished(true);
	return;
}



//////////////////////////////////////////////////////////////////
// Cup Summons

void cArts::StartCupSummons(void)
{
	if (!player->IsKnight(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_KNIGHT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::CUP_SUMMONS));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}


	this->WaitForSelection(&cArts::EndCupSummons, Arts::CUP_SUMMONS);
	this->AddDummyNeighbor();
	this->CaptureCP(NEIGHBORS_TAB, Arts::CUP_SUMMONS);

	return;
}

void cArts::ApplyCupSummons(lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	this->DisplayUsedByOther(n, Arts::CUP_SUMMONS);

	if (n == NO_ACTOR)
		return;

    player->EvokedFX().Activate(Arts::CUP_SUMMONS, false);

	if (!acceptrejectdlg)
	{
		LoadString (hInstance, IDS_CUPSUMMONS_ATTEMPT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
			HWND hDlg = CreateLyraDialog(hInstance, (IDD_ACCEPTREJECT),
							cDD->Hwnd_Main(), (DLGPROC)AcceptRejectDlgProc);
			acceptreject_callback = (&cArts::GotCupSummoned);
			SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
			SendMessage(hDlg, WM_SET_AR_NEIGHBOR, 0, (LPARAM)n);
	}
	return;
}

void cArts::GotCupSummoned(void *value)
{
	int success = *((int*)value);

	if (success)
	{
		player->Teleport (6958, 7522, 979, 46);	// Cup Arena
	}

//	gs->Talk(

	return;
}


const int CUP_SUMMONS_POWER_TOKENS = 1;

void cArts::EndCupSummons(void)
{
	cNeighbor *n = cp->SelectedNeighbor();

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::NO_GUILD);

	if (num_tokens < CUP_SUMMONS_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_NEED_POWER_TOKEN, message, sizeof(message));
		_stprintf(disp_message, message, arts->Descrip(Arts::CUP_SUMMONS));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::CUP_SUMMONS);
		this->ArtFinished(false);
		return;
	} 
	else if (n->flags & ACTOR_SOULSPHERE)
	{ // can't be ss to target
		LoadString (hInstance, IDS_CUP_NO_SS, message, sizeof(message));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	if (n->Room() != player->Room())
	{
		LoadString (hInstance, IDS_NEED_SAME_ROOM, message, sizeof(message));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}
	

	if (!(level->Rooms[player->Room()].flags & ROOM_SANCTUARY))
	{
		LoadString (hInstance, IDS_NEED_SANCUTARY, message, sizeof(message));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}


	for (int i=0; i<CUP_SUMMONS_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	if (n->ID() == player->ID())
	{
		player->Teleport (6958, 7522, 979, 46);	// Cup Arena
	}
	else
	{
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::CUP_SUMMONS, player->Skill(Arts::CUP_SUMMONS), 0);
		this->DisplayUsedOnOther(n, Arts::CUP_SUMMONS);
	}

	this->ArtFinished(true);
	return;
}

////////////////////////////////////////////////////////////////
// *** Arts that require selecting a skill ***
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Self Train

void cArts::StartTrainSelf(void)
{
	this->WaitForSelection(&cArts::EndTrainSelf, Arts::TRAIN_SELF);
	this->CaptureCP(ARTS_TAB, Arts::TRAIN_SELF);
	return;
}

void cArts::EndTrainSelf(void)
{
	lyra_id_t art_id = cp->SelectedArt();
	cItem* tokens[Lyra::INVENTORY_MAX]; // holds token pointers

	int num_tokens = 0;

	if (player->Skill(Arts::TRAIN) == 0)
	{
		LoadString (hInstance, IDS_NEED_TRAIN, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	if ((art_info[art_id].stat != player->FocusStat()) &&
		art_info[art_id].restricted())
	{
		LoadString (hInstance, IDS_NOT_FOCUS_SELFTRAIN, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	if ((art_info[art_id].stat != Stats::DREAMSOUL) &&
		(art_info[art_id].stat != player->FocusStat()) &&
		(art_info[art_id].stat != Stats::NO_STAT) &&
		(player->Skill(art_id)>19))
	{
		LoadString (hInstance, IDS_NOT_FOCUS_LIMIT_SELFTRAIN, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;

	}

  if ((player->Skill(Arts::TRAIN) / 10) < (player->Skill(art_id) / 10))
  {
    LoadString (hInstance, IDS_SELFTRAIN_LOW_TRAIN, disp_message, sizeof(disp_message));
    display->DisplayMessage (disp_message);
    this->ArtFinished (false);
    return;
  }

	if (((player->Skill(art_id)%10) != 9) ||
		((player->Skill(art_id)+1) > player->Orbit()))
	{
		LoadString (hInstance, IDS_NOT_READY_SELFTRAIN, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	int skill_sphere = (int)((player->Skill(art_id)+1)/10);
	int num_tokens_required = skill_sphere;
	if (num_tokens_required < 3)
		num_tokens_required = 3;

	switch (art_id) {
		case Arts::NONE:
		case Arts::FORGE_TALISMAN:
		case Arts::TRAIN:
		case Arts::LEVELTRAIN:
		case Arts::SUPPORT_TRAINING:
		case Arts::SUPPORT_SPHERING:
		case Arts::TRAIN_SELF:
		case Arts::TEHTHUS_OBLIVION:
		LoadString (hInstance, IDS_NO_SELF_TRAIN, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
		default:
			break;
	}

	num_tokens = CountTrainSphereTokens(art_id, player->ID(), (cItem**)tokens, true);

	if (num_tokens >= num_tokens_required)
		gs->SendPlayerMessage(player->ID(), RMsg_PlayerMsg::TRAIN_SELF, art_id, 0);
	else
	{	// failure!
		LoadString (hInstance, IDS_TRAIN_SELF_FAILED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, arts->Descrip(art_id), num_tokens_required, num_tokens);
		display->DisplayMessage (message);
	}

	this->ArtFinished(true);
}

// helper method 
int cArts::CountTrainSphereTokens(lyra_id_t art_id, lyra_id_t target_id, cItem** tokens, 
								  bool unique)
{
	int num_tokens = 0;
	cItem* item;
	lyra_item_train_support_t support[Lyra::INVENTORY_MAX];	// holds token info
	const void* state;
	bool duplicate = false;

	// check that the proper # of unique support tokens are carried
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::SUPPORT_TRAIN_FUNCTION))
		{ // potential candidate for token; support is always item function zero
			state = item->Lmitem().StateField(0);
			memcpy(&support[num_tokens], state, sizeof(lyra_item_train_support_t));
			// check for correct target i
			if ((support[num_tokens].target_id() != target_id) ||
				(support[num_tokens].art_id != art_id))
				continue;

			//if (art_id == SPHERE_TOKEN_ART_ID)
			//{/ // sphere ascension; check vs sphere level
				//if (support[num_tokens].art_level < level_needed)
					//continue; // level check eliminated
			//} 
			//else // training art; check vs art level
//			if (art_id != SPHERE_TOKEN_ART_ID)
	//		{
	//			if (((int)(support[num_tokens].art_level/10) < (int)(level_needed/10)))
	//				continue;
//			} // level check eliminated.

			duplicate = false;
			int i = 0;
			for (i=0; i<num_tokens; i++)
				if (support[i].creator_id() == support[num_tokens].creator_id())
					duplicate = true;
			 // if we want unique tokens, check for duplicates from a single player
			if (unique && duplicate)
				continue;

			tokens[i] = item;
			num_tokens++; // valid token!!!
			
		}
	actors->IterateItems(DONE);
	return num_tokens;
}


void cArts::ResponseTrainSelf(int art_id, int success)
{
	int i, num_tokens = 0;
	cItem* tokens[Lyra::INVENTORY_MAX]; // holds token pointers

	int skill_sphere = (int)(player->Skill(art_id)/10);
	int num_tokens_required = 3 + skill_sphere - 2;
	if (num_tokens_required < 3)
		num_tokens_required = 3;


	if (success)
	{	// success!!!
		num_tokens = CountTrainSphereTokens(art_id, player->ID(), (cItem**)tokens, false);
		player->SetSkill(art_id, 1, SET_RELATIVE, SERVER_UPDATE_ID, true);
		LoadString (hInstance, IDS_TRAIN_SELF_SUCCEEDED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, arts->Descrip(art_id));
		display->DisplayMessage (message);
		for (i=0; i<num_tokens_required; i++)
			tokens[i]->Destroy();
		gs->UpdateServer();
	}
	else
	{	// failure!
		num_tokens = CountTrainSphereTokens(art_id, player->ID(), (cItem**)tokens, true);
		LoadString (hInstance, IDS_TRAIN_SELF_FAILED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, arts->Descrip(art_id), num_tokens_required, num_tokens);
		display->DisplayMessage (message);
	}

}

////////////////////////////////////////////////////////////////
// *** Arts that require selecting an item ***
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Reweave

void cArts::StartReweave(void)
{
	this->WaitForSelection(&cArts::EndReweave, art_in_use);
	this->CaptureCP(INVENTORY_TAB, art_in_use);
	return;
}

void cArts::EndReweave(void)
{
	cItem *item = cp->SelectedItem();
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(art_in_use);
		this->ArtFinished(false);
		return;
	} // do the reweave
	cDS->PlaySound(LyraSound::REWEAVE, player->x, player->y, true);
	this->ArtFinished(item->Reweave((player->SkillSphere(art_in_use))+1));
	return;
}

//////////////////////////////////////////////////////////////////
// Recharge

void cArts::StartRecharge(void)
{
	this->WaitForSelection(&cArts::EndRecharge, Arts::RECHARGE_TALISMAN);
	this->CaptureCP(INVENTORY_TAB, Arts::RECHARGE_TALISMAN);
	return;
}

void cArts::EndRecharge(void)
{

	cItem *item = cp->SelectedItem();
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(Arts::RECHARGE_TALISMAN);
		this->ArtFinished(false);
		return;
	} // do the recharge
	if (item->Recharge(player->SkillSphere(Arts::RECHARGE_TALISMAN)+1))
	{
		cDS->PlaySound(LyraSound::RECHARGE, player->x, player->y, true);
		this->ArtFinished(true);
	}
	else
		this->ArtFinished(false);

	return;
}


//////////////////////////////////////////////////////////////////
// Identify

// arts that require selecting an item
void cArts::StartIdentify(void)
{
	this->WaitForSelection(&cArts::EndIdentify, Arts::IDENTIFY);
	this->CaptureCP(INVENTORY_TAB, Arts::IDENTIFY);
	return;
}

void cArts::EndIdentify(void)
{

	cItem *item = cp->SelectedItem();
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(Arts::IDENTIFY);
		this->ArtFinished(false);
		return;
	} // do the identify
	if (item->Identify(true))
	{
		cDS->PlaySound(LyraSound::IDENTIFY, player->x, player->y, true);
		this->ArtFinished(true);
	}
	else
		this->ArtFinished(false);
	return;
}

//////////////////////////////////////////////////////////////////
// Banish Nightmare

void cArts::StartBanishMare(void)
{
	this->WaitForSelection(&cArts::EndBanishMare, Arts::BANISH_NIGHTMARE);
	this->CaptureCP(INVENTORY_TAB, Arts::BANISH_NIGHTMARE);
	return;
}

void cArts::EndBanishMare(void)
{
	LmItem info;
	LmItemHdr header;
	lyra_item_essence_t essence;
	cItem *essence_item = cp->SelectedItem();
	cItem *banished_item;
	bool is_essence = false;
	const void* state;


	if ((essence_item == NO_ACTOR) || !(actors->ValidItem(essence_item)))
	{
		this->DisplayItemBailed(Arts::BANISH_NIGHTMARE);
		this->ArtFinished(false);
		return;
	}
	// check that the item has mare essence
	for (int i=0; i<essence_item->NumFunctions(); i++)
		if (essence_item->ItemFunction(i) == LyraItem::ESSENCE_FUNCTION)
		{
			state = essence_item->Lmitem().StateField(i);
			memcpy(&essence, state, sizeof(essence));
			if ((essence.mare_type >= Avatars::MIN_NIGHTMARE_TYPE) && (essence.strength > 0))
				is_essence = true;
		}

	if (is_essence)
	{ // transmute into banished talisman
		// create new talisman for banished mare essence
		header.Init(0, 0);
		header.SetFlags(LyraItem::FLAG_IMMUTABLE);
		header.SetGraphic(LyraBitmap::BANISHED_MARE);
		header.SetColor1(0); header.SetColor2(0);
		header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::ESSENCE_FUNCTION), 0, 0));
		essence.strength = 0; // leave other fields alone

		LoadString(hInstance, IDS_BANISHED_NIGHTMARE, message, sizeof(message));
		info.Init(header, message, 0, 0, 0);
		info.SetStateField(0, &essence, sizeof(essence));
		info.SetCharges(1);
		int ttl = 120;
		banished_item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
		if (banished_item == NO_ITEM)
		{
			this->ArtFinished(false);
			return;
		}
		LoadString (hInstance, IDS_NIGHTMARE_BANISHED, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		essence_item->Destroy();
		this->ArtFinished(true);
	}
	else
	{
		LoadString (hInstance, IDS_NOT_ESSENCE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Enslave Nightmare

void cArts::StartEnslaveMare(void)
{
	this->WaitForSelection(&cArts::EndEnslaveMare, Arts::ENSLAVE_NIGHTMARE);
	this->CaptureCP(INVENTORY_TAB, Arts::ENSLAVE_NIGHTMARE);
	return;
}

void cArts::EndEnslaveMare(void)
{
	LmItem info;
	LmItemHdr header;
	lyra_item_essence_t essence;
	cItem *essence_item = cp->SelectedItem();
	cItem *slave_item;
	bool is_essence = false;
	const void* state;


	if ((essence_item == NO_ACTOR) || !(actors->ValidItem(essence_item)))
	{
		this->DisplayItemBailed(Arts::ENSLAVE_NIGHTMARE);
		this->ArtFinished(false);
		return;
	}
	// check that the item has mare essence
	for (int i=0; i<essence_item->NumFunctions(); i++)
		if (essence_item->ItemFunction(i) == LyraItem::ESSENCE_FUNCTION)
		{
			state = essence_item->Lmitem().StateField(i);
			memcpy(&essence, state, sizeof(essence));
			if ((essence.mare_type >= Avatars::MIN_NIGHTMARE_TYPE) && (essence.strength > 0))
				is_essence = true;
		}

	if (is_essence)
	{	// transmute into enslaved talisman

		// create new talisman for enslaved mare essence
		header.Init(0, 0);
		header.SetFlags(LyraItem::FLAG_IMMUTABLE);
		header.SetGraphic(LyraBitmap::ENSLAVED_MARE);
		header.SetColor1(0); header.SetColor2(0);
		header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::ESSENCE_FUNCTION), 0, 0));
		essence.strength = 0; // leave other fields alone

		LoadString(hInstance, IDS_IMPRISONED_NIGHTMARE, message, sizeof(message));
		info.Init(header, message, 0, 0, 0);
		info.SetStateField(0, &essence, sizeof(essence));
		info.SetCharges(1);
		int ttl = 120;
		slave_item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
		if (slave_item == NO_ITEM)
		{
			this->ArtFinished(false);
			return;
		}
		LoadString (hInstance, IDS_NIGHTMARE_ENSLAVED, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		essence_item->Destroy();
		this->ArtFinished(true);
	}
	else
	{
		LoadString (hInstance, IDS_NOT_ESSENCE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Drain Nightmare

void cArts::StartDrainMare(void)
{
	this->WaitForSelection(&cArts::EndDrainMare, Arts::DRAIN_NIGHTMARE);
	this->CaptureCP(INVENTORY_TAB, Arts::DRAIN_NIGHTMARE);
	return;
}

void cArts::EndDrainMare(void)
{

	cItem *item = cp->SelectedItem();
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(Arts::DRAIN_NIGHTMARE);
		this->ArtFinished(false);
		return;
	} // do the drain

	if (item->DrainEssence((player->SkillSphere(Arts::DRAIN_NIGHTMARE))+1))
	{
		LoadString (hInstance, IDS_DRAINED_MARE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message, false);
		cDS->PlaySound(LyraSound::DRAIN_ESSENCE, player->x, player->y, true);
		this->ArtFinished(true);
	}
	else
	{
#ifndef PMARE
		LoadString (hInstance, IDS_NOT_ESSENCE, disp_message, sizeof(disp_message));
#else
		LoadString (hInstance, IDS_PMARE_NOT_ESSENCE, disp_message, sizeof(disp_message));
#endif
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Cleanse Nightmare

void cArts::StartCleanseMare(void)
{
	this->WaitForSelection(&cArts::EndCleanseMare, Arts::CLEANSE_NIGHTMARE);
	this->CaptureCP(INVENTORY_TAB, Arts::CLEANSE_NIGHTMARE);
	return;
}

void cArts::EndCleanseMare(void)
{
	LmItem info;
	LmItemHdr header;
	lyra_item_essence_t essence;
	cItem *essence_item = cp->SelectedItem();
	cItem *banished_item;
	bool is_essence = false;
	const void* state;


	if ((essence_item == NO_ACTOR) || !(actors->ValidItem(essence_item)))
	{
		this->DisplayItemBailed(Arts::CLEANSE_NIGHTMARE);
		this->ArtFinished(false);
		return;
	}

	// only works for GOE

//	if (!(player->GuildRank(Guild::ENTRANCED) >= Guild::INITIATE))
//	{
//		LoadString (hInstance, IDS_MUST_BE_GOE, message, sizeof(message));
//		display->DisplayMessage(message);
//		this->ArtFinished(false);
//		return;
//	}


	// check that the item has mare essence
	for (int i=0; i<essence_item->NumFunctions(); i++)
		if (essence_item->ItemFunction(i) == LyraItem::ESSENCE_FUNCTION)
		{
			state = essence_item->Lmitem().StateField(i);
			memcpy(&essence, state, sizeof(essence));
			if ((essence.mare_type >= Avatars::MIN_NIGHTMARE_TYPE) && (essence.strength > 0))
				is_essence = true;
		}

	if (is_essence)
	{ // transmute into banished talisman
		// create new talisman for cleansed mare essence
		header.Init(0, 0);
		header.SetFlags(LyraItem::FLAG_IMMUTABLE);
		header.SetGraphic(LyraBitmap::CLEANSED_MARE);
		header.SetColor1(0); header.SetColor2(0);
		header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::ESSENCE_FUNCTION), 0, 0));
		essence.strength = 0; // leave other fields alone

		LoadString(hInstance, IDS_CLEANSED_NIGHTMARE, message, sizeof(message));
		info.Init(header, message, 0, 0, 0);
		info.SetStateField(0, &essence, sizeof(essence));
		info.SetCharges(1);
		int ttl = 120;
		banished_item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
		if (banished_item == NO_ITEM)
		{
			this->ArtFinished(false);
			return;
		}
		LoadString (hInstance, IDS_NIGHTMARE_CLEANSED, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		essence_item->Destroy();
		this->ArtFinished(true);
	}
	else
	{
		LoadString (hInstance, IDS_NOT_ESSENCE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
	}
	return;
}

//////////////////////////////////////////////////////////////////
// Corrupt Essence

void cArts::StartCorruptEssence(void)
{
	this->WaitForSelection(&cArts::EndCorruptEssence, Arts::CORRUPT_ESSENCE);
	this->CaptureCP(INVENTORY_TAB, Arts::CORRUPT_ESSENCE);
	return;
}

void cArts::EndCorruptEssence(void)
{
	LmItem info;
	LmItemHdr header;
	lyra_item_essence_t essence;
	cItem *essence_item = cp->SelectedItem();
	cItem *banished_item;
	bool is_essence = false;
	const void* state;


	if ((essence_item == NO_ACTOR) || !(actors->ValidItem(essence_item)))
	{
		this->DisplayItemBailed(Arts::CORRUPT_ESSENCE);
		this->ArtFinished(false);
		return;
	}

	// only works for KOES

	if (!(player->GuildRank(Guild::SHADOW) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_KOES, message, sizeof(message));
		display->DisplayMessage(message);
		this->ArtFinished(false);
		return;
	}


	// check that the item has dreamer essence
	for (int i=0; i<essence_item->NumFunctions(); i++)
		if (essence_item->ItemFunction(i) == LyraItem::ESSENCE_FUNCTION)
		{
			state = essence_item->Lmitem().StateField(i);
			memcpy(&essence, state, sizeof(essence));
			if (((essence.mare_type == Avatars::MALE) ||
				 (essence.mare_type == Avatars::FEMALE)) &&
				(essence.strength > 0))
				is_essence = true;
		}

	if (is_essence)
	{ // transmute into banished talisman
		// create new talisman for cleansed mare essence
		header.Init(0, 0);
		header.SetFlags(LyraItem::FLAG_IMMUTABLE);
		header.SetGraphic(LyraBitmap::EMPHANT_ESSENCE);
		header.SetColor1(0); header.SetColor2(0);
		header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::ESSENCE_FUNCTION), 0, 0));
		essence.strength = 1; // leave other fields alone
		essence.mare_type = Avatars::EMPHANT;

		LoadString(hInstance, IDS_ESSENCE_NODE, message, sizeof(message));
		info.Init(header, message, 0, 0, 0);
		info.SetStateField(0, &essence, sizeof(essence));
		info.SetCharges(1);
		int ttl = 120;
		banished_item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
		if (banished_item == NO_ITEM)
		{
			this->ArtFinished(false);
			return;
		}
		LoadString (hInstance, IDS_ESSENCE_CORRUPTED, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		essence_item->Destroy();
		this->ArtFinished(true);
	}
	else
	{
		LoadString (hInstance, IDS_NOT_DREAMER_ESSENCE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
	}
	return;
}



//////////////////////////////////////////////////////////////////
// DestroyItem

void cArts::StartDestroyItem(void)
{
	this->WaitForSelection(&cArts::EndDestroyItem, Arts::DESTROY_ITEM);
	this->CaptureCP(INVENTORY_TAB, Arts::DESTROY_ITEM);
	return;
}

void cArts::EndDestroyItem(void)
{
	cItem *item = cp->SelectedItem();
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(Arts::DESTROY_ITEM);
		this->ArtFinished(false);
		return;
	}



#ifdef GAMEMASTER
	// If I'm a GM or Debugger, destroy the item
	item->SetWantDestroyAck(true);
	item->Destroy();

	this->ArtFinished(true);
#else
	// If a regular player, make sure it's finitely charged nonartifact
	if (item->NoReap() || (item->Lmitem().Charges() >= (INFINITE_CHARGES - 1)) ||
		(item->Status() != ITEM_OWNED))
	{
		LoadString (hInstance, IDS_DESTROY_FAILED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, item->Name());
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	item->SetWantDestroyAck(true);
	item->Destroy();

	this->ArtFinished(true);
#endif
	return;
}


//////////////////////////////////////////////////////////////////
// Sacrifice

void cArts::StartSacrifice(void)
{
	this->WaitForSelection(&cArts::EndSacrifice, Arts::SACRIFICE);
	this->CaptureCP(INVENTORY_TAB, Arts::SACRIFICE);
	return;
}

void cArts::EndSacrifice(void)
{
	LmItem info;
	LmItemHdr header;
	lyra_item_missile_t missile;
	cItem *chakram_item = cp->SelectedItem();
	cItem *banished_item;
	lyra_item_essence_t essence;
	bool is_missile = false;
	const void* state;

	if ((chakram_item == NO_ACTOR) || !(actors->ValidItem(chakram_item)))
	{
		this->DisplayItemBailed(Arts::SACRIFICE);
		this->ArtFinished(false);
		return;
	}

	// only works for AOE

	if (!(player->GuildRank(Guild::ECLIPSE) >= Guild::INITIATE))
	{
		LoadString (hInstance, IDS_MUST_BE_AOE, message, sizeof(message));
		display->DisplayMessage(message);
		this->ArtFinished(false);
		return;
	}

	// check that the item is a chakram or blade
	for (int i=0; i<chakram_item->NumFunctions(); i++)
		if (chakram_item->ItemFunction(i) == LyraItem::MISSILE_FUNCTION)
		{
			state = chakram_item->Lmitem().StateField(i);
			memcpy(&missile, state, sizeof(missile));
			if (missile.velocity != 0)
				is_missile = true;
		}

	if (is_missile)
	{ // transmute into an imprisoned talisman
      // create new talisman for imprisoned mare essence
		header.Init(0, 0);
		header.SetFlags(LyraItem::FLAG_IMMUTABLE);
		header.SetGraphic(LyraBitmap::ENSLAVED_MARE);
		header.SetColor1(0); header.SetColor2(0);
		header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::ESSENCE_FUNCTION), 0, 0));
		essence.type = LyraItem::ESSENCE_FUNCTION;
		essence.strength = 1 + player->SkillSphere(Arts::SACRIFICE);
		essence.mare_type = 2;
		essence.slaver_id = player->ID();
		essence.weapon_type = 0;

		LoadString(hInstance, IDS_ESSENCE_NODE, message, sizeof(message));
		info.Init(header, message, 0, 0, 0);
		info.SetStateField(0, &essence, sizeof(essence));
		info.SetCharges(1);
		int ttl = 120;
		banished_item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
		if (banished_item == NO_ITEM)
		{
			this->ArtFinished(false);
			return;
		}
		LoadString (hInstance, IDS_CHAKRAM_SACRIFICED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, chakram_item->Name());
		display->DisplayMessage (message);
		chakram_item->Destroy();
		this->ArtFinished(true);
	}
	else
	{
		LoadString (hInstance, IDS_NOT_CHAKRAM, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
	}
	return;
}




////////////////////////////////////////////////////////////////
// *** Arts that require selecting an art and a neighbor ***
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Train

void cArts::StartTrain(void)
{
	this->WaitForSelection(&cArts::MidTrain, Arts::TRAIN);
	this->CaptureCP(ARTS_TAB, Arts::TRAIN);
	return;
}

void cArts::MidTrain(void)
{
	this->WaitForSelection(&cArts::EndTrain, Arts::TRAIN);
	this->CaptureCP(NEIGHBORS_TAB, Arts::TRAIN);
	return;
}

void cArts::ApplyTrain(int art_id, int success, lyra_id_t caster_id)
{
	int skill = success;
	cNeighbor *n = this->LookUpNeighbor(caster_id);

	if (n == NO_ACTOR)
		return;

	player->EvokedFX().Activate(Arts::TRAIN, false);


	if (success)
	{
		player->SetSkill(art_id, skill, SET_ABSOLUTE, SERVER_UPDATE_ID, true);
		LoadString (hInstance, IDS_TRAIN_SUCCEEDED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name(), this->Descrip(art_id), player->Skill(art_id));
		LmAvatar avatar = player->Avatar();
		if (art_id == Arts::DREAMSMITH_MARK) 
		{
			avatar.SetDreamSmith(1);
			player->SetAvatar(avatar, true);
		}
		if (art_id == Arts::WORDSMITH_MARK) 
		{
			avatar.SetWordSmith(1);
			player->SetAvatar(avatar, true);
		}
		if (art_id == Arts::DREAMSTRIKE) 
		{
			avatar.SetDreamstrike(1);
			player->SetAvatar(avatar, true);
		}
		if (art_id == Arts::NP_SYMBOL) 
		{
			avatar.SetNPSymbol(1);
			player->SetAvatar(avatar, true);
		}


	}
	else
	{
		LoadString (hInstance, IDS_TRAIN_FAILED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name(), this->Descrip(art_id));
	}

	display->DisplayMessage(message);

	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::TRAIN_ACK, success, 0);

	if (success)
	{
		LoadString (hInstance, IDS_TRAIN_OTHER_SUCCEEDED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, player->Name(), player->Skill(art_id), this->Descrip(art_id));
	}
	else
	{
		LoadString (hInstance, IDS_TRAIN_OTHER_FAILED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, player->Name(), this->Descrip(art_id));
	}
	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);


	return;
}

cItem* cArts::HasQuestCodex(lyra_id_t neighbor_id, lyra_id_t art_id)
{
	const void* state;
	cItem* item;
	lyra_item_scroll_t scroll;
	cItem* quest_codex = NO_ITEM;

	// check that a Quest codex with the proper target id and art id is in inventory
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::SCROLL_FUNCTION))
		{ // potential candidate for quest scroll
			state = item->Lmitem().StateField(0);
			memcpy(&scroll, state, sizeof(lyra_item_scroll_t));
			int tid = scroll.targetid();
			if (neighbor_id != scroll.targetid())
				continue; 
			// 1 is added to art ID to allow use of art 0
			if (art_id != scroll.art_id -1)
				continue;
			
			quest_codex = item;
			break;
		}
	actors->IterateItems(DONE);
	return quest_codex;
}


void cArts::EndTrain(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	quest_item = NO_ITEM;
	lyra_id_t art_id = cp->SelectedArt();

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::TRAIN);
		this->ArtFinished(false);
		return;
	} else if ((art_id == Arts::FORGE_TALISMAN) &&
			   (0 == player->Skill(Arts::DREAMSMITH_MARK)))
	{
		LoadString (hInstance, IDS_DREAMSMITH_MARK_REQUIRED, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
	else if (n->IsMonster())
	{
		LoadString (hInstance, IDS_NO_TRAIN_MONSTER, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
#ifndef GAMEMASTER //
	else if ((art_id == Arts::TRAIN) ||
		     (art_id == Arts::LEVELTRAIN) ||
			 (art_id == Arts::DREAMSTRIKE) || 
		     (art_id ==Arts::SUPPORT_SPHERING) ||
             (art_id == Arts::SUPPORT_TRAINING) ||
			 (art_id == Arts::GUILDHOUSE) || 
			 (art_id == Arts::TEHTHUS_OBLIVION) ||
			 (art_id == Arts::CHAOS_PURGE) ||
			 (art_id == Arts::FREESOUL_BLADE) ||
			 (art_id == Arts::ILLUMINATED_BLADE) ||
			 (art_id == Arts::CUP_SUMMONS))
	{
		LoadString (hInstance, IDS_GM_ONLY_TRAIN, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
#endif
	// in order to train, teacher must be holding a quest codex for the appropriate
	// player and art

	// necessity of Quest item below 
	else if ((quest_item = HasQuestCodex(n->ID(), art_id)) == NO_ITEM)
	{
		LoadString (hInstance, IDS_NEED_QUEST_ITEM, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;

	}
#ifndef GAMEMASTER	//GMs SHOULD be allowed to train arts even if not within their primary focus
	else if (art_info[art_id].restricted() && (player->Avatar().Focus() != n->Avatar().Focus()))
	{
		LoadString (hInstance, IDS_TRAIN_OTHER_FAILED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name(), this->Descrip(art_id));
		display->DisplayMessage(message);
		this->ArtFinished(false);
		return;
	}
#endif
	else
	{ 
		// skill is set at the lower of either teaching or the skill itself
		int skill = player->Skill(Arts::TRAIN);
		if (player->Skill(art_id)<skill)
			skill = player->Skill(art_id);
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::TRAIN, art_id, skill);
		this->ArtFinished(true);
	}
	return;
}

// this function only exists to destroy the Quest item on successful train,
// so an unsuccessful train won't destroy it
void cArts::CompleteTrain(int success, lyra_id_t target_id)
{
	if (success && actors->ValidItem(quest_item))
		quest_item->Destroy();

	quest_item = NULL;
}


//////////////////////////////////////////////////////////////////
// UnTrain

void cArts::StartUnTrain(void)
{
#ifdef GAMEMASTER
	this->WaitForSelection(&cArts::MidUnTrain, Arts::UNTRAIN);
	this->CaptureCP(ARTS_TAB, Arts::UNTRAIN);
#else
	LoadString (hInstance, IDS_GM_ONLY, disp_message, sizeof(disp_message));
	display->DisplayMessage (disp_message);
	this->ArtFinished(true);
#endif
	return;
}

void cArts::MidUnTrain(void)
{
	this->WaitForSelection(&cArts::EndUnTrain, Arts::UNTRAIN);
	this->CaptureCP(NEIGHBORS_TAB, Arts::UNTRAIN);
	return;
}

void cArts::ApplyUnTrain(int art_id, lyra_id_t caster_id)
{
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (n == NO_ACTOR)
		return;

	player->EvokedFX().Activate(Arts::UNTRAIN, false);
	if (player->Skill(art_id)) {
		player->SetSkill(art_id, 0, SET_ABSOLUTE, caster_id, true);

		LoadString (hInstance, IDS_UNTRAIN, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name(), this->Descrip(art_id));
		display->DisplayMessage(message);

		LoadString (hInstance, IDS_UNTRAIN_OTHER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, player->Name(), this->Descrip(art_id));
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);

		LmAvatar avatar = player->Avatar();
		if ((art_id == Arts::DREAMSTRIKE) || 
			(art_id == Arts::WORDSMITH_MARK) ||
			(art_id == Arts::DREAMSMITH_MARK))
		{	// this will reset the fields properly
			player->SetAvatar(avatar, true);
		}

		return;
	}
	else 
	{
	LoadString(hInstance, IDS_UNTRAIN_FAIL, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, player->Name(), this->Descrip(art_id));
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
	}
}

void cArts::EndUnTrain(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	lyra_id_t art_id = cp->SelectedArt();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::UNTRAIN);
		this->ArtFinished(false);
		return;
	}
	else if (n->IsMonster())
	{
		LoadString (hInstance, IDS_NO_UNTRAIN_MONSTER, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}
	else
	{
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::UNTRAIN, art_id, 0);
		this->ArtFinished(true);
	}
	return;
}


//////////////////////////////////////////////////////////////////
// Support Training

void cArts::StartSupportTraining(void)
{

	this->WaitForSelection(&cArts::MidSupportTraining, Arts::SUPPORT_TRAINING);
	this->CaptureCP(ARTS_TAB, Arts::SUPPORT_TRAINING);
	return;
}

void cArts::MidSupportTraining(void)
{
	this->WaitForSelection(&cArts::EndSupportTraining, Arts::SUPPORT_TRAINING);
	this->CaptureCP(NEIGHBORS_TAB, Arts::SUPPORT_TRAINING);
	return;
}

void cArts::EndSupportTraining(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	lyra_id_t art_id = cp->SelectedArt();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SUPPORT_TRAINING);
		this->ArtFinished(false);
		return;
	}
	else if (n->IsMonster())
	{
		LoadString (hInstance, IDS_NO_TRAIN_MONSTER, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	if (player->Skill(Arts::TRAIN) == 0)
	{
		LoadString (hInstance, IDS_NEED_TRAIN, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	switch (art_id) {
		case Arts::NONE:
		case Arts::FORGE_TALISMAN:
		case Arts::TRAIN:
		case Arts::LEVELTRAIN:
		case Arts::SUPPORT_TRAINING:
		case Arts::SUPPORT_SPHERING:
		case Arts::TRAIN_SELF:
		LoadString (hInstance, IDS_NO_SELF_TRAIN, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
		default:
			break;
	}

	// create the token
	LmItem info;
	LmItemHdr header;
	cItem *item;
	lyra_item_train_support_t support = {LyraItem::SUPPORT_TRAIN_FUNCTION, 0, 0, 0, 0, 0, 0};

	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
	header.SetGraphic(LyraBitmap::GUILD_ASCENSION_TOKEN);
	header.SetColor1(0); header.SetColor2(0);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_TRAIN_FUNCTION), 0, 0));

	support.art_id = art_id;
	support.art_level = player->Skill(art_id);
	support.set_creator_id(player->ID());
	support.set_target_id(n->ID());

_stprintf(message, _T("%s-%s"), arts->Descrip(art_id), n->Name());
	_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
	disp_message[LmItem::NAME_LENGTH-1] = '\0';

	info.Init(header, disp_message, 0, 0, 0);
	info.SetStateField(0, &support, sizeof(support));
	info.SetCharges(1);
	int ttl = 120;
	item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	LoadString (hInstance, IDS_SUPPORT_TRAIN_TOKEN, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, n->Name(), arts->Descrip(art_id));
	display->DisplayMessage (message);

	this->ArtFinished(true);
	return;

}

//////////////////////////////////////////////////////////////////
// Quest

void cArts::StartQuest(void)
{
	this->WaitForSelection(&cArts::MidQuest1, Arts::QUEST);
	this->CaptureCP(ARTS_TAB, Arts::QUEST);
	return;
}

void cArts::MidQuest1(void)
{
	this->WaitForSelection(&cArts::MidQuest2, Arts::QUEST);
	this->CaptureCP(NEIGHBORS_TAB, Arts::QUEST);
	return;
}

void cArts::MidQuest2(void)
{

	if (writescrolldlg)
	{
		this->ArtFinished(false);
		return;
	}

	HWND hDlg =  CreateLyraDialog(hInstance, IDD_WRITE_SCROLL,
					cDD->Hwnd_Main(), (DLGPROC)WriteScrollDlgProc);
	this->WaitForDialog(hDlg, Arts::QUEST);

	return;
}

void cArts::EndQuest(void *value)
{
	scroll_t *scroll_type = (scroll_t*)value;

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::QUEST);
		this->ArtFinished(false);
		return;
	}

	LmItem info;
	LmItemHdr header;
	int flags = 0;
	lyra_item_scroll_t scroll = {LyraItem::SCROLL_FUNCTION, 0, 0, 0, 0, 0};

	header.Init(0, 0);
	header.SetGraphic(LyraBitmap::CODEX);
	header.SetColor1(scroll_type->color1); header.SetColor2(scroll_type->color2);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SCROLL_FUNCTION),0,0));

	if (scroll_type->num_charges == 254)
		flags = LyraItem::FLAG_IMMUTABLE | LyraItem::FLAG_HASDESCRIPTION;
	else
		flags = LyraItem::FLAG_CHANGE_CHARGES | LyraItem::FLAG_HASDESCRIPTION;

	if (scroll_type->artifact)
		flags = flags | LyraItem::FLAG_NOREAP | LyraItem::FLAG_ALWAYS_DROP;

	header.SetFlags(flags);

	scroll.set_creatorid(player->ID());
	scroll.set_targetid(n->ID());
	scroll.art_id = cp->SelectedArt()+1;

	int tid = scroll.targetid();
	

	info.Init(header, scroll_type->name, 0, 0, 0);
	info.SetStateField(0, &scroll, sizeof(scroll));
	info.SetCharges(scroll_type->num_charges);

	// make scrolls last 4 hours
	cItem* item = CreateItem(player->x, player->y, player->angle, info, 0,
		false, GMsg_PutItem::DEFAULT_TTL*24, scroll_type->descrip);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	this->ArtFinished(true);

	return;

}


////////////////////////////////////////////////////////////////
// *** Arts that require selecting a neighbor and an item ***
////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Give

void cArts::StartGive(void)
{
	if (giving_item != NO_ACTOR)
	{
		LoadString (hInstance, IDS_ONE_GIVE_AT_A_TIME, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	if (cp->Giving()) // giving from cp button, so item is already selected
		this->MidGive();
	else
	{
		this->WaitForSelection(&cArts::MidGive, Arts::GIVE);
		this->CaptureCP(INVENTORY_TAB, Arts::GIVE);
	}
}

void cArts::ApplyGive(cItem *item, lyra_id_t caster_id)
{
#if ((defined PMARE) || (defined AGENT))
	LoadString (hInstance, IDS_AGENT_INFURIATED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, player->Name());
	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
	gs->TakeItemAck(GMsg_TakeItemAck::TAKE_NO, item);
	return;
#endif
	if (cp->InventoryFull() || (options.autoreject))
	{ // inv full, or we want to auto reject all gives
		gs->TakeItemAck(GMsg_TakeItemAck::TAKE_NO, item);
		return;
	}

	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (!acceptrejectdlg && (n != NO_ACTOR))
	{
		HWND prevDlg = GetFocus();
		receiving_item = item;
		LoadString (hInstance, IDS_QUERY_GIVE, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name(), item->Name());
		HWND hDlg = CreateLyraDialog(hInstance, (IDD_ACCEPTREJECT),
						cDD->Hwnd_Main(), (DLGPROC)AcceptRejectDlgProc);
		acceptreject_callback = (&cArts::GiveReply);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_SET_AR_NEIGHBOR, 0, (LPARAM)n);


		if (prevDlg) {
			SetActiveWindow(prevDlg);
			SetFocus(prevDlg);
		}
		else {
			SetActiveWindow(cDD->Hwnd_Main());
			SetFocus(cDD->Hwnd_Main());
		}
	}
	else
		gs->TakeItemAck(GMsg_TakeItemAck::TAKE_NO, item);
}

void cArts::GiveReply(void *value)
{
	int success = *((int*)value);

	if ((receiving_item == NO_ACTOR) || !(actors->ValidItem(receiving_item)))
		return;

	if (success)
		gs->TakeItemAck(GMsg_TakeItemAck::TAKE_YES, receiving_item);
	else
		gs->TakeItemAck(GMsg_TakeItemAck::TAKE_NO, receiving_item);
}

void cArts::MidGive(void)
{
	this->WaitForSelection(&cArts::EndGive, Arts::GIVE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::GIVE);
}

void cArts::EndGive(void)
{

	cNeighbor *n = cp->SelectedNeighbor();
	cItem *item = cp->SelectedItem();

	giving_item = NO_ACTOR;
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::GIVE);
		this->ArtFinished(false);
		return;
	}
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)) || (item->Redeeming()))
	{
		this->DisplayItemBailed(Arts::GIVE);
		this->ArtFinished(false);
		return;
	}
	if (item->Temporary() || (item->Status() != ITEM_OWNED))
	{
		LoadString (hInstance, IDS_NO_GIVE, disp_message, sizeof(disp_message));
		_stprintf(message, item->Name());
		display->DisplayMessage(message);
		this->ArtFinished(false);
		return;
	}

	giving_item = item;
	gs->GiveItem(item, n);
	this->ArtFinished(true);
	return;
}


//////////////////////////////////////////////////////////////////
// Show

void cArts::StartShow(void)
{
	this->WaitForSelection(&cArts::MidShow, Arts::SHOW);
	this->CaptureCP(INVENTORY_TAB, Arts::SHOW);
}

void cArts::ApplyShow(GMsg_ViewItem& view_item)
{
#if (defined (PMARE) || (defined AGENT))
	LoadString (hInstance, IDS_AGENT_INFURIATED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, player->Name());
	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, view_item.SourceID());
	return;
#endif

	cNeighbor *n = this->LookUpNeighbor(view_item.SourceID());
	if (n != NO_ACTOR)
	{
		LoadString (hInstance, IDS_SHOW_ITEM, disp_message, sizeof(disp_message));
		if (n->Avatar().AvatarType() == Avatars::MALE)
		_stprintf(message, disp_message, n->Name(), view_item.ItemName());
		else if (n->Avatar().AvatarType() == Avatars::FEMALE)
		_stprintf(message, disp_message, n->Name(), view_item.ItemName());
		else
		_stprintf(message, disp_message, n->Name(), view_item.ItemName());
		display->DisplayMessage(message);
	}
	return;
}


void cArts::MidShow(void)
{
	this->WaitForSelection(&cArts::EndShow, Arts::SHOW);
	this->CaptureCP(NEIGHBORS_TAB, Arts::SHOW);
}

void cArts::EndShow(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	cItem *item = cp->SelectedItem();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SHOW);
		this->ArtFinished(false);
		return;
	}
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(Arts::SHOW);
		this->ArtFinished(false);
		return;
	}
	gs->ShowItem(item, n);
	this->ArtFinished(true);
	return;
}

//////////////////////////////////////////////////////////////////
// Freesoul Blade

void cArts::StartFreesoulBlade(void)
{
	if (!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_RULER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::FREESOUL_BLADE));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::MidFreesoulBlade, Arts::FREESOUL_BLADE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::FREESOUL_BLADE);

	return;
}

void cArts::MidFreesoulBlade(void)
{
	if (player->NumGuilds(Guild::RULER) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::RULER));
		this->EndFreesoulBlade(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CHOOSE_GUILD,
			cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndFreesoulBlade);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::FREESOUL_BLADE);
	}
	return;
}

void cArts::EndFreesoulBlade(void *value)
{
	int guild_id = *((int*)value);
	LmItem info;
	LmItemHdr header;
	lyra_item_essence_t essence;
	lyra_item_missile_t missile;
	cItem *item = NO_ITEM;
	cNeighbor *n = cp->SelectedNeighbor();
	cItem *blade_item;
	const void* state;

	if (guild_id == Guild::NO_GUILD)
	{	 // player hit cancel
		this->ArtFinished(false);
		return;

	}

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::FREESOUL_BLADE);
		this->ArtFinished(false);
		return;
	}

	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION))
		{ // potential candidate for head; it's always in field 0
			if (item->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION)
			{
				state = item->Lmitem().StateField(0);
				memcpy(&essence, state, sizeof(essence));
				if ((essence.mare_type < Avatars::MIN_NIGHTMARE_TYPE) && // dreamer head, check name
					(0 == _tcscmp(item->Name(), n->Name())))
					break;
			}
	}

	if (item == NO_ACTOR) 
	{
		LoadString (hInstance, IDS_NEEDS_HEAD, message, sizeof(message));
		_stprintf(disp_message, message, this->Descrip(Arts::FREESOUL_BLADE));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
		return;
	}

	// nuke it and create a blade
	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_CHANGE_CHARGES | LyraItem::FLAG_HASDESCRIPTION);
	header.SetGraphic(LyraBitmap::DREAMBLADE);
	int color = (rand()%4) + (n->Avatar().Focus()*4) - 4;
	header.SetColor1(color); 
	header.SetColor2(color);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::MISSILE_FUNCTION), 0, 0));
	
	missile.type = LyraItem::MISSILE_FUNCTION;
	missile.bitmap_id = LyraBitmap::DREAMBLADE_MISSILE;
	missile.damage = 1;
	missile.effect = 0;
	missile.velocity = MELEE_VELOCITY;
	
	LoadString(hInstance, IDS_FREESOUL_BLADE, message, sizeof(message));
	info.Init(header, message, 0, 0, 0);
	info.SetStateField(0, &missile, sizeof(missile));
	info.SetCharges(99);  
	int ttl = 120;
	LoadString(hInstance, IDS_FREESOUL_KNIGHT_DESCRIP, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name(), player->Name(), GuildName(guild_id));
	blade_item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl, message);
	if (blade_item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}
	item->Destroy();
	this->ArtFinished(true);

	return;
}

//////////////////////////////////////////////////////////////////
// Illuminated Blade

void cArts::StartIlluminatedBlade(void)
{
	if (!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_RULER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::ILLUMINATED_BLADE));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::MidIlluminatedBlade, Arts::ILLUMINATED_BLADE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::ILLUMINATED_BLADE);

	return;
}

void cArts::MidIlluminatedBlade(void)
{
	if (player->NumGuilds(Guild::RULER) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::RULER));
		this->EndIlluminatedBlade(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CHOOSE_GUILD,
			cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndIlluminatedBlade);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::ILLUMINATED_BLADE);
	}
	return;
}

void cArts::EndIlluminatedBlade(void *value)
{
	int guild_id = *((int*)value);
	LmItem info;
	LmItemHdr header;
	lyra_item_essence_t essence;
	lyra_item_missile_t missile;
	cItem *item = NO_ITEM;
	cNeighbor *n = cp->SelectedNeighbor();
	cItem *blade_item;
	const void* state;

	if (guild_id == Guild::NO_GUILD)
	{	 // player hit cancel
		this->ArtFinished(false);
		return;

	}

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::ILLUMINATED_BLADE);
		this->ArtFinished(false);
		return;
	}

	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION))
		{ // potential candidate for head; it's always in field 0
			if (item->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION)
			{
				state = item->Lmitem().StateField(0);
				memcpy(&essence, state, sizeof(essence));
				if ((essence.mare_type < Avatars::MIN_NIGHTMARE_TYPE) && // dreamer head, check name
					(0 == _tcscmp(item->Name(), n->Name())))
					break;
			}
	}

	if (item == NO_ACTOR) 
	{
		LoadString (hInstance, IDS_NEEDS_HEAD, message, sizeof(message));
		_stprintf(disp_message, message, this->Descrip(Arts::ILLUMINATED_BLADE));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false);
		return;
	}

	// nuke it and create a blade
	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_CHANGE_CHARGES | LyraItem::FLAG_HASDESCRIPTION);
	header.SetGraphic(LyraBitmap::DREAMBLADE);
	int color = (rand()%4) + (n->Avatar().Focus()*4) - 4;
	header.SetColor1(color); 
	header.SetColor2(color);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::MISSILE_FUNCTION), 0, 0));
	
	missile.type = LyraItem::MISSILE_FUNCTION;
	missile.bitmap_id = LyraBitmap::DREAMBLADE_MISSILE;
	missile.damage = 1;
	missile.effect = 0;
	missile.velocity = MELEE_VELOCITY;
	
	LoadString(hInstance, IDS_ILLUMINATED_BLADE, message, sizeof(message));
	info.Init(header, message, 0, 0, 0);
	info.SetStateField(0, &missile, sizeof(missile));
	info.SetCharges(99);  
	int ttl = 120;
	LoadString(hInstance, IDS_ILLUMINATED_KNIGHT_DESCRIP, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name(), player->Name(), GuildName(guild_id));
	blade_item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl, message);
	if (blade_item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}
	item->Destroy();
	this->ArtFinished(true);

	return;
}


////////////////////////////////////////////////////////////////
// *** Arts that require selecting a neighbor and a guild ***
////////////////////////////////////////////////////////////////

// this is a helper function for all the House arts (Initiate, Knight, 
// Ascend), needed because these arts now drain a House prime
// if guild_id == Guild::NO_GUILD, return the first prime found

cItem* cArts::FindPrime(lyra_id_t guild_id, int min_charges)
{
	int num_tokens = 0;
	const void* state;
	cItem* item;
	cItem* prime = NO_ITEM;
	lyra_item_meta_essence_t meta_essence;
	
	// check that the proper # of ruler support tokens are carried
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::META_ESSENCE_FUNCTION))
		{ 
			state = item->Lmitem().StateField(0);
			memcpy(&meta_essence, state, sizeof(meta_essence));
			// the two lines below are a hack to give strength for testing
			//meta_essence.set_strength(meta_essence.strength() + 1300);
			//item->Lmitem().SetStateField(0, &meta_essence, sizeof(meta_essence));
			if ((guild_id != Guild::NO_GUILD) &&
				(meta_essence.guild_id != guild_id))
				continue;
			if (meta_essence.strength() >= min_charges)
			{
				prime = item;
				break;
			}
		}
	actors->IterateItems(DONE);

	return prime;

}


//////////////////////////////////////////////////////////////////
// Initiate

void cArts::StartInitiate(void)
{
	if (!player->IsKnight(Guild::NO_GUILD) && !player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_KNIGHT, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(Arts::INITIATE));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::MidInitiate, Arts::INITIATE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::INITIATE);

	return;
}

void cArts::ApplyInitiate(int guild_id, int success, lyra_id_t caster_id)
{
#if ((defined PMARE) || (defined AGENT))
	LoadString (hInstance, IDS_AGENT_INFURIATED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, player->Name());
	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
	return;
#endif

	cNeighbor *n = this->LookUpNeighbor(caster_id);
	if (n == NO_ACTOR)
		return;

	 player->EvokedFX().Activate(Arts::INITIATE, false);

	if (success && !acceptrejectdlg)
	{

		cDS->PlaySound(LyraSound::INITIATE, player->x, player->x, true);
		initiate_gid = guild_id;
		initiator_id = caster_id;
		LoadString (hInstance, IDS_INITIATE_ATTEMPT, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name(), GuildName(guild_id));
		HWND hDlg = CreateLyraDialog(hInstance, (IDD_ACCEPTREJECT),
						cDD->Hwnd_Main(), (DLGPROC)AcceptRejectDlgProc);
		_tcscpy(knight_name, n->Name());
		acceptreject_callback = (&cArts::GotInitiated);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_SET_AR_NEIGHBOR, 0, (LPARAM)n);
	}
	else // display failure messages
	{
		LoadString (hInstance, IDS_INITIATE_FAILED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name(), GuildName(guild_id));
		display->DisplayMessage (message);
		LoadString (hInstance, IDS_INITIATE_OTHER_FAILED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, player->Name(), GuildName(guild_id));
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
	}

	return;
}

void cArts::GotInitiated(void *value)
{
	int success = *((int*)value);

	gs->SendPlayerMessage(initiator_id, RMsg_PlayerMsg::INITIATE_ACK,
			initiate_gid, success);

	if (success)
	{
		LoadString (hInstance, IDS_INITIATE_SUCCEEDED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, knight_name, GuildName(initiate_gid));
		display->DisplayMessage (message);
		LoadString (hInstance, IDS_INITIATE_OTHER_SUCCEEDED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, player->Name(), GuildName(initiate_gid));
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, initiator_id);

		player->SetGuildRank(initiate_gid, Guild::INITIATE);
		player->SetGuildXPPool(initiate_gid, 0);

		// AUTO TRAIN INITIATE ARTS - 6/14/14 AMR
		if (player->Skill(Arts::HOUSE_MEMBERS)<1) {
			player->SetSkill(Arts::HOUSE_MEMBERS, 1, SET_ABSOLUTE, player->ID(), true);
			LoadString (hInstance, IDS_LEARNED_HOUSE_ART, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(Arts::HOUSE_MEMBERS));
		display->DisplayMessage (message);
		}
	}
	else
	{
		LoadString (hInstance, IDS_INITIATE_REJECTED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, knight_name, GuildName(initiate_gid));
		display->DisplayMessage (message);
		LoadString (hInstance, IDS_INITIATE_OTHER_REJECTED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, player->Name(), GuildName(initiate_gid));
		gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, initiator_id);
	}
	return; // real logic is in the dlg proc
}

void cArts::CompleteInitiate(int guild_id, int success, lyra_id_t initiate)
{
	if (success)
	{ // create the token
		LmItem info;
		LmItemHdr header;
		cItem *item;
		lyra_item_support_t support = {LyraItem::SUPPORT_FUNCTION, 0, 0, 0};

		header.Init(0, 0);
		header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
		header.SetGraphic(LyraBitmap::GUILD_MEMBER_TOKEN_BASE + guild_id);
		header.SetColor1(0); header.SetColor2(0);
		header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_FUNCTION), 0, 0));

		support.set_guild_token(guild_id, Tokens::MEMBERSHIP);
		support.set_creator_id(player->ID());
		support.set_target_id(initiate);

		LoadString(hInstance, IDS_HOUSE_ID, temp_message, sizeof(temp_message));
		_stprintf(message, temp_message, LookUpNeighbor(initiate)->Name());
		_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
		disp_message[LmItem::NAME_LENGTH-1] = '\0';

		info.Init(header, disp_message, 0, 0, 0);
		info.SetStateField(0, &support, sizeof(support));
		info.SetCharges(1);
		int ttl = 120;
		item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
	}
}

void cArts::MidInitiate(void)
{
	int knight_guilds = player->NumGuilds(Guild::KNIGHT);
	int ruler_guilds = player->NumGuilds(Guild::RULER);
	if (knight_guilds + ruler_guilds == 1)
	{ // only one choice, skip straight to end
		int value;
		if (player->NumGuilds(Guild::KNIGHT) == 1)
			value = GuildID(player->GuildFlags(Guild::KNIGHT));
		else
			value = GuildID(player->GuildFlags(Guild::RULER));
		this->EndInitiate(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, (IDD_CHOOSE_GUILD),
						cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndInitiate);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_KNIGHTS, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		this->WaitForDialog(hDlg, Arts::INITIATE);
	}
	return;
}

void cArts::EndInitiate(void *value)
{

	cNeighbor *n = cp->SelectedNeighbor();
	int guild_id = *((int*)value);
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::INITIATE);
		this->ArtFinished(false);
		return;
	}
	else if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}
	else
	{
		cItem* prime = FindPrime(guild_id, Arts::INITIATE_DRAIN);
		//if (0) // use this line to test server checks
		if (prime == NO_ITEM) 
		{
			LoadString (hInstance, IDS_NEED_PRIME, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, this->Descrip(Arts::INITIATE), this->Descrip(Arts::INITIATE), 
				GuildName(guild_id), GuildName(guild_id), Arts::INITIATE_DRAIN);
			display->DisplayMessage (message);
			this->ArtFinished(false);
		} 
		else 
		{
			prime->DrainMetaEssence(Arts::INITIATE_DRAIN);
			gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::INITIATE,
				guild_id, 1);
			this->ArtFinished(true);
		}

	}
	return;
}

//////////////////////////////////////////////////////////////////
// Knight

void cArts::StartKnight(void)
{
	if (!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_RULER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::KNIGHT));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::MidKnight, Arts::KNIGHT);
	this->CaptureCP(NEIGHBORS_TAB, Arts::KNIGHT);

	return;
}


void cArts::ApplyKnight(int guild_id, int success, lyra_id_t caster_id)
{
#if ((defined PMARE) || (defined AGENT))
	LoadString (hInstance, IDS_AGENT_INFURIATED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, player->Name());
	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);
	return;
#endif

	cNeighbor *n = this->LookUpNeighbor(caster_id);

	if (n == NO_ACTOR)
		return;
	 player->EvokedFX().Activate(Arts::KNIGHT, false);

	if (success)
	{
		player->SetGuildRank(guild_id, Guild::KNIGHT);

		// AUTO-TRAIN GUARDIAN ARTS - 6/14/14 AMR
		for (int art=0; art<NUM_ARTS; ++art) {
			switch (art) {
				case Arts::HOUSE_MEMBERS:
				case Arts::INITIATE:
				case Arts::SUPPORT_DEMOTION:
				case Arts::SUPPORT_ASCENSION:
				case Arts::CUP_SUMMONS:
				case Arts::ASCEND:
				case Arts::POWER_TOKEN:
				case Arts::EMPATHY:
					if (player->Skill(art)<1){
						player->SetSkill(art, 1, SET_ABSOLUTE, player->ID(), true);
						LoadString (hInstance, IDS_LEARNED_HOUSE_ART, disp_message, sizeof(disp_message));
							_stprintf(message, disp_message, this->Descrip(art));
						display->DisplayMessage (message);
					}
					continue;
				default:
					continue;
			}
		}


		LoadString (hInstance, IDS_KNIGHT_SUCCEEDED, disp_message, sizeof(disp_message));
	}
	else
		LoadString (hInstance, IDS_KNIGHT_FAILED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, n->Name(), GuildName(guild_id));
	display->DisplayMessage (message);

	if (success)
		LoadString (hInstance, IDS_KNIGHT_OTHER_SUCCEEDED, disp_message, sizeof(disp_message));
	else
		LoadString (hInstance, IDS_KNIGHT_OTHER_FAILED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, player->Name(), GuildName(guild_id));
	gs->Talk(message, RMsg_Speech::SYSTEM_WHISPER, caster_id);

	return;
}

void cArts::MidKnight(void)
{
	if (player->NumGuilds(Guild::RULER) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::RULER));
		this->EndKnight(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CHOOSE_GUILD,
						cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndKnight);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::KNIGHT);
	}
	return;
}

void cArts::EndKnight(void *value)
{
	cNeighbor *n = cp->SelectedNeighbor();
	int guild_id = *((int*)value);


	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::KNIGHT);
		this->ArtFinished(false);
		return;
	}
	else if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}
	else
	{
		cItem* prime = FindPrime(guild_id, Arts::GUARDIAN_DRAIN);
		if (prime == NO_ITEM) 
		{
			LoadString (hInstance, IDS_NEED_PRIME, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::KNIGHT), this->Descrip(Arts::KNIGHT), 
				GuildName(guild_id), GuildName(guild_id), Arts::GUARDIAN_DRAIN);
			display->DisplayMessage (message);
			this->ArtFinished(false);
		} 
		else 
		{
			prime->DrainMetaEssence(Arts::GUARDIAN_DRAIN);
			gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::KNIGHT,
				guild_id, 1);
			this->ArtFinished(true);
		}
	}
	return;
}

// Support Ascension

void cArts::StartSupportAscension(void)
{
	if ((!player->IsRuler(Guild::NO_GUILD)) && (!player->IsKnight(Guild::NO_GUILD)))
	{
		LoadString (hInstance, IDS_MUST_BE_KNIGHT, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(Arts::SUPPORT_ASCENSION));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::MidSupportAscension, Arts::SUPPORT_ASCENSION);
	this->CaptureCP(NEIGHBORS_TAB, Arts::SUPPORT_ASCENSION);

	return;
}

void cArts::MidSupportAscension(void)
{
	if ((player->NumGuilds(Guild::RULER)) + (player->NumGuilds(Guild::KNIGHT)) == 1)
	{ // only one choice, skip straight to end

		int value=0;
		if (player->NumGuilds(Guild::KNIGHT) == 1) {
			value = GuildID(player->GuildFlags(Guild::KNIGHT));
		}
		else {
			value = GuildID(player->GuildFlags(Guild::RULER));
		}
		this->EndSupportAscension(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, (IDD_CHOOSE_GUILD),
						cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndSupportAscension);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_KNIGHTS, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::SUPPORT_ASCENSION);
	}
	return;
}

void cArts::EndSupportAscension(void *value)
{
	cNeighbor *n = cp->SelectedNeighbor();
	int guild_id = *((int*)value);


	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SUPPORT_ASCENSION);
		this->ArtFinished(false);
		return;
	}
	else if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}

	// create the token
	LmItem info;
	LmItemHdr header;
	cItem *item;
	lyra_item_support_t support = {LyraItem::SUPPORT_FUNCTION, 0, 0, 0};

	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
	header.SetGraphic(LyraBitmap::GUILD_ASCENSION_TOKEN);
	header.SetColor1(0); header.SetColor2(0);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_FUNCTION), 0, 0));

	support.set_guild_token(guild_id, Tokens::ASCENSION_TO_RULER);
	support.set_creator_id(player->ID());
	support.set_target_id(n->ID());

LoadString(hInstance, IDS_SUPPORT_DREAMER, temp_message, sizeof(temp_message));
_stprintf(message, temp_message, n->Name());
	_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
	disp_message[LmItem::NAME_LENGTH-1] = '\0';

	info.Init(header, disp_message, 0, 0, 0);
	info.SetStateField(0, &support, sizeof(support));
	info.SetCharges(1);
	int ttl = 120;
	item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	LoadString (hInstance, IDS_SUPPORT_TOKEN_CREATED, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, n->Name(), GuildName(guild_id));
	display->DisplayMessage (message);

	this->ArtFinished(true);
	return;

}

//////////////////////////////////////////////////////////////////
// Support Demotion

void cArts::StartSupportDemotion(void)
{
	if (!player->IsKnight(Guild::NO_GUILD) && !player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_KNIGHT, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, this->Descrip(Arts::SUPPORT_DEMOTION));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::MidSupportDemotion, Arts::SUPPORT_DEMOTION);
	this->CaptureCP(NEIGHBORS_TAB, Arts::SUPPORT_DEMOTION);

	return;
}

void cArts::MidSupportDemotion(void)
{
	int knight_guilds = player->NumGuilds(Guild::KNIGHT);
	int ruler_guilds = player->NumGuilds(Guild::RULER);
	if (knight_guilds + ruler_guilds == 1)
	{ // only one choice, skip straight to end
		int value;
		if (player->NumGuilds(Guild::KNIGHT) == 1)
			value = GuildID(player->GuildFlags(Guild::KNIGHT));
		else
			value = GuildID(player->GuildFlags(Guild::RULER));
		this->EndSupportDemotion(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, (IDD_CHOOSE_GUILD),
						cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndSupportDemotion);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_KNIGHTS, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		this->WaitForDialog(hDlg, Arts::SUPPORT_DEMOTION);
	}
	return;
}

void cArts::EndSupportDemotion(void *value)
{
	cNeighbor *n = cp->SelectedNeighbor();
	int guild_id = *((int*)value);


	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SUPPORT_DEMOTION);
		this->ArtFinished(false);
		return;
	}
	else if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}

	// create the token
	LmItem info;
	LmItemHdr header;
	cItem *item;
	lyra_item_support_t support = {LyraItem::SUPPORT_FUNCTION, 0, 0, 0};

	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
	header.SetGraphic(LyraBitmap::GUILD_DEMOTION_TOKEN);
	header.SetColor1(0); header.SetColor2(0);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_FUNCTION), 0, 0));

	support.set_guild_token(guild_id, Tokens::DEMOTION);
	support.set_creator_id(player->ID());
	support.set_target_id(n->ID());

	LoadString(hInstance, IDS_DEMOTE_DREAMER, temp_message, sizeof(temp_message));
	_stprintf(message, temp_message, n->Name());
	_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
	disp_message[LmItem::NAME_LENGTH-1] = '\0';

	info.Init(header, disp_message, 0, 0, 0);
	info.SetStateField(0, &support, sizeof(support));
	info.SetCharges(1);
	int ttl = 120;
	item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	LoadString (hInstance, IDS_DEMOTION_TOKEN_CREATED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, n->Name(), GuildName(guild_id));
	display->DisplayMessage (message);

	this->ArtFinished(true);
	return;

}


//////////////////////////////////////////////////////////////////
// Create Power Token

void cArts::StartPowerToken(void)
{
	if (!player->IsInitiate(Guild::NO_GUILD) &&
		!player->IsKnight(Guild::NO_GUILD) &&
		!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_IN_HOUSE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::POWER_TOKEN));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	if (player->NumGuilds(Guild::RULER_PENDING) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::RULER_PENDING));
		this->EndPowerToken(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CHOOSE_GUILD,
			cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndPowerToken);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_INITIATES, 0, 0);
		SendMessage(hDlg, WM_ADD_KNIGHTS, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::POWER_TOKEN);
	}
	return;
}


void cArts::EndPowerToken(void *value)
{
	int guild_id = *((int*)value);

	if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}
	else
	{	// any prime will do
		cItem* prime = FindPrime(Guild::NO_GUILD, Arts::POWER_TOKEN_DRAIN);
		if (prime == NO_ITEM) 
		{
			LoadString (hInstance, IDS_NEED_PRIME_PT, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, GuildName(guild_id), GuildName(guild_id), Arts::POWER_TOKEN_DRAIN);
			display->DisplayMessage(message);
			this->ArtFinished(false);
		} 
		else 
		{	// create power token item here!	
			LmItem info;
			LmItemHdr header;
			cItem *power_token;
			lyra_item_support_t support = {LyraItem::SUPPORT_FUNCTION, 0, 0, 0};
			
			header.Init(0, 0);
			header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
			header.SetGraphic(LyraBitmap::SOUL_ESSENCE);
			header.SetColor1(0); header.SetColor2(0);
			header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_FUNCTION), 0, 0));
			
			support.set_guild_token(guild_id, Tokens::POWER_TOKEN);
			support.set_creator_id(player->ID());
			/// ARGH - we need target ID!!!
			support.set_target_id(0);
			
			LoadString(hInstance, IDS_POWER_TOKEN_HOUSE, temp_message, sizeof(temp_message));
			_stprintf(message, temp_message, GuildName(guild_id));
			_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
			disp_message[LmItem::NAME_LENGTH-1] = '\0';
			
			info.Init(header, disp_message, 0, 0, 0);
			info.SetStateField(0, &support, sizeof(support));
			info.SetCharges(1);
			int ttl = 120;
			power_token = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
			
			LoadString (hInstance, IDS_PT_CREATED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, GuildName(guild_id));
			display->DisplayMessage(message);

			prime->DrainMetaEssence(Arts::POWER_TOKEN_DRAIN);

			this->ArtFinished(true);
		}
	}
	return;
}


//////////////////////////////////////////////////////////////////
// House Members

void cArts::StartHouseMembers(void)
{
	if (!player->IsInitiate(Guild::NO_GUILD) &&
		!player->IsKnight(Guild::NO_GUILD) &&
		!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_IN_HOUSE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::HOUSE_MEMBERS));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	if (player->NumGuilds(Guild::RULER_PENDING) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::RULER_PENDING));
		this->EndHouseMembers(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CHOOSE_GUILD,
			cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndHouseMembers);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_INITIATES, 0, 0);
		SendMessage(hDlg, WM_ADD_KNIGHTS, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::HOUSE_MEMBERS);
	}
	return;
}

// we piggyback on locate dreamer by using the same message for the return value
void cArts::EndHouseMembers(void *value)
{
	int guild_id = *((int*)value);

	if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}
	art_in_use = Arts::HOUSE_MEMBERS;
	gs->SendPlayerMessage(0, RMsg_PlayerMsg::HOUSE_MEMBERS, guild_id, 0);
	this->ArtFinished(true);
}


//////////////////////////////////////////////////////////////////
// Create ID Token

void cArts::StartCreateIDToken(void)
{
	if (!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_RULER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::CREATE_ID_TOKEN));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::MidCreateIDToken, Arts::CREATE_ID_TOKEN);
	this->CaptureCP(INVENTORY_TAB, Arts::CREATE_ID_TOKEN);
	return;
}

void cArts::MidCreateIDToken(void)
{
	cItem *item = cp->SelectedItem();
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(Arts::CREATE_ID_TOKEN);
		this->ArtFinished(false);
		return;
	} 
	
	// check that the item is a dreamer head
	const void *state;
	lyra_item_essence_t head;
	bool is_head = false;
	
	if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION))
	{ // potential candidate for head; it's always in field 0
		state = item->Lmitem().StateField(0);
		memcpy(&head, state, sizeof(lyra_item_essence_t));
		// check for correct player id & guild id
		if (head.mare_type < Avatars::MIN_NIGHTMARE_TYPE) 
			is_head = true;
	}

	if (!is_head) 
	{
		LoadString (hInstance, IDS_NEED_HEAD, message, sizeof(message));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}
		
	if (player->NumGuilds(Guild::RULER) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::RULER));
		this->EndCreateIDToken(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CHOOSE_GUILD,
			cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndCreateIDToken);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::CREATE_ID_TOKEN);
	}
	return;
}

void cArts::EndCreateIDToken(void *value)
{
	int guild_id = *((int*)value);
	cItem *item = cp->SelectedItem();

	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(Arts::CREATE_ID_TOKEN);
		this->ArtFinished(false);
		return;
	} 
	else if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}
	else
	{
		cItem* prime = FindPrime(guild_id, Arts::CREATE_ID_DRAIN);
		if (prime == NO_ITEM) 
		{
			LoadString (hInstance, IDS_NEED_PRIME_CID, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, this->Descrip(Arts::CREATE_ID_TOKEN), this->Descrip(Arts::CREATE_ID_TOKEN), 
				GuildName(guild_id), GuildName(guild_id), Arts::CREATE_ID_DRAIN);
			display->DisplayMessage(message);
			this->ArtFinished(false);
		} 
		else 
		{	// create item here!

			const void *state;
			lyra_item_essence_t head;
	
			state = item->Lmitem().StateField(0);
			memcpy(&head, state, sizeof(lyra_item_essence_t));

			LmItem info;
			LmItemHdr header;
			cItem *id_token;
			lyra_item_support_t support = {LyraItem::SUPPORT_FUNCTION, 0, 0, 0};
			
			header.Init(0, 0);
			header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
			header.SetGraphic(LyraBitmap::GUILD_MEMBER_TOKEN_BASE + guild_id);
			header.SetColor1(0); header.SetColor2(0);
			header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_FUNCTION), 0, 0));
			
			support.set_guild_token(guild_id, Tokens::MEMBERSHIP);
			support.set_creator_id(player->ID());
			support.set_target_id(head.slaver_id);
			
			LoadString(hInstance, IDS_NEW_ID, temp_message, sizeof(temp_message));
			_stprintf(message, temp_message, item->Name());
			_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
			disp_message[LmItem::NAME_LENGTH-1] = '\0';
			
			info.Init(header, disp_message, 0, 0, 0);
			info.SetStateField(0, &support, sizeof(support));
			info.SetCharges(1);
			int ttl = 120;
			id_token = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
			prime->DrainMetaEssence(Arts::CREATE_ID_DRAIN);

			
			LoadString (hInstance, IDS_CREATED_ID_TOKEN, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, item->Name());
			display->DisplayMessage(message);
			item->Destroy();

			this->ArtFinished(true);
		}
	}

	return;
}


//////////////////////////////////////////////////////////////////
// Combine

void cArts::StartCombine(void)
{
	this->WaitForSelection(&cArts::MidCombine, art_in_use);
	this->CaptureCP(INVENTORY_TAB, art_in_use);
	return;
}

void cArts::MidCombine(void)
{
	cItem *item = cp->SelectedItem();
	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(art_in_use);
		this->ArtFinished(false);
		return;
	} 
	
	combining_item = item; 
	this->WaitForSelection(&cArts::EndCombine, art_in_use);
	this->CaptureCP(INVENTORY_TAB, art_in_use);
	
	return;
}

void cArts::EndCombine(void)
{
	cItem* item1 = combining_item;
	cItem *item2 = cp->SelectedItem();
	combining_item = NO_ACTOR;
	LmItem info;
	LmItemHdr header;
	cItem *combined_item;

	bool combinable = true;

	if ((item1 == NO_ACTOR) || !(actors->ValidItem(item1)) || 
		(item2 == NO_ACTOR) || !(actors->ValidItem(item2)))
	{
		this->DisplayItemBailed(art_in_use);
		this->ArtFinished(false);
		return;
	} 


	if (item1->ID() == item2->ID())
	{
		LoadString (hInstance, IDS_SAME_ITEM, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	const void *state;
	int num_charges = item1->Lmitem().Charges() + item2->Lmitem().Charges();
	if (num_charges > 100) // max charges = 100 
		num_charges = 100;

	header.Init(0, 0);
	header.SetFlags(item1->Lmitem().Header().Flags() | LyraItem::FLAG_HASDESCRIPTION);
	header.SetGraphic(item1->Lmitem().Header().Graphic());
	header.SetColor1(item1->Lmitem().Header().Color1()); 
	header.SetColor2(item1->Lmitem().Header().Color2());
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_FUNCTION), 0, 0));
	header.SetStateFormat(item1->Lmitem().Header().StateFormat());

	if ((item1->Lmitem().Header().Flags() & LyraItem::FLAG_HASDESCRIPTION) ||
		(item2->Lmitem().Header().Flags() & LyraItem::FLAG_HASDESCRIPTION))
		combinable = false;
	else if (item1->ItemFunction(0) != item2->ItemFunction(0))
		combinable = false; 
	else if ((item1->ItemFunction(0) != LyraItem::CHANGE_STAT_FUNCTION) &&
		(item1->ItemFunction(0) != LyraItem::MISSILE_FUNCTION) &&
		(item1->ItemFunction(0) != LyraItem::EFFECT_PLAYER_FUNCTION))
		combinable = false; 
	else if ((item1->NumFunctions() > 1) ||
			 (item2->NumFunctions() > 1))
		combinable = false; 
	else if ((item1->Lmitem().Charges() == INFINITE_CHARGES) ||
		(item2->Lmitem().Charges() == INFINITE_CHARGES))
		combinable = false;

	else switch (item1->ItemFunction(0))
	{
	case LyraItem::CHANGE_STAT_FUNCTION:
		{
			lyra_item_change_stat_t change_stat1, change_stat2;
			state = item1->Lmitem().StateField(0);
			memcpy(&change_stat1, state, sizeof(lyra_item_change_stat_t));
			state = item2->Lmitem().StateField(0);
			memcpy(&change_stat2, state, sizeof(lyra_item_change_stat_t));
			if ((change_stat1.modifier != change_stat2.modifier) ||
				(change_stat1.stat != change_stat2.stat))
				combinable = false;
			info.Init(header, item1->Name(), 0, 0, 0);
			info.SetStateField(0, &change_stat1, sizeof(change_stat1));
			break;
		}

	case LyraItem::MISSILE_FUNCTION:
		{
			lyra_item_missile_t missile1, missile2;
			state = item1->Lmitem().StateField(0);
			memcpy(&missile1, state, sizeof(lyra_item_missile_t));
			state = item2->Lmitem().StateField(0);
			memcpy(&missile2, state, sizeof(lyra_item_missile_t));
			if ((missile1.bitmap_id != missile2.bitmap_id) ||
				(missile1.damage != missile2.damage ) ||
				(missile1.effect != missile2.effect ) ||
				(missile1.velocity != missile2.velocity))
				combinable = false;
			info.Init(header, item1->Name(), 0, 0, 0);
			info.SetStateField(0, &missile1, sizeof(missile1));
			break;
		}

	case LyraItem::EFFECT_PLAYER_FUNCTION:
		{
			lyra_item_effect_player_t effect_player1, effect_player2;
			state = item1->Lmitem().StateField(0);
			memcpy(&effect_player1, state, sizeof(lyra_item_effect_player_t));
			state = item2->Lmitem().StateField(0);
			memcpy(&effect_player2, state, sizeof(lyra_item_effect_player_t));
			if ((effect_player1.duration != effect_player2.duration) ||
				(effect_player1.effect != effect_player2.effect))
				combinable = false;
			info.Init(header, item1->Name(), 0, 0, 0);
			info.SetStateField(0, &effect_player1, sizeof(effect_player1));
		break;
		}
	}

	if (!combinable) 
	{
		LoadString (hInstance, IDS_NO_COMBINE, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	} else 
	{
		info.SetCharges(num_charges);
		int ttl = 120; // default
		TCHAR description[ITEM_DESCRIP_LENGTH];
		LoadString (hInstance, IDS_COMBINED_DESCRIP, description, sizeof(description));

		combined_item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl, description);
	
		LoadString (hInstance, IDS_COMBINED, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);

		item1->Destroy();
		item2->Destroy();
	
		this->ArtFinished(true);
	}
	

	return;
}


//////////////////////////////////////////////////////////////////
// VampiricDraw


void cArts::StartVampiricDraw(void)
{
	this->WaitForSelection(&cArts::EndVampiricDraw, Arts::VAMPIRIC_DRAW);
	this->CaptureCP(NEIGHBORS_TAB, Arts::VAMPIRIC_DRAW);
	return;
}

void cArts::ApplyVampiricDraw(lyra_id_t caster_id, int amount, int stat)
{
	int i = 1;
	cNeighbor *n = this->LookUpNeighbor(caster_id);
	player->EvokedFX().Activate(Arts::VAMPIRIC_DRAW, false); 
	int actual_amount = amount;
	if (n != NO_ACTOR)
	{
		LoadString (hInstance, IDS_VP_DRAW_OUCH, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
		display->DisplayMessage (message);
		// Shields probably shouldn't help against Vampiric Draw...
		if (player->CurrStat(stat) < amount)
			actual_amount = player->CurrStat(stat);
		player->SetCurrStat(stat, -actual_amount, SET_RELATIVE, caster_id);
		// And the amount of stat gained by evoker shouldn't be..
		// Comment: More than the health lost by target
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::VAMPIRIC_DRAW_ACK, actual_amount, stat);
	}
}

void cArts::VampiricDrawAck(lyra_id_t caster_id, int amount, int stat)
{
	cNeighbor *n = NO_ACTOR;
	n = this->LookUpNeighbor(caster_id);
	if (amount && (n != NO_ACTOR))
	{
//		player->EvokedFX().Activate(Arts::VAMPIRIC_DRAW, false);
		player->SetCurrStat(stat, amount, SET_RELATIVE, n->ID());
		LoadString (hInstance, IDS_VP_DRAW, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
		display->DisplayMessage (message);
	}
	else
	{
		LoadString (hInstance, IDS_VP_DRAW_FAILED, message, sizeof(message));
		display->DisplayMessage (message);
	}
	return;
}


void cArts::EndVampiricDraw(void)
{
	cNeighbor *n = cp->SelectedNeighbor();

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::VAMPIRIC_DRAW);
		this->ArtFinished(false);
		return;
	}
	else if (n->flags & ACTOR_SOULSPHERE)
	{ // can't be ss to target
		LoadString (hInstance, IDS_VD_NO_SS, message, sizeof(message));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}
	else
	{
		cItem *item = NO_ITEM;
		const void *state;
		lyra_item_essence_t head;

		for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
			if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::ESSENCE_FUNCTION))
			{ // potential candidate for head; it's always in field 0
				state = item->Lmitem().StateField(0);
				memcpy(&head, state, sizeof(lyra_item_essence_t));
				// check for correct player id & guild id
				if ((head.mare_type < Avatars::MIN_NIGHTMARE_TYPE) && // dreamer head, check name
					(0 == _tcscmp(item->Name(), n->Name())))
						break;
			}
		actors->IterateItems(DONE);


		if (item == NO_ITEM) // check inventory, kill item!
		{
			LoadString (hInstance, IDS_VP_DRAW_FAILED, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, n->Name());
			display->DisplayMessage (message);
			this->CancelArt();
			return;
		}
		unsigned char amount = player->SkillSphere(Arts::VAMPIRIC_DRAW)*3+5;
		gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::VAMPIRIC_DRAW, amount, player->SelectedStat());

		if (player->SkillSphere(Arts::VAMPIRIC_DRAW) <= rand()%15) {
			item->Lmitem().SetCharges(0); // destroy essence with confirmation message
			//if (!item->Destroy()) // destroy returns true for delayed destruction
			//	item->SetTerminate();
		}
//		this->VampiricDrawAck(player->ID(), amount);
	}

	this->ArtFinished(true);
	return;

}


//////////////////////////////////////////////////////////////////
// Support Sphering

void cArts::StartSupportSphering(void)
{

	this->WaitForSelection(&cArts::EndSupportSphering, Arts::SUPPORT_SPHERING);
	this->CaptureCP(NEIGHBORS_TAB, Arts::SUPPORT_SPHERING);
	return;
}

void cArts::EndSupportSphering(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SUPPORT_SPHERING);
		this->ArtFinished(false);
		return;
	}
	else if (n->IsMonster())
	{
		LoadString (hInstance, IDS_NO_TRAIN_MONSTER, disp_message, sizeof(disp_message));
		display->DisplayMessage(disp_message);
		this->ArtFinished(false);
		return;
	}

	// create the token
	LmItem info;
	LmItemHdr header;
	cItem *item;
	lyra_item_train_support_t support = {LyraItem::SUPPORT_TRAIN_FUNCTION, 0, 0, 0, 0, 0, 0};

	header.Init(0, 0);
	header.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
	header.SetGraphic(LyraBitmap::GUILD_ASCENSION_TOKEN);
	header.SetColor1(0); header.SetColor2(0);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_TRAIN_FUNCTION), 0, 0));

	support.art_id = SPHERE_TOKEN_ART_ID; 
	support.art_level = player->Sphere(); // this is no longer used, but left in for now
	support.set_creator_id(player->ID());
	support.set_target_id(n->ID());

	LoadString(hInstance, IDS_SPHERE_DREAMER, temp_message, sizeof(temp_message));
	_stprintf(message, temp_message, n->Name());
	_tcsnccpy(disp_message, message, LmItem::NAME_LENGTH-1);
	disp_message[LmItem::NAME_LENGTH-1] = '\0';

	info.Init(header, disp_message, 0, 0, 0);
	info.SetStateField(0, &support, sizeof(support));
	info.SetCharges(1);
	int ttl = 120;
	item = CreateItem(player->x, player->y, player->angle, info, 0, false, ttl);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	LoadString (hInstance, IDS_SUPPORT_SPHERE_TOKEN, disp_message, sizeof(disp_message));
_stprintf(message, disp_message, n->Name());
	display->DisplayMessage (message);

	this->ArtFinished(true);
	return;

}




////////////////////////////////////////////////////////////////
// *** Arts that require dialog input only ***
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Ascend

void cArts::StartAscend(void)
{
	if (!player->IsKnight(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_KNIGHT, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::ASCEND));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	if (player->NumGuilds(Guild::KNIGHT) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::KNIGHT));
		this->EndAscend(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg =  CreateLyraDialog(hInstance, (IDD_CHOOSE_GUILD),
						cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndAscend);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_KNIGHTS, 0, 0);
		this->WaitForDialog(hDlg, Arts::ASCEND);
	}
	return;
}

void cArts::EndAscend(void *value)
{
	int num_tokens = 0;
	int guild_id = *((int*)value);
	cItem* tokens[Guild::DEMOTE_RULER]; // holds token pointers

	if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}

	num_tokens = CountAscensionTokens(guild_id, (cItem**)tokens);
	
	if (num_tokens >= Guild::DEMOTE_RULER)
	{
		cItem* prime = FindPrime(guild_id, Arts::RULER_DRAIN);
		if (prime == NO_ITEM) 
		{
			LoadString (hInstance, IDS_NEED_PRIME, disp_message, sizeof(disp_message));
			_stprintf(message, disp_message, this->Descrip(Arts::ASCEND), this->Descrip(Arts::ASCEND), 
				GuildName(guild_id), GuildName(guild_id), Arts::RULER_DRAIN);
			display->DisplayMessage (message);
			this->ArtFinished(false);
		} 
		else 
		{
			prime->DrainMetaEssence(Arts::RULER_DRAIN);
			gs->SendPlayerMessage(player->ID(), RMsg_PlayerMsg::ASCEND, guild_id, 1);
			this->ArtFinished(true);
		}
	}
	else
	{	// failure!
		LoadString (hInstance, IDS_ASCEND_FAILED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, GuildName(guild_id), num_tokens, Guild::DEMOTE_RULER);
		display->DisplayMessage (message);
		this->ArtFinished(false);
	}

}

int cArts::CountAscensionTokens(lyra_id_t guild_id, cItem** tokens)
{
	int i,num_tokens = 0;
	const void* state;
	bool duplicate = false;
	cItem* item;
	lyra_item_support_t support[Lyra::INVENTORY_MAX];	// holds token info
	
	// check that the proper # of ruler support tokens are carried
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::SUPPORT_FUNCTION))
		{ // potential candidate for token; support is always item function zero
			state = item->Lmitem().StateField(0);
			memcpy(&support[num_tokens], state, sizeof(lyra_item_support_t));
			// check for correct player id & guild id
			if ((support[num_tokens].target_id() != player->ID()) ||
				(support[num_tokens].guild_id() != guild_id) ||
				(support[num_tokens].creator_id() == player->ID()) ||
				(support[num_tokens].token_type() != Tokens::ASCENSION_TO_RULER))
				continue;
			// check for duplicates from a single ruler
			duplicate = false;
			for (i=0; i<num_tokens; i++)
				if (support[i].creator_id() == support[num_tokens].creator_id())
					duplicate = true;
				if (duplicate)
					continue;
				tokens[i] = item;
				num_tokens++; // valid token!!!
				if (num_tokens == Guild::DEMOTE_RULER)
					break;
		}
	actors->IterateItems(DONE);

	return num_tokens;
}

void cArts::ResponseAscend(int guild_id, int success)
{
	int i,num_tokens = 0;
	cItem* tokens[Guild::DEMOTE_RULER]; // holds token pointers

	num_tokens = CountAscensionTokens(guild_id, (cItem**)tokens);

	if (success)
	{	// success!!!
		player->SetGuildRank(guild_id, Guild::RULER);

		// AUTO-UNTRAIN GUARDIAN ONLY ARTS - 6/14/14 AMR
		if (!player->IsKnight(Guild::NO_GUILD))
		{ // If no longer a Guardian in any house, remove guardian arts
			player->SetSkill(Arts::CUP_SUMMONS, 0, SET_ABSOLUTE, player->ID(), true);
			player->SetSkill(Arts::ASCEND, 0, SET_ABSOLUTE, player->ID(), true);
		}
		
		// AUTO-TRAIN RULER ARTS - 6/14/14 AMR

		for (int art=0; art<NUM_ARTS; ++art) {
			switch (art) {
				case Arts::HOUSE_MEMBERS:
				case Arts::INITIATE:
				case Arts::SUPPORT_DEMOTION:
				case Arts::SUPPORT_ASCENSION:
				case Arts::DEMOTE:
				case Arts::POWER_TOKEN:
				case Arts::EXPEL:
				case Arts::KNIGHT:
				case Arts::CREATE_ID_TOKEN:
					if (player->Skill(art)<1) {
						player->SetSkill(art, 1, SET_ABSOLUTE, player->ID(), true);
						LoadString (hInstance, IDS_LEARNED_HOUSE_ART, disp_message, sizeof(disp_message));
							_stprintf(message, disp_message, this->Descrip(art));
						display->DisplayMessage (message);
					}
					continue;
				default:
					continue;
			}
		}


		LoadString (hInstance, IDS_ASCEND_SUCCEEDED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, GuildName(guild_id));
		display->DisplayMessage (message);
		for (i=0; i<num_tokens; i++)
			tokens[i]->Destroy();
		gs->UpdateServer();
	}
	
	else
	{	// failure!
		LoadString (hInstance, IDS_ASCEND_FAILED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, GuildName(guild_id), num_tokens, Guild::DEMOTE_RULER);
		display->DisplayMessage (message);
	}
	
}

////////////////////////////////////////////////////////////////
// Demote

void cArts::StartDemote(void)
{
	if (!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_RULER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::DEMOTE));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	this->WaitForSelection(&cArts::MidDemote, Arts::DEMOTE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::DEMOTE);

	return;
}

void cArts::MidDemote(void)
{

	if (player->NumGuilds(Guild::RULER) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::RULER));
		this->EndDemote(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg =  CreateLyraDialog(hInstance, (IDD_CHOOSE_GUILD),
						cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndDemote);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::DEMOTE);
	}
	return;
}

void cArts::EndDemote(void *value)
{
	int num_tokens = 0;
	cItem* tokens[MAX_DEMOTE_TOKENS_NEEDED]; // holds token pointers
	int guild_id = *((int*)value);
	bool duplicate = false;
	cNeighbor *n = cp->SelectedNeighbor();

	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::DEMOTE);
		this->ArtFinished(false);
		return;
	}
	else if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}

	num_tokens = this->CountDemotionTokens(guild_id, (cItem**)tokens, n);
	
	gs->SendPlayerMessage(n->ID(), RMsg_PlayerMsg::DEMOTE, guild_id, num_tokens);
	this->ArtFinished(true);
	return;
}

int cArts::CountDemotionTokens(lyra_id_t guild_id, cItem** tokens, cNeighbor* n)
{
	int i,num_tokens = 0;
	const void* state;
	bool duplicate = false;
	cItem* item;
	lyra_item_support_t support[Lyra::INVENTORY_MAX];	// holds token info

	// check that the proper # of demote tokens are carried
	for (item = actors->IterateItems(INIT); item != NO_ACTOR; item = actors->IterateItems(NEXT))
		if ((item->Status() == ITEM_OWNED) && (item->ItemFunction(0) == LyraItem::SUPPORT_FUNCTION))
		{ // potential candidate for token; support is always item function zero
			state = item->Lmitem().StateField(0);
			memcpy(&support[num_tokens], state, sizeof(lyra_item_support_t));
			// check for correct player id & guild id, & token not made by caster
			if ((support[num_tokens].target_id() != n->ID()) ||
				(support[num_tokens].guild_id() != guild_id) ||
				((support[num_tokens].creator_id() == player->ID()) &&
				(support[num_tokens].token_type() == Tokens::DEMOTION)) ||
				((support[num_tokens].token_type() != Tokens::DEMOTION) &&
				(support[num_tokens].token_type() != Tokens::MEMBERSHIP)))
				continue;
			// a membership token alone is enough for demotion
			if (support[num_tokens].token_type() == Tokens::MEMBERSHIP)
			{
				tokens[0] = item;
				num_tokens = MAX_DEMOTE_TOKENS_NEEDED + 1000;
				break;
			}
			// check for duplicates from a single ruler
			duplicate = false;
			for (i=0; i<num_tokens; i++)
				if (support[i].creator_id() == support[num_tokens].creator_id())
					duplicate = true;
			if (duplicate)
				continue;
			tokens[num_tokens] = item;
			num_tokens++; // valid token!!!
		}
	actors->IterateItems(DONE);
	return num_tokens;
}


void cArts::ResponseDemote(bool success, realmid_t target_id, int guild_id, int num_tokens_used)
{
	TCHAR targetname[Lyra::PLAYERNAME_MAX] = { NULL };
	cItem* tokens[MAX_DEMOTE_TOKENS_NEEDED]; // holds token pointers
	int i, num_tokens = 0;
	cNeighbor* n;


	n = LookUpNeighbor(target_id);
	if (n == NO_ACTOR)
	{
		LoadString(hInstance, IDS_THE_TARGET, message, sizeof(message));
		_tcscpy(targetname, message);
	}
	else
		_tcsnccpy(targetname, n->Name(), _tcslen(n->Name()));

	num_tokens = this->CountDemotionTokens(guild_id, (cItem**)tokens, n);

	if (success)
	{	// success!!!
		// remove the appropriate tokens
		LoadString (hInstance, IDS_DEMOTE_SUCCEEDED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, targetname, GuildName(guild_id));
		display->DisplayMessage (message);
	
		if (num_tokens > 1000) // special code to signify a membership token found
			num_tokens = 1;
		for (i=0; i<num_tokens; i++)
			tokens[i]->Destroy();
		gs->UpdateServer();
	}
	else
	{	// failure!
		LoadString (hInstance, IDS_DEMOTE_FAILED, disp_message, sizeof(disp_message));
	_stprintf(message, disp_message, targetname, GuildName(guild_id), num_tokens_used);
		display->DisplayMessage (message);
	}
	this->ArtFinished(true);
	return;
}

void cArts::ApplyDemote(int guild_id)
{
	int guild_rank = player->GuildRank(guild_id);

	if (guild_rank >= Guild::INITIATE)
	{ // decrement rank on client
		player->SetGuildRank(guild_id, guild_rank - 1);

		LoadString (hInstance, IDS_DEMOTED, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, GuildName(guild_id));
		display->DisplayMessage (message);
	}
}

//////////////////////////////////////////////////////////////////
// SummonPrime

const int SUMMON_PRIME_POWER_TOKENS = 10;

void cArts::StartSummonPrime(void)
{
	if (!player->IsRuler(Guild::NO_GUILD))
	{
		LoadString (hInstance, IDS_MUST_BE_RULER, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, this->Descrip(Arts::SUMMON_PRIME));
		display->DisplayMessage (message);
		this->ArtFinished(false);
		return;
	}

	if (player->NumGuilds(Guild::RULER) == 1)
	{ // only one choice, skip straight to end
		int value = GuildID(player->GuildFlags(Guild::RULER));
		this->EndSummonPrime(&value);
		return;
	}
	else
	{
		if (chooseguilddlg)
		{
			this->ArtFinished(false);
			return;
		}
		chooseguilddlg = true;
		HWND hDlg = CreateLyraDialog(hInstance, IDD_CHOOSE_GUILD,
						cDD->Hwnd_Main(), (DLGPROC)ChooseGuildDlgProc);
		chooseguild_callback = (&cArts::EndSummonPrime);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_ADD_RULERS, 0, 0);
		this->WaitForDialog(hDlg, Arts::SUMMON_PRIME);
	}
	return;
}


void cArts::ApplySummonPrime(int guild_id, int success)
{
	player->EvokedFX().Activate(Arts::SUMMON_PRIME, false);

	if (success)
	{
		LoadString (hInstance, IDS_SUMMON_PRIME_SUCCEEDED, disp_message, sizeof(disp_message));
	}
	else
		LoadString (hInstance, IDS_SUMMON_PRIME_FAILED, disp_message, sizeof(disp_message));
	
	_stprintf(message, disp_message, GuildName(guild_id));
	display->DisplayMessage (message);

	return;
}

void cArts::EndSummonPrime(void *value)
{
	int guild_id = *((int*)value);

	if (guild_id == Guild::NO_GUILD)
	{
		this->CancelArt();
		return;
	}

	cItem* power_tokens[Lyra::INVENTORY_MAX];
	int num_tokens = CountPowerTokens((cItem**)power_tokens, Guild::NO_GUILD);

	if (num_tokens < SUMMON_PRIME_POWER_TOKENS)
	{
		LoadString (hInstance, IDS_MUST_HAVE_POWER_TOKENS, message, sizeof(message));
		_stprintf(disp_message, message, SUMMON_PRIME_POWER_TOKENS, GuildName(Guild::NO_GUILD), arts->Descrip(Arts::SUMMON_PRIME));
		display->DisplayMessage(disp_message); 
		this->ArtFinished(false);
		return;
	}

	gs->SendPlayerMessage(player->ID(), RMsg_PlayerMsg::SUMMON_PRIME, guild_id, 0);

	for (int i=0; i<SUMMON_PRIME_POWER_TOKENS; i++)
		power_tokens[i]->Destroy();

	this->ArtFinished(true);
}


////////////////////////////////////////////////////////////////
// Locate Avatar

void cArts::StartLocate(void)
{
	if (locateavatardlg)
	{
		this->ArtFinished(false);
		return;
	}

	fDoingLocate = true;

	LoadString (hInstance, IDS_AVATAR_NAME, message, sizeof(message));
	HWND hDlg =  CreateLyraDialog(hInstance, IDD_LOCATE_AVATAR,
					cDD->Hwnd_Main(), (DLGPROC)LocateAvatarDlgProc);
	this->WaitForDialog(hDlg, Arts::LOCATE_AVATAR);

	return;
}

// NULL value means to locate everybody on the buddy list
void cArts::EndLocate(void *value)
{
	TCHAR *name = (TCHAR*)value;
	bool locate_all = false;
	int i, num_buddies;
	unsigned long result,size;
	DWORD reg_type;
	HKEY reg_key;
	other_t buddies[GMsg_LocateAvatar::MAX_PLAYERS];
	GMsg_LocateAvatar locate_msg;

	if (name == NULL)
		locate_all = true;

	if (locate_all)
	{
		RegCreateKeyEx(HKEY_CURRENT_USER, RegPlayerKey(),0,
						NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);

		size = sizeof(num_buddies);
		LoadString(hInstance, IDS_NUM_BUDDIES, message, sizeof(message));
		result = RegQueryValueEx(reg_key, message, NULL, &reg_type,
			(unsigned char *)(&num_buddies), &size);
		if ((result != ERROR_SUCCESS) || !num_buddies)
		{
			RegCloseKey(reg_key);
			this->ArtFinished(false);
			return;
		}

		size = GMsg_LocateAvatar::MAX_PLAYERS*sizeof(other_t);
		LoadString(hInstance, IDS_WATCH_LIST, message, sizeof(message));
		result = RegQueryValueEx(reg_key, message, NULL, &reg_type,
			(unsigned char *)buddies, &size);
		RegCloseKey(reg_key);

		if (result != ERROR_SUCCESS)
		{
			this->ArtFinished(false);
			return;
		}

		locate_msg.Init(num_buddies); // set num players
		int count = 0;
		for (i=0; i<num_buddies; i++)
		{
			if (WhichMonsterName(buddies[i].name))
			{
				LoadString (hInstance, IDS_LOCATE_MARE, disp_message, sizeof(disp_message));
				display->DisplayMessage(disp_message, false);
			}
			else
			{
				locate_msg.SetPlayerName(count, buddies[i].name);
				count++;
			}
		}
		locate_msg.SetNumPlayers(count);
		if (count)
		{
			gs->LocateAvatar(locate_msg);
			this->ArtFinished(true);
			return;
		}
		else
		{
			this->ArtFinished(false);
			return;
		}
	}
	else
	{
		if (WhichMonsterName(name))
		{
			LoadString (hInstance, IDS_LOCATE_MARE, disp_message, sizeof(disp_message));
			display->DisplayMessage(disp_message, false);
			this->ArtFinished(false);
			return;
		}
		else
		{
			locate_msg.Init(1); // set num players
			locate_msg.SetPlayerName(0, name);
			gs->LocateAvatar(locate_msg);
			this->ArtFinished(true);
		}
	}
}


////////////////////////////////////////////////////////////////
// Write Scroll

void cArts::StartWriteScroll(void)
{
	if (writescrolldlg)
	{
		this->ArtFinished(false);
		return;
	}

	HWND hDlg =  CreateLyraDialog(hInstance, IDD_WRITE_SCROLL,
					cDD->Hwnd_Main(), (DLGPROC)WriteScrollDlgProc);
	this->WaitForDialog(hDlg, Arts::WRITE_SCROLL);

	return;
}

void cArts::EndWriteScroll(void *value)
{
	scroll_t *scroll_type = (scroll_t*)value;

	LmItem info;
	LmItemHdr header;
	int flags = 0;
	lyra_item_scroll_t scroll = {LyraItem::SCROLL_FUNCTION, 0, 0, 0, 0, 0};

	header.Init(0, 0);
	header.SetGraphic(LyraBitmap::CODEX);
	header.SetColor1(scroll_type->color1); header.SetColor2(scroll_type->color2);
	header.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SCROLL_FUNCTION),0,0));

	if (scroll_type->num_charges == 254)
		flags = LyraItem::FLAG_IMMUTABLE | LyraItem::FLAG_HASDESCRIPTION;
	else
		flags = LyraItem::FLAG_CHANGE_CHARGES | LyraItem::FLAG_HASDESCRIPTION;

	if (scroll_type->artifact)
		flags = flags | LyraItem::FLAG_NOREAP | LyraItem::FLAG_ALWAYS_DROP;

	header.SetFlags(flags);

	scroll.set_creatorid(player->ID());

	info.Init(header, scroll_type->name, 0, 0, 0);
	info.SetStateField(0, &scroll, sizeof(scroll));
	info.SetCharges(scroll_type->num_charges);

	// make scrolls last 4 hours
	cItem* item = CreateItem(player->x, player->y, player->angle, info, 0,
		false, GMsg_PutItem::DEFAULT_TTL*24, scroll_type->descrip);
	if (item == NO_ITEM)
	{
		this->ArtFinished(false);
		return;
	}

	this->ArtFinished(true);

	return;
}

// Psuedo-Arts 

// using a Gratitude works like a psuedo-art; we grab the control panel 
// and wait for a selection, then confirm the selection

void cArts::StartShowGratitude(void)
{
	this->WaitForSelection(&cArts::MidShowGratitude, Arts::SHOW_GRATITUDE);
	this->CaptureCP(NEIGHBORS_TAB, Arts::SHOW_GRATITUDE);
}

void cArts::MidShowGratitude(void)
{
	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SHOW_GRATITUDE);
		this->ArtFinished(false, false);
		return;
	}
	if (!acceptrejectdlg)
	{	
		LoadString (hInstance, IDS_QUERY_GRATITUDE, disp_message, sizeof(disp_message));
		_stprintf(message, disp_message, n->Name());
		HWND hDlg = CreateLyraDialog(hInstance, (IDD_ACCEPTREJECT),
						cDD->Hwnd_Main(), (DLGPROC)AcceptRejectDlgProc);
		acceptreject_callback = (&cArts::EndShowGratitude);
		SendMessage(hDlg, WM_SET_ART_CALLBACK, 0, 0);
		SendMessage(hDlg, WM_SET_AR_NEIGHBOR, 0, (LPARAM)n);
	}
	else 
	{
		LoadString (hInstance, IDS_NO_GRATITUDE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false, false);
	}
}

// force a confirmation from the user before assigning the gratitude token
void cArts::EndShowGratitude(void* value)
{
	int success = *((int*)value);
	if (!success)
	{
		LoadString (hInstance, IDS_NO_GRATITUDE, disp_message, sizeof(disp_message));
		display->DisplayMessage (disp_message);
		this->ArtFinished(false, false);
		return;
	}

	cNeighbor *n = cp->SelectedNeighbor();
	if ((n == NO_ACTOR) || !(actors->ValidNeighbor(n)))
	{
		this->DisplayNeighborBailed(Arts::SHOW_GRATITUDE);
		this->ArtFinished(false, false);
		return;
	}

	cItem *item = cp->SelectedItem();

	if ((item == NO_ACTOR) || !(actors->ValidItem(item)))
	{
		this->DisplayItemBailed(Arts::SHOW_GRATITUDE);
		this->ArtFinished(false);
		return;
	} 

	item->ApplyGratitude(n);
	this->ArtFinished(false, false);
}

// Destructor
cArts::~cArts(void)
{
}


// Check invariants

#ifdef CHECK_INVARIANTS

void cArts::CheckInvariants(int line, TCHAR *file)
{
	return;
}

#endif
