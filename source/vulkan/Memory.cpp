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

	void Memory::createVerticiesBuffer() {
		uint32_t verticiesBufferSize = triangleVerticies.size() * sizeof(triangleVerticies[0]);
		createBuffer(stagingBuffer, verticiesBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		
		VkMemoryRequirements stagingBufferMRequirements{};
		vkGetBufferMemoryRequirements(swapchain.queues.backend.device, stagingBuffer, &stagingBufferMRequirements);	
		allocateMemory(stagingBufferM, stagingBufferMRequirements.size, getMemoryTypeIndex(stagingBufferMRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		vkBindBufferMemory(swapchain.queues.backend.device, stagingBuffer, stagingBufferM, 0);

		void* stagingBufferMAddress = nullptr;
		vkMapMemory(swapchain.queues.backend.device, stagingBufferM, 0, verticiesBufferSize, 0, &stagingBufferMAddress);
		memcpy(stagingBufferMAddress, triangleVerticies.data(), verticiesBufferSize);
	}

	void Memory::createIndicesBuffer() {
	
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