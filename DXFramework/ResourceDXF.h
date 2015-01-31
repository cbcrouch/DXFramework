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

	struct EntityDX_t {

		//
		// TODO: mesh shouldn't be a pointer
		//
		MeshDX_t*        pMesh;

		MaterialDX_t     material;
		PrimitivesDX_t   primitives;

		ID3D11Texture2D* pTexture;


		//
		// TODO: the transformation heirarchy should be independent of the entity but will need
		//       some mechanism or adapter for interfacing with the entity
		//
		// TransformHierarchy_t ...

		//
		// TODO: build an adapater/load-function of some kind for loading a file format into an entity
		//
		CDXUTSDKMesh *pMeshSDK;

		//
		// TODO: model matrix should be a part of the transform hierarchy
		//
		XMMATRIX model;


		//
		// TODO: bounding box / shape(forces applied to bounding
		//       box/shape could influence Mesh through tessellation control points ??)
		//

	};


	//
	// TODO: remove SAL annotations, replace _In_ with const and use __RESTRICT__ when two or more
	//       pointers are passed to a function
	//

	//
	// TODO: pointer qualifier '*' should have no whitespace between it and the type
	//

	//
	// TODO: to use or not to use custom const macros ??
	//
	// int* const pInt; // cannot modify pointer but can modify pointed at values
	// const int* pInt; // cannot modify pointed at values but can modify pointer itself

	HRESULT LoadEntity(CONST_PTR_VAL(RendererDX_t*) pRenderer, LPCTSTR fileName, CONST_PTR(EntityDX_t*) pEntity);
	//HRESULT LoadEntity(const RendererDX_t* const pRenderer, LPCTSTR fileName, EntityDX_t* const pEntity);

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


	HRESULT InitPrimitives(_In_ MeshDX_t *pMesh, _In_ RendererDX_t *pRenderer, _Out_ PrimitivesDX_t *pPrimitives);
	void DestroyPrimitives(_Inout_ PrimitivesDX_t *pPrimitives);


	//
	// TODO: also create a color coded guide line for x, y, z axis in a GenerateAxisGuide function
	//
	HRESULT GenerateGridXZ(_In_ RendererDX_t *pRenderer, const int32_t extent, _Out_ EntityDX_t *pEntity);

	//
	// TODO: should have a generic destroy entity function
	//
	void DestroyGridXZ(_Inout_ EntityDX_t *pEntity);


	//
	// TODO: function for drawing a primitives skeletal animation rig or frame data
	//
	// animation data that is stored in SDKmesh file format (not 100% of how to visualize)
	//SDKANIMATION_FILE_HEADER* m_pAnimationHeader;
	//SDKANIMATION_FRAME_DATA* m_pAnimationFrameData;
	//DirectX::XMFLOAT4X4* m_pBindPoseFrameMatrices;
	//DirectX::XMFLOAT4X4* m_pTransformedFrameMatrices;
	//DirectX::XMFLOAT4X4* m_pWorldPoseFrameMatrices;


	void InitViewVolume(_In_ RendererDX_t *pRenderer, _Out_ ViewVolume_t *pViewVolume);
};
