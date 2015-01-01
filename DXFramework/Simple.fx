//
// File:     Simple.fx
// Project:  DXFramework
//
// Copyright (c) 2015 Casey Crouch. All rights reserved.
//

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

//
// TODO: view will change almost every frame once camera controls implemented
//
cbuffer cbNeverChanges : register(b0) {
	matrix View;
};

cbuffer cbChangeOnResize : register(b1) {
	matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b2) {
	matrix World;

	//
	// TODO: is it not possible to pass an array of arbitrary length to an HLSL shader, the
	//       best solution is most likely to store the light direction vector and light color
	//       in a bound buffer and pass in the number of lights in the constant buffer
	//
};

//--------------------------------------------------------------------------------------

struct VS_INPUT {
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct PS_INPUT {
	float4 Pos : SV_POSITION;
	float4 WorldPos : TEXCOORD2; // TODO: why storing as a texcoord ?? HLSL does not support multiple SV_POSITION ??
	float3 Norm : TEXCOORD1;
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input) {
	PS_INPUT output = (PS_INPUT)0;

	// transform vertex to world space, then view space, and finally projection (frustum) space
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// transform normal and position into world space
	output.Norm = mul(float4(input.Norm, 1), World).xyz;
	output.WorldPos = mul(input.Pos, World);

	// pass through texture
	output.Tex = input.Tex;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS(PS_INPUT input) : SV_TARGET {
	return txDiffuse.Sample(samLinear, input.Tex);
}
