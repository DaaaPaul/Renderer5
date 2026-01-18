#include "../headers/Memory.h"
#include "../../geometry/headers/Transformation.h"
#include <glm/glm.hpp>
#include <iostream>

namespace Vulkan {
	Memory::Memory(Queues* queues, MemoryCreateInfo const& createInfo) : queues{ queues }, createInfo{ createInfo } {
		std::cout << "---CREATING MEMORY...---\n";

		setupBuffersAndMemory();
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

	void Memory::setupBuffersAndMemory() {
		createBufferTuples();
		createDeviceLocalMemory_O();
		createHostVisibleMemory_O_A();
	}

	void Memory::createBufferTuples() {
		for(int i = 0; i < createInfo.bufferCount; i++) {
			if(createInfo.pIsHostVisible[i]) {
				pushBackBuffer_R_S_B(i);
			} else {
				pushBackMappedBuffer_R_S_B(i);
			}
		}
	}

	void Memory::pushBackBuffer_R_S_B(uint32_t index) {
		void* bufferData = createInfo.ppBufferDatas[index];
		VkDeviceSize bufferSize = createInfo.pBufferSizes[index];
		VkBufferUsageFlags bufferUsage = createInfo.pBufferUsages[index];
		
		std::tuple<VkMemoryRequirements, VkDeviceSize, VkDeviceSize, VkBuffer> buffer_R_S_B{};

		std::get<2>(buffer_R_S_B) = bufferSize;
		std::get<3>(buffer_R_S_B) = getBuffer(bufferSize, bufferUsage);
		vkGetBufferMemoryRequirements(queues->backend.device, std::get<3>(buffer_R_S_B), &std::get<0>(buffer_R_S_B));

		buffers_R_O_S_B.push_back(buffer_R_S_B);
	}

	void Memory::pushBackMappedBuffer_R_S_B(uint32_t index) {
		void* bufferData = createInfo.ppBufferDatas[index];
		VkDeviceSize bufferSize = createInfo.pBufferSizes[index];
		VkBufferUsageFlags bufferUsage = createInfo.pBufferUsages[index];

		std::tuple<VkMemoryRequirements, VkDeviceSize, VkDeviceSize, VkBuffer, void*> mappedBuffer_R_S_B{};

		std::get<2>(mappedBuffer_R_S_B) = bufferSize;
		std::get<3>(mappedBuffer_R_S_B) = getBuffer(bufferSize, bufferUsage);
		vkGetBufferMemoryRequirements(queues->backend.device, std::get<3>(mappedBuffer_R_S_B), &std::get<0>(mappedBuffer_R_S_B));
		
		mappedBuffers_R_O_S_B_A.push_back(mappedBuffer_R_S_B);
	}

	void Memory::createDeviceLocalMemory_O() {
		VkDeviceSize allocationSize{};
		uint32_t allocationMemoryTypeIndex = UINT32_MAX;

		std::vector<VkDeviceSize> buffersRequiredSizes{};
		std::vector<VkDeviceSize> buffersRequiredAlignments{};
		uint32_t buffersCompatibleMemoryTypeIndices = UINT32_MAX;

		for(int i = 0; i < buffers_R_O_S_B.size(); i++) {
			buffersRequiredSizes.push_back(std::get<0>(buffers_R_O_S_B[i]).size);
			buffersRequiredAlignments.push_back(std::get<0>(buffers_R_O_S_B[i]).alignment);
			buffersCompatibleMemoryTypeIndices &= std::get<0>(buffers_R_O_S_B[i]).memoryTypeBits;
		}

		std::vector<VkDeviceSize> offsets(buffersRequiredSizes.size());
		allocationSize = getMemoryAllocationSize(buffersRequiredSizes, buffersRequiredAlignments, offsets);

		for(int i = 0; i < offsets.size(); i++) {
			std::get<1>(buffers_R_O_S_B[i]) = offsets[i];
		}

		allocationMemoryTypeIndex = getMemoryTypeIndex(buffersCompatibleMemoryTypeIndices, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		deviceLocalMemory = getMemory(allocationSize, allocationMemoryTypeIndex);
		std::cout << "Created device local memory with size " << allocationSize << " and type " << allocationMemoryTypeIndex << '\n';
	}

	void Memory::createHostVisibleMemory_O_A() {
		VkDeviceSize allocationSize{};
		uint32_t allocationMemoryTypeIndex = UINT32_MAX;

		std::vector<VkDeviceSize> mappedBuffersRequiredSizes{};
		std::vector<VkDeviceSize> mappedBuffersRequiredAlignments{};
		uint32_t mappedBuffersCompatibleMemoryTypeIndices = UINT32_MAX;

		for (int i = 0; i < mappedBuffers_R_O_S_B_A.size(); i++) {
			mappedBuffersRequiredSizes.push_back(std::get<0>(mappedBuffers_R_O_S_B_A[i]).size);
			mappedBuffersRequiredAlignments.push_back(std::get<0>(mappedBuffers_R_O_S_B_A[i]).alignment);
			mappedBuffersCompatibleMemoryTypeIndices &= std::get<0>(mappedBuffers_R_O_S_B_A[i]).memoryTypeBits;
		}

		std::vector<VkDeviceSize> offsets(mappedBuffersRequiredSizes.size());
		allocationSize = getMemoryAllocationSize(mappedBuffersRequiredSizes, mappedBuffersRequiredAlignments, offsets);

		for (int i = 0; i < offsets.size(); i++) {
			std::get<1>(mappedBuffers_R_O_S_B_A[i]) = offsets[i];
		}

		allocationMemoryTypeIndex = getMemoryTypeIndex(mappedBuffersCompatibleMemoryTypeIndices, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		hostVisibleMemory = getMemory(allocationSize, allocationMemoryTypeIndex);
		std::cout << "Created host visible memory with size " << allocationSize << " and type " << allocationMemoryTypeIndex << '\n';

		for(int i = 0; i < mappedBuffers_R_O_S_B_A.size(); i++) {
			vkMapMemory(queues->backend.device, hostVisibleMemory, offsets[i], std::get<2>(mappedBuffers_R_O_S_B_A[i]), 0, &std::get<4>(mappedBuffers_R_O_S_B_A[i]));
		}
	}

	VkDeviceMemory Memory::getMemory(VkDeviceSize byteSize, uint32_t memoryTypeIndex) {
		VkDeviceMemory memory{};

		VkMemoryAllocateInfo memoryAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = byteSize,
			.memoryTypeIndex = memoryTypeIndex
		};

		if (vkAllocateMemory(queues->backend.device, &memoryAllocateInfo, nullptr, &memory) != VK_SUCCESS) {
			throw std::runtime_error("Memory creation failure");
		}
		else {
			return memory;
		}
	}

	VkDeviceSize Memory::getMemoryAllocationSize(std::vector<VkDeviceSize> bufferSizes, std::vector<VkDeviceSize> alignments, std::vector<VkDeviceSize>& offsetsToSet) {
		VkDeviceSize allocationSize = 0;
		uint32_t buffersCount = static_cast<uint32_t>(bufferSizes.size());

		for (int i = 0; i < buffersCount; i++) {
			while (allocationSize % alignments[i] != 0) {
				allocationSize++;
			}

			offsetsToSet[i] = allocationSize;

			allocationSize += bufferSizes[i];
		}

		return allocationSize;
	}

	uint32_t Memory::getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask) {
		uint32_t index = UINT32_MAX;
		
		VkPhysicalDeviceMemoryProperties2 memoryProperties = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2
		};
		vkGetPhysicalDeviceMemoryProperties2(queues->backend.physicalDevice, &memoryProperties);

		for (int i = 0; i < memoryProperties.memoryProperties.memoryTypeCount; i++) {
			if ((memoryRequirementsMask & (1 << i)) && 
				((memoryProperties.memoryProperties.memoryTypes[i].propertyFlags & propertyMask) == propertyMask)) {
				index = i;
			}
		};

		if(index == UINT32_MAX) {
			throw std::runtime_error("No suitable memory type found\n");
		} else {
			return index;
		}
	}

	void Memory::populateBuffer(void* dataToPutIn, size_t bytesToPutIn, VkBuffer& dst) {

	}

	VkBuffer Memory::getBuffer(VkDeviceSize byteSize, VkBufferUsageFlags usage) {
		VkBuffer buffer{};
		
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

		if (vkCreateBuffer(queues->backend.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("Memory creation failure");
		} else {
			return buffer;
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