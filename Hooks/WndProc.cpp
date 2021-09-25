#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Hooks::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached WndProc.")); Init = false; }

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		g::PressedKeys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		g::PressedKeys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		g::PressedKeys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		g::PressedKeys[VK_RBUTTON] = false;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		g::PressedKeys[VK_MBUTTON] = true;
		break;
	case WM_MBUTTONUP:
		g::PressedKeys[VK_MBUTTON] = false;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONDBLCLK:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			g::PressedKeys[VK_XBUTTON1] = true;
		}
		else if (button == XBUTTON2)
		{
			g::PressedKeys[VK_XBUTTON2] = true;
		}
		break;
	}
	case WM_XBUTTONUP:
	{
		UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			g::PressedKeys[VK_XBUTTON1] = false;
		}
		else if (button == XBUTTON2)
		{
			g::PressedKeys[VK_XBUTTON2] = false;
		}
		break;
	}
	case WM_KEYDOWN:
	{
		g::PressedKeys[wParam] = true;

		if (wParam == VK_INSERT)
			g_Menu.menuOpened = !g_Menu.menuOpened;

		break;
	}
	case WM_KEYUP:
		g::PressedKeys[wParam] = false;
		break;
	case WM_SYSKEYDOWN:
		if (wParam == VK_MENU)
			g::PressedKeys[VK_MENU] = true;
		break;
	case WM_SYSKEYUP:
		if (wParam == VK_MENU)
			g::PressedKeys[VK_MENU] = false;
		break;
	default: break;
	}

	if (g_Menu.menuOpened && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProcA(g_Hooks.pOriginalWNDProc, hWnd, uMsg, wParam, lParam);
}
