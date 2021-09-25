#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

ShouldSkipAnimFrame_t ShouldSkipAnimFrame_h;

bool __fastcall Hooks::ShouldSkipAnimationFrame(void* ECX, void* EDX)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached ShouldSkipAnimationFrame.")); Init = false; }

    return false;
}