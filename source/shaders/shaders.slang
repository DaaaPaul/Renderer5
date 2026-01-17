struct VertexIn {
    [[vk::location(0)]] float4 pos;
    [[vk::location(1)]] float4 col;
};

struct VertexOut {
    float4 pos : SV_Position;
    float4 col;
};

struct Transformation {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

[[vk::binding(0, 0)]]
ConstantBuffer<Transformation> transformation;

[shader("vertex")]
VertexOut vertexShader(VertexIn given) {
    VertexOut final;
    final.pos = mul(transformation.projection, mul(transformation.view, mul(transformation.model, given.pos)));
    final.col = given.col;

    return final;
}

[shader("fragment")]
float4 fragmentShader(VertexOut processed) {
    return processed.col;
}