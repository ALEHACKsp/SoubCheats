#include "ESP.h"
#include "..\..\Security\Xor.h"
#include "..\Aimbot\Aimbot.h"
#include "..\Aimbot\LagComp.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\SDK\ISurface.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\SDK\IMaterial.h"
#include "..\..\SDK\IVModelInfo.h"
#include "..\..\SDK\IVRenderView.h"
#include "..\..\SDK\CGlowObjectManager.h"
#include "..\..\SDK\ClientClass.h"
#include "..\..\SDK\CModelRender.h"
#include "..\..\Utils\Math.h"
#include "..\..\Menu\Menu.h"

#include <locale>
#include "../../SDK/IClientMode.h"

ESP g_ESP;

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

char GetWeaponIcon(C_BaseCombatWeapon* Weapon)
{
	auto id = Weapon->GetItemDefinitionIndex();
	switch (id)
	{
	case ItemDefinitionIndex::WEAPON_DEAGLE:
	case ItemDefinitionIndex::WEAPON_REVOLVER:
		return 'F';
	case ItemDefinitionIndex::WEAPON_ELITE:
		return 'S';
	case ItemDefinitionIndex::WEAPON_FIVESEVEN:
		return 'U';
	case ItemDefinitionIndex::WEAPON_GLOCK:
	case ItemDefinitionIndex::WEAPON_TEC9:
		return 'C';
	case ItemDefinitionIndex::WEAPON_AK47:
		return 'B';
	case ItemDefinitionIndex::WEAPON_AUG:
		return 'E';
	case ItemDefinitionIndex::WEAPON_AWP:
		return 'R';
	case ItemDefinitionIndex::WEAPON_FAMAS:
		return 'T';
	case ItemDefinitionIndex::WEAPON_G3SG1:
	case ItemDefinitionIndex::WEAPON_SCAR20:
		return 'I';
	case ItemDefinitionIndex::WEAPON_GALILAR:
		return 'V';
	case ItemDefinitionIndex::WEAPON_M249:
		return 'Z';
	case ItemDefinitionIndex::WEAPON_M4A1:
	case ItemDefinitionIndex::WEAPON_M4A1_SILENCER:
		return 'W';
	case ItemDefinitionIndex::WEAPON_MAC10:
		return 'L';
	case ItemDefinitionIndex::WEAPON_P90:
		return 'M';
	case ItemDefinitionIndex::WEAPON_UMP45:
		return 'Q';
	case ItemDefinitionIndex::WEAPON_XM1014:
		return ']';
	case ItemDefinitionIndex::WEAPON_BIZON:
	case ItemDefinitionIndex::WEAPON_MP9:
		return 'D';
	case ItemDefinitionIndex::WEAPON_MAG7:
	case ItemDefinitionIndex::WEAPON_SAWEDOFF:
	case ItemDefinitionIndex::WEAPON_NOVA:
		return 'K';
	case ItemDefinitionIndex::WEAPON_NEGEV:
		return 'Z';
	case ItemDefinitionIndex::WEAPON_USP_SILENCER:
	case ItemDefinitionIndex::WEAPON_TASER:
	case ItemDefinitionIndex::WEAPON_HKP2000:
	case ItemDefinitionIndex::WEAPON_P250:
	case ItemDefinitionIndex::WEAPON_CZ75A:
		return 'Y';
	case ItemDefinitionIndex::WEAPON_MP7:
		return 'X';
	case ItemDefinitionIndex::WEAPON_SG556:
		return '[';
	case ItemDefinitionIndex::WEAPON_SSG08:
		return 'N';
	case ItemDefinitionIndex::WEAPON_FLASHBANG:
	case ItemDefinitionIndex::WEAPON_DECOY:
		return 'G';
	case ItemDefinitionIndex::WEAPON_HEGRENADE:
	case ItemDefinitionIndex::WEAPON_MOLOTOV:
	case ItemDefinitionIndex::WEAPON_INCGRENADE:
		return 'H';
	case ItemDefinitionIndex::WEAPON_SMOKEGRENADE:
		return 'P';
	case ItemDefinitionIndex::WEAPON_C4:
		return '\\';
	case ItemDefinitionIndex::WEAPON_KNIFE:
	case ItemDefinitionIndex::WEAPON_KNIFE_T:
	case ItemDefinitionIndex::WEAPON_KNIFE_BAYONET:
	case ItemDefinitionIndex::WEAPON_KNIFE_FLIP:
	case ItemDefinitionIndex::WEAPON_KNIFE_GUT:
	case ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT:
	case ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET:
	case ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL:
	case ItemDefinitionIndex::WEAPON_KNIFE_FALCHION:
	case ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE:
	case ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY:
	case ItemDefinitionIndex::WEAPON_KNIFE_PUSH:
		return 'J';
	}
}

