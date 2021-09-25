#pragma once

#include "Utils\Interfaces.h"
#include "SDK\IBaseClientDll.h"
#include "SDK\IClientMode.h"
#include "SDK\ISurface.h"
#include "SDK\IGameEvent.h"
#include "SDK\CInput.h"
#include "SDK\CModelRender.h"
#include "SDK\IVModelInfo.h"
#include "SDK\IMaterial.h"
#include "SDK\CPrediction.h"
#include "SDK\IVRenderView.h"
#include <assert.h>
#include <d3d9.h>
#include <intrin.h>

namespace HookIndexes
{
	constexpr auto OverRideView = 18;
	constexpr auto PaintTraverse = 41;
	constexpr auto CreateMove   = 24;
	constexpr auto FrameStage = 37;
	constexpr auto DME = 21;
	constexpr auto ExtraBonePro	= 197;
	constexpr auto SceneEnd     = 9;
	constexpr auto EndScene		= 42;
	constexpr auto SceneReset   = 16;
	constexpr auto AccumulateLayers = 206;
	constexpr auto IsHltv = 93;
	constexpr auto SvCheats = 13;
	constexpr auto RunCommand = 19; 
	constexpr auto EyeAngles = 169;
	constexpr auto WriteUserCMD = 24;
	constexpr auto ViewModelFov = 35;
	constexpr auto SendNetMsg = 40;
	constexpr auto StandardBlendingRules = 205;
	constexpr auto SetupBones = 13;
	constexpr auto IsPaused = 90;
	constexpr auto ShouldInterpolate = 125;
}

class IMatRenderContext;
class CBoneBitList;
class VMTHook;
class ShadowVTManager;

typedef float Quaternion[4];

using ShouldSkipAnimFrame_t = bool(__fastcall*)(void*, void*);
extern ShouldSkipAnimFrame_t ShouldSkipAnimFrame_h;

using CheckForSequenceChange_t = void(__fastcall*)(void*, void*, void*, int, bool, bool);
extern CheckForSequenceChange_t CheckForSequenceChange_h;

using BuildTransformations_t = void(__fastcall*)(void*, void*, int, int, int, int, int, int);
extern BuildTransformations_t BuildTransformations_h;

using CalculateView_t = void(__fastcall*)(void*, void*, Vector&, Vector&, float&, float&, float&);
extern CalculateView_t CalculateView_h;

class Hooks
{
public:
    // Initialization setup, called on injection
    static void Init();
	static void InitMinHook();
    static void Restore();

    /*---------------------------------------------*/
    /*-------------Hooked functions----------------*/
    /*---------------------------------------------*/

	static void     __stdcall   FrameStageNotify(ClientFrameStage_t curStage);
    static bool     __fastcall  CreateMove(IClientMode*, void*, float, CUserCmd*);
	static void		__fastcall  PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
	static void     __fastcall  OverrideView(void* ecx, void* edx, CViewSetup* pSetup);
	static void     __fastcall  DrawModelExecute(void* ecx, void* edx, IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& render_info, matrix3x4_t* matrix);
	static void     __fastcall  DoExtraBonesProcessing(void* ECX, void* EDX, void* UNKN1, void* UNKN2, void* UNKN3, void* UNKN4, CBoneBitList& UNKN5, void* UNKN6);
	static void     __fastcall  SceneEnd(void *ecx, void *edx);
	static long		__stdcall   EndScene(IDirect3DDevice9* pDevice);
	static long		__stdcall   EndSceneReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	static void		__fastcall  BuildTransformations(void* ECX, void* EDX, int A2, int A3, int A4, int A5, int A6, int A7);
	static void	    __fastcall  AccumulateLayers(C_BaseEntity* Entity, void* ECX, void** BoneSetup, Vector* Position, float Time, Quaternion* Quat);
	static bool		__fastcall  IsHltv(uintptr_t ECX, uintptr_t EDX);
	static bool		__fastcall  SvCheats(PVOID ConVar, void* EDX);
	static Vector*  __fastcall  GetEyeAngles(void* ECX, void* EDX);
	static bool		__fastcall  WriteUserCmdDeltaToBuffer(void* EDX, void* ECX, int Slot, bf_write* Buffer, int From, int To, bool NewCMD);
	static void		__fastcall  RunCommand(void* ECX, void* EDX, C_BaseEntity* Entity, CUserCmd* CMD, IMoveHelper* MoveHelper);
	static float	__fastcall  GetViewmodelFOV();
	static bool     __fastcall  SendNetMsg(NetChannel* NetChannel, void* EDX, INetMessage& MSG, bool ForceReliable, bool Voice);
	static int		__fastcall  SendDatagram(void* NetChannel, void* EDX, void* Datagram);
	static void		__fastcall  StandardBlendingRules(void* ECX, void* EDX, studiohdr_t* HDR, Vector* Position, Quaternion* Quat, float CurrentTime, int CurrentMask);
	static bool		__fastcall  SetupBones(void* ECX, void* EDX, matrix3x4_t* MatrixOut, int MaxBones, int Mask, float CurrentTime);
	static bool		__fastcall  ShouldSkipAnimationFrame(void* ECX, void* EDX);
	static void		__fastcall  CheckForSequenceChange(void* ECX, void* EDX, void* HDR, int CurrentSequence, bool ForceNewSequence, bool Interpolate);
	static void		__fastcall  CalculateView(void* ECX, void* EDX, Vector& EyeOrigin, Vector& EyeAngles, float& ZNear, float& ZFar, float& FOV);
	static bool		__stdcall   IsPaused();
	static bool		__fastcall  ShouldInterpolate(void* ECX);

