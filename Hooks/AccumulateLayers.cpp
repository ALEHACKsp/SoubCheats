#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

void __fastcall Hooks::AccumulateLayers(C_BaseEntity* Entity, void* ECX, void** BoneSetup, Vector* Position, float Time, Quaternion* Quat)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached AccumulateLayers.")); Init = false; }

	static auto oAccumulateLayers = g_Hooks.pPlayerHook->GetOriginal<AccumulateLayers_t>(HookIndexes::AccumulateLayers);

	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return oAccumulateLayers(Entity, ECX, BoneSetup, Position, Time, Quat);

	if (!Entity)
		oAccumulateLayers(Entity, ECX, BoneSetup, Position, Time, Quat);

	*reinterpret_cast<BYTE*>(Entity + 2600) &= ~0xA;
	oAccumulateLayers(Entity, ECX, BoneSetup, Position, Time, Quat);
	*reinterpret_cast<BYTE*>(Entity + 2600) |= 0xA;
}