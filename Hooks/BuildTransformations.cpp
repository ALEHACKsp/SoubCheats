#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

BuildTransformations_t BuildTransformations_h;

void __fastcall Hooks::BuildTransformations(void* ECX, void* EDX, int A2, int A3, int A4, int A5, int A6, int A7)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached BuildTransformations.")); Init = false; }

	auto Entity = (C_BaseEntity*)(ECX);

	Entity->JiggleBones() = false;

	return BuildTransformations_h(ECX, EDX, A2, A3, A4, A5, A6, A7);
}