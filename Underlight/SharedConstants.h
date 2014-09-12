#ifndef SHAREDCONSTANTS_H
#define SHAREDCONSTANTS_H

// *** REALLY IMPORTANT NOTE -- READ THIS OR DIE ***
// IF YOU CHANGE *ANYTHING* IN THIS FILE, NOTIFY BRENT,
// OR REAL BAD THINGS CAN HAPPEN!  THIS IS NOT AN IDLE WARNING.
// HIS EMAIL ADDRESS IS brent@lyrastudios.  MEMORIZE IT.
// *** NOW BACK TO YOUR REGULARLY SCHEDULED CODE ***

// effects constants - indexes into header array for effects
// NOTE: keep consistent with Visual Effects.ASC!

 
const int NUM_TIMED_EFFECTS = 29; // add one for NONE
const int NUM_ARTS = 144;
const int NUM_MODIFIERS = 64;
const int NUM_DURATIONS = 64;
const int NUM_PLAYER_STATS = 5;
const int NUM_GUILDS = 8;
const int NUM_HOUSES = 8;
const int NUM_RANKS = 3;
const int NUM_TOKENS = 4;
const int MIN_VELOCITY = -7;
const int MAX_VELOCITY = 7;
const int NUM_MISSILE_BITMAPS = 3;
const int MELEE_VELOCITY = 0;
const int ART_MISSILE_VELOCITY = 4;
const int NUM_ACTOR_COLORS = 16;
const int NUM_MONSTER_COLORS = 16;
const int ANY_COLOR = NUM_ACTOR_COLORS;

const int PPOINTS_PER_PMARE_CREDIT = 20;

// these are indexes into the modifier table
const int EMPHANT_DAMAGE = 15;
const int BOGROM_DAMAGE = 16;
const int AGOKNIGHT_DAMAGE = 60;
const int SHAMBLIX_DAMAGE = 60;
const int HORRON_DAMAGE = 38;


struct Stats {
	enum {
		STAT_MAX = 99,
		STAT_MIN = 0,
		SKILL_MAX = 99,
		SKILL_MIN = 0,
		ORBIT_MIN = 0,
		ORBIT_MAX = 99,
		SPHERE_MIN = 0,
		SPHERE_MAX = 9
	};
	enum  
	{
		DREAMSOUL	= 0,
		WILLPOWER,
		INSIGHT,
		RESILIENCE,
		LUCIDITY,
		NO_STAT		= 9
	};
};

struct Avatars
{
	enum 
	{
		UNDEFINED	= -1,
		// players
		MALE		= 0, 
		FEMALE = 1, 
		// nightmares
		EMPHANT		= 2,	 
		MIN_NIGHTMARE_TYPE = EMPHANT,
		BOGROM		= 3, 
		AGOKNIGHT	= 4, 
		SHAMBLIX	= 5,
		HORRON		= 6,
		
		MAX_AVATAR_TYPE = HORRON,
		MIN_AVATAR_TYPE = MALE,			//RR For consistency
		VIEWS = 6,

		// PMARE_RESUME is a special avatar constant used to indicate
		// that a pmare session should be resumed with the avatar it last 
		// left off with
		PMARE_RESUME = MAX_AVATAR_TYPE + 1
	};
};

#if 0
struct Avatars
{
  enum {
	    MALE = 0, // players
		FEMALE = 1, 
		EMPHANT, // nightmares
		MIN_NIGHTMARE_TYPE = EMPHANT,
		BOGROM, 
		AGOKNIGHT, 
		SHAMBLIX,
		HORRON,
		MAX_AVATAR_TYPE = HORRON,
		MIN_AVATAR_TYPE = MALE,			//RR For consistency
		VIEWS = 6,
		// PMARE_RESUME is a special avatar constant used to indicate
		// that a pmare session should be resumed with the avatar it last 
		// left off with
		PMARE_RESUME = MAX_AVATAR_TYPE + 1
  };
};
#endif


