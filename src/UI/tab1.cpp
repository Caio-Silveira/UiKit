#include "../../include/UI/tab1.h"
#include <windows.h>

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

        if(input.KeyPress(VK_ESCAPE))
            Close();

        if(input.KeyPress(VK_INSERT))
            input.Read();

        Print(input.Text(), 390, 300, RGB(255,255,255));
    }

    return 0;
}