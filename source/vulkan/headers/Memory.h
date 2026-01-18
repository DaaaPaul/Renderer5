#pragma once

#include "Queues.h"
#include "../../geometry/headers/Vertex.h"
#include <tuple>

namespace Vulkan {
	struct MemoryCreateInfo {
		uint32_t bufferCount;
		void** ppBufferDatas;
		size_t* pBufferDataSizes;
		VkDeviceSize* pBufferSizes;
		VkBufferUsageFlags* pBufferUsages;
		bool* pIsHostVisible;
	};

	struct Memory {
	public:
		bool isSalvagedRemains;

		Queues* queues;

		MemoryCreateInfo createInfo;

		// memory and buffers
		VkDeviceMemory deviceLocalMemory;
		VkDeviceMemory hostVisibleMemory;
		std::vector<std::tuple<VkMemoryRequirements, VkDeviceSize, VkDeviceSize, VkBuffer>> buffers_R_O_S_B;
		std::vector<std::tuple<VkMemoryRequirements, VkDeviceSize, VkDeviceSize, VkBuffer, void*>> mappedBuffers_R_O_S_B_A;

		// setup
		void setupBuffersAndMemory();
		void createBufferTuples();
		void createDeviceLocalMemory_O();
		void createHostVisibleMemory_O_A();

		void pushBackBuffer_R_S_B(uint32_t index);
		void pushBackMappedBuffer_R_S_B(uint32_t index);

		// setup utility
		void clean();

		// general utility
		VkDeviceMemory getMemory(VkDeviceSize byteSize, uint32_t memoryTypeIndex);
		VkDeviceSize getMemoryAllocationSize(std::vector<VkDeviceSize> bufferSizes, std::vector<VkDeviceSize> alignments, std::vector<VkDeviceSize>& offsetsToSet);
		uint32_t getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask);

		void populateBuffer(void* dataToPutIn, size_t bytesToPutIn, VkBuffer& dst);
		VkBuffer getBuffer(VkDeviceSize byteSize, VkBufferUsageFlags usage);
		void copyBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize sizeFromBeginning);

		Memory(Queues* queues, MemoryCreateInfo const& createInfo);
		Memory(Memory&& salvageMemory);
		~Memory();

		DELETE_COPYING(Memory);
	};
}