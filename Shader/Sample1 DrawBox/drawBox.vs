cbuffer cbPerObject{
    float4x4 mvp;
}

struct a2v{
    float3 vertex : POSITIONT;
    float4 color : COLOR;
};

struct v2f{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

v2f main(a2v v){
    v2f o;
    o.pos = mul(float4(v.vertex,1.0),mvp);
    o.color = v.color;
    return o;
}