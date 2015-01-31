//
// File:     ResourceDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#include "ResourceDXF.h"

namespace DXF {

	HRESULT GenerateGridXZ(_In_ RendererDX_t *pRenderer, const int32_t extent, _Out_ EntityDX_t *pEntity) {
		pEntity->pMesh = (MeshDX_t *)malloc(sizeof(MeshDX_t));
		assert(pEntity->pMesh != NULL);

		int numVertices = (extent * 8) + 4;
		pEntity->pMesh->numVertices = numVertices;

		pEntity->pMesh->pVertices = (XMVertex_t *)malloc(numVertices * sizeof(XMVertex_t));
		pEntity->pMesh->pIndices = (WORD *)malloc(numVertices * sizeof(WORD));
		assert(pEntity->pMesh->pVertices != NULL);
		assert(pEntity->pMesh->pIndices != NULL);

		// set vertex data
		int index = 0;
		for (int i = -extent; i <= extent; ++i) {
			pEntity->pMesh->pVertices[index].pos   = XMFLOAT3((float)extent, 0.0f, (float)i);
			pEntity->pMesh->pVertices[index+1].pos = XMFLOAT3((float)-extent, 0.0f, (float)i);
			index += 2;
		}
		for (int i = -extent; i <= extent; ++i) {
			pEntity->pMesh->pVertices[index].pos = XMFLOAT3((float)i, 0.0f, (float)extent);
			pEntity->pMesh->pVertices[index + 1].pos = XMFLOAT3((float)i, 0.0f, (float)-extent);
			index += 2;
		}

		// set index data
		for (int i = 0; i < numVertices; ++i) {
			pEntity->pMesh->pIndices[i] = i;
		}



		HRESULT hr;
		D3D11_BUFFER_DESC bd;
		D3D11_SUBRESOURCE_DATA initData;

		// create vertex buffer
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(XMVertex_t) * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;


		//
		// TODO: value initialization, zero memory sizeof(var), or zero memory sizeof(type) ??
		//       really need to pick one as standard for the project
		//
		//initData = {};
		//ZeroMemory(&initData, sizeof(initData));

		//
		// value initialization is much faster than zero memory for debug builds but for release builds
		// they are for all intents and purposes identical in speed
		//
		// it is still important to keep debug builds running fast, preferrably as close to release builds
		// as possible so therefore should favor value initialization over zero memory
		//

		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = pEntity->pMesh->pVertices;

		hr = pRenderer->pDevice->CreateBuffer(&bd, &initData, &(pEntity->primitives.pVertexBuffer));
		if (FAILED(hr)) {
			return hr;
		}



		//
		// TODO: do not need input assembly calls i.e. IASetVertexBuffers, IASetIndexBuffer, IASetPrimitiveTopology
		//       for populating buffers, they will get called when rendering an entity
		//

		// set vertex buffer
		//UINT stride = sizeof(XMVertex_t);
		//UINT offset = 0;
		//pRenderer->pImmediateContext->IASetVertexBuffers(0, 1, &(pEntity->primitives.pVertexBuffer), &stride, &offset);



		// create index buffer
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * numVertices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = pEntity->pMesh->pIndices;

		hr = pRenderer->pDevice->CreateBuffer(&bd, &initData, &(pEntity->primitives.pIndexBuffer));
		if (FAILED(hr)) {
			return hr;
		}



		// set index buffer
		//pRenderer->pImmediateContext->IASetIndexBuffer(pEntity->primitives.pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// set primitive topology
		//pRenderer->pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);




		// create default texture
		unsigned char defaultTex[] = {
			255, 255, 255, 255,    128, 128, 128, 255,
			128, 128, 128, 255,    255, 255, 255, 255
		};

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = 2;
		desc.Height = 2;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;   // number of multisamples per pixel
		desc.SampleDesc.Quality = 0; // multisample quality levels
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		// using D3D11_SUBRESOURCE_DATA to init the texture with in the create call
		D3D11_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = defaultTex;
		initialData.SysMemPitch = 8; // distance in bytes from beginning of one line of texture to the next
		initialData.SysMemSlicePitch = 0; // distance in bytes from the beginning of one depth level to the next

		hr = pRenderer->pDevice->CreateTexture2D(&desc, &initialData, &(pEntity->pTexture));
		DXF_CHECK_HRESULT(hr);

		return S_OK;
	}

