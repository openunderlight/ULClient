// Header file for the Neighbor class - derived from the Actor class

// Copyright Lyra LLC, 1996. All rights reserved. 

#ifndef CNEIGHBOR_H
#define CNEIGHBOR_H

#include "Central.h"
#include "4dx.h"
#include "cActor.h"
#include "LmAvatar.h"
#include "RmRemotePlayer.h"


//////////////////////////////////////////////////////////////////
// Constants

#define NO_NEIGHBOR		0

const int GENERIC_NEIGHBOR = 0;
const int NUM_POSES = 8;

//////////////////////////////////////////////////////////////////
// Enumerations

enum neighbor_type {
  LOCAL = 1,     // in local group      
  OUTSIDER = 2,     // outsider
};

// poses are listed in order of priority
enum pose_type {
	INJURED,
	MELEE_ATTACK,
	MISSILE_ATTACK,
	EVOKING,
	JUMP,
	WAVE,
	WALKING,
	STANDING
};

///////////////////////////////////////////////////////////////////
// Structures 

struct avatar_poses_t {
	int		start_frame;
	int		end_frame;
	bool    cycle;
	bool    repeat_backwards;
};

//////////////////////////////////////////////////////////////////
// Class Definition
class cNeighbor : public cActor
{
   friend class cActorList;

   public:

   protected:
		lyra_id_t			id;
		unsigned char		realtime_id;
		short				room;
		in_addr				ip; // neighbor's IP address
		TCHAR				name[Lyra::PLAYERNAME_MAX];
		TCHAR				generic_name[Lyra::PLAYERNAME_MAX];
		bool				visible;
		bool				locked; // in party?
		DWORD				last_attack;
		DWORD				last_hit;
		bool				strafing;
		bool				moved; // has been moved by player since last update?
		bool				jumping;
		bool				waving;
		bool				evoking;
		int					status; // local, outsider, neither
		LmAvatar			avatar; // their avatar 
		int					pose;
		int					frames_in_pose;
	    unsigned char       icon[ICON_WIDTH*ICON_HEIGHT*BYTES_PER_PIXEL];
		HWND				hwnd_acceptreject; 
		DWORD				enter_time;
		bool				blasting; // being blasted (to prevent chain blasting)
		bool				newbie_message; // to ensure we don't double message guarians/rulers
		bool				on_controlpanel;
    bool        got_update;

		struct
		{
			int effect_id;
			int animate_ticks;
			int current_frame;
		} halo;
 
   public:
	  float				destx,desty;
	  cNeighbor(const RmRemotePlayer& info, unsigned __int64 flags=0, int n_sector = DEAD_SECTOR); 
	  ~cNeighbor(void);
	  bool LeftClick(void);
	  bool RightClick(void);
	  void SetUpdateFlags(const LmPeerUpdate& update);
	  void SelectFrame(void);
	  void PlayFootstep(void);
	  
	  // Locking/Unlocking functions
	  inline bool Locked(void) { return locked; };
	  inline void Lock(void) { locked = TRUE; };
	  void Unlock(void); 

		// Selectors
	  inline lyra_id_t ID(void) { return id; };
	  inline unsigned short Room(void) { return room; };
	  inline unsigned char RealtimeID(void) { return realtime_id; };
	  inline in_addr IP(void) { return ip; };
//    inline TCHAR* Name(void) { return name };
	  inline DWORD LastAttack(void) { return last_attack; };
	  inline DWORD LastHit(void) { return last_hit; };
	  inline LmAvatar Avatar(void) { return avatar;};
	  inline unsigned char* IconBits(void) { return (unsigned char*)icon; };
	  inline bool Moved(void) { return moved; };
	  inline bool SurviveLevelChange(void) { return FALSE; };
	  inline bool Visible(void) { return visible; };
	  inline int Pose(void) { return pose; }; 
	  inline bool Jumping(void) { return jumping; }; 
	  inline bool Waving(void) { return waving; }; 
	  inline bool Evoking(void) { return evoking; }; 
	  inline bool Strafing(void) { return strafing; };
	  inline bool Blasting(void) { return blasting; };
	  inline int GuildPatchID(void) { return avatar.GuildID(); };
	  inline int GuildRankID(void) { return avatar.GuildRank(); };
	  inline int ShowGuildID(void) { return avatar.ShowGuild(); };
	  inline int HeadID(void) { return avatar.Head(); };
	  inline int ShowSphereID(void) { return avatar.ShowSphere(); };
	  inline int SphereID(void) { return avatar.Sphere(); };
	  int TeacherID(void);
	  inline int HaloBitmap (void) { return halo.effect_id + halo.current_frame ; };
	  inline HWND Hwnd_AR(void) { return hwnd_acceptreject; };
	  inline DWORD EnterTime(void) { return enter_time; };
	  inline bool NewbieMessage(void) { return newbie_message; };
    inline bool GotUpdate (void) { return got_update; };
	 
      TCHAR* Name(void);  

	  unsigned int GetAvatarType (void);
	  unsigned int GetAccountType (void);
	  unsigned int GetTransformedMonsterType (void);

	  bool IsFemale (void);
	  bool IsMale (void);
	  bool IsMonster (void);
	  bool IsVulnerable (void);
	  bool CanWhisper(void);
	  bool IsAgentAccount (void);
	  bool IsDreamerAccount (void);

	  // Mutators
	  inline void SetIP(in_addr value) { ip = value; };
	  inline void SetLastAttack(DWORD time) { last_attack = time; };
	  inline void SetLastHit(DWORD time) { last_hit = time; };
	  inline void SetStrafe(bool strafe) { strafing = strafe; };
	  inline void SetMoved(bool value) { moved = value; };
	  inline void SetVisible(bool value) { visible = value; };
	  inline void SetJumping(bool value) { jumping = value; }; 
	  inline void SetWaving(bool value) { waving = value; }; 
	  inline void SetBlasting(bool value) { blasting = value; }; 
	  inline void SetEvoking(bool value) { evoking = value; }; 
	  inline void SetHwnd_AR(HWND value) { hwnd_acceptreject = value; };
	  inline void SetNewbieMessage(bool value) { newbie_message = value; };
    inline void SetGotUpdate (bool value) { got_update = value; };

	  void SetRoom(short value);
	  void SetPose(int value, bool force = false);
	  void SetAvatar(LmAvatar new_avatar);

	  void Form(void);
	  void Dissolve(void);

	  // Status Functions
	  inline bool IsLocal(void) { if (status == LOCAL) return TRUE; else return FALSE; };
	  inline bool IsOutsider(void) { if (status == OUTSIDER) return TRUE; else return FALSE; };
	  void MakeLocal(void);
	  void MakeOutsider(void); 
	  bool Render(void);
	  inline bool RenderNameTag(void) { return !(this->IsMonster()); };

   protected:

   private:
	  bool Update(void);
	void UpdateHaloFX(void);

	// copy constructor and assignment operator are
	// private and undefined -> errors if used
	cNeighbor(const cNeighbor& x);
	cNeighbor& operator=(const cNeighbor& x);

	
#ifdef CHECK_INVARIANTS
	void CheckInvariants(int line, TCHAR *file);
#else
	inline void CheckInvariants(int line, TCHAR *file) {};
#endif

};
#endif

