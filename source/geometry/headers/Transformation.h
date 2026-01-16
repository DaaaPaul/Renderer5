#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Geometry {
	struct Transformation {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;

		static VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding(uint32_t binding, uint32_t quantity);
	};
}
