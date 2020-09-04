/*

Visual#9999
*/

#include "Header Files/includes.h"
#include "Header Files/menu/menu.h"
#include "Header Files/config/Config.h"

VOID Main() {
    SettingsHelper::Initialize();
    if (!Util::Initialize()) {
        return;
    }

    if (!Offsets::Initialize()) {
        return;
    }

    if (!Core::Initialize()) {
        return;
    }

    if (!Render::Initialize()) {
        return;
    }
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        Main();
    }

    return TRUE;
}