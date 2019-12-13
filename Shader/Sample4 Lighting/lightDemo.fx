#include "../Common/Light.hlsl"

cbuffer cbPerObject{
    Light light;
    float3 diffuse;
    float3 specular;
    float4x4 mvp;
    float4x4 transInvModel;
    float4x4 model;
    float3 viewPos;

    Light pointLight;
    Light spotLight;
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

float GetAtten(Light l,float d){
    return 1.0 / (l.Kc+l.Ki*d,l.Kq*d*d);
}

float4 GetPointLightResult(float3 worldNormal,float3 worldViewDir,float3 fragmentWorldPos){
    worldNormal = normalize(worldNormal);
    worldViewDir = normalize(worldViewDir);
    float3 worldLightDir = normalize(pointLight.pos - fragmentWorldPos);
    float NdotL = max(dot(worldNormal,worldLightDir),0);
    float3 diff = NdotL*diffuse*pointLight.color;
    
    float3 halfDir = normalize(worldLightDir+worldViewDir);
    float spec = pow(max(0,dot(halfDir,worldNormal)),32) * specular * pointLight.color;
    
    float distan = distance(fragmentWorldPos,pointLight.pos);
    float atten = GetAtten(pointLight,distan);

    return float4((spec+diff)*atten,1.0f);
}

float4 GetSportLightResult(float3 worldNormal,float3 worldViewDir,float3 worldPos){
    worldNormal = normalize(worldNormal);
    worldViewDir = normalize(worldViewDir);

    float3 worldLightDir = normalize(spotLight.pos - worldPos);
    float NdotL = max(dot(worldNormal,worldLightDir),0);
    float3 diff = NdotL*diffuse*spotLight.color;
    
    float3 halfDir = normalize(worldLightDir+worldViewDir);
    float spec = pow(max(0,dot(halfDir,worldNormal)),32) * specular * spotLight.color;
    
    float distan = distance(worldPos,spotLight.pos);
    float atten = GetAtten(spotLight,distan);

    // 计算聚光灯外角到内角的衰减

    // worldLightDir和SpotDir的夹角的余弦值,
    // 只有这个夹角<聚光灯外角时,才能显示物体
    float angle = max(0,dot(worldLightDir,normalize(spotLight.dir)));

    float spotAtten = clamp((angle-spotLight.Phi)/(spotLight.Theta-spotLight.Phi),0,1);

    float3 result = (diff+spec)*atten*spotAtten;


    return float4(result,1.0f);
}

v2f vert(a2v v){
    v2f o;
    o.pos = mul(float4(v.vertex,1.0f),mvp);
    o.worldNormal = normalize(mul(v.normal,(float3x3)transInvModel));    
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

    float3 pointLightResult = GetPointLightResult(worldNomral,worldViewDir,i.worldPos).xyz;
    float3 spotLightReulst = GetSportLightResult(worldNomral,worldViewDir,i.worldPos).xyz;

    float3 result = ambi+specu+diff;
    // float3 result = float3(0.1,0.1,0.1);
    result += pointLightResult;
    result += spotLightReulst;

    return float4(result,1.0);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,frag()));    
    }
}