cbuffer cbPerObject{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

float4 vert(float3 v:POSITION): SV_Position{
    float4x4 mvp = mul(mul(model,view),proj);
    return mul(float4(v,1.0f),mvp);
}

float4 frag(float4 vertex : SV_Position) : SV_Target{
    return float4(1,1,1,1);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,frag()));    
    }
}