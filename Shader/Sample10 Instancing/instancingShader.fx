#include "../Common/Light.hlsl"

struct InstanceData{
    matrix model;
    matrix invModel;
    uint materialIndex;
};
struct MaerialData{
    float4 diffuseColor;
    float4 specularColor;
};

cbuffer cbPerObject{
    matrix view;
    matrix proj;
    float3 viewPos;
    Light light;
};

struct a2v{
    float3 vertex : POSITION;
    float3 normal : NORMAL;            
};

struct v2f{
    float4 pos : SV_Position;
    float3 worldNormal : NORMAL;
    float3 worldPos : POSITION;
    nointerpolation uint matIndex : MATINDEX;
};

StructuredBuffer<InstanceData> gInstanceData ;
StructuredBuffer<MaerialData> gMaterialData ;

v2f vert(a2v v,uint instanceID : SV_InstanceID){
    v2f o;
    InstanceData instData = gInstanceData[instanceID];

    // matrix mvp = mul(mul(instData.model,view),proj);
    matrix mvp = mul(instData.model,mul(view,proj));

    o.pos = mul(float4(v.vertex,1.0),mvp);
    o.worldNormal = normalize(mul(v.normal,(float3x3)instData.invModel));
    o.worldPos = mul(float4(v.vertex,1.0),instData.model).xyz;
    o.matIndex = instData.materialIndex;

    return o;
}

float4 pixel(v2f i) : SV_Target{
    MaerialData mData = gMaterialData[i.matIndex];
    
    float4 diffuseColor = mData.diffuseColor;
    float4 specuColor = mData.specularColor;
    float4 ambient = diffuseColor * 0.1f;

    float3 worldNormal = normalize(i.worldNormal);
    float3 worldLightDir = normalize(light.dir);

    float NDotL = dot(worldNormal,worldLightDir);
    NDotL = NDotL*0.5+0.5;
    float3 diff = NDotL * diffuseColor.rgb * light.color;

    float3 worldViewDir = normalize(viewPos - i.worldPos);
    
    float3 halfDir = normalize(worldLightDir+worldViewDir);
    float HDotN = dot(halfDir,worldNormal);
    HDotN = max(0,HDotN);

    float3 specu = HDotN * specuColor.rgb;

    return float4(ambient.rgb + diff + specu,1.0);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,pixel()));    
    }
}