#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

bool __fastcall Hooks::IsHltv(uintptr_t ECX, uintptr_t EDX)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached IsHltv.")); Init = false; }

	auto oIsHltv = g_Hooks.pEngineHook->GetOriginal<IsHltv_t>(HookIndexes::IsHltv);

	static const auto AccumulateLayersCall = reinterpret_cast<void*>(Utils::FindSignature(XorStr("client.dll"), XorStr("84 C0 75 0D F6 87")));
	static const auto SetupVelocityCall = reinterpret_cast<void*>(Utils::FindSignature(XorStr("client.dll"), XorStr("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80")));

	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return oIsHltv(ECX, EDX);

	if (_ReturnAddress() == SetupVelocityCall)
		return true;

	if (_ReturnAddress() == AccumulateLayersCall)
		return true;

	oIsHltv(ECX, EDX);
}