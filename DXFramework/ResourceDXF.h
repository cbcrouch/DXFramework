//
// File:     ResourceDXF.h
// Project:  DXFramework
//
// Copyright (c) 2017 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"
#include "UtilsDXF.h"
#include "RendererDXF.h"

//
// TODO: SDKmesh code and format should be encapsulated in the resourceDX module
//
#include "..\DXUT\SDKmesh.h"

//
// TODO: DX primitves allocated in a heap that the render thread can access
//       and the memory in said heap is managed via a handle, that way the
//       simulation aspect of the engine can coorelate a higher level
//       representation of some data with some set of primitives to be drawn
//
namespace DXF {



    //
    // TODO: move view volume and associated functions out into CameraDXF module
    //
    struct ViewVolume_t {
        XMMATRIX view;
        XMMATRIX projection;
    };



    struct XMVertex_t {
        XMFLOAT3 pos;
        XMFLOAT3 normal;
        XMFLOAT2 tex;
    };

    struct MeshBounds_t {
        float max_x, min_x;
        float max_y, min_y;
        float max_z, min_z;
    };

    struct MeshDX_t {
        DXF::XMVertex_t *pVertices;
        WORD *pIndices;

        int numVertices;
        int numIndices;

        // need vertex memory stride and offset

        //
        // TODO: can probably safely get rid of vertex/mesh colors
        //
        XMFLOAT4 meshColor;
    };

    struct PrimitivesDX_t {
        // mesh primitive data buffers
        ID3D11Buffer*           pVertexBuffer;
        ID3D11Buffer*           pIndexBuffer;

        // draw state
        DXGI_FORMAT indexFormat;
        D3D11_PRIMITIVE_TOPOLOGY topology;
    };

    struct DataMapDX_t {
        int width;
        int height;

        DXGI_FORMAT format;

        // format also includes data type e.g. DXGI_FORMAT_R8G8B8A8_UNORM

        size_t size;
        unsigned int rowByteSize;
        unsigned char *pData;

        //
        // TODO: should find a good way to separate/decouple the GPU specific data
        //
        ID3D11Texture2D *pTexture;
    };

    // phong material based on Wavefront obj format
    struct MaterialDX_t {
        float Ns; // specular coefficient
        float Ni; // optical density (also known as index of refraction)

        // dissolve factor
        float d;
        BOOL dHalo;

        float Tr; // transparency
        float Tf[3]; // transmission filter

        //
        // TODO: illumination model should be an enum
        //
        int illumModel;

        float Ka[3]; // ambient color
        float Kd[3]; // diffuse color
        float Ks[3]; // specular color
        float Ke[3]; // emissive color

        DataMapDX_t map_Ka; // ambient texture map
        DataMapDX_t map_Kd; // diffuse texture map
        DataMapDX_t map_Ks; // specular color texture map
        DataMapDX_t map_Ns; // specular highlight component

        //DataMapDX_t map_Tr; // transparency map
        //DataMapDX_t map_bump; // or broken out into: bump, disp, decalT
    };

    //
    // TODO: the following structs/functions are the extent to which the sdkmesh file
    //       format supports animations
    //
/*
    struct SDKANIMATION_FILE_HEADER
    {
        UINT Version;
        BYTE IsBigEndian;
        UINT FrameTransformType;
        UINT NumFrames;
        UINT NumAnimationKeys;
        UINT AnimationFPS;
        UINT64 AnimationDataSize;
        UINT64 AnimationDataOffset;
    };

    struct SDKANIMATION_DATA
    {
        DirectX::XMFLOAT3 Translation;
        DirectX::XMFLOAT4 Orientation;
        DirectX::XMFLOAT3 Scaling;
    };

    struct SDKANIMATION_FRAME_DATA
    {
        char FrameName[MAX_FRAME_NAME];
        union
        {
            UINT64 DataOffset;
            SDKANIMATION_DATA* pAnimationData;
        };
    };

    struct SDKMESH_FRAME
    {
        char Name[MAX_FRAME_NAME];
        UINT Mesh;
        UINT ParentFrame;
        UINT ChildFrame;
        UINT SiblingFrame;
        DirectX::XMFLOAT4X4 Matrix;
        UINT AnimationDataIndex;		//Used to index which set of keyframes transforms this frame
    };
*/

