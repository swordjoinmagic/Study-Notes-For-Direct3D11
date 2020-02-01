cbuffer cbPerObject{
    matrix mvp;
    matrix model;
    // 投影器的VP矩阵
    matrix projectorVP;

};

// 要投影的纹理
Texture2D projTex;

// 本身的纹理
Texture2D MainTex;

struct a2v{
    float3 vertex : POSITION;
    float2 texcoord : TEXCOORD0;    
};
struct v2f{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;

    float4 projUV : TEXCOORD1;
    float3 worldPos : POSITION;
};

SamplerState state{
    Filter =  MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    BorderColor = float4(1,1,1,1);
};

v2f vert(a2v v){
    v2f o;
    o.pos = mul(float4(v.vertex,1.0),mvp);
    o.uv = v.texcoord;

    o.worldPos = mul(float4(v.vertex,1.0),model).xyz;
    o.projUV = mul(float4(o.worldPos,1.0),projectorVP);

    return o;
}
float4 pixel(v2f i):SV_TARGET{
    float4 texColor = MainTex.Sample(state,i.uv);

    // 投影器相关操作
    // 对在投影器裁剪坐标下的位置进行齐次除法
    float3 projUV = i.projUV.xyz / i.projUV.w;
    // 将xy坐标缩放到[0,1]区间
    float2 uv = float2(0.5*projUV.x+0.5,-0.5*projUV.y+0.5);

    // 根据uv对投影纹理进行采样
    float4 projTexColor = projTex.Sample(state,uv);

    // 混合两个纹理
    float4 finalColor = texColor * projTexColor;

    return projTexColor;
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,pixel()));    
    }
}