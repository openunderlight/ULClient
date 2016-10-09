// Header file for cArts class 

// Copyright Lyra LLC, 1997. All rights reserved. 

#ifndef CARTS_H
#define CARTS_H

#define STRICT

#include "Central.h"
#include "SharedConstants.h"
#include "GMsg_ViewItem.h"
#include "LyraDefs.h"

//////////////////////////////////////////////////////////////////
// Constants

const int ART_DESCRIP_LEN = 18;
const int NOT_CASTING = 0;
 // can't learn arts outside your focus with min orbit of 20+
const int MAX_OUT_OF_FOCUS_MIN_ORBIT = 19;

class cArts;
class cNeighbor;
class cItem;
class cPostQuest;

// function type for cArts methods to use arts
typedef void (cArts::*art_method_t)(void);
typedef void (cArts::*art_dlg_callback_t)(void *value);
typedef void (cPostQuest::*quest_dlg_callback_t)(void* value);


//////////////////////////////////////////////////////////////////
// Helpers

void ArtsDlgCallback(void* value);

//////////////////////////////////////////////////////////////////
// Class Definition
class cArts
{


public: 

	friend class cItem; // let cItem use our methods/variables

private:

	TCHAR no_art[20];
	bool waiting_for_sel; // true when we're waiting for a selection to be made
	bool waiting_for_dlg; // true when we're waiting for dialog box input
  bool displayed_await_update_art;
	HWND active_dlg; // hwnd for dialog box we're waiting on
	lyra_id_t art_in_use; // set to the id of art in use, else NO_ART
	art_method_t callback_method; // pointer to callback method when for mouse selection, else NULL
	cNeighbor *dummy; // dummy neighbor representing the player
	cNeighbor *quest_student; // set before Quest text dialog so other Who list selections during evoke don't interrupt
	int cp_mode; // store current tab on cp for when we change it
	DWORD art_completion_time; // time at which current casting finishes
	bool fDoingLocate;
	bool fDoingNewlies;
	bool fDoingMares;
	bool fDoingSense;


	// random state variables used by arts
	TCHAR knight_name[Lyra::PLAYERNAME_MAX];
	lyra_id_t restore_id, initiator_id, initiate_gid, rogerwilco_id;
	int restore_skill, restore_art, demote_guild_id, rally_x, rally_y;
	cItem *giving_item;
	cItem *combining_item;
	cItem *gratitude_item;
	cItem *receiving_item;
	cItem *quest_item;
	TCHAR art_name[64];
	
public:
    cArts(void);
    ~cArts(void);
	//CreateIcons(void); // create icons & add to control panel
	void BeginArt(int art_id, bool bypass = false); // begin casting
	bool CanUseArt(int art_id, bool bypass = false); // can the art be evoked?
	void ApplyArt(void); // casting has finished - perform skill trial & launch if successful
	void CheckForSelection(void); 
	void CancelArt(void);
	void CheckTarget(void);
	inline bool Waiting(void) { return (waiting_for_sel || waiting_for_dlg);};
	inline bool WaitingForSelection(void) { return waiting_for_sel; };
	inline bool WaitingForDialog(void) { return waiting_for_dlg; };
	inline bool DummyActive(void) { return (dummy != NULL); };
	inline void SetGivingItem(cItem *value) { giving_item = value;};
	inline void SetGratitudeItem(cItem *value) { gratitude_item = value;};
	inline bool DoingLocate(void) {return fDoingLocate; };
  inline bool DisplayedAwaitUpdateArt (void) { return displayed_await_update_art; };
	
	int EffectiveForgeSkill(int player_skill, bool usePowerToken);
	int Duration(int art_id, int skill); // calculates duration for art
	bool UseInSanctuary(int art_id); // usable in sanctuary?
	bool Restricted(int art_id); // focus restricted?
	int CanPlateauArt(int art_id);
	bool Learnable(int art_id);
	bool DisplayLearnable(int art_id);
	bool IncreaseSkill(int art_id,int chance_increase);
	short PPMultiplier(int art_id);
	void DamagePlayer(int damage, lyra_id_t caster_id);

	
	// arts that require a selection of a target are broken into
	// two methods - start<art> and end<art>. Start initializes things
	// to wait for the selection, and end performs the art after the
	// selection has been made.

