#pragma once

#include "ui_core.h"
#include "ui_context.h"
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace UiKit {

    struct AppWindow {
        HWND hwnd;
        int width;
        int height;
        bool running;
        
        Window* context;
        ID3D12Device* device;
        IDXGIFactory4* factory;
        ID3D12CommandQueue* commandQueue;
    };

    AppWindow* CreateAppWindow(const char* title, int width, int height);
    void DestroyAppWindow(AppWindow* app);
    bool IsRunning(AppWindow* app);
    void ProcessEvents(AppWindow* app);

}