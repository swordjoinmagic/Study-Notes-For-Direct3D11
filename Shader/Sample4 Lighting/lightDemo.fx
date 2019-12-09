struct Light{
    float3 pos;
    float Kc;  
    float3 dir;
    float Ki;
    float3 color;
    float Kq;
    float Phi;
    float Theta;
};

cbuffer cbPerObject{
    Light light;
    float3 diffuse;
    float3 specular;
    float4x4 mvp;
    float4x4 transInvMvp;
    float4x4 model;
    float3 viewPos;
};

struct a2v{
    float3 vertex : POSITION;
    float3 normal : NORMAL;        
};

struct v2f{
    float4 pos : SV_Position;
    float3 worldNormal : NORMAL;
    float3 worldPos : POSITION;
};

v2f vert(a2v v){
    v2f o;
    o.pos = mul(float4(v.vertex,1.0f),mvp);
    o.worldNormal = normalize(mul(v.normal,(float3x3)transInvMvp));    
    o.worldPos = mul(float4(v.vertex,1.0f),model).xyz;
    return o;
}

float4 frag(v2f i):SV_Target{
    float3 worldLightDir = normalize(light.dir);
    float3 worldNomral = normalize(i.worldNormal);
    float NdotL = dot(worldNomral,worldLightDir)*0.5+0.5;

    // 漫反射分量
    float3 diff = diffuse*NdotL*light.color;
    // 环境光
    float3 ambi = diff * 0.1f;

    float3 worldViewDir = normalize(viewPos - i.worldPos);    

    float3 halfDir = normalize(worldViewDir + worldLightDir);

    float3 specu = pow(max(dot(worldNomral,halfDir),0),32) * specular;

    return float4(ambi+specu+diff,1.0);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,frag()));    
    }
}