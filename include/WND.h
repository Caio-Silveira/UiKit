#ifndef DXUT_WND_H
#define DXUT_WND_H

#include "Types.h"
#include <windows.h>
#include <windowsx.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace UiKit {

    enum WindowModes { FULLSCREEN, WINDOWED };

    class WND {
        public:
            using EventCallback = void(*)(WPARAM, LPARAM);

        private:
            HWND id;
            int width;
            int height;
            int x;
            int y;
            int cenX;
            int cenY;
            HICON icon;
            HCURSOR cursor;
            COLORREF color;
            std::string title;
            DWORD style;
            int mode;
            static std::unordered_map<UINT, std::vector<EventCallback>> events;

        public:
            WND();
            HWND Id() const;
            int Width() const;
            int Height() const;
            int Mode() const;
            int CenterX() const;
            int CenterY() const;
            std::string Title() const;
            COLORREF Color() const;

            void Icon(const uint i);
            void Icon(const LPCSTR i);
            void Cursor(const LPCSTR c);
            void Title(const std::string t);
            void Size(int w, int h);
            void Mode(int m);
            void Color(int r, int g, int b);

            void HideCursor(bool h);
            void Print(std::string t, int x, int y, COLORREF c);
            void Close();
            bool Create();
            void Clear();
            
            static void On(UINT msg, EventCallback fn);
            static void Off(UINT msg, EventCallback fn);
            static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    };

    inline HWND WND::Id() const {
        return id;
    }

    inline int WND::Width() const {
        return width;
    }
    
    inline int WND::Height() const {
        return height;
    }

    inline int WND::Mode() const {
        return mode;
    }

    inline int WND::CenterX() const {
        return cenX;
    }

    inline int WND::CenterY() const {
        return cenY;
    }

    inline std::string WND::Title() const {
        return title;
    }

    inline COLORREF  WND::Color() const {
        return color;
    }

    inline void WND::Icon(const uint i) {
        icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(i));
    }

    inline void WND::Icon(const LPCSTR i) {
        icon = LoadIconA(GetModuleHandle(NULL), i);
    }

    inline void WND::Title(std::string t) {
        title = t;
    }

    inline void WND::Color(int r, int g, int b) {
        color = RGB(r, g, b);
    }

    inline void WND::Cursor(const LPCSTR c){
        cursor = LoadCursorA(NULL, c);
    }

    inline void WND::HideCursor(bool h) {
        ShowCursor(!h);
    }

    inline void WND::Close() {
        PostMessage(id, WM_DESTROY, 0, 0);
    }
}
#endif