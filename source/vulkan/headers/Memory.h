#pragma once

#include "Swapchain.h"
#include "../../geometry/headers/Vertex.h"
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

		const uint16_t FLIGHT_COUNT;

		std::vector<Geometry::Vertex> vertices;
		std::vector<uint32_t> indices;
		uint32_t graphicsQueueFamilyIndex;
		VkMemoryRequirements verticesBufferRequirements;
		VkDeviceSize verticesBufferOffset;
		uint32_t verticesBufferSize;
		VkMemoryRequirements indicesBufferRequirements;
		VkDeviceSize indicesBufferOffset;
		uint32_t indicesBufferSize;
		VkMemoryRequirements stagedVerticesRequirements;
		VkDeviceSize stagedVerticesOffset;
		uint32_t stagedVerticesSize;
		VkMemoryRequirements stagedIndicesRequirements;
		VkDeviceSize stagedIndicesOffset;
		uint32_t stagedIndicesSize;

		Swapchain swapchain;
		VkDeviceMemory stagingMemory;
		VkDeviceMemory gpuMemory;
		VkBuffer verticesBuffer;
		VkBuffer indicesBuffer;
		VkBuffer stagedVertices;
		VkBuffer stagedIndices;

		std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<void*> uniformBuffersAddresses;

		void setupVerticesIndicesBuffers();
		void setupDescriptorSets();

		void createVerticesBuffer();
		void createIndicesBuffer();
		void createUniformBuffer();
		void createStagedVertices();
		void createStagedIndices();
		void allocateGPUMemory();
		void allocateStagingMemory();
		void populateVerticesBuffer();
		void populateIndicesBuffer();

		void createDescriptorSetLayout();
		void createDescriptorPool();
		void createDescriptorSet();

		uint32_t getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask);
		void allocateMemory(VkDeviceMemory& memory, VkDeviceSize byteSize, uint32_t memoryTypeIndex);
		VkDeviceSize calculateAllocationSize(VkDeviceSize size1, VkDeviceSize alignment1, VkDeviceSize size2, VkDeviceSize alignment2, VkDeviceSize& offset1, VkDeviceSize& offset2);
		void createBuffer(VkBuffer& buffer, VkDeviceSize byteSize, VkBufferUsageFlags usage);
		void copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize sizeFromBeginning);

	public:
		Memory(Swapchain&& salvageSwapchain);
		Memory(Memory&& salvageMemory);
		~Memory();

		Memory(Memory const&) = delete;
		Memory& operator=(Memory const&) = delete;
	};
}