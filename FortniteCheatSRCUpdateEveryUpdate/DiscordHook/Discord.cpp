/*


*/
#include <cstdint>
#include <Windows.h>
#include "../Helper/Helper.h"
#include "Discord.h"

#define DEVELOPER

uintptr_t Discord::GetDiscordModuleBase()
{
    // This is static because we only need to get once.
    static uintptr_t discordModuleBase = 0;

    // If its false, we use GetModuleHandle to grab the Module Base adress. 
    if (!discordModuleBase)
        discordModuleBase = (uintptr_t)GetModuleHandleA("DiscordHook64.dll");

    return discordModuleBase;
}

bool Discord::CreateHook(uintptr_t originalPresent, uintptr_t hookFunction, uintptr_t pOriginal)
{

    static uintptr_t addrCreateHook = NULL;

    if (!addrCreateHook)
    {
        addrCreateHook = Helper::PatternScan(GetDiscordModuleBase(), "41 57 41 56 56 57 55 53 48 83 EC 68 4D 89 C6 49 89 D7");

#ifdef DEVELOPER
#endif
    }

    if (!addrCreateHook)
        return false;

    using CreateHook_t = uint64_t(__fastcall*)(LPVOID, LPVOID, LPVOID*);
    auto fnCreateHook = (CreateHook_t)addrCreateHook;
    return fnCreateHook((void*)originalPresent, (void*)hookFunction, (void**)pOriginal) == 0 ? true : false;

}

bool Discord::EnableHook(uintptr_t pTarget, bool toggle)
{
    static uintptr_t addrEnableHook = NULL;

    if (!addrEnableHook)
    {
        addrEnableHook = Helper::PatternScan(GetDiscordModuleBase(),
            "41 56 56 57 53 48 83 EC 28 49 89 CE BF 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74"
        );
    }

    if (!addrEnableHook)
        return false;

    using EnableHook_t = uint64_t(__fastcall*)(LPVOID, bool);
    auto fnEnableHook = (EnableHook_t)addrEnableHook;

    return fnEnableHook((void*)pTarget, toggle) == 0 ? true : false;
}

bool Discord::EnableHookQue()
{
    static uintptr_t addrEnableHookQueu = NULL;

    if (!addrEnableHookQueu)
    {
        addrEnableHookQueu = Helper::PatternScan(GetDiscordModuleBase(),
            "41 57 41 56 41 55 41 54 56 57 55 53 48 83 EC 38 48 ? ? ? ? ? ? 48 31 E0 48 89 44 24 30 BE 01 00 00 00 31 C0 F0 ? ? ? ? ? ? ? 74 2B");

    }

    if (!addrEnableHookQueu)
        return false;

    using EnableHookQueu_t = uint64_t(__stdcall*)(VOID);
    auto fnEnableHookQueu = (EnableHookQueu_t)addrEnableHookQueu;

    return fnEnableHookQueu() == 0 ? true : false;
}

short Discord::GetAsyncKeyState(const int vKey)
{
    static uintptr_t addrGetAsyncKeyState = NULL;

    if (!addrGetAsyncKeyState)
    {
        addrGetAsyncKeyState = Helper::PatternScan(GetDiscordModuleBase(),
            "48 FF ? ? ? ? ? CC CC CC CC CC CC CC CC CC 48 FF ? ? ? ? ? CC CC CC CC CC CC CC CC CC 48 83 EC 28 48 ? ? ? ? ? ? 48 85 C9");
    }

    if (!addrGetAsyncKeyState)
        return false;

    using GetAsyncKeyState_t = short(__fastcall*)(int);
    auto fnGetAyncKeyState = (GetAsyncKeyState_t)addrGetAsyncKeyState;

    return fnGetAyncKeyState(vKey);
}

short Discord::SetCursorPos(int x, int y)
{
    static uintptr_t addrSetCursorPos = NULL;

    if (!addrSetCursorPos)
    {
        addrSetCursorPos = Helper::PatternScan(GetDiscordModuleBase(),
            "44 0F B6 05 ? ? ? ? 45 84 C0");

#ifdef DEVELOPER
#endif
    }

    if (!addrSetCursorPos)
        return false;

    using SetCursorPos_t = short(__fastcall*)(int, int);
    auto fnSetCursorPos = (SetCursorPos_t)addrSetCursorPos;

    return fnSetCursorPos(x, y);
}


bool Discord::GetCursorPos(LPPOINT lpPoint)
{
    static uintptr_t addrGetCursorPos = NULL;

    if (!addrGetCursorPos)
    {
        addrGetCursorPos = Helper::PatternScan(GetDiscordModuleBase(),
            "40 53 48 83 EC 20 48 8B D9 FF 15 ? ? ? ? 0F B6 15 ? ? ? ?");

#ifdef DEVELOPER
#endif
    }

    if (!addrGetCursorPos)
        return false;

    using GetCursorPos_t = short(__fastcall*)(LPPOINT);
    auto fnGetCursorPos = (GetCursorPos_t)addrGetCursorPos;

    return fnGetCursorPos(lpPoint);
}

HCURSOR Discord::SetCursor(HCURSOR hCursor)
{
    static uintptr_t addrSetCursor = NULL;

    if (!addrSetCursor)
    {
        addrSetCursor = Helper::PatternScan(GetDiscordModuleBase(),
            "40 53 48 83 EC 20 0F B6 05 ? ? ? ? 48 8B D9 84 C0 74 4F");

#ifdef DEVELOPER
#endif
    }

    if (!addrSetCursor)
        return 0;

    using SetCursor_t = HCURSOR(__fastcall*)(HCURSOR);
    auto fnSetCursor = (SetCursor_t)addrSetCursor;

    return fnSetCursor(hCursor);
}

bool Discord::HookFunction(uintptr_t originalFunction, uintptr_t hookedFunction, uintptr_t pOriginalPresent)
{
#ifdef DEVELOPER
#endif


    if (DISCORD.CreateHook(originalFunction, hookedFunction, pOriginalPresent))
    {
#ifdef DEVELOPER
#endif
        if (DISCORD.EnableHook(originalFunction, true))
        {
#ifdef DEVELOPER
#endif

            if (DISCORD.EnableHookQue())
            {
#ifdef DEVELOPER
#endif
                return true;
            }
        }
    }

    return false;
}