	// arts that require no selection
	void ApplyWeapon(void); 
	void NotImplemented(void); // place holder
	void Meditate(void);
	void Lock(void);
	void Ward(void);
	void Key(void);
	void Amulet(void);
	void Shatter(void);
	void Know(void);
	void Chamele(void);
	void Invisibility(void);
	void Blend(void);
	void CreateLocalWeapon(int color); // used by next 4 arts
	void Dreamblade(void);
	void GateSmasher(void);
	void FateSlayer(void);
	void SoulReaper(void);
	void LaunchFireball(void); // used by next 4 arts
	void FlameShaft(void);
	void TranceFlame(void);
	void FlameSear(void);
	void FlameRuin(void);
//	void DreamsmithMark(void); // Jared 1-31-00 Not evokable
	void StartSenseDreamers(void);
	void EndSenseDreamers(void* value);
	void Random(void);
	void Trail(void);
	void Push(void);
	void SoulEvoke(void);
	void NightmareForm(void);
	void Recall(void);
	void Return(void);
	void Reflect(void);
	void ApplyReflectedArt(int art_id, lyra_id_t caster_id);
	void Firestorm(void);
	void ApplyFirestorm(int skill, lyra_id_t caster_id);
	void Razorwind(void);
	void ApplyRazorwind(int skill, lyra_id_t caster_id);
	void HypnoticWeave(void);
	void ApplyHypnoticWeave(int skill, lyra_id_t caster_id);
	void Earthquake(void);
	void ApplyEarthquake(int skill, lyra_id_t caster_id);
	void Darkness(void);
	void ApplyDarkness(int skill, lyra_id_t caster_id);
	void StartForgeTalisman(void);
	void EndForgeTalisman(void *value, bool usePT);
	void Terror(void);
	void ApplyTerror(int skill, lyra_id_t caster_id);
	void HealingAura(void);
	void ApplyHealingAura(int skill, lyra_id_t caster_id);
	void GotHugged(void *value);
	void EndHealingAura(void);
	void StartFindNewlies(void);
	void EndFindNewlies(void *value);
	void StartFindMares(void);
	void EndFindMares(void *value);
	void RadiantBlaze(void);
	void ApplyRadiantBlaze(int skill, lyra_id_t caster_id);
	void PoisonCloud(void);
	void ApplyPoisonCloud(int skill, lyra_id_t caster_id);
	void StartBreakCovenant(void);
	void ApplyBreakCovenant(int skill, lyra_id_t caster_id);
	void EndBreakCovenant(void);
	void Dazzle(void);
	void ApplyDazzle(int skill, lyra_id_t caster_id);
	void GuildHouse(void);
	void TehthusOblivion(void);
  void Tempest (void);
  void ApplyTempest (int skill, int angle, lyra_id_t caster_id);
  void Misdirection (void);
  void ApplyMisdirection (int skill, lyra_id_t caster_id);
  void ChaoticVortex (void);
  void ApplyChaoticVortex (int skill, lyra_id_t caster_id);
  void EssenceContainer(void);

