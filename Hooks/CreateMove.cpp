#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"
#include "../SDK/ClientState.h"

// fix later
bool __fastcall Hooks::SendNetMsg(NetChannel* NetChannel, void* EDX, INetMessage& MSG, bool ForceReliable, bool Voice)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached SendNetMsg.")); Init = false; }

	auto oSendNetMsg = g_Hooks.pNetChannelHook->GetOriginal<SendNetMsg_t>(HookIndexes::SendNetMsg);

	if (MSG.GetType() == 14)
		return false;

	if (MSG.GetGroup() == 9)
		Voice = true;

	//g_Hooks.g_pNetChannelHook = std::make_unique<VMTHook>(g_pClientState->pNetChannel); // put in cmove
	//g_Hooks.g_pNetChannelHook->Hook(HookIndexes::SendNetMsg, Hooks::SendNetMsg);// put in cmove

	return oSendNetMsg(NetChannel, MSG, ForceReliable, Voice);
}

bool __fastcall Hooks::CreateMove(IClientMode* thisptr, void* edx, float sample_frametime, CUserCmd* pCmd)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached CreateMove.")); Init = false; }

	static auto oCreateMove = g_Hooks.pClientModeHook->GetOriginal<CreateMove_t>(HookIndexes::CreateMove);
	oCreateMove(thisptr, edx, sample_frametime, pCmd);

	if (!pCmd || !pCmd->command_number)
		return oCreateMove;

	g::pCmd = pCmd;
	g::OriginalView = pCmd->viewangles;
	g::pLocalEntity = g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer());

	if (!g::pLocalEntity)
		return oCreateMove;

	uintptr_t* framePtr;
	__asm mov framePtr, ebp;

	g_Features.CreateMoveFeatures(pCmd);

	*(bool*)(*framePtr - 0x1C) = g::bSendPacket;

	return false;
}