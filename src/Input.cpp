#include "../include/Input.h"

namespace UiKit {
    
    bool Input::keys[256] = { 0 };
    bool Input::ctrl[256] = { 0 };
    uint Input::textindex = 0;
    char Input::text[textlimit] = { 0 };
    int Input::mouseX = 0;
    int Input::mouseY = 0;
    short Input::mouseWheel = 0;

    Input::Input(WND& wnd) {
        window = &wnd;

        window->On(WM_KEYDOWN, Input::OnKeyDown);
        window->On(WM_KEYUP, Input::OnKeyUp);
        window->On(WM_CHAR, Input::OnChar);
        window->On(WM_LBUTTONDOWN, Input::OnLeftDown);
        window->On(WM_LBUTTONUP, Input::OnLeftUp);
        window->On(WM_RBUTTONDOWN, Input::OnRightDown);
        window->On(WM_RBUTTONUP, Input::OnRightUp);
        window->On(WM_MBUTTONDOWN, Input::OnMiddleDown);
        window->On(WM_MBUTTONUP, Input::OnMiddleUp);
        window->On(WM_MOUSEMOVE, Input::OnMouseMove);
        window->On(WM_MOUSEWHEEL, Input::OnMouseWheel);        
    }

    Input::~Input() {
        if(window) {
            window->Off(WM_KEYDOWN);
            window->Off(WM_KEYUP);
            window->Off(WM_CHAR);
            window->Off(WM_LBUTTONDOWN);
            window->Off(WM_LBUTTONUP);
            window->Off(WM_RBUTTONDOWN);
            window->Off(WM_RBUTTONUP);
            window->Off(WM_MBUTTONDOWN);
            window->Off(WM_MBUTTONUP);
            window->Off(WM_MOUSEMOVE);
            window->Off(WM_MOUSEWHEEL);
        }
    }

    bool Input::Press(int vkcode) {
        if(ctrl[vkcode]) {
            if(Down(vkcode)) {
                ctrl[vkcode] = false;
                return true;
            }
        }
        else if(Up(vkcode)) {
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

    void Input::OnLeftDown(WPARAM, LPARAM) {
        keys[VK_LBUTTON] = true;
    }

    void Input::OnLeftUp(WPARAM, LPARAM) {
        keys[VK_LBUTTON] = false;
    }

    void Input::OnRightDown(WPARAM, LPARAM) {
        keys[VK_RBUTTON] = true;
    }

    void Input::OnRightUp(WPARAM, LPARAM) {
        keys[VK_RBUTTON] = false;
    }

    void Input::OnMiddleDown(WPARAM, LPARAM) {
        keys[VK_MBUTTON] = true;
    }

    void Input::OnMiddleUp(WPARAM, LPARAM) {
        keys[VK_MBUTTON] = false;
    }

    void Input::OnMouseMove(WPARAM, LPARAM lParam) {
        mouseX = GET_X_LPARAM(lParam);
        mouseY = GET_Y_LPARAM(lParam);
    }

    void Input::OnMouseWheel(WPARAM wParam, LPARAM) {
        mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
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