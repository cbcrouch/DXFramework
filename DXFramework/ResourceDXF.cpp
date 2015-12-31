//
// File:     ResourceDXF.cpp
// Project:  DXFramework
//
// Copyright (c) 2016 Casey Crouch. All rights reserved.
//

#include "ResourceDXF.h"

namespace DXF {

    HRESULT GenerateGridXZ(const RendererDX_t& renderer, const int32_t extent, EntityDX_t* const pEntity) {
        pEntity->pMesh = (MeshDX_t *)malloc(sizeof(MeshDX_t));
        assert(pEntity->pMesh != nullptr);

        int numVertices = (extent * 8) + 4;
        pEntity->pMesh->numVertices = numVertices;

        pEntity->pMesh->pVertices = (XMVertex_t *)malloc(numVertices * sizeof(XMVertex_t));
        pEntity->pMesh->pIndices = (WORD *)malloc(numVertices * sizeof(WORD));
        assert(pEntity->pMesh->pVertices != nullptr);
        assert(pEntity->pMesh->pIndices != nullptr);

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



        D3D11_BUFFER_DESC bd = {};
        D3D11_SUBRESOURCE_DATA initData = {};

        // create vertex buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(XMVertex_t) * numVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        initData = {};
        initData.pSysMem = pEntity->pMesh->pVertices;
        HRESULT hr = renderer.pDevice->CreateBuffer(&bd, &initData, &(pEntity->primitives.pVertexBuffer));
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
        bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * numVertices;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        initData = {};
        initData.pSysMem = pEntity->pMesh->pIndices;

        hr = renderer.pDevice->CreateBuffer(&bd, &initData, &(pEntity->primitives.pIndexBuffer));
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

        D3D11_TEXTURE2D_DESC desc = {};
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
        D3D11_SUBRESOURCE_DATA initialData = {};
        initialData.pSysMem = defaultTex;
        initialData.SysMemPitch = 8; // distance in bytes from beginning of one line of texture to the next
        initialData.SysMemSlicePitch = 0; // distance in bytes from the beginning of one depth level to the next

        hr = renderer.pDevice->CreateTexture2D(&desc, &initialData, &(pEntity->pTexture));
        DXF_CHECK_HRESULT(hr);

        return S_OK;
    }

    HRESULT GenerateGridXZ(EntityDX_t& entity, const int32_t extent, const RendererDX_t& renderer) {
        entity.pMesh = (MeshDX_t *)malloc(sizeof(MeshDX_t));
        assert(entity.pMesh != nullptr);

        int numVertices = (extent * 8) + 4;
        entity.pMesh->numVertices = numVertices;

        entity.pMesh->pVertices = (XMVertex_t *)malloc(numVertices * sizeof(XMVertex_t));
        entity.pMesh->pIndices = (WORD *)malloc(numVertices * sizeof(WORD));
        assert(entity.pMesh->pVertices != nullptr);
        assert(entity.pMesh->pIndices != nullptr);

        // set vertex data
        int index = 0;
        for (int i = -extent; i <= extent; ++i) {
            entity.pMesh->pVertices[index].pos = XMFLOAT3((float)extent, 0.0f, (float)i);
            entity.pMesh->pVertices[index + 1].pos = XMFLOAT3((float)-extent, 0.0f, (float)i);
            index += 2;
        }
        for (int i = -extent; i <= extent; ++i) {
            entity.pMesh->pVertices[index].pos = XMFLOAT3((float)i, 0.0f, (float)extent);
            entity.pMesh->pVertices[index + 1].pos = XMFLOAT3((float)i, 0.0f, (float)-extent);
            index += 2;
        }

        // set index data
        for (int i = 0; i < numVertices; ++i) {
            entity.pMesh->pIndices[i] = i;
        }



        D3D11_BUFFER_DESC bd = {};
        D3D11_SUBRESOURCE_DATA initData = {};

        // create vertex buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(XMVertex_t) * numVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        initData = {};
        initData.pSysMem = entity.pMesh->pVertices;
        HRESULT hr = renderer.pDevice->CreateBuffer(&bd, &initData, &(entity.primitives.pVertexBuffer));
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
        bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * numVertices;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        initData = {};
        initData.pSysMem = entity.pMesh->pIndices;

        hr = renderer.pDevice->CreateBuffer(&bd, &initData, &(entity.primitives.pIndexBuffer));
        if (FAILED(hr)) {
            return hr;
        }



        // set index buffer
        //pRenderer->pImmediateContext->IASetIndexBuffer(pEntity->primitives.pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // set primitive topology
        //pRenderer->pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);




        // create default texture
        unsigned char defaultTex[] = {
            255, 255, 255, 255, 128, 128, 128, 255,
            128, 128, 128, 255, 255, 255, 255, 255
        };

        D3D11_TEXTURE2D_DESC desc = {};
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
        D3D11_SUBRESOURCE_DATA initialData = {};
        initialData.pSysMem = defaultTex;
        initialData.SysMemPitch = 8; // distance in bytes from beginning of one line of texture to the next
        initialData.SysMemSlicePitch = 0; // distance in bytes from the beginning of one depth level to the next

        hr = renderer.pDevice->CreateTexture2D(&desc, &initialData, &(entity.pTexture));
        DXF_CHECK_HRESULT(hr);

        return S_OK;
    }