    //frame manipulation
    //void TransformBindPoseFrame(_In_ UINT iFrame, _In_ DirectX::CXMMATRIX parentWorld);
    //void TransformFrame(_In_ UINT iFrame, _In_ DirectX::CXMMATRIX parentWorld, _In_ double fTime);
    //void TransformFrameAbsolute(_In_ UINT iFrame, _In_ double fTime);

    //virtual HRESULT LoadAnimation(_In_z_ const WCHAR* szFileName);

    //Animation
    //UINT              GetNumInfluences(_In_ UINT iMesh) const;
    //DirectX::XMMATRIX GetMeshInfluenceMatrix(_In_ UINT iMesh, _In_ UINT iInfluence) const;
    //UINT              GetAnimationKeyFromTime(_In_ double fTime) const;
    //DirectX::XMMATRIX GetWorldMatrix(_In_ UINT iFrameIndex) const;
    //DirectX::XMMATRIX GetInfluenceMatrix(_In_ UINT iFrameIndex) const;
    //bool              GetAnimationProperties(_Out_ UINT* pNumKeys, _Out_ float* pFrameTime) const;

    struct EntityDX_t {

        //
        // TODO: mesh shouldn't be a pointer
        //
        MeshDX_t*        pMesh;

        MaterialDX_t     material;
        PrimitivesDX_t   primitives;

        ID3D11Texture2D* pTexture;


        //
        // TODO: build an adapater/load-function of some kind for loading a file format into an entity
        //
        CDXUTSDKMesh *pMeshSDK;


        //
        // TODO: the transformation heirarchy should be independent of the entity but will need
        //       some mechanism or adapter for interfacing with the entity
        //
        // TransformHierarchy_t ...

        // model matrix is a placeholder until transform hierarchy is implemented
        XMMATRIX model;

        // animation data that is stored in SDKmesh file format (not 100% of how to visualize)
        //SDKANIMATION_FILE_HEADER* m_pAnimationHeader;
        //SDKANIMATION_FRAME_DATA* m_pAnimationFrameData;

        //DirectX::XMFLOAT4X4* m_pBindPoseFrameMatrices;
        //DirectX::XMFLOAT4X4* m_pTransformedFrameMatrices;
        //DirectX::XMFLOAT4X4* m_pWorldPoseFrameMatrices;


        //
        // TODO: bounding box / shape(forces applied to bounding
        //       box/shape could influence Mesh through tessellation control points ??)
        //

    };


    HRESULT LoadSDKMesh(EntityDX_t& entity, LPCTSTR fileName, const RendererDX_t& renderer);
    void DestroySDKMesh(EntityDX_t& entity);


    MeshBounds_t CalcBoundingBox(const EntityDX_t& entity);

    //
    // TODO: function to scale entity to fit within a unit cube
    //


    //
    // TODO: utility functions for creating default vertex/index buffers
    //
    //void CreateDefaultVertexBuffer(ID3D11Buffer** ppVertexBuffer, const uint32_t numVertices, const uint32_t stride, const uint32_t offset, void* pData);
    //void CreateDefaultIndexBuffer(ID3D11Buffer** ppIndexBuffer, const uint32_t numIndices, const uint32_t format, void* pData);



    HRESULT InitPrimitives(PrimitivesDX_t& primitives, const MeshDX_t& mesh, const RendererDX_t& renderer);
    void DestroyPrimitives(PrimitivesDX_t& primitives);

    //
    // TODO: also create a color coded guide line for x, y, z axis in a GenerateAxisGuide function
    //    

    HRESULT GenerateGridXZ(EntityDX_t& entity, const int32_t extent, const RendererDX_t& renderer);
    void DestroyGridXZ(EntityDX_t& entity);

    //
    // TODO: should have a generic destroy entity function
    //

    //
    // TODO: function for drawing a primitives skeletal animation rig
    //

    void InitViewVolume(ViewVolume_t& viewVolume, const RendererDX_t& renderer);
};
