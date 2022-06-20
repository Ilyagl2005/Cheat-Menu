#include "pch.h"
#include "cheatmenu.h"
#include "updater.h"
#include "rpc.h"

void MenuThread(void* param)
{
    /* 
        Wait for game init
        // Sleep(3000);
        Doing it like this doesn't prevent from attaching a debugger
    */ 
    static bool gameStarted = false;
    Events::processScriptsEvent +=[]
    {
        gameStarted = true;
    };

    while (!gameStarted)
    {
        Sleep(500);
    }
    
    /*
    	Had to put this in place since some people put the folder in root
    	directory and the asi in modloader. Why??
    */
    if (!std::filesystem::is_directory(PLUGIN_PATH((char*)FILE_NAME)))
    {
        std::string msg = std::format("{} folder not found. You need to put both '{}.asi' & '{}' folder in the same directory", FILE_NAME, FILE_NAME, FILE_NAME);
        Log::Print<eLogLevel::Error>(msg.c_str());
        MessageBox(NULL, msg.c_str(), FILE_NAME, MB_ICONERROR);
        return;
    }

    /*
    	Need SilentPatch since all gta games have issues with mouse input
    	Implementing mouse fix is a headache anyway
    */
    if (!GetModuleHandle(BY_GAME("SilentPatchSA.asi","SilentPatchVC.asi","SilentPatchIII.asi")))
    {
        Log::Print<eLogLevel::Error>("SilentPatch not found. Please install it from here https://gtaforums.com/topic/669045-silentpatch/");
        int msgID = MessageBox(NULL, "SilentPatch not found. Do you want to install Silent Patch? (Game restart required)", FILE_NAME, MB_OKCANCEL | MB_DEFBUTTON1);

        if (msgID == IDOK)
        {
            ShellExecute(nullptr, "open", "https://gtaforums.com/topic/669045-silentpatch/", nullptr, nullptr, SW_SHOWNORMAL);
        };
        return;
    }

    /*
        TODO: Find a better way
        Since you could still name it something else
    */
#ifdef GTASA
    if (GetModuleHandle("SAMP.dll") || GetModuleHandle("SAMP.asi"))
    {
        Log::Print<eLogLevel::Error>(FILE_NAME " doesn't support SAMP");
        MessageBox(NULL, "SAMP detected. Exiting " FILE_NAME, FILE_NAME, MB_ICONERROR);
        return;
    }
    CFastman92limitAdjuster::Init();
#elif GTAVC
    if (GetModuleHandle("vcmp-proxy.dll") || GetModuleHandle("vcmp-proxy.asi"))
    {
        Log::Print<eLogLevel::Error>(FILE_NAME " doesn't support VCMP");
        MessageBox(NULL, "VCMP detected. Exiting " FILE_NAME, FILE_NAME, MB_ICONERROR);
        return;
    }
#endif

    Log::Print<eLogLevel::None>("Stating " MENU_TITLE "\nAuthor: Grinch_\nDiscord: " DISCORD_INVITE "\nMore Info: " GITHUB_LINK "\n");

    CheatMenu::Init();

    // Checking for updates once a day
    SYSTEMTIME st;
    GetSystemTime(&st);
    if (gConfig.Get("Menu.LastUpdateChecked", 0) != st.wDay)
    {
        Updater::CheckUpdate();
        Updater::IncrementDailyUsageCounter();
        gConfig.Set("Menu.LastUpdateChecked", st.wDay);
    }

    while (true)
    {
        Updater::Process();
        RPC::Process();
        Sleep(1000);
    }
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
        uint gameVer = GetGameVersion();

#ifdef GTASA
        if (gameVer == GAME_10US_HOODLUM || gameVer == GAME_10US_COMPACT)
#else
        if (gameVer == BY_GAME(NULL, GAME_10EN, GAME_10EN))
#endif
        {
            CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&MenuThread, nullptr, NULL, nullptr);
        }
        else
        {
            Log::Print<eLogLevel::Error>("Unknown game version. GTA " BY_GAME("SA v1.0 US Hoodlum or Compact", "VC v1.0 EN", "III v1.0 EN") " is required.");
            MessageBox(HWND_DESKTOP, "Unknown game version. GTA " BY_GAME("SA v1.0 US Hoodlum or Compact", "VC v1.0 EN", "III v1.0 EN") " is required.", FILE_NAME, MB_ICONERROR);
        }
    }

    return TRUE;
}