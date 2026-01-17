#include "../headers/Memory.h"
#include "../../geometry/headers/Transformation.h"
#include <glm/glm.hpp>
#include <iostream>

namespace Vulkan {
	Memory::Memory(Swapchain&& salvageSwapchain) :
	isSalvagedRemains{ false },
	FLIGHT_COUNT{ 3 },
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
	verticesBuffer{ VK_NULL_HANDLE },
	indicesBuffer{ VK_NULL_HANDLE },
	stagedVertices{ VK_NULL_HANDLE },
	stagedIndices{ VK_NULL_HANDLE },

	descriptorSetLayout{ VK_NULL_HANDLE },
	descriptorPool{ VK_NULL_HANDLE },
	descriptorSets(swapchain.queues.graphicsQueues.size() * FLIGHT_COUNT),
	uniformBuffers(swapchain.queues.graphicsQueues.size() * FLIGHT_COUNT),
	uniformBuffersRequirements(uniformBuffers.size()),
	uniformBuffersOffsets(uniformBuffers.size()),
	uniformBuffersAddresses(uniformBuffers.size()),
	uniformBufferBindingNum{ 0 },
	descriptorSetLayoutBinding{ Geometry::Transformation::getDescriptorSetLayoutBinding(uniformBufferBindingNum, 1) } {
		std::cout << "---CREATING MEMORY...---\n";

		setupBuffersAndMemory();
		setupDescriptors();
	}

	Memory::Memory(Memory&& salvageMemory) :
		isSalvagedRemains{ false },
		FLIGHT_COUNT{ salvageMemory.FLIGHT_COUNT },
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
		verticesBuffer{ salvageMemory.verticesBuffer },
		indicesBuffer{ salvageMemory.indicesBuffer },
		stagedVertices{ salvageMemory.stagedVertices },
		stagedIndices{ salvageMemory.stagedIndices },

		descriptorSetLayout{ salvageMemory.descriptorSetLayout },
		descriptorPool{ salvageMemory.descriptorPool },
		descriptorSets{ salvageMemory.descriptorSets },
		uniformBuffers{ salvageMemory.uniformBuffers },
		uniformBuffersRequirements{ salvageMemory.uniformBuffersRequirements },
		uniformBuffersOffsets{ salvageMemory.uniformBuffersOffsets },
		uniformBuffersAddresses{ salvageMemory.uniformBuffersAddresses },
		uniformBufferBindingNum{ salvageMemory.uniformBufferBindingNum },
		descriptorSetLayoutBinding{ salvageMemory.descriptorSetLayoutBinding } {
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
		salvageMemory.descriptorSets = {};
		salvageMemory.uniformBuffers = {};
		salvageMemory.uniformBuffersRequirements = {};
		salvageMemory.uniformBuffersOffsets = {};
		salvageMemory.uniformBuffersAddresses = {};
		salvageMemory.uniformBufferBindingNum = 0xFFFFFFFF;
		salvageMemory.descriptorSetLayoutBinding = {};
		
		std::cout << "---MOVED MEMORY---\n";
	}

	Memory::~Memory() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING MEMORY...---\n";

			vkDestroyBuffer(swapchain.queues.backend.device, verticesBuffer, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, indicesBuffer, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, stagedVertices, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, stagedIndices, nullptr);

			for(int i = 0; i < uniformBuffers.size(); i++) {
				vkDestroyBuffer(swapchain.queues.backend.device, uniformBuffers[i], nullptr);
			}

			vkDestroyDescriptorSetLayout(swapchain.queues.backend.device, descriptorSetLayout, nullptr);
			vkFreeDescriptorSets(swapchain.queues.backend.device, descriptorPool, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data());
			vkDestroyDescriptorPool(swapchain.queues.backend.device, descriptorPool, nullptr);

