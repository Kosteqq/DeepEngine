
static float2 positions[3] = 
{
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5),
};

struct VOut
{
    float4 Pos : SV_POSITION;
};

VOut main(uint VertexIndex : SV_VertexID)
{
    VOut o = (VOut)0;
    o.Pos = float4(positions[VertexIndex], 0.0, 1.0);
    return o;
}