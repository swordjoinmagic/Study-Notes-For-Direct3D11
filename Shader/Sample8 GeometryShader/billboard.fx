cbuffer cbPerObject{
    // transfrom
    float4x4 model;
    float4x4 vpMatrix;

    // viewPos
    float3 viewPos;

    // 该quad的宽高
    float2 Size;
};

cbuffer cbFixed{
    float2 gTexC[4] = {
        float2(0.0f,1.0f),
        float2(0.0f,0.0f),
        float2(1.0f,1.0f),
        float2(1.0f,0.0f)
    };
};

SamplerState SamplerState1{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

Texture2D MainTex;

struct a2v{
    float3 vertex : POSITION;
};

struct v2g{
    float3 worldPos : POSITION;
};

struct g2f{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    uint PirmID : SV_PrimitiveID;
};

v2g vert(a2v v){
    v2g o;
    o.worldPos = mul(float4(v.vertex,1.0f),model).xyz;
    return o;
}

/*
    将一个点扩充为平行四边形,并该quad始终面对摄像机    
*/
[maxvertexcount(4)]
void geometry(point v2g gin[1],uint primID : SV_PrimitiveID,
             inout TriangleStream<g2f> triStream){
    // 计算该广告板的三个轴
    float3 up = float3(0.0f,1.0f,0.0f);
    float3 look = viewPos - gin[0].worldPos;
    look.y = 0.0f;
    look = normalize(look);
    float3 right = cross(up,look);

    float halfWidth = 0.5f * Size.x;
    float halfHeight = 0.5f * Size.y;

    // 构建quad的四个顶点的世界坐标位置
    float4 v[4];
    // 右下
    v[0] = float4(gin[0].worldPos + halfWidth*right - halfHeight*up,1.0f);
    // 右上
    v[1] = float4(gin[0].worldPos + halfWidth*right + halfHeight*up,1.0f);
    // 左下
    v[2] = float4(gin[0].worldPos - halfWidth*right - halfHeight*up,1.0f);
    // 左上
    v[3] = float4(gin[0].worldPos - halfWidth*right + halfHeight*up,1.0f);

    g2f o;
    [unroll]
    for(int i=0;i<4;i++){
        o.pos = mul(v[i],vpMatrix);
        o.uv = gTexC[i];
        o.PirmID = primID;
        triStream.Append(o);
    }

}

float4 pixelShader(g2f i) : SV_Target{
    float4 texColor = MainTex.Sample(SamplerState1,i.uv);
    clip(texColor.a-0.05f);
    return float4(texColor.rgb,1);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetGeometryShader(CompileShader(gs_5_0,geometry()));
        SetPixelShader(CompileShader(ps_5_0,pixelShader()));
    }
}