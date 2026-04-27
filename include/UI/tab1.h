#ifndef UI_TAB1_H
#define UI_TAB1_H

#include "../DXUT.h"

class Tab1 : public UiKit::WND {
private:
    UiKit::Input input;
    void OnFocus(WPARAM wParam, LPARAM lParam);
    void OnLostFocus(WPARAM wParam, LPARAM lParam);

public:
    Tab1();
    ~Tab1();

    int Run();
};

#endif