Color GetHealthColor(C_BaseEntity* Entity)
{
	const float Multiplier = 2.55;

	int Red, Green, Blue, Alpha;
	int Health = Entity->GetHealth();

	Red = 255 - (Health * Multiplier);
	Green = Health * Multiplier;
	Blue = 0;
	Alpha = 255;

	Color Color(Red, Green, Blue, Alpha);

	return Color;
}

bool ESP::WorldToScreen(const Vector& Origin, Vector& Screen)
{
	auto W2S = [&Origin, &Screen]() -> bool {
		static uintptr_t ViewMatrix = 0;
		if (!ViewMatrix) {
			ViewMatrix = static_cast<std::uintptr_t>(Utils::FindSignature(XorStr("client.dll"), XorStr("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9")));
			ViewMatrix = *reinterpret_cast<uintptr_t*>(ViewMatrix + 0x3) + 176;
		}

		const matrix3x4_t& Matrix = *(matrix3x4_t*)ViewMatrix;
		Screen.x = Matrix[0][0] * Origin[0] + Matrix[0][1] * Origin[1] + Matrix[0][2] * Origin[2] + Matrix[0][3];
		Screen.y = Matrix[1][0] * Origin[0] + Matrix[1][1] * Origin[1] + Matrix[1][2] * Origin[2] + Matrix[1][3];

		float W = Matrix[3][0] * Origin[0] + Matrix[3][1] * Origin[1] + Matrix[3][2] * Origin[2] + Matrix[3][3];

		if (W < 0.001f) {
			return true;
		}

		float invw = 1.0f / W;
		Screen.x *= invw;
		Screen.y *= invw;

		return false;
	};

	W2S();

	int w, h;
	g_pEngine->GetScreenSize(w, h);
	Screen.x = (w * .5f) + (Screen.x * w) * .5f;
	Screen.y = (h * .5f) - (Screen.y * h) * .5f;

	return true;
}

