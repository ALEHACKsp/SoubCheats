#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

bool __fastcall Hooks::ShouldInterpolate(void* ECX)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached ShouldInterpolate.")); Init = false; }

	auto Entity = (C_BaseEntity*)(ECX);

	if (Entity == g::pLocalEntity)
		return true;

	if (Entity)
		return false;

	return false;
}