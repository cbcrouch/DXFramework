//
// File:     RendererDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"

// NOTE: should only be enabled for debugging Direct3D issues that might be related to the driver
//#define ENABLE_REFERENCE_DEVICE

namespace DXF {

	struct RendererDX_t {
		//
		// TODO: populate members for intializing device or pass in a configuration struct containing
		//       init members and leave the device an opaque struct
		//
		DXGI_FORMAT bufferFormat;
		UINT bufferCount;
		DXGI_RATIONAL refreshRate; // target refresh rate
		DXGI_SAMPLE_DESC sampleDesc; // count 1, quality 0 == don't use multi-sampling

		D3D11_VIEWPORT vp;

		// DXGI usage ?? (this one might be too application specific or might be nice
		// to support rendering to an offscreen buffer and then encoding as video ??)

		// clear color ?? (e.g. if debug make it hot pink, else black)

		// driver config
		D3D_DRIVER_TYPE    driverType; // target driver type
		D3D_FEATURE_LEVEL  featureLevel; // target feature level

		//
		// TODO: move everything above this comment into a separate config struct
		//


		// device and context
		ID3D11Device            *pDevice;

		//http://msdn.microsoft.com/en-us/library/windows/desktop/ff476892(v=vs.85).aspx
		ID3D11DeviceContext     *pImmediateContext;

		//http://msdn.microsoft.com/en-us/library/windows/desktop/ff476887(v=vs.85).aspx
		//ID3D11DeviceContext     *pDeferredContext;

		//
		// TODO: does the device context number correspond to the DirectX feature level ?? do devices and their context
		//       have to be explicitly tied to the feature level (i.e ID3D11Device is not a general case that will support
		//       all of the DirectX 11 spec including 11.1 and 11.2) ??
		//

		//http://msdn.microsoft.com/en-us/library/windows/desktop/ff476880(v=vs.85).aspx

		//ID3D11Device1
		//ID3D11Device2
		//ID3D11DeviceContext1
		//ID3D11DeviceContext2


		IDXGISwapChain          *pSwapChain;
		ID3D11RenderTargetView  *pRenderTargetView;
		ID3D11DepthStencilView  *pDepthStencilView;

	};


	HRESULT InitRenderer(_In_ HWND hwnd, _Out_ RendererDX_t *pRenderer);
	void DestroyRenderer(_Inout_ RendererDX_t *pRenderer);

	void PresentView(_In_ RendererDX_t *pRenderer);
	void ClearView(_In_ RendererDX_t *pRenderer);
};
