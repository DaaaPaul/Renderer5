struct Vertex {
    [[vk::location(0)]] float4 pos : SV_Position;
    [[vk::location(1)]] float4 col;
};

[shader("vertex")]
Vertex vertexShader(Vertex given) {
    return given;
}

[shader("fragment")]
float4 fragmentShader(Vertex processed) {
    return processed.col;
}