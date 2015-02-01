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
	struct SamplerDX_t {
		ID3D11SamplerState*        pSamplerState;
	};

	HRESULT InitSampler(RendererDX_t* pRenderer, SamplerDX_t* pSampler);
	void DestroySampler(SamplerDX_t* pSampler);
};
