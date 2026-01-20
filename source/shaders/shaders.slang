struct VertexIn {
    [[vk::location(0)]] float4 pos;
    [[vk::location(1)]] float4 col;
    [[vk::location(2)]] float2 texCoord;
};

struct VertexOut {
    float4 pos : SV_Position;
    float4 col;
    float2 texCoord;
};

struct Transformation {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

// binding 0 set 0
[[vk::binding(0, 0)]]
ConstantBuffer<Transformation> transformation;

// binding 0 set 1
[[vk::binding(0, 1)]]
Sampler2D textureSampler;

[shader("vertex")]
VertexOut vertexShader(VertexIn given) {
    VertexOut final;
    final.pos = mul(transformation.projection, mul(transformation.view, mul(transformation.model, given.pos)));
    final.col = given.col;
    final.texCoord = given.texCoord;

    return final;
}

[shader("fragment")]
float4 fragmentShader(VertexOut processed) {
    float4 sampledColor = textureSampler.Sample(processed.texCoord);
    float4 givenColor = processed.col;

    return float4(sampledColor.x * givenColor.x, sampledColor.y * givenColor.y, sampledColor.z * givenColor.z, 1.0f);
}