#ifndef DXUT_INPUT_H
#define DXUT_INPUT_H

#include <windows.h>

namespace UiKit {
    class Input {
        private:
            static bool keys[256];
            static bool ctrl[256];
            static char character;

            static void OnKeyDown(WPARAM wParam, LPARAM lParam);
            static void OnKeyUp(WPARAM wParam, LPARAM lParam);
            static void OnChar(WPARAM wParam, LPARAM lParam);

        public:
            Input();
            ~Input();

            bool KeyDown(int vkcode);
            bool KeyUp(int vkcode);
            bool KeyPress(int vkcode);
            char ReadChar();
    };

    inline bool Input::KeyDown(int vkcode){
        return keys[vkcode];
    }

    inline bool Input::KeyUp(int vkcode){
        return !(keys[vkcode]);
    }
}
#endif