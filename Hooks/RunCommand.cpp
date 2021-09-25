#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../SDK/ClientState.h"
#include "../Features/Features.h"

void __fastcall Hooks::RunCommand(void* ECX, void* EDX, C_BaseEntity* Entity, CUserCmd* CMD, IMoveHelper* MoveHelper)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached RunCommand.")); Init = false; }

	static auto oRunCommand = g_Hooks.pPredictHook->GetOriginal<RunCommand_t>(HookIndexes::RunCommand);

	if (Entity == nullptr || !g::pLocalEntity || !g::pLocalEntity->IsAlive())
		return oRunCommand(ECX, EDX, Entity, CMD, MoveHelper);

	if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
	{
		if (Entity->EntIndex() == g_pEngine->GetLocalPlayer())
		{
			if (1.f / g_pGlobalVars->intervalPerTick + g_Aimbot.DoubleTapStruct.UnPredTick + 8 <= CMD->tick_count)
			{
				CMD->hasbeenpredicted = true;
				return;
			}

			float VelocityModifierBackup = g::pLocalEntity->GetVelocityModifier();

			int Tickbase = g::pLocalEntity->GetTickBase();
			int CurrentTime = g_pGlobalVars->curtime;

			if (CMD->command_number == g_Aimbot.DoubleTapStruct.ShotCMD)
			{
				g::pLocalEntity->GetTickBaseAlt() = g_Aimbot.DoubleTapStruct.BaseTick - g::TickBaseShift + 1;
				g_pGlobalVars->curtime = TICKS_TO_TIME(g::pLocalEntity->GetTickBaseAlt());
			}

			if (g_Aimbot.DoubleTapStruct.OverrideVelMod && CMD->command_number == g_pClientState->m_nLastCommandAck + 1)
				g::pLocalEntity->GetVelocityModifier() = g_Aimbot.DoubleTapStruct.VelMod;

			oRunCommand(ECX, EDX, Entity, CMD, MoveHelper);

			if (CMD->command_number == g_Aimbot.DoubleTapStruct.ShotCMD)
			{
				g::pLocalEntity->GetTickBaseAlt() = Tickbase;
				g_pGlobalVars->curtime = CurrentTime;
			}

			if (!g_Aimbot.DoubleTapStruct.OverrideVelMod)
				g::pLocalEntity->GetVelocityModifier() = VelocityModifierBackup;
		}
	}
	else
		oRunCommand(ECX, EDX, Entity, CMD, MoveHelper);
}