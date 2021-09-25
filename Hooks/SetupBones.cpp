#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../SDK/ICvar.h"
#include "../SDK/ClientState.h"
#include "../Features/Features.h"

// no fix laters

bool __fastcall Hooks::SetupBones(void* ECX, void* EDX, matrix3x4_t* MatrixOut, int MaxBones, int Mask, float CurrentTime) 
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached SetupBones.")); Init = false; }

	auto oSetupBones = g_Hooks.pPlayerRenderableHook->GetOriginal<SetupBones_t>(HookIndexes::SetupBones);

	const auto Entity = reinterpret_cast<C_BaseEntity*>(uintptr_t(ECX) - 4);
	if (g::pLocalEntity && Entity && !Entity->IsDormant())
	{
		if (Entity == g::pLocalEntity) 
		{
			// deprecated
		}
		else
		{
			auto OldOrigin = Entity->GetOrigin();
			auto OldABSVelocity = Entity->GetAbsVelocity();
			auto OldEFlags = Entity->GetEFlags();
			*reinterpret_cast<int*>(uintptr_t(Entity) + 0xA68) = 0;

			Entity->SetAbsOrigin(Entity->GetOrigin());
			Entity->InvalidateBoneCache();

			Entity->GetEFlags() &= ~4096;

			Entity->GetAbsVelocity() = Entity->GetVelocity();

			Entity->Effects() |= 0x8;
			bool ReturnValue = oSetupBones(Entity->GetClientRenderable(), EDX, MatrixOut, MaxBones, Mask, CurrentTime);
			Entity->Effects() &= ~0x8;


			Entity->SetAbsOrigin(OldOrigin);
			Entity->GetAbsVelocity() = OldABSVelocity;
			Entity->GetEFlags() = OldEFlags;

			return ReturnValue;
		}
	}

	return oSetupBones(ECX, EDX, MatrixOut, MaxBones, Mask, CurrentTime);
}