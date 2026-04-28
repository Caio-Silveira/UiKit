#include "../../include/UI/tab1.h"
#include <windows.h>
#include <string>

Tab1::Tab1() : input(*this) {
    Mode(UiKit::WINDOWED);
    Size(1024, 600);
    Color(255, 141, 161);
    Title("UiKit");
    Icon(IDI_APPLICATION);
    Cursor(IDC_ARROW);

    Create();

    On(WM_SETFOCUS, [this](WPARAM wParam, LPARAM lParam) {
        OnFocus(wParam, lParam);
    });

    On(WM_KILLFOCUS, [this](WPARAM wParam, LPARAM lParam) {
        OnLostFocus(wParam, lParam);
    });
}

Tab1::~Tab1() {
}

void Tab1::OnFocus(WPARAM, LPARAM) {
    Clear();
    Print("Focused", 20, 20, RGB(255,255,255));
}

void Tab1::OnLostFocus(WPARAM, LPARAM) {
    Clear();
    Print("UnFocused", 20, 20, RGB(255,0,0));
}

int Tab1::Run() {
    MSG msg;

    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if(input.Press(VK_ESCAPE))
            Close();

        if(input.Press(VK_INSERT))
            input.Read();

        Clear();

        Print(input.Text(), 390, 300, RGB(255,255,255));

        std::string mousePos = "Mouse X: " + std::to_string(input.MouseX()) +
                               " Y: " + std::to_string(input.MouseY());

        Print(mousePos, 20, 60, RGB(255,255,255));

        std::string wheel = "Wheel: " + std::to_string(input.MouseWheel());
        Print(wheel, 20, 90, RGB(255,255,255));

        if(input.Down(VK_LBUTTON))
            Print("Left Button", 20, 120, RGB(255,255,255));

        if(input.Down(VK_RBUTTON))
            Print("Right Button", 20, 150, RGB(255,255,255));

        if(input.Down(VK_MBUTTON))
            Print("Middle Button", 20, 180, RGB(255,255,255));
    }

    return 0;
}