#pragma once
#include "MinHook.h"
#include <iostream>
#include "d3d9.h"
#include "d3dx9.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#pragma comment(lib, "imgui.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

typedef HRESULT(__stdcall* PresentSignature)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT(__stdcall* ResetSignature)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

class CRender {
public:
	CRender() {
		MH_CreateHook(this->GetFunctionAddress(17), this->OnPresent, reinterpret_cast<LPVOID*>(&this->origPresentSignature));
		MH_EnableHook(this->GetFunctionAddress(17));
		MH_CreateHook(this->GetFunctionAddress(16), this->OnReset, reinterpret_cast<LPVOID*>(&this->origResetSignature));
		MH_EnableHook(this->GetFunctionAddress(16));
	}
	~CRender() {
		MH_DisableHook(this->GetFunctionAddress(17));
		MH_DisableHook(this->GetFunctionAddress(16));
	}

	PresentSignature origPresentSignature;
	ResetSignature origResetSignature;

	static HRESULT __stdcall OnPresent(IDirect3DDevice9* device_ptr, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	static HRESULT __stdcall OnReset(IDirect3DDevice9* device_ptr, D3DPRESENT_PARAMETERS* parameters);
private:
	void* GetFunctionAddress(int VTableIndex);
	std::uintptr_t FindDevice(std::uint32_t Len);
};