#include "common.hlsl"

struct VertexIn
{
    float3 PosL  : POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};

struct GS_INPUT
{
    float3 PosL  : POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};

struct PS_INPUT
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};

// https://github.com/LeifNode/Novus-Engine 
void imageAtomicRGBA8Avg(RWTexture3D<uint> imgUI, uint3 coords, float4 val)
{
	val.rgb *= 255.0f; // Optimize following calculations
	uint newVal = convVec4ToRGBA8(val);
	uint prevStoredVal = 0;
	uint curStoredVal = 0;

	// Loop as long as destination value gets changed by other threads

	[allow_uav_condition] do //While loop does not work and crashes the graphics driver, but do while loop that does the same works; compiler error?
	{
		InterlockedCompareExchange(imgUI[coords], prevStoredVal, newVal, curStoredVal);

		if (curStoredVal == prevStoredVal)
			break;

		prevStoredVal = curStoredVal;
		float4 rval = convRGBA8ToVec4(curStoredVal);
		rval.xyz = (rval.xyz * rval.w); // Denormalize
		float4 curValF = rval + val; // Add new value
		curValF.xyz /= (curValF.w); // Renormalize
		newVal = convVec4ToRGBA8(curValF);


	} while (true);
}

GS_INPUT VS(VertexIn vin)
{
    GS_INPUT gin;

	gin.PosL = mul(float4(vin.PosL.xyz, 1.0), gWorld).xyz;

    // Just pass vertex color into the pixel shader.
    gin.TexC = vin.TexC;
    gin.Normal = normalize(mul(vin.Normal, (float3x3)gWorld));

    return gin;
}

[maxvertexcount(3)]
void GS(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> triStream)
{
	PS_INPUT output;
	float4 outputPosH[3] = { float4(0.0f, 0.0f, 0.0f, 0.0f),
							 float4(0.0f, 0.0f, 0.0f, 0.0f),
							 float4(0.0f, 0.0f, 0.0f, 0.0f) };

	const float2 halfPixel = float2(1.0f, 1.0f) / 512.0f;

	float3 posW0 = input[0].PosL.xyz;
	float3 posW1 = input[1].PosL.xyz;
	float3 posW2 = input[2].PosL.xyz;
	float3 normal = cross(normalize(posW1 - posW0), normalize(posW2 - posW0)); //Get face normal

	float axis[] = {
		abs(dot(normal, float3(1.0f, 0.0f, 0.0f))),
		abs(dot(normal, float3(0.0f, 1.0f, 0.0f))),
		abs(dot(normal, float3(0.0f, 0.0f, 1.0f))),
	};


	//Find dominant axis
	int index = 0;
	int i = 0;

	[unroll]
	for (i = 1; i < 3; i++)
	{
		[flatten]
		if (axis[i] > axis[i - 1])
			index = i;
	}

	[unroll]
	for (i = 0; i < 3; i++)
	{
		float4 inputPosL;

		[flatten]
		switch (index)
		{
		case 0:
			inputPosL = float4(input[i].PosL.yzx, 1.0f);
			break;
		case 1:
			inputPosL = float4(input[i].PosL.xzy, 1.0f);
			break;
		case 2:
			inputPosL = float4(input[i].PosL.xyz, 1.0f);
			break;
		}


		outputPosH[i] = mul(inputPosL, gVoxelViewProj);

		output.TexC = input[i].TexC;
		output.PosW = input[i].PosL / 200.0f; // make sure model in screen space
		output.PosH = outputPosH[i];
		output.Normal = input[i].Normal;

		triStream.Append(output);
	}
}

void PS(PS_INPUT pin)
{
    pin.Normal = normalize(pin.Normal);
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);

	int3 texDimensions;
	gVoxelizerAlbedo.GetDimensions(texDimensions.x, texDimensions.y, texDimensions.z);

	uint3 texIndex = uint3(((pin.PosW.x * 0.5) + 0.5f) * texDimensions.x,
		((pin.PosW.y * 0.5) + 0.5f) * texDimensions.y,
		((pin.PosW.z * 0.5) + 0.5f) * texDimensions.z);

	//diffuseAlbedo.xyz += float3(0.1, 0.1, 0.1);

	if (all(texIndex < texDimensions.xyz) && all(texIndex >= 0))
	{
		//gVoxelizer[texIndex] = diffuseAlbedo;
		gVoxelizerAlbedo[texIndex] = convVec4ToRGBA8(float4(diffuseAlbedo.xyz, 1.0) * 255.0f);
		gVoxelizerNormal[texIndex] = convVec4ToRGBA8(float4((pin.Normal.xyz + float3(0.5, 0.5, 0.5)) / 2.0, 1.0) * 255.0f);
	}
}

[numthreads(8, 8, 8)]
void CompReset(int3 dispatchThreadID : SV_DispatchThreadID)
{
	int x = dispatchThreadID.x;
	int y = dispatchThreadID.y;
	int z = dispatchThreadID.z;
	gVoxelizerAlbedo[int3(x, y, z)] = 0;
	gVoxelizerNormal[int3(x, y, z)] = 0;
	gVoxelizerEmissive[int3(x, y, z)] = 0;
	gVoxelizerRadiance[int3(x, y, z)] = 0;
}