#include "../Common/Light.hlsl"

cbuffer cbPerObject : register(b0){
    matrix mvp;
    // model的逆转置矩阵,用于法线的变换
    matrix transInvModel;
    // model矩阵,用于变换坐标到世界空间
    matrix model;

    // 世界-光源空间的变换矩阵
    matrix World2LightMatrix;

    // 平行光光源
    Light light;

    // 摄像机坐标
    float3 viewPos;

    float4 diffuseColor;
    float4 specularColor;
    float4 shadowColor;

    float bias;
};

// 物体自身的纹理
Texture2D mainTex;

// 阴影贴图(光源空间的深度图)
Texture2D shadowMap;

SamplerState state{
    Filter =  MIN_MAG_MIP_LINEAR;
    AddressU = BORDER;
    AddressV = BORDER;
    BorderColor = float4(1,1,1,1);
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
    float4 shadowUV : TEXCOORD1;
};

v2f vert(a2v v){
    v2f o;

    o.pos = mul(float4(v.vertex,1.0f),mvp);
    o.worldPos = mul(float4(v.vertex,1.0),model).xyz;
    o.worldNormal = mul(float4(v.normal,0),transInvModel).xyz;
    o.uv = v.texcoord;
    o.shadowUV = mul(float4(o.worldPos,1.0),World2LightMatrix);

    return o;
}

float4 pixel(v2f i) : SV_Target{

    float3 worldNormal = normalize(i.worldNormal);
    float3 worldLightDir = normalize(light.dir);
    // 半兰伯特漫反射
    float NDotL = dot(worldNormal,worldLightDir)*0.5+0.5;
    float4 diffuse = NDotL * mainTex.Sample(state,i.uv) * diffuseColor;

    float3 worldViewDir = normalize(viewPos - i.worldPos);
    // 光源和观察坐标之间的中间向量(只有法线为该中间向量,才会反射到观察点处)
    float3 halfDir = normalize(worldLightDir+worldViewDir);

    // biiln-phong高光反射
    float NDotH = max(0,dot(worldNormal,halfDir));

    float4 specular = NDotH * specularColor;

    float4 ambient = diffuse * 0.1;

    // 计算阴影
    // float4 shadowUV = mul(float4(i.worldPos,1.0),World2LightMatrix);
    float4 shadowUV = i.shadowUV;
    // 进行齐次除法
    shadowUV.xyz /= shadowUV.w;

    // float2 uv = float2(shadowUV.x,1-shadowUV.y);
    // uv = uv * 0.5 + float2(0.5,0.5);
    float2 uv = float2(0.5*shadowUV.x+0.5,-0.5*shadowUV.y+0.5);

    float shdaowBias = 0.005;

    float4 finalColor = ambient + diffuse;

    // 阴影因子,越接近1则阴影感越强,越接近0则阴影感越弱
    float shadowFactor = 0;    

    uint width,height,numMips;
    shadowMap.GetDimensions(0,width,height,numMips);

    // 纹素的大小
    float dx = 1.0 / (float)width;

    const float2 offsets[9] = {
        float2(-dx,-dx),float2(0,-dx),float2(dx,-dx),
        float2(-dx,0),float2(0,0),float2(dx,0),
        float2(-dx,dx),float2(0,dx),float2(dx,dx)
    };

    // 获得当前像素坐标在光源空间下的实际深度        
    float depth = shadowUV.z;
    for(int i=0;i<9;i++){
        // 采样获得当前坐标在光源空间下的最近深度
        float nearestDepth = shadowMap.Sample(state,uv+offsets[i]).r + bias;        
        // 如果当前像素深度大于最近深度,说明当前像素被某个物体遮挡了,应该产生阴影
        shadowFactor += depth > nearestDepth;
    }
    
    shadowFactor /= 9;
    

    return lerp(finalColor,shadowColor,shadowFactor);
}

technique11{
    pass P0{
        SetVertexShader(CompileShader(vs_5_0,vert()));
        SetPixelShader(CompileShader(ps_5_0,pixel()));    
    }
}