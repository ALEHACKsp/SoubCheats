#pragma once
#include "Definitions.h"
#include "IClientUnknown.h"
#include "IClientEntityList.h"
#include "CInput.h"
#include "..\Utils\Utils.h"
#include "..\Utils\NetvarManager.h"
#include "../Security/Xor.h"

#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = g_pNetvars->GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
	}

#define OFFSET(type, var, offset) \
    type& var() { \
        return *(type*)(uintptr_t(this) + offset); \
    } \

class C_BaseViewModel;

// class predefinition
class C_BaseCombatWeapon;

struct animstate_pose_param_cache_t {
	std::uint8_t pad_0x0[0x4]; //0x0
	std::uint32_t m_idx; //0x4
	char* m_name; //0x8
};

class AnimationLayer {
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	unsigned int m_nOrder; //0x0014
	unsigned int m_nSequence; //0x0018
	float m_flPrevCycle; //0x001C
	float m_flWeight; //0x0020
	float m_flWeightDeltaRate; //0x0024
	float m_flPlaybackRate; //0x0028
	float m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038

class C_AnimState 
{
public:
	std::uint8_t pad_0x0000[0x4]; //0x0000
	bool m_force_update; //0x0005
	std::uint8_t pad_0x0006[0x5A]; //0x0006
	C_BaseEntity* m_entity; //0x0060
	C_BaseCombatWeapon* m_weapon; //0x0064
	C_BaseCombatWeapon* m_last_weapon; //0x0068
	float m_last_clientside_anim_update; //0x006C
	std::uint32_t m_last_clientside_anim_framecount; //0x0070
	float m_last_clientside_anim_update_time_delta; //0x0074
	float m_eye_yaw; //0x0078
	float m_pitch; //0x007C
	float m_abs_yaw; //0x0080
	float m_feet_yaw; //0x0084
	float m_body_yaw; //0x0088
	float m_body_yaw_clamped; //0x008C
	float m_feet_vel_dir_delta; //0x0090
	std::uint8_t pad_0x0094[0x4]; //0x0094
	float m_feet_cycle; //0x0098
	float m_feet_yaw_rate; //0x009C
	std::uint8_t pad_0x00A0[0x4]; //0x00A0
	float m_duck_amount; //0x00A4
	float m_landing_duck_additive; //0x00A8
	std::uint8_t pad_0x00AC[0x4]; //0x00AC
	Vector m_origin; //0x00B0
	Vector m_old_origin; //0x00BC
	Vector2D m_vel2d; //0x00C8
	std::uint8_t pad_0x00D0[0x10]; //0x00D0
	Vector2D m_last_accelerating_vel; //0x00E0
	std::uint8_t pad_0x00E8[0x4]; //0x00E8
	float m_speed2d; //0x00EC
	float m_up_vel; //0x00F0
	float m_speed_normalized; //0x00F4
	float m_run_speed; //0x00F8
	float m_unk_feet_speed_ratio; //0x00FC
	float m_time_since_move; //0x0100
	float m_time_since_stop; //0x0104
	bool m_on_ground; //0x0108
	bool m_hit_ground; //0x0109
	std::uint8_t pad_0x010A[0x4]; //0x010A
	float m_time_in_air; //0x0110
	std::uint8_t pad_0x0114[0x6]; //0x0114
	float m_ground_fraction; //0x011C
	std::uint8_t pad_0x0120[0x2]; //0x0120
	float m_unk_fraction; //0x0124
	std::uint8_t pad_0x0128[0xC]; //0x0128
	bool m_moving; //0x0134
	std::uint8_t pad_0x0135[0x7B]; //0x0135
	animstate_pose_param_cache_t m_lean_yaw_pose; //0x1B0
	animstate_pose_param_cache_t m_speed_pose; //0x01BC
	animstate_pose_param_cache_t m_ladder_speed_pose; //0x01C8
	animstate_pose_param_cache_t m_ladder_yaw_pose; //0x01D4
	animstate_pose_param_cache_t m_move_yaw_pose; //0x01E0
	animstate_pose_param_cache_t m_run_pose; //0x01EC
	animstate_pose_param_cache_t m_body_yaw_pose; //0x01F8
	animstate_pose_param_cache_t m_body_pitch_pose; //0x0204
	animstate_pose_param_cache_t m_dead_yaw_pose; //0x0210
	animstate_pose_param_cache_t m_stand_pose; //0x021C
	animstate_pose_param_cache_t m_jump_fall_pose; //0x0228
	animstate_pose_param_cache_t m_aim_blend_stand_idle_pose; //0x0234
	animstate_pose_param_cache_t m_aim_blend_crouch_idle_pose; //0x0240
	animstate_pose_param_cache_t m_strafe_yaw_pose; //0x024C
	animstate_pose_param_cache_t m_aim_blend_stand_walk_pose; //0x0258
	animstate_pose_param_cache_t m_aim_blend_stand_run_pose; //0x0264
	animstate_pose_param_cache_t m_aim_blend_crouch_walk_pose; //0x0270
	animstate_pose_param_cache_t m_move_blend_walk_pose; //0x027C
	animstate_pose_param_cache_t m_move_blend_run_pose; //0x0288
	animstate_pose_param_cache_t m_move_blend_crouch_pose; //0x0294
	std::uint8_t pad_0x02A0[0x4]; //0x02A0
	float m_vel_unk; //0x02A4
	std::uint8_t pad_0x02A8[0x86]; //0x02A8
	float m_min_yaw; //0x0330
	float m_max_yaw; //0x0334
	float m_max_pitch; //0x0338
	float m_min_pitch; //0x033C

