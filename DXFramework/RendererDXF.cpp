//
// File:     RendererDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
//

#include "RendererDXF.h"

namespace DXF {

	void checkDirectX11_2(ID3D11Device *pDevice);

	//
	// TODO: move to InitDevice and provide function for setting clear color
	//
	//static const float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // DirectX blue
	//static const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // black
	static const float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; // gray


	HRESULT InitRenderer(_Out_ RendererDX_t *pRenderer, _In_ HWND hwnd) {
		HRESULT hr = S_OK;

		// get the width and height of the HWND
		RECT rc;
		GetClientRect(hwnd, &rc);
		UINT width = rc.right - rc.left;
		UINT height = rc.bottom - rc.top;

		UINT CreateRendererFlags = 0;
#ifdef _DEBUG
		CreateRendererFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// NOTE: only use ref driver for debugging/testing correctness of Direct3D
#ifndef ENABLE_REFERENCE_DEVICE
		// driver types are orderd by preference
		D3D_DRIVER_TYPE driverTypes[] = {
			//
			// TODO: could use a way of manually setting hardware versus WARP and also have
			//       ability to use the reference device without a recompile
			//
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP
		};
#else
		D3D_DRIVER_TYPE driverTypes[] = {
			D3D_DRIVER_TYPE_REFERENCE
		};
#endif
		UINT numDriverTypes = ARRAYSIZE(driverTypes);

		// ordered by perferred feature levels
		// NOTE: DirectX 11.2 is not a specific feature level, just a collection of optional features
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			//D3D_FEATURE_LEVEL_10_1,
			//D3D_FEATURE_LEVEL_10_0
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		// configure swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2; // double buffered to minimize latency
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // most common swap chain format
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1; // sample description 1, 0 == don't use multi-sampling
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		// attempt to get perferred driver type and feature level
		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex) {
			pRenderer->driverType = driverTypes[driverTypeIndex];

			hr = D3D11CreateDeviceAndSwapChain(NULL, pRenderer->driverType, NULL, CreateRendererFlags, featureLevels,
				numFeatureLevels, D3D11_SDK_VERSION, &sd, &(pRenderer->pSwapChain), &(pRenderer->pDevice),
				&(pRenderer->featureLevel), &(pRenderer->pImmediateContext));

			if (SUCCEEDED(hr)) {
#ifdef _DEBUG
				switch (driverTypes[driverTypeIndex]) {
					case D3D_DRIVER_TYPE_HARDWARE: OutputDebugString(TEXT("created hardware accelerated video adapater\n")); break;
					case D3D_DRIVER_TYPE_WARP: OutputDebugString(TEXT("created software (WARP) video adapater\n")); break;
					case D3D_DRIVER_TYPE_REFERENCE: OutputDebugString(TEXT("created debug reference video adapter\n")); break;
					default: assert(FALSE); break; // should never get here
				}
				switch (pRenderer->pDevice->GetFeatureLevel()) {
					case D3D_FEATURE_LEVEL_11_1: {
						OutputDebugString(TEXT("video adapter max feature level DirectX 11.1\n"));
						checkDirectX11_2(pRenderer->pDevice);
					} break;
					case D3D_FEATURE_LEVEL_11_0: OutputDebugString(TEXT("video adapter max feature level DirectX 11.0\n")); break;
					//case D3D_FEATURE_LEVEL_10_1: OutputDebugString(TEXT("video adapter max feature level DirectX 10.1\n")); break;
					//case D3D_FEATURE_LEVEL_10_0: OutputDebugString(TEXT("video adapter max feature level DirectX 10.0\n")); break;
					default: assert(FALSE); break; // should never get here
				}
#endif
				// created a valid driver, can break out of loop
				break;
			}
		}

		// create a render target view
		ID3D11Texture2D *pBackBuffer = NULL;
		hr = pRenderer->pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
		DXF_CHECK_HRESULT(hr);