	void DestroyGridXZ(_Inout_ EntityDX_t *pEntity) {

		if (pEntity->pTexture) { pEntity->pTexture->Release(); pEntity->pTexture = NULL; }

		if (pEntity->pMesh->pIndices) { free(pEntity->pMesh->pIndices); pEntity->pMesh->pIndices = NULL; }
		if (pEntity->pMesh->pVertices) { free(pEntity->pMesh->pVertices); pEntity->pMesh->pVertices = NULL; }

		if (pEntity->pMesh) { free(pEntity->pMesh); pEntity->pMesh = NULL; }
	}

	HRESULT LoadEntity(CONST_PTR_VAL(RendererDX_t*) pRenderer, LPCTSTR fileName, CONST_PTR(EntityDX_t*) pEntity) {
	//HRESULT LoadEntity(const RendererDX_t* const pRenderer, LPCTSTR fileName, EntityDX_t* const pEntity) {
		HRESULT hr = S_OK;
		pEntity->pMeshSDK = new CDXUTSDKMesh();
		hr = pEntity->pMeshSDK->Create(pRenderer->pDevice, fileName);

		//
		// TODO: calculate the center point and use it to set model matrix
		//
		XMVECTOR vCenter = XMVectorSet(0.25767413f, -28.503521f, 111.00689f, 0.0f);
		pEntity->model = XMMatrixTranslationFromVector(vCenter);

		return hr;
	}

	//
	// TODO: update DestroyEntity call to clean up all entity resource
	//
	void DestroyEntity(EntityDX_t *pEntity) {
		if (pEntity->pMeshSDK) { delete(pEntity->pMeshSDK); }
	}

	MeshBounds_t CalcBoundingBox(const EntityDX_t* const pEntity) {
		MeshBounds_t bounds;
		for (UINT iMesh = 0; iMesh < pEntity->pMeshSDK->GetNumMeshes(); ++iMesh) {
			for (UINT iVB = 0; iVB < pEntity->pMeshSDK->GetNumVBs(); ++iVB) {

				BYTE *pVertices = pEntity->pMeshSDK->GetRawVerticesAt(iVB);
				UINT64 nVertices = pEntity->pMeshSDK->GetNumVertices(iMesh, iVB);
				UINT64 vertexStride = pEntity->pMeshSDK->GetVertexStride(iMesh, iVB);

				float *pVertex = (float *)pVertices;
				bounds.max_x = bounds.min_x = *pVertex; ++pVertex;
				bounds.max_y = bounds.min_y = *pVertex; ++pVertex;
				bounds.max_z = bounds.min_z = *pVertex; ++pVertex;

				for (UINT64 i = 0; i < nVertices; ++i) {
					pVertex = (float *)pVertices;

					if (*pVertex < bounds.min_x) { bounds.min_x = *pVertex; }
					if (*pVertex > bounds.max_x) { bounds.max_x = *pVertex; }
					++pVertex;

					if (*pVertex < bounds.min_y) { bounds.min_y = *pVertex; }
					if (*pVertex > bounds.max_y) { bounds.max_y = *pVertex; }
					++pVertex;

					if (*pVertex < bounds.min_z) { bounds.min_z = *pVertex; }
					if (*pVertex > bounds.max_z) { bounds.max_z = *pVertex; }

					pVertices += vertexStride;
				}

				//
				// TODO: get bounds and center point for each mesh and vertex buffer
				//

				// upper: {x=188.352112 y=55.1263733 z=254.667694 }
				// lower: {x=-188.293976 y=-127.013367 z=-258.593750 }
				// half: {x=188.323044 y=91.0698700 z=256.630737 }
				// bounding box center {x=0.0290679932 y=-35.9434967 z=-1.96301270 }

				//XMVECTOR vCenter = sdkmesh.pMeshSDK->GetMeshBBoxCenter(iMesh);
				//XMVECTOR vExtents = sdkmesh.pMeshSDK->GetMeshBBoxExtents(iMesh);

				//
				// TODO: reverse engineer why this is done (is this the new center point after the rotations are applied ??)
				//
				//XMVECTOR vCenter = XMVectorSet(0.25767413f, -28.503521f, 111.00689f, 0.0f);
				//pEntity->model = XMMatrixTranslationFromVector(vCenter);
			}
		}

		//
		// TODO: make a utility function to calc center point
		//

		float avg_x = (bounds.max_x + bounds.min_x) / 2.0f;
		float avg_y = (bounds.max_y + bounds.min_y) / 2.0f;
		float avg_z = (bounds.max_z + bounds.min_z) / 2.0f;

		float abs_x, abs_y, abs_z;
		abs_x = (fabsf(bounds.max_x) > fabsf(bounds.min_x)) ? fabsf(bounds.max_x) : fabsf(bounds.min_x);
		abs_y = (fabsf(bounds.max_y) > fabsf(bounds.min_y)) ? fabsf(bounds.max_y) : fabsf(bounds.min_y);
		abs_z = (fabsf(bounds.max_z) > fabsf(bounds.min_z)) ? fabsf(bounds.max_z) : fabsf(bounds.min_z);

		// need to double abs_x, etc. otherwise will scale -1 -> 1 instead of -0.5 -> 0.5
		abs_x *= 2.0f;
		abs_y *= 2.0f;
		abs_z *= 2.0f;

		float scaleFactor;
		scaleFactor = abs_x > abs_y ? abs_x : abs_y;
		scaleFactor = scaleFactor > abs_z ? scaleFactor : abs_z;

		//
		// TODO: does DirectX follow the convetion of w = 1 for vectors 0 for points ??
		//
		XMVECTOR unitScalar = { 1.0f / scaleFactor, 1.0f / scaleFactor, 1.0f / scaleFactor, 1.0f };

		return bounds;
	}

