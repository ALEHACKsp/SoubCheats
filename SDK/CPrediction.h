#pragma once
#include "..\Utils\Utils.h"

#define MAX_SPLITSCREEN_PLAYERS 1

class CBaseHandle;
class C_BaseEntity;
class IClientEntity;
class CUserCmd;

class IMoveHelper
{
public:
    void SetHost(C_BaseEntity* host)
    {
        return Utils::CallVFunc<1, void>(this, host);
    }
};
extern IMoveHelper* g_pMoveHelper;

struct CMoveData { byte data[184]; };

class IGameMovement
{
public:
    virtual			~IGameMovement(void) {}

    virtual void	ProcessMovement(C_BaseEntity *pPlayer, CMoveData *pMove) = 0;
    virtual void	Reset(void) = 0;
    virtual void	StartTrackPredictionErrors(C_BaseEntity *pPlayer) = 0;
    virtual void	FinishTrackPredictionErrors(C_BaseEntity *pPlayer) = 0;
    virtual void	DiffPrint(char const *fmt, ...) = 0;

    virtual Vector const&	GetPlayerMins(bool ducked) const = 0;
    virtual Vector const&	GetPlayerMaxs(bool ducked) const = 0;
    virtual Vector const&   GetPlayerViewOffset(bool ducked) const = 0;

    virtual bool			IsMovingPlayerStuck(void) const = 0;
    virtual C_BaseEntity*	GetMovingPlayer(void) const = 0;
    virtual void			UnblockPusher(C_BaseEntity* pPlayer, C_BaseEntity *pPusher) = 0;

    virtual void    SetupMovementBounds(CMoveData *pMove) = 0;
};
extern IGameMovement* g_pMovement;

class CPrediction
{
    // Construction
public:
    bool InPrediction()
    {
        typedef bool(__thiscall* oInPrediction)(void*);
        return Utils::GetVFunc<oInPrediction>(this, 14)(this);
    }

    void RunCommand(C_BaseEntity* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
    {
        typedef void(__thiscall* oRunCommand)(void*, C_BaseEntity*, CUserCmd*, IMoveHelper*);
        return Utils::GetVFunc<oRunCommand>(this, 19)(this, player, ucmd, moveHelper);
    }

    void SetupMove(C_BaseEntity* player, CUserCmd* ucmd, IMoveHelper* moveHelper, void* pMoveData)
    {
        typedef void(__thiscall* oSetupMove)(void*, C_BaseEntity*, CUserCmd*, IMoveHelper*, void*);
        return Utils::GetVFunc<oSetupMove>(this, 20)(this, player, ucmd, moveHelper, pMoveData);
    }

    void FinishMove(C_BaseEntity* player, CUserCmd* ucmd, void* pMoveData)
    {
        typedef void(__thiscall* oFinishMove)(void*, C_BaseEntity*, CUserCmd*, void*);
        return Utils::GetVFunc<oFinishMove>(this, 21)(this, player, ucmd, pMoveData);
    }
};

extern CPrediction* g_pPrediction;