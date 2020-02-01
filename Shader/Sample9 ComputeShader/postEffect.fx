cbuffer cbPerObject{
    matrix mvp;
}
SamplerState SamplerState1{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
Texture2D MainTex;

struct a2v{
    float3 vertex : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct v2f{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

v2f vert(a2v v){
    v2f o;
    o.pos = float4(v.vertex,1.0);
    o.uv = float2(v.texcoord.x,1-v.texcoord.y);
    return o;
}

float4 pixel(v2f i) : SV_Target{
    return MainTex.Sample(SamplerState1,i.uv);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,pixel()));
    }
}