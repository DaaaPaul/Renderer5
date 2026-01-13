#pragma once

#include "Swapchain.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan {
	class Pipeline;
	class Sync;
	class Commands;
	class Engine;

	class Memory {
		friend class Pipeline;
		friend class Sync;
		friend class Commands;
		friend class Engine;

	private:
		bool isSalvagedRemains;

		uint32_t graphicsQueueFamilyIndex;

		Swapchain swapchain;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferM;
		VkBuffer verticiesBuffer;
		VkDeviceMemory verticiesBufferM;
		VkBuffer indicesBuffer;
		VkDeviceMemory indicesBufferM;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersM;
		std::vector<void*> uniformBuffersA;

		void createVerticiesBuffer();
		void createIndicesBuffer();

		void allocateMemory(VkDeviceMemory& memory, VkDeviceSize byteSize, uint32_t memoryTypeIndex);
		void createBuffer(VkBuffer& buffer, VkDeviceSize byteSize, VkBufferUsageFlags usage);
		void copyBuffer();
	public:
		Memory(Swapchain&& salvageSwapchain);
		Memory(Memory&& salvageMemory);
		~Memory();

		Memory(Memory const&) = delete;
		Memory& operator=(Memory const&) = delete;
	};
}