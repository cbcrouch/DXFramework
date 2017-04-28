//
// File:     SamplerDXF.h
// Project:  DXFramework
//
// Copyright (c) 2017 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"
#include "UtilsDXF.h"
#include "RendererDXF.h"

namespace DXF {

    //
    // TODO: store useful params in struct like max anisotropy to avoid having
    //       to get it from Direct3D
    //
    struct SamplerDX_t {
        ID3D11SamplerState*        pSamplerState;
    };

    HRESULT InitSampler(SamplerDX_t& sampler, const RendererDX_t& renderer);
    void DestroySampler(SamplerDX_t& sampler);
};
