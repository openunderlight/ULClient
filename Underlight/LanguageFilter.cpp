// LanguageFilter.cpp: Functions and constants for language filtering

// Copyright Lyra LLC, 2001. All rights reserved. 

#define STRICT


#include "Central.h"
#include <windows.h>
#include <stdio.h>
#include "RMsg_Speech.h"
#include "LanguageFilter.h"

// below is the list of adult words filtered out
const int NUM_BAD_WORDS = 119;

static TCHAR* bad_words[NUM_BAD_WORDS] = {
_T("anal"), 
_T("ass"),
_T("ass lick"),
_T("ass munch"),
_T("asshole"),
_T("assman"),
_T("bastard"),
_T("big dick"),
_T("big tits"),
_T("bitch"),
_T("bitch slap"),
_T("bitches"),
_T("bitchy"),
_T("blow job"),
_T("boob"),
_T("boob man"),
_T("boobies"),
_T("booby"),
_T("butt fuck"),
_T("butt fucked"),
_T("bullshit"),
_T("cock"),
_T("cock biter"),
_T("cock fight"),
_T("cock fighter"),
_T("cock fuck"),
_T("cock sucker"),
_T("coke head"),
_T("condom"),
_T("cum"),
_T("cum bubble"),
_T("cum stain"),
_T("cumming"),
_T("cunt"),
_T("cunt head"),
_T("damn"),
_T("damn you"),
_T("devil worship"),
_T("dick"),
_T("dick tease"),
_T("dickwad"),
_T("dickwipe"),
_T("dildo"),
_T("easy fuck"),
_T("faggot"),
_T("fornicate"),
_T("fornicating"),
_T("fornicator"),
_T("fuck"),
_T("fuck a duck"),
_T("fuck it"),
_T("fuck me"),
_T("fuck you"),
_T("fucked"),
_T("fucker"),
_T("fuckhead"),
_T("fucking"),
_T("fuckster"),
_T("fudge packer"),
_T("gigolo"),
_T("gonorrhea"),
_T("handjob"),
_T("hardon"),
_T("hickie"),
_T("hicky"),
_T("homo"),
_T("homosexual"),
_T("hooker"),
_T("horny"),
_T("huge dick"),
_T("intercourse"),
_T("jack off"),
_T("jack offer"),
_T("jacked off"),
_T("jacking off"),
_T("marijuana"),
_T("masturbate"),
_T("mother fucker"),
_T("necrophiliac"),
_T("nigger"),
_T("oral sex"),
_T("penis"),
_T("penis head"),
_T("penis lick"),
_T("pussy"),
_T("rape"),
_T("rapist"),
_T("rectum"),
_T("shit"),
_T("sh1t"),
_T("sh1thead"),
_T("shit canned"),
_T("shit eater"),
_T("shit faced"),
_T("shit for brains"),
_T("shit for grin"),
_T("slut"),
_T("smoke crack"),
_T("smoke pot"),
_T("snatch"),
_T("son of a bitch"),
_T("sperm"),
_T("spermicide"),
_T("stripper"),
_T("suck a dick"),
_T("suck dick"),
_T("suck pussy"),
_T("suicide"),
_T("syphilis"),
_T("testicle"),
_T("tits"),
_T("titty"),
_T("twat"),
_T("vibrator"),
_T("wanker"),
_T("wankster"),
_T("whore"),
_T("x rated"),
_T("XXX"),
};


/////////////////////////////////////////////////////////////////
// Functions

TCHAR* MareSpeak(TCHAR *in_buffer, int speechType)
{
	switch (speechType)
	{
		case RMsg_Speech::EMOTE:
// 		return GruntFilter(in_buffer);
		case RMsg_Speech::SHOUT:
		case RMsg_Speech::WHISPER:
		case RMsg_Speech::WHISPER_EMOTE:
		case RMsg_Speech::SPEECH:
		case RMsg_Speech::RAW_EMOTE:
		case RMsg_Speech::GLOBALSHOUT:
			return BabbleFilter(in_buffer);
		default:
			break;
	}
	return in_buffer;
}

