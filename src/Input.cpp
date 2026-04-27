#include "../include/Input.h"

namespace UiKit {
    
    bool Input::keys[256] = { 0 };
    bool Input::ctrl[256] = { 0 };
    char Input::character = '\0';

    Input::Input(WND& wnd) {
        window = &wnd;

        window->On(WM_KEYDOWN, Input::OnKeyDown);
        window->On(WM_KEYUP, Input::OnKeyUp);
        window->On(WM_CHAR, Input::OnChar);
    }

    Input::~Input() {
        if(window) {
            window->Off(WM_KEYDOWN);
            window->Off(WM_KEYUP);
            window->Off(WM_CHAR);
        }
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

    char Input::Read() {
        char c = character;
        character = '\0';
        return c;
    }
}