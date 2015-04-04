//
// File:     ShaderDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "ShaderDXF.h"

#include <d3dcompiler.h>

// TODO: update to D3DCompile_47.DLL (will possibly then require Windows 8.1)

// NOTE: any project that uses run-time shader compilation must have D3DCOMPILER_46.DLL copied to the
//       local executable path for the project
// Custom Build Step - General
// Command Line: copy /y "$(WindowsSdkDir)redist\d3d\$(PlatformTarget)\D3DCompile*.DLL" "$(TargetDir)"
// Description: Performing Custom Build Step
// Outputs: D3DCompile_46.DLL;%(Outputs)
// Target Output As Content: Yes




// NOTE: using the follow defines to give some context to the aligned byte offsets used in the shader
//static_assert(sizeof(float) == 4, "assert failed, sizeof(float) != 4 bytes");

//
// TODO: use static assert
//

C_ASSERT(sizeof(float) == 4);

#define XMFLOAT2_SIZE 8
#define XMFLOAT3_SIZE 12
#define XMFLOAT4_SIZE 16



namespace DXF {

    HRESULT InitProgram(RendererDX_t* pRenderer, LPCTSTR szProgramName, ProgramDX_t* pProgram) {
        HRESULT hr;

        // compile the vertex shader
        ID3DBlob *pVSBlob = nullptr;
        hr = CompileShaderFromFile(szProgramName, TEXT("VS"), TEXT("vs_4_0"), &pVSBlob);
        if (FAILED(hr)) {
            return hr;
        }

        // create the vertex shader
        hr = pRenderer->pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
            nullptr, &(pProgram->pVertexShader));
        if (FAILED(hr)) {
            // NOTE: not using CHECK_HRESULT macor here since need to perform some custom error handling
            pVSBlob->Release();
            return hr;
        }

        // setup the input layout
        const D3D11_INPUT_ELEMENT_DESC layout[] = {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        // create the input layout
        UINT numElements = ARRAYSIZE(layout);
        hr = pRenderer->pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(), &(pProgram->pInputLayout));
        pVSBlob->Release();
        if (FAILED(hr)) {
            return hr;
        }

        //
        // TODO: if input layout doesn't need to be reset when changing shaders then no need to store it
        //

        // NOTE: IA prefix on context functions stands for input assembler
        //       OM -> output-merger (stage)

        // set the input layout
        pRenderer->pImmediateContext->IASetInputLayout(pProgram->pInputLayout);


        // compile the pixel shader
        ID3DBlob *pPSBlob = nullptr;
        hr = CompileShaderFromFile(szProgramName, TEXT("PS"), TEXT("ps_4_0"), &pPSBlob);
        if (FAILED(hr)) {
            return hr;
        }

        // create the pixel shader
        hr = pRenderer->pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
            nullptr, &(pProgram->pPixelShader));
        pPSBlob->Release();
        if (FAILED(hr)) {
            return hr;
        }

        return S_OK;
    }

    void DestroyProgram(ProgramDX_t* pProgram) {
        if (pProgram->pInputLayout) { pProgram->pInputLayout->Release(); }
        if (pProgram->pVertexShader) { pProgram->pVertexShader->Release(); }
        if (pProgram->pPixelShader) { pProgram->pPixelShader->Release(); }

        if (pProgram->pBlinnPhong) { pProgram->pBlinnPhong->Release(); }
    }

    HRESULT InitConstBuffers(RendererDX_t* pRenderer, ConstantsDX_t* pConstants) {
        HRESULT hr;
        D3D11_BUFFER_DESC bd = {};

        // setup three constant buffers
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(CBNeverChanges_t);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = pRenderer->pDevice->CreateBuffer(&bd, nullptr, &(pConstants->pNeverChanges));
        if (FAILED(hr)) {
            return hr;
        }

        bd.ByteWidth = sizeof(CBChangeOnResize_t);
        hr = pRenderer->pDevice->CreateBuffer(&bd, nullptr, &(pConstants->pChangeOnResize));
        if (FAILED(hr)) {
            return hr;
        }

        bd.ByteWidth = sizeof(CBChangesEveryFrame_t);
        hr = pRenderer->pDevice->CreateBuffer(&bd, nullptr, &(pConstants->pChangesEveryFrame));
        if (FAILED(hr)) {
            return hr;
        }

        //
        // TODO: need to re-think how lights should be stored
        //
        bd.ByteWidth = sizeof(CBLightBuffer_t);
        hr = pRenderer->pDevice->CreateBuffer(&bd, nullptr, &(pConstants->pLightBuffer));
        if (FAILED(hr)) {
            return hr;
        }

        return S_OK;
    }

    void DestroyConstBuffers(ConstantsDX_t* pConstants) {
        if (pConstants->pChangeOnResize) { pConstants->pChangeOnResize->Release(); }
        if (pConstants->pChangesEveryFrame) { pConstants->pChangesEveryFrame->Release(); }
        if (pConstants->pNeverChanges) { pConstants->pNeverChanges->Release(); }

        if (pConstants->pLightBuffer) { pConstants->pLightBuffer->Release(); }
    }

    //
    // TODO: the common convention with Direct3D is to compile the shaders during compile
    //       time rather than runtime, the drivers will then perform their optimizations
    //       when converting the D3D blob into architecture specific machine code
    //

    // NOTE: when the cso (compiled shader object) file is read the resulting read is the compiled byte arrays
    //       i.e. the blob which should match the ID3DBlob output from D3DCompileFromFile

    HRESULT CompileShaderFromFile(LPCTSTR szFilename, LPCTSTR szEntryPoint, LPCTSTR szShaderModel, ID3DBlob** ppBlobOut) {
        HRESULT hr = S_OK;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        // will embed debug information in the shaders but still allows the shaders to be
        // optimized and to run exactly the same as they would in release mode
        dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob* pErrorBlob = nullptr;

#ifdef _UNICODE
        SIZE_T size = SizeNeededInANSI(szEntryPoint);
        LPSTR szEntryPointA = (LPSTR)LocalAlloc(LMEM_ZEROINIT, size);
        assert(szEntryPointA != nullptr);

        hr = UTF8toANSI(szEntryPoint, (int)size, szEntryPointA);
        if (FAILED(hr)) {
            DXF_ERROR_BOX();
        }

        size = SizeNeededInANSI(szShaderModel);
        LPSTR szShaderModelA = (LPSTR)LocalAlloc(LMEM_ZEROINIT, size);
        assert(szShaderModelA != nullptr);

        hr = UTF8toANSI(szShaderModel, (int)size, szShaderModelA);
        if (FAILED(hr)) {
            DXF_ERROR_BOX();
        }

        hr = D3DCompileFromFile(szFilename, nullptr, nullptr, szEntryPointA, szShaderModelA,
            dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

        LocalFree(szEntryPointA);
        LocalFree(szShaderModelA);
#else
        hr = D3DCompileFromFile(szFilename, nullptr, nullptr, szEntryPoint, szShaderModel,
            dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
#endif

        if (FAILED(hr)) {
            if (pErrorBlob != nullptr) {
                //
                // TODO: check error messages in pErrorBlob
                //
                DXF_ERROR_BOX();
            }

            if (pErrorBlob) {
                pErrorBlob->Release();
            }
            return hr;
        }

        if (pErrorBlob) {
            pErrorBlob->Release();
        }
        return hr;
    }
};