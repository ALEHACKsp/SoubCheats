#include "Menu.h"
#include "TGFCfg.h"
#include "..\SDK\Vector.h"
#include "..\SDK\ISurface.h"
#include "..\Utils\Color.h"
#include "..\Utils\GlobalVars.h"

#include "../Dependencies/ImGui/imgui.h"
#include "../Dependencies/ImGui/imgui_impl_dx9.h"
#include "../Dependencies/ImGui/imgui_impl_win32.h"
#include "../Dependencies/ImGui/imgui_internal.h"

#include "../Security/Xor.h"

#include "Console.h"

Menu g_Menu;

DWORD ChildFlags = ImGuiWindowFlags_NoScrollbar;
DWORD ColorFlags = ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs;

bool Keyhandler::IsKeyDown(int Key)
{
    return HIWORD(GetKeyState(Key));
}

bool Keyhandler::IsKeyUp(int Key)
{
    return !HIWORD(GetKeyState(Key));
}

bool Keyhandler::IsKeyPressed(int Key)
{
    return false;
}

bool Keyhandler::AutoCheck(int KeyStyle, int Key)
{
    switch (KeyStyle) {
    case 0:
        return true;
    case 1:
        return IsKeyDown(Key);
    case 2:
        return LOWORD(GetKeyState(Key));
    case 3:
        return IsKeyUp(Key);
    default:
        return true;
    }
}

constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b)
{
    return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
};

void Menu::CreateStyle()
{
    auto& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
    const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
    const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

    const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
    const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
    const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);

    const ImVec4 textColor = ColorFromBytes(255, 255, 255);
    const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
    const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

    colors[ImGuiCol_Text] = textColor;
    colors[ImGuiCol_TextDisabled] = textDisabledColor;
    colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
    colors[ImGuiCol_WindowBg] = bgColor;
    colors[ImGuiCol_ChildBg] = bgColor;
    colors[ImGuiCol_PopupBg] = bgColor;
    colors[ImGuiCol_Border] = borderColor;
    colors[ImGuiCol_BorderShadow] = borderColor;
    colors[ImGuiCol_FrameBg] = panelColor;
    colors[ImGuiCol_FrameBgHovered] = lightBgColor;
    colors[ImGuiCol_FrameBgActive] = lightBgColor;
    colors[ImGuiCol_TitleBg] = bgColor;
    colors[ImGuiCol_TitleBgActive] = bgColor;
    colors[ImGuiCol_TitleBgCollapsed] = bgColor;
    colors[ImGuiCol_MenuBarBg] = panelColor;
    colors[ImGuiCol_ScrollbarBg] = panelColor;
    colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
    colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
    colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
    colors[ImGuiCol_CheckMark] = panelActiveColor;
    colors[ImGuiCol_SliderGrab] = panelHoverColor;
    colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
    colors[ImGuiCol_Button] = panelColor;
    colors[ImGuiCol_ButtonHovered] = panelHoverColor;
    colors[ImGuiCol_ButtonActive] = panelHoverColor;
    colors[ImGuiCol_Header] = panelColor;
    colors[ImGuiCol_HeaderHovered] = panelHoverColor;
    colors[ImGuiCol_HeaderActive] = panelActiveColor;
    colors[ImGuiCol_Separator] = borderColor;
    colors[ImGuiCol_SeparatorHovered] = borderColor;
    colors[ImGuiCol_SeparatorActive] = borderColor;
    colors[ImGuiCol_ResizeGrip] = bgColor;
    colors[ImGuiCol_ResizeGripHovered] = panelColor;
    colors[ImGuiCol_ResizeGripActive] = lightBgColor;
    colors[ImGuiCol_PlotLines] = panelActiveColor;
    colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
    colors[ImGuiCol_PlotHistogram] = panelActiveColor;
    colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
    colors[ImGuiCol_DragDropTarget] = bgColor;
    colors[ImGuiCol_NavHighlight] = bgColor;
    colors[ImGuiCol_Tab] = bgColor;
    colors[ImGuiCol_TabActive] = panelActiveColor;
    colors[ImGuiCol_TabUnfocused] = bgColor;
    colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
    colors[ImGuiCol_TabHovered] = panelHoverColor;

    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding = 0.0f;
}

