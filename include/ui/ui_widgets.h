#pragma once

#include "ui_core.h"
#include "ui_draw.h"
#include "ui_context.h"
#include <cstring>

namespace UiKit {

    enum class MouseButton : uint8_t {
        Left = 0,
        Right = 1,
        Middle = 2,
        Count = 3
    };

    enum class WidgetState : uint8_t {
        None = 0,
        Hovered = 1 << 0,
        Active = 1 << 1,
        Focused = 1 << 2,
        Disabled = 1 << 3
    };

    inline WidgetState operator|(WidgetState a, WidgetState b) {
        return static_cast<WidgetState>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    inline WidgetState& operator|=(WidgetState& a, WidgetState b) {
        a = a | b;
        return a;
    }

    inline bool HasState(WidgetState state, WidgetState flag) {
        return (static_cast<uint8_t>(state) & static_cast<uint8_t>(flag)) != 0;
    }

    struct InputState {
        Vec2 mousePosition;
        Vec2 mousePositionPrevious;
        Vec2 mouseDelta;
        float mouseWheelDelta;
        
        bool mouseButtons[static_cast<size_t>(MouseButton::Count)];
        bool mouseButtonsPrevious[static_cast<size_t>(MouseButton::Count)];
        
        bool keys[512];
        bool keysPrevious[512];
        
        char textInput[32];
        uint32_t textInputLength;
        
        InputState() : mousePosition{}, mousePositionPrevious{}, mouseDelta{}, 
                    mouseWheelDelta(0.0f), textInputLength(0) {
            for (size_t i = 0; i < static_cast<size_t>(MouseButton::Count); ++i) {
                mouseButtons[i] = false;
                mouseButtonsPrevious[i] = false;
            }
            for (size_t i = 0; i < 512; ++i) {
                keys[i] = false;
                keysPrevious[i] = false;
            }
            textInput[0] = '\0';
        }
        
        void NewFrame() {
            mousePositionPrevious = mousePosition;
            mouseDelta = mousePosition - mousePositionPrevious;
            mouseWheelDelta = 0.0f;
            
            for (size_t i = 0; i < static_cast<size_t>(MouseButton::Count); ++i) {
                mouseButtonsPrevious[i] = mouseButtons[i];
            }
            for (size_t i = 0; i < 512; ++i) {
                keysPrevious[i] = keys[i];
            }
            
            textInputLength = 0;
            textInput[0] = '\0';
        }
        
        void SetMousePosition(Vec2 pos) {
            mousePosition = pos;
            mouseDelta = mousePosition - mousePositionPrevious;
        }
        
        void SetMouseButton(MouseButton button, bool down) {
            mouseButtons[static_cast<size_t>(button)] = down;
        }
        
        void SetMouseWheel(float delta) {
            mouseWheelDelta = delta;
        }
        
        void SetKey(uint32_t keycode, bool down) {
            if (keycode < 512) {
                keys[keycode] = down;
            }
        }
        
        void AddText(const char* text) {
            if (!text) return;
            
            size_t len = strlen(text);
            if (textInputLength + len >= 31) {
                len = 31 - textInputLength;
            }
            
            if (len > 0) {
                memcpy(textInput + textInputLength, text, len);
                textInputLength += static_cast<uint32_t>(len);
                textInput[textInputLength] = '\0';
            }
        }
        
        bool IsMouseDown(MouseButton button) const {
            return mouseButtons[static_cast<size_t>(button)];
        }
        
        bool IsMousePressed(MouseButton button) const {
            size_t idx = static_cast<size_t>(button);
            return mouseButtons[idx] && !mouseButtonsPrevious[idx];
        }
        
        bool IsMouseReleased(MouseButton button) const {
            size_t idx = static_cast<size_t>(button);
            return !mouseButtons[idx] && mouseButtonsPrevious[idx];
        }
        
        bool IsMouseHovering(Rect rect) const {
            return RectContains(rect, mousePosition);
        }
        
        bool IsKeyDown(uint32_t keycode) const {
            return keycode < 512 ? keys[keycode] : false;
        }
        
        bool IsKeyPressed(uint32_t keycode) const {
            return keycode < 512 ? (keys[keycode] && !keysPrevious[keycode]) : false;
        }
        
        bool IsKeyReleased(uint32_t keycode) const {
            return keycode < 512 ? (!keys[keycode] && keysPrevious[keycode]) : false;
        }
        
        bool HasText() const {
            return textInputLength > 0;
        }
    };

    // ! Define your custom widget function headers below
    // ! Example: bool Button(const char* label, Rect bounds, InputState& input);
    // ! Example: bool Slider(float* value, Rect bounds, InputState& input, float min, float max);
}