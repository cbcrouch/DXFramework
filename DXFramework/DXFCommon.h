//
// File:     DXFCommon.h
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//


#pragma once

// eliminate as much extraneous headers and functionality as possible
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

// force unicode support if not already defined
#ifndef _UNICODE
#define _UNICODE
#endif






// base operating system
#include <Windows.h>

// SDK platform, currently including highest available platform
//#include <SDKDDKVer.h>



// TODO: using preprocessor dynamically include d3d headers if possible and determine
//       which header to include (i.e. if on a DX10 platform is it still okay to use
//       the the DX11_x header ??)

//#include <d3d11.h>
//#include <d3d11_1.h>
#include <d3d11_2.h>

// NOTE: DX math should usually be avoided in favor of a more general purpose math lib
#include <directxmath.h>
using namespace DirectX;



//
// TODO: these project generic macros/typedefs/etc. should be in their own "common" header file
//

#define CONST_PTR(P) P const
#define CONST_VAL(P) const P
#define CONST_PTR_VAL(P) const P const


//
// TODO: either use C stdint types or declare own (Windows.h might bring these in automatically)
//       and replace all implicit int/float/etc. types with explicit types int32/real32/etc.
//
//#include <cstdint>
//using std::int32_t;
//etc.

//typedef int int32;
//typedef float real32;
//etc.


#include "UtilsDXF.h"
