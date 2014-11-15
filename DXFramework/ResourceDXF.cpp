//
// File:     ResourceDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2014 Casey Crouch. All rights reserved.
//

#include "ResourceDXF.h"

namespace DXF {
	HRESULT GenerateGridXY(_Out_ EntityDX_t *pEntity, _In_ RendererDX_t *pRenderer, int extent) {

		//
		// TODO: to get up and running pass in an EntityDX_t and a ProgramDX_t to get something basic working, then clean it up
		//

		//
		// TODO: consider using Win32 HeapAlloc for the sake of learning something new
		//

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
		for (int i = -extent; i <= extent; i++) {
			pEntity->pMesh->pVertices[index].pos   = XMFLOAT3((float)extent, 0.0f, (float)i);
			pEntity->pMesh->pVertices[index+1].pos = XMFLOAT3((float)-extent, 0.0f, (float)i);
			index += 2;
		}
		for (int i = -extent; i <= extent; i++) {
			pEntity->pMesh->pVertices[index].pos = XMFLOAT3((float)i, 0.0f, (float)extent);
			pEntity->pMesh->pVertices[index + 1].pos = XMFLOAT3((float)i, 0.0f, (float)-extent);
			index += 2;
		}

		// set index data
		for (int i = 0; i < numVertices; i++) {
			pEntity->pMesh->pIndices[i] = i;
		}

		//
		// TODO: would be worthwhile to experiment with BC7 texture compression and usage since it seems commonly used
		//

		unsigned char defaultTex[] = {
			255, 255, 255, 255,    128, 128, 128, 255,
			128, 128, 128, 255,    255, 255, 255, 255
		};


		// NOTE: this example creates a texture, with dynamic usage, for use as a shader resource with cpu write access
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = 2;
		desc.Height = 2;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0; // check if this is correct
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		desc.CPUAccessFlags = 0;

		desc.MiscFlags = 0;


		// another option would be to create D3D11_SUBRESOURCE_DATA to init the texture with in the create call
		D3D11_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = defaultTex;
		initialData.SysMemPitch = 8; // distance in bytes from beginning of one line of texture to the next
		initialData.SysMemSlicePitch = 0; // distance in bytes from the beginning of one depth level to the next



		ID3D11Texture2D *pTexture = NULL;

		//HRESULT hr = pRenderer->pDevice->CreateTexture2D(&desc, &initialData, &pTexture);

		HRESULT hr = pRenderer->pDevice->CreateTexture2D(&desc, NULL, &pTexture);

		CHECK_HRESULT(hr);


		D3D11_BOX dstRegion;

		// is top left the origin ??

		// dimensions are in texels for textures and bytes for buffers

		// are coordinates zero based ?? will top 0 bottom 1 update two rows ??
		dstRegion.top = 0;
		dstRegion.bottom = 1;
		dstRegion.left = 0;
		dstRegion.right = 1;

		// http://msdn.microsoft.com/en-us/library/windows/desktop/ff476486(v=vs.85).aspx


		//pRenderer->pImmediateContext->UpdateSubresource(pTexture, 0, &dstRegion, defaultTex, 2, 2);

		pRenderer->pImmediateContext->UpdateSubresource(pTexture, 0, NULL, defaultTex, 2, 2);


		//
		// TODO: store texture pointer in the entity in order to properly clean it up
		//
		if (pTexture) { pTexture->Release(); pTexture = NULL; }


		return S_OK;
	}

	void DestroyGridXY(_Inout_ EntityDX_t *pEntity) {

		//
		// TODO: destroy texture once it has been added into the entity
		//

		if (pEntity->pMesh->pIndices) { free(pEntity->pMesh->pIndices); pEntity->pMesh->pIndices = NULL; }
		if (pEntity->pMesh->pVertices) { free(pEntity->pMesh->pVertices); pEntity->pMesh->pVertices = NULL; }

		if (pEntity->pMesh) { free(pEntity->pMesh); pEntity->pMesh = NULL; }
	}


	HRESULT LoadEntity(_Out_ EntityDX_t *pEntity, _In_ RendererDX_t *pRenderer, _In_z_ LPCTSTR fileName) {
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

	void DestroyEntity(_Inout_ EntityDX_t *pEntity) {
		if (pEntity->pMeshSDK) { delete(pEntity->pMeshSDK); }
	}

	void CalcBoundingBox(_Inout_ EntityDX_t *pEntity, _In_ RendererDX_t *pRenderer) {
		float max_x, min_x;
		float max_y, min_y;
		float max_z, min_z;

		for (UINT iMesh = 0; iMesh < pEntity->pMeshSDK->GetNumMeshes(); iMesh++) {
			for (UINT iVB = 0; iVB < pEntity->pMeshSDK->GetNumVBs(); iVB++) {

				BYTE *pVertices = pEntity->pMeshSDK->GetRawVerticesAt(iVB);
				UINT64 nVertices = pEntity->pMeshSDK->GetNumVertices(iMesh, iVB);
				UINT64 vertexStride = pEntity->pMeshSDK->GetVertexStride(iMesh, iVB);

				float *pVertex = (float *)pVertices;
				max_x = min_x = *pVertex; pVertex++;
				max_y = min_y = *pVertex; pVertex++;
				max_z = min_z = *pVertex; pVertex++;

				for (UINT64 i = 0; i < nVertices; i++) {
					pVertex = (float *)pVertices;

					if (*pVertex < min_x) { min_x = *pVertex; }
					if (*pVertex > max_x) { max_x = *pVertex; }
					pVertex++;

					if (*pVertex < min_y) { min_y = *pVertex; }
					if (*pVertex > max_y) { max_y = *pVertex; }
					pVertex++;

					if (*pVertex < min_z) { min_z = *pVertex; }
					if (*pVertex > max_z) { max_z = *pVertex; }

					pVertices += vertexStride;
				}

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

				//
				// TODO: translate bounding box center to the origin by taking the inverse of each component
				//

				float avg_x = (max_x + min_x) / 2.0f;
				float avg_y = (max_y + min_y) / 2.0f;
				float avg_z = (max_z + min_z) / 2.0f;

				float abs_x, abs_y, abs_z;
				abs_x = (fabsf(max_x) > fabsf(min_x)) ? fabsf(max_x) : fabsf(min_x);
				abs_y = (fabsf(max_y) > fabsf(min_y)) ? fabsf(max_y) : fabsf(min_y);
				abs_z = (fabsf(max_z) > fabsf(min_z)) ? fabsf(max_z) : fabsf(min_z);

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
			}
		}
	}

	HRESULT InitPrimitives(_Out_ PrimitivesDX_t *pPrimitives, _In_ MeshDX_t *pMesh, _In_ RendererDX_t *pRenderer) {
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
	void InitViewVolume(_In_ ViewVolume_t *pViewVolume, _In_ RendererDX_t *pRenderer) {
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
