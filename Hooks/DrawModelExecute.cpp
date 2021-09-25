#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

void __fastcall Hooks::DrawModelExecute(void* ecx, void* edx, IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached DrawModelExecute.")); Init = false; }

	static auto oDrawModelExecute = g_Hooks.pModelHook->GetOriginal<DrawModelExecute_t>(HookIndexes::DME);

	std::string ModelName = g_pModelInfo->GetModelName((model_t*)info.pModel);

	C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(info.index);

	static std::vector< IMaterial* > Materials;

	static bool InitMats = false;
	if (!InitMats)
	{
		Materials.push_back(g_ESP.CreateMaterial(true, false, false)); // material.
		Materials.push_back(g_ESP.CreateMaterial(false, false, false, 0)); // flat.
		Materials.push_back(g_ESP.CreateMaterial(true, false, true)); // material zignore.
		Materials.push_back(g_ESP.CreateMaterial(false, false, true)); // flat zignore.
		InitMats = true;
	}

	float Color[3] = { 255,255,255 };

	if (g_pEngine->IsConnected() && g_pEngine->IsInGame())
	{
		if (g::pLocalEntity && g::pLocalEntity->IsAlive())
		{
			if (ModelName.find(XorStr("v_sleeve")) != std::string::npos && g_Menu.Config.RemoveSleeves && g_Menu.Config.EnableRemovals)
			{
				IMaterial* Material = g_pMaterialSys->FindMaterial(ModelName.c_str(), TEXTURE_GROUP_MODEL);

				if (!Material)
					return;

				Material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				g_pModelRender->ForcedMaterialOverride(Material);
				oDrawModelExecute(ecx, context, state, info, matrix);
			}

			if (ModelName.find(XorStr("models/player")) != std::string::npos && g_Menu.Config.FakeChams)
			{	
				if (pPlayerEntity == g::pLocalEntity)
				{
					for (auto& i : g::FakeMatrix) // anti-fakelag
					{
						i[0][3] += info.origin.x;
						i[1][3] += info.origin.y;
						i[2][3] += info.origin.z;
					}
					g_pRenderView->SetColorModulation(Color);
					g_pModelRender->ForcedMaterialOverride(Materials[1]);
					oDrawModelExecute(ecx, context, state, info, g::FakeMatrix);
					g_pModelRender->ForcedMaterialOverride(nullptr);
				}
			}
		}
	}

	oDrawModelExecute(ecx, context, state, info, matrix);
}
