
struct VSInput
{
    float4 position : POSITION;
    float2 texcoordUV : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 texcoordUV : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = input.position;
    output.texcoordUV = input.texcoordUV;
    
    return output;
}

Texture2D m_texture : register(t0, space0);
SamplerState m_sampler : register(s0, space0);

float4 PSMain(VSOutput input)
{
    return m_texture.Sample(m_sampler, input.texcoordUV);
}