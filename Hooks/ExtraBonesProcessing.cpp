#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

void __fastcall Hooks::DoExtraBonesProcessing(void* ECX, void* EDX, void* UNKN1, void* UNKN2, void* UNKN3, void* UNKN4, CBoneBitList& UNKN5, void* UNKN6)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached DoExtraBonesProcessing.")); Init = false; }

	return;
}
