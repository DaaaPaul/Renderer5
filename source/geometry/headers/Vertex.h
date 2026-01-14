#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

namespace Geometry {
	struct Vertex {
		glm::vec4 position;
		glm::vec4 color;

		static VkVertexInputBindingDescription getInputBindingDescription();
		static std::vector<VkVertexInputAttributeDescription> getInputAttributeDescriptions();
	};
}

