#include "AnimationFix.h"
#include "..\AntiAim\AntiAim.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\SDK\ICvar.h"
#include "..\..\SDK\ISurface.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\SDK\CEntity.h"
#include "..\..\SDK\IMaterial.h"
#include "..\..\SDK\IVModelInfo.h"
#include "..\..\SDK\IVRenderView.h"
#include "..\..\SDK\CGlowObjectManager.h"
#include "..\..\SDK\ClientClass.h"
#include "..\..\SDK\CModelRender.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK\MemAlloc.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\Menu\Menu.h"
#include "../../SDK/CPrediction.h"
#include "../../SDK/Vector.h"
#include <array>
#include "../../SDK/ClientState.h"

AnimationFix g_AnimFix;

void AnimationFix::LocalEntity()
{
	if (!g_pEngine->IsInGame())
		return;

	if (!g::pLocalEntity)
		return;

	if (!g::pLocalEntity->IsAlive())
		return;

	if (!g::pCmd)
		return;

	auto AnimState = g::pLocalEntity->AnimState();

	if (!AnimState)
		return;

	auto RefreshTick = []() -> bool {
		static int OldTick = 0;
		if (OldTick != g_pGlobalVars->tickcount) {
			OldTick = g_pGlobalVars->tickcount;
			return true;
		}
		return false;
	};

	static float ProperABS = AnimState->m_abs_yaw;
	static std::array<float, 24> SentPoseParams = g::pLocalEntity->PoseParamters();

	static AnimationLayer BackupLayers[15];

	if (!BackupLayers)
		std::memcpy(BackupLayers, g::pLocalEntity->AnimOverlays(), (sizeof(AnimationLayer) * 15));

	const auto BackupFrametime = g_pGlobalVars->frametime;
	const auto BackupRealtime = g_pGlobalVars->realtime;
	const auto BackupEFlags = g::pLocalEntity->GetEFlags();
	const auto BackupFlags = g::pLocalEntity->GetFlags();

	g_pGlobalVars->frametime = FLT_MAX;

	if (RefreshTick())
	{
		std::memcpy(BackupLayers, g::pLocalEntity->AnimOverlays(), (sizeof(AnimationLayer) * 15));

		g::pLocalEntity->ClientSideAnimation() = true;

		g::pLocalEntity->UpdateAnimState(AnimState, g::pCmd->viewangles);

		if (AnimState->m_last_clientside_anim_framecount == g_pGlobalVars->framecount)
			AnimState->m_last_clientside_anim_framecount = g_pGlobalVars->framecount - 1.f;

		if (AnimState->m_hit_ground)
			AnimState->m_time_in_air = 0.0f;

		g::pLocalEntity->UpdateClientAnimation();

		if (g_pClientState->m_nChokedCommands)
		{
			ProperABS = AnimState->m_abs_yaw;
			SentPoseParams = g::pLocalEntity->PoseParamters();
		}
	}

	g::pLocalEntity->ClientSideAnimation() = false;
	g::pLocalEntity->SetAbsAngles(Vector(0, ProperABS, 0));

	g_pGlobalVars->realtime = BackupRealtime;
	g_pGlobalVars->frametime = BackupFrametime;
	g::pLocalEntity->GetFlags() = BackupFlags;
	g::pLocalEntity->GetEFlags() = BackupEFlags;

	std::memcpy(g::pLocalEntity->AnimOverlays(), BackupLayers, (sizeof(AnimationLayer) * 15));
	g::pLocalEntity->PoseParamters() = SentPoseParams;
}

