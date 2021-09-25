#include "Features.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../SDK/ClientState.h"

Features g_Features;

void Features::CreateMoveFeatures(CUserCmd* CMD)
{
	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return;

	if (!g::pLocalEntity)
		return;

	if (!CMD)
		return;

	if (g_Menu.Config.FastDuck)
	{
		g::pCmd->buttons |= IN_BULLRUSH;
	}

	g_Misc.Clantag();

	g_Misc.RevealHiddenCvars();

	g_Misc.DisablePostProcessing();

	g_Misc.ModifyViewModel();

	g_Misc.RemoveShadows();

	g_Misc.Bhop();

	g_Misc.AutoStrafe(g::OriginalView);

	g_Misc.EdgeJump(CMD);

	g_AntiAim.FakeLag();

	g_Aimbot.InitDoubleTap(CMD);

	g_EnginePrediction.RunEnginePred(CMD);

	g_Aimbot.RunRagebot(CMD);

	g_Aimbot.DoubleTapStruct.OverrideVelMod = false;
	g_Aimbot.DoubleTap(CMD);

	g_EnginePrediction.EndEnginePred(CMD);

	g_AntiAim.RunAntiAim(CMD);

	g_Aimbot.FixTickCount(CMD);

	g_Misc.MovementFix(g::OriginalView);

	if (g_Menu.Config.AntiUntrusted)
	{
		g_Math.Clamp(g::pCmd->viewangles);
	}

	if (g::bSendPacket)
		g::FakeAngle = CMD->viewangles;
	else 
		g::RealAngle = CMD->viewangles;

	g_AnimFix.FakeLocalEntity();
}