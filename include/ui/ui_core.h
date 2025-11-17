#pragma once

#include <cmath>

namespace UiKit {

    struct Vec2 {
        float x, y;
        
        Vec2() : x(0.0f), y(0.0f) {}
        Vec2(float x, float y) : x(x), y(y) {}
        
        Vec2 operator+(Vec2 o) const { return {x + o.x, y + o.y}; }
        Vec2 operator-(Vec2 o) const { return {x - o.x, y - o.y}; }
        Vec2 operator*(float s) const { return {x * s, y * s}; }
        Vec2 operator/(float s) const { return {x / s, y / s}; }
        
        Vec2& operator+=(Vec2 o) { x += o.x; y += o.y; return *this; }
        Vec2& operator-=(Vec2 o) { x -= o.x; y -= o.y; return *this; }
        
        bool operator==(Vec2 o) const { return x == o.x && y == o.y; }
        bool operator!=(Vec2 o) const { return !(*this == o); }
        
        float Length() const { return sqrtf(x * x + y * y); }
        float LengthSquared() const { return x * x + y * y; }
        
        Vec2 Normalized() const {
            float len = Length();
            return len > 0.0f ? (*this / len) : Vec2{};
        }
    };

    // * Four-sided properties: padding, borders, corners
    struct Vec4 {
        float x, y, z, w;
        
        Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        Vec4(float all) : x(all), y(all), z(all), w(all) {}
        Vec4(float xy, float zw) : x(xy), y(zw), z(xy), w(zw) {}
        Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        
        float Horizontal() const { return x + z; }
        float Vertical() const { return y + w; }
    };

    using Padding = Vec4;
    using BorderThickness = Vec4;
    using CornerRadius = Vec4;

    inline unsigned int FloatToU8(float value);

    struct Color {
        float r, g, b, a;
        
        Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
        Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
        
        Color(unsigned int rgba) {
            r = ((rgba >> 24) & 0xFF) / 255.0f;
            g = ((rgba >> 16) & 0xFF) / 255.0f;
            b = ((rgba >> 8) & 0xFF) / 255.0f;
            a = (rgba & 0xFF) / 255.0f;
        }
        
        unsigned int ToRGBA() const {
            return (FloatToU8(r) << 24) | (FloatToU8(g) << 16) | (FloatToU8(b) << 8) | FloatToU8(a);
        }
        
        unsigned int ToABGR() const {
            return (FloatToU8(a) << 24) | (FloatToU8(b) << 16) | (FloatToU8(g) << 8) | FloatToU8(r);
        }
    };

    struct Rect {
        Vec2 position;
        Vec2 size;
        
        Rect() = default;
        Rect(Vec2 pos, Vec2 sz) : position(pos), size(sz) {}
        Rect(float x, float y, float w, float h) : position(x, y), size(w, h) {}
        
        float Left() const { return position.x; }
        float Right() const { return position.x + size.x; }
        float Top() const { return position.y; }
        float Bottom() const { return position.y + size.y; }
        float Width() const { return size.x; }
        float Height() const { return size.y; }
        
        Vec2 Center() const { return position + size * 0.5f; }
        Vec2 TopLeft() const { return position; }
        Vec2 TopRight() const { return {Right(), Top()}; }
        Vec2 BottomLeft() const { return {Left(), Bottom()}; }
        Vec2 BottomRight() const { return position + size; }
    };

}