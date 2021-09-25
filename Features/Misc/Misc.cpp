#include "Misc.h"
#include <iostream>
#include <algorithm>
#include "../Aimbot/Autowall.h"
#include "../Aimbot/Aimbot.h"
#include "../Aimbot/LagComp.h"
#include "../..\Utils/GlobalVars.h"
#include "../..\Utils/Math.h"
#include "../..\SDK/ICvar.h"
#include "../..\SDK/CPrediction.h"
#include "../../Security/Xor.h"
#include "../../Menu/Menu.h"
#include "../../SDK/Definitions.h"

Misc g_Misc;

float GetCorrectCameraDistance(CViewSetup* Setup, int Distance)
{
	const auto Factor = std::clamp(Distance, 25, 500);
	auto InverseAngles = Setup->angles;

	InverseAngles.x *= -1.f;
	InverseAngles.y += 180.f;

	Vector Forward;
	g_Math.AngleVectors(InverseAngles, &Forward);

	CTraceWorldOnly Filter(g::pLocalEntity);
	C_Trace Trace;

	g_pTrace->TraceRay(C_Ray(g::pLocalEntity->GetEyePosition(), g::pLocalEntity->GetEyePosition() + Forward * Factor), mask_all, &Filter, &Trace);

	return Factor * Trace.flFraction - 10.f;
}

void Misc::Thirdperson(CViewSetup* Setup)
{
	if (!g::pLocalEntity)
		return;

	if (!g_Menu.Config.Thirdperson)
		return;

	if (Keyhandler::AutoCheck(g_Menu.Config.ThirdpersonKeyType, g_Menu.Config.ThirdpersonKey))
	{
		if (g::pLocalEntity->IsAlive())
		{
			if (!g_pInput->m_fCameraInThirdPerson)
				g_pInput->m_fCameraInThirdPerson = true;

			g_pInput->m_vecCameraOffset.z = GetCorrectCameraDistance(Setup, g_Menu.Config.ThirdpersonDist);

			if (WasDeadInThirdperson)
				WasDeadInThirdperson = false;
		}
		else
		{
			if (g_pInput->m_fCameraInThirdPerson)
				g_pInput->m_fCameraInThirdPerson = false;

			if (!WasDeadInThirdperson)
			{
				g::pLocalEntity->SetObserverMode(5);
				WasDeadInThirdperson = true;
			}
		}
	}
	else
	{
		if (g_pInput->m_fCameraInThirdPerson)
			g_pInput->m_fCameraInThirdPerson = false;

		if (WasDeadInThirdperson)
		{
			g::pLocalEntity->SetObserverMode(4);
			WasDeadInThirdperson = false;
		}

		auto CurrentMode = g::pLocalEntity->GetObserverMode();
		if (OldState != CurrentMode)
			OldState = CurrentMode;
	}
}

void Misc::Bhop()
{
	if (!g::pLocalEntity->IsAlive())
		return;

	if (g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_LADDER
		|| g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_NOCLIP
		|| g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_OBSERVER)
		return;

	if (!g_Menu.Config.Bhop)
		return;

	if (g_Menu.Config.EnableLegitBhop)
	{
		static int HopsRestricted = 0;
		static int HopsHit = 0;

		if (!(g::pLocalEntity->GetFlags() & FL_ONGROUND))
		{
			g::pCmd->buttons &= ~IN_JUMP;
			HopsRestricted = 0;
		}
		else if ((rand() % 100 > g_Menu.Config.BhopHitchance && HopsRestricted < g_Menu.Config.BhopsRestricted) || (g_Menu.Config.MaxBhops > 0 && HopsHit > g_Menu.Config.MaxBhops))
		{
			g::pCmd->buttons &= ~IN_JUMP;
			HopsRestricted++;
			HopsHit = 0;
		}
		else
		{
			HopsHit++;
		}
	}
	else
	{
		static bool LastJumped = false;
		static bool ShouldFake = false;

		if (!LastJumped && ShouldFake)
		{
			ShouldFake = false;
			g::pCmd->buttons |= IN_JUMP;
		}
		else if (g::pCmd->buttons & IN_JUMP)
		{
			if (g::pLocalEntity->GetFlags() & FL_ONGROUND)
			{
				ShouldFake = LastJumped = true;
			}
			else
			{
				g::pCmd->buttons &= ~IN_JUMP;
				LastJumped = false;
			}
		}
		else
		{
			ShouldFake = LastJumped = false;
		}
	}
}