    static LRESULT  __stdcall   WndProc   (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    /*---------------------------------------------*/
    /*-------------VMT Hook pointers---------------*/
    /*---------------------------------------------*/

	std::unique_ptr<VMTHook> pClientHook;
    std::unique_ptr<VMTHook> pClientModeHook;
    std::unique_ptr<VMTHook> pSurfaceHook;
	std::unique_ptr<VMTHook> pPanelHook;
	std::unique_ptr<VMTHook> pModelHook;
	std::unique_ptr<VMTHook> pRenderViewHook;
	std::unique_ptr<VMTHook> pDirectXHook;
	std::unique_ptr<VMTHook> pEngineHook;
	std::unique_ptr<VMTHook> pPredictHook;
	std::unique_ptr<VMTHook> pNetChannelHook;

	std::unique_ptr<ShadowVTManager> pRunCommandHook = nullptr;
	std::unique_ptr<ShadowVTManager> pPlayerHook = nullptr;
	std::unique_ptr<ShadowVTManager> pSvCheatsHook = nullptr;
	std::unique_ptr<ShadowVTManager> pPlayerRenderableHook = nullptr;

    /*---------------------------------------------*/
    /*-------------Hook prototypes-----------------*/
    /*---------------------------------------------*/

	typedef void (__stdcall*  FrameStageNotify_t) (ClientFrameStage_t);
    typedef bool (__fastcall* CreateMove_t) (IClientMode*, void*, float, CUserCmd*);
	typedef void (__thiscall* PaintTraverse_t) (PVOID, unsigned int, bool, bool);
	typedef void (__fastcall* OverrideView_t) (void*, void*, CViewSetup*);
	typedef void (__thiscall* DrawModelExecute_t) (void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	typedef void (__thiscall* ExtraBoneProcess_t) (void*, void*, void*, void*, void*, CBoneBitList&, void*);
	typedef void (__fastcall* SceneEnd_t) (void*, void*);
	typedef long (__stdcall* EndScene_t)(IDirect3DDevice9*);
	typedef long (__stdcall* EndSceneReset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	typedef void (__fastcall* BuildTransformations_t) (void*, void*, int, int, int, int, int, int);
	typedef void (__fastcall* AccumulateLayers_t) (C_BaseEntity*, void*, void**, Vector*, float, Quaternion*);
	typedef bool (__fastcall* IsHltv_t) (uintptr_t, uintptr_t);
	typedef bool (__fastcall* SvCheats_t) (PVOID, void*);
	typedef Vector*(__fastcall* EyeAngles_t) (void*, void*);
	typedef void (__fastcall* RunCommand_t) (void*, void*, C_BaseEntity*, CUserCmd*, IMoveHelper*);
	typedef bool (__fastcall* WriteUserCMD_t) (void*, void*, int, bf_write*, int, int, bool);
	typedef bool (__fastcall* SendNetMsg_t) (NetChannel*, INetMessage&, bool, bool);
	typedef bool (__fastcall* SendDatagram_t) (void*, void*, void*);
	typedef void (__fastcall* StandardBlendingRules_t) (void*, void*, studiohdr_t*, Vector*, Quaternion*, float, int);
	typedef bool (__fastcall* SetupBones_t) (void*, void*, matrix3x4_t*, int, int, float);
	typedef bool(__stdcall* IsPaused_t) ();
	typedef bool(__stdcall* ShouldInterpolate_t) (void*);

	HWND hCSGOWindow = nullptr;
private:
    bool bInitializedDrawManager = false;   
    WNDPROC pOriginalWNDProc        = nullptr;
};

extern Hooks g_Hooks;

struct INetMessage
{
	virtual					~INetMessage() { }
	virtual void			SetNetChannel(void* pNetChannel) = 0;
	virtual void			SetReliable(bool bState) = 0;
	virtual bool			Process() = 0;
	virtual	bool			ReadFromBuffer(bf_read& buffer) = 0;
	virtual	bool			WriteToBuffer(bf_write& buffer) = 0;
	virtual bool			IsReliable() const = 0;
	virtual int				GetType() const = 0;
	virtual int				GetGroup() const = 0;
	virtual const char* GetName() const = 0;
	virtual void* GetNetChannel(void) const = 0;
	virtual const char* ToString() const = 0;
};

template<typename T>
class CNetMessagePB : public INetMessage, public T {};

class CCLCMsg_Move
{
private:
	char __PAD0[0x8];
public:
	int numBackupCommands;
	int numNewCommands;
};

using CCLCMsg_Move_t = CNetMessagePB<CCLCMsg_Move>;

class VMTHook
{
public:
	VMTHook(void* ppClass)
	{
		this->ppBaseClass = static_cast<std::uintptr_t**>(ppClass);

		// loop through all valid class indexes. When it will hit invalid (not existing) it will end the loop
		while (static_cast<std::uintptr_t*>(*this->ppBaseClass)[this->indexCount])
			++this->indexCount;

		const std::size_t kSizeTable = this->indexCount * sizeof(std::uintptr_t);

		this->pOriginalVMT = *this->ppBaseClass;
		this->pNewVMT = std::make_unique<std::uintptr_t[]>(this->indexCount);

		// copy original vtable to our local copy of it
		std::memcpy(this->pNewVMT.get(), this->pOriginalVMT, kSizeTable);

		// replace original class with our local copy
		*this->ppBaseClass = this->pNewVMT.get();
	};
	~VMTHook() { *this->ppBaseClass = this->pOriginalVMT; };

	template<class Type>
	Type GetOriginal(const std::size_t index)
	{
		return reinterpret_cast<Type>(this->pOriginalVMT[index]);
	};

	HRESULT Hook(const std::size_t index, void* fnNew)
	{
		if (index > this->indexCount)   // check if given index is valid
			return E_INVALIDARG;

		this->pNewVMT[index] = reinterpret_cast<std::uintptr_t>(fnNew);
		return S_OK;
	};

	HRESULT Unhook(const std::size_t index)
	{
		if (index > this->indexCount)
			return E_INVALIDARG;

		this->pNewVMT[index] = this->pOriginalVMT[index];
		return S_OK;
	};

private:
	std::unique_ptr<std::uintptr_t[]> pNewVMT = nullptr;    // Actual used vtable
	std::uintptr_t**                  ppBaseClass = nullptr;             // Saved pointer to original class
	std::uintptr_t*                   pOriginalVMT = nullptr;             // Saved original pointer to the VMT
	std::size_t                       indexCount = 0;                     // Count of indexes inside out f-ction
};


class ProtectGuard
{
public:

	ProtectGuard(void *base, uint32_t len, uint32_t protect)
	{
		this->base = base;
		this->len = len;

		if (!VirtualProtect(base, len, protect, (PDWORD)&old_protect))
			throw std::runtime_error("Failed to protect region!");
	}

	~ProtectGuard()
	{
		VirtualProtect(base, len, old_protect, (PDWORD)&old_protect);
	}

private:

	void *base;
	uint32_t len;
	uint32_t old_protect;
};

class ShadowVTManager // GLAD
{

public:

	ShadowVTManager() : class_base(nullptr), method_count(0), shadow_vtable(nullptr), original_vtable(nullptr) {}
	ShadowVTManager(void *base) : class_base(base), method_count(0), shadow_vtable(nullptr), original_vtable(nullptr) {}
	~ShadowVTManager()
	{
		RestoreTable();

		delete[] shadow_vtable;
	}

	inline void Setup(void *base = nullptr)
	{
		if (base != nullptr)
			class_base = base;

		if (class_base == nullptr)
			return;

		original_vtable = *(uintptr_t**)class_base;
		method_count = GetMethodCount(original_vtable);

		if (method_count == 0)
			return;

		shadow_vtable = new uintptr_t[method_count + 1]();

		shadow_vtable[0] = original_vtable[-1];
		std::memcpy(&shadow_vtable[1], original_vtable, method_count * sizeof(uintptr_t));

		try
		{
			auto guard = ProtectGuard{ class_base, sizeof(uintptr_t), PAGE_READWRITE };
			*(uintptr_t**)class_base = &shadow_vtable[1];
		}
		catch (...)
		{
			delete[] shadow_vtable;
		}
	}

	template<typename T>
	inline void Hook(uint32_t index, T method)
	{
		assert(index < method_count);
		shadow_vtable[index + 1] = reinterpret_cast<uintptr_t>(method);
	}

	inline void Unhook(uint32_t index)
	{
		assert(index < method_count);
		shadow_vtable[index + 1] = original_vtable[index];
	}

	template<typename T>
	inline T GetOriginal(uint32_t index)
	{
		return (T)original_vtable[index];
	}

	inline void RestoreTable()
	{
		try
		{
			if (original_vtable != nullptr)
			{
				auto guard = ProtectGuard{ class_base, sizeof(uintptr_t), PAGE_READWRITE };
				*(uintptr_t**)class_base = original_vtable;
				original_vtable = nullptr;
			}
		}
		catch (...) {}
	}

private:

	inline uint32_t GetMethodCount(uintptr_t *vtable_start)
	{
		uint32_t len = -1;

		do ++len; while (vtable_start[len]);

		return len;
	}

	void *class_base;
	uint32_t method_count;
	uintptr_t *shadow_vtable;
	uintptr_t *original_vtable;
};