		hr = pRenderer->pDevice->CreateRenderTargetView(pBackBuffer, NULL, &(pRenderer->pRenderTargetView));
		pBackBuffer->Release();
		DXF_CHECK_HRESULT(hr);

		// create depth stencil view
		ID3D11Texture2D *pDepthStencilBuffer = NULL;
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = pRenderer->pDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencilBuffer);
		DXF_CHECK_HRESULT(hr);

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = pRenderer->pDevice->CreateDepthStencilView(pDepthStencilBuffer, &descDSV, &(pRenderer->pDepthStencilView));
		pDepthStencilBuffer->Release();
		DXF_CHECK_HRESULT(hr);

		// set render target
		pRenderer->pImmediateContext->OMSetRenderTargets(1, &(pRenderer->pRenderTargetView), pRenderer->pDepthStencilView);

		// setup the viewport
		pRenderer->vp.Width = (FLOAT)width;
		pRenderer->vp.Height = (FLOAT)height;
		pRenderer->vp.MinDepth = 0.0f;
		pRenderer->vp.MaxDepth = 1.0f;
		pRenderer->vp.TopLeftX = 0;
		pRenderer->vp.TopLeftY = 0;
		pRenderer->pImmediateContext->RSSetViewports(1, &(pRenderer->vp));

		return hr;
	}

	void PresentView(_In_ RendererDX_t *pRenderer) {
		pRenderer->pSwapChain->Present(0, 0);
	}

	void ClearView(_In_ RendererDX_t *pRenderer) {
		pRenderer->pImmediateContext->ClearRenderTargetView(pRenderer->pRenderTargetView, clearColor);
		pRenderer->pImmediateContext->ClearDepthStencilView(pRenderer->pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
	}

	void DestroyRenderer(_Inout_ RendererDX_t *pRenderer) {
		if (pRenderer->pImmediateContext) { pRenderer->pImmediateContext->ClearState(); }
		if (pRenderer->pRenderTargetView) { pRenderer->pRenderTargetView->Release(); }
		if (pRenderer->pSwapChain) { pRenderer->pSwapChain->Release(); }
		if (pRenderer->pImmediateContext) { pRenderer->pImmediateContext->Release(); }
		if (pRenderer->pDevice) { pRenderer->pDevice->Release(); }
	}

	void checkDirectX11_2(ID3D11Device *pDevice) {
		D3D11_FEATURE_DATA_D3D11_OPTIONS1 featureData;
		pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &featureData, sizeof(featureData));
		D3D11_TILED_RESOURCES_TIER tiledResourcesTier = featureData.TiledResourcesTier;

		if (featureData.MinMaxFiltering == TRUE) {
			OutputDebugString(TEXT("video adapter supports DirectX 11.2 MinMaxFiltering\n"));
		}

		if (featureData.ClearViewAlsoSupportsDepthOnlyFormats == TRUE) {
			OutputDebugString(TEXT("video adapter supports DirectX 11.2 ClearViewAlsoSupportsDepthOnlyFormats\n"));
		}

		if (featureData.MapOnDefaultBuffers == TRUE) {
			OutputDebugString(TEXT("video adapter supports DirectX 11.2 MapOnDefaultBuffers\n"));
		}

		switch (tiledResourcesTier)
		{
			case D3D11_TILED_RESOURCES_NOT_SUPPORTED:
				OutputDebugString(TEXT("video adapter does NOT support DirectX 11.2 tiled resources\n"));
				break;
			case D3D11_TILED_RESOURCES_TIER_1:
				OutputDebugString(TEXT("video adapter supports DirectX 11.2 tier 1 tiled resources\n"));
				break;
			case D3D11_TILED_RESOURCES_TIER_2:
				OutputDebugString(TEXT("video adapter supports DirectX 11.2 tier 2 tiled resources\n"));
				break;
			default:
				assert(FALSE); // should never get here
				break;
		}
	}
};
