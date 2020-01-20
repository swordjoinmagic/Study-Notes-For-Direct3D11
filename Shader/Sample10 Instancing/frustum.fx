struct InstancedData{
    matrix model;
    float4 color;
};
cbuffer cbPerObject{
    matrix view;
    matrix proj;
};

struct a2v{
    float3 vertex : POSITION;
    float2 texcoord : TEXCOORD0;
};
struct v2f{
    float4 pos : SV_Position;
    nointerpolation float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

StructuredBuffer<InstancedData> instancedDatas;

Texture2D mainTex;
SamplerState state1{
    Filter =  MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

v2f vert(a2v v,uint instanceID : SV_InstanceID){
    v2f o;
    
    InstancedData data = instancedDatas[instanceID];
    matrix mvp = mul(data.model,mul(view,proj));
    
    o.pos = mul(float4(v.vertex,1.0),mvp);
    o.uv = v.texcoord;
    o.color = data.color;

    return o;
}

float4 pixel(v2f i) : SV_TARGET{
    return mainTex.Sample(state1,i.uv) * i.color;
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,pixel()));
    }
}