struct Arts {
	enum  {
	NONE = -1,
	JOIN_PARTY = 0,
	GATEKEEPER,
	DREAMSEER,
	SOULMASTER,
	FATESENDER,
	RANDOM,
	MEDITATION,
	RESIST_FEAR,
	PROTECTION,
	FREE_ACTION,
	WARD,  // 10
	AMULET,
	SHATTER,
	RETURN,
	KNOW,
	JUDGEMENT,
	IDENTIFY,
	IDENTIFY_CURSE,
	CHAMELE,
	VISION,
	BLAST,  // 20
	BLEND,
	FORGE_TALISMAN,
	RECHARGE_TALISMAN,
	RESTORE,
	REWEAVE,
	PURIFY,
	DRAIN_SELF,
	ABJURE,
	POISON,
	ANTIDOTE,  //30
	CURSE,
	DRAIN_NIGHTMARE,
	BANISH_NIGHTMARE,
	ENSLAVE_NIGHTMARE,
	TRAP_NIGHTMARE,
	DREAMBLADE,
	TRAIL,
	SCARE,
	STAGGER,
	DEAFEN, // 40
	BLIND,
	DARKNESS,
	PARALYZE,
	FIRESTORM,
	RAZORWIND,
	RECALL,
	PUSH,
	SOULEVOKE,
	DREAMSTRIKE,
	NIGHTMARE_FORM, // 50
	LOCATE_AVATAR,
	TRAIN,
	INITIATE,
	KNIGHT,
	SUPPORT_ASCENSION,
	ASCEND,
	FINGER_OF_DEATH, // gm only
	GRANT_XP, // gm only
	TERMINATE, // gm only
	LEVELTRAIN, // 60
	SUPPORT_DEMOTION,
	DEMOTE,
	INVISIBILITY,
	GIVE,
	GATESMASHER,
	FATESLAYER,
	SOULREAPER,
	FLAMESHAFT,
	TRANCEFLAME, 
	FLAMESEAR,  //70
	FLAMERUIN,
	WRITE_SCROLL,
	DESTROY_ITEM,
	MIND_BLANK,
	SHOW,
	BOOT, // gm only
	UNTRAIN, // gm only
	GRANT_RP_XP,
	EARTHQUAKE, 
	HYPNOTIC_WEAVE, // 80
	VAMPIRIC_DRAW,
	TERROR,
	HEALING_AURA,
	ROGER_WILCO, 
	DREAMSMITH_MARK,
	SUPPORT_TRAINING,
	SUPPORT_SPHERING,
	TRAIN_SELF,
	SOUL_SHIELD,
	SUMMON, // 90
	SUSPEND,
	REFLECT,
	SACRIFICE,
	CLEANSE_NIGHTMARE,
	CREATE_ID_TOKEN,
	SENSE_DREAMERS,
	EXPEL,
	LOCATE_NEWLIES,
	COMBINE,
	POWER_TOKEN, // 100
	SHOW_GRATITUDE, 
	QUEST,
	EMPATHY,
	RADIANT_BLAZE,
	POISON_CLOUD,
	BREAK_COVENANT,
	PEACE_AURA,
	SABLE_SHIELD,
	ENTRANCEMENT,
	SHADOW_STEP,  // 110
	DAZZLE,
	GUILDHOUSE,
	CORRUPT_ESSENCE,
	TEHTHUS_OBLIVION,
	CHAOS_PURGE,
	WORDSMITH_MARK,
	CUP_SUMMONS,
	HOUSE_MEMBERS,
	FREESOUL_BLADE,
	ILLUMINATED_BLADE, // 120
	SUMMON_PRIME, 
	GRANT_PPOINT, // dummy art; it should not be learned
	SCAN,  // judgement 
	PASSLOCK, // blend clone
	HEAL, // restore clone
	SANCTIFY, // protection
	LOCK, // ward
	KEY, // amulet
	BREAK_LOCK, // shatter
	REPAIR, // reweave // 130
	REMOVE_CURSE, // purify
	HOLD_AVATAR, // paralyze
	SANCTUARY, // recall
	SHOVE, // push
	SCRIBE_NOT, // placeholder - not implemented
	FORGE_MASTER, // dreamsmith mark 
	MERGE_TALISMAN, // combine
	NP_SYMBOL,	 // use NP symbol on chest
	SENSE_MARE,  // sense pmares & dark mares
  TEMPEST,    // Tempest // 140
  KINESIS,    // Kinesis
  MISDIRECTION, // Misdirection
  CHAOTIC_VORTEX, // Chaotic Vortex

