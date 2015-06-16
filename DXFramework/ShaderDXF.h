//
// File:     ShaderDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"
#include "UtilsDXF.h"
#include "RendererDXF.h"

namespace DXF {

    //
    // NOTE: constant buffers require struct is aligned on 16 byte boundaires
    //

    __declspec(align(16)) struct CBNeverChanges_t {
        XMMATRIX view;
    };

    __declspec(align(16)) struct CBChangeOnResize_t {
        XMMATRIX projection;
    };

    __declspec(align(16)) struct CBChangesEveryFrame_t {
        XMMATRIX world;
        //XMFLOAT4 cameraPos;
    };

    __declspec(align(16)) struct CBLightBuffer_t {
        //
        // TODO: could use a better light definition
        //
        XMFLOAT4 lightPos;

        // intensity multipliers
        XMFLOAT4 lightDiffuseColor;
        XMFLOAT4 lightSpecularColor;

        XMFLOAT4 diffuseColor;
        XMFLOAT4 ambientLightColor;
        //XMFLOAT4 emissiveColor; // not currently used in the shader
        XMFLOAT4 specularColor;

        float lightDistanceSquared;
        float specularPower;
    };


    struct ProgramDX_t {
        ID3D11VertexShader*     pVertexShader;
        ID3D11PixelShader*      pPixelShader;
        ID3D11InputLayout*      pInputLayout;

        //
        // TODO: remove and create a separate program instance
        //
        ID3D11PixelShader*      pBlinnPhong;
    };

    struct ConstantsDX_t {
        ID3D11Buffer*  pNeverChanges;
        ID3D11Buffer*  pChangeOnResize;
        ID3D11Buffer*  pChangesEveryFrame;

        ID3D11Buffer*  pLightBuffer;
    };


    //
    // TODO: init should have the ability to determine if a shader source or binary file is being loaded
    //       (may just add an enum)
    //
    HRESULT InitProgram(ProgramDX_t& program, LPCTSTR szProgramName, const RendererDX_t& renderer);
    void DestroyProgram(ProgramDX_t& program);

    HRESULT InitConstBuffers(ConstantsDX_t& constants, RendererDX_t& renderer);
    void DestroyConstBuffers(ConstantsDX_t& constants);
};