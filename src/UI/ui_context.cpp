#include "../include/ui/ui_context.h"

namespace UiKit {

    static Window* currentWindow = nullptr;

    Context::Context() 
        : displaySize{}, deltaTime(0.0f), time(0.0f) {
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

    void Context::Render() {
    }

    DrawList* Context::GetDrawList() {
        return &drawList;
    }

    void Context::SetDisplaySize(Vec2 size) {
        displaySize = size;
    }

    Vec2 Context::GetDisplaySize() const {
        return displaySize;
    }

    float Context::GetDeltaTime() const {
        return deltaTime;
    }

    float Context::GetTime() const {
        return time;
    }

    Window* CreateWindow() {
        Window* window = new Window();
        if (!currentWindow) {
            SetCurrentWindow(window);
        }
        return window;
    }

    void DestroyWindow(Window* window) {
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