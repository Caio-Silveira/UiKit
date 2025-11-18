#include "../include/ui/ui.h"
#include "../include/ui/ui_shaders.h"
#include "../include/ui/ui_context.h"
#include <windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

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
        
        RECT rect = {0, 0, width, height};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
        
        app->hwnd = CreateWindowA(
            "UiKitWindow",
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr, nullptr,
            GetModuleHandle(nullptr),
            nullptr
        );
        
        SetWindowLongPtr(app->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
        
        ShowWindow(app->hwnd, SW_SHOW);
        UpdateWindow(app->hwnd);
        
        app->context = CreateUIWindow();
        
        if (!InitD3D12(app)) {
            DestroyAppWindow(app);
            return nullptr;
        }

        return app;
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
}