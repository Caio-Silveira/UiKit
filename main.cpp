#include "include/ui/ui.h"
#include "include/ui/ui_widgets.h"
#include "include/ui/verdana.h"
#include <windows.h>
#include <cstdio>
using namespace UiKit;

int main() {
    AppWindow* app = CreateAppWindow("UiKit Test", 1280, 720);

    if (!app) return -1;

    SetClearColor(app, Color{0.047f, 0.043f, 0.05f, 1.0f});

    InputState input;

    LARGE_INTEGER freq, last, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);

    while (IsRunning(app)) {
        ProcessEvents(app);

        QueryPerformanceCounter(&now);
        float deltaTime = (float)((now.QuadPart - last.QuadPart) / (double)freq.QuadPart);
        last = now;

        input.NewFrame();

        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(app->hwnd, &pt);
        input.SetMousePosition({(float)pt.x, (float)pt.y});
        input.SetMouseButton(MouseButton::Left, (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);

        Window* window = app->context;
        window->NewFrame(deltaTime);

        DrawList* draw = window->GetDrawList();

        draw->DrawFill4({100, 100, 200, 100}, {0.2f, 0.3f, 0.8f, 1.0f}, Vec4{8.0f});
        draw->DrawFill0({400, 300}, 50.0f, {0.8f, 0.2f, 0.3f, 1.0f});
        draw->Draw4({600, 100, 200, 100}, {1.0f, 1.0f, 0.0f, 1.0f}, 3.0f, Vec4{4.0f});
        draw->DrawFill3({800, 100}, {900, 100}, {850, 200}, {0.3f, 0.8f, 0.3f, 1.0f});

        bool isHovering = input.IsMouseHovering({500, 400, 150, 50});
        Color buttonColor = isHovering ? Color{0.4f, 0.4f, 0.4f, 1.0f}
                                       : Color{0.2f, 0.2f, 0.2f, 1.0f};
        draw->DrawFill4({500, 400, 150, 50}, buttonColor, Vec4{6.0f});

        if (isHovering && input.IsMousePressed(MouseButton::Left)) {
            OutputDebugStringA("Button Clicked!\n");
        }

        Vec2 points[5] = {{100, 100}, {200, 150}, {300, 100}, {400, 150}, {500, 100}};
        draw->Draw2(points, 5, {1.0f, 0.0f, 0.0f, 1.0f}, 2.0f);

        draw->DrawString("Hello World", {50, 50}, {1,1,1,1}, FontBuffer, 32.0f);

        float fps = 1.0f / deltaTime;
        char fpsText[32];
        snprintf(fpsText, sizeof(fpsText), "FPS: %.0f", fps);
        draw->DrawString(fpsText, {20, 20}, {1,1,0,1}, FontBuffer, 28.0f);

        BeginFrame(app);
        EndFrame(app);
    }

    DestroyAppWindow(app);
    return 0;
}
