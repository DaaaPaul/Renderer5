#include "Memory.h"
#include <iostream>

namespace Vulkan {
	Memory::Memory(Swapchain&& salvageSwapchain) {
		std::cout << "---CREATING MEMORY...---";
	}

	Memory::Memory(Memory&& salvageMemory) {
	
		std::cout << "---MOVED MEMORY---";
	}

	Memory::~Memory() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING MEMORY...---";
		}
	}

	void Memory::createVerticesBuffer() {
		createBuffer(verticesBuffer, sizeof(vertices[0]) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		vkGetBufferMemoryRequirements(swapchain.queues.backend.device, verticesBuffer, &verticesBufferRequirements);

		std::cout << "Created vertices buffer with size " << sizeof(vertices[0]) * vertices.size() << " requiring size of " << verticesBufferRequirements.size << '\n';
	}

	void Memory::createIndicesBuffer() {
		createBuffer(indicesBuffer, sizeof(indices[0]) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		vkGetBufferMemoryRequirements(swapchain.queues.backend.device, indicesBuffer, &indicesBufferRequirements);

		std::cout << "Created indices buffer with size " << sizeof(indices[0]) * indices.size() << " requiring size of " << indicesBufferRequirements.size << '\n';
	}

	void Memory::allocateStagingMemory() {

	}

	void Memory::allocateGPUMemory() {

	}

	void Memory::initializeStagedVertices() {

	}

	void Memory::initializeStagedIndices() {

	}

	uint32_t Memory::getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask) {
		VkPhysicalDeviceMemoryProperties2 memoryProperties{};
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


	void Memory::copyBuffer() {

	}
}