	// END OF ARTS LIST - below are art-related constants

	INITIATE_DRAIN = 100,   // amount of essences drained from prime to Initiate
	GUARDIAN_DRAIN = 500,   // amount of essences drained from prime to Knight
	RULER_DRAIN =   1000,   // amount of essences drained from prime to Ascend Ruler
	DEMOTE_DRAIN = 500,		// amount of essences drained from prime to demote
	EXPEL_DRAIN =  100,		// amount of essences drained from prime to expel a soulsphere
	POWER_TOKEN_DRAIN = 50, // amount of essences drained from prime to make a power token
	CREATE_ID_DRAIN = 5000,// amount of essences drained from prime to create an id token

	};
};


//Arts flags
const int SANCT = 0x00000001; 		// Use in Sanctuary
const int NEIGH = 0x00000002; 		// Needs others
const int NEED_ITEM = 0x00000004;	// Needs item
const int MAKE_ITEM = 0x00000008;	// Makes item
const int FOCUS = 0x00000010; 		// Focus only
const int LEARN = 0x00000020; 		// Display Learnable

const int SPHERE_TOKEN_ART_ID = 255; // art # used to distinguish Sphere support tokens from Train Self tokens

// for each art, the descrip, method, stat, min orbit, drain, and duration
struct art_t
{
//	TCHAR			descrip[ART_DESCRIP_LEN];
	unsigned int	descrip; // pointer to string table
//	art_method_t	method;
	short 		stat;
	short 		min_orbit;
	short 		drain;
	short 		duration;
	short 		casting_time;
	short		pp_multiplier;
	unsigned 	flags;	// see arts flags below


public:
	bool usable_in_sanctuary() { return  (flags & SANCT) == SANCT;};
	bool requires_neighbor()	{ return  (flags & NEIGH) == NEIGH;};
	bool requires_item() 		{ return  (flags & NEED_ITEM) == NEED_ITEM;};
	bool creates_item()			{ return  (flags & MAKE_ITEM) == MAKE_ITEM;};
	bool restricted() 			{ return  (flags & FOCUS) == FOCUS;};
	bool display_learnable()	{ return  (flags & LEARN) == LEARN;};
	unsigned long my_checksum(int id, unsigned long my_name)	{ return  0xFFFF & (	(unsigned) ( (stat << 8) ^ min_orbit ) + 
																			(unsigned) ( ( id * 0x2468) & 0xFFFF ) +
																			(unsigned) (( drain + duration) ^ 0x0124) +
																		
																			(unsigned) (casting_time << 4)+ 
																			(unsigned) (flags << 2) + 
																			(unsigned) id) ^ (unsigned) my_name;};//RRR

};


struct LyraBitmap {
	enum visual_effect_IDs {
		NONE = 0,
		DREAMBLADE = 1, 

		WARD = 4,				 // lock on teleportal
		AMULET = 5,				 // pass teleportal
		EMPHANT_ESSENCE = 6,	 // enslaved nightmare essence
		M_AVATAR_ESSENCE = 7,	 // enslaved male avatar essence
		F_AVATAR_ESSENCE = 8,	 // enslaved male avatar essence

		GUILD_ASCENSION_TOKEN = 9, // support bitmap = base + guild id
		GUILD_DEMOTION_TOKEN = 10, // support bitmap = base + guild id

		CLEANSED_MARE = 16,
		BANISHED_MARE = 17,
	    ENSLAVED_MARE = 18,
		META_ESSENCE = 19,
		SOUL_ESSENCE = 20,

