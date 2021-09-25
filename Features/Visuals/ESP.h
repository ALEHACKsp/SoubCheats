#pragma once
#include "..\..\Utils\GlobalVars.h"

class ESP
{
public:
public:
	void Draw();
	void DrawBox(C_BaseEntity* Entity, Color Color);
	void DrawHealth(C_BaseEntity* Entity, Color CColor);
	void DrawName(C_BaseEntity* Entity, Color CColor, int Index);
	void DrawWeapon(C_BaseEntity* Entity, Color CColor, int Index);
	void DrawAmmo(C_BaseEntity* Entity, Color CColor);
	void DrawLby(C_BaseEntity* Entity, Color CColor);
	void DrawFlags(C_BaseEntity* Entity);
	void DrawNades(C_BaseCombatWeapon* Grenade);

	void RemoveFlash();
	void RemoveSmoke();
	void RemoveScope();

	bool WorldToScreen(const Vector& Origin, Vector& Screen);
	bool GetBox(C_BaseEntity* Entity, int& x, int& y, int& w, int& h);

	IMaterial* CreateMaterial(bool Shade, bool Wireframe, bool Ignorez, int RimlightBoost = 0);
	void DrawChams();
	void DrawGlow();

	float PlayerAlpha[255];
	float FadeAlpha[65];
	float DormantTime[65];

	Color BoxColor;
	Color NameColor;
	Color WeaponColor;
	Color AmmoColor;
	Color SkeletonColor;
	Color HealthColor;
	Color LBYColor;

	Color NadeColor;

	int BoxX;
	int BoxY;
	int BoxW;
	int BoxH;
	float Alpha = 0.f;
	float RealAlpha = 0.f;
};
extern ESP g_ESP;