	// arts that require selecting a neighbor
	void StartChannel(void);
	bool ExpireChannel(void);
	bool SetChannel(lyra_id_t nid);
	void EndChannel(void);
	void StartJoin(void);
	void EndJoin(void);
	void StartResistFear(void);
	void ApplyResistFear(int skill, lyra_id_t caster_id);
	void EndResistFear(void);
	void StartResistCurse(void);
	void ApplyResistCurse(int art_id, int skill, lyra_id_t caster_id);
	void EndResistCurse(void);
	void StartResistParalysis(void);
	void ApplyResistParalysis(int skill, lyra_id_t caster_id);
	void EndResistParalysis(void);
	void StartAntidote(void);
	void ApplyAntidote(int skill, lyra_id_t caster_id);
	void EndAntidote(void);
	void StartIdentifyCurse(void);
	void ApplyIdentifyCurse(int skill, lyra_id_t caster_id);
	void EndIdentifyCurse(void);
	void StartVision(void);
	void ApplyVision(int skill, lyra_id_t caster_id);
	void EndVision(void);
    void StartRestore(void);
	void ApplyRestore(int art_id, int skill, lyra_id_t caster_id);
	void GotRestored(void *value);
	void EndRestore(void);
    void StartRogerWilco(void);
	void QueryRogerWilco(lyra_id_t caster_id);
	void RogerWilco(void *value);
	void RogerWilcoAck(int success);
	void EndRogerWilco(void);
	void StartPurify(void);
	void ApplyPurify(int art_id, int skill, lyra_id_t caster_id);
	void EndPurify(void);
	void StartDrainSelf(void);
	void ApplyDrainSelf(int stat, int amount, lyra_id_t caster_id);
	void EndDrainSelf(void);
	void StartScare(void);
	void ApplyScare(int skill, lyra_id_t caster_id);
	void EndScare(void);
	void StartCurse(void);
	void ApplyCurse(int skill, lyra_id_t caster_id);
	void EndCurse(void);
	void StartParalyze(void);
	void ApplyParalyze(int art_id, int skill, lyra_id_t caster_id);
	void EndParalyze(void);
	void StartStagger(void);
	void ApplyStagger(int skill, lyra_id_t caster_id);
	void EndStagger(void);
	void StartDeafen(void);
	void ApplyDeafen(int skill, lyra_id_t caster_id, bool roar = false);
	void EndDeafen(void);
	void StartBlind(void);
	void ApplyBlind(int skill, lyra_id_t caster_id);
	void EndBlind(void);
	void StartBlast(void);
	void ApplyBlast(int skill, lyra_id_t caster_id);
	void EndBlast(void);
	void StartPoison(void);
	void ApplyPoison(int skill, lyra_id_t caster_id);
	void EndPoison(void);
	void StartTrapMare(void);
	void ApplyTrapMare(int skill, int guild_flags, lyra_id_t caster_id);
	void EndTrapMare(void);
	void StartJudgement(void);
	void ApplyJudgement(int skill, lyra_id_t caster_id);
	void ApplyScan(int skill, lyra_id_t caster_id);
	void EndJudgement(void);
	void StartAbjure(void);
	void ApplyAbjure(int skill, lyra_id_t caster_id);
	void EndAbjure(void);
	void StartDie(void);
	void ApplyDie(lyra_id_t caster_id);
	void EndDie(void);
	void StartGrantXP(void);
	void MidGrantXP(void);
	void ApplyGrantXP(int k, int c, lyra_id_t caster_id);
	void EndGrantXP(void *value);
	void StartTerminate(void);
	void ApplyTerminate(lyra_id_t caster_id);
	void EndTerminate(void);
	void StartDreamStrike(void);
	void ApplyDreamStrike(lyra_id_t caster_id, int success);
	void EndDreamStrike(void);
	void StartSphere(void);
	void ApplySphere(int success, lyra_id_t caster_id);
	void ResponseSphere(lyra_id_t caster_id, int success, int sphere);
	void EndSphere(void);
	void StartMindBlank(void);
	void ApplyMindBlank(int skill, lyra_id_t caster_id);
	void EndMindBlank(void);
	void StartBoot(void);
	void ApplyBoot(lyra_id_t caster_id);
	void EndBoot(void);
	void StartGrantRPXP(void);
	void MidGrantRPXP(void);
	void EndGrantRPXP(void *value);
	void StartVampiricDraw(void);
	void ApplyVampiricDraw(lyra_id_t caster_id, int amount, int stat);
	void VampiricDrawAck(lyra_id_t caster_id, int amount, int stat);
	void EndVampiricDraw(void);
	void StartSupportSphering(void);
	void EndSupportSphering(void);
	void StartSoulShield(void);
	void ApplySoulShield(int skill, lyra_id_t caster_id);
	void EndSoulShield(void);
	void StartSummon(void);
	void ApplySummon(lyra_id_t caster_id, int x, int y, int lvl);
	void MidSummon(void);
	void EndSummon(void *value);
	void StartSuspend(void);
	void MidSuspend(void);
	void ApplySuspend(int num_days, lyra_id_t caster_id);
	void EndSuspend(void* value);
	void StartExpel(void);
	void ApplyExpel(int guild_id, lyra_id_t caster_id);
	void EndExpel(void);
	void StartPeaceAura(void);
	void ApplyPeaceAura(int skill, lyra_id_t caster_id);
	void EndPeaceAura(void);
	void StartSableShield(void);
	void ApplySableShield(int skill, lyra_id_t caster_id);
	void EndSableShield(void);
	void StartEntrancement(void);
	void ApplyEntrancement(int skill, lyra_id_t caster_id);
	void EndEntrancement(void);
	void StartShadowStep(void);
	void ApplyShadowStep(int skill, lyra_id_t caster_id);
	void EndShadowStep(void);
	void StartChaosPurge(void);
	void ApplyChaosPurge(lyra_id_t caster_id);
	void EndChaosPurge(void);
	void StartCupSummons(void);
	void ApplyCupSummons(lyra_id_t caster_id);
	void GotCupSummoned(void *value);
	void EndCupSummons(void);
  void StartKinesis (void);
  void ApplyKinesis (int skill, lyra_id_t caster_id, int angle);
  void EndKinesis (void);
  void StartRally (void);
  void ApplyRally(lyra_id_t caster_id, int x, int y);
  void GotRallied (void *value);
  void EndRally (void);

