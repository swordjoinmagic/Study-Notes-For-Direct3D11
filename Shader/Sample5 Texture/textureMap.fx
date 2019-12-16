cbuffer cbPerObject{
    float4x4 mvp;
};

Texture2D MainTex;


SamplerState state1{
    Filter =  MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

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
    o.pos = mul(float4(v.vertex,1.0),mvp);
    o.uv = v.texcoord;
    return o;
}
float4 frag(v2f i):SV_Target{
    return MainTex.Sample(state1,i.uv);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,frag()));    
    }
}