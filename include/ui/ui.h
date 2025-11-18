#pragma once

#include "ui_core.h"
#include "ui_context.h"
#include <windows.h>

namespace UiKit {

struct AppWindow {
    HWND hwnd;
    int width;
    int height;
    bool running;
    
    Window* context;
};

AppWindow* CreateAppWindow(const char* title, int width, int height);
void DestroyAppWindow(AppWindow* app);
bool IsRunning(AppWindow* app);
void ProcessEvents(AppWindow* app);

}