		GUILD_MEMBER_TOKEN_BASE = 21,

		TALISMAN0 = 30, 
		TALISMAN1 = 31,
		TALISMAN2 = 32, 
		TALISMAN3 = 33, 
		TALISMAN4 = 34, 
		TALISMAN5 = 35, 
		TALISMAN6 = 36, 
		TALISMAN7 = 37, 
		TALISMAN8 = 38,  

		CODEX = 39,

		// missiles
		DREAMBLADE_MISSILE = 40,
		MINIMUM_MISSILE_BITMAP = DREAMBLADE_MISSILE,
		MARE_MELEE_MISSILE = 41,
		PUSH_MISSILE = MARE_MELEE_MISSILE, // dummy bitmap
		FIREBALL_MISSILE = 42,
		MAXIMUM_MISSILE_BITMAP = FIREBALL_MISSILE,
		
		BOGROM_ESSENCE = 60,	 // nightmare essence
		AGOKNIGHT_ESSENCE = 61,	 // nightmare essence
		SHAMBLIX_ESSENCE = 62,	 // nightmare essence

			// avatars
		MALE_AVATAR = 256,
		FEMALE_AVATAR = 400,
		EMPHANT_AVATAR = 544,
		BOGROM_AVATAR = 634, 
		AGOKNIGHT_AVATAR = 1600, 
		SHAMBLIX_AVATAR = 1500,
		HORRON_AVATAR = SHAMBLIX_AVATAR,
		//HORRON_AVATAR = 1680,

		// special effects
		SOULSPHERE_EFFECT = 728,
		FORMREFORM_EFFECT = 738,
		ENTRYEXIT_EFFECT = 758,
		BANNER_EFFECT = 778,
		
		HARMLESS_EVOKED = 1100,
		HARMFUL_EVOKED = 1110,
		HARMLESS_EVOKING = 1120,
		HARMFUL_EVOKING = 1130,

		FIREBALL_EXPLODE = 1080,

		// ornaments
		TRAIL_MARKER = 791,
		ITEM_GENERATOR = 792,
		AGENT_GENERATOR = 793,

		// special effects
		TEST_EFFECT = 1020,

		//avatar patches
		FEMALE_HEAD1       = 1140,
		AVATAR_GUILD_START = 1150,
		AVATAR_HALO        = 1174,
		AVATAR_SPHERE_START= 1190,
		AVATAR_NP_START    = 1220,

		BLADE			   = 1300,

		// interface components

		OK			   = 1800,
		CANCEL		   = 1801,
		YES			   = 1802,
		NO			   = 1803,
		EXIT		   = 1804,

		CHECK_BUTTON   = 1805,
		HELP_GOAL_BUTTON = 1817,

		DLG_BACKGROUND = 1820,
		DLG_BACKGROUND2 = 1821,
		LISTBOX_ARROWS  = 1823,

		FUNCTION_LABEL	= 1838,
		CHARGES_LABEL  = 1839,
		COLORS_LABEL   = 1840,
		GRAPHIC_LABEL = 1841,

		ART_ICONS	   = 1894, 

		HOUSE_BITMAPS  = 1901, // add house id to get bitmap id

		POST_GOAL_BUTTON	= 1920,
		SHOW_DETAILS_BUTTON	= 1921,
		ACCEPT_GOAL_BUTTON	= 1922,
		SHOW_REPORTS_BUTTON	= 1923,
		REFRESH_LIST_BUTTON	= 1924,
		NEXT_PAGE_BUTTON	= 1925,
		PREV_PAGE_BUTTON	= 1926,
		GOAL_BOOK_BUTTON	= 1927,
		READ_GOAL_BUTTON	= 1928,
		ABSTAIN_BUTTON		= 1929,
		VOTE_NO_BUTTON		= 1930,
		VOTE_YES_BUTTON		= 1931,
		COMPLETE_BUTTON		= 1932,
		REMOVE_GOAL_BUTTON	= 1933,
		REPORT_BUTTON		= 1934,
		SEND_GOAL_BUTTON	= 1935,
		REPLY_REPORT_BUTTON = 1936,
		POST_GOAL_OFF		= 1937,
		SHOW_DETAILS_OFF	= 1938,

