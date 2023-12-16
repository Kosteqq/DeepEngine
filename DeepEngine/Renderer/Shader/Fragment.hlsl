
struct PSIn
{
    float4 Color : COLOR0;
};

struct PSOut
{
    float4 Color : SV_TARGET;
};

PSOut main(PSIn i){
    PSOut o ;
    o.Color = i.Color;
    return o;
}
