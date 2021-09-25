#include "Aimbot.h"
#include "Autowall.h"
#include "LagComp.h"
#include "..\AntiAim\AntiAim.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\SDK\ICvar.h"
#include "..\..\SDK\ISurface.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\SDK\CEntity.h"
#include "..\..\SDK\IMaterial.h"
#include "..\..\SDK\IVModelInfo.h"
#include "..\..\SDK\IVRenderView.h"
#include "..\..\SDK\CGlowObjectManager.h"
#include "..\..\SDK\ClientClass.h"
#include "..\..\SDK\CModelRender.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\Menu\Menu.h"
#include "../../SDK/CPrediction.h"

Aimbot g_Aimbot;

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

void TraceLineAlt(Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, C_BaseEntity* ignore, C_Trace* ptr)
{
	C_TraceFilter filter(ignore);
	g_pTrace->TraceRay(C_Ray(vecAbsStart, vecAbsEnd), mask, &filter, ptr);
}

bool IsTargetingLocal(C_BaseEntity* Entity)
{
	Vector src, dst, forward;
	C_Trace tr;

	if (!Entity)
		return false;

	Vector Viewangle = Entity->GetEyeAngles();

	g_Math.AngleVectors(Viewangle, &forward);
	forward *= 8142.f;
	src = Entity->GetEyePosition();
	dst = src + forward;

	TraceLineAlt(src, dst, mask_shot, Entity, &tr);

	if (tr.m_pEnt == g::pLocalEntity)
		return true;

	return false;
}

