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


    HRESULT InitProgram(RendererDX_t* pRenderer, LPCTSTR szProgramName, ProgramDX_t* pProgram);
    void DestroyProgram(ProgramDX_t* pProgram);

    HRESULT InitConstBuffers(RendererDX_t* pRenderer, ConstantsDX_t* pConstants);
    void DestroyConstBuffers(ConstantsDX_t* pConstants);

    HRESULT CompileShaderFromFile(LPCTSTR szFilename, LPCTSTR szEntryPoint, LPCTSTR szShaderModel, ID3DBlob** ppBlobOut);

    //
    // TODO: implement load shader from file to load precompiled blobs
    //
};