#include "Menu.h"
#include "TGFCfg.h"

#include "../Security/Xor.h"

void CConfig::Setup()
{
	SetupValue(g_Menu.Config.AntiUntrusted, true, XorStr("anti_untrusted"));

	SetupValue(g_Menu.Config.EnableESP, false, XorStr("enable_esp"));
	SetupValue(g_Menu.Config.EnableFadeESP, false, XorStr("enable_dormant_esp"));
	SetupValue(g_Menu.Config.BoxESP, false, XorStr("box_esp"));
	SetupValue(g_Menu.Config.NameESP, false, XorStr("name_esp"));
	SetupValue(g_Menu.Config.WeaponESPType, 0, XorStr("weapon_esp_type"));
	SetupValue(g_Menu.Config.WeaponESP, false, XorStr("weapon_esp"));
	SetupValue(g_Menu.Config.HealthESP, false, XorStr("health_bar_esp"));
	SetupValue(g_Menu.Config.HealthCustomColor, false, XorStr("health_bar_use_custom_color"));
	SetupValue(g_Menu.Config.AmmoESP, false, XorStr("ammo_esp"));

	SetupValue(g_Menu.Config.InfoESP, false, XorStr("info_esp"));
	SetupValue(g_Menu.Config.InfoMoney, false, XorStr("info_show_money"));
	SetupValue(g_Menu.Config.InfoArmor, false, XorStr("info_show_armor"));
	SetupValue(g_Menu.Config.InfoZoom, false, XorStr("info_show_if_scoped"));
	SetupValue(g_Menu.Config.InfoBombCarrier, false, XorStr("info_show_bomb_carrier"));
	SetupValue(g_Menu.Config.InfoFlashed, false, XorStr("info_show_if_flashed"));
	SetupValue(g_Menu.Config.InfoImmune, false, XorStr("info_show_if_entity_is_immune"));
	SetupValue(g_Menu.Config.InfoMoney, false, XorStr("info_show_if_entity_is_lethal"));

	SetupValue(g_Menu.Config.EnableRemovals, false, XorStr("enable_removals"));
	SetupValue(g_Menu.Config.RemoveScope, false, XorStr("remove_scope"));
	SetupValue(g_Menu.Config.RemoveZoom, false, XorStr("remove_zoom"));
	SetupValue(g_Menu.Config.RemoveAimPunch, false, XorStr("remove_view_aim_punch"));
	SetupValue(g_Menu.Config.RemoveSmoke, false, XorStr("remove_smoke"));
	SetupValue(g_Menu.Config.RemoveFlash, false, XorStr("remove_flash_effect"));
	SetupValue(g_Menu.Config.RemoveSleeves, false, XorStr("remove_sleeves"));
	SetupValue(g_Menu.Config.RemoveShadows, false, XorStr("remove_shadows"));

	SetupColorValue(g_Menu.Config.ScopeLine, XorStr("scope_lines_color"));
	SetupColorValue(g_Menu.Config.BoxColor, XorStr("box_esp_color"));
	SetupColorValue(g_Menu.Config.NameColor, XorStr("name_esp_color"));
	SetupColorValue(g_Menu.Config.WeaponColor, XorStr("weapon_esp_color"));
	SetupColorValue(g_Menu.Config.AmmoColor, XorStr("ammo_esp_color"));
	SetupColorValue(g_Menu.Config.HealthColor, XorStr("health_esp_color"));

	SetupValue(g_Menu.Config.EnableChams, false, XorStr("enable_player_chams"));
	SetupValue(g_Menu.Config.EnableHidChams, false, XorStr("enable_hidden_player_chams"));
	SetupColorValue(g_Menu.Config.VisChamsColor, XorStr("player_chams_color"));
	SetupColorValue(g_Menu.Config.HidChamsColor, XorStr("hidden_player_chams_color"));

	SetupValue(g_Menu.Config.ChamsMat, 0, XorStr("player_chams_mat"));

	SetupValue(g_Menu.Config.EnablePlayerGlow, false, XorStr("enable_player_glow"));
	SetupColorValue(g_Menu.Config.PlayerGlowColor, XorStr("player_glow_color"));

	SetupValue(g_Menu.Config.DebugFov, 90, XorStr("debug_fov"));
	SetupValue(g_Menu.Config.DebugViewFov, 68, XorStr("debug_viewmodel_fov"));

	SetupValue(g_Menu.Config.Bhop, false, XorStr("enable_bunnyhopping"));
	SetupValue(g_Menu.Config.BhopHitchance, 0, XorStr("legit_bhops_hitchance"));
	SetupValue(g_Menu.Config.BhopsRestricted, 0, XorStr("legit_bhops_max"));
	SetupValue(g_Menu.Config.MaxBhops, 0, XorStr("legit_bhops_max_hits"));

	SetupValue(g_Menu.Config.AutoStrafe, false, XorStr("enable_auto_strafer"));
	SetupValue(g_Menu.Config.AutoStrafeType, 0, XorStr("auto_strafer_type"));

	SetupValue(g_Menu.Config.Thirdperson, false, XorStr("enable_force_thirdperson"));
	SetupValue(g_Menu.Config.ThirdpersonKey, 0, XorStr("thirdperson_key"));
	SetupValue(g_Menu.Config.ThirdpersonKeyType, 0, XorStr("thirdperson_key_type"));
	SetupValue(g_Menu.Config.ThirdpersonDist, 150, XorStr("thirdperson_distance"));

	SetupValue(g_Menu.Config.EdgeJump, false, XorStr("enable_edge_jump"));
	SetupValue(g_Menu.Config.EdgeJumpKey, 0, XorStr("edge_jump_key"));
	SetupValue(g_Menu.Config.EdgeJumpKeyType, 0, XorStr("edge_jump_key_type"));

	SetupValue(g_Menu.Config.Clantag, false, XorStr("enable_clantag_spammer"));

	SetupValue(g_Menu.Config.DisablePostEffects, false, XorStr("disable_postprocessing_effects"));
	SetupValue(g_Menu.Config.RevealHiddenCvars, false, XorStr("reveal_hidden_convars"));

	SetupValue(g_Menu.Config.ForceCheats, false, XorStr("force_sv_cheats"));

	SetupValue(g_Menu.Config.AllowViewModify, false, XorStr("allow_view_angle_modification"));
	SetupValue(g_Menu.Config.ViewOffsetX, 0, XorStr("view_model_offset_x"));
	SetupValue(g_Menu.Config.ViewOffsetY, 0, XorStr("view_model_offset_y"));
	SetupValue(g_Menu.Config.ViewOffsetZ, 0, XorStr("view_model_offset_z"));

	SetupValue(g_Menu.Config.BuyBotEnable, false, XorStr("enable_auto_purchase"));
	SetupValue(g_Menu.Config.BuyMisc, false, 4,XorStr("misc_items_to_purchase"));
	SetupValue(g_Menu.Config.BuyBotWeapons, 0, XorStr("auto_purchase_primary"));
	SetupValue(g_Menu.Config.BuyBotPistols, 0, XorStr("auto_purchase_secondary"));
	SetupValue(g_Menu.Config.BuyFlash, false, XorStr("auto_purchase_flash"));
	SetupValue(g_Menu.Config.BuyDecoy, false, XorStr("auto_purchase_decoy"));

	SetupValue(g_Menu.Config.EnableRagebot, false, XorStr("enable_rage_aimbot"));
	SetupValue(g_Menu.Config.RagebotKey, 0, XorStr("rage_aimbot_key"));
	SetupValue(g_Menu.Config.RagebotKeyType, 0, XorStr("rage_aimbot_key_type"));

	SetupValue(g_Menu.Config.RagebotFov, 180, XorStr("rage_aimbot_max_field_of_view"));

	SetupValue(g_Menu.Config.TargetMode, 0, XorStr("rage_aimbot_targeting_mode"));

	SetupValue(g_Menu.Config.SilentAim, false, XorStr("rage_aimbot_silent_aim"));
	SetupValue(g_Menu.Config.NoAimRecoil, false, XorStr("rage_aimbot_no_recoil"));

	SetupValue(g_Menu.Config.HitboxesToScan, 0, XorStr("rage_aimbot_hitboxes_to_scan"));

	SetupValue(g_Menu.Config.Hitchance, 0, XorStr("rage_aimbot_hitchance"));
	SetupValue(g_Menu.Config.MinDamage, 0, XorStr("rage_aimbot_min_damage"));

	SetupValue(g_Menu.Config.Headscale, 0, XorStr("rage_aimbot_head_scale"));
	SetupValue(g_Menu.Config.Bodyscale, 0, XorStr("rage_aimbot_body_scale"));

	SetupValue(g_Menu.Config.AutoScope, false, XorStr("rage_aimbot_auto_scope"));
	SetupValue(g_Menu.Config.AutoStop, false, XorStr("rage_aimbot_auto_stop"));
	SetupValue(g_Menu.Config.StopBetweenShots, false, XorStr("rage_aimbot_stop_between_shots"));
	SetupValue(g_Menu.Config.DontShootLagComp, false, XorStr("rage_aimbot_dont_shoot_entitys_breaking_lag_comp"));

	SetupValue(g_Menu.Config.DoubleTap, false, XorStr("exploits_enable_doubletap"));
	SetupValue(g_Menu.Config.DoubleTapInstant, false, XorStr("exploits_enable_doubletap_instant"));
	SetupValue(g_Menu.Config.DoubleTapTeleport, false, XorStr("exploits_enable_doubletap_teleport"));
	SetupValue(g_Menu.Config.DoubleTapKey, 0, XorStr("exploits_double_tap_key"));
	SetupValue(g_Menu.Config.DoubleTapKeyType, 0, XorStr("exploits_double_tap_key_type"));

	SetupValue(g_Menu.Config.EnableAntiAim, false, XorStr("enable_anti_aim"));
	SetupValue(g_Menu.Config.EnableFakeLag, false, XorStr("enable_fake_lag"));
	SetupValue(g_Menu.Config.FakelagAmmount, 0, XorStr("fake_lag_max_limit"));

	SetupValue(g_Menu.Config.FastDuck, false, XorStr("enable_fast_duck"));
}

