#include "../Entry.h"
#include "../Menu/Menu.h"
#include "../Utils/Math.h"
#include "../Features/Features.h"

#include "../Dependencies/ImGui/imgui.h"
#include "../Dependencies/ImGui/imgui_impl_dx9.h"
#include "../Dependencies/ImGui/imgui_impl_win32.h"
#include "../Dependencies/ImGui/imgui_internal.h"

IDirect3DStateBlock9* PixelState = NULL; 
IDirect3DVertexDeclaration9* VertDec; 
IDirect3DVertexShader9* VertShader;
DWORD OldD3drsColorWriteEnable;

void SaveState(IDirect3DDevice9* Device)
{
	Device->GetRenderState(D3DRS_COLORWRITEENABLE, &OldD3drsColorWriteEnable);
	Device->GetVertexDeclaration(&VertDec);
	Device->GetVertexShader(&VertShader);
	Device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	Device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	Device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	Device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	Device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
}

void RestoreState(IDirect3DDevice9* Device)
{
	Device->SetRenderState(D3DRS_COLORWRITEENABLE, OldD3drsColorWriteEnable);
	Device->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
	Device->SetVertexDeclaration(VertDec);
	Device->SetVertexShader(VertShader);
}

long __stdcall Hooks::EndScene(IDirect3DDevice9* Device)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached EndScene.")); Init = false; }

	static auto oEndScene = g_Hooks.pDirectXHook->GetOriginal<EndScene_t>(HookIndexes::EndScene);

	static auto WantedReturnddress = _ReturnAddress();
	if (_ReturnAddress() == WantedReturnddress)
	{
		static bool Init = true;
		if (Init)
		{
			ImGui::CreateContext();
			ImGui_ImplDX9_Init(Device);
			ImGui_ImplWin32_Init(g_Hooks.hCSGOWindow);
			Init = false;
		}

		SaveState(Device);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		g_Menu.CreateStyle();

		if (g_Menu.menuOpened)
		{
			DWORD WindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::SetNextWindowPos({ 200, 200 }, ImGuiCond_Once);
			ImGui::SetNextWindowSize({ 500, 416 }, ImGuiCond_Once);
			ImGui::Begin(XorStr(" soub framework - current version will not resemble final product"), NULL, WindowFlags);
			{
				g_Menu.Render();
			}
			ImGui::End();
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		RestoreState(Device);
	}

	return oEndScene(Device);
}

long __stdcall Hooks::EndSceneReset(IDirect3DDevice9* Device, D3DPRESENT_PARAMETERS* PresentationParameters)
{
	static bool Init = true;
	if (Init) { Utils::Log(XorStr("Attached EndSceneReset.")); Init = false; }

	static auto oEndSceneReset = g_Hooks.pDirectXHook->GetOriginal<EndSceneReset_t>(HookIndexes::SceneReset);

	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto ResetReturn = oEndSceneReset(Device, PresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();

	return ResetReturn;
}