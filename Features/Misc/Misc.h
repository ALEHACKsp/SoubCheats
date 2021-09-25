#pragma once
#include "../../SDK/Vector.h"
#include "../../SDK/CInput.h"
#include "../../SDK/IClientMode.h"

class Misc
{
public:
	void Bhop();
	void AutoStrafe(Vector& Original);
	void MovementFix(Vector& Original);
	void Clantag();
	void DisablePostProcessing();
	void RevealHiddenCvars();
	void ModifyViewModel();
	void EdgeJump(CUserCmd* CMD);
	void RemoveShadows();

	void ReturnServerType();
	void Thirdperson(CViewSetup* pSetup);
	void DisableInterp();
	void FixAnimLOD();

	float RedianToDegrees(float ang) 
	{
		return (ang * 180.0f) / M_PI;
	}

	float NormalizeAng(float ang) 
	{
		while (ang < -180.0f) ang += 360.0f;
		while (ang > 180.0f) ang -= 360.0f;
		return ang;
	}

	void NormalizeVector(Vector& angle)
	{
		for (auto i = 0; i < 3; i++) 
		{
			while (angle[i] < -180.0f) angle[i] += 360.0f;
			while (angle[i] > 180.0f) angle[i] -= 360.0f;
		}
	}

	int OldState = -1;
	bool WasDeadInThirdperson = false;
private:
	void MultiDirStrafer(Vector& Original);
	void StaticStrafer(Vector& Original);
};
extern Misc g_Misc;