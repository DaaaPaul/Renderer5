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
		VkMemoryRequirements stagedVerticesRequirements;
		VkMemoryRequirements stagedIndicesRequirements;

		Swapchain swapchain;
		VkDeviceMemory stagingMemory;
		VkDeviceMemory gpuMemory;
		VkBuffer verticesBuffer;
		VkBuffer indicesBuffer;
		VkBuffer stagedVertices;
		VkBuffer stagedIndices;

		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<void*> uniformBuffersAddresses;

		void createVerticesBuffer();
		void createIndicesBuffer();
		void createStagedVertices();
		void createStagedIndices();
		void allocateGPUMemory();
		void allocateStagingMemory();
		void populateVerticesBuffer();
		void populateIndicesBuffer();

		uint32_t getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask);

		void allocateMemory(VkDeviceMemory& memory, VkDeviceSize byteSize, uint32_t memoryTypeIndex);
		VkDeviceSize calculateAllocationSize(VkDeviceSize size1, VkDeviceSize alignment1, VkDeviceSize size2, VkDeviceSize alignment2);
		void createBuffer(VkBuffer& buffer, VkDeviceSize byteSize, VkBufferUsageFlags usage);
		void copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize copySizeFromStart);
	public:
		Memory(Swapchain&& salvageSwapchain);
		Memory(Memory&& salvageMemory);
		~Memory();

		Memory(Memory const&) = delete;
		Memory& operator=(Memory const&) = delete;
	};
}