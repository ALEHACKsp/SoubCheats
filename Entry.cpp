#include <thread>
#include "Entry.h"
#include "Utils\Utils.h"
#include "Utils\Math.h"
#include "SDK\ICvar.h"
#include "Features\Features.h"
#include "Menu\Menu.h"
#include "SDK\Hitboxes.h"
#include "libloaderapi.h"
#include "Security/Xor.h"
#include "SDK/ClientState.h"
#include "Features/Events/Events.h"
#include "SDK/CPrediction.h"

#include "Dependencies/ImGui/imgui.h"
#include "Dependencies/ImGui/imgui_impl_dx9.h"
#include "Dependencies/ImGui/imgui_impl_win32.h"
#include "Dependencies/ImGui/imgui_internal.h"

#include "Dependencies/MinHook/minhook.h"

Hooks g_Hooks;

#define TICK_INTERVAL			(g_pGlobalVars->intervalPerTick)
#define TICKS_TO_TIME(t)        (g_pGlobalVars->intervalPerTick * (t))
#define TIME_TO_TICKS(dt)		((int)(0.5f + (float)(dt) / TICK_INTERVAL))

void Warning(const char* msg, ...)
{
	if (msg == nullptr)
		return;
	typedef void(__cdecl* MsgFn)(const char* msg, va_list);
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle(XorStr("tier0.dll")), XorStr("Warning"));
	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list);
}

void Message(const char* msg, ...)
{
	if (msg == nullptr)
		return;
	typedef void(__cdecl* MsgFn)(const char* msg, va_list);
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle(XorStr("tier0.dll")), XorStr("Msg"));
	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list);
}

