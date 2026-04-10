#include "include/DXUT.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    UiKit::WND * window = new UiKit::WND();

    window->Mode(UiKit::WINDOWED);
    window->Size(1024,600);
    window->Color(255,141,161);
    window->Title("UiKit");
    window->Icon(IDI_APPLICATION);
    window->Cursor(IDC_ARROW);
    window->Create();

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete window;
    return 0;
}