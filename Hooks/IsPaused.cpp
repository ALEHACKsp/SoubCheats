#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

bool __stdcall Hooks::IsPaused()
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached IsPaused.")); Init = false; }

	auto oIsPaused = g_Hooks.pEngineHook->GetOriginal<IsPaused_t>(HookIndexes::IsPaused);

	static auto AdrOfIsPausedExtrapolate = reinterpret_cast<uintptr_t*>(Utils::FindSignature(XorStr("client.dll"), XorStr("0F B6 0D ? ? ? ? 84 C0 0F 44")));

	if (_ReturnAddress() == (void*)AdrOfIsPausedExtrapolate)
		return true;

	return oIsPaused();
}