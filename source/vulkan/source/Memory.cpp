#include "../headers/Memory.h"
#include "../../geometry/headers/Transformation.h"
#include <glm/glm.hpp>
#include <iostream>

namespace Vulkan {
	Memory::Memory(Queues* queues, MemoryCreateInfo const& createInfo) {
		std::cout << "---CREATING MEMORY...---\n";

	}

	Memory::Memory(Memory&& salvageMemory) {
		std::cout << "---MOVING MEMORY...---\n";
	}

	Memory::~Memory() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING MEMORY...---\n";

			clean();
		}
	}

	uint32_t Memory::getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask) {
		VkPhysicalDeviceMemoryProperties2 memoryProperties = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2
		};
		vkGetPhysicalDeviceMemoryProperties2(queues->backend.physicalDevice, &memoryProperties);

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

		if(vkAllocateMemory(queues->backend.device, &memoryAllocateInfo, nullptr, &memory) == VK_SUCCESS) {
			std::cout << "Allocated device memory with " << memoryAllocateInfo.allocationSize << " size and " << memoryAllocateInfo.memoryTypeIndex << " memory type\n";
		} else {
			throw std::runtime_error("Memory creation failure");
		}
	}

	VkDeviceSize Memory::calculateAllocationSize(std::vector<VkDeviceSize> bufferSizes, std::vector<VkDeviceSize> alignments, std::vector<VkDeviceSize>& offsetsToSet) {
		VkDeviceSize allocationSize = 0;
		uint32_t buffersCount = static_cast<uint32_t>(bufferSizes.size());

		for(int i = 0; i < buffersCount; i++) {
			while(allocationSize % alignments[i] != 0) {
				allocationSize++;
			}
			
			offsetsToSet[i] = allocationSize;

			allocationSize += bufferSizes[i];
		}

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
			.pQueueFamilyIndices = &queues->backend.graphicsFamilyIndex
		};

		if (vkCreateBuffer(queues->backend.device, &bufferInfo, nullptr, &buffer) == VK_SUCCESS) {
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
			.queueFamilyIndex = queues->backend.graphicsFamilyIndex
		};
		if (vkCreateCommandPool(queues->backend.device, &poolInfo, nullptr, &tempPool) != VK_SUCCESS) {
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
		vkAllocateCommandBuffers(queues->backend.device, &cmdBufInfo, &tempCmdBuf);

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
		vkQueueSubmit(queues->graphicsQueues[0], 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queues->graphicsQueues[0]);

		vkDestroyCommandPool(queues->backend.device, tempPool, nullptr);
	}
}