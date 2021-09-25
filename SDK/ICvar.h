#pragma once
#include "../Utils/Utils.h"

class ConVar
{
public:
	//void SetString(const char *pValue)
	//{
	//	typedef void(__thiscall* SetStringFn)(void*, const char *);
	//	call_vfunc<SetStringFn>(this, 17)(this, pValue);
	//}

	void SetValue(const char* str)
	{
		typedef void(__thiscall* Fn)(void*, const char*);
		return Utils::GetVFunc<Fn>(this, 13)(this, str);
	}
	void SetValue(float value)
	{
		typedef void(__thiscall* Fn)(void*, float);
		return Utils::GetVFunc<Fn>(this, 15)(this, value);
	}
	void SetValue(int value)
	{
		typedef void(__thiscall* Fn)(void*, int);
		return Utils::GetVFunc<Fn>(this, 16)(this, value);
	}
	void SetValueChar(const char* name)
	{
		typedef void(__thiscall* Fn)(void*, const char*);
		return Utils::GetVFunc<Fn>(this, 14)(this, name);
	}
	void InternalSetString(const char* str)
	{
		typedef void(__thiscall* Fn)(void*, const char*);
		return Utils::GetVFunc<Fn>(this, 17)(this, str);
	}
	char* GetBaseName()
	{
		typedef char*(__thiscall* Fn)(void*);
		return Utils::GetVFunc<Fn>(this, 6)(this);
	}
	float GetFloat()
	{
		typedef float(__thiscall* Fn)(void*);
		return Utils::GetVFunc< Fn >(this, 12)(this);
	}
	int GetInt(void) const {
		return pParent->nValue;
	}
	const char* GetString(void) const {
		return pParent->pszDefaultValue;
	}

	bool GetBool() { return !!GetInt(); }

	char pad_0x0000[0x4]; //0x0000
	ConVar* pNext; //0x0004 
	__int32 bRegistered; //0x0008 
	char* pszName; //0x000C 
	char* pszHelpString; //0x0010 
	__int32 nFlags; //0x0014 
	char pad_0x0018[0x4]; //0x0018
	ConVar* pParent; //0x001C 
	char* pszDefaultValue; //0x0020 
	char* strString; //0x0024 
	__int32 StringLength; //0x0028 
	float fValue; //0x002C 
	__int32 nValue; //0x0030 
	__int32 bHasMin; //0x0034 
	float fMinVal; //0x0038 
	__int32 bHasMax; //0x003C 
	float fMaxVal; //0x0040 
	void *fnChangeCallback; //0x0044 


};//Size=0x0048


class IAppSystem
{
public:
	virtual ~IAppSystem()
	{
	}

	virtual void func0() = 0;
	virtual void func1() = 0;
	virtual void func2() = 0;
	virtual void func3() = 0;
	virtual void func4() = 0;
	virtual void func5() = 0;
	virtual void func6() = 0;
	virtual void func7() = 0;
	virtual void func8() = 0;
	virtual void func9() = 0;
};

struct CVarDLLIdentifier_t;

class ConCommandBase
{
public:
	PVOID pVMTBase; // 0x00

	ConCommandBase* m_pNext; // 0x04
	bool m_bRegistered; // 0x08
	const char* m_pszName; // 0x0C
	const char* m_pszHelpString; // 0x10
	int m_nFlags; // 0x14

	ConCommandBase* s_pConCommandBases; // 0x18
	void* s_pAccessor; // 0x1C
}; // Size: 0x0020
class ConCommand;

class ICVar
{
public:
	template <typename... Values>
	void Consolecolor_tPrintf(const Color& Msgcolor_t, const char* szMsgFormat, Values... Parameters)
	{
		typedef void(*oConsolecolor_tPrintf)(void*, const color_t&, const char*, ...);
		return Utils::GetVFunc<oConsolecolor_tPrintf>(this, 25)(this, Msgcolor_t, szMsgFormat, Parameters...);
	}

	void UnregisterConCommand(ConVar* pCommandBase)
	{
		typedef ConVar* (__thiscall* FindVarFn)(void*, ConVar*);
		Utils::GetVFunc< FindVarFn >(this, 13)(this, pCommandBase);
	}

	ConVar* FindVar(const char* var_name)
	{
		typedef ConVar* (__thiscall* FindVarFn)(void*, const char*);
		return Utils::GetVFunc< FindVarFn >(this, 16)(this, var_name);
	}

	void RegisterConCommand(ConVar* pCommandBase)
	{
		typedef ConVar* (__thiscall* FindVarFn)(void*, ConVar*);
		Utils::GetVFunc< FindVarFn >(this, 12)(this, pCommandBase);
	}
};

extern ICVar* g_pCvar;