void CConfig::SetupValue(int& value, int def, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue< int >(name, &value));
}

void CConfig::SetupValue(float& value, float def, std::string name)
{
	value = def;
	floats.push_back(new ConfigValue< float >(name, &value));
}

void CConfig::SetupValue(bool& value, bool def, std::string name)
{
	value = def;
	bools.push_back(new ConfigValue< bool >(name, &value));
}

void CConfig::SetupColorValue(float* value, std::string name)
{
	floats.push_back(new ConfigValue< float >(name + "_red", &value[0]));
	floats.push_back(new ConfigValue< float >(name + "_green", &value[1]));
	floats.push_back(new ConfigValue< float >(name + "_blue", &value[2]));
	floats.push_back(new ConfigValue< float >(name + "_alpha", &value[3]));
}

void CConfig::SetupValue(bool* value, bool def, int size, std::string name)
{
	for (int c = 0; c < size; c++)
	{
		value[c] = def;

		name += std::to_string(c);

		bools.push_back(new ConfigValue< bool >(name, &value[c]));
	}
}

void CConfig::Save()
{
	std::string folder, file;

	folder = "C:\\onetap.soub\\";
	switch (g_Menu.Config.ConfigList)
	{
	case 0: file = "C:\\onetap.soub\\auto"; break;
	case 1: file = "C:\\onetap.soub\\scout"; break;
	case 2: file = "C:\\onetap.soub\\awp"; break;
	case 3: file = "C:\\onetap.soub\\pistol"; break;
	case 4: file = "C:\\onetap.soub\\heavy_pistol"; break;
	case 5: file = "C:\\onetap.soub\\general"; break;
	case 6: file = "C:\\onetap.soub\\legit"; break;
	}

	CreateDirectory("C:\\onetap.soub\\", NULL);

	for (auto value : ints)
		WritePrivateProfileString("soub_config_parser", value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : floats)
		WritePrivateProfileString("soub_config_parser", value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileString("soub_config_parser", value->name.c_str(), *value->value ? "true" : "false", file.c_str());
}

void CConfig::Load()
{
	std::string folder, file;

	folder = "C:\\onetap.soub\\";
	switch (g_Menu.Config.ConfigList)
	{
	case 0: file = "C:\\onetap.soub\\auto"; break;
	case 1: file = "C:\\onetap.soub\\scout"; break;
	case 2: file = "C:\\onetap.soub\\awp"; break;
	case 3: file = "C:\\onetap.soub\\pistol"; break;
	case 4: file = "C:\\onetap.soub\\heavy_pistol"; break;
	case 5: file = "C:\\onetap.soub\\general"; break;
	case 6: file = "C:\\onetap.soub\\legit"; break;
	}

	CreateDirectory(folder.c_str(), NULL);

	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileString("soub_config_parser", value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atoi(value_l);
	}

	for (auto value : floats)
	{
		GetPrivateProfileString("soub_config_parser", value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atof(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileString(XorStr("soub_config_parser"), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = !strcmp(value_l, "true");
	}
}

CConfig* g_Config = new CConfig();