#ifndef DXUT_INPUT_H
#define DXUT_INPUT_H

#include <windows.h>
#include "WND.h"

namespace UiKit {
    class Input {
        private:
            WND* window;

            static bool keys[256];
            static bool ctrl[256];
            static char character;

            static void OnKeyDown(WPARAM wParam, LPARAM lParam);
            static void OnKeyUp(WPARAM wParam, LPARAM lParam);
            static void OnChar(WPARAM wParam, LPARAM lParam);

        public:
            Input(WND& wnd);
            ~Input();

            bool KeyDown(int vkcode);
            bool KeyUp(int vkcode);
            bool KeyPress(int vkcode);
            char Read();
    };

    inline bool Input::KeyDown(int vkcode) {
        return keys[vkcode];
    }

    inline bool Input::KeyUp(int vkcode) {
        return !keys[vkcode];
    }
}

#endif