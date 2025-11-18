#include "../include/ui/ui.h"
#include <windows.h>
#include "../include/ui/ui_context.h"

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
        
        return app;
    }

    void DestroyAppWindow(AppWindow* app) {
        if (!app) return;
        
        DestroyUIWindow(app->context);
        
        if (app->hwnd) {
            DestroyWindow(app->hwnd);
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