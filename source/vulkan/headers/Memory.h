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

		VkDescriptorSetLayout u_descriptorSetLayout;
		VkDescriptorPool u_descriptorPool;
		std::vector<VkDescriptorSet> u_descriptorSets;
		VkDescriptorSetLayoutBinding u_descriptorSetLayoutBinding;
		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkMemoryRequirements> uniformBuffersRequirements;
		std::vector<VkDeviceSize> uniformBuffersOffsets;
		std::vector<void*> uniformBuffersAddresses;
		uint32_t uniformBufferBindingNum;

		unsigned char* textureAddress;
		int textureWidth;
		int textureHeight;
		uint32_t textureSize;
		VkBuffer stagedTexture;
		VkMemoryRequirements stagedTextureRequirements;
		VkDeviceSize stagedTextureOffset;
		VkImage textureImage;
		VkImageView textureImageView;
		VkMemoryRequirements textureImageRequirements;
		VkDeviceSize textureImageOffset;

		VkSampler textureSampler;
		VkDescriptorSetLayout t_descriptorSetLayout;
		VkDescriptorPool t_descriptorPool;
		std::vector<VkDescriptorSet> t_descriptorSets;
		VkDescriptorSetLayoutBinding t_descriptorSetLayoutBinding;

		VkImage depthImage;
		VkImageView depthImageView;
		VkMemoryRequirements depthImageRequirements;
		VkDeviceSize depthImageOffset;

		void setupModel();
		void setupDepthImage();
		void setupTextureImage();
		void setupBuffersAndMemory();
		void setup_u_Descriptors();
		void setupSampler();
		void setup_t_Descriptors();

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
		void bindDepthImageToGpuMemory();
		void populateVerticesBuffer();
		void populateIndicesBuffer();
		void populateTextureBuffer();

		void mapUniformBuffers();
		void create_u_DescriptorSetLayout();
		void create_u_DescriptorPool();
		void create_u_DescriptorSets();
		void uniformBuffersToDescriptors();

		void loadTexture();
		void createTextureImage();
		void createTextureImageView();

		void create_t_DescriptorSetLayout();
		void create_t_DescriptorPool();
		void create_t_DescriptorSets();
		void samplerToDescriptors();

		void createDepthImage();
		void createDepthImageView();

		void allocateBeginOneTimeCommandBuffer(VkCommandBuffer& cmdBuf, VkCommandPool& pool);
		void endSubmitFreeOneTimeCommandBuffer(VkCommandBuffer& cmdBuf, VkCommandPool& pool);
		void insertImageMemoryBarrier(VkCommandBuffer& cmdBuf, VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 sourceStage, VkAccessFlags2 sourceAccess, VkPipelineStageFlags2 destStage, VkAccessFlags2 destAccess);
		uint32_t getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask);
		void allocateMemory(VkDeviceMemory& memory, VkDeviceSize byteSize, uint32_t memoryTypeIndex);
		VkDeviceSize calculateAllocationSize(std::vector<VkDeviceSize> bufferSizes, std::vector<VkDeviceSize> alignments, std::vector<VkDeviceSize>& offsetsToSet);
		void createBuffer(VkBuffer& buffer, VkDeviceSize byteSize, VkBufferUsageFlags usage);
		void copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize sizeFromBeginning);
		void copyBufferToImage(VkBuffer& src, VkImage& dst, VkExtent2D imageExtent);
		void createImage(VkImage& image, VkFormat format, VkExtent3D extent, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage);
		void createImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspect);

	public:
		Memory(Swapchain&& salvageSwapchain);
		Memory(Memory&& salvageMemory);
		~Memory();

		Memory(Memory const&) = delete;
		Memory& operator=(Memory const&) = delete;
	};
}