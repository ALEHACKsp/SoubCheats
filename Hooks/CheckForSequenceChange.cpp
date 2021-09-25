#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../SDK/ClientState.h"
#include "../Features/Features.h"

CheckForSequenceChange_t CheckForSequenceChange_h;

void __fastcall Hooks::CheckForSequenceChange(void* ECX, void* EDX, void* HDR, int CurrentSequence, bool ForceNewSequence, bool Interpolate) 
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached CheckForSequenceChange.")); Init = false; }

    return CheckForSequenceChange_h(ECX, EDX, HDR, CurrentSequence, ForceNewSequence, false);
}