    void DestroyGridXZ(EntityDX_t& entity) {

        if (entity.pTexture) { entity.pTexture->Release(); entity.pTexture = nullptr; }

        if (entity.pMesh->pIndices) { free(entity.pMesh->pIndices); entity.pMesh->pIndices = nullptr; }
        if (entity.pMesh->pVertices) { free(entity.pMesh->pVertices); entity.pMesh->pVertices = nullptr; }

        if (entity.pMesh) { free(entity.pMesh); entity.pMesh = nullptr; }
    }



    //
    // TODO: replace with a reference to the entity ???
    //
/*
    HRESULT LoadSDKMesh(const RendererDX_t& renderer, LPCTSTR fileName, CONST_PTR(EntityDX_t*) pEntity) {
        HRESULT hr = S_OK;
        pEntity->pMeshSDK = new CDXUTSDKMesh();

        hr = pEntity->pMeshSDK->Create(renderer.pDevice, fileName);

        //
        // TODO: calculate the center point and use it to set model matrix
        //
        XMVECTOR vCenter = XMVectorSet(0.25767413f, -28.503521f, 111.00689f, 0.0f);
        pEntity->model = XMMatrixTranslationFromVector(vCenter);

        return hr;
    }
*/

    HRESULT LoadSDKMesh(EntityDX_t& entity, LPCTSTR fileName, const RendererDX_t& renderer) {
        HRESULT hr = S_OK;
        entity.pMeshSDK = new CDXUTSDKMesh();

        hr = entity.pMeshSDK->Create(renderer.pDevice, fileName);

        //
        // TODO: calculate the center point and use it to set model matrix
        //
        XMVECTOR vCenter = XMVectorSet(0.25767413f, -28.503521f, 111.00689f, 0.0f);
        entity.model = XMMatrixTranslationFromVector(vCenter);

        return hr;
    }




    //void DestroySDKMesh(EntityDX_t* pEntity) {
    //    if (pEntity->pMeshSDK) { delete(pEntity->pMeshSDK); }
    //}

    void DestroySDKMesh(EntityDX_t& entity) {
        if (entity.pMeshSDK) { delete(entity.pMeshSDK); }
    }


    MeshBounds_t CalcBoundingBox(const EntityDX_t& entity) {
        MeshBounds_t bounds = {};
        for (UINT iMesh = 0; iMesh < entity.pMeshSDK->GetNumMeshes(); ++iMesh) {
            for (UINT iVB = 0; iVB < entity.pMeshSDK->GetNumVBs(); ++iVB) {

                BYTE *pVertices = entity.pMeshSDK->GetRawVerticesAt(iVB);
                UINT64 nVertices = entity.pMeshSDK->GetNumVertices(iMesh, iVB);
                UINT64 vertexStride = entity.pMeshSDK->GetVertexStride(iMesh, iVB);

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

    HRESULT InitPrimitives(PrimitivesDX_t& primitives, const MeshDX_t& mesh, const RendererDX_t& renderer) {
        HRESULT hr;
        D3D11_BUFFER_DESC bd = {};
        D3D11_SUBRESOURCE_DATA initData = {};

        // create vertex buffer
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(XMVertex_t) * mesh.numVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        initData.pSysMem = mesh.pVertices;
        hr = renderer.pDevice->CreateBuffer(&bd, &initData, &(primitives.pVertexBuffer));
        if (FAILED(hr)) {
            return hr;
        }

        // set vertex buffer
        UINT stride = sizeof(XMVertex_t);
        UINT offset = 0;
        renderer.pImmediateContext->IASetVertexBuffers(0, 1, &(primitives.pVertexBuffer), &stride, &offset);

        // create index buffer
        bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WORD) * mesh.numIndices;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        initData = {};
        initData.pSysMem = mesh.pIndices;
        hr = renderer.pDevice->CreateBuffer(&bd, &initData, &(primitives.pIndexBuffer));
        if (FAILED(hr)) {
            return hr;
        }

        // set index buffer
        renderer.pImmediateContext->IASetIndexBuffer(primitives.pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // set primitive topology
        renderer.pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        return S_OK;
    }

    void DestroyPrimitives(PrimitivesDX_t& primitives) {
        if (primitives.pIndexBuffer) { primitives.pIndexBuffer->Release(); }
        if (primitives.pVertexBuffer) { primitives.pVertexBuffer->Release(); }
    }


    //
    // NOTE: normally init transform would check some saved data and init to the last saved
    //       values or if they don't exist use some defaults or CLI values
    //
    void InitViewVolume(ViewVolume_t& viewVolume, const RendererDX_t& renderer) {
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
        viewVolume.view = XMMatrixLookAtLH(eye, at, up);

        // initialize projection matrix
        viewVolume.projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, renderer.vp.Width / renderer.vp.Height, 2.0f, 2048.0f);
    }
};
