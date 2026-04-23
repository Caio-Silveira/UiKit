#include "../include/Input.h"
#include "../include/WND.h"
namespace UiKit {
    
    bool Input::keys[256] = { 0 };
    bool Input::ctrl[256] = { 0 };

    Input::Input(){
        /*Change the window's winproc to InputProc*/
        SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)Input::InputProc);
    }

    Input::~Input(){
        /*switch back to using winproc from the window*/
        SetWindowLongPtr(GetActiveWindow(), GWLP_WNDPROC, (LONG_PTR)WND::WinProc);
    }

    bool Input::KeyPress(int vkcode){
        if(ctrl[vkcode]){
            if(KeyDown(vkcode)){
                ctrl[vkcode] = false;
                return true;
            }
        }
        else if(KeyUp(vkcode))
            ctrl[vkcode] = true;
            
        return false;
    }

    LRESULT CALLBACK Input::InputProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
        switch(msg){
            case WM_KEYDOWN:
                keys[wParam] = true;
                return 0;

            case WM_KEYUP:
                keys[wParam] = false;
                return 0;
        }
        return CallWindowProc(WND::WinProc, hWnd, msg, wParam, lParam);
    }
}