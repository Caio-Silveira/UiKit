#include "../include/Input.h"
#include <windows.h>

namespace UiKit {
    
    bool Input::keys[256] = { 0 };
    bool Input::ctrl[256] = { 0 };
    uint Input::textindex = 0;
    char Input::text[textlimit] = { 0 };

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
        switch (wParam) {
            //backspace
            case 0x08:
                if(textindex > 0)   {
                    textindex--;
                    text[textindex] = '\0';
                }
                break;
            //tab and enter
            case 0x09:
            case 0x0D:
                break;
            default:
               if(textindex < textlimit)
                  text[textindex++] = char(wParam);
                break;
        }
        //nao precisa com directx
        InvalidateRect(GetActiveWindow(), NULL, TRUE);
        return;
    }

    void Input::Read() {
        textindex = 0;
        ZeroMemory(text, textlimit);
    }
}