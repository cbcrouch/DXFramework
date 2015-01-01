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
	typedef struct ViewVolume_t {
		XMMATRIX view;
		XMMATRIX projection;
	} ViewVolume_t;


	typedef struct XMVertex_t {
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 tex;
	} XMVertex_t;

	typedef struct MeshBounds_t {
		float max_x, min_x;
		float max_y, min_y;
		float max_z, min_z;
	} MeshBounds_t;

	typedef struct MeshDX_t {
		DXF::XMVertex_t *pVertices;
		WORD *pIndices;

		int numVertices;
		int numIndices;

		// need vertex memory stride and offset

		//
		// TODO: can probably safely get rid of vertex/mesh colors
		//
		XMFLOAT4 meshColor;
	} MeshDX_t;

	typedef struct PrimitivesDX_t {
		// mesh primitive data buffers
		ID3D11Buffer*           pVertexBuffer;
		ID3D11Buffer*           pIndexBuffer;

		// draw state
		DXGI_FORMAT indexFormat;
		D3D11_PRIMITIVE_TOPOLOGY topology;
	} PrimitivesDX_t;

	typedef struct DataMapDX_t {
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
	} DataMapDX_t;

	//
	// TODO: this is a Phong based material, should differentiate from say a PBR surface model
	//
	typedef struct MaterialDX_t {
		float Ns; // specular coefficient
		float Ni; // optical density (also known as index of refraction)

		// dissolve factor
		float d;
		BOOL dHalo;

		float Tr; // transparency
		float Tf[3]; // transmission filter

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
	} MaterialDX_t;

	typedef struct EntityDX_t {

		MeshDX_t*        pMesh;
		MaterialDX_t*    pMaterial;
		PrimitivesDX_t*  pPrimitives;


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

	} EntityDX_t;

	//
	// TODO: use correct SAL annotations
	//

	// http://msdn.microsoft.com/en-us/library/hh916382.aspx

	// _In_			data is passed to the called function, and is treated as read-only
	// _Inout_		usable data is passed into the function and potentially modified
	// _In_z_		same as in except input has to be a NULL terminated string

	// _Out_		the caller only provides space for the called function to write to; the called function writes data into that space
	// _Outptr_		like _Out_ the value that's returned by the called function is a pointer i.e. createFunc(_Outptr_ **ppData, ...)
	// _*opt_		parameter is allowed to be NULL


	//
	// TODO: make sure _Outptr_ is used only for creation functions that allocate memory, and use _Out_ for initialization function
	//

	// annotate all pointer parameters
	// annotate value-range annotations so that code analysis can ensure buffer and pointer safety
	// annotate locking rules and locking side effects
	// annotate driver properties and other domain-specific properties


	//
	// TODO: update to common format of listing in parameters first followed by out parameters
	//
	HRESULT LoadEntity(_Out_ EntityDX_t *pEntity, _In_ RendererDX_t *pRenderer, _In_z_ LPCTSTR fileName);
	void DestroyEntity(_Inout_ EntityDX_t *pEntity);

	//
	// TODO: calc bounding box doesn't need renderer pointer passed in
	//
	MeshBounds_t CalcBoundingBox(_Inout_ EntityDX_t *pEntity);

	//
	// TODO: function to scale entity to fit within a unit cube
	//


	HRESULT InitPrimitives(_Out_ PrimitivesDX_t *pPrimitives, _In_ MeshDX_t *pMesh, _In_ RendererDX_t *pRenderer);
	void DestroyPrimitives(_Inout_ PrimitivesDX_t *pPrimitives);

	HRESULT GenerateGridXY(_Out_ EntityDX_t *pEntity, _In_ RendererDX_t *pRenderer, int extent);
	void DestroyGridXY(_Inout_ EntityDX_t *pEntity);

	//
	// TODO: also create a color coded guide line for x, y, z axis in a GenerateAxisGuide function
	//

	void InitViewVolume(_In_ ViewVolume_t *pViewVolume, _In_ RendererDX_t *pRenderer);
};
