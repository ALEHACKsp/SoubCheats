#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t curStage)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached FrameStageNotify.")); Init = false; }

	static auto oFrameStage = g_Hooks.pClientHook->GetOriginal<FrameStageNotify_t>(HookIndexes::FrameStage);

	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return oFrameStage(curStage);

	switch (curStage) {
	case FRAME_START:
		break;
	case FRAME_NET_UPDATE_START:
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		g_Misc.DisableInterp();
		break;
	case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		break;
	case FRAME_NET_UPDATE_END:
		break;
	case FRAME_RENDER_START:
		g_AnimFix.LocalEntity();
		g_AnimFix.EntityAnimFix();
		g_Misc.DisablePostProcessing();
		g_Misc.FixAnimLOD();
		g_ESP.RemoveFlash();
		break;
	case FRAME_RENDER_END:
		break;
	}

	oFrameStage(curStage);
}