const char* KeyType[] =
{
    XorStr("Forced"),
    XorStr("Hold"),
    XorStr("Toggle"),
};

void OpenCFGFolder()
{
    ShellExecute(NULL, XorStr("open"), XorStr("C:\\onetap.soub"), NULL, NULL, SW_SHOWDEFAULT);
}

void RenderRage()
{
    ImGui::BeginChild(XorStr("##MainRage"), { 0, 340 }, true);

    static int PageIndex = 0;

    if (ImGui::Button(XorStr("Main"), { 80, 105 })) {
        PageIndex = 0;
    }
    if (ImGui::Button(XorStr("Settings"), { 80, 105 })) {
        PageIndex = 1;
    }
    if (ImGui::Button(XorStr("Misc"), { 80, 105 })) {
        PageIndex = 2;
    }

    ImGui::SameLine();

    switch (PageIndex) {
    case 0:
        ImGui::SetCursorPosY(8);

        ImGui::BeginChild(XorStr("##MainTab"), { -1, 324 }, true);
        {
            {
                ImGui::BeginChild(XorStr("##Rage2"), { 178, 308 }, true);

                ImGui::Checkbox(XorStr("Enable Ragebot"), &g_Menu.Config.EnableRagebot);
                ImGui::SameLine();
                ImGui::Keybind(XorStr("##RageKey"), &g_Menu.Config.RagebotKey, &g_Menu.Config.RagebotKeyType);
                
                const char* ScanType[] = { XorStr("Fov"), XorStr("Distance"), XorStr("Threat") };
                ImGui::PushItemWidth(160);
                ImGui::Combo(XorStr("##ScanType"), &g_Menu.Config.TargetMode, ScanType, ARRAYSIZE(ScanType));
                ImGui::SliderInt(XorStr("##FOV"), &g_Menu.Config.RagebotFov, 0, 180, XorStr("FOV %d"));
                ImGui::PopItemWidth();

                ImGui::Checkbox(XorStr("Silent Aim"), &g_Menu.Config.SilentAim);
                ImGui::Checkbox(XorStr("No Recoil"), &g_Menu.Config.NoAimRecoil);
                ImGui::Checkbox(XorStr("Auto Scope"), &g_Menu.Config.AutoScope);
                ImGui::Checkbox(XorStr("Auto Stop"), &g_Menu.Config.AutoStop);
                ImGui::Checkbox(XorStr("Between Shots"), &g_Menu.Config.StopBetweenShots);
                ImGui::Checkbox(XorStr("Ignore Lag Comp"), &g_Menu.Config.DontShootLagComp);

                ImGui::Checkbox(XorStr("Double Tap"), &g_Menu.Config.DoubleTap);
                ImGui::SameLine();
                ImGui::Keybind(XorStr("##DTKey"), &g_Menu.Config.DoubleTapKey, &g_Menu.Config.DoubleTapKeyType);
                ImGui::Checkbox(XorStr("Instant"), &g_Menu.Config.DoubleTapInstant);
                ImGui::Checkbox(XorStr("Teleport"), &g_Menu.Config.DoubleTapTeleport);

                ImGui::EndChild();
            }

            ImGui::SameLine();

            {
                ImGui::BeginChild(XorStr("##Rage3"), { 178, 307 }, true);

                const char* HitboxesToScanType[] = { XorStr("Only Head"), XorStr("Only Body"), XorStr("Full Scan") };
                ImGui::PushItemWidth(160);
                ImGui::Combo(XorStr("Hitboxes"), &g_Menu.Config.HitboxesToScan, HitboxesToScanType, ARRAYSIZE(HitboxesToScanType));
                ImGui::PopItemWidth();

                ImGui::PushItemWidth(160);
                ImGui::Text(XorStr("Head Scale"));
                ImGui::SliderInt(XorStr("##HeadScale"), &g_Menu.Config.Headscale, 0, 100);
                ImGui::Text(XorStr("Body Scale"));
                ImGui::SliderInt(XorStr("##BodyScale"), &g_Menu.Config.Bodyscale, 0, 100);
                ImGui::Text(XorStr("Hitchance"));
                ImGui::SliderInt(XorStr("##Hitchance"), &g_Menu.Config.Hitchance, 0, 100);
                ImGui::Text(XorStr("Minimum Damage"));
                ImGui::SliderInt(XorStr("##MinDamage"), &g_Menu.Config.MinDamage, 0, 100);
                
                ImGui::PopItemWidth();

                ImGui::Checkbox(XorStr("##FastDuck"), &g_Menu.Config.FastDuck);
                ImGui::SameLine(31);
                ImGui::TextColored(ImColor(255, 255, 0, 255), XorStr("Fast Duck"));

                ImGui::Checkbox(XorStr("Fake Model"), &g_Menu.Config.FakeChams);

                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
        break;
    }

    ImGui::EndChild();
}

void RenderAntiAim()
{
    ImGui::BeginChild(XorStr("##MainAA"), { 0, 340 }, true, ChildFlags);

    static int PageIndex = 0;

    if (ImGui::Button(XorStr("Main"), { 80, 324 })) {
        PageIndex = 0;
    }

    ImGui::SameLine();

    static bool Bool = false;
    static int Int = 0;

    switch (PageIndex) {
    case 0:
        ImGui::SetCursorPosY(8);

        ImGui::BeginChild(XorStr("##MainAATab"), { -1, 324 }, true, ChildFlags);
        {
            {
                ImGui::BeginChild(XorStr("##AA1"), { 178, 150 }, true);

                ImGui::Checkbox(XorStr("Enable AA"), &g_Menu.Config.EnableAntiAim);

                ImGui::EndChild();
            }

            ImGui::SameLine();

            {
                ImGui::BeginChild(XorStr("##AA2"), { 178, 150 }, true, ChildFlags);


                ImGui::EndChild();
            }


            ImGui::SetCursorPos({ 8, 165 });

            {
                ImGui::BeginChild(XorStr("##AA3"), { 178, 150 }, true, ChildFlags);

                ImGui::EndChild();
            }

            ImGui::SetCursorPos({ 194, 165 });

            {
                ImGui::BeginChild(XorStr("##AA4"), { 178, 150 }, true, ChildFlags);

                ImGui::Checkbox(XorStr("Enable Fakelag"), &g_Menu.Config.EnableFakeLag);
                ImGui::SliderInt(XorStr("##FakeLagTicks"), &g_Menu.Config.FakelagAmmount, 0, 16, XorStr("Ticks %0.0f"));

                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
        break;
    }

    ImGui::EndChild();
}

void RenderVisuals()
{
    ImGui::BeginChild(XorStr("##MainMisc"), { 0, 340 }, true, ChildFlags);

    static int PageIndex = 0;

    if (ImGui::Button(XorStr("Players"), { 80, 160 })) {
        PageIndex = 0;
    }
    if (ImGui::Button(XorStr("Visuals"), { 80, 160 })) {
        PageIndex = 1;
    }

    ImGui::SameLine();

    static bool Bool = false;
    static int Int = 0;

    switch (PageIndex) {
    case 0:

        ImGui::SetCursorPosY(8);

        ImGui::BeginChild(XorStr("##Vis1MainTab"), { -1, 324 }, true);
        {
            {
                ImGui::BeginChild(XorStr("##Vis1"), { 178, 308 }, true);

                ImGui::Checkbox(XorStr("Enable ESP"), &g_Menu.Config.EnableESP);
                ImGui::Checkbox(XorStr("Dormant ESP"), &g_Menu.Config.EnableFadeESP);

                ImGui::Checkbox(XorStr("Player Box"), &g_Menu.Config.BoxESP);
                ImGui::SameLine(150);
                ImGui::ColorEdit4(XorStr("##BoxColor"), g_Menu.Config.BoxColor, ColorFlags);

                ImGui::Checkbox(XorStr("Player Name"), &g_Menu.Config.NameESP);
                ImGui::SameLine(150);
                ImGui::ColorEdit4(XorStr("##NameColor"), g_Menu.Config.NameColor, ColorFlags);

                ImGui::Checkbox(XorStr("Player Weapon"), &g_Menu.Config.WeaponESP);
                if (g_Menu.Config.WeaponESP)
                {
                    ImGui::TextColored(ImColor(29, 151, 236, 255), XorStr("->"));
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(30);
                    const char* WeaponEspType[] = { XorStr("Text"), XorStr("Icon") };
                    ImGui::PushItemWidth(112);
                    ImGui::Combo(XorStr("##WeaponEspType"), &g_Menu.Config.WeaponESPType, WeaponEspType, ARRAYSIZE(WeaponEspType));
                    ImGui::SameLine();
                    ImGui::ColorEdit4(XorStr("##WeaponColor"), g_Menu.Config.WeaponColor, ColorFlags);
                }

                ImGui::Checkbox(XorStr("Health Bar"), &g_Menu.Config.HealthESP);
                ImGui::SameLine(115);
                ImGui::PushItemWidth(55);
                if (ImGui::BeginCombo(XorStr("##HealthType"), XorStr("Mode")))
                {
                    ImGui::Checkbox(XorStr("Custom Color"), &g_Menu.Config.HealthCustomColor);
                    ImGui::SameLine();
                    ImGui::ColorEdit4(XorStr("##HealthColor"), g_Menu.Config.HealthColor, ColorFlags);
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::Checkbox(XorStr("Player Ammo"), &g_Menu.Config.AmmoESP);
                ImGui::SameLine(150);
                ImGui::ColorEdit4(XorStr("##AmmoColor"), g_Menu.Config.AmmoColor, ColorFlags);

                ImGui::Checkbox(XorStr("Player Info"), &g_Menu.Config.InfoESP);
                ImGui::SameLine(115);
                ImGui::PushItemWidth(55);
                if (ImGui::BeginCombo(XorStr("##FlagsType"), XorStr("Mode")))
                {
                    ImGui::Checkbox(XorStr("Money"), &g_Menu.Config.InfoMoney);
                    ImGui::Checkbox(XorStr("Armor"), &g_Menu.Config.InfoArmor);
                    ImGui::Checkbox(XorStr("Scoped"), &g_Menu.Config.InfoZoom);
                    ImGui::Checkbox(XorStr("Bomb"), &g_Menu.Config.InfoBombCarrier);
                    ImGui::Checkbox(XorStr("Flashed"), &g_Menu.Config.InfoFlashed);
                    ImGui::Checkbox(XorStr("Immune"), &g_Menu.Config.InfoImmune);
                    ImGui::Checkbox(XorStr("Lethal"), &g_Menu.Config.InfoLethal);
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::EndChild();
            }

            ImGui::SameLine();

            {
                ImGui::BeginChild(XorStr("##Vis2"), { 178, 150 }, true, ChildFlags);

                ImGui::Checkbox(XorStr("Player Chams"), &g_Menu.Config.EnableChams);
                ImGui::SameLine(150);
                ImGui::ColorEdit4(XorStr("##VisChamColor"), g_Menu.Config.VisChamsColor, ColorFlags);

                ImGui::Checkbox(XorStr("Hidden Chams"), &g_Menu.Config.EnableHidChams);
                ImGui::SameLine(150);
                ImGui::ColorEdit4(XorStr("##HiddenChamColor"), g_Menu.Config.HidChamsColor, ColorFlags);

                const char* ChamsMatType[] = { XorStr("Texture"), XorStr("Flat") };
                ImGui::PushItemWidth(160);
                ImGui::Combo(XorStr("##ChamType"), &g_Menu.Config.ChamsMat, ChamsMatType, ARRAYSIZE(ChamsMatType));
                ImGui::PopItemWidth();

                ImGui::Checkbox(XorStr("Player Glow"), &g_Menu.Config.EnablePlayerGlow);
                ImGui::SameLine(150);
                ImGui::ColorEdit4(XorStr("##GlowColor"), g_Menu.Config.PlayerGlowColor, ColorFlags);

                ImGui::EndChild();
            }

            ImGui::SetCursorPos({ 194, 165 });

            {
                ImGui::BeginChild(XorStr("##Vis3"), { 178, 150 }, true, ChildFlags);

                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
        break;
    case 1:
        ImGui::SetCursorPosY(8);

        ImGui::BeginChild(XorStr("##VisMainTab"), { -1, 324 }, true, ChildFlags);
        {
            {
                ImGui::BeginChild(XorStr("#Vis1"), { 178, 308 }, true);

                ImGui::EndChild();
            }

            ImGui::SameLine();

            {
                ImGui::BeginChild(XorStr("##Vis2"), { 178, 150 }, true, ChildFlags);

                ImGui::EndChild();
            }

            ImGui::SetCursorPos({ 194, 165 });

            {
                ImGui::BeginChild(XorStr("##Vis3"), { 178, 150 }, true, ChildFlags);

                ImGui::Checkbox(XorStr("Removals"), &g_Menu.Config.EnableRemovals);
                ImGui::SameLine(113);
                ImGui::PushItemWidth(55);
                if (ImGui::BeginCombo(XorStr("##RemovalsType"), XorStr("Mode")))
                {
                    ImGui::Checkbox(XorStr("Scope"), &g_Menu.Config.RemoveScope);
                    ImGui::SameLine();
                    ImGui::ColorEdit4(XorStr("##ScopeColor"), g_Menu.Config.ScopeLine, ColorFlags);
                    ImGui::Checkbox(XorStr("Zoom"), &g_Menu.Config.RemoveZoom);
                    ImGui::Checkbox(XorStr("Aim Punch"), &g_Menu.Config.RemoveAimPunch);
                    ImGui::Checkbox(XorStr("Smoke"), &g_Menu.Config.RemoveSmoke);
                    ImGui::Checkbox(XorStr("Flash"), &g_Menu.Config.RemoveFlash);
                    ImGui::Checkbox(XorStr("Sleeves"), &g_Menu.Config.RemoveSleeves);
                    ImGui::Checkbox(XorStr("Shadows"), &g_Menu.Config.RemoveShadows);
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::Checkbox(XorStr("Modify View"), &g_Menu.Config.AllowViewModify);
                ImGui::SameLine(113);
                ImGui::PushItemWidth(55);
                if (ImGui::BeginCombo(XorStr("##ModifyViewType"), XorStr("Mode")))
                {
                    ImGui::SliderInt(XorStr("##DebugFOVSlider"), &g_Menu.Config.DebugFov, 0, 120, XorStr("FOV %0.0f"));
                    ImGui::SliderInt(XorStr("##ViewFOVSlider"), &g_Menu.Config.DebugViewFov, 0, 120, XorStr("View FOV %0.0f"));

                    ImGui::SliderInt(XorStr("##X"), &g_Menu.Config.ViewOffsetX, -35, 35, XorStr("X %0.0f"));
                    ImGui::SliderInt(XorStr("##Y"), &g_Menu.Config.ViewOffsetY, -35, 35, XorStr("Y %0.0f"));
                    ImGui::SliderInt(XorStr("##Z"), &g_Menu.Config.ViewOffsetZ, -35, 35, XorStr("Z %0.0f"));
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::Checkbox(XorStr("Thirdperson"), &g_Menu.Config.Thirdperson);
                ImGui::SameLine(113);
                ImGui::Keybind(XorStr("##TPKey"), &g_Menu.Config.ThirdpersonKey, &g_Menu.Config.ThirdpersonKeyType);

                ImGui::PushItemWidth(160);
                ImGui::SliderInt(XorStr("##TPDist"), &g_Menu.Config.ThirdpersonDist, 25, 500, XorStr("Distance %0.0f"));
                ImGui::Checkbox(XorStr("Thirdperson Spectate"), &g_Menu.Config.ThirdpersonWhileSpec);
                ImGui::PopItemWidth();

                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
        break;
    }

    ImGui::EndChild();
}

void RenderMisc()
{
    ImGui::BeginChild(XorStr("##MainMisc"), { 0, 340 }, true, ChildFlags);

    static int PageIndex = 0;

    if (ImGui::Button(XorStr("Main"), { 80, 324 })) {
        PageIndex = 0;
    }

    ImGui::SameLine();

    static bool Bool = false;
    static int Int = 0;

    switch (PageIndex) {
    case 0:
        ImGui::SetCursorPosY(8);

        ImGui::BeginChild(XorStr("##MainMiscTab"), { -1, 324 }, true, ChildFlags);
        {
            {
                ImGui::BeginChild(XorStr("##Misc1"), { 178, 150 }, true);

                ImGui::Checkbox(XorStr("##AntiUntrusted"), &g_Menu.Config.AntiUntrusted);
                ImGui::SameLine(31);
                ImGui::TextColored(ImColor(255, 255, 0, 255), XorStr("Anti-Untrusted"));

                ImGui::Checkbox(XorStr("Bhop"), &g_Menu.Config.Bhop);
                ImGui::SameLine(115);
                ImGui::PushItemWidth(55);
                if (ImGui::BeginCombo(XorStr("##BhopMode"), XorStr("Mode")))
                {
                    ImGui::PushItemWidth(150);
                    ImGui::Checkbox(XorStr("Legit Bhop"), &g_Menu.Config.EnableLegitBhop);
                    ImGui::SliderInt(XorStr("Bhop Hitchance"), &g_Menu.Config.BhopHitchance, 0, 100, XorStr("%d"));
                    ImGui::SliderInt(XorStr("Max Bhops Allowed"), &g_Menu.Config.BhopsRestricted, 0, 20, XorStr("Hops %d"));
                    ImGui::SliderInt(XorStr("Max Bhops Hit"), &g_Menu.Config.MaxBhops, 0, 20, XorStr("Hops %d"));
                    ImGui::PopItemWidth();
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::Checkbox(XorStr("Auto Strafe"), &g_Menu.Config.AutoStrafe);
                ImGui::SameLine(115);
                ImGui::PushItemWidth(55);
                if (ImGui::BeginCombo(XorStr("##StrafeType"), XorStr("Type")))
                {
                    const char* AutoStrafeType[] = { XorStr("Static"), XorStr("Movement-Based") };
                    ImGui::PushItemWidth(125);
                    ImGui::Combo(XorStr("##StrafeType"), &g_Menu.Config.AutoStrafeType, AutoStrafeType, ARRAYSIZE(AutoStrafeType));
                    ImGui::PopItemWidth();
                    ImGui::EndCombo();
                }
                ImGui::PopItemWidth();

                ImGui::Checkbox(XorStr("Clantag Spammer"), &g_Menu.Config.Clantag);

                ImGui::Checkbox(XorStr("Edge Jump"), &g_Menu.Config.EdgeJump);
                ImGui::SameLine();
                ImGui::Keybind(XorStr("##EdgeJumpKey"), &g_Menu.Config.EdgeJumpKey, &g_Menu.Config.EdgeJumpKeyType);
              
                ImGui::Checkbox(XorStr("Disable Post Effects"), &g_Menu.Config.DisablePostEffects);

                ImGui::EndChild();
            }

            ImGui::SameLine();

            {
                ImGui::BeginChild(XorStr("##Misc2"), { 178, 150 }, true, ChildFlags);

                ImGui::Checkbox(XorStr("Reveal Hidden Cvars"), &g_Menu.Config.RevealHiddenCvars);

                ImGui::Checkbox(XorStr("##ForceCheats"), &g_Menu.Config.ForceCheats);
                ImGui::SameLine(31);
                ImGui::TextColored(ImColor(255, 255, 0, 255), XorStr("Force Cheats"));

                if (ImGui::Button(XorStr("Rage Quit"), { 160, 41 }))
                {
                    auto CrashInit = reinterpret_cast<int*>(0);
                    *CrashInit = *CrashInit;
                }

                if (ImGui::Button(XorStr("Join Discord"), { 160, 41 }))
                {
                    ShellExecute(0, 0, XorStr("https://discord.gg/bfnrJtCfGW"), 0, 0, SW_SHOW);
                }

                ImGui::EndChild();
            }


            ImGui::SetCursorPos({ 8, 165 });

            {
                ImGui::BeginChild(XorStr("##Misc3"), { 178, 150 }, true, ChildFlags);

                ImGui::Checkbox(XorStr("Buy Bot"), &g_Menu.Config.BuyBotEnable);
                const char* BuyBotWeapons[] = { XorStr("Auto"), XorStr("Scout"), XorStr("Awp") };
                ImGui::Combo(XorStr("##BuyBotWeapons"), &g_Menu.Config.BuyBotWeapons, BuyBotWeapons, ARRAYSIZE(BuyBotWeapons));
                const char* BuyBotPistols[] = { XorStr("Dual Berettas"), XorStr("Revolver/Deagle") };
                ImGui::Combo(XorStr("##BuyBotPistols"), &g_Menu.Config.BuyBotPistols, BuyBotPistols, ARRAYSIZE(BuyBotPistols));

                const char* BuyMiscList[] = { XorStr("Defuse Kit"), XorStr("Armor"), XorStr("Grenades"), XorStr("Taser") };
                if (ImGui::BeginCombo(XorStr("##BuyExtra"), XorStr("Buy Extra")))
                {
                    for (int i = 0; i < 4; i++)
                    {
                        ImGui::Selectable(BuyMiscList[i], &g_Menu.Config.BuyMisc[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
                    }
                    ImGui::EndCombo();
                }

                ImGui::Checkbox(XorStr("Buy Flash"), &g_Menu.Config.BuyFlash);
                ImGui::Checkbox(XorStr("Buy Decoy"), &g_Menu.Config.BuyDecoy);

                ImGui::EndChild();
            }

            ImGui::SetCursorPos({ 194, 165 });

            {
                ImGui::BeginChild(XorStr("##Misc4"), { 178, 150 }, true, ChildFlags);

                const char* ConfigSelect[] = { XorStr("Auto Sniper"), XorStr("Scout"), XorStr("AWP"), XorStr("Pistol"), XorStr("Heavy Pistol"), XorStr("General"), XorStr("Legit") };
                ImGui::PushItemWidth(160);
                ImGui::Combo(XorStr("##ConfigType"), &g_Menu.Config.ConfigList, ConfigSelect, ARRAYSIZE(ConfigSelect));
                ImGui::PopItemWidth();

                if (ImGui::Button(XorStr("Save"), { 76, 35 }))
                {
                    g_Config->Save();
                }
                ImGui::SameLine();
                if (ImGui::Button(XorStr("Load"), { 76, 35 }))
                {
                    g_Config->Load();
                }
                if (ImGui::Button(XorStr("Open Folder"), { 160, 32 }))
                {
                    OpenCFGFolder();
                }
                if (ImGui::Button(XorStr("Reset Current Config"), { 160, 32 }))
                {
                    g_Config->Setup();
                }

                ImGui::EndChild();
            }
        }
        ImGui::EndChild();
        break;
    }

    ImGui::EndChild();
}

static const std::string GenerateKey(const int len) {

    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    srand((unsigned)time(NULL) * rand() % INT_MAX);

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];


    return tmp_s;

}

void Menu::Render()
{
    static int TabIndex = -1;

    if (ImGui::Button(XorStr("Rage"), { 62, 20 })) {
        TabIndex = -1;
    }
    ImGui::SameLine();
    if (ImGui::Button(XorStr("Antiaim"), { 62, 20 })) {
        TabIndex = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button(XorStr("Legit"), { 62, 20 })) {
        TabIndex = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button(XorStr("Visuals"), { 62, 20 })) {
        TabIndex = 2;
    }
    ImGui::SameLine();
    if (ImGui::Button(XorStr("Misc"), { 62, 20 })) {
        TabIndex = 3;
    }
    ImGui::SameLine();
    if (ImGui::Button(XorStr("Skins"), { 62, 20 })) {
        TabIndex = 4;
    }
    ImGui::SameLine();
    if (ImGui::Button(XorStr("Console"), { 63, 20 })) {
        TabIndex = 5;
    }

    switch (TabIndex) {
    case -1:
        RenderRage();
        break;
    case 0:
        RenderAntiAim();
        break;
    case 1:
        break;
    case 2:
        RenderVisuals();
        break;
    case 3:
        RenderMisc();
        break;
    case 4:
        break;
    case 5:
        RenderConsole();
        break;
    }

    ImGui::SetCursorPos({ 7, 396 });
#ifdef _DEBUG
    ImGui::Text(XorStr("Debug Build"));
#else
    ImGui::Text(XorStr("Stable Build"));
#endif

    static bool InitKey = true;
    static std::string Key;
    if (InitKey) {
        Key = GenerateKey(25);
        InitKey = false;
    }

    ImGui::SetCursorPos({ 316, 396 });
    ImGui::TextColored(ImColor(255, 255, 255, 15), Key.c_str());
}