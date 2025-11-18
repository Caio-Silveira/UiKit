#include "../include/ui/ui.h"
#include <windows.h>
#include "../include/ui/ui_context.h"
#include <dxgi1_6.h>
#include <d3d12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace UiKit {

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        AppWindow* app = reinterpret_cast<AppWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        
        switch (msg) {
            case WM_CLOSE:
                if (app) app->running = false;
                return 0;
                
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    bool InitD3D12(AppWindow* app) {
        UINT dxgiFactoryFlags = 0;
        
        #ifdef _DEBUG
        ID3D12Debug* debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            debugController->Release();
        }
        #endif
        
        if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&app->factory)))) {
            return false;
        }
        
        if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&app->device)))) {
            return false;
        }
        
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        
        if (FAILED(app->device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&app->commandQueue)))) {
            return false;
        }

        return true;
    }

    AppWindow* CreateAppWindow(const char* title, int width, int height) {
        AppWindow* app = new AppWindow{};
        app->width = width;
        app->height = height;
        app->running = true;
        
        WNDCLASSA wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = "UiKitWindow";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        
        RegisterClassA(&wc);
        
        RECT rect = {0, 0, width, height};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
        
        app->hwnd = CreateWindowA(
            "UiKitWindow",
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr, nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );
        
        SetWindowLongPtr(app->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
        
        ShowWindow(app->hwnd, SW_SHOW);
        UpdateWindow(app->hwnd);
        
        app->context = CreateUIWindow();
        
        if (!InitD3D12(app)) {
            DestroyAppWindow(app);
            return nullptr;
        }

        return app;
    }

    void DestroyAppWindow(AppWindow* app) {
        if (!app) return;
        
        if (app->commandQueue) app->commandQueue->Release();
        if (app->device) app->device->Release();
        if (app->factory) app->factory->Release();
        
        DestroyUIWindow(app->context);
        
        if (app->hwnd) {
            ::DestroyWindow(app->hwnd);
        }
        
        delete app;
    }

    bool IsRunning(AppWindow* app) {
        return app && app->running;
    }

    void ProcessEvents(AppWindow* app) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                app->running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}