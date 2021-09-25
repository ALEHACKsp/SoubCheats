#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../SDK/ClientState.h"
#include "../Features/Features.h"

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

void WriteUserCMD(bf_write* Buffer, CUserCmd* In, CUserCmd* Out)
{
	static DWORD WriteUsercmdF = (DWORD)Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));

	__asm
	{
		mov     ecx, Buffer
		mov     edx, In
		push	Out
		call    WriteUsercmdF
		add     esp, 4
	}
}

bool __fastcall Hooks::WriteUserCmdDeltaToBuffer(void* EDX, void* ECX, int Slot, bf_write* Buffer, int From, int To, bool NewCMD)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached WriteUserCmdDeltaToBuffer.")); Init = false; }

	static auto oWriteCmdDeltaBuffer = g_Hooks.pClientHook->GetOriginal<WriteUserCMD_t>(HookIndexes::WriteUserCMD);

	if (g::TickBaseShift <= 0 || !g::pLocalEntity || !g_pEngine->IsConnected() || !g_pEngine->IsInGame() || !g::pLocalEntity->IsAlive())
		return oWriteCmdDeltaBuffer(EDX, ECX, Slot, Buffer, From, To, NewCMD);

	if (From != -1)
		return true;

	auto BackupCMD = reinterpret_cast<int*>(std::uintptr_t(Buffer) - 48);
	auto NewCMDS = reinterpret_cast<int*>(std::uintptr_t(Buffer) - 44);

	auto NewCommands = *NewCMDS;
	auto NextCMDNumber = g_pClientState->m_nLastOutgoingCommand + g_pClientState->m_nChokedCommands + 1;
	auto TotalNewCMDS = clamp(g::TickBaseShift, 0, 62);
	g::TickBaseShift -= TotalNewCMDS;

	*NewCMDS = TotalNewCMDS;
	*BackupCMD = 0;

	for (To = NextCMDNumber - NewCommands + 1; To <= NextCMDNumber; To++)
	{
		if (!oWriteCmdDeltaBuffer(EDX, ECX, Slot, Buffer, From, To, true))
			return false;

		From = To;
	}

	auto LastRealCMD = g_pInput->GetUserCmd(Slot, From);
	auto FromCMD = CUserCmd{};

	if (LastRealCMD)
		FromCMD = *LastRealCMD;

	auto ToCMD = FromCMD;
	ToCMD.command_number = NextCMDNumber++;

	if (!g_Menu.Config.DoubleTapTeleport)
		ToCMD.tick_count += g_pGlobalVars->intervalPerTick * 3;
	else
		ToCMD.tick_count--;

	for (auto i = NewCommands; i <= TotalNewCMDS; i++)
	{
		WriteUserCMD(Buffer, &ToCMD, &FromCMD);
		FromCMD = ToCMD;
		ToCMD.command_number = NextCMDNumber++;
		if (!g_Menu.Config.DoubleTapTeleport)
			ToCMD.tick_count++;
		else
			ToCMD.tick_count--;
	}

	return true;
}
