#pragma once

#include "Aimbot\EnginePrediction.h"
#include "Visuals\ESP.h"
#include "Misc\Misc.h"
#include "Resolver\Resolver.h"
#include "AntiAim\AntiAim.h"
#include "Aimbot\Aimbot.h"
#include "Legit\Legitbot.h"
#include "Aimbot/LagComp.h"
#include "Animations/AnimationFix.h"

class Features
{
public:
	void CreateMoveFeatures(CUserCmd* CMD);
private:
};

extern Features g_Features;