		ACCEPTEES_LABEL		= 1940,
		EXPIRE_LABEL		= 1941,
		VOTE_EXPIRE_LABEL	= 1942,
		MAX_ACCEPTEES_LABEL = 1943,
		MESSAGE_LABEL		= 1944,
		NO_VOTES_LABEL		= 1945,
		NUM_ACCEPTEES_LABEL	= 1946,
		REPORT_FROM_LABEL	= 1947,
		REPORT_TO_LABEL		= 1948,
		REPORTS_LABEL		= 1949,
		RET_INFO_LABEL		= 1950,
		SUG_SPHERE_LABEL	= 1951,
		SUG_STAT_LABEL		= 1952,
		SUMMARIES_LABEL		= 1953,
		SUMMARY_LABEL		= 1954,
		XP_AWARD_LABEL		= 1955,
		YES_VOTES_LABEL		= 1956,
		POSTED_BY_LABEL		= 1957,
		STATUS_LABEL		= 1958,

		HOUSE_GOALBOARD_TITLES = 1960,

		EDIT_BUTTON	   = 1976,
		GUARDIAN_LABEL = 1977,

		QUEST_BUILDER = 1978,
		QUEST_XP_AWARD_LABEL = 1979,
		RETRIEVE_LABEL = 1980,
		COMPLETE_LABEL = 1981,
		KEYWORDS_LABEL = 1982,
		VELOCITY_LABEL = 1983,
		EFFECT_LABEL =  1984,
		DAMAGE_LABEL =  1985,

		CP_DDOWNA     = 1992,
		CP_DDOWNB     = 1993,
		CP_DUPA       = 1994,
		CP_DUPB       = 1995,
		CP_DOWNA      = 1996,
		CP_DOWNB      = 1997,
		CP_UPA        = 1998,
		CP_UPB        = 1999,

		CP_TAB1    	  = 1703,
		CP_TAB2    	  = 1706,
		CP_TAB3    	  = 1709,
		CP_TAB4       = 1712,
		CP_WINDOW     = 1700,
		// button bitmaps
		CP_DROPA     = 1715,
		CP_DROPB     = 1718,
		CP_USEA     = 1727,
		CP_USEB     = 1730,
		CP_METAA     = 1733,
		CP_METAB     = 1736,
		CP_LEFTA     = 1739,
		CP_LEFTB     = 1742,
		CP_RIGHTA     = 1745,
		CP_RIGHTB     = 1748,

		CP_BULLET	  = 1751,

		CP_GIVEA	 = 1721,
		CP_GIVEB	 = 1724,

		CP_USEPPA     = 1760,
		CP_USEPPB     = 1763,

		CP_GRANTPPA     = 1766,
		CP_GRANTPPB     = 1769,

		BULLET		 = 2030,
		
		CHANGING_PLANES	= 2044,
		SPLASH		 = 2045,
		INTRO		 = 2046,
		CURSOR   = 2047
	};
};

// NOTE: keep consistent with Sound Effects.ASC
// NOTE: table controlling whether or not sounds are propagated to other 
// players is kept in cDSound.cpp
struct LyraSound {
	enum sound_effect_id
	{
		NONE = 0,
		EXPLODE =  1,
		LAUNCH  =  2,
		ENTRY   =  3,
		MALE_SCREAM1 =  4,
		MALE_SCREAM2 =  5,
		MALE_SCREAM3 =  6,
		EMPHANT_ROAR =  7,		// SCREAM4
		BANSHEE_ROAR =  8,		// SCREAM5
		MESSAGE =  9,	
		INTRO  =  10,	
		EXIT    =  11,
		TELEPORT =  12,
		MONSTER_ROAR =  13,		// MONSTER
		POTION  =  14,

