#pragma once

#include "ui_core.h"
#include "ui_helpers.h"
#include <vector>

namespace UiKit {

    struct DrawVertex {
        Vec2 position;
        Vec2 uv;
        unsigned int color;
    };

    struct DrawCommand {
        unsigned int indexCount;
        unsigned int indexOffset;
        unsigned int vertexOffset;
        Rect clipRect;
        void* textureId;
    };

    class DrawList {
    public:
        std::vector<DrawVertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<DrawCommand> commands;
        
        void Clear() {
            vertices.clear();
            indices.clear();
            commands.clear();
            clipRect = {};
            textureId = nullptr;
        }
        
        void SetClipRect(Rect rect) { clipRect = rect; }
        void ResetClipRect() { clipRect = {}; }
        void SetTextureId(void* id) { textureId = id; }
        
        void AddLine(Vec2 start, Vec2 end, Color color, float thickness = 1.0f) {
            if (thickness <= 0.0f) return;
            
            Vec2 offset = GetPerpendicularNormalized(end - start) * (thickness * 0.5f);
            Vec2 pos[4] = {start + offset, end + offset, end - offset, start - offset};
            Vec2 uvs[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
            
            AddVert4(pos, uvs, color);
        }
        
        void Draw0(Vec2 center, float radius, Color color, float thickness = 1.0f, int segments = 0) {
            if (segments <= 0) segments = CalculateCircleSegmentCount(radius);
            AddArc(center, radius, 0.0f, {color, thickness, segments});
        }
        
        void DrawFill0(Vec2 center, float radius, Color color, int segments = 0) {
            if (segments <= 0) segments = CalculateCircleSegmentCount(radius);
            
            unsigned int centerIdx = MarkCenterVertex(color);
            PushVert({center, {0.5f, 0.5f}});
            AddCircleVert(center, radius, segments);
            Vert2Mid(centerIdx);
        }
        
        void Draw3(Vec2 p1, Vec2 p2, Vec2 p3, Color color, float thickness = 1.0f) {
            Vec2 pts[3] = {p1, p2, p3};
            AddLineLoop(pts, 3, {color, thickness});
        }
        
        void DrawFill3(Vec2 p1, Vec2 p2, Vec2 p3, Color color) {
            Vec2 pos[3] = {p1, p2, p3};
            Vec2 uvs[3] = {{0, 0}, {1, 0}, {0.5f, 1}};
            AddVert3(pos, uvs, color);
        }
        
        void Draw4(Rect rect, Color color, float thickness = 1.0f, Vec4 rounding = {}) {
            rounding = ClampCornerRadius(rect, rounding);
            
            if (IsCornerRadiusZero(rounding)) {
                Vec2 pts[4] = {rect.TopLeft(), rect.TopRight(), rect.BottomRight(), rect.BottomLeft()};
                AddLineLoop(pts, 4, {color, thickness});
                return;
            }
            
            DrawRoundedCorners(rect, rounding, {color, thickness, 8});
            DrawRoundedSides(rect, rounding, {color, thickness});
        }
        
        void DrawFill4(Rect rect, Color color, Vec4 rounding = {}) {
            rounding = ClampCornerRadius(rect, rounding);
            
            if (IsCornerRadiusZero(rounding)) {
                Vec2 pos[4] = {rect.TopLeft(), rect.TopRight(), rect.BottomRight(), rect.BottomLeft()};
                Vec2 uvs[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
                AddVert4(pos, uvs, color);
                return;
            }
            
            unsigned int centerIdx = MarkCenterVertex(color);
            PushVert({rect.Center(), {0.5f, 0.5f}});
            FillRoundedCorners(rect, rounding, 8);
            Vert2Mid(centerIdx);
        }
        
        void DrawImg(Rect rect, void* textureId, Color tint = Color{1, 1, 1, 1}) {
            SetTextureId(textureId);
            DrawFill4(rect, tint);
            SetTextureId(nullptr);
        }

    private:
        Rect clipRect;
        void* textureId = nullptr;
        unsigned int currentColor = 0xFFFFFFFF;
        
        unsigned int MarkCenterVertex(Color color) {
            currentColor = color.ToABGR();
            return static_cast<unsigned int>(vertices.size());
        }
        
        void PushVert(DrawVertex vertex) {
            vertex.color = currentColor;
            vertices.push_back(vertex);
        }
        
        void PushIdx(unsigned int index) {
            indices.push_back(index);
            if (indices.size() % 3 == 0) EnsureCmd();
        }
        
        void AddVert3(const Vec2* pos, const Vec2* uvs, Color color) {
            unsigned int baseIdx = static_cast<unsigned int>(vertices.size());
            currentColor = color.ToABGR();
            
            for (int i = 0; i < 3; ++i) PushVert({pos[i], uvs[i]});
            
            PushIdx(baseIdx);
            PushIdx(baseIdx + 1);
            PushIdx(baseIdx + 2);
        }
        
        void AddVert4(const Vec2* pos, const Vec2* uvs, Color color) {
            unsigned int baseIdx = static_cast<unsigned int>(vertices.size());
            currentColor = color.ToABGR();
            
            for (int i = 0; i < 4; ++i) PushVert({pos[i], uvs[i]});
            
            PushIdx(baseIdx);
            PushIdx(baseIdx + 1);
            PushIdx(baseIdx + 2);
            PushIdx(baseIdx);
            PushIdx(baseIdx + 2);
            PushIdx(baseIdx + 3);
        }
        
        void AddLineLoop(const Vec2* pts, int count, LineState state) {
            for (int i = 0; i < count; ++i) {
                AddLine(pts[i], pts[(i + 1) % count], state.color, state.thickness);
            }
        }
        
        void AddArcVert(Vec2 center, float radius, float startAngle, int segments) {
            float step = (PI * 0.5f) / segments;
            for (int i = 0; i <= segments; ++i) {
                PushVert({GetCirclePoint(center, radius, startAngle + i * step), {0, 0}});
            }
        }
        
        void AddCircleVert(Vec2 center, float radius, int segments) {
            float step = TAU / segments;
            for (int i = 0; i <= segments; ++i) {
                PushVert({GetCirclePoint(center, radius, i * step), {0, 0}});
            }
        }
        
        void AddArc(Vec2 center, float radius, float startAngle, ArcState state) {
            float step = (PI * 0.5f) / state.segments;
            for (int i = 0; i < state.segments; ++i) {
                AddLine(GetCirclePoint(center, radius, startAngle + i * step),
                    GetCirclePoint(center, radius, startAngle + (i + 1) * step),
                    state.color, state.thickness);
            }
        }
        
        void DrawRoundedCorners(Rect rect, Vec4 rounding, ArcState state) {
            CornerArcPositions corners = GetCornerArcCenters(rect, rounding);
            float angles[4] = {PI, PI * 1.5f, 0.0f, PI * 0.5f};
            Vec2 centers[4] = {corners.topLeft, corners.topRight, corners.bottomRight, corners.bottomLeft};
            float radii[4] = {rounding.x, rounding.y, rounding.z, rounding.w};
            
            for (int i = 0; i < 4; ++i) AddArc(centers[i], radii[i], angles[i], state);
        }
        
        void DrawRoundedSides(Rect rect, Vec4 rounding, LineState state) {
            Vec2 pts[8] = {
                {rect.Left() + rounding.x, rect.Top()}, {rect.Right() - rounding.y, rect.Top()},
                {rect.Right(), rect.Top() + rounding.y}, {rect.Right(), rect.Bottom() - rounding.z},
                {rect.Right() - rounding.z, rect.Bottom()}, {rect.Left() + rounding.w, rect.Bottom()},
                {rect.Left(), rect.Bottom() - rounding.w}, {rect.Left(), rect.Top() + rounding.y}
            };
            
            for (int i = 0; i < 8; i += 2) AddLine(pts[i], pts[i + 1], state.color, state.thickness);
        }
        
        void FillRoundedCorners(Rect rect, Vec4 rounding, int segments) {
            CornerArcPositions corners = GetCornerArcCenters(rect, rounding);
            float angles[4] = {PI, PI * 1.5f, 0.0f, PI * 0.5f};
            Vec2 centers[4] = {corners.topLeft, corners.topRight, corners.bottomRight, corners.bottomLeft};
            float radii[4] = {rounding.x, rounding.y, rounding.z, rounding.w};
            
            for (int i = 0; i < 4; ++i) AddArcVert(centers[i], radii[i], angles[i], segments);
        }
        
        void Vert2Mid(unsigned int centerIdx) {
            unsigned int count = static_cast<unsigned int>(vertices.size()) - centerIdx - 1;
            
            for (unsigned int i = 0; i < count - 1; ++i) {
                PushIdx(centerIdx);
                PushIdx(centerIdx + i + 1);
                PushIdx(centerIdx + i + 2);
            }
            
            PushIdx(centerIdx);
            PushIdx(centerIdx + count);
            PushIdx(centerIdx + 1);
        }
        
        void EnsureCmd() {
            if (commands.empty() || IsStateChanged()) {
                CreateCmd();
            }
            
            commands.back().indexCount += 3;
        }
        
        bool IsStateChanged() {
            if (commands.empty()) return true;
            
            DrawCommand& last = commands.back();
            return last.clipRect.position != clipRect.position ||
                last.clipRect.size != clipRect.size ||
                last.textureId != textureId;
        }
        
        void CreateCmd() {
            DrawCommand cmd;
            cmd.indexCount = 0;
            
            if (commands.empty()) {
                cmd.indexOffset = 0;
            } else {
                DrawCommand& last = commands.back();
                cmd.indexOffset = last.indexOffset + last.indexCount;
            }
            
            cmd.vertexOffset = 0;
            cmd.clipRect = clipRect;
            cmd.textureId = textureId;
            commands.push_back(cmd);
        }
    };

}