void Misc::StaticStrafer(Vector& Original)
{
	if (g::pCmd->sidemove != 0.0f || g::pCmd->forwardmove != 0.0f)
		return;

	const float YVelocity = RedianToDegrees(atan2(g::pLocalEntity->GetVelocity().y, g::pLocalEntity->GetVelocity().x));
	const float DifferentAngle = NormalizeAng(Original.y - YVelocity);
	static float SideSpeed = g_pCvar->FindVar(XorStr("cl_sidespeed"))->GetFloat();

	if (g::pCmd->buttons & IN_BACK) {
		g::pCmd->buttons &= ~IN_BACK;
	}
	else if (g::pCmd->buttons & IN_FORWARD) {
		g::pCmd->buttons &= ~IN_FORWARD;
	}
	else if (g::pCmd->buttons & IN_MOVELEFT) {
		g::pCmd->buttons &= ~IN_MOVELEFT;
	}
	else if (g::pCmd->buttons & IN_MOVERIGHT) {
		g::pCmd->buttons &= ~IN_MOVERIGHT;
	}

	g::pCmd->sidemove = (DifferentAngle > 0.0) ? -SideSpeed : SideSpeed;
	Original.y = NormalizeAng(Original.y - DifferentAngle);
}

void Misc::MultiDirStrafer(Vector& Original)
{
	if (g::pCmd->buttons & IN_SPEED && g::pLocalEntity->GetVelocity().Length2D() < 1.f)
		return;

	static float YawAdd = 0.f;
	static const auto SideSpeed = g_pCvar->FindVar(XorStr("cl_sidespeed"));

	bool Back = g::pCmd->buttons & IN_BACK;
	bool Forward = g::pCmd->buttons & IN_FORWARD;
	bool Right = g::pCmd->buttons & IN_MOVELEFT;
	bool Left = g::pCmd->buttons & IN_MOVERIGHT;

	if (Back)
	{
		YawAdd = -180.f;
		if (Right)
			YawAdd -= 45.f;
		else if (Left)
			YawAdd += 45.f;
	}
	else if (Right) 
	{
		YawAdd = 90.f;
		if (Back)
			YawAdd += 45.f;
		else if (Forward)
			YawAdd -= 45.f;
	}
	else if (Left) 
	{
		YawAdd = -90.f;
		if (Back)
			YawAdd -= 45.f;
		else if (Forward)
			YawAdd += 45.f;
	}
	else
	{
		YawAdd = 0.f;
	}

	Original.y += YawAdd;
	g::pCmd->forwardmove = 0.f;
	g::pCmd->sidemove = 0.f;

	const auto Delta = NormalizeAng(Original.y - RedianToDegrees(atan2(g::pLocalEntity->GetVelocity().y, g::pLocalEntity->GetVelocity().x)));

	g::pCmd->sidemove = Delta > 0.f ? -SideSpeed->GetFloat() : SideSpeed->GetFloat();

	Original.y = NormalizeAng(Original.y - Delta);
}

void Misc::AutoStrafe(Vector& Original)
{
	if (!g::pLocalEntity->IsAlive())
		return;

	if (g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_LADDER
		|| g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_NOCLIP
		|| g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_OBSERVER)
		return;

	if (g::pLocalEntity->GetFlags() & FL_ONGROUND || g::pLocalEntity->GetFlags() == FL_WATERJUMP)
		return;

	if (g_Menu.Config.AutoStrafe)
	{
		switch (g_Menu.Config.AutoStrafeType)
		{
		case 0:
			StaticStrafer(Original);
			break;
		case 1:
			MultiDirStrafer(Original);
			break;
		}
	}
}