		THROW_ITEM =  16,
		OTHER_STEP_1 = 17,
		OTHER_STEP_2 = 18,
		PLAYER_STEP_1 = 19,
		PLAYER_STEP_2 = 20,		
		AGOKNIGHT_ROAR = 21,
		PUSH = 22, 
		CHAMELE = 23,
		IDENTIFY = 24,
		SCARE_LOOP = 25, 
		FIRESTORM = 26,
		CURSE = 27,
		TRAPMARE = 28,
		POISON = 29,
		RESTORE = 30,
		REWEAVE = 31,
		HOLYLIGHT = 32,
		WARD = 33,
		MENU_CLICK = 34,
		FIREBALL_HIT_WALL = 35,
		JUMPLANDING  = 36,
		MESSAGE_SENT = 37,

		MESSAGE_ALERT = 38,
		ABJURE		= 39,
		FORGE		= 40,
		RECALL       = 41,
		BLAST		= 42, 
		BLEND		= 43,
		ID_CURSE	= 44, 
		VISION		= 45,
		STAGGER		= 46,
		DREAMBLADE	= 47,
		PARALYZE	= 48,
		RAZORWIND	= 49,
		TRAIL		= 50,
		DRAIN_ESSENCE = 51,
		TRAP_NIGHTMARE = 52,
		MEDITATION   = 53,
		PROTECT_AVATAR  = 54,
		RETURN		= 55,
		SHATTER 	= 56,
		ERROR_NOISE	= 57,
		EXPENDED_ITEM = 58,
		RANDOM		= 59,
		REJECTED	= 60,
		BLIND_LOOP  = 61,
		DEAFEN_LOOP = 62,
		ENTER_WATER = 63,
		WATER_STEP_1 = 64,
		WATER_STEP_2 = 65,
		INITIATE = 66,
		KNOW = 67,
		RECHARGE = 68,
		EARTHQUAKE = 73,
		REFLECT = 74,

		// TO DO : REORDER
		FEMALE_SCREAM1 = 69,
		FEMALE_SCREAM2 = 70,
		FEMALE_SCREAM3 = 71,
		FEMALE_SCREAM4 = 72,
		MAX_SOUND_ID = 73
	};

};

// NOTE: keep consistent with Palettes.ASC
struct LyraPalette {
	enum palette_ids {
		NONE	= -1,
		BANNER_PALETTE = 255,
		FX_PALETTE     = 1020
	};
};

// effects and other stuff
struct LyraEffect {
  enum {
	NONE = 0,
		// helpful effects
    PLAYER_DETECT_INVISIBLE,
    PLAYER_PROT_FEAR,
    PLAYER_PROT_CURSE,
    PLAYER_PROT_PARALYSIS,
	PLAYER_CHAMELED, 
    PLAYER_INVISIBLE,   
	MIN_GOOD_EFFECT = PLAYER_DETECT_INVISIBLE,
	MAX_GOOD_EFFECT = PLAYER_INVISIBLE,	

		// harmful effects
    PLAYER_DEAF,
    PLAYER_DRUNK,
    PLAYER_FEAR,
    PLAYER_POISONED, 
    PLAYER_CURSED,
    PLAYER_BLIND,
    PLAYER_PARALYZED,

	MIN_BAD_EFFECT = PLAYER_DEAF,
	MAX_BAD_EFFECT = PLAYER_PARALYZED,	
//	MAX_ITEM_EFFECT = PLAYER_PARALYZED, 
	PLAYER_RECALL, 
	PLAYER_TRANSFORMED,
	PLAYER_BLENDED,
    PLAYER_MEDITATING,
    PLAYER_TRAIL,
	PLAYER_RETURN,
	PLAYER_SOULEVOKE,
	PLAYER_MIND_BLANKED,
	PLAYER_REGENERATING,
	PLAYER_SOUL_SHIELD,
	PLAYER_REFLECT,
	PLAYER_PEACE_AURA,
	PLAYER_NO_PARTY,
	PLAYER_NO_POISON,
  PLAYER_SPIN,