	float_t& m_flVelocity()
	{
		return *(float_t*)((uintptr_t)this + 0xEC);
	}

};

class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate; // Set to false when this var doesn't
										  // need Interpolate() called on it anymore.
	void* data;
	void* watcher;
};
struct VarMapping_t
{
	VarMapping_t()
	{
		m_nInterpolatedEntries = 0;
	}

	VarMapEntry_t* m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};

class C_BaseEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable
{
private:
    template<class T>
    T GetPointer(const int offset)
    {
        return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
    }
    // To get value from the pointer itself
    template<class T>
    T GetValue(const int offset)
    {
        return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
    }

	template <typename T>
	T& SetValue(uintptr_t offset)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

public:
	OFFSET(int, Effects, 0xF0);

	datamap_t* GetDataDescMap()
	{
		typedef datamap_t* (__thiscall* o_GetPredDescMap)(void*);
		return Utils::GetVFunc<o_GetPredDescMap>(this, 15)(this);
	}

	datamap_t* GetPredDescMap()
	{
		typedef datamap_t* (__thiscall* o_GetPredDescMap)(void*);
		return Utils::GetVFunc<o_GetPredDescMap>(this, 17)(this);
	}

	VarMapping_t* GetVarMap()
	{
		return reinterpret_cast<VarMapping_t*>((DWORD)this + 0x24);
	}

	std::array< float, 24 >& PoseParamters()
	{
		static int _m_flPoseParameter = g_pNetvars->GetOffset("DT_BaseAnimating", "m_flPoseParameter");
		return *reinterpret_cast<std::array<float, 24>*>(reinterpret_cast<uintptr_t>(this) + _m_flPoseParameter);
	}

	C_AnimState *AnimState()
	{
		return *reinterpret_cast<C_AnimState**>(uintptr_t(this) + 0x3914);
	}

	AnimationLayer *AnimOverlays()
	{
		return *reinterpret_cast<AnimationLayer**>(uintptr_t(this) + 0x2990);
	}

	AnimationLayer* GetAnimOverlay(int i)
	{
		if (i < 15)
			return &AnimOverlays()[i];
		return nullptr;
	}

	int NumOverlays()
	{
		return 15;
	}

	void UpdateClientAnimation()
	{
		Utils::GetVFunc<void(__thiscall*)(void*)>(this, 223)(this);
	}

	bool& ClientSideAnimation()
	{
		static int m_bClientSideAnimation = g_pNetvars->GetOffset("DT_BaseAnimating", "m_bClientSideAnimation");
		return *reinterpret_cast<bool*>(uintptr_t(this) + m_bClientSideAnimation);
	}

	int GetSequence()
	{
		static int m_nSequence = g_pNetvars->GetOffset("DT_BaseAnimating", "m_nSequence");
		return GetValue<int>(m_nSequence);
	}

