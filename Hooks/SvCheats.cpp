#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

bool __fastcall Hooks::SvCheats(PVOID ConVar, void* EDX)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached SvCheats.")); Init = false; }

	static auto oSvCheats = g_Hooks.pSvCheatsHook->GetOriginal<SvCheats_t>(HookIndexes::SvCheats);

	if (!oSvCheats)
		return false;

	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return oSvCheats(ConVar, EDX);

	static auto CAM_THINK = Utils::FindSignature(XorStr("client.dll"), XorStr("85 C0 75 30 38 86"));

	if (reinterpret_cast<DWORD*>(_ReturnAddress()) == reinterpret_cast<DWORD*>(CAM_THINK))
		return true;

	return oSvCheats(ConVar, EDX);
}