	MAX_ITEM_EFFECT = PLAYER_SPIN
	};
};


// Guild Ranks, IDs, & Flags
struct Guild { // these values CAN NOT be changed or rearranged
	enum { // ranks (pending ones unused?)
		INITIATE_PENDING	= -1,
		KNIGHT_PENDING		= -2,
		RULER_PENDING       = -3,
		NO_RANK				= -4,
		INITIATE			= 1,
		KNIGHT				= 2,
		RULER				= 3,
		QUEST				= 4
	};

	enum { // constants for goals; do not make negative!
		SPHERE_ANY = 87,
		RULER_ISSUE = 88
	};

	enum { 
		NO_GUILD    =   15,
		NO_HOUSE	=   15,
		MOON		=	0,
		ECLIPSE		=	1,
		SHADOW		=	2,
		COVENANT	=	3,
		RADIANCE	=	4,
		CALENTURE	=	5,
		ENTRANCED	=	6,
		LIGHT		=	7
	};

	enum {
		MOON_FLAG		=	0x01,
		ECLIPSE_FLAG	=	0x02,
		SHADOW_FLAG		=	0x04,
		COVENANT_FLAG	=	0x08,
		RADIANCE_FLAG	=	0x10,
		CALENTURE_FLAG	=	0x20,
		ENTRANCED_FLAG	=	0x40,
		LIGHT_FLAG		=	0x80
	};

	enum { // goal status indicators
		GOAL_ACTIVE			= 0,
		GOAL_COMPLETED		= 1,
		GOAL_MAX_ACCEPTED	= 2,
		GOAL_TIME_EXPIRED	= 3,
		GOAL_PENDING_VOTE	= 4,
		GOAL_VOTED_DOWN		= 5,
		GOAL_RULER_VOTE		= 6,
		GOAL_RULER_PASSED	= 7,
		GOAL_RULER_FAILED	= 8
	};

	enum { // voting defs
		NO_VOTE					= 0,
		YES_VOTE				= 1,
		VOTE_EXPIRATION_DAYS	= 3
	};

	enum { // number of tokens required to demote one rank
	  DEMOTE_INITIATE = 1,
	  DEMOTE_KNIGHT = 2,
	  DEMOTE_RULER = 3
	};

	enum { // report flags -- max of six (2 digit num in db)
		REPORT_READ = 0x01 // report has been read by recipient
	};

	enum { // goal flags -- max of six (2 digit num in db)
		GOAL_ALL_MANAGE = 0x01 // all guardians can award xp for goal
	};

};

struct Quest { // for Quest builder missions
	enum { // types of quests	
		TALISMAN = 0,
		CODEX = 1,
		ANY_COLOR
	};

};

struct Weapons {
  enum {
	GATEKEEPER = 1,
	DREAMSEER,
	SOULMASTER,
	FATESENDER
  };
};

struct Tokens {
	enum {
		MEMBERSHIP = 1,
		ASCENSION_TO_RULER,
		DEMOTION,
		POWER_TOKEN
	};
};

const int MAX_DEMOTE_TOKENS_NEEDED = Guild::DEMOTE_RULER;

// for showing guild/sphere/shield patches
struct Patches {
	enum {  
		DONT_SHOW = 0,
		SHOW_FRONT = 1,    
		SHOW_BACK  = 2,
		SHOW_BOTH = 3
	};
};

enum QuestType {
  TALISMAN = 0,
  CODEX = 1
};

struct UsePPoint {
	enum {
		STAT_INCREASE = 0,
		BYPASS_TRAIN = 1,
		BYPASS_SPHERE = 2,
		USE_ART = 3
	};
};

const int no_dreamstrike_levels[] = 
{1, 17, 18, 20, 21, 22, 23, 24, 25, 26, 42, 44, 45, 39, 41, 47};

const int num_no_dreamstrike_levels =
  sizeof(no_dreamstrike_levels) / sizeof(int);

#endif
