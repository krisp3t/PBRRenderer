struct VSOut
{
    float3 color : COLOR;
    float4 pos: SV_POSITION;
};

VSOut main(float2 pos : Position, float3 color: Color)
{
    VSOut vso;
    vso.pos = float4(pos, 0.0f, 1.0f);
    vso.color = color;
    return vso;
}