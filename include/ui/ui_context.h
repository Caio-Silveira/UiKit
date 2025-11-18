#pragma once

#include "ui_core.h"
#include "ui_draw.h"

namespace UiKit {

    class Context {
    public:
        Context();
        ~Context();
        
        void NewFrame(float deltaTime);
        
        DrawList* GetDrawList();
        
        float GetDeltaTime() const;
        float GetTime() const;

    private:
        DrawList drawList;
        float deltaTime;
        float time;
    };

    using Window = Context;

    Window* CreateUIWindow();
    void DestroyUIWindow(Window* window);
    Window* GetCurrentWindow();
    void SetCurrentWindow(Window* window);

}