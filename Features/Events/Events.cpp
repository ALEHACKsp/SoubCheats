#include "Events.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\Utils\Math.h"
#include "../../Security/Xor.h"
#include "..\..\Menu\Menu.h"
#include <iostream>
#include <algorithm>

Events g_Events;

Events::ItemPurchaseListener _ItemPurchaseListener;
Events::PlayerHurtListener _PlayerHurtListener;
Events::BulletImpactListener _BulletImpactListener;
Events::RoundStartListener _RoundStartListener;
Events::WeaponFiredListener _WeaponFiredListener;

void Events::InitializeEventListeners()
{
	g_pEventManager->AddListener(&_ItemPurchaseListener, "item_purchase", false);
	g_pEventManager->AddListener(&_PlayerHurtListener, "player_hurt", false);
	g_pEventManager->AddListener(&_BulletImpactListener, "bullet_impact", false);
	g_pEventManager->AddListener(&_RoundStartListener, "round_start", false);
	g_pEventManager->AddListener(&_WeaponFiredListener, "weapon_fire", false);
}

void Events::ItemPurchaseListener::FireGameEvent(IGameEvent* GameEvent)
{
	if (!GameEvent)
		return;
}


void Events::PlayerHurtListener::FireGameEvent(IGameEvent* GameEvent)
{
	if (!GameEvent)
		return;
}

void Events::BulletImpactListener::FireGameEvent(IGameEvent* GameEvent)
{
	if (!GameEvent)
		return;
}

void Events::WeaponFiredListener::FireGameEvent(IGameEvent* GameEvent)
{
	if (!GameEvent)
		return;
}

void Events::RoundStartListener::FireGameEvent(IGameEvent* GameEvent)
{
	if (!GameEvent)
		return;

	if (!g::pLocalEntity)
		return;

	if (g_Menu.Config.BuyBotEnable) 
	{
		if (g::pLocalEntity->GetMoney() <= 800)
		{
			switch (g_Menu.Config.BuyBotPistols) {
			case 0:
				g_pEngine->ExecuteClientCmd("buy elite");
				break;
			case 1:
				g_pEngine->ExecuteClientCmd("buy deagle;buy revolver");
				break;
			}

			if (g_Menu.Config.BuyMisc[0]) {
				g_pEngine->ExecuteClientCmd("buy defuser");
			}

			if (g_Menu.Config.BuyMisc[3]) {
				g_pEngine->ExecuteClientCmd("buy taser 34;");
			}
		}
		else {
			switch (g_Menu.Config.BuyBotPistols) {
			case 0:
				g_pEngine->ExecuteClientCmd("buy elite");
				break;
			case 1:
				g_pEngine->ExecuteClientCmd("buy deagle;buy revolver");
				break;
			}

			if (g_Menu.Config.BuyMisc[0]) {
				g_pEngine->ExecuteClientCmd("buy defuser");
			}

			switch (g_Menu.Config.BuyBotWeapons) {
			case 0:
				g_pEngine->ExecuteClientCmd("buy scar20;buy g3sg1");
				break;
			case 1:
				g_pEngine->ExecuteClientCmd("buy ssg08");
				break;
			case 2:
				g_pEngine->ExecuteClientCmd("buy awp");
				break;
			}

			if (g_Menu.Config.BuyMisc[1]) {
				g_pEngine->ExecuteClientCmd("buy vest;buy vesthelm");
			}
			if (g_Menu.Config.BuyMisc[2]) {
				g_pEngine->ExecuteClientCmd("buy incgrenade;buy molotov;buy hegrenade;buy smokegrenade");
			}
			if (g_Menu.Config.BuyMisc[3]) {
				g_pEngine->ExecuteClientCmd("buy taser 34;");
			}

			if (g_Menu.Config.BuyFlash) {
				g_pEngine->ExecuteClientCmd("buy flashbang;");
			}

			if (g_Menu.Config.BuyDecoy) {
				g_pEngine->ExecuteClientCmd("buy decoy;");
			}
		}
	}
}