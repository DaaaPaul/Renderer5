#pragma once

#include "Swapchain.h"
#include "../geometry/Vertex.h"
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

		std::vector<Geometry::Vertex> vertices;
		std::vector<uint32_t> indices;
		uint32_t graphicsQueueFamilyIndex;
		VkMemoryRequirements verticesBufferRequirements;
		VkMemoryRequirements indicesBufferRequirements;

		Swapchain swapchain;
		VkDeviceMemory stagingM;
		VkDeviceMemory gpuM;
		VkBuffer stagedvertices;
		VkBuffer stagedIndices;
		VkBuffer verticesBuffer;
		VkBuffer indicesBuffer;

		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<void*> uniformBuffersA;

		void createVerticesBuffer();
		void createIndicesBuffer();
		void allocateStagingMemory();
		void allocateGPUMemory();
		void initializeStagedVertices();
		void initializeStagedIndices();

		uint32_t getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask);

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