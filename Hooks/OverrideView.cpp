#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

void __fastcall Hooks::OverrideView(void* ecx, void* edx, CViewSetup* pSetup)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached OverrideView.")); Init = false; }

	static auto oOverrideView = g_Hooks.pClientModeHook->GetOriginal<OverrideView_t>(HookIndexes::OverRideView);

	if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
	{
		g_Misc.Thirdperson(pSetup);

		if (!g::pLocalEntity)
			return;

		if (!g::pLocalEntity->IsAlive())
			return;

		if (g_Menu.Config.RemoveAimPunch && g_Menu.Config.EnableRemovals)
		{
			Vector viewPunch = g::pLocalEntity->GetViewPunchAngle();
			Vector aimPunch = g::pLocalEntity->GetAimPunchAngle();

			pSetup->angles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
			pSetup->angles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
			pSetup->angles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		}

		if (g_Menu.Config.DebugFov != 0 && !g::pLocalEntity->IsScoped())
			pSetup->fov = g_Menu.Config.DebugFov;

		if (g_Menu.Config.RemoveZoom && g_Menu.Config.EnableRemovals && g::pLocalEntity->IsScoped())
			pSetup->fov = (g_Menu.Config.DebugFov == 0) ? 90 : g_Menu.Config.DebugFov;
	}

	oOverrideView(ecx, edx, pSetup);
}