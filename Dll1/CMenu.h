#pragma once
#include "d3d9.h"
#include "MinHook.h"
#include <iostream>
#include <Windows.h>
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

extern bool objectRenderStatus[7];
extern bool textRenderStatus;
extern bool lineRenderStatus;
extern bool renderWindow;

class CMenu {
public:
	CMenu(IDirect3DDevice9* device_ptr);
	~CMenu();

	void Render();
	void Theme();
};