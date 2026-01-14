#include "../headers/Vertex.h"

namespace Geometry {
	VkVertexInputBindingDescription Vertex::getInputBindingDescription() {
		VkVertexInputBindingDescription vertexInputBinding = {
			.binding = 0,
			.stride = sizeof(Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};

		return vertexInputBinding;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getInputAttributeDescriptions() {
		VkVertexInputAttributeDescription positionDescription = {
			.location = 0,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = offsetof(Vertex, position)
		};
		VkVertexInputAttributeDescription colorDescription = {
			.location = 1,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = offsetof(Vertex, color)
		};

		return { positionDescription, colorDescription };
	}
}