TCHAR* GruntFilter(TCHAR *in_buffer)
{
	static TCHAR grunt_buffer[DEFAULT_MESSAGE_SIZE];
	_tcscpy(grunt_buffer,_T("Grunt"));
	return grunt_buffer;
}

TCHAR* AdultFilter(TCHAR *in_buffer)
{
	TCHAR* punc=_T("`~!@#$%^&*()_+[{]}\\|;:'\",<.>/? ");
	TCHAR* word;
	static TCHAR adult_buffer[DEFAULT_MESSAGE_SIZE];
	static TCHAR lowercase_adult_buffer[DEFAULT_MESSAGE_SIZE];
		//so capitalized swear words don't slip through the filter

	_tcscpy((TCHAR*)adult_buffer, in_buffer);
	_tcscpy((TCHAR*)lowercase_adult_buffer, in_buffer);

	for(UINT i=0; i<_tcslen(lowercase_adult_buffer); i++)
		lowercase_adult_buffer[i]=_totlower(lowercase_adult_buffer[i]);

	for (UINT i=0; i<NUM_BAD_WORDS; i++) 
	{
		word = _tcsstr(lowercase_adult_buffer, bad_words[i]);
		if (word != NULL) // copy over bad (whole) words with whitespace
		{	// if next character is not whitespace, skip
			int offset = (int)(word - lowercase_adult_buffer);
			int buffer_len = _tcslen(lowercase_adult_buffer);
			int bad_word_len = _tcslen(bad_words[i]);
			if ((buffer_len >= (offset + bad_word_len + 1)) &&
				(_tcschr(punc, lowercase_adult_buffer[offset + bad_word_len]) == NULL))
				continue;
			// if character preceding bad word is not whitespace, skip
			if ((offset > 0) &&	(_tcschr(punc, in_buffer[offset -1]) == NULL))
				continue;

			for (UINT j=0; j<_tcslen(bad_words[i]); j++)
				adult_buffer[offset + j] = ' ';
				//so we don't lose capitalization in the original message
		}
	}
	
	return adult_buffer;
}


TCHAR* BabbleFilter(TCHAR *in_buffer) //obfuscates what player mares say
{
	static TCHAR babble_buffer[DEFAULT_MESSAGE_SIZE];
	TCHAR* vowels=_T("aeiou");
	TCHAR* punc=_T("`~!@#$%^&*()_+[{]}\\|;:'\",<.>/? ");
	TCHAR* alpha=_T("abcdefghijklmnopqrstuvwxyz");
	bool cap_next = true;
	bool was_space = false;
	//bool escape = false;

	size_t i = 0, j = 0;
	for (i=0,j=0; i< _tcslen(in_buffer); i++,j++)
	{
		TCHAR c = in_buffer[i];

		if (_istupper(c))
			cap_next = true;

		c =_totlower(c);
		babble_buffer[j] = c;

		if (cap_next)
		{
// 		cap_next = false;
			c =_totupper(c);
		}
		TCHAR* p = _tcschr(vowels,c);
		if (p&&*p) continue;

		if (c ==' ')
		{
			was_space = true; continue;
		}

		p = _tcschr(punc,c);
		if (p&&*p)
		{
			//escape = false;
			continue;
		}

		c = alpha[rand()%26];
		if (cap_next)
		{
			cap_next = false;
			c =_totupper(c);
		}
		babble_buffer[j] = c;

		if ((rand()%100)< ((was_space)?3:10))
		{
			babble_buffer[++j] = ' ';
			was_space = true;
			cap_next = ((rand()%3))?false:true;
		}
		else
			was_space = false;

	}
	babble_buffer[j]='\0';
	return babble_buffer;
}