	// arts that require selecting a skill
	void StartTrainSelf(void);
	void EndTrainSelf(void);
	void ResponseTrainSelf(int art_id, int success);

	// arts that require selecting an item
	void StartReweave(void);
	void EndReweave(void);
	void StartRecharge(void);
	void EndRecharge(void);
	void StartIdentify(void);
	void EndIdentify(void);
	void StartDrainMare(void);
	void EndDrainMare(void);
	void StartBanishMare(void);
	void EndBanishMare(void);
	void StartEnslaveMare(void);
	void EndEnslaveMare(void);
	void StartDestroyItem(void);
	void EndDestroyItem(void);
	void StartSacrifice(void);
	void EndSacrifice(void);
	void StartCleanseMare(void);
	void EndCleanseMare(void);
	void StartCorruptEssence(void);
	void EndCorruptEssence(void);

	// arts that require selecting a neighbor and a skill
	void StartTrain(void);
	void ApplyTrain(int art_id, int success, lyra_id_t caster_id);
	void MidTrain(void);
	void EndTrain(void);
	void CompleteTrain(int success, lyra_id_t target_id);
	void StartUnTrain(void);
	void ApplyUnTrain(int art_id, lyra_id_t caster_id);
	void MidUnTrain(void);
	void EndUnTrain(void);
	void StartSupportTraining(void);
	void MidSupportTraining(void);
	void EndSupportTraining(void);
	void StartQuest(void);
	void MidQuest1(void);
	void MidQuest2(void);
	void EndQuest(void* value);

	// arts that require selecting a neighbor and an item
	void StartGive(void);
	void ApplyGive(cItem *item, lyra_id_t caster_id);
	void GiveReply(void *value);
	void MidGive(void);
	void EndGive(void);
	void StartShow(void);
	void ApplyShow(GMsg_ViewItem& view_item);
	void MidShow(void);
	void EndShow(void);
	void StartFreesoulBlade(void);
	void MidFreesoulBlade(void);
	void EndFreesoulBlade(void* value);
	void StartIlluminatedBlade(void);
	void MidIlluminatedBlade(void);
	void EndIlluminatedBlade(void* value);

	// arts that require selecting a neighbor and a guild
	void StartInitiate(void);
	void ApplyInitiate(int guild_id, int success, lyra_id_t caster_id);
	void CompleteInitiate(int guild_id, int success, lyra_id_t initiate);
	void GotInitiated(void *value);
	void MidInitiate(void);
	void EndInitiate(void *value);
	void StartKnight(void);
	void ApplyKnight(int guild_id, int success, lyra_id_t caster_id);
	void MidKnight(void);
	void EndKnight(void *value);
	void StartSupportAscension(void);
	void MidSupportAscension(void);
	void EndSupportAscension(void *value);
	void StartSupportDemotion(void);
	void MidSupportDemotion(void);
	void EndSupportDemotion(void *value);


