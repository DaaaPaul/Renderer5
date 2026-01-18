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

		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkMemoryRequirements> uniformBuffersRequirements;
		std::vector<VkDeviceSize> uniformBuffersOffsets;
		std::vector<void*> uniformBuffersAddresses;
		uint32_t uniformBufferBindingNum;
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;

		unsigned char* textureAddress;
		int textureWidth;
		int textureHeight;
		uint32_t textureSize;
		VkBuffer stagedTexture;
		VkMemoryRequirements stagedTextureRequirements;
		VkDeviceSize stagedTextureOffset;
		VkImage textureImage;
		VkMemoryRequirements textureImageRequirements;
		VkDeviceSize textureImageOffset;

		void setupBuffersAndMemory();
		void setupDescriptors();

		void createVerticesBuffer();
		void createIndicesBuffer();
		void createUniformBuffers();
		void createTextureBuffer();
		void createStagedVertices();
		void createStagedIndices();
		void allocateGPUMemory();
		void allocateStagingMemory();
		void bindUniformBuffersToStagingMemory();
		void bindTextureImageToGpuMemory();
		void populateVerticesBuffer();
		void populateIndicesBuffer();
		void populateTextureBuffer();

		void mapUniformBuffers();
		void createDescriptorSetLayout();
		void createDescriptorPool();
		void createDescriptorSet();
		void uniformBuffersToDescriptors();

		void loadTexture();
		void createTextureImage();

		void allocateBeginOneTimeCommandBuffer(VkCommandBuffer& cmdBuf, VkCommandPool& pool);
		void endSubmitFreeOneTimeCommandBuffer(VkCommandBuffer& cmdBuf, VkCommandPool& pool);
		void insertImageMemoryBarrier(VkCommandBuffer& cmdBuf, VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 sourceStage, VkAccessFlags2 sourceAccess, VkPipelineStageFlags2 destStage, VkAccessFlags2 destAccess);
		uint32_t getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask);
		void allocateMemory(VkDeviceMemory& memory, VkDeviceSize byteSize, uint32_t memoryTypeIndex);
		VkDeviceSize calculateAllocationSize(std::vector<VkDeviceSize> bufferSizes, std::vector<VkDeviceSize> alignments, std::vector<VkDeviceSize>& offsetsToSet);
		void createBuffer(VkBuffer& buffer, VkDeviceSize byteSize, VkBufferUsageFlags usage);
		void copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize sizeFromBeginning);
		void copyBufferToImage(VkBuffer& src, VkImage& dst, VkExtent2D imageExtent);

	public:
		Memory(Swapchain&& salvageSwapchain);
		Memory(Memory&& salvageMemory);
		~Memory();

		Memory(Memory const&) = delete;
		Memory& operator=(Memory const&) = delete;
	};
}