#pragma once
#include <Windows.h>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct ColorV2
{
	float red, green, blue, alpha;
};


namespace Keyhandler {
	bool IsKeyDown(int Key);
	bool IsKeyUp(int Key);
	bool IsKeyPressed(int Key);

	bool AutoCheck(int KeyStyle, int Key);

	static bool Keys[256] = { false };
	static bool Pressed[256] = { false };
	static bool Toggles[256] = { false };
};

class Menu
{
public:
	bool menuOpened = true;
	void Render();
	void CreateStyle();

	struct
	{
		/* ESP */
		bool EnableESP = false;
		bool EnableFadeESP = false;

		bool NadeESP = false;

		bool BoxESP = false;
		bool NameESP = false;
		int  WeaponESPType = 0;
		bool WeaponESP = false;
		bool HealthESP = false;
		bool HealthCustomColor = false;
		bool AmmoESP = false;
		bool InfoESP = false;
		bool LbyESP = false;

		bool InfoMoney = false;
		bool InfoArmor = false;
		bool InfoZoom = false;
		bool InfoBombCarrier = false;
		bool InfoFlashed = false;
		bool InfoImmune = false;
		bool InfoLethal = false;

		bool EnableRemovals = false;
		bool RemoveScope = false;
		bool RemoveZoom = false;
		bool RemoveAimPunch = false;
		bool RemoveSmoke = false;
		bool RemoveFlash = false;
		bool RemoveSleeves = false;
		bool RemoveShadows = false;

		bool FakeChams = false;

		float ScopeLine[4] = { 0.f, 0.f, 0.f, 1.f };

		float BoxColor[3] = { 1.f, 1.f, 1.f };
		float NameColor[3] = { 1.f, 1.f, 1.f };
		float WeaponColor[3] = { 1.f, 1.f, 1.f };
		float AmmoColor[3] = { 0.f, 0.5f, 1.f };
		float SkeletonColor[3] = { 1.f, 1.f, 1.f };
		float HealthColor[3] = { 1.f, 1.f, 1.f };
		float LBYColor[3] = { 0.4f, 0.f, 1.f };

		bool EnableChams = false;
		bool EnableHidChams = false;

		bool EnablePlayerGlow = false;
		float PlayerGlowColor[4] = { 1.f, 1.f, 1.f, 0.5f };

		float VisChamsColor[4] = { 0.4f, 0.f, 1.f, 1.f };
		float HidChamsColor[4] = { 0.f, 0.5f, 1.f, 1.f };

		int ChamsMat = 0;

		int DebugFov = 90;
		int DebugViewFov = 68;

		bool DebugModel = false;

		/* Misc */
		bool AntiUntrusted = true;
		bool Bhop = false;

		bool EnableLegitBhop = false;
		int BhopHitchance = 0;
		int BhopsRestricted = 0;
		int MaxBhops = 0;

		bool AutoStrafe = false;
		int AutoStrafeType = 0;

		bool Thirdperson = false;
		int ThirdpersonKey = 0;
		int ThirdpersonKeyType = 0;
		int ThirdpersonDist = 150;
		bool ThirdpersonWhileSpec = false;

		bool EdgeJump = false;
		int EdgeJumpKey = 0;
		int EdgeJumpKeyType = 0;

		bool Clantag = false;

		bool DisablePostEffects = false;
		bool RevealHiddenCvars = false;

		bool ForceCheats = false;
		bool FastDuck = false;

		bool AutoPistol = false;

		bool AllowViewModify = false;
		int ViewOffsetX = 0;
		int ViewOffsetY = 0;
		int ViewOffsetZ = 0;

		bool BuyBotEnable = false;
		bool BuyMisc[4];
		int BuyBotWeapons = 0;
		int BuyBotPistols = 0;

		bool BuyFlash = false;
		bool BuyDecoy = false;

		int ConfigList = 0;
	
		/* AIM */

		bool EnableRagebot = false;
		int RagebotKey = 0;
		int RagebotKeyType = 0;

		int RagebotFov = 180;

		int TargetMode = 0;

		bool SilentAim = false;
		bool NoAimRecoil = false;

		int HitboxesToScan = 0;

		int Hitchance = 0;
		int MinDamage = 0;

		int Headscale = 0;
		int Bodyscale = 0;

		bool AutoScope = false;
		bool AutoStop = false;
		bool StopBetweenShots = false;

		bool DontShootLagComp = false;

		bool DoubleTap = false;
		bool DoubleTapInstant = false;
		bool DoubleTapTeleport = false;
		int DoubleTapKey = 0;
		int DoubleTapKeyType = 0;

		/* AA */
		bool EnableAntiAim = false;

		bool EnableFakeLag = false;
		int FakelagAmmount = 0;

	}Config;
private:
};

extern Menu g_Menu;