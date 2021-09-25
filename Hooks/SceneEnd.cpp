#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

void __fastcall Hooks::SceneEnd(void* ecx, void* edx)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached SceneEnd.")); Init = false; }

	static auto oSceneEnd = g_Hooks.pRenderViewHook->GetOriginal<SceneEnd_t>(HookIndexes::SceneEnd);
	oSceneEnd(ecx, edx);

	if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
	{
		g_ESP.DrawChams();
		g_ESP.DrawGlow();
		g_ESP.RemoveSmoke();
	}
}