//
// File:     DXFCommon.h
// Project:  DXFramework
//
// Copyright (c) 2017 Casey Crouch. All rights reserved.
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


//
// TODO: using preprocessor dynamically include d3d headers if possible and determine
//       which header to include (i.e. if on a DX10 platform is it still okay to use
//       the the DX11_x header ??)
//
//#include <d3d11.h>
//#include <d3d11_1.h>
#include <d3d11_2.h>

// NOTE: DX math should usually be avoided in favor of a more general purpose math lib
#include <directxmath.h>
using namespace DirectX;

#include <cstdint>

using std::int16_t;
using std::int32_t;
using std::int64_t;

using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

using byte_t = uint8_t;
using signed_byte_t = int8_t;

using float32_t = float;
using float64_t = double;