void Misc::MovementFix(Vector& Original)
{
	if (!g::pLocalEntity->IsAlive())
		return;

	if (g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_LADDER
		|| g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_NOCLIP
		|| g::pLocalEntity->GetMoveType() == MoveType_t::MOVETYPE_OBSERVER)
		return;

	Vector RealViewangles;
	g_pEngine->GetViewAngles(RealViewangles);

	Vector VectorMove(g::pCmd->forwardmove, g::pCmd->sidemove, g::pCmd->upmove);
	float Speed = sqrt(VectorMove.x * VectorMove.x + VectorMove.y * VectorMove.y);

	Vector AngleMove;
	g_Math.VectorAngles(VectorMove, AngleMove);

	float Yaw = DEG2RAD(g::pCmd->viewangles.y - Original.y + AngleMove.y);

	g::pCmd->forwardmove = cos(Yaw) * Speed;
	g::pCmd->sidemove = sin(Yaw) * Speed;

	g::pCmd->buttons &= ~IN_RIGHT;
	g::pCmd->buttons &= ~IN_MOVERIGHT;
	g::pCmd->buttons &= ~IN_LEFT;
	g::pCmd->buttons &= ~IN_MOVELEFT;
	g::pCmd->buttons &= ~IN_FORWARD;
	g::pCmd->buttons &= ~IN_BACK;

	if (g::pCmd->forwardmove > 0.f)
		g::pCmd->buttons |= IN_FORWARD;
	else if (g::pCmd->forwardmove < 0.f)
		g::pCmd->buttons |= IN_BACK;

	if (g::pCmd->sidemove > 0.f)
	{
		g::pCmd->buttons |= IN_RIGHT;
		g::pCmd->buttons |= IN_MOVERIGHT;
	}
	else if (g::pCmd->sidemove < 0.f)
	{
		g::pCmd->buttons |= IN_LEFT;
		g::pCmd->buttons |= IN_MOVELEFT;
	}
}

void Misc::EdgeJump(CUserCmd* CMD)
{
	if (!g_Menu.Config.EdgeJump)
		return;

	if (!g_pTrace)
		return;

	if (!Keyhandler::AutoCheck(g_Menu.Config.EdgeJumpKeyType, g_Menu.Config.EdgeJumpKey))
		return;

	Vector Start, End;
	Start = g::pLocalEntity->GetOrigin();

	VectorCopy(g::pLocalEntity->GetOrigin(), Start);
	VectorCopy(Start, End);

	End.z -= 2;

	C_Trace Trace;
	C_TraceFilter Filter(g::pLocalEntity);
	Filter.pSkip1 = g::pLocalEntity;

	g_pTrace->TraceRay(C_Ray(Start, End), mask_playersolid_brushonly, &Filter, &Trace);

	if (Trace.flFraction == 1.0f)
	{
		CMD->buttons |= IN_JUMP;
	}
}

void Misc::RemoveShadows()
{
	ConVar* PropShadows = g_pCvar->FindVar(XorStr("cl_csm_static_prop_shadows"));
	ConVar* FootShadows = g_pCvar->FindVar(XorStr("cl_foot_contact_shadows"));
	ConVar* Shadows = g_pCvar->FindVar(XorStr("cl_csm_shadows"));

	if (!PropShadows && !FootShadows && !Shadows)
		return;

	*(int*)((DWORD)&Shadows->fnChangeCallback + 0xC) = 0;
	*(int*)((DWORD)&FootShadows->fnChangeCallback + 0xC) = 0;
	*(int*)((DWORD)&PropShadows->fnChangeCallback + 0xC) = 0;

	if (g_Menu.Config.RemoveShadows)
	{
		Shadows->SetValue(0);
		FootShadows->SetValue(0);
		PropShadows->SetValue(0);
	}
	else
	{
		Shadows->SetValue(1);
		FootShadows->SetValue(1);
		PropShadows->SetValue(1);
	}
}

