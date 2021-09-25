#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

Vector* __fastcall Hooks::GetEyeAngles(void* ECX, void* EDX)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached GetEyeAngles.")); Init = false; }

	static auto oEyeAngle = g_Hooks.pPlayerHook->GetOriginal<EyeAngles_t>(HookIndexes::EyeAngles);

	if (ECX != g::pLocalEntity || !g::pLocalEntity)
		return oEyeAngle(ECX, EDX);

	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return oEyeAngle(ECX, EDX);

	static auto PitchReturn = std::uintptr_t(Utils::FindSignature(XorStr("client.dll"), XorStr("8B CE F3 0F 10 00 8B 06 F3 0F 11 45 ? FF 90 ? ? ? ? F3 0F 10 55")));
	static auto YawReturn = std::uintptr_t(Utils::FindSignature(XorStr("client.dll"), XorStr("F3 0F 10 55 ? 51 8B 8E")));

	if (std::uintptr_t(_ReturnAddress()) == PitchReturn || std::uintptr_t(_ReturnAddress()) == YawReturn)
		return g::pCmd ? &g::RealAngle : &g::pLocalEntity->GetEyeAngles();

	return oEyeAngle(ECX, EDX);
}