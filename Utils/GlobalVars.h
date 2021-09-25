#pragma once
#include "..\SDK\CInput.h"
#include "..\SDK\CEntity.h"

namespace g
{
    extern CUserCmd*      pCmd;
    extern C_BaseEntity*  pLocalEntity;
    extern std::uintptr_t uRandomSeed;
	extern Vector         OriginalView;
	extern bool           bSendPacket;
	extern bool			  LagPeek;
	extern int            TargetIndex;
	extern Vector         EnemyEyeAngs[65];
	extern Vector         AimbotHitbox[65][28];
	extern Vector         RealAngle;
	extern Vector         FakeAngle;
	extern bool           Shot[65];
	extern bool           Hit[65];
	extern int            MissedShots[65];
	extern bool 		  PressedKeys[256];
	extern HMODULE        DLL;
	extern int			  TickBaseShift;
	extern Vector		  ViewmodelAimbotAngle;
	extern bool			  ValveServer;
	extern bool			  LocalServer;
	extern bool		      CommunityServer;
	extern bool		      NotConnectedToServer;
	extern float		  SpawnTime;
	extern std::array< AnimationLayer, 15 > FakeLayers;
	extern std::array< float, 24 >  FakePoses;
	extern C_AnimState*  FakeState;
	extern matrix3x4_t   FakeMatrix[128];
	extern matrix3x4_t   FakePosMatrix[128];
	extern bool			 HasFakeMatrix ;
	extern bool		     FakeLocalUpdate;
	extern bool		     InitFakeAnimation;

	extern DWORD CourierNew;
	extern DWORD Tahoma;
	extern DWORD HPFont;
	extern DWORD NameFont;
	extern DWORD WeaponFont;
	extern DWORD WeaponIconFont;
}