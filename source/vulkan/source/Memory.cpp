#include "../headers/Memory.h"
#include <glm/glm.hpp>
#include <iostream>

namespace Vulkan {
	Memory::Memory(Swapchain&& salvageSwapchain) :
	isSalvagedRemains{ false },		
	vertices{ 
		Geometry::Vertex(glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
		Geometry::Vertex(glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
		Geometry::Vertex(glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
		Geometry::Vertex(glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f))
	},
	indices{
		0, 1, 2,
		0, 2, 3
	},
	graphicsQueueFamilyIndex{ swapchain.graphicsQueueFamilyIndex },
	verticesBufferRequirements{},
	verticesBufferOffset{},
	verticesBufferSize{ static_cast<uint32_t>(sizeof(vertices[0]) * vertices.size()) },
	indicesBufferRequirements{},
	indicesBufferOffset{},
	indicesBufferSize{ static_cast<uint32_t>(sizeof(indices[0]) * indices.size()) },
	stagedVerticesRequirements{},
	stagedVerticesOffset{},
	stagedVerticesSize{ verticesBufferSize },
	stagedIndicesRequirements{},
	stagedIndicesOffset{},
	stagedIndicesSize{ indicesBufferSize },

	swapchain{ std::move(salvageSwapchain) },
	stagingMemory{ VK_NULL_HANDLE },
	gpuMemory{ VK_NULL_HANDLE },
	stagedVertices{ VK_NULL_HANDLE },
	stagedIndices{ VK_NULL_HANDLE },
	verticesBuffer{ VK_NULL_HANDLE },
	indicesBuffer{ VK_NULL_HANDLE },

	descriptorSetLayout{ VK_NULL_HANDLE },
	descriptorPool{ VK_NULL_HANDLE },
	descriptorSet{ VK_NULL_HANDLE },
	uniformBuffers{},
	uniformBuffersAddresses{} {
		std::cout << "---CREATING MEMORY...---\n";

		createVerticesBuffer();
		createIndicesBuffer();
		createStagedVertices();
		createStagedIndices();
		allocateGPUMemory();
		allocateStagingMemory();
		populateVerticesBuffer();
		populateIndicesBuffer();
	}

	Memory::Memory(Memory&& salvageMemory) :
		isSalvagedRemains{ false },
		vertices{ std::move(salvageMemory.vertices) },
		indices{ std::move(salvageMemory.indices) },
		graphicsQueueFamilyIndex{ salvageMemory.graphicsQueueFamilyIndex },
		verticesBufferRequirements{ salvageMemory.verticesBufferRequirements },
		verticesBufferOffset{ salvageMemory.verticesBufferOffset },
		verticesBufferSize{ salvageMemory.verticesBufferSize },
		indicesBufferRequirements{ salvageMemory.indicesBufferRequirements },
		indicesBufferOffset{ salvageMemory.indicesBufferOffset },
		indicesBufferSize{ salvageMemory.indicesBufferSize },
		stagedVerticesRequirements{ salvageMemory.stagedVerticesRequirements },
		stagedVerticesOffset{ salvageMemory.stagedVerticesOffset },
		stagedVerticesSize{ salvageMemory.stagedVerticesSize },
		stagedIndicesRequirements{ salvageMemory.stagedIndicesRequirements },
		stagedIndicesOffset{ salvageMemory.stagedIndicesOffset },
		stagedIndicesSize{ salvageMemory.stagedIndicesSize },

		swapchain{ std::move(salvageMemory.swapchain) },
		stagingMemory{ salvageMemory.stagingMemory },
		gpuMemory{ salvageMemory.gpuMemory },
		stagedVertices{ salvageMemory.stagedVertices },
		stagedIndices{ salvageMemory.stagedIndices },
		verticesBuffer{ salvageMemory.verticesBuffer },
		indicesBuffer{ salvageMemory.indicesBuffer },

		descriptorSetLayout{ salvageMemory.descriptorSetLayout },
		descriptorPool{ salvageMemory.descriptorPool },
		descriptorSet{ salvageMemory.descriptorSet },
		uniformBuffers{ salvageMemory.uniformBuffers },
		uniformBuffersAddresses{ salvageMemory.uniformBuffersAddresses } {
		salvageMemory.isSalvagedRemains = true;

		salvageMemory.graphicsQueueFamilyIndex = 0xFFFFFFFF;
		salvageMemory.verticesBufferRequirements = VkMemoryRequirements{};
		salvageMemory.verticesBufferOffset = 0xFFFFFFFF;
		salvageMemory.verticesBufferSize = 0xFFFFFFFF;
		salvageMemory.indicesBufferRequirements = VkMemoryRequirements{};
		salvageMemory.indicesBufferOffset = 0xFFFFFFFF;
		salvageMemory.indicesBufferSize = 0xFFFFFFFF;
		salvageMemory.stagedVerticesRequirements = VkMemoryRequirements{};
		salvageMemory.stagedVerticesOffset = 0xFFFFFFFF;
		salvageMemory.stagedVerticesSize = 0xFFFFFFFF;
		salvageMemory.stagedIndicesRequirements = VkMemoryRequirements{};
		salvageMemory.stagedIndicesOffset = 0xFFFFFFFF;
		salvageMemory.stagedIndicesSize = 0xFFFFFFFF;

		salvageMemory.stagingMemory = VK_NULL_HANDLE;
		salvageMemory.gpuMemory = VK_NULL_HANDLE;
		salvageMemory.stagedVertices = VK_NULL_HANDLE;
		salvageMemory.stagedIndices = VK_NULL_HANDLE;
		salvageMemory.verticesBuffer = VK_NULL_HANDLE;
		salvageMemory.indicesBuffer = VK_NULL_HANDLE;

		salvageMemory.descriptorSetLayout = VK_NULL_HANDLE;
		salvageMemory.descriptorPool = VK_NULL_HANDLE;
		salvageMemory.descriptorSet = VK_NULL_HANDLE;
		salvageMemory.uniformBuffers.clear();
		salvageMemory.uniformBuffersAddresses.clear();
		
		std::cout << "---MOVED MEMORY---";
	}

	Memory::~Memory() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING MEMORY...---\n";

			vkDestroyBuffer(swapchain.queues.backend.device, verticesBuffer, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, indicesBuffer, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, stagedVertices, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, stagedIndices, nullptr);

			vkFreeMemory(swapchain.queues.backend.device, gpuMemory, nullptr);
			vkFreeMemory(swapchain.queues.backend.device, stagingMemory, nullptr);
		}
	}

