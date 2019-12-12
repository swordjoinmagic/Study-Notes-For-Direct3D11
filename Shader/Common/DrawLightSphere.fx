// 用于绘制光(即简单的标识光的位置)
cbuffer cbPerObject{
    float4x4 mvp;
}

struct a2v{
    float3 vertex : POSITION;
};

float4 vert(a2v v) : SV_Position{
    return mul(float4(v.vertex,1.0),mvp);
}
float4 frag(float4 pos : SV_Position):SV_Target{
    return float4(1,1,1,1);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,frag()));    
    }
}