	HRESULT InitPrimitives(_In_ MeshDX_t *pMesh, _In_ RendererDX_t *pRenderer, _Out_ PrimitivesDX_t *pPrimitives) {
		HRESULT hr;
		D3D11_BUFFER_DESC bd;
		D3D11_SUBRESOURCE_DATA initData;

		// create vertex buffer
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(XMVertex_t) * pMesh->numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = pMesh->pVertices;
		hr = pRenderer->pDevice->CreateBuffer(&bd, &initData, &(pPrimitives->pVertexBuffer));
		if (FAILED(hr)) {
			return hr;
		}

		// set vertex buffer
		UINT stride = sizeof(XMVertex_t);
		UINT offset = 0;
		pRenderer->pImmediateContext->IASetVertexBuffers(0, 1, &(pPrimitives->pVertexBuffer), &stride, &offset);

		// create index buffer
		ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * pMesh->numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = pMesh->pIndices;
		hr = pRenderer->pDevice->CreateBuffer(&bd, &initData, &(pPrimitives->pIndexBuffer));
		if (FAILED(hr)) {
			return hr;
		}

		// set index buffer
		pRenderer->pImmediateContext->IASetIndexBuffer(pPrimitives->pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// set primitive topology
		pRenderer->pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return S_OK;
	}

	void DestroyPrimitives(_Inout_ PrimitivesDX_t *pPrimitives) {
		if (pPrimitives->pIndexBuffer) { pPrimitives->pIndexBuffer->Release(); }
		if (pPrimitives->pVertexBuffer) { pPrimitives->pVertexBuffer->Release(); }
	}

	// NOTE: normally init transform would check some saved data and init to the last saved
	//       values or if they don't exist use some defaults or ini/CLI values
	void InitViewVolume(_In_ RendererDX_t *pRenderer, _Out_ ViewVolume_t *pViewVolume) {
		// initialize world matrix
		//pviewVolume->world = XMMatrixIdentity();

		//
		// TODO: need to start putting together camera controls
		//

		XMFLOAT4 cameraPos = XMFLOAT4(0.0f, 175.0f, -750.0f, 0.0f);
		//pviewVolume->cameraPos = cameraPos;

		// initialize view matrix
		XMVECTOR eye = XMLoadFloat4(&cameraPos);
		XMVECTOR at = XMVectorSet(0.0f, 100.0f, 0.0f, 0.0f);
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		pViewVolume->view = XMMatrixLookAtLH(eye, at, up);

		// initialize projection matrix
		pViewVolume->projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, pRenderer->vp.Width / pRenderer->vp.Height, 2.0f, 2048.0f);
	}
};