			vkFreeMemory(swapchain.queues.backend.device, gpuMemory, nullptr);
			vkFreeMemory(swapchain.queues.backend.device, stagingMemory, nullptr);
		}
	}

	void Memory::setupBuffersAndMemory() {
		createVerticesBuffer();
		createIndicesBuffer();
		createUniformBuffers();
		createStagedVertices();
		createStagedIndices();
		allocateGPUMemory();
		allocateStagingMemory();
		bindUniformBuffersToStagingMemory();
		populateVerticesBuffer();
		populateIndicesBuffer();
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

	void Memory::createUniformBuffers() {
		for(int i = 0; i < uniformBuffers.size(); i++) {
			createBuffer(uniformBuffers[i], sizeof(Geometry::Transformation), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
			vkGetBufferMemoryRequirements(swapchain.queues.backend.device, uniformBuffers[i], &uniformBuffersRequirements[i]);
		}

		std::cout << "Created " << uniformBuffers.size() << " uniform buffers with size " << sizeof(Geometry::Transformation) << '\n';
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
		std::vector<VkDeviceSize> gpuBufferSizeRequirements{ verticesBufferRequirements.size, indicesBufferRequirements.size };

		std::vector<VkDeviceSize> gpuBufferAlignments{ verticesBufferRequirements.alignment, indicesBufferRequirements.alignment };

		std::vector<VkDeviceSize> offsets(gpuBufferAlignments.size());

		VkDeviceSize allocationSize = calculateAllocationSize(gpuBufferSizeRequirements, gpuBufferAlignments, offsets);
		verticesBufferOffset = offsets[0];
		indicesBufferOffset = offsets[1];
		
		uint32_t validMemoryTypes = verticesBufferRequirements.memoryTypeBits & indicesBufferRequirements.memoryTypeBits;
		VkMemoryPropertyFlags neededMemoryType = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		uint32_t memoryTypeIndex = getMemoryTypeIndex(validMemoryTypes, neededMemoryType);
		allocateMemory(gpuMemory, allocationSize, memoryTypeIndex);

		std::cout << "Allocated gpu memory with size " << allocationSize << " and memory type " << memoryTypeIndex << '\n';
	}

	void Memory::allocateStagingMemory() {
		std::vector<VkDeviceSize> stagingBufferSizeRequirements = { stagedVerticesRequirements.size, stagedIndicesRequirements.size };
		for(int i = 0; i < uniformBuffers.size(); i++) {
			stagingBufferSizeRequirements.push_back(uniformBuffersRequirements[i].size);
		}

		std::vector<VkDeviceSize> stagingBufferAlignments = { stagedVerticesRequirements.alignment , stagedIndicesRequirements.alignment };
		for (int i = 0; i < uniformBuffers.size(); i++) {
			stagingBufferAlignments.push_back(uniformBuffersRequirements[i].alignment);
		}

		std::vector<VkDeviceSize> offsets(stagingBufferAlignments.size());

		VkDeviceSize allocationSize = calculateAllocationSize(stagingBufferSizeRequirements, stagingBufferAlignments, offsets);
		stagedVerticesOffset = offsets[0];
		stagedIndicesOffset = offsets[1];
		offsets.erase(offsets.begin());
		offsets.erase(offsets.begin());
		uniformBuffersOffsets = offsets;

		uint32_t validMemoryTypes = stagedVerticesRequirements.memoryTypeBits & stagedIndicesRequirements.memoryTypeBits & uniformBuffersRequirements[0].memoryTypeBits;
		VkMemoryPropertyFlags neededMemoryType = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		uint32_t memoryTypeIndex = getMemoryTypeIndex(validMemoryTypes, neededMemoryType);
		allocateMemory(stagingMemory, allocationSize, memoryTypeIndex);

		std::cout << "Allocated staging memory with size " << allocationSize << " and memory type " << memoryTypeIndex << '\n';
	}

	void Memory::bindUniformBuffersToStagingMemory() {
		for(int i = 0; i < uniformBuffers.size(); i++) {
			if (vkBindBufferMemory(swapchain.queues.backend.device, uniformBuffers[i], stagingMemory, uniformBuffersOffsets[i]) != VK_SUCCESS) {
				throw std::runtime_error("Binding uniform buffers to staging memory failure");
			}
		}
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

	void Memory::setupDescriptors() {
		mapUniformBuffers();
		createDescriptorSetLayout();
		createDescriptorPool();
		createDescriptorSet();
		uniformBuffersToDescriptors();
	}

	void Memory::mapUniformBuffers() {
		for(int i = 0; i < uniformBuffers.size(); i++) {
			vkMapMemory(swapchain.queues.backend.device, stagingMemory, uniformBuffersOffsets[i], sizeof(Geometry::Transformation), 0, &uniformBuffersAddresses[i]);
			vkUnmapMemory(swapchain.queues.backend.device, stagingMemory);
		}
	}

	void Memory::createDescriptorSetLayout() {
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = 1,
			.pBindings = &descriptorSetLayoutBinding
		};

		if(vkCreateDescriptorSetLayout(swapchain.queues.backend.device, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) == VK_SUCCESS) {
			std::cout << "Descriptor set layout created\n";
		} else {
			throw std::runtime_error("Descriptor set layout creation failure");
		}
	}

	void Memory::createDescriptorPool() {
		VkDescriptorPoolSize uniformBufferDescriptors = {
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast<uint32_t>(uniformBuffers.size())
		};

		VkDescriptorPoolCreateInfo descriptorPoolInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets = static_cast<uint32_t>(uniformBuffers.size()),
			.poolSizeCount = 1,
			.pPoolSizes = &uniformBufferDescriptors
		};

		if(vkCreateDescriptorPool(swapchain.queues.backend.device, &descriptorPoolInfo, nullptr, &descriptorPool) == VK_SUCCESS) {
			std::cout << "Descriptor pool created\n";
		} else {
			throw std::runtime_error("Descriptor pool creation failure");
		}
	}

	void Memory::createDescriptorSet() {
		std::vector<VkDescriptorSetLayout> layoutsDuplicated(uniformBuffers.size(), descriptorSetLayout);
		VkDescriptorSetAllocateInfo descriptorSetInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = descriptorPool,
			.descriptorSetCount = static_cast<uint32_t>(uniformBuffers.size()),
			.pSetLayouts = layoutsDuplicated.data()
		};

		if(vkAllocateDescriptorSets(swapchain.queues.backend.device, &descriptorSetInfo, descriptorSets.data()) == VK_SUCCESS) {
			std::cout << "Descriptor sets created\n";
		} else {
			throw std::runtime_error("Descriptor sets creation failure");
		}
	}

	void Memory::uniformBuffersToDescriptors() {
		for(int i = 0; i < uniformBuffers.size(); i++) {
			VkDescriptorBufferInfo bufferInfo = {
				.buffer = uniformBuffers[i],
				.offset = 0,
				.range = VK_WHOLE_SIZE
			};

			VkWriteDescriptorSet writeInfo = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = descriptorSets[i],
				.dstBinding = uniformBufferBindingNum,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pImageInfo = nullptr,
				.pBufferInfo = &bufferInfo,
				.pTexelBufferView = nullptr
			};

			vkUpdateDescriptorSets(swapchain.queues.backend.device, 1, &writeInfo, 0, nullptr);
		}
	}

	uint32_t Memory::getMemoryTypeIndex(uint32_t memoryRequirementsMask, VkMemoryPropertyFlags propertyMask) {
		VkPhysicalDeviceMemoryProperties2 memoryProperties = {
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