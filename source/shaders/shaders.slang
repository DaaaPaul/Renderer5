struct VertexIn {
    [[vk::location(0)]] float4 pos;
    [[vk::location(1)]] float4 col;
};

struct VertexOut {
    float4 pos : SV_Position;
    float4 col;
};

[shader("vertex")]
VertexOut vertexShader(VertexIn given) {
    VertexOut final;
    final.pos = given.pos;
    final.col = given.col;

    return final;
}

[shader("fragment")]
float4 fragmentShader(VertexOut processed) {
    return processed.col;
}