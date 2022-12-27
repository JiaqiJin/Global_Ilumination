#include "common.hlsl"

struct VertexIn
{
    float3 PosL  : POSITION;
    float3 Normal : NORMAL;
    float2 Texc : TEXCOORD;
};

struct VertexOut
{
    float4 PosH  : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float3 PosW    : POSITION1;
    float3 Normal : NORMAL;
    float2 Texc : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.PosH = float4((vin.PosL) * 2.0f + float3(-1.0f, 1.0f, 0.0f), 1.0f);

    vout.Texc = vin.Texc;
    vout.Normal = vin.Normal;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 PosW = gPositionMap.Sample(gsamLinearWrap, pin.Texc);
    float4 Alb = gAlbedoMap.Sample(gsamLinearWrap, pin.Texc);
    float4 Nor = gNormalMap.Sample(gsamLinearWrap, pin.Texc);

    float3 lightDir = gLightPosW;
    lightDir = normalize(lightDir);
    float lamb = dot(lightDir, Nor);

    float4 col = float4(gAlbedoMap.Sample(gsamLinearWrap, pin.Texc).rgb, 1.0f);
    col = col * lamb;

    return col;
}