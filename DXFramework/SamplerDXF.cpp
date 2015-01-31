//
// File:     SamplerDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "SamplerDXF.h"

namespace DXF {
	HRESULT InitSampler(_In_ RendererDX_t *pRenderer, _Out_ SamplerDX_t *pSampler) {
		HRESULT hr = S_OK;

		// create the sampler state
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		//sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		//sampDesc.MaxAnisotropy = 1;

		hr = pRenderer->pDevice->CreateSamplerState(&sampDesc, &(pSampler->pSamplerState));
		return hr;
	}

	void DestroySampler(_Inout_ SamplerDX_t *pSampler) {
		if (pSampler->pSamplerState) { pSampler->pSamplerState->Release(); }
	}
};
