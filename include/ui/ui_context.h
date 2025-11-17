#pragma once

#include "ui_core.h"
#include "ui_draw.h"

namespace UiKit {

    class Context {
    public:
        Context();
        ~Context();
        
        void NewFrame(float deltaTime);
        void Render();
        
        DrawList* GetDrawList();
        
        void SetDisplaySize(Vec2 size);
        Vec2 GetDisplaySize() const;
        
        float GetDeltaTime() const;
        float GetTime() const;

    private:
        DrawList drawList;
        Vec2 displaySize;
        float deltaTime;
        float time;
    };

    using Window = Context;

    Window* CreateWindow();
    void DestroyWindow(Window* window);
    Window* GetCurrentWindow();
    void SetCurrentWindow(Window* window);
}