void Misc::Clantag()
{
	if (!(g::pCmd->command_number % 16 == 0))
		return;

	if (g_Menu.Config.Clantag)
	{
		switch (int(g_pGlobalVars->curtime * 2.2) % 30)
		{
		case 0: Utils::ChangeTag("s", "soubcheats.net"); break;
		case 1: Utils::ChangeTag("so", "soubcheats.net"); break;
		case 2: Utils::ChangeTag("sou", "soubcheats.net"); break;
		case 3: Utils::ChangeTag("soub", "soubcheats.net"); break;
		case 4: Utils::ChangeTag("soubc", "soubcheats.net"); break;
		case 5: Utils::ChangeTag("soubch", "soubcheats.net"); break;
		case 6: Utils::ChangeTag("soubche", "soubcheats.net"); break;
		case 7: Utils::ChangeTag("soubchea", "soubcheats.net"); break;
		case 8: Utils::ChangeTag("soubcheat", "soubcheats.net"); break;
		case 9: Utils::ChangeTag("soubcheats", "soubcheats.net"); break;
		case 10: Utils::ChangeTag("soubcheats.", "soubcheats.net"); break;
		case 11: Utils::ChangeTag("soubcheats.n", "soubcheats.net"); break;
		case 12: Utils::ChangeTag("soubcheats.ne", "soubcheats.net"); break;
		case 13: Utils::ChangeTag("soubcheats.net", "soubcheats.net"); break;
		case 14: Utils::ChangeTag("soubcheats.net", "soubcheats.net"); break;
		case 15: Utils::ChangeTag("soubcheats.ne\\", "soubcheats.net"); break;
		case 16: Utils::ChangeTag("soubcheats.n/", "soubcheats.net"); break;
		case 17: Utils::ChangeTag("soubcheats.\\", "soubcheats.net"); break;
		case 18: Utils::ChangeTag("soubcheats/", "soubcheats.net"); break;
		case 19: Utils::ChangeTag("soubcheat\\", "soubcheats.net"); break;
		case 20: Utils::ChangeTag("soubchea/", "soubcheats.net"); break;
		case 21: Utils::ChangeTag("soubche\\", "soubcheats.net"); break;
		case 22: Utils::ChangeTag("soubch/", "soubcheats.net"); break;
		case 23: Utils::ChangeTag("soubc\\", "soubcheats.net"); break;
		case 24: Utils::ChangeTag("soub/", "soubcheats.net"); break;
		case 25: Utils::ChangeTag("sou\\", "soubcheats.net"); break;
		case 26: Utils::ChangeTag("so/", "soubcheats.net"); break;
		case 27: Utils::ChangeTag("s\\", "soubcheats.net"); break;
		case 28: Utils::ChangeTag("/", "soubcheats.net"); break;
		case 29: Utils::ChangeTag(" ", "soubcheats.net"); break;
		}
	}
	else
	{
		Utils::ChangeTag(" ", " ");
	}
}

void Misc::DisablePostProcessing()
{
	ConVar* PostProcessing = g_pCvar->FindVar(XorStr("mat_postprocess_enable"));

	if (!PostProcessing)
		return;

	*(int*)((DWORD)&PostProcessing->fnChangeCallback + 0xC) = 0;

	if (g_Menu.Config.DisablePostEffects)
	{
		PostProcessing->SetValue(0);
	}
	else
	{
		PostProcessing->SetValue(1);
	}
}

void Misc::RevealHiddenCvars()
{
	if (!g_Menu.Config.RevealHiddenCvars)
		return;

	if (!g_pCvar)
		return;

	auto Base = **reinterpret_cast<ConCommandBase***>(g_pCvar + 0x34);
	for (auto c = Base->m_pNext; c != nullptr; c = c->m_pNext)
	{
		ConCommandBase* CMDBase = c;
		CMDBase->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
		CMDBase->m_nFlags &= ~FCVAR_HIDDEN;
	}
}