	// arts that require selecting a neighbor and making a value entry
	void StartEmpathy(void);
	void MidEmpathy(void);
	void ApplyEmpathy(int success, lyra_id_t caster_id);
	void EndEmpathy(void* value);

	// arts that require selecting a neighbor and making a value entry
	void StartGrantPPoint(void);
	void MidGrantPPoint(void);
	void EndGrantPPoint(void* value);

	// arts that require selecting a guild
	void StartPowerToken(void);
	void EndPowerToken(void *value);

	void StartHouseMembers(void);
	void EndHouseMembers(void* value);

	// arts that require selecting an item and a guild
	void StartCreateIDToken(void);
	void MidCreateIDToken(void);
	void EndCreateIDToken(void *value);

	// arts that require selecting two items
	void StartCombine(void);
	void MidCombine(void);
	void EndCombine(void);

	// arts that require input from dialogs only
	void StartAscend(void);
	void EndAscend(void *value);
	void ResponseAscend(int guild_id, int success);
	void StartLocate(void);
	void EndLocate(void *value);
	void StartWriteScroll(void);
	void EndWriteScroll(void *value);
	void StartDemote(void);
	void MidDemote(void);
	void SelfDemote(void *value);
	void EndDemote(void *value);
	void ResponseDemote(bool success, realmid_t target_id, int guild_id, int num_tokens_used);
	void ApplyDemote(int guild_id);
	void StartSummonPrime(void);
	void ApplySummonPrime(int guild_id, int success);
	void EndSummonPrime(void* value);
	int CountPowerTokens(cItem** tokens, lyra_id_t guild_id = Guild::NO_GUILD);

	// pseudo arts that need to use cArts methods 
	void StartShowGratitude(void);
	void MidShowGratitude(void);
	void EndShowGratitude(void* value);

	// Selectors
	TCHAR* Descrip(int art_id);
	int	  Stat(int art_id);
	int	  Drain(int art_id);
	int	  MinOrbit(int art_id);
	inline lyra_id_t CurrentArt(void) { return art_in_use;};
	inline bool Casting(void) { return ((art_in_use != Arts::NONE) && (art_completion_time != NOT_CASTING));};
	inline DWORD ArtCompletionTime(void) { return art_completion_time;};
	inline HWND ActiveDlg(void) { return active_dlg; };
	cNeighbor* LookUpNeighbor(lyra_id_t id);
	void WaitForDialog(HWND hDlg, lyra_id_t art_id);


private:
	void CreatePass(const TCHAR* name, int strength);
	bool PlaceLock(lyra_item_ward_t ward, LmItemHdr header);
	void WaitForSelection(art_method_t callback, lyra_id_t art_id); // to start waiting for a click
	void AddDummyNeighbor(void); // to allow selection of player as a target
	void RemoveDummyNeighbor(void);
	void CaptureCP(int new_mode, lyra_id_t art_id); // set cp mode
	void RestoreCP(void); // restore cp mode
	void DisplayUsedByOther(cNeighbor *n, lyra_id_t art_id);
	void DisplayUsedOnOther(cNeighbor *n, lyra_id_t art_id);
	void DisplayNeighborBailed(lyra_id_t art_id);
	void DisplayItemBailed(lyra_id_t art_id);
	void DisplayArtBailed(lyra_id_t art_id);
	void DrainStat(lyra_id_t art_id);
	void ArtFinished(bool drain, bool allow_skill_increase = true);

	// helper methods that go through a player's inventory
	int CountTrainSphereTokens(lyra_id_t art_id, lyra_id_t target_id, cItem** tokens, bool unique = true);
	int CountAscensionTokens(lyra_id_t guild_id, cItem** tokens);
	int CountDemotionTokens(lyra_id_t guild_id, cItem** tokens, cNeighbor* n);
	cItem* FindPrime(lyra_id_t guild_id, int min_charges);
	cItem* HasQuestCodex(lyra_id_t neighbor_id, lyra_id_t art_id);
	bool IsSharesFocus(lyra_id_t target_focus_id);


	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cArts(const cArts& x);
	cArts& operator=(const cArts& x);

#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};

#endif