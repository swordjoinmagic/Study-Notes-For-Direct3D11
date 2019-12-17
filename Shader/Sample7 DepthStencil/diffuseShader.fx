#include "../Common/Light.hlsl"

cbuffer cbPerObject{
    // transform
    float4x4 mvp;
    float4x4 model;
    float4x4 invModel;

    // Light
    Light light;
    float3 viewPos;

    // material
    float3 diffuseColor;
    float3 specularColor;
};

Texture2D MainTex;
SamplerState sampleState1{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct a2v{
    float3 vertex : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

struct v2f{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD0;
};

v2f vert(a2v v){
    v2f o;
    o.pos = mul(float4(v.vertex,1.0),mvp);
    o.worldPos = mul(float4(v.vertex,1.0),model).xyz;
    o.worldNormal = mul(float4(v.normal,1.0),invModel).xyz;
    o.uv = v.texcoord;
    return o;
}

float4 frag(v2f i) : SV_Target{
    float3 worldNormal = normalize(i.worldNormal);
    float3 lightDir = normalize(light.dir);
    
    float NdotL = dot(worldNormal,lightDir)*0.5+0.5;
    
    float4 texColor =  MainTex.Sample(sampleState1,i.uv);
    // 兰伯特漫反射光照
    float3 diffuse = NdotL * diffuseColor * light.color * texColor.rgb;

    float3 worldViewDir = normalize(viewPos - i.worldPos);
    float3 halfDir = normalize(lightDir+worldViewDir);

    float specu = pow(max(0,dot(worldNormal,halfDir)),256);
    float3 specular = specu * specularColor * light.color;

    float3 ambient = diffuse * 0.1f;

    return float4(diffuse+specular+ambient,texColor.a);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,frag()));
    }
}