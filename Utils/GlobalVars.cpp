#include "GlobalVars.h"


namespace g
{
    CUserCmd*      pCmd         = nullptr;
    C_BaseEntity*  pLocalEntity = nullptr;
    std::uintptr_t uRandomSeed  = NULL;
	Vector         OriginalView;
	bool           bSendPacket  = true;
	bool		   LagPeek      = false;
	int            TargetIndex  = -1;
	Vector         EnemyEyeAngs[65];
	Vector         AimbotHitbox[65][28];
	Vector         RealAngle;
	Vector         FakeAngle;
	bool           Shot[65];
	bool           Hit[65];
	int            MissedShots[65];
	bool	       PressedKeys[256] = {};
	HMODULE        DLL;
	int			   TickBaseShift;
	Vector		   ViewmodelAimbotAngle;
	bool		   ValveServer;
	bool		   LocalServer;
	bool		   CommunityServer;
	bool		   NotConnectedToServer;
	float		   SpawnTime = 0.f;
	std::array< AnimationLayer, 15 > FakeLayers;
	std::array< float, 24 >  FakePoses;
	C_AnimState*  FakeState = nullptr;
	matrix3x4_t   FakeMatrix[128];
	matrix3x4_t   FakePosMatrix[128];
	bool		  HasFakeMatrix = false;
	bool		  FakeLocalUpdate = false;
	bool		  InitFakeAnimation = false;


	DWORD CourierNew;
	DWORD Tahoma;
	DWORD HPFont;
	DWORD NameFont;
	DWORD WeaponFont;
	DWORD WeaponIconFont;
}