void Hooks::Init()
{
	while (!(g_Hooks.hCSGOWindow = FindWindowA(XorStr("Valve001"), nullptr)))
		Sleep(50);

	Utils::Log(XorStr("Found the CSGO window."));

	g_pNetvars = std::make_unique<NetvarTree>();

	if (g_Hooks.hCSGOWindow)
		g_Hooks.pOriginalWNDProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(g_Hooks.hCSGOWindow, GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(g_Hooks.WndProc)));

	DWORD g_pPlayerRenderable = (DWORD)(Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 8B 0D")));
	DWORD g_pCCSPlayer = (DWORD)Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C")) + 0x47;
	DWORD g_pDirectX = **(DWORD**)(Utils::FindSignature(XorStr("shaderapidx9.dll"), XorStr("A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C")) + 1);

	g_Hooks.pClientHook = std::make_unique<VMTHook>(g_pClientDll);
	g_Hooks.pClientModeHook = std::make_unique<VMTHook>(g_pClientMode);
	g_Hooks.pSurfaceHook = std::make_unique<VMTHook>(g_pSurface);
	g_Hooks.pModelHook = std::make_unique<VMTHook>(g_pModelRender);
	g_Hooks.pRenderViewHook = std::make_unique<VMTHook>(g_pRenderView);
	g_Hooks.pEngineHook = std::make_unique<VMTHook>(g_pEngine);
	g_Hooks.pDirectXHook = std::make_unique<VMTHook>((DWORD**)g_pDirectX);
	g_Hooks.pPanelHook = std::make_unique<VMTHook>(g_pPanel);
	g_Hooks.pPredictHook = std::make_unique<VMTHook>(g_pPrediction);

	g_Hooks.pPanelHook->Hook(HookIndexes::PaintTraverse, Hooks::PaintTraverse);
	g_Hooks.pClientHook->Hook(HookIndexes::FrameStage, Hooks::FrameStageNotify);
	g_Hooks.pClientModeHook->Hook(HookIndexes::CreateMove, Hooks::CreateMove);
	g_Hooks.pClientModeHook->Hook(HookIndexes::OverRideView, Hooks::OverrideView);
	g_Hooks.pClientModeHook->Hook(HookIndexes::ViewModelFov, Hooks::GetViewmodelFOV);
	g_Hooks.pModelHook->Hook(HookIndexes::DME, Hooks::DrawModelExecute);
	g_Hooks.pRenderViewHook->Hook(HookIndexes::SceneEnd, Hooks::SceneEnd);
	g_Hooks.pEngineHook->Hook(HookIndexes::IsHltv, Hooks::IsHltv);
	g_Hooks.pEngineHook->Hook(HookIndexes::IsPaused, Hooks::IsPaused);
	g_Hooks.pDirectXHook->Hook(HookIndexes::EndScene, Hooks::EndScene);
	g_Hooks.pDirectXHook->Hook(HookIndexes::SceneReset, Hooks::EndSceneReset);
	g_Hooks.pClientHook->Hook(HookIndexes::WriteUserCMD, Hooks::WriteUserCmdDeltaToBuffer);
	g_Hooks.pPredictHook->Hook(HookIndexes::RunCommand, Hooks::RunCommand);

	g_Hooks.pPlayerHook = std::make_unique<ShadowVTManager>();
	g_Hooks.pPlayerHook->Setup((uintptr_t*)g_pCCSPlayer);

	g_Hooks.pPlayerHook->Hook(HookIndexes::ExtraBonePro, Hooks::DoExtraBonesProcessing);
	g_Hooks.pPlayerHook->Hook(HookIndexes::AccumulateLayers, Hooks::AccumulateLayers);
	//g_Hooks.pPlayerHook->Hook(HookIndexes::EyeAngles, Hooks::GetEyeAngles);
	g_Hooks.pPlayerHook->Hook(HookIndexes::StandardBlendingRules, Hooks::StandardBlendingRules);
	g_Hooks.pPlayerHook->Hook(HookIndexes::ShouldInterpolate, Hooks::ShouldInterpolate);

	g_Hooks.pSvCheatsHook = std::make_unique<ShadowVTManager>();
	g_Hooks.pSvCheatsHook->Setup(g_pCvar->FindVar(XorStr("sv_cheats")));
	g_Hooks.pSvCheatsHook->Hook(HookIndexes::SvCheats, Hooks::SvCheats);

	//g_Hooks.pPlayerRenderableHook = std::make_unique<ShadowVTManager>();
	//g_Hooks.pPlayerRenderableHook->Setup((uintptr_t*)g_pPlayerRenderable);
	//g_Hooks.pPlayerRenderableHook->Hook(HookIndexes::SetupBones, Hooks::SetupBones); // see Hooks\SetupBones.cpp

	g_Events.InitializeEventListeners();

	ConVar* BoneSetup = g_pCvar->FindVar(XorStr("cl_threaded_bone_setup"));
	*(int*)((DWORD)&BoneSetup->fnChangeCallback + 0xC) = 0;
	BoneSetup->SetValue(1);

	ConVar* Cheats = g_pCvar->FindVar(XorStr("sv_cheats"));
	*(int*)((DWORD)&Cheats->fnChangeCallback + 0xC) = 0;
	Cheats->SetValue(1);

	ConVar* Extapolate = g_pCvar->FindVar(XorStr("cl_extrapolate"));
	*(int*)((DWORD)&Extapolate->fnChangeCallback + 0xC) = 0;
	Extapolate->SetValue(0);

	Utils::Log(XorStr("Hooking functions."));
}

void Hooks::InitMinHook()
{
	MH_Initialize();

	const auto _ShouldSkipAnim = (void*)(Utils::FindSignature("client.dll", "57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"));
	MH_CreateHook(_ShouldSkipAnim, Hooks::ShouldSkipAnimationFrame, (void**)&ShouldSkipAnimFrame_h);

	const auto _CheckForSeqChange = (void*)(Utils::FindSignature("client.dll", "55 8B EC 51 53 8B 5D 08 56 8B F1 57 85"));
	MH_CreateHook(_CheckForSeqChange, Hooks::CheckForSequenceChange, (void**)&CheckForSequenceChange_h);

	const auto _BuildTransformations = (void*)(Utils::FindSignature("client.dll", "55 8B EC 56 8B 75 18 57"));
	MH_CreateHook(_BuildTransformations, Hooks::BuildTransformations, (void**)&BuildTransformations_h);

	MH_EnableHook(MH_ALL_HOOKS);

	Utils::Log(XorStr("Hooked MinHook functions."));
}

void Hooks::Restore()
{
	exit(EXIT_SUCCESS);
}