template<class T, class U>
T fine(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

void Aimbot::ExtrapolateEntitys()
{
	if (!g::pLocalEntity->IsAlive())
		return;

	for (int i = 1; i <= g_pGlobalVars->maxClients; i++)
	{
		C_BaseEntity* Entity = g_pEntityList->GetClientEntity(i);
		if (Entity) 
		{
			float SimtimeDelta = Entity->GetSimulationTime() - Entity->GetOldSimulationTime();
			int ChokedTicks = fine(TIME_TO_TICKS(SimtimeDelta), 1, 15);
			Vector LastOrigin;

			if (LastOrigin.Length() != Entity->GetOrigin().Length())
				LastOrigin = Entity->GetOrigin();

			float DeltaDistance = (Entity->GetOrigin() - LastOrigin).LengthSqr();
			if (DeltaDistance > 4096.f)
			{
				Vector VelocityPerTick = Entity->GetVelocity() *g_pGlobalVars->intervalPerTick;
				auto NewOrigin = Entity->GetOrigin() + (VelocityPerTick * ChokedTicks);
				Entity->SetAbsOrigin(NewOrigin);
			}
		}
	}
}

void Aimbot::FixTickCount(CUserCmd* CMD)
{
	auto InterpRatio = g_pCvar->FindVar("cl_interp_ratio");
	auto UpdateRate = g_pCvar->FindVar("cl_updaterate");
	int LerpTicks = TIME_TO_TICKS(InterpRatio->GetFloat() / UpdateRate->GetFloat());

	for (int i = 1; i <= g_pGlobalVars->maxClients; i++)
	{
		C_BaseEntity* Entity = g_pEntityList->GetClientEntity(i);

		if (!Entity)
			continue;

		if (Entity->GetHealth() <= 0 || !Entity->IsAlive())
			continue;

		if (Entity->GetOrigin() == Vector(0, 0, 0))
			continue;

		if (Entity->IsImmune())
			continue;

		if (Entity->IsDormant())
			continue;

		bool IsLocalPlayer = Entity == g::pLocalEntity;
		bool IsTeammate = g::pLocalEntity->GetTeam() == Entity->GetTeam() && !IsLocalPlayer;

		if (IsLocalPlayer)
			continue;

		if (IsTeammate)
			continue;

		CMD->tick_count = TIME_TO_TICKS(Entity->GetSimulationTime()) + LerpTicks;
	}
}

void Aimbot::RunRagebot(CUserCmd* CMD)
{
	Entities.clear();

	if (g_Menu.Config.EnableRagebot)
	{
		if (Keyhandler::AutoCheck(g_Menu.Config.RagebotKeyType, g_Menu.Config.RagebotKey))
		{
			SelectTarget();
			ShootEnemy(CMD);
		}
	}
}

void Aimbot::SelectTarget()
{
	if (!g::pLocalEntity)
		return;

	BestFov = g_Menu.Config.RagebotFov;
	BestDist = 8192.f;
	BestThreat = 0.f;
	BestTarget = nullptr;

	for (int i = 1; i <= g_pGlobalVars->maxClients; i++)
	{
		C_BaseEntity* Entity = g_pEntityList->GetClientEntity(i);

		if (!Entity)
			continue;

		if (Entity->GetHealth() <= 0 || !Entity->IsAlive())
			continue;

		if (Entity->GetOrigin() == Vector(0, 0, 0))
			continue;

		if (Entity->IsImmune())
			continue;

		if (Entity->IsDormant())
			continue;

		bool IsLocalPlayer = Entity == g::pLocalEntity;
		bool IsTeammate = g::pLocalEntity->GetTeam() == Entity->GetTeam() && !IsLocalPlayer;

		if (IsLocalPlayer)
			continue;

		if (IsTeammate)
			continue;

		if (g_Menu.Config.TargetMode == 0)
		{
			float Fov = g_Math.GetFov(g::pCmd->viewangles, g_Math.CalcAngle(g::pLocalEntity->GetEyePosition(), Entity->GetBonePos(HITBOX_PELVIS)));
			if (Fov < BestFov)
			{
				BestFov = Fov;
				BestTarget = Entity;
			}
		}

		if (g_Menu.Config.TargetMode == 1)
		{
			float Fov = g_Math.GetFov(g::pCmd->viewangles, g_Math.CalcAngle(g::pLocalEntity->GetEyePosition(), Entity->GetBonePos(HITBOX_PELVIS)));
			float Dist = g_Math.VectorDistance(g::pLocalEntity->GetOrigin(), Entity->GetOrigin());
			if (Dist < BestDist && Fov < BestFov)
			{
				BestDist = Dist;
				BestTarget = Entity;
			}
		}

		if (g_Menu.Config.TargetMode == 2)
		{
			float Fov = g_Math.GetFov(g::pCmd->viewangles, g_Math.CalcAngle(g::pLocalEntity->GetEyePosition(), Entity->GetBonePos(HITBOX_PELVIS)));
			float Dist = g_Math.VectorDistance(g::pLocalEntity->GetOrigin(), Entity->GetOrigin());
			float Health = (float)Entity->GetHealth();

			float Threat = Health / Dist;

			if (IsTargetingLocal(Entity))
				Threat += 100;

			if (Threat > BestThreat && Fov < BestFov)
			{
				BestThreat = Threat;
				BestTarget = Entity;
			}
		}



		AimbotData_t Data = AimbotData_t(BestTarget, i);

		Entities.push_back(Data);
	}
}

void Aimbot::ShootEnemy(CUserCmd* CMD)
{
	if (!g::pLocalEntity)
		return;

	if (g::PressedKeys[VK_LBUTTON])
		return;

	auto Weapon = reinterpret_cast<C_BaseCombatWeapon*>(g_pEntityList->GetClientEntity(g::pLocalEntity->GetActiveWeaponIndex()));

	if (!Weapon)
		return;

	if (Weapon->GetAmmo() == 0)
		return;

	if (g::pLocalEntity->GetHealth() <= 0)
		return;

	if (Weapon->IsKnifeorNade())
		return;

	if (!CanShoot(CMD))
	{
		CMD->buttons &= ~IN_ATTACK;
		return;
	}

	Vector AimAngles;

	for (auto Players : Entities)
	{
		auto Entity = Players.pPlayer;

		if (!Entity)
			continue;

		if (Entity->GetHealth() <= 0 || !Entity->IsAlive())
			continue;

		if (!Entity->IsAlive())
			continue;

		if (Entity->GetOrigin() == Vector(0, 0, 0))
			continue;

		if (Entity->IsImmune())
			continue;

		if (Entity->IsDormant())
			continue;

		bool IsLocalPlayer = Entity == g::pLocalEntity;
		bool IsTeammate = g::pLocalEntity->GetTeam() == Entity->GetTeam() && !IsLocalPlayer;

		if (IsLocalPlayer)
			continue;

		if (IsTeammate)
			continue;

		Vector LocalPosition = g::pLocalEntity->GetEyePosition();
		Vector ShootHere;

		switch (g_Menu.Config.HitboxesToScan)
		{
		case 0:
			ShootHere = HeadMultipoint(Entity);
			break;
		case 1:
			ShootHere = BodyMultipoint(Entity);
			break;
		case 2:
			ShootHere = FullMultipoint(Entity);
			break;
		}

		if (ShootHere == Vector(0, 0, 0))
			continue;

		if (Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP || Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 ||
			Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SCAR20 || Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_G3SG1 ||
			Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AUG || Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SG556) {

			if (g_Menu.Config.AutoScope) {
				if (!g::pLocalEntity->IsScoped())
					CMD->buttons |= IN_ATTACK2;
			}
		}

		bool Shouldstop = g_Menu.Config.StopBetweenShots ? true : CanShoot(CMD);

		if (g_Menu.Config.AutoStop)
		{
			if (g::pLocalEntity->GetFlags() & FL_ONGROUND)
			{
				if (Weapon->IsKnifeorNade() || Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_TASER)
					return;

				if (Shouldstop && g::pLocalEntity->GetVelocity().Length2D() > 2.5f)
				{
					Autostop();
				}
			}
		}

		AimAngles = g_Math.CalcAngle(LocalPosition, ShootHere);

		Vector VecPosition[65];
		Vector OriginDelta[65];
		if (Entity->GetOrigin() != VecPosition[Entity->EntIndex()])
		{
			OriginDelta[Entity->EntIndex()] = Entity->GetOrigin() - VecPosition[Entity->EntIndex()];
			VecPosition[Entity->EntIndex()] = Entity->GetOrigin();

			LagComp = fabs(OriginDelta[Entity->EntIndex()].Length()) > 64;
		}

		if (LagComp && Entity->GetVelocity().Length2D() > 300 && g_Menu.Config.DontShootLagComp)
			return;

		if (AcceptedInaccuracy(Weapon) < g_Menu.Config.Hitchance)
			continue;

		CMD->buttons |= IN_ATTACK;
		break;
	}

	if (CMD->buttons & IN_ATTACK)
	{
		g::bSendPacket = true;

		if (g_Menu.Config.NoAimRecoil)
			AimAngles = AimAngles - (g::pLocalEntity->GetAimPunchAngle() * g_pCvar->FindVar("weapon_recoil_scale")->GetFloat());

		if (g_Menu.Config.SilentAim)
			CMD->viewangles = AimAngles;
		else
			g_pEngine->SetViewAngles(AimAngles);

		g::ViewmodelAimbotAngle = AimAngles;
	}
}

float Aimbot::AcceptedInaccuracy(C_BaseCombatWeapon* Weapon)
{
	if (!g::pLocalEntity) 
		return 0;

	if (!Weapon) 
		return 0;

	if (Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_TASER) 
		return 0;

	float Inaccuracy = Weapon->GetInaccuracy();

	if (Inaccuracy == 0) 
		Inaccuracy = 0.0000001;

	Inaccuracy = 1 / Inaccuracy;

	return Inaccuracy;
}

float Aimbot::GetCurtime()
{
	if (!g::pLocalEntity)
		return -1;

	return static_cast<float>(g::pLocalEntity->GetTickBase()) * g_pGlobalVars->intervalPerTick;
}

bool Aimbot::CanShoot(CUserCmd* CMD)
{
	if (!g::pLocalEntity || g::pLocalEntity->GetHealth() <= 0)
	 	return false;

	auto Weapon = reinterpret_cast<C_BaseCombatWeapon*>(g_pEntityList->GetClientEntity(g::pLocalEntity->GetActiveWeaponIndex()));

	if (Weapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
	{
		if (fabs(Weapon->GetPostponeFireReadyTime() - GetCurtime()) <= 0.05f)
		{
			if (CMD->buttons & IN_ATTACK)
				return true;
		}
	}

	float ServerTime = g::pLocalEntity->GetTickBase() * g_pGlobalVars->intervalPerTick;

	return (Weapon->GetNextPrimaryAttack() <= ServerTime && Weapon->GetAmmo() > 0);
}

void Aimbot::Autostop()
{
	Vector Velocity = g::pLocalEntity->GetVelocity();
	static float Speed = Velocity.Length2D();

	Vector Direction;
	Vector RealView;
	g_Math.VectorAngles(Velocity, Direction);
	g_pEngine->GetViewAngles(RealView);
	Direction.y = RealView.y - Direction.y;

	Vector Forward;
	g_Math.AngleVectors(Direction, &Forward);
	Vector NegativeDirection = Forward * -Speed;

	g::pCmd->forwardmove = NegativeDirection.x;
	g::pCmd->sidemove = NegativeDirection.y;
}

void Aimbot::MinWalk(CUserCmd* CMD, float MaxSpeed)
{
	if (MaxSpeed <= 0.f)
		return;

	float MinSpeed = (float)(sqrt(square(CMD->forwardmove) + square(CMD->sidemove) + square(CMD->upmove)));

	if (MinSpeed <= 0.f)
		return;

	if (CMD->buttons & IN_DUCK)
		MaxSpeed *= 2.94117647f;

	if (MinSpeed <= MaxSpeed)
		return;

	float Ratio = MaxSpeed / MinSpeed;

	CMD->forwardmove *= Ratio;
	CMD->sidemove *= Ratio;
	CMD->upmove *= Ratio;
}

std::vector<Vector> Aimbot::GetMultiplePointsForHitbox(C_BaseEntity* Local, C_BaseEntity* Entity, int iHitbox, matrix3x4_t BoneMatrix[128])
{
	auto VectorTransformWrapper = [](const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		auto VectorTransform = [](const float* in1, const matrix3x4_t& in2, float* out)
		{
			auto DotProducts = [](const float* v1, const float* v2)
			{
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = DotProducts(in1, in2[0]) + in2[0][3];
			out[1] = DotProducts(in1, in2[1]) + in2[1][3];
			out[2] = DotProducts(in1, in2[2]) + in2[2][3];
		};
		VectorTransform(&in1.x, in2, &out.x);
	};

	studiohdr_t* StudioModel = g_pModelInfo->GetStudiomodel(Entity->GetModel());
	mstudiohitboxset_t* Set = StudioModel->GetHitboxSet(0);
	mstudiobbox_t* Hitbox = Set->GetHitbox(iHitbox);

	std::vector<Vector> VecArray;

	Vector Max;
	Vector Min;
	VectorTransformWrapper(Hitbox->max, BoneMatrix[Hitbox->bone], Max);
	VectorTransformWrapper(Hitbox->min, BoneMatrix[Hitbox->bone], Min);

	auto Center = (Min + Max) * 0.5f;

	Vector CurrentAngles = g_Math.CalcAngle(Center, g::pLocalEntity->GetEyePosition());

	Vector Forward;
	g_Math.AngleVectors(CurrentAngles, &Forward);

	Vector Right = Forward.Cross(Vector(0, 0, 1));
	Vector Left = Vector(-Right.x, -Right.y, Right.z);

	Vector Top = Vector(0, 0, 1);
	Vector Bot = Vector(0, 0, -1);

	switch (iHitbox) {
	case 0:
		for (auto i = 0; i < 4; ++i)
			VecArray.emplace_back(Center);

		VecArray[1] += Top * (Hitbox->radius * g_Menu.Config.Headscale / 100);
		VecArray[2] += Right * (Hitbox->radius * g_Menu.Config.Headscale / 100);
		VecArray[3] += Left * (Hitbox->radius * g_Menu.Config.Headscale / 100);
		break;

	default:

		for (auto i = 0; i < 3; ++i)
			VecArray.emplace_back(Center);

		VecArray[1] += Top * (Hitbox->radius * g_Menu.Config.Bodyscale / 100);
		VecArray[2] += Right * (Hitbox->radius * g_Menu.Config.Bodyscale / 100);
		break;
	}
	return VecArray;
}

Vector Aimbot::HeadMultipoint(C_BaseEntity* Entity)
{
	if (!g::pLocalEntity)
		return Vector(0, 0, 0);

	Vector LocalPosition = g::pLocalEntity->GetEyePosition();
	Vector VectorBestPoint = Vector(0, 0, 0);

	MostDamage = g_Menu.Config.MinDamage;

	matrix3x4_t Matrix[128];

	if (!Entity->SetupBones(Matrix, 128, 256, 0))
		return Vector(0, 0, 0);

	int Hitboxes[] =
	{
		HITBOX_HEAD
	};

	for (int i = 0; i < ARRAYSIZE(Hitboxes); i++)
	{
		for (Vector Point : GetMultiplePointsForHitbox(g::pLocalEntity, Entity, Hitboxes[i], Matrix))
		{
			C_Trace Trace;
			int Damage = g_Autowall.Damage(Point);

			if (Damage > MostDamage)
			{
				BestHitbox = Hitboxes[i];
				MostDamage = Damage;
				VectorBestPoint = Point;

				if (MostDamage >= Entity->GetHealth())
					return VectorBestPoint;
			}

		}
	}

	return VectorBestPoint;
}

Vector Aimbot::BodyMultipoint(C_BaseEntity* Entity)
{
	if (!g::pLocalEntity)
		return Vector(0, 0, 0);

	Vector LocalPosition = g::pLocalEntity->GetEyePosition();
	Vector VectorBestPoint = Vector(0, 0, 0);

	MostDamage = g_Menu.Config.MinDamage;

	matrix3x4_t Matrix[128];

	if (!Entity->SetupBones(Matrix, 128, 256, 0))
		return Vector(0, 0, 0);

	int Hitboxes[] =
	{
		HITBOX_PELVIS,
	};

	for (int i = 0; i < ARRAYSIZE(Hitboxes); i++)
	{
		for (Vector Point : GetMultiplePointsForHitbox(g::pLocalEntity, Entity, Hitboxes[i], Matrix))
		{
			C_Trace Trace;
			int Damage = g_Autowall.Damage(Point);

			if (Damage > MostDamage)
			{
				BestHitbox = Hitboxes[i];
				MostDamage = Damage;
				VectorBestPoint = Point;

				if (MostDamage >= Entity->GetHealth())
					return VectorBestPoint;
			}

		}
	}

	return VectorBestPoint;
}

Vector Aimbot::FullMultipoint(C_BaseEntity* Entity)
{
	if (!g::pLocalEntity)
		return Vector(0, 0, 0);

	Vector LocalPosition = g::pLocalEntity->GetEyePosition();
	Vector VectorBestPoint = Vector(0, 0, 0);

	MostDamage = g_Menu.Config.MinDamage;

	matrix3x4_t Matrix[128];

	if (!Entity->SetupBones(Matrix, 128, 256, 0))
		return Vector(0, 0, 0);

	int Hitboxes[] =
	{
		HITBOX_HEAD,
		HITBOX_BELLY,
		HITBOX_PELVIS,
		HITBOX_LEFT_UPPER_ARM,
		HITBOX_RIGHT_UPPER_ARM,
		HITBOX_UPPER_CHEST,
		HITBOX_LOWER_CHEST,
		HITBOX_LEFT_CALF,
		HITBOX_RIGHT_CALF,
		HITBOX_LEFT_THIGH,
		HITBOX_RIGHT_THIGH
	};

	for (int i = 0; i < ARRAYSIZE(Hitboxes); i++)
	{
		for (Vector Point : GetMultiplePointsForHitbox(g::pLocalEntity, Entity, Hitboxes[i], Matrix))
		{
			C_Trace Trace;
			int Damage = g_Autowall.Damage(Point);

			if (Damage > MostDamage)
			{
				BestHitbox = Hitboxes[i];
				MostDamage = Damage;
				VectorBestPoint = Point;

				if (MostDamage >= Entity->GetHealth())
					return VectorBestPoint;
			}

		}
	}

	return VectorBestPoint;
}

void Aimbot::InitDoubleTap(CUserCmd* CMD)
{
	if (g_Aimbot.DoubleTapStruct.SkipTicks > 0 && !g_Aimbot.DoubleTapStruct.DTCharged)
	{
		g_Aimbot.DoubleTapStruct.SkipTicks--;
		if (g_Aimbot.DoubleTapStruct.SkipTicks == 0) {
			g_Aimbot.DoubleTapStruct.DTCharged = true;
		}
		CMD->tick_count = INT_MAX;
		g::bSendPacket = true;
	}
	g_Aimbot.DoubleTapStruct.UnPredTick = g_pGlobalVars->tickcount;

	g_Aimbot.DoubleTapStruct.OverrideVelMod = true;

	if (g_Aimbot.DoubleTapStruct.VelMod < 1.f)
		*(bool*)((uintptr_t)g_pPrediction + 0x24) = true;
}

void Aimbot::DoubleTap(CUserCmd* CMD)
{
	if (!g_Menu.Config.DoubleTap)
		return;

	if (!g::pLocalEntity->IsAlive())
		return;

	C_BaseCombatWeapon* LocalWeapon = g::pLocalEntity->GetActiveWeapon();

	if (!LocalWeapon)
		return;

	if (Keyhandler::AutoCheck(g_Menu.Config.DoubleTapKeyType, g_Menu.Config.DoubleTapKey))
	{
		g::bSendPacket = true;
		{
			static int nNum = 0;
			static bool bShotDT = false;

			if ((CMD->buttons & IN_ATTACK) && LocalWeapon->IsGun())
			{
				g::TickBaseShift = 16;
				DoubleTapStruct.DidShot = true;
				DoubleTapStruct.CMDTick = CMD->command_number;
				DoubleTapStruct.BaseTick = g::pLocalEntity->GetTickBase();
				nNum = 0;
				bShotDT = true;
			}
			else if (bShotDT || DoubleTapStruct.ForceRecharge)
			{
				bool(CMD->buttons & IN_ATTACK) ? nNum = 0 : nNum++;
				bool bCooldownDone = nNum == TIME_TO_TICKS(0.75f);

				if ((DoubleTapStruct.DidShot && bCooldownDone) || DoubleTapStruct.ForceRecharge)
				{
					DoubleTapStruct.SkipTicks = g_Menu.Config.DoubleTapInstant ? 16 : 8;
					DoubleTapStruct.DTCharged = false;
					nNum = 0;
					bShotDT = false;
					DoubleTapStruct.DidShot = false;
					DoubleTapStruct.ForceRecharge = false;
				}
			}
		}
		DoubleTapStruct.NextShotDT = DoubleTapStruct.DTCharged;
	}
	else
	{
		DoubleTapStruct.ForceRecharge = true;
		DoubleTapStruct.ShootedFirstBullet = false;
		DoubleTapStruct.TickShifted = false;
		DoubleTapStruct.SinceUse = 0;
	}
}