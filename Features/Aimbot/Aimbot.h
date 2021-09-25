#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include <deque>

struct AimbotData_t
{
	AimbotData_t(C_BaseEntity* Player, const int& IDX)
	{
		this->pPlayer = Player;
		this->Index = IDX;
	}
	C_BaseEntity*   pPlayer;
	int				Index;
};

#define square( x ) ( x * x )

#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / g_pGlobalVars->intervalPerTick ) )
#define TICKS_TO_TIME( t )		( g_pGlobalVars->intervalPerTick *( t ) )

class Aimbot
{
public:
	struct DoubleTap
	{
		int SkippedTicks;
		bool DidShot = false;
		bool ShouldAA;
		bool NextShotDT;
		int CMDTick;
		int BaseTick;
		int SkipTicks;
		bool DTCharged;
		bool Shot = false;
		bool ForceRecharge;
		int UnPredTick;
		uint32_t ShotCMD{};
		bool OverrideVelMod;
		float VelMod;
		int32_t SinceUse;
		bool TickShifted;
		bool ShootedFirstBullet;
		bool IsItWorthTheShift;
	}DoubleTapStruct;

	void RunRagebot(CUserCmd* CMD);
	void Autostop();
	void SelectTarget();
	void ShootEnemy(CUserCmd* CMD);
	bool CanShoot(CUserCmd* CMD);
	float AcceptedInaccuracy(C_BaseCombatWeapon* Weapon);
	float GetCurtime();
	void ExtrapolateEntitys();

	void FixTickCount(CUserCmd* CMD);

	void MinWalk(CUserCmd* CMD, float MaxSpeed);

	void InitDoubleTap(CUserCmd* CMD);
	void DoubleTap(CUserCmd* CMD);

	std::vector<Vector> GetMultiplePointsForHitbox(C_BaseEntity* Local, C_BaseEntity* Entity, int iHitbox, matrix3x4_t BoneMatrix[128]);

	Vector HeadMultipoint(C_BaseEntity* Entity);
	Vector BodyMultipoint(C_BaseEntity* Entity);
	Vector FullMultipoint(C_BaseEntity* Entity);

	matrix3x4_t Matrix[65][128];
	bool Backtrack[65];

	std::vector<AimbotData_t>	Entities;
private:
	int BestHitbox = -1, MostDamage;
	Vector Multipoints[128];
	int MultipointCount = 0;
	bool LagComp;
	float BestFov = 360.0f;
	float BestDist = 8192.f;
	float BestThreat = 0.f;
	C_BaseEntity* BestTarget = nullptr;
};

extern Aimbot g_Aimbot;