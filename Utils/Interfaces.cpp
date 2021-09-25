#include "Interfaces.h"
#include "Utils.h"

#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"
#include "..\SDK\IClientEntityList.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\IGameEvent.h"
#include "..\SDK\ISurface.h"
#include "..\SDK\IEngineTrace.h"
#include "..\SDK\ISurfaceData.h"
#include "..\SDK\ICvar.h"
#include "..\SDK\IVModelInfo.h"
#include "..\SDK\CModelRender.h"
#include "..\SDK\IMaterial.h"
#include "..\SDK\IVRenderView.h"
#include "..\SDK\CGlowObjectManager.h"
#include "..\SDK\ClientState.h"
#include "..\SDK\MemAlloc.h"

#include "..\Security\Xor.h"

#include <fstream>

IBaseClientDLL*     g_pClientDll    = nullptr;
IClientMode*        g_pClientMode   = nullptr;
IClientEntityList*  g_pEntityList   = nullptr;
IVEngineClient*     g_pEngine       = nullptr;
CPrediction*        g_pPrediction   = nullptr;
IGameMovement*      g_pMovement     = nullptr;
IMoveHelper*        g_pMoveHelper   = nullptr;
CGlobalVarsBase*    g_pGlobalVars   = nullptr;
IGameEventManager*  g_pEventManager = nullptr;
ISurface*           g_pSurface      = nullptr;
IEngineTrace*       g_pTrace        = nullptr;
IPhysicsSurfaceProps* g_pSurfaceData = nullptr;
ICVar*              g_pCvar			= nullptr;
IPanel*				g_pPanel		= nullptr;
IVModelInfo*		g_pModelInfo	= nullptr;
CModelRender*       g_pModelRender  = nullptr;
IMaterialSystem*    g_pMaterialSys  = nullptr;
IVRenderView*       g_pRenderView   = nullptr;
CInput*				g_pInput		= nullptr;
CGlowObjectManager* g_pGlowManager  = nullptr;
ClientState*		g_pClientState  = nullptr;
IMemAlloc*			g_pMemAlloc     = nullptr;

namespace Interfaces
{
	template< class T >
	T* FindClass(std::string szModuleName, std::string szInterfaceName, bool bSkip = false)
	{
		if (szModuleName.empty() || szInterfaceName.empty())
			return nullptr;
		typedef PVOID(*CreateInterfaceFn)(const char* pszName, int* piReturnCode);
		CreateInterfaceFn hInterface = nullptr;
		while (!hInterface)
		{
			hInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(szModuleName.c_str()), XorStr("CreateInterface"));
			Sleep(5);
		}

		char pszBuffer[256];
		for (int i = 0; i < 100; i++)
		{
			sprintf_s(pszBuffer, XorStr("%s%0.3d"), szInterfaceName.c_str(), i);
			PVOID pInterface = hInterface(pszBuffer, nullptr);

			if (pInterface && pInterface != NULL)
			{
				if (bSkip)
					sprintf_s(pszBuffer, XorStr("%s%0.3d"), szInterfaceName.c_str(), i + 1);

				Sleep(5);
				break;
			}
		}

		return (T*)hInterface(pszBuffer, nullptr);
	}

	template<typename T>
	T* CaptureInterface(const char* szModuleName, const char* szInterfaceVersion)
	{
		HMODULE moduleHandle = GetModuleHandleA(szModuleName);
		if (moduleHandle)
		{
			CreateInterfaceFn pfnFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(moduleHandle, XorStr("CreateInterface")));
			return reinterpret_cast<T*>(pfnFactory(szInterfaceVersion, nullptr));
		}
		return nullptr;
	}

    void Init()
    {
        g_pClientDll    = FindClass<IBaseClientDLL>(XorStr("client.dll"), XorStr("VClient"));
        g_pClientMode   = **reinterpret_cast<IClientMode***>((*reinterpret_cast<uintptr_t**>(g_pClientDll))[10] + 0x5u);  
        g_pGlobalVars   = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(g_pClientDll))[0]  + 0x1Fu); 
        g_pEntityList   = FindClass<IClientEntityList>(XorStr("client.dll"), XorStr("VClientEntityList"));
        g_pEngine       = FindClass<IVEngineClient>(XorStr("engine.dll"), XorStr("VEngineClient"));
        g_pPrediction   = FindClass<CPrediction>(XorStr("client.dll"), XorStr("VClientPrediction"));
        g_pMovement     = FindClass<IGameMovement>(XorStr("client.dll"), XorStr("GameMovement"));
        g_pMoveHelper   = **reinterpret_cast<IMoveHelper***>((Utils::FindSignature(XorStr("client.dll"), XorStr("8B 0D ? ? ? ? 8B 46 08 68")) + 0x2));
        g_pEventManager = CaptureInterface<IGameEventManager>(XorStr("engine.dll"), XorStr("GAMEEVENTSMANAGER002"));
        g_pSurface      = FindClass<ISurface>(XorStr("vguimatsurface.dll"), XorStr("VGUI_Surface"));
		g_pTrace        = FindClass<IEngineTrace>(XorStr("engine.dll"), XorStr("EngineTraceClient"));
		g_pSurfaceData  = FindClass<IPhysicsSurfaceProps>(XorStr("vphysics.dll"), XorStr("VPhysicsSurfaceProps"));
		g_pCvar         = CaptureInterface<ICVar>(XorStr("vstdlib.dll"), XorStr("VEngineCvar007"));
		g_pPanel		= FindClass<IPanel>(XorStr("vgui2.dll"), XorStr("VGUI_Panel"));
		g_pModelInfo    = FindClass<IVModelInfo>(XorStr("engine.dll"), XorStr("VModelInfoClient"));
		g_pModelRender  = FindClass<CModelRender>(XorStr("engine.dll"), XorStr("VEngineModel"));
		g_pMaterialSys  = FindClass<IMaterialSystem>(XorStr("materialsystem.dll"), XorStr("VMaterialSystem"));
		g_pRenderView   = FindClass<IVRenderView>(XorStr("engine.dll"), XorStr("VEngineRenderView"));	
		g_pInput		= (CInput*)*(DWORD*)(Utils::FindSignature(XorStr("client.dll"), XorStr("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1);
		g_pGlowManager  = *reinterpret_cast<CGlowObjectManager**>(Utils::FindSignature(XorStr("client.dll"), XorStr("0F 11 05 ? ? ? ? 83 C8 01")) + 3);
		g_pClientState = **(ClientState***)(Utils::FindSignature(XorStr("engine.dll"), XorStr("A1 ? ? ? ? 8B 80 ? ? ? ? C3")) + 1);
		g_pMemAlloc     = *reinterpret_cast<IMemAlloc**>(GetProcAddress(GetModuleHandleA(XorStr("tier0.dll")), XorStr("g_pMemAlloc")));
    }
}