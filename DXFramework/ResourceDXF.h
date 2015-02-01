//
// File:     ResourceDXF.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

#pragma once

#include "DXFCommon.h"
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
	// TODO: move view volume and associated functions out into a separate module
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


	//
	// TODO: make all functions as const correct as possible and decide on wether to pass by
	//       pointer or pass by reference (benchmark but there should be no real difference)
	//

	//
	// TODO: to use or not to use custom const macros ??
	//
	// int* const pInt; // cannot modify pointer but can modify pointed at values
	// const int* pInt; // cannot modify pointed at values but can modify pointer itself

	
	HRESULT LoadEntity(CONST_PTR_VAL(RendererDX_t*) pRenderer, LPCTSTR fileName, CONST_PTR(EntityDX_t*) pEntity);
	//HRESULT LoadEntity(const RendererDX_t* const pRenderer, LPCTSTR fileName, EntityDX_t* const pEntity);

	//
	// TODO: should be using __RESTRICT__ with functions that take two or more pointers params
	//


	void DestroyEntity(EntityDX_t* pEntity);


	MeshBounds_t CalcBoundingBox(const EntityDX_t* const pEntity);
	//
	// TODO: function to scale entity to fit within a unit cube
	//


	//
	// TODO: utility functions for creating default vertex/index buffers, need shorter names
	//
	//void CreateDefaultVertexBuffer(ID3D11Buffer**, numVertices, stride, offset, pData);
	//void CreateDefaultIndexBuffer(ID3D11Buffer**, numIndices, format, pData);


	HRESULT InitPrimitives(MeshDX_t* pMesh, RendererDX_t* pRenderer, PrimitivesDX_t* pPrimitives);
	void DestroyPrimitives(PrimitivesDX_t* pPrimitives);


	//
	// TODO: also create a color coded guide line for x, y, z axis in a GenerateAxisGuide function
	//
	HRESULT GenerateGridXZ(RendererDX_t* pRenderer, const int32_t extent, EntityDX_t* pEntity);

	//
	// TODO: should have a generic destroy entity function
	//
	void DestroyGridXZ(EntityDX_t* pEntity);


	//
	// TODO: function for drawing a primitives skeletal animation rig or frame data
	//

	void InitViewVolume(RendererDX_t* pRenderer, ViewVolume_t* pViewVolume);
};
