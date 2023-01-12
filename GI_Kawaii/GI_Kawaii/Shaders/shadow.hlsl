#include "common.hlsl"

struct VertexIn
{
    float3 PosL  : POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};

void PS(VertexOut pin)
{
 
}

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut)0.0f;

    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
    vout.Normal = vin.Normal;
    vout.TexC = vin.TexC;

    return vout;
}