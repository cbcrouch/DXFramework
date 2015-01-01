//
// File:     SamplerDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"

#include "RendererDXF.h"

namespace DXF {

	//
	// TODO: store useful params in struct like max anisotropy to avoid having
	//       to get it from Direct3D
	//
	typedef struct SamplerDX_t {

		//...

		ID3D11SamplerState*        pSamplerState;
	} SamplerDX_t;

	HRESULT InitSampler(_Out_ SamplerDX_t *pSampler, _In_ RendererDX_t *pRenderer);
	void DestroySampler(_Inout_ SamplerDX_t *pSampler);
};
