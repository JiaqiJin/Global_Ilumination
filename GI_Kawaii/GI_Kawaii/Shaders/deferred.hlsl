#include "common.hlsl"

struct VertexIn
{
    float3 PosL  : POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};
struct VertexOut
{
    float4 PosW : POSITION;
    float4 PosH  : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut)0.0f;

    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
    vout.Normal = normalize(mul(vin.Normal, (float3x3)gWorld));
    vout.TexC = vin.TexC;
    vout.PosW = posW;

    return vout;
}

struct PS_OUT 
{
    float4 POS: SV_Target0;
    float4 ALB: SV_Target1;
    float4 NOR: SV_Target2;
    float4 DEP: SV_Target3;
};


PS_OUT PS(VertexOut pin)
{
    PS_OUT output;

    float4 diffuseAlbedo = 1.0f;

    // Dynamically look up the texture in the array.
    diffuseAlbedo *= gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC);
    output.ALB = diffuseAlbedo;
    output.POS = pin.PosW;
    output.NOR = float4(pin.Normal, 1.0f);
    float depCol = pin.PosH.z;
    output.DEP = float4(depCol, depCol, depCol, 1.0);
    return output;
}