	void SetSequence(int Sequence)
	{
		static int m_nSequence = g_pNetvars->GetOffset("DT_BaseAnimating", "m_nSequence");
		*reinterpret_cast<int*>(uintptr_t(this) + m_nSequence) = Sequence;
	}

	void SimulatedEveryTick(bool value)
	{
		static int m_bSimulatedEveryTick = g_pNetvars->GetOffset("DT_BaseEntity", "m_bSimulatedEveryTick");
		*reinterpret_cast<bool*>(uintptr_t(this) + m_bSimulatedEveryTick) = value;
	}

	void SetAbsAngles(Vector angles);
	void SetAbsOrigin(Vector origin);

	void InvalidateBoneCache();

	void CreateAnimState(C_AnimState* State);
	void UpdateAnimState(C_AnimState* state, Vector angle);

	int GetSequenceActivity(int Sequence);

	int GetActiveWeaponIndex()
	{
		static int m_hActiveWeapon = g_pNetvars->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
		return *reinterpret_cast<int*>(uintptr_t(this) + m_hActiveWeapon) & 0xFFF;
	}

	ClientClass* GetClientClass()
	{
		void* Networkable = (void*)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(void*);
		return Utils::GetVFunc<OriginalFn>(Networkable, 2)(Networkable);
	}

	AnimationLayer* GetAnimOverlayNumber(int i)
	{
		if (i < 16)
			return &AnimOverlays()[i];
	}

	void SetAnimLayers(AnimationLayer* layers) 
	{
		std::memcpy(AnimOverlays(), layers, sizeof(AnimationLayer) * 13);
	}

	Vector GetAbsOrigin()
	{
		return Utils::GetVFunc<Vector&(__thiscall*)(void*)>(this, 10)(this);
	}

	bool& JiggleBones()
	{
		return *(bool*)((DWORD)this + 0x292C);
	}

	void SetAbsVelocity(Vector velocity);

    C_BaseCombatWeapon* GetActiveWeapon()
    {
        static int m_hActiveWeapon = g_pNetvars->GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
        const auto weaponData      = GetValue<CBaseHandle>(m_hActiveWeapon);
        return reinterpret_cast<C_BaseCombatWeapon*>(g_pEntityList->GetClientEntityFromHandle(weaponData));
    }

    int GetTeam()
    {
        static int m_iTeamNum = g_pNetvars->GetOffset("DT_BaseEntity", "m_iTeamNum");
        return GetValue<int>(m_iTeamNum);
    }

	float SetSimulationTime(float simtime)
	{
		static int m_flSimulationTime = g_pNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime");
		return *(float*)((DWORD)this + m_flSimulationTime) = simtime;
	}

	int& GetFlags()
	{
		static int m_fFlags = g_pNetvars->GetOffset("DT_BasePlayer", "m_fFlags");
		return *(int*)((DWORD)this + m_fFlags);
	}

	uint32_t& GetEFlags()
	{
		static unsigned int _m_iEFlags = Utils::FindInDataMap(GetPredDescMap(), "m_iEFlags");
		return *(uint32_t*)((uintptr_t)this + _m_iEFlags);
	}

    MoveType_t GetMoveType()
    {
        static int m_Movetype = g_pNetvars->GetOffset("DT_BaseEntity", "m_nRenderMode") + 1;
        return GetValue<MoveType_t>(m_Movetype);
    }

	float GetSimulationTime()
	{
		static int m_flSimulationTime = g_pNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime");
		return GetValue<float>(m_flSimulationTime);
	}

	float GetOldSimulationTime()
	{
		static int m_flOldSimulationTime = g_pNetvars->GetOffset("DT_BaseEntity", "m_flSimulationTime") + 4;
		return GetValue<float>(m_flOldSimulationTime);
	}

	float GetLowerBodyYaw()
	{
		static int m_flLowerBodyYawTarget = g_pNetvars->GetOffset("DT_CSPlayer", "m_flLowerBodyYawTarget");
		return GetValue<float>(m_flLowerBodyYawTarget);
	}

	float& GetDuckAmt()
	{
		static int m_flDuckAmount = g_pNetvars->GetOffset("DT_BasePlayer", "m_flDuckAmount");
		return *(float*)((uintptr_t)this + m_flDuckAmount);
	}

