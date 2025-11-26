#include "../include/ui/ui.h"
#include "../include/ui/ui_shaders.h"
#include "../include/ui/ui_context.h"
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <stdio.h>
#include <dwmapi.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")

namespace UiKit {

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        AppWindow* app = reinterpret_cast<AppWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        
        switch (msg) {
            case WM_CLOSE:
                if (app) app->running = false;
                return 0;
                
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    bool InitD3D12(AppWindow* app) {
        UINT dxgiFactoryFlags = 0;
        
        #ifdef _DEBUG
        ID3D12Debug* debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            debugController->Release();
        }
        #endif
        
        if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&app->factory)))) {
            return false;
        }
        
        if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&app->device)))) {
            return false;
        }
        
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        
        if (FAILED(app->device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&app->commandQueue)))) {
            return false;
        }

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Width = app->width;
        swapChainDesc.Height = app->height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;
        
        IDXGISwapChain1* swapChain1;
        if (FAILED(app->factory->CreateSwapChainForHwnd(
            app->commandQueue,
            app->hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
        ))) {
            return false;
        }
        
        swapChain1->QueryInterface(IID_PPV_ARGS(&app->swapChain));
        swapChain1->Release();
        
        app->frameIndex = app->swapChain->GetCurrentBackBufferIndex();

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 2;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        
        if (FAILED(app->device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&app->rtvHeap)))) {
            return false;
        }
        
        app->rtvDescriptorSize = app->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = app->rtvHeap->GetCPUDescriptorHandleForHeapStart();
        
        for (UINT i = 0; i < 2; i++) {
            app->swapChain->GetBuffer(i, IID_PPV_ARGS(&app->renderTargets[i]));
            app->device->CreateRenderTargetView(app->renderTargets[i], nullptr, rtvHandle);
            rtvHandle.ptr += app->rtvDescriptorSize;
        }

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        clearValue.Color[0] = 0.0f;
        clearValue.Color[1] = 0.0f;
        clearValue.Color[2] = 0.0f;
        clearValue.Color[3] = 1.0f;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        for (UINT i = 0; i < 2; i++) {
            if (FAILED(app->device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(&app->commandAllocators[i])
            ))) {
                return false;
            }
        }

        if (FAILED(app->device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            app->commandAllocators[app->frameIndex],
            nullptr,
            IID_PPV_ARGS(&app->commandList)
        ))) {
            return false;
        }
        
        app->commandList->Close();

        if (FAILED(app->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&app->fence)))) {
            return false;
        }
        
        app->fenceValues[0] = 1;
        app->fenceValues[1] = 1;

        app->fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (app->fenceEvent == nullptr) {
            return false;
        }

        if (!CreatePipelineState(app)) {
            return false;
        }

        app->vertexBuffer = nullptr;
        app->indexBuffer = nullptr;
        app->vertexBufferSize = 0;
        app->indexBufferSize = 0;

        return true;
    }

    AppWindow* CreateAppWindow(const char* title, int width, int height) {
        AppWindow* app = new AppWindow{};
        app->width = width;
        app->height = height;
        app->running = true;
        
        WNDCLASSA wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = "UiKitWindow";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        
        RegisterClassA(&wc);
        
        
        app->hwnd = CreateWindowA(
            "UiKitWindow",
            title,
            WS_POPUP,
            CW_USEDEFAULT, CW_USEDEFAULT,
            width, height,
            nullptr, nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );
        
        SetWindowLongPtr(app->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
        
        DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(app->hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

        ShowWindow(app->hwnd, SW_SHOW);
        UpdateWindow(app->hwnd);
        
        app->context = CreateUIWindow();
        
        if (!InitD3D12(app)) {
            DestroyAppWindow(app);
            return nullptr;
        }

        app->context->GetDrawList()->SetDevice(app->device);
        
        app->clearColor = Color{0.1f, 0.1f, 0.1f, 1.0f};

        return app;
    }

    void UpdateBuffers(AppWindow* app) {
        DrawList* drawList = app->context->GetDrawList();
        
        UINT requiredVertexSize = static_cast<UINT>(drawList->vertices.size() * sizeof(DrawVertex));
        UINT requiredIndexSize = static_cast<UINT>(drawList->indices.size() * sizeof(unsigned int));
        
        if (app->vertexBuffer == nullptr || app->vertexBufferSize < requiredVertexSize) {
            if (app->vertexBuffer) {
                app->vertexBuffer->Release();
            }
            
            app->vertexBufferSize = requiredVertexSize + 5000 * sizeof(DrawVertex);
            
            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            
            D3D12_RESOURCE_DESC resourceDesc = {};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            resourceDesc.Width = app->vertexBufferSize;
            resourceDesc.Height = 1;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            
            app->device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&app->vertexBuffer)
            );
        }
        
        if (app->indexBuffer == nullptr || app->indexBufferSize < requiredIndexSize) {
            if (app->indexBuffer) {
                app->indexBuffer->Release();
            }
            
            app->indexBufferSize = requiredIndexSize + 10000 * sizeof(unsigned int);
            
            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            
            D3D12_RESOURCE_DESC resourceDesc = {};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            resourceDesc.Width = app->indexBufferSize;
            resourceDesc.Height = 1;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            
            app->device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&app->indexBuffer)
            );
        }
        
        void* vertexData;
        app->vertexBuffer->Map(0, nullptr, &vertexData);
        memcpy(vertexData, drawList->vertices.data(), requiredVertexSize);
        app->vertexBuffer->Unmap(0, nullptr);
        
        void* indexData;
        app->indexBuffer->Map(0, nullptr, &indexData);
        memcpy(indexData, drawList->indices.data(), requiredIndexSize);
        app->indexBuffer->Unmap(0, nullptr);
        
        app->vertexBufferView.BufferLocation = app->vertexBuffer->GetGPUVirtualAddress();
        app->vertexBufferView.SizeInBytes = requiredVertexSize;
        app->vertexBufferView.StrideInBytes = sizeof(DrawVertex);
        
        app->indexBufferView.BufferLocation = app->indexBuffer->GetGPUVirtualAddress();
        app->indexBufferView.SizeInBytes = requiredIndexSize;
        app->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
    }

    void WaitForGPU(AppWindow* app) {
        const UINT64 fenceValue = app->fenceValues[app->frameIndex];
        app->commandQueue->Signal(app->fence, fenceValue);
        
        if (app->fence->GetCompletedValue() < fenceValue) {
            app->fence->SetEventOnCompletion(fenceValue, app->fenceEvent);
            WaitForSingleObject(app->fenceEvent, INFINITE);
        }
        
        app->fenceValues[app->frameIndex]++;
    }

    void DestroyAppWindow(AppWindow* app) {
        if (!app) return;
        
        WaitForGPU(app);

        if (app->vertexBuffer) app->vertexBuffer->Release();
        if (app->indexBuffer) app->indexBuffer->Release();

        if (app->fenceEvent) CloseHandle(app->fenceEvent);
        if (app->fence) app->fence->Release();

        if (app->commandList) app->commandList->Release();
        for (UINT i = 0; i < 2; i++) {
            if (app->commandAllocators[i]) app->commandAllocators[i]->Release();
        }

        for (UINT i = 0; i < 2; i++) {
            if (app->renderTargets[i]) app->renderTargets[i]->Release();
        }
        if (app->rtvHeap) app->rtvHeap->Release();
        if (app->swapChain) app->swapChain->Release();

        if (app->commandQueue) app->commandQueue->Release();
        if (app->device) app->device->Release();
        if (app->factory) app->factory->Release();
        
        DestroyUIWindow(app->context);
        
        if (app->hwnd) {
            ::DestroyWindow(app->hwnd);
        }
        
        delete app;
    }

    bool IsRunning(AppWindow* app) {
        return app && app->running;
    }

    void ProcessEvents(AppWindow* app) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                app->running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    bool BeginFrame(AppWindow* app) {
        app->frameIndex = app->swapChain->GetCurrentBackBufferIndex();
        
        app->commandAllocators[app->frameIndex]->Reset();
        app->commandList->Reset(app->commandAllocators[app->frameIndex], app->pipelineState);
        
        return true;
    }

        void EndFrame(AppWindow* app) {
        UpdateBuffers(app);

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = app->renderTargets[app->frameIndex];
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        app->commandList->ResourceBarrier(1, &barrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = app->rtvHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += app->frameIndex * app->rtvDescriptorSize;
        
        const float clearColor[] = {app->clearColor.r, app->clearColor.g, app->clearColor.b, app->clearColor.a};
        app->commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        app->commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        
        D3D12_VIEWPORT viewport = {0.0f, 0.0f, (float)app->width, (float)app->height, 0.0f, 1.0f};
        D3D12_RECT scissor = {0, 0, (LONG)app->width, (LONG)app->height};
        app->commandList->RSSetViewports(1, &viewport);
        app->commandList->RSSetScissorRects(1, &scissor);
        
        app->commandList->SetGraphicsRootSignature(app->rootSignature);
        float screenSize[2] = {(float)app->width, (float)app->height};
        app->commandList->SetGraphicsRoot32BitConstants(0, 2, screenSize, 0);

        app->commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        app->commandList->IASetVertexBuffers(0, 1, &app->vertexBufferView);
        app->commandList->IASetIndexBuffer(&app->indexBufferView);
        
        DrawList* drawList = app->context->GetDrawList();
        for (auto& cmd : drawList->commands) {
            app->commandList->DrawIndexedInstanced(cmd.indexCount, 1, cmd.indexOffset, cmd.vertexOffset, 0);
        }

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        app->commandList->ResourceBarrier(1, &barrier);
        
        app->commandList->Close();
        
        ID3D12CommandList* commandLists[] = {app->commandList};
        app->commandQueue->ExecuteCommandLists(1, commandLists);

        const UINT64 currentFenceValue = app->fenceValues[app->frameIndex];
        app->commandQueue->Signal(app->fence, currentFenceValue);
        
        app->swapChain->Present(1, 0);

        app->frameIndex = app->swapChain->GetCurrentBackBufferIndex();
        
        if (app->fence->GetCompletedValue() < app->fenceValues[app->frameIndex]) {
            app->fence->SetEventOnCompletion(app->fenceValues[app->frameIndex], app->fenceEvent);
            WaitForSingleObject(app->fenceEvent, INFINITE);
        }

        app->fenceValues[app->frameIndex] = currentFenceValue + 1;
    }

    void SetClearColor(AppWindow* app, Color color) {
        app->clearColor = color;
    }
}