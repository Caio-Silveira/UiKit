#include "../include/ui/ui_shaders.h"
#include "../include/ui/ui.h"
#include <d3dcompiler.h>
#include <cstring>

#pragma comment(lib, "d3dcompiler.lib")

namespace {
    const char* vertexShaderSource = R"(
        cbuffer ScreenData : register(b0) {
            float2 screenSize;
        };
        
        struct VSInput {
            float2 position : POSITION;
            float2 uv : TEXCOORD;
            float4 color : COLOR;
        };
        
        struct PSInput {
            float4 position : SV_POSITION;
            float2 uv : TEXCOORD;
            float4 color : COLOR;
        };
        
        PSInput main(VSInput input) {
            PSInput output;
            
            float2 ndc;
            ndc.x = (input.position.x / screenSize.x) * 2.0f - 1.0f;
            ndc.y = 1.0f - (input.position.y / screenSize.y) * 2.0f;
            
            output.position = float4(ndc, 0.0f, 1.0f);
            output.uv = input.uv;
            output.color = input.color;
            return output;
        }
    )";
    
    const char* pixelShaderSource = R"(
        struct PSInput {
            float4 position : SV_POSITION;
            float2 uv : TEXCOORD;
            float4 color : COLOR;
        };
        
        float4 main(PSInput input) : SV_TARGET {
            return input.color;
        }
    )";
}

namespace UiKit {

    bool CreatePipelineState(AppWindow* app) {
        D3D12_ROOT_PARAMETER rootParam = {};
        rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        rootParam.Constants.ShaderRegister = 0;
        rootParam.Constants.RegisterSpace = 0;
        rootParam.Constants.Num32BitValues = 2;
        rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.NumParameters = 1;
        rootSignatureDesc.pParameters = &rootParam;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        
        ID3DBlob* signature;
        ID3DBlob* error;
        if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error))) {
            return false;
        }
        
        if (FAILED(app->device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&app->rootSignature)))) {
            signature->Release();
            return false;
        }
        signature->Release();
        
        ID3DBlob* vertexShader;
        ID3DBlob* pixelShader;
        
        UINT compileFlags = 0;
        #ifdef _DEBUG
        compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        #endif
        
        if (FAILED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource), nullptr, nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, &vertexShader, &error))) {
            return false;
        }
        
        if (FAILED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource), nullptr, nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, &pixelShader, &error))) {
            vertexShader->Release();
            return false;
        }
        
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
        };
        
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = app->rootSignature;
        psoDesc.VS = {vertexShader->GetBufferPointer(), vertexShader->GetBufferSize()};
        psoDesc.PS = {pixelShader->GetBufferPointer(), pixelShader->GetBufferSize()};
        psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        psoDesc.InputLayout = {inputLayout, _countof(inputLayout)};
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 4;
        psoDesc.SampleDesc.Quality = 0;
        
        if (FAILED(app->device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&app->pipelineState)))) {
            vertexShader->Release();
            pixelShader->Release();
            return false;
        }
        
        vertexShader->Release();
        pixelShader->Release();
        
        return true;
    }

}