	void Memory::createVerticesBuffer() {
		createBuffer(verticesBuffer, sizeof(vertices[0]) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		vkGetBufferMemoryRequirements(swapchain.queues.backend.device, verticesBuffer, &verticesBufferRequirements);

		std::cout << "Created vertices buffer with size " << sizeof(vertices[0]) * vertices.size() << 
			" requiring size of " << verticesBufferRequirements.size << 
			" and offsetted at " << verticesBufferRequirements.alignment << '\n';
	}

	void Memory::createIndicesBuffer() {
		createBuffer(indicesBuffer, sizeof(indices[0]) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		vkGetBufferMemoryRequirements(swapchain.queues.backend.device, indicesBuffer, &indicesBufferRequirements);

		std::cout << "Created indices buffer with size " << sizeof(indices[0]) * indices.size() << 
			" requiring size of " << indicesBufferRequirements.size << 
			" and offsetted at " << indicesBufferRequirements.alignment << '\n';
	}

	void Memory::createStagedVertices() {
		createBuffer(stagedVertices, sizeof(vertices[0]) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		vkGetBufferMemoryRequirements(swapchain.queues.backend.device, stagedVertices, &stagedVerticesRequirements);

		std::cout << "Created staged vertices buffer with size " << sizeof(vertices[0]) * vertices.size() <<
			" requiring size of " << stagedVerticesRequirements.size <<
			" and offsetted at " << stagedVerticesRequirements.alignment << '\n';
	}

	void Memory::createStagedIndices() {
		createBuffer(stagedIndices, sizeof(indices[0]) * indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		vkGetBufferMemoryRequirements(swapchain.queues.backend.device, stagedIndices, &stagedIndicesRequirements);

		std::cout << "Created staged indices buffer with size " << sizeof(indices[0]) * indices.size() <<
			" requiring size of " << stagedIndicesRequirements.size <<
			" and offsetted at " << stagedIndicesRequirements.alignment << '\n';
	}

	void Memory::allocateGPUMemory() {
		VkDeviceSize allocationSize = calculateAllocationSize(verticesBufferRequirements.size, verticesBufferRequirements.alignment, indicesBufferRequirements.size, indicesBufferRequirements.alignment, verticesBufferOffset, indicesBufferOffset);
		uint32_t memoryTypeIndex = getMemoryTypeIndex(verticesBufferRequirements.memoryTypeBits & indicesBufferRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		allocateMemory(gpuMemory, allocationSize, memoryTypeIndex);

		std::cout << "Allocated gpu memory with size " << allocationSize << " and memory type " << memoryTypeIndex << '\n';
	}

	void Memory::allocateStagingMemory() {
		VkDeviceSize allocationSize = calculateAllocationSize(stagedVerticesRequirements.size, stagedVerticesRequirements.alignment, stagedIndicesRequirements.size, stagedIndicesRequirements.alignment, stagedVerticesOffset, stagedIndicesOffset);
		uint32_t memoryTypeIndex = getMemoryTypeIndex(stagedVerticesRequirements.memoryTypeBits & stagedIndicesRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		allocateMemory(stagingMemory, allocationSize, memoryTypeIndex);

		std::cout << "Allocated staging memory with size " << allocationSize << " and memory type " << memoryTypeIndex << '\n';
	}

	void Memory::populateVerticesBuffer() {
		vkBindBufferMemory(swapchain.queues.backend.device, stagedVertices, stagingMemory, stagedVerticesOffset);
		vkBindBufferMemory(swapchain.queues.backend.device, verticesBuffer, gpuMemory, verticesBufferOffset);

		void* stagedVerticesAddress = nullptr;
		if(vkMapMemory(swapchain.queues.backend.device, stagingMemory, stagedVerticesOffset, stagedVerticesSize, 0, &stagedVerticesAddress) != VK_SUCCESS) {
			throw std::runtime_error("Mapping memory failure");
		}
		memcpy(stagedVerticesAddress, vertices.data(), stagedVerticesSize);
		vkUnmapMemory(swapchain.queues.backend.device, stagingMemory);
		
		copyBuffer(stagedVertices, verticesBuffer, stagedVerticesSize);
	}

	void Memory::populateIndicesBuffer() {
		vkBindBufferMemory(swapchain.queues.backend.device, stagedIndices, stagingMemory, stagedIndicesOffset);
		vkBindBufferMemory(swapchain.queues.backend.device, indicesBuffer, gpuMemory, indicesBufferOffset);

		void* stagedIndicesAddress = nullptr;
		if (vkMapMemory(swapchain.queues.backend.device, stagingMemory, stagedIndicesOffset, stagedIndicesSize, 0, &stagedIndicesAddress) != VK_SUCCESS) {
			throw std::runtime_error("Mapping memory failure");
		}
		memcpy(stagedIndicesAddress, indices.data(), stagedIndicesSize);
		vkUnmapMemory(swapchain.queues.backend.device, stagingMemory);

		copyBuffer(stagedIndices, indicesBuffer, stagedIndicesSize);
	}

	uint32_t Memory::getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask) {
		VkPhysicalDeviceMemoryProperties2 memoryProperties{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2
		};
		vkGetPhysicalDeviceMemoryProperties2(swapchain.queues.backend.physicalDevice, &memoryProperties);

		for (int i = 0; i < memoryProperties.memoryProperties.memoryTypeCount; i++) {
			if ((memoryRequirementsMask & (1 << i)) && 
				((memoryProperties.memoryProperties.memoryTypes[i].propertyFlags & propertyMask) == propertyMask)) {
				return i;
			}
		};

		throw std::runtime_error("No suitable memory type found\n");
	}

	void Memory::allocateMemory(VkDeviceMemory& memory, VkDeviceSize byteSize, uint32_t memoryTypeIndex) {
		VkMemoryAllocateInfo memoryAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = byteSize,
			.memoryTypeIndex = memoryTypeIndex
		};

		if(vkAllocateMemory(swapchain.queues.backend.device, &memoryAllocateInfo, nullptr, &memory) == VK_SUCCESS) {
			std::cout << "Allocated device memory with " << memoryAllocateInfo.allocationSize << " size and " << memoryAllocateInfo.memoryTypeIndex << " memory type\n";
		} else {
			throw std::runtime_error("Memory creation failure");
		}
	}

	VkDeviceSize Memory::calculateAllocationSize(VkDeviceSize size1, VkDeviceSize alignment1, VkDeviceSize size2, VkDeviceSize alignment2, VkDeviceSize& offset1, VkDeviceSize& offset2) {
		VkDeviceSize allocationSize = 0;
		offset1 = 0;

		allocationSize += size1;

		while (allocationSize % alignment2 != 0) {
			allocationSize++;
		}
		offset2 = allocationSize;

		allocationSize += size2;

		return allocationSize;
	}

	void Memory::createBuffer(VkBuffer& buffer, VkDeviceSize byteSize, VkBufferUsageFlags usage) {
		VkBufferCreateInfo bufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = byteSize,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &graphicsQueueFamilyIndex
		};

		if (vkCreateBuffer(swapchain.queues.backend.device, &bufferInfo, nullptr, &buffer) == VK_SUCCESS) {
			std::cout << "Created buffer with " << bufferInfo.size << " size\n";
		} else {
			throw std::runtime_error("Memory creation failure");
		}
	}


	void Memory::copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize sizeFromBeginning) {
		VkCommandPool tempPool = VK_NULL_HANDLE;
		VkCommandPoolCreateInfo poolInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			.queueFamilyIndex = graphicsQueueFamilyIndex
		};
		if (vkCreateCommandPool(swapchain.queues.backend.device, &poolInfo, nullptr, &tempPool) != VK_SUCCESS) {
			throw std::runtime_error("Temporary command pool allocation failure");
		}

		VkCommandBuffer tempCmdBuf = VK_NULL_HANDLE;
		VkCommandBufferAllocateInfo cmdBufInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = tempPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		vkAllocateCommandBuffers(swapchain.queues.backend.device, &cmdBufInfo, &tempCmdBuf);

		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr
		};
		vkBeginCommandBuffer(tempCmdBuf, &beginInfo);
		VkBufferCopy fullSize = {
			.srcOffset = 0,
			.dstOffset = 0,
			.size = sizeFromBeginning
		};
		vkCmdCopyBuffer(tempCmdBuf, src, dst, 1, &fullSize);
		vkEndCommandBuffer(tempCmdBuf);

		VkSubmitInfo submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,
			.commandBufferCount = 1,
			.pCommandBuffers = &tempCmdBuf,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr
		};
		vkQueueSubmit(swapchain.queues.graphicsQueues[0], 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(swapchain.queues.graphicsQueues[0]);

		vkDestroyCommandPool(swapchain.queues.backend.device, tempPool, nullptr);
	}
}