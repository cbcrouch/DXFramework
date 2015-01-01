//
// File:     ShaderDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"

#include "RendererDXF.h"

namespace DXF {

	//
	// NOTE: constant buffers require struct is aligned on 16 byte boundaires
	//

	typedef __declspec(align(16)) struct CBNeverChanges_t {
		XMMATRIX view;
	} CBNeverChanges_t;

	typedef __declspec(align(16)) struct CBChangeOnResize_t {
		XMMATRIX projection;
	} CBChangeOnResize_t;

	typedef __declspec(align(16)) struct CBChangesEveryFrame_t {
		XMMATRIX world;
		//XMFLOAT4 cameraPos;
	} CBChangesEveryFrame_t;

	typedef __declspec(align(16)) struct CBLightBuffer_t {
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
	} CBLightBuffer_t;


	typedef struct ProgramDX_t {
		ID3D11VertexShader*     pVertexShader;
		ID3D11PixelShader*      pPixelShader;
		ID3D11InputLayout*      pInputLayout;

		//
		// TODO: remove and create a separate program instance
		//
		ID3D11PixelShader*      pBlinnPhong;
	} ProgramDX_t;

	typedef struct ConstantsDX_t {
		ID3D11Buffer*  pNeverChanges;
		ID3D11Buffer*  pChangeOnResize;
		ID3D11Buffer*  pChangesEveryFrame;

		ID3D11Buffer*  pLightBuffer;
	} ConstantsDX_t;


	HRESULT InitProgram(_Out_ ProgramDX_t *pProgram, _In_z_ LPCTSTR szProgramName, _In_ RendererDX_t *pRenderer);
	void DestroyProgram(_Inout_ ProgramDX_t *pProgram);

	HRESULT InitConstBuffers(_Out_ ConstantsDX_t *pConstants, _In_ RendererDX_t *pRenderer);
	void DestroyConstBuffers(_Inout_ ConstantsDX_t *pConstants);

	HRESULT CompileShaderFromFile(_In_z_ LPCTSTR szFilename, _In_z_ LPCTSTR szEntryPoint, _In_z_ LPCTSTR szShaderModel, _Outptr_ ID3DBlob **ppBlobOut);

	//
	// TODO: implement load shader from file to load precompiled blobs
	//
};