void AnimationFix::FakeLocalEntity()
{
	if (!g::pLocalEntity)
		return;

	if (!g::pLocalEntity->AnimState())
		return;

	if (!g::pLocalEntity->AnimOverlays())
		return;

	if (!g::pLocalEntity || !g::pLocalEntity->IsAlive()) {
		g::FakeLocalUpdate = true;
		return;
	}

	if (g::SpawnTime != g::pLocalEntity->GetSpawnTime() || g::FakeLocalUpdate)
	{
		g::InitFakeAnimation = false;
		g::SpawnTime = g::pLocalEntity->GetSpawnTime();
		g::FakeLocalUpdate = false;
	}

	if (!g::InitFakeAnimation)
	{
		g::FakeState = reinterpret_cast<C_AnimState*> (g_pMemAlloc->Alloc(sizeof(C_AnimState)));

		if (g::FakeState != nullptr)
			g::pLocalEntity->CreateAnimState(g::FakeState);

		g::InitFakeAnimation = true;
	}

	if (g::bSendPacket)
	{

		const auto Origin = g::pLocalEntity->GetRenderOrigin();

		const auto StoreAbsAngles = g::pLocalEntity->GetAbsAngles();

		const auto BackupFrametime = g_pGlobalVars->frametime;

		g_pGlobalVars->frametime = FLT_MAX;

		std::memcpy(g::FakeLayers.data(), g::pLocalEntity->AnimOverlays(), sizeof(g::FakeLayers));

		std::memcpy(g::FakePoses.data(), g::pLocalEntity->PoseParamters().data(), sizeof(g::FakePoses));

		g::pLocalEntity->InvalidateBoneCache();

		g::pLocalEntity->UpdateAnimState(g::FakeState, g::pCmd->viewangles);

		g::pLocalEntity->AnimOverlays()[12].m_flWeight = FLT_EPSILON;

		g::pLocalEntity->SetupBones(g::FakeMatrix, 128, 0x0007FF00 & ~0x00000200, g_pGlobalVars->curtime);

		memcpy(g::FakePosMatrix, g::FakeMatrix, sizeof(g::FakePosMatrix));

		for (auto& i : g::FakeMatrix)
		{
			i[0][3] -= Origin.x;
			i[1][3] -= Origin.y;
			i[2][3] -= Origin.z;
		}


		g::pLocalEntity->SetAbsAngles(StoreAbsAngles);

		g_pGlobalVars->frametime = BackupFrametime;

		std::memcpy(g::pLocalEntity->AnimOverlays(), g::FakeLayers.data(), sizeof(g::FakeLayers));

		std::memcpy(g::pLocalEntity->PoseParamters().data(), g::FakePoses.data(), sizeof(g::FakePoses));

		g::pLocalEntity->InvalidateBoneCache();
	}
}

void AnimationFix::EntityAnimFix() 
{
	for (int i = 1; i <= g_pGlobalVars->maxClients; i++)
	{
		C_BaseEntity* Entity = g_pEntityList->GetClientEntity(i);

		if (!Entity)
			continue;

		if (Entity == g::pLocalEntity)
			continue;

		auto AnimState = Entity->AnimState();

		if (!AnimState)
			continue;

		const float StoreCurtime = g_pGlobalVars->curtime;
		const float StoreFrametime = g_pGlobalVars->frametime;
		const int StoredTicks = g_pGlobalVars->tickcount;

		static auto host_timescale = g_pCvar->FindVar(XorStr("host_timescale"));

		Vector StoreOrigin = Entity->GetOrigin();
		Vector StoreABSOrigin = Entity->GetAbsOrigin();
		Vector StoreVelocity = Entity->GetVelocity();
		int StoreFlags = Entity->GetFlags();
		int StoreEFlags = Entity->GetEFlags();

		g_pGlobalVars->curtime = Entity->GetOldSimulationTime() + g_pGlobalVars->intervalPerTick;
		g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick * host_timescale->GetFloat();

		//static AnimationLayer BackupLayers[15];

		//std::memcpy(BackupLayers, Entity->AnimOverlays(), (sizeof(AnimationLayer) * 15));

		if (Entity->AnimOverlays()[5].m_flWeight)
			Entity->GetFlags() |= FL_ONGROUND;

		Entity->GetEFlags() &= ~0x1000;

		Entity->GetAbsVelocity() = Entity->GetVelocity();

		if (AnimState->m_last_clientside_anim_framecount == g_pGlobalVars->framecount)
			AnimState->m_last_clientside_anim_framecount = g_pGlobalVars->framecount - 1;

		Entity->ClientSideAnimation() = true;

		// correct here

		Entity->UpdateClientAnimation();

		Entity->ClientSideAnimation() = false;

		//std::memcpy(Entity->AnimOverlays(), BackupLayers, (sizeof(AnimationLayer) * 15));

		Entity->GetOrigin() = StoreOrigin;
		Entity->SetAbsOrigin(StoreABSOrigin);
		Entity->GetVelocity() = StoreVelocity;
		Entity->GetFlags() = StoreFlags;
		Entity->GetEFlags() = StoreEFlags;

		g_pGlobalVars->curtime = StoreCurtime;
		g_pGlobalVars->frametime = StoreFrametime;
		g_pGlobalVars->tickcount = StoredTicks;

		Entity->InvalidateBoneCache();
		Entity->SetupBones(nullptr, -1, 0x7FF00, g_pGlobalVars->curtime);
	}
}

