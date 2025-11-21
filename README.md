# UiKit

Minimal immediate-mode UI framework with DirectX 12 backend.

## Features

- **Primitives**: Rectangles, circles, triangles, lines with rounded corners
- **Text**: TTF font rendering with anti-aliasing
- **Input**: Mouse tracking and button states
- **Backend**: DirectX 12 with MSAA 4x

## Quick Start

```cpp
#include "include/ui/ui.h"

AppWindow* app = CreateAppWindow("UiKit", 1280, 720);
DrawList* draw = app->context->GetDrawList();

while (IsRunning(app)) {
    ProcessEvents(app);
    BeginFrame(app);
    
    draw->DrawFill4({100, 100, 200, 100}, {0.2f, 0.3f, 0.8f, 1.0f}, Vec4{8.0f});
    draw->DrawString("Hello World", {50, 50}, {1, 1, 1, 1}, FontData, 18.0f);
    
    EndFrame(app);
}

DestroyAppWindow(app);
```

## API

```cpp
DrawFill4(rect, color, rounding)     // Filled rectangle
Draw4(rect, color, thickness, rounding) // Rectangle outline
DrawFill0(center, radius, color)     // Filled circle
Draw0(center, radius, color, thickness) // Circle outline
DrawFill3(p1, p2, p3, color)         // Filled triangle
Draw2(points, count, color, thickness) // Polyline
DrawString(text, pos, color, font, size) // Text
DrawImg(rect, texture, tint)         // Image
```

## Build

```bash
cmake -B build
cmake --build build
```

**Requirements**: Windows 10+, DirectX 12, CMake 3.15+

## License
MIT