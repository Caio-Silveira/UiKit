#ifndef DXUT_INPUT_H
#define DXUT_INPUT_H

#include <minwindef.h>
#include "WND.h"

namespace UiKit {
    class Input {
        private:
            WND* window;

            static bool keys[256];
            static bool ctrl[256];
            static int mouseX;
            static int mouseY;
            static short mouseWheel;

            static const uint textlimit = 80;
            static uint textindex;
            static char text[textlimit];

            static void OnKeyDown(WPARAM wParam, LPARAM lParam);
            static void OnKeyUp(WPARAM wParam, LPARAM lParam);
            static void OnChar(WPARAM wParam, LPARAM lParam);
            static void OnLeftDown(WPARAM wParam, LPARAM lParam);
            static void OnLeftUp(WPARAM wParam, LPARAM lParam);
            static void OnRightDown(WPARAM wParam, LPARAM lParam);
            static void OnRightUp(WPARAM wParam, LPARAM lParam);
            static void OnMiddleDown(WPARAM wParam, LPARAM lParam);
            static void OnMiddleUp(WPARAM wParam, LPARAM lParam);
            static void OnMouseMove(WPARAM wParam, LPARAM lParam);
            static void OnMouseWheel(WPARAM wParam, LPARAM lParam);

        public:
            Input(WND& wnd);
            ~Input();

            bool Down(int vkcode);
            bool Up(int vkcode);
            bool Press(int vkcode);

            int MouseX();
            int MouseY();
            short MouseWheel();

            void Read();
            static char* Text();
    };

    inline bool Input::Down(int vkcode) {
        return keys[vkcode];
    }

    inline bool Input::Up(int vkcode) {
        return !keys[vkcode];
    }

    inline char* Input::Text(){
        return text;
    }

    inline int Input::MouseX() {
        return mouseX;
    }

    inline int Input::MouseY() {
        return mouseY;
    }

    inline short Input::MouseWheel() {
        short value = mouseWheel;
        mouseWheel = 0;
        return value;
    }
}

#endif