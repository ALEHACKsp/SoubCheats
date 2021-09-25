#include "AntiAim.h"
#include "../Aimbot/Aimbot.h"
#include "..\Aimbot\Autowall.h"
#include "..\..\Utils\Utils.h"
#include "../../SDK/ClientState.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\Utils\Math.h"
#include "..\..\Menu\Menu.h"

AntiAim g_AntiAim;

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / g_pGlobalVars->intervalPerTick ) )
#define TICKS_TO_TIME( t )		( g_pGlobalVars->intervalPerTick *( t ) )

void AntiAim::FakeLag()
{

}

float OldLBY;
float LBYBreakerTimer;
float LastLBYUpdateTime;
bool bSwitch;

float GetLatency()
{
	INetChannelInfo* nci = g_pEngine->GetNetChannelInfo();
	if (nci) {
		float Latency = nci->GetAvgLatency(FLOW_OUTGOING) + nci->GetAvgLatency(FLOW_INCOMING);
		return Latency;
	}
	else {
		return 0.0f;
	}
}

float GetOutgoingLatency()
{
	INetChannelInfo* nci = g_pEngine->GetNetChannelInfo();
	if (nci) {
		float OutgoingLatency = nci->GetAvgLatency(FLOW_OUTGOING);
		return OutgoingLatency;
	}
	else {
		return 0.0f;
	}
}

float GetIncomingLatency()
{
	INetChannelInfo* nci = g_pEngine->GetNetChannelInfo();
	if (nci) {
		float IncomingLatency = nci->GetAvgLatency(FLOW_INCOMING);
		return IncomingLatency;
	}
	else {
		return 0.0f;
	}
}

bool NextLBYUpdate()
{
	float flServerTime = (float)(g::pLocalEntity->GetTickBase() * g_pGlobalVars->intervalPerTick);


	if (OldLBY != g::pLocalEntity->GetLowerBodyYaw())
	{
		LBYBreakerTimer++;
		OldLBY = g::pLocalEntity->GetLowerBodyYaw();
		bSwitch = !bSwitch;
		LastLBYUpdateTime = flServerTime;
	}

	if (g::pLocalEntity->GetVelocity().Length2D() > 0.5)
	{
		LastLBYUpdateTime = flServerTime;
		return false;
	}

	if ((LastLBYUpdateTime + 1 - (GetLatency() * 2) < flServerTime) && (g::pLocalEntity->GetFlags() & FL_ONGROUND))
	{
		if (LastLBYUpdateTime + 1.1 - (GetLatency() * 2) < flServerTime)
		{
			LastLBYUpdateTime += 1.1;
		}
		return true;
	}
	return false;
}

float AntiAim::GetMaxDesyncDelta()
{
	auto animstate = uintptr_t(g::pLocalEntity->AnimState());

	float rate = 180;
	float duckammount = *(float*)(animstate + 0xA4);
	float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));

	float speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));

	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
	float unk2 = unk1 + 1.1f;
	float unk3;

	if (duckammount > 0) {

		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));

	}

	unk3 = *(float*)(animstate + 0x334) * unk2;

	return unk3;
}

bool AntiAim::Disable(CUserCmd* CMD)
{
	if (!g_Menu.Config.EnableAntiAim)
		return true;

	if (!g::pLocalEntity || !g::pLocalEntity->IsAlive())
		return true;

	if (g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_LADDER
		|| g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_NOCLIP
		|| g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_OBSERVER)
		return true;

	if (CMD->buttons & IN_USE)
		return true;

	float ServerTime = g::pLocalEntity->GetTickBase() * g_pGlobalVars->intervalPerTick;

	C_BaseCombatWeapon* CBaseWeapon = reinterpret_cast<C_BaseCombatWeapon*>(g_pEntityList->GetClientEntity(g::pLocalEntity->GetActiveWeaponIndex()));
	C_BaseCombatWeapon* Weapon = g::pLocalEntity->GetActiveWeapon();

	if (CBaseWeapon->IsGun() && g_Aimbot.CanShoot(CMD) && (CMD->buttons & IN_ATTACK || (CBaseWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER && CMD->buttons & IN_ATTACK2)))
	{
		return true;
	}
	else if (Weapon->GetCSWpnData()->type == WEAPONTYPE_KNIFE)
	{
		if ((CMD->buttons & IN_ATTACK && Weapon->GetNextPrimaryAttack() <= ServerTime) ||
			(CMD->buttons & IN_ATTACK2 && Weapon->GetNextSecondaryAttack() <= ServerTime))
			return true;
	}
	else if (auto pGrenade = reinterpret_cast<CBaseCSGrenade*>(Weapon); pGrenade != nullptr && Weapon->GetCSWpnData()->type == WEAPONTYPE_GRENADE)
	{
		if (!pGrenade->PinPulled() || CMD->buttons & (IN_ATTACK | IN_ATTACK2))
		{
			if (pGrenade->ThrowTime() > 0.f)
				return true;
		}
	}

	return false;
}

void AntiAim::RunAntiAim(CUserCmd* CMD)
{
	if (Disable(CMD))
		return;

	CMD->viewangles.x += 89.9f;

	static bool flip = false;

	switch (rand() % 2)
	{
	case 0: flip = false; break;
	case 1: flip = true; break;
	}

	if (g::bSendPacket || CMD->command_number % 3) 
	{
		auto yaw = /*flip ? +GetMaxDesyncDelta() : -*/GetMaxDesyncDelta() - 30;
		CMD->viewangles.y -= 179.95f + yaw;
	}
	else
	{
		if (NextLBYUpdate())
			CMD->viewangles.y += 179.95f;
		else
			CMD->viewangles.y -= 179.95f;
	}
}