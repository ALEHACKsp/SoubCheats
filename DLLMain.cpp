#include <thread>
#include "Entry.h"
#include "Utils\Utils.h"
#include "Utils\GlobalVars.h"
#include "Security/Xor.h"
#include "Menu/Menu.h"

#define BUILD_NUMBER 13794

BOOL DirectoryExists(LPCTSTR Path)
{
    DWORD Attrib = GetFileAttributes(Path);
    return (Attrib != INVALID_FILE_ATTRIBUTES && (Attrib & FILE_ATTRIBUTE_DIRECTORY));
}

DWORD WINAPI OnDllAttach(LPVOID Instance)
{
    while (!(GetModuleHandleA(XorStr("engine.dll"))
        && GetModuleHandleA(XorStr("client.dll"))
        && GetModuleHandleA(XorStr("serverbrowser.dll"))))
        Sleep(200);

#ifdef _DEBUG
    AllocConsole();                                 
    freopen_s((FILE**)stdin, XorStr("CONIN$"), XorStr("r"), stdin);
    freopen_s((FILE**)stdout, XorStr("CONOUT$"), XorStr("w"), stdout);
    SetConsoleTitleA(XorStr(" Debug console"));
#endif

    if (!DirectoryExists(XorStr("C:\\onetap.soub\\")))
    {
        CreateDirectory(XorStr("C:\\onetap.soub\\"), NULL);
        Utils::Log(XorStr("Created config directory."));
    }

    try
    {
        Interfaces::Init();
        Utils::Log(XorStr("Interfaces set."));
        if (g_pEngine->GetEngineBuildNumber() == BUILD_NUMBER)
        {
            Hooks::InitMinHook();
            Hooks::Init();

            //std::thread ThreadOne(Hooks::InitMinHook);
            //std::thread ThreadTwo(Hooks::Init);

            //ThreadTwo.join();
            //ThreadOne.join();
        }
        else
        {
#ifdef _DEBUG
            std::stringstream ss; 
            ss << "engine number mismatch!" << " current engine number: " << g_pEngine->GetEngineBuildNumber() << " | past engine number: " << BUILD_NUMBER;
            MessageBoxA(NULL, XorStr(ss.str().c_str()), XorStr("onetap.soub"), MB_OK | MB_ICONERROR);
            FreeLibraryAndExitThread(static_cast<HMODULE>(Instance), EXIT_SUCCESS);
#else
            MessageBoxA(NULL, XorStr("engine number mismatch!"), XorStr("onetap.soub"), MB_OK | MB_ICONERROR);
            FreeLibraryAndExitThread(static_cast<HMODULE>(Instance), EXIT_SUCCESS);
#endif
            Sleep(100);
            FreeLibraryAndExitThread(static_cast<HMODULE>(Instance), EXIT_SUCCESS);
        }

#ifdef _DEBUG
        while (!g::PressedKeys[VK_END])
            Sleep(100);
#else
        while (true)
            Sleep(INT_MAX);
#endif

        FreeLibraryAndExitThread(static_cast<HMODULE>(Instance), EXIT_SUCCESS);
    }
    catch (const std::exception& ex)
    {
#ifdef _DEBUG
        MessageBoxA(NULL, ex.what(), XorStr("error!"), MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(static_cast<HMODULE>(Instance), EXIT_SUCCESS);
#else
        MessageBoxA(NULL, XorStr("error durring cheat startup!"), XorStr("onetap.soub"), MB_OK | MB_ICONERROR);
        FreeLibraryAndExitThread(static_cast<HMODULE>(Instance), EXIT_SUCCESS);
#endif
    }
}

BOOL WINAPI OnDllDetach()
{
    Hooks::Restore();
    return TRUE;
}

BOOL APIENTRY DllMain(CONST HMODULE DLL, CONST DWORD Reason, CONST LPVOID Reserved)
{
    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
        if (GetModuleHandleA(XorStr("csgo.exe"))) 
        {
            DisableThreadLibraryCalls(DLL);
            CreateThread(nullptr, 0, OnDllAttach, DLL, 0, nullptr);
            g::DLL = DLL;
        }
        return TRUE;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (Reserved == nullptr)
        {
            return OnDllDetach();
        }
        return TRUE;
    default:
        return TRUE;
    }
}