	void SetLowerBodyYaw(float value)
	{
		static int m_flLowerBodyYawTarget = g_pNetvars->GetOffset("DT_CSPlayer", "m_flLowerBodyYawTarget");
		*reinterpret_cast<float*>(uintptr_t(this) + m_flLowerBodyYawTarget) = value;
	}

	bool GetHeavyArmor()
	{
		static int m_bHasHeavyArmor = g_pNetvars->GetOffset("DT_CSPlayer", "m_bHasHeavyArmor");
		return GetValue<bool>(m_bHasHeavyArmor);
	}

	int ArmorValue()
	{
		static int m_ArmorValue = g_pNetvars->GetOffset("DT_CSPlayer", "m_ArmorValue");
		return GetValue<int>(m_ArmorValue);
	}

	bool HasHelmet()
	{
		static int m_bHasHelmet = g_pNetvars->GetOffset("DT_CSPlayer", "m_bHasHelmet");
		return GetValue<bool>(m_bHasHelmet);
	}

    bool GetLifeState()
    {
        static int m_lifeState = g_pNetvars->GetOffset("DT_BasePlayer", "m_lifeState");
        return GetValue<bool>(m_lifeState);
    }

	int GetMoney()
	{
		static int m_iAccount = g_pNetvars->GetOffset("DT_CSPlayer", "m_iAccount");
		return *reinterpret_cast<int*>(uintptr_t(this) + m_iAccount);
	}

	float& FlashDuration()
	{
		static int m_flFlashDuration = g_pNetvars->GetOffset("DT_CSPlayer", "m_flFlashDuration");
		return *(float*)((DWORD)this + m_flFlashDuration);
	}

	bool IsScoped()
	{
		static int m_bIsScoped = g_pNetvars->GetOffset("DT_CSPlayer", "m_bIsScoped");
		return GetValue<bool>(m_bIsScoped);
	}

    int GetHealth()
    {
        static int m_iHealth = g_pNetvars->GetOffset("DT_BasePlayer", "m_iHealth");
        return GetValue<int>(m_iHealth);
    }

	float GetSpawnTime()
	{
		return *(float*)((uintptr_t)this + 0xA370);
	}


	bool IsKnifeorNade();

	bool IsKnife();

	bool IsNade();

    bool IsAlive() { return this->GetHealth() > 0 && this->GetLifeState() == 0; }

	bool IsEnemy();

	bool SetupBonesAlt(VMatrix* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);

    bool IsImmune()
    {
        static int m_bGunGameImmunity = g_pNetvars->GetOffset("DT_CSPlayer", "m_bGunGameImmunity");
        return GetValue<bool>(m_bGunGameImmunity);
    }

	void SetObserverMode(int Mode)
	{
		static int m_iObserverMode = g_pNetvars->GetOffset("DT_BasePlayer", "m_iObserverMode");
		*(int*)((uintptr_t)this + m_iObserverMode) = Mode;
	}

	int GetObserverMode()
	{
		static int m_iObserverMode = g_pNetvars->GetOffset("DT_BasePlayer", "m_iObserverMode");
		return *(int*)((uintptr_t)this + m_iObserverMode);
	}

	int& GetTickBaseAlt()
	{
		static int m_nTickBase = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_nTickBase");
		return *(int*)((DWORD)this + m_nTickBase);
	}

    int GetTickBase()
    {
        static int m_nTickBase = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_nTickBase");
        return GetValue<int>(m_nTickBase);
    }

	int GetShotsFired()
	{
		static int m_iShotsFired = g_pNetvars->GetOffset("DT_CSPlayer", "cslocaldata", "m_iShotsFired");
		return GetValue<int>(m_iShotsFired);
	}

	void SetTickBase(int TickBase)
	{
		static int m_nTickBase = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_nTickBase");
		*reinterpret_cast<int*>(uintptr_t(this) + m_nTickBase) = TickBase;
	}

	Vector GetEyeAngles()
	{
		static int m_angEyeAngles = g_pNetvars->GetOffset("DT_CSPlayer", "m_angEyeAngles");
		return GetValue<Vector>(m_angEyeAngles);
	}

