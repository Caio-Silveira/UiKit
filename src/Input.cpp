#include "../include/Input.h"
#include "../include/WND.h"

namespace UiKit {
    
    bool Input::keys[256] = { 0 };
    bool Input::ctrl[256] = { 0 };
    char Input::character = '\0';

    Input::Input() {
        WND::On(WM_KEYDOWN, Input::OnKeyDown);
        WND::On(WM_KEYUP, Input::OnKeyUp);
        WND::On(WM_CHAR, Input::OnChar);
    }

    Input::~Input() {
        WND::Off(WM_KEYDOWN, Input::OnKeyDown);
        WND::Off(WM_KEYUP, Input::OnKeyUp);
        WND::Off(WM_CHAR, Input::OnChar);
    }

    bool Input::KeyPress(int vkcode) {
        if(ctrl[vkcode]) {
            if(KeyDown(vkcode)) {
                ctrl[vkcode] = false;
                return true;
            }
        }
        else if(KeyUp(vkcode)) {
            ctrl[vkcode] = true;
        }
            
        return false;
    }

    void Input::OnKeyDown(WPARAM wParam, LPARAM) {
        keys[wParam] = true;
    }

    void Input::OnKeyUp(WPARAM wParam, LPARAM) {
        keys[wParam] = false;
    }

    void Input::OnChar(WPARAM wParam, LPARAM) {
        character = static_cast<char>(wParam);
    }

    char Input::ReadChar() {
        char c = character;
        character = '\0';
        return c;
    }
}