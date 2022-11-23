#include "CMenu.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WNDPROC old_wndproc;

bool objectRenderStatus[7] = { false, false, false, false, false, false, false };
bool lineRenderStatus = false;
bool renderWindow = false;

inline auto PauseScreen(bool state) {
    static DWORD
        updateMouseProtection,
        rsMouseSetPosProtFirst,
        rsMouseSetPosProtSecond;

    if (state) {
        ::VirtualProtect(reinterpret_cast<void*>(0x53F3C6U), 5U, PAGE_EXECUTE_READWRITE, &updateMouseProtection);
        ::VirtualProtect(reinterpret_cast<void*>(0x53E9F1U), 5U, PAGE_EXECUTE_READWRITE, &rsMouseSetPosProtFirst);
        ::VirtualProtect(reinterpret_cast<void*>(0x748A1BU), 5U, PAGE_EXECUTE_READWRITE, &rsMouseSetPosProtSecond);

        *reinterpret_cast<uint8_t*>(0x53F3C6U) = 0xE9U;
        *reinterpret_cast<uint32_t*>(0x53F3C6U + 1U) = 0x15BU;

        memset(reinterpret_cast<void*>(0x53E9F1U), 0x90, 5U);
        memset(reinterpret_cast<void*>(0x748A1BU), 0x90, 5U);
    }
    else {
        memcpy(reinterpret_cast<void*>(0x53F3C6U), "\xE8\x95\x6C\x20\x00", 5U);
        memcpy(reinterpret_cast<void*>(0x53E9F1U), "\xE8\xAA\xAA\x0D\x00", 5U);
        memcpy(reinterpret_cast<void*>(0x748A1BU), "\xE8\x80\x0A\xED\xFF", 5U);

        using CPad_ClearMouseHistory_t = void(__cdecl*)();
        using CPad_UpdatePads_t = void(__cdecl*)();
        reinterpret_cast<CPad_ClearMouseHistory_t>(0x541BD0U)();
        reinterpret_cast<CPad_UpdatePads_t>(0x541DD0U)();

        ::VirtualProtect(reinterpret_cast<void*>(0x53F3C6U), 5U, updateMouseProtection, &updateMouseProtection);
        ::VirtualProtect(reinterpret_cast<void*>(0x53E9F1U), 5U, rsMouseSetPosProtFirst, &rsMouseSetPosProtFirst);
        ::VirtualProtect(reinterpret_cast<void*>(0x748A1BU), 5U, rsMouseSetPosProtSecond, &rsMouseSetPosProtSecond);
    } return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_KEYDOWN:
    {
        switch (wParam) {
        case VK_END:
            renderWindow = !renderWindow;
            PauseScreen(renderWindow);
            return true;
        case VK_ESCAPE:
            if (renderWindow) {
                renderWindow = !renderWindow;
                PauseScreen(renderWindow);
                return true;
            }
        }
    }
    }

    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    return CallWindowProcA(old_wndproc, hWnd, msg, wParam, lParam);
}

CMenu::CMenu(IDirect3DDevice9* device_ptr) {
    old_wndproc = (WNDPROC)SetWindowLongA(**reinterpret_cast<HWND**>(0xC17054), GWL_WNDPROC, (LONG)WndProc);
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(**reinterpret_cast<HWND**>(0xC17054));
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    io.WantCaptureMouse = true;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\trebucbd.ttf", 15.0f);
    this->Theme();
    ImGui_ImplDX9_Init(device_ptr);
}

CMenu::~CMenu() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void CMenu::Render() {
    if (renderWindow) ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    ImGui::GetIO().MouseDrawCursor = renderWindow;
    if (renderWindow) {
        unsigned long dwLenX = *reinterpret_cast<unsigned long*>(0xC17044);
        unsigned long dwLenY = *reinterpret_cast<unsigned long*>(0xC17048);
        ImGui::SetNextWindowPos(ImVec2(dwLenX / 2.0f, dwLenY / 2.0f), ImGuiCond_FirstUseEver, ImVec2(0.5, 0.5));
        ImGui::SetNextWindowSize(ImVec2(190, 250), ImGuiCond_FirstUseEver);
        ImGui::Begin("##begin", &renderWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("Render:");
        ImGui::Checkbox("Container", &objectRenderStatus[0]);
        ImGui::Checkbox("Corpse", &objectRenderStatus[1]);
        ImGui::Checkbox("Horseshoe", &objectRenderStatus[2]);
        ImGui::Checkbox("Totem", &objectRenderStatus[3]);
        ImGui::Checkbox("Loot", &objectRenderStatus[4]);
        ImGui::Checkbox("Materials", &objectRenderStatus[5]);
        ImGui::Checkbox("Horse", &objectRenderStatus[6]);
        ImGui::Checkbox("Draw a line to targets", &lineRenderStatus);
        //if (ImGui::Button("Unload plugin")) {
            //UnloadPlugin();
        //}
        ImGui::TextDisabled("Author: Snoopcheg");
        ImGui::End();
    }
}

void CMenu::Theme() {
    ImGui::GetStyle().WindowBorderSize = 0.0f;
    ImGui::GetStyle().WindowRounding = 5.5f;
    ImGui::GetStyle().FrameRounding = 10.0f;
    ImGui::GetStyle().GrabRounding = 10.0f;
    ImGui::GetStyle().FramePadding = ImVec2(3.5f, 3.5f);
    ImGui::GetStyle().WindowPadding = ImVec2(3.5f, 3.5f);
    ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.4f);
    ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.00f, 0.56f, 0.60f, 0.40f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.60f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.60f, 0.10f, 0.10f, 0.50f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.60f, 0.10f, 0.10f, 0.50f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.60f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.05f, 0.79f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_Button] = ImVec4(0.60f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.60f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.80f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.10f, 0.10f, 1.00f);
}