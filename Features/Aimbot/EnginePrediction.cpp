#include "EnginePrediction.h"
#include "..\..\SDK\CEntity.h"
#include "..\..\SDK\ClientState.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\Menu\Menu.h"

EnginePrediction g_EnginePrediction;

float OldCurtime;
float OldFrametime;

void EnginePrediction::RunEnginePred(CUserCmd* CMD)
{
    static int TickBase;
    static CUserCmd* LastCmd;

    /* fix tickbase if game didnt render previous tick */
    if (LastCmd) {
        if (LastCmd->hasbeenpredicted)
            TickBase = g::pLocalEntity->GetTickBase();
        else
            ++TickBase;
    }

    LastCmd = CMD;
    OldCurtime = g_pGlobalVars->curtime;
    OldFrametime = g_pGlobalVars->frametime;

    g_pGlobalVars->curtime = TickBase * g_pGlobalVars->intervalPerTick;
    g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick;

    g_pMovement->StartTrackPredictionErrors(g::pLocalEntity);

    CMoveData Data;
    memset(&Data, 0, sizeof(CMoveData));

    g_pMoveHelper->SetHost(g::pLocalEntity);
    g_pPrediction->SetupMove(g::pLocalEntity, CMD, g_pMoveHelper, &Data);
    g_pMovement->ProcessMovement(g::pLocalEntity, &Data);
    g_pPrediction->FinishMove(g::pLocalEntity, CMD, &Data);
}

void EnginePrediction::EndEnginePred(CUserCmd* CMD)
{
    g_pMovement->FinishTrackPredictionErrors(g::pLocalEntity);
    g_pMoveHelper->SetHost(nullptr);

    g_pGlobalVars->curtime = OldCurtime;
    g_pGlobalVars->frametime = OldFrametime;
}