struct v2f{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

float4 main(v2f i) : SV_Target{
    return i.color;
}
