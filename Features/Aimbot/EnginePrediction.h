#pragma once
#include "../../SDK/CInput.h"
#include "../../SDK/Vector.h"
#include "..\..\SDK\CInput.h"
#include <array>

class EnginePrediction
{
public:
    void RunEnginePred(CUserCmd* CMD);
    void EndEnginePred(CUserCmd* CMD);
};

extern EnginePrediction g_EnginePrediction;