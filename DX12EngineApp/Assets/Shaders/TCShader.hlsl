
struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texcoordUV : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 texcoordUV : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = input.position;
    output.color = input.color;
    output.texcoordUV = input.texcoordUV;
    
    return output;
}

Texture2D m_texture : register(t0, space0);
SamplerState m_sampler : register(s0, space0);

float4 PSMain(VSOutput input) : SV_Target
{
    float4 texColor = m_texture.Sample(m_sampler, input.texcoordUV);
    float4 finalColor = texColor * input.color;
    
    return finalColor;
}