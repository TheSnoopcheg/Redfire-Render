#include "CRender.h"
#include "dllmain.h"

std::uintptr_t CRender::FindDevice(std::uint32_t Len) {
    static std::uintptr_t base = [](std::size_t Len) {
        std::string path_to(MAX_PATH, '\0');
        if (auto size = GetSystemDirectoryA(path_to.data(), MAX_PATH)) {
            path_to.resize(size);
            path_to += "\\d3d9.dll";
            std::uintptr_t dwObjBase = reinterpret_cast<std::uintptr_t>(LoadLibraryA(path_to.c_str()));
            while (dwObjBase++ < dwObjBase + Len) {
                if (*reinterpret_cast<std::uint16_t*>(dwObjBase + 0x00) == 0x06C7 &&
                    *reinterpret_cast<std::uint16_t*>(dwObjBase + 0x06) == 0x8689 &&
                    *reinterpret_cast<std::uint16_t*>(dwObjBase + 0x0C) == 0x8689) {
                    dwObjBase += 2;
                    break;
                }
            }
            return dwObjBase;
        }
        return std::uintptr_t(0);
    }(Len);
    return base;
}

void* CRender::GetFunctionAddress(int VTableIndex) {
    return (*reinterpret_cast<void***>(FindDevice(0x128000)))[VTableIndex];
}

HRESULT __stdcall CRender::OnPresent(IDirect3DDevice9* device_ptr, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
    static bool ImGui_init = false;
    if (!ImGui_init) {
        pMenu = new CMenu(device_ptr);
        ImGui_init = true;
    }
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    pMenu->Render();
    pRObjects->Render();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    return pRender->origPresentSignature(device_ptr, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT __stdcall CRender::OnReset(IDirect3DDevice9* device_ptr, D3DPRESENT_PARAMETERS* parameters) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    return pRender->origResetSignature(device_ptr, parameters);
}