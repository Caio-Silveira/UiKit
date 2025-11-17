#pragma once

#include "ui_core.h"
#include <cmath>

namespace UiKit {

#pragma region Math

    template<typename T>
    inline T Min(T a, T b) {
        return a < b ? a : b;
    }

    template<typename T>
    inline T Max(T a, T b) {
        return a > b ? a : b;
    }

    template<typename T>
    inline T Clamp(T value, T min, T max) {
        return value < min ? min : (value > max ? max : value);
    }

    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    inline float InverseLerp(float a, float b, float value) {
        return (value - a) / (b - a);
    }

    inline float Remap(float value, float fromMin, float fromMax, float toMin, float toMax) {
        float t = InverseLerp(fromMin, fromMax, value);
        return Lerp(toMin, toMax, t);
    }

    inline float Saturate(float value) {
        return Clamp(value, 0.0f, 1.0f);
    }

    inline float Abs(float value) {
        return value < 0.0f ? -value : value;
    }

    inline float Sign(float value) {
        return value < 0.0f ? -1.0f : (value > 0.0f ? 1.0f : 0.0f);
    }

    inline float Floor(float value) {
        return floorf(value);
    }

    inline float Ceil(float value) {
        return ceilf(value);
    }

    inline float Round(float value) {
        return floorf(value + 0.5f);
    }

    inline float Sqrt(float value) {
        return sqrtf(value);
    }

    inline float Pow(float base, float exponent) {
        return powf(base, exponent);
    }

    inline float Sin(float radians) {
        return sinf(radians);
    }

    inline float Cos(float radians) {
        return cosf(radians);
    }

    inline float Tan(float radians) {
        return tanf(radians);
    }

    inline float Atan2(float y, float x) {
        return atan2f(y, x);
    }

    constexpr float PI = 3.14159265358979323846f;
    constexpr float TAU = PI * 2.0f;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;

    inline float DegreesToRadians(float degrees) {
        return degrees * DEG_TO_RAD;
    }

    inline float RadiansToDegrees(float radians) {
        return radians * RAD_TO_DEG;
    }

    #pragma endregion

#pragma region Draw
    struct CornerArcPositions {
        Vec2 topLeft;
        Vec2 topRight;
        Vec2 bottomRight;
        Vec2 bottomLeft;
    };

    struct LineState {
        Color color;
        float thickness;
    };

    struct ArcState {
        Color color;
        float thickness;
        int segments;
    };

    inline Vec2 GetCirclePoint(Vec2 center, float radius, float angle) {
        return center + Vec2{Cos(angle), Sin(angle)} * radius;
    }

    inline int CalculateCircleSegmentCount(float radius) {
        int segments = static_cast<int>(Sqrt(radius) * 4.0f);
        return Clamp(segments, 12, 128);
    }

    inline Vec2 GetPerpendicularNormalized(Vec2 direction) {
        return Vec2{-direction.y, direction.x}.Normalized();
    }

    inline Vec4 ClampCornerRadius(Rect rect, Vec4 radius) {
        float maxRadius = Min(rect.Width(), rect.Height()) * 0.5f;
        return {
            Min(radius.x, maxRadius),
            Min(radius.y, maxRadius),
            Min(radius.z, maxRadius),
            Min(radius.w, maxRadius)
        };
    }

    inline bool IsCornerRadiusZero(Vec4 radius) {
        return radius.x <= 0.0f && radius.y <= 0.0f && 
            radius.z <= 0.0f && radius.w <= 0.0f;
    }

    inline CornerArcPositions GetCornerArcCenters(Rect rect, Vec4 rounding) {
        return {
            {rect.Left() + rounding.x, rect.Top() + rounding.y},
            {rect.Right() - rounding.y, rect.Top() + rounding.y},
            {rect.Right() - rounding.z, rect.Bottom() - rounding.z},
            {rect.Left() + rounding.w, rect.Bottom() - rounding.w}
        };
    }
    #pragma endregion

    inline unsigned int FloatToU8(float value) {
        return static_cast<unsigned int>(value * 255.0f);
    }
    
    inline Rect ClipRect(Rect rect, Rect clipRect) {
        float x1 = Max(rect.Left(), clipRect.Left());
        float y1 = Max(rect.Top(), clipRect.Top());
        float x2 = Min(rect.Right(), clipRect.Right());
        float y2 = Min(rect.Bottom(), clipRect.Bottom());
        
        if (x2 < x1) x2 = x1;
        if (y2 < y1) y2 = y1;
        
        return {x1, y1, x2 - x1, y2 - y1};
    }

    inline Rect ExpandRect(Rect rect, float amount) {
        return {
            rect.position.x - amount, 
            rect.position.y - amount,
            rect.size.x + amount * 2.0f, 
            rect.size.y + amount * 2.0f
        };
    }

    inline Rect ShrinkRect(Rect rect, float amount) {
        return ExpandRect(rect, -amount);
    }

    inline bool RectContains(Rect rect, Vec2 point) {
        return point.x >= rect.Left() && point.x <= rect.Right() &&
            point.y >= rect.Top() && point.y <= rect.Bottom();
    }

    inline bool RectIntersects(Rect a, Rect b) {
        return a.Left() < b.Right() && a.Right() > b.Left() &&
            a.Top() < b.Bottom() && a.Bottom() > b.Top();
    }
}