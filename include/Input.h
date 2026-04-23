#ifndef DXUT_INPUT_H
#define DXUT_INPUT_H

#include <windows.h>

namespace UiKit {
    class Input
    {
        private:
            static bool keys[256];  //state keyboard/mouse
            static bool ctrl[256];  //control free

        public:
            
            Input();    //construct
            ~Input();   //destruct

            bool KeyDown(int vkcode);
            bool KeyUp(int vkcode);
            bool KeyPress(int vkcode);

        static LRESULT CALLBACK InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

    inline bool Input::KeyDown(int vkcode){
        return keys[vkcode];
    }

    inline bool Input::KeyUp(int vkcode){
        return !(keys[vkcode]);
    }
}
#endif