	Vector& GetAbsAngles()
	{
		typedef Vector& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)Utils::GetVFunc<OriginalFn>(this, 11))(this);
	}

	float* GetPoseParameter()
	{
		static int m_flPoseParameter = g_pNetvars->GetOffset("CBaseAnimating", "m_flPoseParameter");
		return reinterpret_cast<float*>(uintptr_t(this) + m_flPoseParameter);
	}

	void GetPoseParameters(float* Poses) 
	{
		std::memcpy(Poses, GetPoseParameter(), sizeof(float) * 24);
	}

	void SetPoseParameters(float* Poses)
	{
		std::memcpy(GetPoseParameter(), Poses, sizeof(float) * 24);
	}

	void SetEyeAngles(Vector Angle)
	{
		static int m_angEyeAngles = g_pNetvars->GetOffset("DT_CSPlayer", "m_angEyeAngles");
		*reinterpret_cast<Vector*>(uintptr_t(this) + m_angEyeAngles) = Angle;
	}

    Vector GetOrigin()
    {
        static int m_vecOrigin = g_pNetvars->GetOffset("DT_BaseEntity", "m_vecOrigin");
        return GetValue<Vector>(m_vecOrigin);
    }

	Vector GetOldOrigin()
	{
		static int m_vecOldOrigin = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_flFriction") + 12;
		return GetValue<Vector>(m_vecOldOrigin);
	}

	Vector GetNetworkOrigin()
	{
		static int m_vecNetworkOrigin = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_flFriction") + 40;
		return GetValue<Vector>(m_vecNetworkOrigin);
	}

	void SetOrigin(Vector Origin)
	{
		static int m_vecOrigin = g_pNetvars->GetOffset("DT_BaseEntity", "m_vecOrigin");
		*reinterpret_cast<Vector*>(uintptr_t(this) + m_vecOrigin) = Origin;
	}

	Vector GetVelocity()
	{
		static int m_vecVelocity = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_vecVelocity[0]");
		return GetValue<Vector>(m_vecVelocity);
	}

	Vector& GetAbsVelocity()
	{
		static unsigned int _m_vecAbsVelocity = Utils::FindInDataMap(GetPredDescMap(), "m_vecAbsVelocity");
		return *(Vector*)((uintptr_t)this + _m_vecAbsVelocity);
	}

	void SetVelocity(Vector velocity)
	{
		static int m_vecVelocity = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_vecVelocity[0]");
		*reinterpret_cast<Vector*>(uintptr_t(this) + m_vecVelocity) = velocity;
	}

	bool HasC4()
	{
		static auto HasC4 = reinterpret_cast<bool(__thiscall*)(void*)>(Utils::FindSignature(XorStr("client.dll"), XorStr("56 8B F1 85 F6 74 31")));
		return HasC4(this);
	}

	bool IsFlashed()
	{
		static auto m_flFlashMaxAlpha = g_pNetvars->GetOffset(XorStr("DT_CSPlayer"), XorStr("m_flFlashMaxAlpha"));
		return *(float*)((uintptr_t)this + m_flFlashMaxAlpha - 0x8) > 200.0;
	}

	char* GetArmorName()
	{
		if (ArmorValue() > 0)
		{
			if (HasHelmet())
				return "HK";
			else
				return "K";
		}
		else
			return " ";
	}

	Vector GetAimPunchAngle()
	{
		return *reinterpret_cast<Vector*>(uintptr_t(this) + uintptr_t(0x302C));
	}

	Vector GetViewPunchAngle()
	{
		return *reinterpret_cast<Vector*>(uintptr_t(this) + uintptr_t(0x3020));
	}

	float& GetVelocityModifier()
	{
		return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0xA38C);
	}

    Vector GetEyePosition() 
	{
		Vector ret;
		typedef void(__thiscall *OrigFn)(void *, Vector&);
		Utils::GetVFunc<OrigFn>(this, 284)(this, ret);
		return ret;
	}

	ICollideable* GetCollideable()
	{
		static int m_Collision = g_pNetvars->GetOffset("DT_BasePlayer", "m_Collision");
		return (ICollideable*)((DWORD)this + m_Collision);
	}

	void SetCurrentCommand(CUserCmd *cmd)
	{
		static int m_hConstraintEntity = g_pNetvars->GetOffset("DT_BasePlayer", "localdata", "m_hConstraintEntity");
		*reinterpret_cast<CUserCmd**>(uintptr_t(this) + m_hConstraintEntity - 0xC) = cmd;
	}

	void SetAbsoluteAngles(const Vector& Angle);

	float FireRate();

	void FixSetupBones(matrix3x4_t*Matrix);
	Vector GetHitboxPosition(int Hitbox, matrix3x4_t *Matrix, float *Radius);
	Vector GetHitboxPosition(int Hitbox, matrix3x4_t *Matrix);
	Vector GetBonePos(int iBone);
};


