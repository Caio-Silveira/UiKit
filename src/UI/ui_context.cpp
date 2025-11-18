#include "../include/ui/ui_context.h"

namespace UiKit {

    static Window* currentWindow = nullptr;

    Context::Context() : deltaTime(0.0f), time(0.0f) {
    }

    Context::~Context() {
        if (currentWindow == this) {
            currentWindow = nullptr;
        }
    }

    void Context::NewFrame(float dt) {
        deltaTime = dt;
        time += dt;
        drawList.Clear();
    }

    DrawList* Context::GetDrawList() {
        return &drawList;
    }

    float Context::GetDeltaTime() const {
        return deltaTime;
    }

    float Context::GetTime() const {
        return time;
    }

    Window* CreateUIWindow() {
        Window* window = new Window();
        if (!currentWindow) {
            SetCurrentWindow(window);
        }
        return window;
    }

    void DestroyUIWindow(Window* window) {
        if (window) {
            delete window;
        }
    }

    Window* GetCurrentWindow() {
        return currentWindow;
    }

    void SetCurrentWindow(Window* window) {
        currentWindow = window;
    }

}