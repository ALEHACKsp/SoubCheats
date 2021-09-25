#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

void __fastcall Hooks::StandardBlendingRules(void* ECX, void* EDX, studiohdr_t* HDR, Vector* Position, Quaternion* Quat, float CurrentTime, int CurrentMask)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached StandardBlendingRules.")); Init = false; }

	static auto oStandardBlendingRules = g_Hooks.pPlayerHook->GetOriginal<StandardBlendingRules_t>(HookIndexes::StandardBlendingRules);

	auto Entity = (C_BaseEntity*)(ECX);

	if (!(Entity->Effects() & 8))
		Entity->Effects() |= 8;

	oStandardBlendingRules(ECX, EDX, HDR, Position, Quat, CurrentTime, CurrentMask);
	Entity->Effects() &= ~0x8;
}
