#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

C_BaseEntity* UTIL_PlayerByIndex(int index)
{
	typedef C_BaseEntity* (__fastcall* PlayerByIndex)(int);
	static PlayerByIndex UTIL_PlayerByIndex = (PlayerByIndex)Utils::FindSignature(XorStr("server.dll"), XorStr("85 C9 7E 2A A1"));

	if (!UTIL_PlayerByIndex)
		return false;

	return UTIL_PlayerByIndex(index);
}

void __fastcall Hooks::PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached PaintTraverse.")); Init = false; }

	static auto oPaintTraverse = g_Hooks.pPanelHook->GetOriginal<PaintTraverse_t>(HookIndexes::PaintTraverse);
	static unsigned int panelID, panelHudID;

	if (!panelHudID)
	{
		if (!strcmp(XorStr("HudZoom"), g_pPanel->GetName(vguiPanel)))
		{
			panelHudID = vguiPanel;
		}
	}

	if (panelHudID == vguiPanel && g::pLocalEntity && g::pLocalEntity->IsAlive() && g_Menu.Config.RemoveScope && g_Menu.Config.EnableRemovals
		&& g_pEngine->IsInGame() && g_pEngine->IsConnected())
	{
		if (g::pLocalEntity->IsScoped())
			return;
	}

	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);

	if (!panelID)
	{
		if (!strcmp(XorStr("FocusOverlayPanel"), g_pPanel->GetName(vguiPanel)))
		{
			panelID = vguiPanel;
		}
	}

	if (panelID == vguiPanel)
	{
		g_pPanel->SetMouseInputEnabled(vguiPanel, g_Menu.menuOpened);

		if (g_pSurface->ShouldReloadFonts())
		{
			g::CourierNew = g_pSurface->FontCreate();
			g_pSurface->SetFontGlyphSet(g::CourierNew, XorStr("Courier New"), 14, 300, 0, 0, FONTFLAG_OUTLINE);
			g::Tahoma = g_pSurface->FontCreate();
			g_pSurface->SetFontGlyphSet(g::Tahoma, XorStr("Tahoma"), 12, 500, 0, 0, FONTFLAG_DROPSHADOW);
			g::HPFont = g_pSurface->FontCreate();
			g_pSurface->SetFontGlyphSet(g::HPFont, XorStr("Small Fonts"), 8, 400, 0, 0, FONTFLAG_OUTLINE);
			g::NameFont = g_pSurface->FontCreate();
			g_pSurface->SetFontGlyphSet(g::NameFont, XorStr("Tahoma"), 12, 400, 0, 0, FONTFLAG_DROPSHADOW);
			g::WeaponFont = g_pSurface->FontCreate();
			g_pSurface->SetFontGlyphSet(g::WeaponFont, XorStr("Small Fonts"), 9, 400, 0, 0, FONTFLAG_OUTLINE);
			g::WeaponIconFont = g_pSurface->FontCreate();
			g_pSurface->SetFontGlyphSet(g::WeaponIconFont, XorStr("Counter-Strike"), 24, 400, 0, 0, FONTFLAG_ANTIALIAS);
		}

		g_ESP.Draw();
		g_ESP.RemoveScope();
	}

	if (!strcmp("MatSystemTopPanel", g_pPanel->GetName(vguiPanel)))
	{
		if (g_Menu.Config.DebugModel) // server hitboxes
		{
			static uintptr_t pCall = (uintptr_t)Utils::FindSignature("server.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE");

			float fDuration = -1.f;

			PVOID pEntity = nullptr;
			pEntity = UTIL_PlayerByIndex(g::pLocalEntity->EntIndex());

			if (pEntity)
			{
				__asm
				{
					pushad
					movss xmm1, fDuration
					push 0 //bool monoColor
					mov ecx, pEntity
					call pCall
					popad
				}
			}
		}
	}
}
