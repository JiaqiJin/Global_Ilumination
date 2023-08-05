#include "Common.hlsl"

cbuffer cbRadiance : register(b0)
{
    float3 gLightDir;
    float pad0;
    float3 gLightCol;
    float pad1;
    float4x4 gLight2World;
    float voxelScale;
};

RWTexture3D<uint> gVoxelizerAlbedo : register(u0);
RWTexture3D<uint> gVoxelizerNormal : register(u1);
RWTexture3D<uint> gVoxelizerEmissive : register(u2);
RWTexture3D<uint> gVoxelizerRadiance : register(u3);

Texture2D gShadowMap : register(t0);

[numthreads(16, 16, 1)]
void Radiance(uint3 DTid : SV_DispatchThreadID)
{
    int shadowTexDimensions;
    gShadowMap.GetDimensions(shadowTexDimensions.x, shadowTexDimensions.y);

    int voTexDimensions;
    gVoxelizerAlbedo.GetDimensions(volTexDimensions.x, volTexDimensions.y, volTexDimensions.z);

    if (DTid.x >= (uint)shadowTexDimensions.x || DTid.y >= (uint)shadowTexDimensions.y)
        return;

    float2 uv = DTid.yx / float2(shadowTexDimensions);
    float4 screenSpacePos = float4(uv * 2.0 - 1.0, gShadowMap.Load(int3(DTid.xy, 0)).x, 1.0);
    screenSpacePos.y = -screenSpacePos.y;

    float4 volumeSpacePos = mul(screenSpacePos, gLight2World);
    volumeSpacePos.xyz /= voxelScale;

    uint3 texIndex = uint3(((volumeSpacePos.x * 0.5) + 0.5f) * volTexDimensions.x,
        ((volumeSpacePos.y * 0.5) + 0.5f) * volTexDimensions.y + 1, 
        ((volumeSpacePos.z * 0.5) + 0.5f) * volTexDimensions.z);

    float4 col = float4(convRGBA8ToVec4(gVoxelizerAlbedo[texIndex]).xyz / 255.0, 1.0f);

    gVoxelizerRadiance[int3(texIndex)] = convVec4ToRGBA8(col);
}