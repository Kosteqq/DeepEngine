
static float2 positions[3] = 
{
    float2(-0.0, 0.5),
    float2(-0.5, -0.5),
    float2(0.5, -0.5),
};

static float4 colors[3] =
{
    float4(1.0, 0.0, 0.0, 1.0),
    float4(0.0, 1.0, 0.0, 1.0),
    float4(0.0, 0.0, 1.0, 1.0)
};

struct VOut
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VOut main(uint VertexIndex : SV_VertexID)
{
    VOut o = (VOut)0;
    o.Pos = float4(positions[VertexIndex], 0.0, 1.0);
    o.Color = colors[VertexIndex];
    return o;
}