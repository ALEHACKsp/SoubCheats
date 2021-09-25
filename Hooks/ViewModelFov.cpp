#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

float __fastcall Hooks::GetViewmodelFOV()
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached GetViewmodelFOV.")); Init = false; }

	return g_Menu.Config.DebugViewFov;
}