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

GS_INPUT VS(VertexIn vin)
{
    GS_INPUT gin;

    gin.PosL = mul(vin.PosL.xyz, gWorld);

    // Just pass vertex color into the pixel shader.
    gin.TexC = vin.TexC;
    gin.Normal = normalize(mul(vin.Normal, (float3x3)gWorld));

    return gin;
}

[maxvertexcount(3)]
void GS()
{
    PS_INPUT output;
}

void PS(PS_INPUT pin)
{
    pin.Normal = normalize(pin.Normal);
    float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);

    int3 texDimensions;
    gVoxelizer.GetDimensions(texDimensions.x, texDimensions.y, texDimensions.z);

    uint3 texIndex = uint3(
        ((pin.PosW.x * 0.5) + 0.5f) * texDimensions.x,
        ((pin.PosW.y * 0.5) + 0.5f) * texDimensions.y,
        ((pin.PosW.z * 0.5) + 0.5f) * texDimensions.z);

    if (all(texIndex < texDimensions.xyz) && all(texIndex >= 0))
    {
        gVoxelizer[texIndex] = diffuseAlbedo;
    }
}