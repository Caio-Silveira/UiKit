#include "../include/WND.h"

namespace UiKit {

    WND::WND() {
        id = 0;
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        icon = LoadIcon(NULL, IDI_APPLICATION);
        cursor = LoadCursor(NULL, IDC_ARROW);
        color = RGB(0,0,0);
        title = std::string("Windows App");
        style = WS_POPUP | WS_VISIBLE;
        mode = FULLSCREEN;
        x = 0;
        y = 0;
        cenX = width / 2;
        cenY = height / 2;
    }

    void WND::Mode(int m) {
        mode = m;

        if(m == WINDOWED)
            style = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE;
        else
            style = WS_EX_TOPMOST | WS_POPUP | WS_VISIBLE;
    }

    void WND::Size(int w, int h) {
        width = w;
        height = h;

        cenX = width / 2;
        cenY = height / 2;

        x = (GetSystemMetrics(SM_CXSCREEN) / 2) - cenX;
        y = (GetSystemMetrics(SM_CYSCREEN) / 2) - cenY;
    }

    void WND::Print(std::string t, int x, int y, COLORREF c) {
        HDC xdc = GetDC(id);

        SetTextColor(xdc, c);
        SetBkMode(xdc, TRANSPARENT);

        TextOutA(xdc, x, y, t.c_str(), (int)t.size());

        ReleaseDC(id, xdc);
    }

    void WND::Clear() {
        HDC hdc = GetDC(id);

        RECT rect;
        GetClientRect(id, &rect);

        HBRUSH brush = CreateSolidBrush(color);
        FillRect(hdc, &rect, brush);

        DeleteObject(brush);
        ReleaseDC(id, hdc);
    }

    bool WND::Create() {
        HINSTANCE hInstance = GetModuleHandle(NULL);

        WNDCLASSEXA wc = {};
        wc.cbSize = sizeof(WNDCLASSEXA);
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WND::WinProc;
        wc.hInstance = hInstance;
        wc.hIcon = icon;
        wc.hCursor = cursor;
        wc.hbrBackground = (HBRUSH)CreateSolidBrush(color);
        wc.lpszClassName = "AppWindow";
        wc.hIconSm = icon;

        if(!RegisterClassExA(&wc))
            return false;

        DWORD dwExStyle = 0;
        DWORD dwStyle = style;
        RECT rect = {0,0,width,height};

        if(mode == FULLSCREEN) {
            dwExStyle = WS_EX_TOPMOST;
            dwStyle = WS_POPUP | WS_VISIBLE;
            x = 0;
            y = 0;
            width = GetSystemMetrics(SM_CXSCREEN);
            height = GetSystemMetrics(SM_CYSCREEN);
        } else {
            dwStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE;
            AdjustWindowRectEx(&rect, dwStyle, FALSE, dwExStyle);

            width = rect.right - rect.left;
            height = rect.bottom - rect.top;

            x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
            y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
        }

        id = CreateWindowExA(
            dwExStyle | WS_EX_LAYERED,
            "AppWindow",
            title.c_str(),
            dwStyle,
            x, y,
            width, height,
            NULL,
            NULL,
            hInstance,
            NULL
        );

        if(!id)
            return false;

        SetWindowLongPtr(id, GWLP_USERDATA, (LONG_PTR)this);

        int opacity = 255;
        SetLayeredWindowAttributes(id, 0, opacity, LWA_ALPHA);
        ShowWindow(id, SW_SHOW);
        UpdateWindow(id);

        return true;
    }

    void WND::On(UINT msg, EventCallback fn) {
        events[msg].push_back(fn);
    }

    void WND::Off(UINT msg) {
        events.erase(msg);
    }

    LRESULT CALLBACK WND::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        WND* window = (WND*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

        if(window) {
            auto it = window->events.find(msg);

            if(it != window->events.end()) {
                for(auto& fn : it->second) {
                    if(fn) {
                        fn(wParam, lParam);
                    }
                }
            }
        }

        switch(msg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}