bool ESP::GetBox(C_BaseEntity* Entity, int& x, int& y, int& w, int& h)
{
	Vector vOrigin, min, max, flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	vOrigin = Entity->GetAbsOrigin();
	min = Entity->GetCollideable()->OBBMins() + vOrigin;
	max = Entity->GetCollideable()->OBBMaxs() + vOrigin;

	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	if (!WorldToScreen(points[3], flb) || !WorldToScreen(points[5], brt)
		|| !WorldToScreen(points[0], blb) || !WorldToScreen(points[4], frt)
		|| !WorldToScreen(points[2], frb) || !WorldToScreen(points[1], brb)
		|| !WorldToScreen(points[6], blt) || !WorldToScreen(points[7], flt))
		return false;

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	for (int i = 1; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	x = (int)left;
	y = (int)top;
	w = (int)(right - left);
	h = (int)(bottom - top);

	return true;
}

void ESP::Draw()
{
	if (!g_Menu.Config.EnableESP)
		return;

	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return;

	if (!g::pLocalEntity)
		return;

	for (int i = 1; i <= g_pEngine->GetMaxClients(); i++) {
		C_BaseEntity* Entity = g_pEntityList->GetClientEntity(i);

		if (!Entity)
			continue;

		bool IsLocal = Entity == g::pLocalEntity;

		if (IsLocal)
			continue;

		if (Entity->GetHealth() <= 0 || !Entity->IsAlive())
			continue;

		if (Entity->GetOrigin() == Vector(0, 0, 0))
			continue;

		if (!GetBox(Entity, BoxX, BoxY, BoxW, BoxH))
			continue;

		if (Entity->IsDormant() && PlayerAlpha[i] > 0) 
		{
			PlayerAlpha[i] -= .3;
		}
		else if (!Entity->IsDormant() && PlayerAlpha[i] < 255) 
		{
			PlayerAlpha[i] = 255;
		}

		int PlayerDormantAlpha = PlayerAlpha[i];

		Color DormantFadeColor = Color(100, 100, 100, PlayerDormantAlpha);
		Color DormantColor = Color(100, 100, 100, 255);

		if (Entity->IsDormant()) {
			if (g_Menu.Config.EnableFadeESP) {
				BoxColor = DormantFadeColor;
				NameColor = DormantFadeColor;
				WeaponColor = DormantFadeColor;
				AmmoColor = DormantFadeColor;
				SkeletonColor = DormantFadeColor;
				HealthColor = DormantFadeColor;
				LBYColor = DormantFadeColor;
			}
			else {
				BoxColor = DormantColor;
				NameColor = DormantColor;
				WeaponColor = DormantColor;
				AmmoColor = DormantColor;
				SkeletonColor = DormantColor;
				HealthColor = DormantColor;
				LBYColor = DormantColor;
			}
		}
		else if (!Entity->IsDormant()) {
			BoxColor = Color(g_Menu.Config.BoxColor[0] * 255, g_Menu.Config.BoxColor[1] * 255, g_Menu.Config.BoxColor[2] * 255, 255);
			NameColor = Color(g_Menu.Config.NameColor[0] * 255, g_Menu.Config.NameColor[1] * 255, g_Menu.Config.NameColor[2] * 255, 255);
			WeaponColor = Color(g_Menu.Config.WeaponColor[0] * 255, g_Menu.Config.WeaponColor[1] * 255, g_Menu.Config.WeaponColor[2] * 255, 255);
			AmmoColor = Color(g_Menu.Config.AmmoColor[0] * 255, g_Menu.Config.AmmoColor[1] * 255, g_Menu.Config.AmmoColor[2] * 255, 255);
			SkeletonColor = Color(g_Menu.Config.SkeletonColor[0] * 255, g_Menu.Config.SkeletonColor[1] * 255, g_Menu.Config.SkeletonColor[2] * 255, 255);
			HealthColor = g_Menu.Config.HealthCustomColor ? Color(g_Menu.Config.HealthColor[0] * 255, g_Menu.Config.HealthColor[1] * 255, g_Menu.Config.HealthColor[2] * 255, 255) : GetHealthColor(Entity);
			LBYColor = Color(g_Menu.Config.LBYColor[0] * 255, g_Menu.Config.LBYColor[1] * 255, g_Menu.Config.LBYColor[2] * 255, 255);
		}

		Alpha = PlayerAlpha[Entity->EntIndex()];

		if (g_Menu.Config.EnableFadeESP) {
			RealAlpha = Alpha;
		}
		else {
			RealAlpha = 255;
		}

		if (!Entity)
			RealAlpha = 0;

		if (g_Menu.Config.NadeESP)
			DrawNades((C_BaseCombatWeapon*)Entity);

		if (Entity && Entity->GetTeam() != g::pLocalEntity->GetTeam())
		{
			if (Entity || Entity->IsAlive())
			{
				if (g_Menu.Config.BoxESP)
					DrawBox(Entity, BoxColor);
				if (g_Menu.Config.NameESP)
					DrawName(Entity, NameColor, i);
				if (g_Menu.Config.WeaponESP)
					DrawWeapon(Entity, WeaponColor, i);
				if (g_Menu.Config.HealthESP)
					DrawHealth(Entity, HealthColor);
				if (g_Menu.Config.AmmoESP)
					DrawAmmo(Entity, AmmoColor);
				if (g_Menu.Config.InfoESP)
					DrawFlags(Entity);
				if (g_Menu.Config.LbyESP)
					DrawLby(Entity, LBYColor);
			}
		}
	}
}

void ESP::DrawBox(C_BaseEntity* Entity, Color CColor)
{
	g_pSurface->OutlinedRect(BoxX, BoxY, BoxW, BoxH, CColor);
	g_pSurface->OutlinedRect(BoxX - 1, BoxY - 1, BoxW + 2, BoxH + 2, Color(20, 20, 20, RealAlpha));
	g_pSurface->OutlinedRect(BoxX + 1, BoxY + 1, BoxW - 2, BoxH - 2, Color(20, 20, 20, RealAlpha));
}

void ESP::DrawHealth(C_BaseEntity* Entity, Color CColor)
{
	int EntityHealth = Entity->GetHealth();

	auto HP = BoxH - (((BoxH * EntityHealth) / 100));

	g_pSurface->FilledRect(BoxX - 6, BoxY - 1, 2, BoxH + 2, Color(20, 20, 20, RealAlpha));
	g_pSurface->FilledRect(BoxX - 6, BoxY + HP, 2, BoxH - HP, CColor);
	g_pSurface->OutlinedRect(BoxX - 6, BoxY - 1, 3, BoxH + 2, Color(20, 20, 20, RealAlpha));

	if (Entity->GetHealth() != 100 && Entity->IsDormant())
	{
		g_pSurface->DrawT(BoxX - 10, BoxY + HP, Color(100, 100, 100, RealAlpha), g::HPFont, false,std::to_string(EntityHealth).c_str());
	}
	else if (Entity->GetHealth() != 100)
	{
		g_pSurface->DrawT(BoxX - 10, BoxY + HP, Color(255, 255, 255, RealAlpha), g::HPFont, false, std::to_string(EntityHealth).c_str());
	}
}

void ESP::DrawName(C_BaseEntity* Entity, Color CColor, int Index)
{
	PlayerInfo_t EntInfo;
	g_pEngine->GetPlayerInfo(Index, &EntInfo);

	std::string Name = EntInfo.szName;
	g_pSurface->DrawT(BoxX + (BoxW / 2), BoxY - 14, CColor, g::NameFont, true, Name.c_str());
}

void ESP::DrawWeapon(C_BaseEntity* Entity, Color CColor, int Index)
{
	PlayerInfo_t EntInfo; g_pEngine->GetPlayerInfo(Index, &EntInfo);

	C_BaseCombatWeapon* CBaseWeapon = reinterpret_cast<C_BaseCombatWeapon*>(g_pEntityList->GetClientEntity(Entity->GetActiveWeaponIndex()));
	C_BaseCombatWeapon* Weapon = Entity->GetActiveWeapon();

	if (!Weapon)
		return;

	if (!CBaseWeapon)
		return;

	bool ShowingAmmo;

	int AddToY;

	if (g_Menu.Config.LbyESP) {
		AddToY = 6;
	}
	else {
		AddToY = 0;
	}

	if (g_Menu.Config.AmmoESP) {
		ShowingAmmo = true;
	}
	else {
		ShowingAmmo = false;
	}

	int DistanceOn = ShowingAmmo ? BoxY + BoxH + 6 + AddToY : BoxY + BoxH + 2 + AddToY;

	std::string WeaponName = CBaseWeapon->GetName();
	std::transform(WeaponName.begin(), WeaponName.end(), WeaponName.begin(), ::toupper);
	WeaponName.erase(0, 7);

	switch (g_Menu.Config.WeaponESPType)
	{
	case 0:
		if (CBaseWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
			g_pSurface->DrawT(BoxX + (BoxW / 2), DistanceOn, CColor, g::WeaponFont, true, "R8 REVOLVER");
		else if (CBaseWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_USP_SILENCER)
			g_pSurface->DrawT(BoxX + (BoxW / 2), DistanceOn, CColor, g::WeaponFont, true, "USP-S");
		else if (CBaseWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_M4A1_SILENCER)
			g_pSurface->DrawT(BoxX + (BoxW / 2), DistanceOn, CColor, g::WeaponFont, true, "M4A1-S");
		else if (CBaseWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_HKP2000)
			g_pSurface->DrawT(BoxX + (BoxW / 2), DistanceOn, CColor, g::WeaponFont, true, "P2000");
		else if (CBaseWeapon)
			g_pSurface->DrawT(BoxX + (BoxW / 2), DistanceOn, CColor, g::WeaponFont, true, WeaponName.c_str());
		else
			g_pSurface->DrawT(BoxX + (BoxW / 2), DistanceOn, CColor, g::WeaponFont, true, "NONE");
		break;
	case 1:
		if (CBaseWeapon)
		{
			g_pSurface->DrawT(BoxX + (BoxW / 2), DistanceOn - 4, CColor, g::WeaponIconFont, true, "%c", GetWeaponIcon(CBaseWeapon));
		}
		else
		{
			g_pSurface->DrawT(BoxX + (BoxW / 2), DistanceOn, CColor, g::WeaponFont, true, "NONE");
		}
		break;
	}
}

void ESP::DrawAmmo(C_BaseEntity* Entity, Color CColor)
{
	if (!g::pLocalEntity)
		return;

	if (!Entity || !Entity->IsAlive())
		return;

	C_BaseCombatWeapon* CBaseWeapon = reinterpret_cast<C_BaseCombatWeapon*>(g_pEntityList->GetClientEntity(Entity->GetActiveWeaponIndex()));

	if (!CBaseWeapon)
		return;

	auto AnimLayer = Entity->GetAnimOverlayNumber(1);

	if (!AnimLayer->m_pOwner)
		return;

	auto Activity = Entity->GetSequenceActivity(AnimLayer->m_nSequence);

	if (!Activity)
		return;

	int iClip = CBaseWeapon->GetAmmo();
	int iClipMax = CBaseWeapon->GetCSWpnData()->max_clip;

	int MainWidth = 0;

	if (Activity == 967 && AnimLayer->m_flWeight != 0.f)
	{
		float Cycle = AnimLayer->m_flCycle;
		MainWidth = ((BoxW * Cycle) / 1.f);
	}
	else
	{
		MainWidth = (BoxW * float(iClip)) / iClipMax;

	}

	g_pSurface->FilledRect(BoxX, BoxY + BoxH + 2, BoxW, 4, Color(20, 20, 20, RealAlpha));
	g_pSurface->FilledRect(BoxX, BoxY + BoxH + 2, MainWidth, 4, CColor);
	g_pSurface->OutlinedRect(BoxX, BoxY + BoxH + 2, BoxW, 4, Color(20, 20, 20, RealAlpha));
}

void ESP::DrawLby(C_BaseEntity* Entity, Color CColor)
{

}

void ESP::DrawFlags(C_BaseEntity* Entity)
{
	std::vector<std::pair<std::string, Color >> StoredInfo;

	if (!g::pLocalEntity)
		return;

	auto Weapon = Entity->GetActiveWeapon();

	if (!Weapon)
		return;

	if (g_Menu.Config.InfoESP) {
		if (g_Menu.Config.InfoMoney) {
			StoredInfo.push_back(std::pair<std::string, Color>("$" + std::to_string(Entity->GetMoney()), Color(25, 185, 0, RealAlpha)));
		}
		if (g_Menu.Config.InfoArmor) {
			if (Entity->ArmorValue() > 0)
				StoredInfo.push_back(std::pair<std::string, Color>(Entity->GetArmorName(), Color(225, 225, 225, RealAlpha)));
		}
		if (g_Menu.Config.InfoZoom) {
			if (Entity->IsScoped())
				StoredInfo.push_back(std::pair<std::string, Color>("ZOOM", Color(0, 145, 255, RealAlpha)));
		}
		if (g_Menu.Config.InfoBombCarrier) {
			if (Entity->HasC4()) {
				StoredInfo.push_back(std::pair<std::string, Color>("C4", Color(255, 135, 0, RealAlpha)));
			}
		}
		if (g_Menu.Config.InfoFlashed) {
			if (Entity->IsFlashed()) {
				StoredInfo.push_back(std::pair<std::string, Color>("FLASHED", Color(255, 255, 0, RealAlpha)));
			}
		}
		if (g_Menu.Config.InfoImmune) {
			if (Entity->IsImmune()) {
				StoredInfo.push_back(std::pair<std::string, Color>("IMMUNE", Color(150, 0, 255, RealAlpha)));
			}
		}
		if (g_Menu.Config.InfoLethal) {
			if (Entity->GetHealth() <= (Weapon->GetCSWpnData()->damage)) {
				StoredInfo.push_back(std::pair<std::string, Color>("LETHAL", Color(255, 0, 0, RealAlpha)));
			}
		}
	}

	int i = 0;
	for (auto Text : StoredInfo)
	{
		g_pSurface->DrawT(BoxX + BoxW + 2, BoxY + i, Entity->IsDormant() ? Color(100, 100, 100, RealAlpha) : Text.second, g::WeaponFont, false, Text.first.c_str());
		i += 9;
	}
}

void ESP::DrawNades(C_BaseCombatWeapon* Grenade)
{
	if (!g_Menu.Config.NadeESP)
		return;

	if (!g_pEngine->IsInGame())
		return;

	if (!Grenade)
		return;

	//const model_t* Model = Grenade->GetModel();

	//if (!Model)
	//	return;

	//studiohdr_t* HDR = g_pModelInfo->GetStudiomodel(Model);

	//if (!HDR)
	//	return;

	//std::string EntityName = HDR->name, IconCharacter;

	//switch (Grenade->GetClientClass()->ClassID)
	//{
	//case 9:
	//	if (EntityName[16] == 's')
	//	{
	//		NadeColor = Color(255, 255, 255, 255);
	//		EntityName = "Flash";
	//		IconCharacter = "G";
	//	}
	//	else
	//	{
	//		NadeColor = Color(255, 255, 255, 255);
	//		EntityName = "Frag";
	//		IconCharacter = "H";
	//	}
	//	break;
	//case 134:
	//	NadeColor = Color(255, 255, 255, 255);
	//	EntityName = "Smoke";
	//	IconCharacter = "P";
	//	break;
	//case 98:
	//	NadeColor = Color(255, 255, 255, 255);
	//	EntityName = "Fire";
	//	IconCharacter = "P";
	//	break;
	//case 41:
	//	NadeColor = Color(255, 255, 255, 255);
	//	EntityName = "Decoy";
	//	IconCharacter = "G";
	//	break;
	//default:
	//	return;
	//}

	//int NadeX, NadeY, NadeW, NadeH;
	//if (!GetBox(Grenade, NadeX, NadeY, NadeW, NadeH))
	//	return;

	//g_pSurface->DrawT(NadeX, NadeY, NadeColor, g::WeaponFont, true, EntityName.c_str());
}

IMaterial* ESP::CreateMaterial(bool Shade, bool Wireframe, bool Ignorez, int RimlightBoost)
{
	static const std::string MaterialName = "game_material.vmt";
	const std::string MaterialType = Shade ? "VertexLitGeneric" : "UnlitGeneric";

	std::string MaterialData;

	MaterialData += "\"" + MaterialType + "\"\n{\n";
	MaterialData += "\t\"$basetexture\" \"vgui/white_additive\"\n";
	MaterialData += "\t\"$envmap\" \"\"\n";
	MaterialData += "\t\"$model\" \"1\"\n";
	MaterialData += "\t\"$flat\" \"1\"\n";
	MaterialData += "\t\"$nocull\" \"0\"\n";
	MaterialData += "\t\"$selfillum\" \"1\"\n";
	MaterialData += "\t\"$halflambert\" \"1\"\n";
	MaterialData += "\t\"$znearer\" \"0\"\n";
	MaterialData += "\t\"$rimlightboost\" \"" + std::to_string(RimlightBoost) + "\"\n";
	MaterialData += "\t\"$rimlightexponent\" \"" + std::to_string(4) + "\"\n";
	MaterialData += "\t\"$ambientreflectionboost\" \"0.2\"\n";
	MaterialData += "\t\"$envmaplightscale\" \"" + std::to_string(0.1) + "\"\n";
	MaterialData += "\t\"$wireframe\" \"" + std::to_string(Wireframe) + "\"\n";
	MaterialData += "\t\"$ignorez\" \"" + std::to_string(Ignorez) + "\"\n";
	MaterialData += "}\n";

	auto InitKeyValues = [](KeyValuesV2* KeyValues, const char* KeyName) -> void
	{
		using InitKeyValues_t = void(__thiscall*)(void*, const char*);
		static InitKeyValues_t InitKeyValuesEx = nullptr;
		static auto pThis = *reinterpret_cast<DWORD**>(Utils::FindSignature(XorStr("client.dll"), XorStr("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);
		if (!InitKeyValuesEx)
			InitKeyValuesEx = *reinterpret_cast<InitKeyValues_t>(Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 51 33 C0 C7 45")));

		InitKeyValuesEx(KeyValues, KeyName);
	};
	auto LoadFromBuffer = [](KeyValuesV2* KeyValues, const char* ResourceName, const char* Buf, void* FileSys = nullptr, const char* PathID = nullptr, void* EvalSymProc = nullptr, void* UNK = nullptr) -> void
	{
		using LoadFromBuffer_t = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, void*);
		static LoadFromBuffer_t LoadFromBufferEx = nullptr;

		if (!LoadFromBufferEx)
			LoadFromBufferEx = *reinterpret_cast<LoadFromBuffer_t>(Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89")));

		LoadFromBufferEx(KeyValues, ResourceName, Buf, FileSys, PathID, EvalSymProc, UNK);
	};

	auto* KeyValues = new KeyValuesV2();
	InitKeyValues(KeyValues, MaterialType.c_str());
	LoadFromBuffer(KeyValues, MaterialName.c_str(), MaterialData.c_str());

	return g_pMaterialSys->CreateMaterialAlt(MaterialName.c_str(), KeyValues);
}

void ESP::DrawChams()
{
	if (!g::pLocalEntity)
		return;

	for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| !g::pLocalEntity)
			continue;

		bool IsTeammate = pPlayerEntity->GetTeam() == g::pLocalEntity->GetTeam();
		bool IsEnemy = pPlayerEntity->GetTeam() != g::pLocalEntity->GetTeam();
		bool IsLocal = pPlayerEntity == g::pLocalEntity;


		static std::vector< IMaterial* > Materials;

		if (g_Menu.Config.EnableChams)
		{
			static bool Init = false;
			if (!Init)
			{
				Materials.push_back(CreateMaterial(true, false, false)); // material.
				Materials.push_back(CreateMaterial(false, false, false, 0)); // flat.
				Materials.push_back(CreateMaterial(true, false, true)); // material zignore.
				Materials.push_back(CreateMaterial(false, false, true)); // flat zignore.
				Init = true;
			}
		}

		float VisColor[3] = { g_Menu.Config.VisChamsColor[0], g_Menu.Config.VisChamsColor[1], g_Menu.Config.VisChamsColor[2] };
		float HidColor[3] = { g_Menu.Config.HidChamsColor[0], g_Menu.Config.HidChamsColor[1], g_Menu.Config.HidChamsColor[2] };

		if (IsEnemy)
		{
			if (g_Menu.Config.EnableChams)
			{
				if (g_Menu.Config.EnableHidChams)
				{
					g_pRenderView->SetBlend(g_Menu.Config.HidChamsColor[3]);
					switch (g_Menu.Config.ChamsMat)
					{
					case 0:
						g_pModelRender->ForcedMaterialOverride(Materials.at(2));
						break;
					case 1:
						g_pModelRender->ForcedMaterialOverride(Materials.at(3));
						break;
					}
					g_pRenderView->SetColorModulation(HidColor);
					pPlayerEntity->DrawModel(0x00000001, 255);
				}

				g_pRenderView->SetBlend(g_Menu.Config.VisChamsColor[3]);
				switch (g_Menu.Config.ChamsMat)
				{
				case 0:
					g_pModelRender->ForcedMaterialOverride(Materials.at(0));
					break;
				case 1:
					g_pModelRender->ForcedMaterialOverride(Materials.at(1));
					break;
				}
				g_pRenderView->SetColorModulation(VisColor);
				pPlayerEntity->DrawModel(0x00000001, 255);
			}
		}

		g_pModelRender->ForcedMaterialOverride(nullptr);
	}
}

void ESP::DrawGlow()
{
	for (auto i = 0; i < g_pGlowManager->GetSize(); i++)
	{
		auto& GlowObject = g_pGlowManager->m_GlowObjectDefinitions[i];
		auto Entity = reinterpret_cast<C_BaseEntity*>(GlowObject.m_pEntity);
		auto LocalPlayer = reinterpret_cast<C_BaseEntity*>(g_pEntityList->GetClientEntity(g_pEngine->GetLocalPlayer()));

		if (!Entity)
			continue;

		if (!LocalPlayer)
			continue;

		if (GlowObject.IsUnused())
			continue;

		bool IsLocalPlayer = Entity == LocalPlayer;
		bool IsTeammate = LocalPlayer->GetTeam() == Entity->GetTeam() && !IsLocalPlayer;
		bool IsEnemy = Entity->GetTeam() != LocalPlayer->GetTeam();

		if (IsEnemy)
		{
			if (g_Menu.Config.EnablePlayerGlow)
			{
				auto ClassId = Entity->GetClientClass()->ClassID;

				switch (ClassId)
				{
				default:
					GlowObject.m_flAlpha = 0.f;
					break;
				case 40:
					GlowObject.m_nGlowStyle = 0;
					GlowObject.m_flAlpha = g_Menu.Config.PlayerGlowColor[3];
					break;
				}

				GlowObject.m_flRed = g_Menu.Config.PlayerGlowColor[0];
				GlowObject.m_flGreen = g_Menu.Config.PlayerGlowColor[1];
				GlowObject.m_flBlue = g_Menu.Config.PlayerGlowColor[2];
				GlowObject.m_bRenderWhenOccluded = true;
				GlowObject.m_bRenderWhenUnoccluded = false;
			}
		}
	}
}

void ESP::RemoveFlash()
{
	if (!g::pLocalEntity)
		return;

	if (!g_Menu.Config.EnableRemovals)
		return;

	if (g_Menu.Config.RemoveFlash)
	{
		if (g::pLocalEntity->FlashDuration() > 0.f)
			g::pLocalEntity->FlashDuration() = 0.f;
	}
}

void ESP::RemoveSmoke()
{
	static auto LineThroughSmoke = Utils::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");

	if (!g_Menu.Config.EnableRemovals)
		return;

	if (g_Menu.Config.RemoveSmoke) 
	{
		std::vector<const char*> VistaSmokeWireframe =
		{
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
		};

		std::vector<const char*> VistasmokeNodraw =
		{
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
		};

		for (auto mat_s : VistaSmokeWireframe)
		{
			IMaterial* mat = g_pMaterialSys->FindMaterial(mat_s, "Other textures");
			mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
		}

		for (auto mat_n : VistasmokeNodraw)
		{
			IMaterial* mat = g_pMaterialSys->FindMaterial(mat_n, "Other textures");
			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		}

		static auto smokecout = *(DWORD*)(LineThroughSmoke + 0x8);
		*(int*)(smokecout) = 0;
	}

}

void ESP::RemoveScope()
{
	if (!g::pLocalEntity)
		return;

	if (g_pEngine->IsInGame() && g_pEngine->IsConnected())
	{
		if (g_Menu.Config.EnableRemovals)
		{
			if (g_Menu.Config.RemoveScope)
			{
				if (!g::pLocalEntity->IsScoped())
					return;

				int Width;
				int Height;
				g_pEngine->GetScreenSize(Width, Height);

				Color ScopeColor = Color(g_Menu.Config.ScopeLine[0] * 255, g_Menu.Config.ScopeLine[1] * 255, g_Menu.Config.ScopeLine[2] * 255, 255);
				g_pSurface->Line(Width / 2, 0, Width / 2, Height, ScopeColor);
				g_pSurface->Line(0, Height / 2, Width, Height / 2, ScopeColor);

			}
		}
	}
}