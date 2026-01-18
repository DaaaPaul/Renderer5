#pragma once

#include "Queues.h"
#include "../../geometry/headers/Vertex.h"
#include <tuple>

namespace Vulkan {
	struct MemoryCreateInfo {
		uint32_t bufferCount;
		VkDeviceSize* pBufferSizes;
		VkBufferUsageFlags* pBufferUsages;
		bool* pIsHostVisible;
	};

	struct Memory {
	public:
		bool isSalvagedRemains;

		Queues* queues;

		// memory and buffers
		VkDeviceMemory deviceLocalMemory;
		VkDeviceMemory hostVisibleMemory;
		std::vector<std::tuple<VkMemoryRequirements, VkDeviceSize, VkDeviceSize, VkBuffer>> buffers_R_O_S_B;
		std::vector<std::tuple<VkMemoryRequirements, VkDeviceSize, VkDeviceSize, VkBuffer, void*>> mappedBuffers_R_O_S_B_A;

		// setup utility
		void clean();

		// general utility
		uint32_t getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask);
		VkDeviceSize calculateAllocationSize(std::vector<VkDeviceSize> bufferSizes, std::vector<VkDeviceSize> alignments, std::vector<VkDeviceSize>& offsetsToSet);
		void allocateMemory(VkDeviceMemory& memoryToSet, VkDeviceSize byteSize, uint32_t memoryTypeIndex);

		void createBuffer(VkBuffer& bufferToSet, VkDeviceSize byteSize, VkBufferUsageFlags usage);
		void copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize sizeFromBeginning);

		Memory(Queues* queues, MemoryCreateInfo const& createInfo);
		Memory(Memory&& salvageMemory);
		~Memory();

		DELETE_COPYING(Memory);
	};
}