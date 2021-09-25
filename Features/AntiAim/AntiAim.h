#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"

class AntiAim
{
public:
	void FakeLag();
	void RunAntiAim(CUserCmd* CMD);
	bool Disable(CUserCmd* CMD);
	float GetMaxDesyncDelta();
private:

};
extern AntiAim g_AntiAim;