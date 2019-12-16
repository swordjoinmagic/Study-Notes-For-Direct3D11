cbuffer cbPerObject{
    // transform
    float4x4 mvp;
    float4x4 model;

    // fog
    float3 fogColor;    // 雾的颜色
    float fogStart;     // 线性雾产生的最小距离
    float fogRange;     // 线性雾产生的最大距离    

    // viewPos
    float3 viewPos;
}

SamplerState sampleState{
    Filter =  MIN_MAG_MIP_LINEAR;
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
    float3 worldPos : POSITION;
};

v2f vert(a2v v){
    v2f o;
    o.pos = mul(float4(v.vertex,1.0),mvp);
    o.uv = v.texcoord;
    o.worldPos = mul(float4(v.vertex,1.0),model).xyz;
    return o;
}

float4 frag(v2f i):SV_TARGET{    

    float3 finalColor = MainTex.Sample(sampleState,i.uv).xyz;

    // 片元距离观察者的距离
    float dist = distance(i.worldPos,viewPos);

    // 线性雾系数
    float fogAmount = saturate((dist-fogStart)/fogRange);
    finalColor = lerp(finalColor,fogColor,fogAmount);

    return float4(finalColor,1.0f);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,frag()));
    }
}