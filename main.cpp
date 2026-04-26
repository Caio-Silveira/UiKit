#include "include/DXUT.h"

UiKit::WND* g_window = nullptr;

void OnFocus(WPARAM, LPARAM) {
    if (g_window) {
        g_window->Clear();
        g_window->Print("OnFocus", 20, 20, RGB(255,255,255));
    }
}

void OnLostFocus(WPARAM, LPARAM) {
    if (g_window) {
        g_window->Clear();
        g_window->Print("LostFocus", 20, 20, RGB(255,0,0));
    }
}
 
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    
    UiKit::WND* window = new UiKit::WND();
    g_window = window;

    window->Mode(UiKit::WINDOWED);
    window->Size(1024,600);
    window->Color(255,141,161);
    window->Title("UiKit");
    window->Icon(IDI_APPLICATION);
    window->Cursor(IDC_ARROW);
    window->Create();

    UiKit::WND::On(WM_SETFOCUS, OnFocus);
    UiKit::WND::On(WM_KILLFOCUS, OnLostFocus);

    UiKit::Input* input = new UiKit::Input();

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if(input->KeyPress(VK_ESCAPE))
            window->Close();
    }

    delete input;
    delete window;
    return 0;
}