void Misc::ModifyViewModel()
{
	if (!g_pCvar)
		return;

	ConVar* ViewmodelOffsetX = g_pCvar->FindVar(XorStr("viewmodel_offset_x"));
	ConVar* ViewmodelOffsetY = g_pCvar->FindVar(XorStr("viewmodel_offset_y"));
	ConVar* ViewmodelOffsetZ = g_pCvar->FindVar(XorStr("viewmodel_offset_z"));

	if (!ViewmodelOffsetX || !ViewmodelOffsetY || !ViewmodelOffsetZ)
		return;

	*(int*)((DWORD)&ViewmodelOffsetX->fnChangeCallback + 0xC) = 0;
	*(int*)((DWORD)&ViewmodelOffsetY->fnChangeCallback + 0xC) = 0;
	*(int*)((DWORD)&ViewmodelOffsetZ->fnChangeCallback + 0xC) = 0;

	if (g_Menu.Config.AllowViewModify)
	{
		ViewmodelOffsetX->SetValue(g_Menu.Config.ViewOffsetX / 2);
		ViewmodelOffsetY->SetValue(g_Menu.Config.ViewOffsetY / 2);
		ViewmodelOffsetZ->SetValue(g_Menu.Config.ViewOffsetZ / 2);
	}
	else 
	{
		g_pCvar->FindVar("viewmodel_offset_x")->SetValue(0);
		g_pCvar->FindVar("viewmodel_offset_y")->SetValue(0);
		g_pCvar->FindVar("viewmodel_offset_z")->SetValue(0);
	}
}

void Misc::ReturnServerType()
{
	INetChannelInfo* NetChannel = g_pEngine->GetNetChannelInfo();

	if (!NetChannel)
		return;

	std::string ServerAddress;
	if (g_pEngine->IsInGame()) 
	{
		ServerAddress = std::string(NetChannel->GetAddress());

		if (ServerAddress == XorStr("loopback"))
		{
			ServerAddress = XorStr("Local Server");
			g::LocalServer = true;
			g::NotConnectedToServer = false;
		}
		else
			g::LocalServer = false;
		if (ServerAddress.find(XorStr("=[")) != std::string::npos) 
		{
			ServerAddress = XorStr("Valve Server");
			g::ValveServer = true;
			g::NotConnectedToServer = false;
		}
		else 
			g::ValveServer = false;

		if (!(ServerAddress.find(XorStr("=[")) != std::string::npos || ServerAddress != XorStr("loopback")))
		{
			g::CommunityServer = true;
			g::NotConnectedToServer = false;
		}
		else
			g::CommunityServer = false;
	}
	else 
	{
		ServerAddress = XorStr("Not Connected");
		g::NotConnectedToServer = true;
	}
}

void Misc::DisableInterp()
{
	for (int i = 1; i < g_pGlobalVars->maxClients; i++)
	{
		C_BaseEntity* Entity = g_pEntityList->GetClientEntity(i);

		if (!Entity)
			continue;

		if (!Entity->IsAlive())
			continue;

		if (Entity == g::pLocalEntity)
			continue;
		
		VarMapping_t* Map = Entity->GetVarMap();

		if (!Map)
			continue;

		for (int i = 0; i < Map->m_nInterpolatedEntries; i++)
		{
			VarMapEntry_t* e = &Map->m_Entries[i];
			e->m_bNeedsToInterpolate = false;
		}
	}
}

void Misc::FixAnimLOD()
{
	if (!g::pLocalEntity)
		return;

	for (int i = 1; i <= g_pGlobalVars->maxClients; i++) 
	{
		C_BaseEntity* Entity = g_pEntityList->GetClientEntity(i);

		if (!Entity || Entity->IsDormant() || !Entity->IsAlive())
			continue;

		if (Entity == g::pLocalEntity)
			continue;

		*reinterpret_cast<int*>(Entity + 0xA28) = 0;
		*reinterpret_cast<int*>(Entity + 0xA30) = g_pGlobalVars->framecount;
	}
}