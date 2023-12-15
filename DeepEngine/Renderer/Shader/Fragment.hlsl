
struct PSOut
{
    float4 Color : SV_TARGET;
};

PSOut main() {
    PSOut o ;
    o.Color = float4(1.0, 0.0, 0.0, 1.0);
    return o;
}
