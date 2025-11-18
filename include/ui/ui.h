#pragma once

#include "ui_core.h"
#include "ui_context.h"
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace UiKit {

    struct AppWindow {
        HWND hwnd;
        int width;
        int height;
        bool running;
        
        Window* context;
        ID3D12Device* device;
        IDXGIFactory4* factory;
        ID3D12CommandQueue* commandQueue;

        IDXGISwapChain3* swapChain;
        ID3D12Resource* renderTargets[2];       // * double buffering
        ID3D12DescriptorHeap* rtvHeap;
        UINT frameIndex;
        UINT rtvDescriptorSize;   

        ID3D12CommandAllocator* commandAllocators[2];
        ID3D12GraphicsCommandList* commandList;  

        ID3D12Fence* fence;
        UINT64 fenceValues[2];
        HANDLE fenceEvent; 

        ID3D12RootSignature* rootSignature;
        ID3D12PipelineState* pipelineState;

        ID3D12Resource* vertexBuffer;
        ID3D12Resource* indexBuffer;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        UINT vertexBufferSize;
        UINT indexBufferSize;
    };

    AppWindow* CreateAppWindow(const char* title, int width, int height);
    void DestroyAppWindow(AppWindow* app);
    bool IsRunning(AppWindow* app);
    void ProcessEvents(AppWindow* app);
    bool BeginFrame(AppWindow* app);
    void EndFrame(AppWindow* app);
}