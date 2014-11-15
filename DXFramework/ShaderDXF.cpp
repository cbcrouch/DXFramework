//
// File:     ShaderDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
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
static_assert(sizeof(float) == 4, "assert failed, sizeof(float) != 4 bytes");

#define XMFLOAT2_SIZE 8
#define XMFLOAT3_SIZE 12
#define XMFLOAT4_SIZE 16

namespace DXF {

	HRESULT InitProgram(_Out_ ProgramDX_t *pProgram, _In_z_ LPCTSTR szProgramName, _In_ RendererDX_t *pRenderer) {
		HRESULT hr;

		// compile the vertex shader
		ID3DBlob *pVSBlob = NULL;
		hr = CompileShaderFromFile(szProgramName, TEXT("VS"), TEXT("vs_4_0"), &pVSBlob);
		if (FAILED(hr)) {
			return hr;
		}

		// create the vertex shader
		hr = pRenderer->pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
			NULL, &(pProgram->pVertexShader));
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
		ID3DBlob *pPSBlob = NULL;
		hr = CompileShaderFromFile(szProgramName, TEXT("PS"), TEXT("ps_4_0"), &pPSBlob);
		if (FAILED(hr)) {
			return hr;
		}

		// create the pixel shader
		hr = pRenderer->pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
			NULL, &(pProgram->pPixelShader));
		pPSBlob->Release();
		if (FAILED(hr)) {
			return hr;
		}

		return S_OK;
	}

	void DestroyProgram(_Inout_ ProgramDX_t *pProgram) {
		if (pProgram->pInputLayout) { pProgram->pInputLayout->Release(); }
		if (pProgram->pVertexShader) { pProgram->pVertexShader->Release(); }
		if (pProgram->pPixelShader) { pProgram->pPixelShader->Release(); }

		if (pProgram->pBlinnPhong) { pProgram->pBlinnPhong->Release(); }
	}

	HRESULT InitConstBuffers(_Out_ ConstantsDX_t *pConstants, _In_ RendererDX_t *pRenderer) {
		HRESULT hr;
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

		// setup three constant buffers
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CBNeverChanges_t);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		hr = pRenderer->pDevice->CreateBuffer(&bd, NULL, &(pConstants->pNeverChanges));
		if (FAILED(hr)) {
			return hr;
		}

		bd.ByteWidth = sizeof(CBChangeOnResize_t);
		hr = pRenderer->pDevice->CreateBuffer(&bd, NULL, &(pConstants->pChangeOnResize));
		if (FAILED(hr)) {
			return hr;
		}

		bd.ByteWidth = sizeof(CBChangesEveryFrame_t);
		hr = pRenderer->pDevice->CreateBuffer(&bd, NULL, &(pConstants->pChangesEveryFrame));
		if (FAILED(hr)) {
			return hr;
		}

		//
		// TODO: need to re-think how lights should be stored
		//
		bd.ByteWidth = sizeof(CBLightBuffer_t);
		hr = pRenderer->pDevice->CreateBuffer(&bd, NULL, &(pConstants->pLightBuffer));
		if (FAILED(hr)) {
			return hr;
		}

		return S_OK;
	}

	void DestroyConstBuffers(_Inout_ ConstantsDX_t *pConstants) {
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

	HRESULT CompileShaderFromFile(_In_z_ LPCTSTR szFilename, _In_z_ LPCTSTR szEntryPoint, _In_z_ LPCTSTR szShaderModel, _Outptr_ ID3DBlob **ppBlobOut) {
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
		// will embed debug information in the shaders but still allows the shaders to be
		// optimized and to run exactly the same as they would in release mode
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob *pErrorBlob;

		//
		// TODO: clean up string converion code
		//

		// CP_ACP  for ANSI code page
/*
		// ANSI string to Unicode (UTF8)
		LPSTR lpszFunction = "Test Text";
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, lpszFunction, -1, NULL, 0);
		LPWSTR lpszFunctionW = (LPWSTR)LocalAlloc(LMEM_ZEROINIT, sizeNeeded * 2); // why does this have to be * 2 ?? would using CP_ACP help ??
		hr = MultiByteToWideChar(CP_UTF8, 0, lpszFunction, -1, lpszFunctionW, sizeNeeded);
		if (FAILED(hr)) {
			ERROR_BOX();
		}

		//...

		LocalFree(lpszFunctionW);
*/

#ifdef _UNICODE

		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, szEntryPoint, -1, NULL, 0, NULL, FALSE);
		LPSTR szEntryPointA = (LPSTR)LocalAlloc(LMEM_ZEROINIT, sizeNeeded);
		hr = WideCharToMultiByte(CP_UTF8, 0, szEntryPoint, -1, szEntryPointA, sizeNeeded, NULL, FALSE);
		if (FAILED(hr)) {
			ERROR_BOX();
		}

		sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, szShaderModel, -1, NULL, 0, NULL, FALSE);
		LPSTR szShaderModelA = (LPSTR)LocalAlloc(LMEM_ZEROINIT, sizeNeeded);
		hr = WideCharToMultiByte(CP_UTF8, 0, szShaderModel, -1, szShaderModelA, sizeNeeded, NULL, FALSE);
		if (FAILED(hr)) {
			ERROR_BOX();
		}

		hr = D3DCompileFromFile(szFilename, NULL, NULL, szEntryPointA, szShaderModelA,
			dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

		LocalFree(szEntryPointA);
		LocalFree(szShaderModelA);
#else
		hr = D3DCompileFromFile(szFilename, NULL, NULL, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
#endif

		if (FAILED(hr)) {
			if (pErrorBlob != NULL) {
				//
				// TODO: check error messages in pErrorBlob
				//
				ERROR_BOX();
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