class C_BaseCombatWeapon : public C_BaseEntity
{
private:
    template<class T>
    T GetPointer(const int offset)
    {
        return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
    }
    // To get value from the pointer itself
    template<class T>
    T GetValue(const int offset)
    {
        return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) + offset);
    }

public:

	bool IsC4();
	bool IsGun();

    ItemDefinitionIndex GetItemDefinitionIndex()
    {
        static int m_iItemDefinitionIndex = g_pNetvars->GetOffset("DT_BaseAttributableItem", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
        return GetValue<ItemDefinitionIndex>(m_iItemDefinitionIndex);
    }

	float GetPostponeFireReadyTime()
	{
		static int m_flPostponeFireReadyTime = g_pNetvars->GetOffset("DT_WeaponCSBase", "m_flPostponeFireReadyTime");
		return *reinterpret_cast<float*>(uintptr_t(this) + m_flPostponeFireReadyTime);
	}

	float GetInaccuracy()
	{
		typedef float(__thiscall* oInaccuracy)(PVOID);
		return Utils::GetVFunc< oInaccuracy >(this, 482)(this);
	}

	float GetSpread()
	{
		typedef float(__thiscall* oWeaponSpread)(PVOID);
		return Utils::GetVFunc< oWeaponSpread >(this, 452)(this);
	}

	float GetAccuracyPenalty()
	{
		static int m_fAccuracyPenalty = g_pNetvars->GetOffset("DT_WeaponCSBase", "m_fAccuracyPenalty");
		return GetValue<float>(m_fAccuracyPenalty);
	}

	float GetLastShotTime()
	{
		static int m_fLastShotTime = g_pNetvars->GetOffset("DT_WeaponCSBase", "m_fLastShotTime");
		return GetValue<float>(m_fLastShotTime);
	}

	void AccuracyPenalty()
	{
		typedef void(__thiscall *OrigFn)(void *);
		return Utils::GetVFunc<OrigFn>(this, 483)(this);
	}

    float GetNextPrimaryAttack()
    {
        static int m_flNextPrimaryAttack = g_pNetvars->GetOffset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
        return GetValue<float>(m_flNextPrimaryAttack);
    }

	float GetNextSecondaryAttack()
	{
		static int m_flNextSecondaryAttack = g_pNetvars->GetOffset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextSecondaryAttack");
		return GetValue<float>(m_flNextSecondaryAttack);
	}

    int GetAmmo()
    {
        static int m_iClip1 = g_pNetvars->GetOffset("DT_BaseCombatWeapon", "m_iClip1");
        return GetValue<int>(m_iClip1);
    }

	float GetMaxSpeed()
	{
		typedef float(__thiscall* OrigFn)(C_BaseCombatWeapon*);
		return Utils::GetVFunc<OrigFn>(this, 350)(this);
	}

    WeaponInfo_t* GetCSWpnData()
    {
        return Utils::CallVFunc<460, WeaponInfo_t*>(this);
    }

    std::string GetName()
    {
        return std::string(this->GetCSWpnData()->weapon_name);
    }
};

class CBaseCSGrenade : C_BaseCombatWeapon
{
public:
	float PinPulled()
	{
		static int m_bPinPulled = g_pNetvars->GetOffset("DT_BaseCSGrenade", "m_bPinPulled");
		return *reinterpret_cast<float*>(uintptr_t(this) + m_bPinPulled);
	}

	float ThrowTime()
	{
		static int m_fThrowTime = g_pNetvars->GetOffset("DT_BaseCSGrenade", "m_fThrowTime");
		return *reinterpret_cast<float*>(uintptr_t(this) + m_fThrowTime);
	}
};