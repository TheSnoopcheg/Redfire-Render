#include "dllmain.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CRender* pRender;
CRObjects* pRObjects;
CMenu* pMenu;

typedef void(__cdecl* CTimerUpdate)();
CTimerUpdate origCTimerUpdate;

void __cdecl HOOKED_CTimerUpdate() {
    static bool init = false;
    if (!init) {
        pRender = new CRender();
        pRObjects = new CRObjects();
        init = true;
    }

    return origCTimerUpdate();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MH_Initialize();
        MH_CreateHook(reinterpret_cast<void*>(0x561B10), &HOOKED_CTimerUpdate, reinterpret_cast<LPVOID*>(&origCTimerUpdate));
        MH_EnableHook(reinterpret_cast<void*>(0x561B10));
        break;
    case DLL_PROCESS_DETACH:
        MH_DisableHook(reinterpret_cast<void*>(0x561B10));
        break;
    }
    return TRUE;
}