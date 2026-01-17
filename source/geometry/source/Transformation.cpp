#include "../headers/Transformation.h"

namespace Geometry {
	VkDescriptorSetLayoutBinding Transformation::getDescriptorSetLayoutBinding(uint32_t binding, uint32_t quantity) {
		VkDescriptorSetLayoutBinding layoutBinding = {
			.binding = binding,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = quantity,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.pImmutableSamplers = nullptr
		};

		return layoutBinding;
	}
}