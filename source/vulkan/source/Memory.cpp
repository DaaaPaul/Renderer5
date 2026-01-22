#include "../headers/Memory.h"
#include "../../geometry/headers/Transformation.h"
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <iostream>

namespace Vulkan {
	Memory::Memory(Swapchain&& salvageSwapchain) :
	isSalvagedRemains{ false },
	FLIGHT_COUNT{ 3 },
	vertices{},
	indices{},
	graphicsQueueFamilyIndex{ swapchain.queues.backend.graphicsFamilyIndex },
	verticesBufferRequirements{},
	verticesBufferOffset{},
	verticesBufferSize{},
	indicesBufferRequirements{},
	indicesBufferOffset{},
	indicesBufferSize{},
	stagedVerticesRequirements{},
	stagedVerticesOffset{},
	stagedVerticesSize{},
	stagedIndicesRequirements{},
	stagedIndicesOffset{},
	stagedIndicesSize{},

	swapchain{ std::move(salvageSwapchain) },
	stagingMemory{ VK_NULL_HANDLE },
	gpuMemory{ VK_NULL_HANDLE },
	verticesBuffer{ VK_NULL_HANDLE },
	indicesBuffer{ VK_NULL_HANDLE },
	stagedVertices{ VK_NULL_HANDLE },
	stagedIndices{ VK_NULL_HANDLE },

	u_descriptorSetLayout{ VK_NULL_HANDLE },
	u_descriptorPool{ VK_NULL_HANDLE },
	u_descriptorSets(swapchain.queues.graphicsQueues.size() * FLIGHT_COUNT),
	u_descriptorSetLayoutBinding{ Geometry::Transformation::getDescriptorSetLayoutBinding(uniformBufferBindingNum, 1) },
	uniformBuffers(swapchain.queues.graphicsQueues.size() * FLIGHT_COUNT),
	uniformBuffersRequirements(uniformBuffers.size()),
	uniformBuffersOffsets(uniformBuffers.size()),
	uniformBuffersAddresses(uniformBuffers.size()),
	uniformBufferBindingNum{ 0 },

	textureAddress{},
	textureWidth{},
	textureHeight{},
	textureSize{},
	mipmapLevels{},
	stagedTexture{},
	stagedTextureRequirements{},
	stagedTextureOffset{},
	textureImage{},
	textureImageView{},
	textureImageRequirements{},
	textureImageOffset{},

	textureSampler{},
	t_descriptorSetLayout{},
	t_descriptorPool{},
	t_descriptorSets(swapchain.queues.graphicsQueues.size() * FLIGHT_COUNT),
	t_descriptorSetLayoutBinding{},

	depthImage{},
	depthImageView{},
	depthImageRequirements{},
	depthImageOffset{} {
		std::cout << "---CREATING MEMORY...---\n";
		
		setupModel();
		setupDepthImage();
		setupTextureImage();
		setupBuffersAndMemory();
		setup_u_Descriptors();
		createDepthImageView();
		createTextureImageView();
		setupSampler();
		setup_t_Descriptors();
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

		u_descriptorSetLayout{ salvageMemory.u_descriptorSetLayout },
		u_descriptorPool{ salvageMemory.u_descriptorPool },
		u_descriptorSets{ salvageMemory.u_descriptorSets },
		u_descriptorSetLayoutBinding{ salvageMemory.u_descriptorSetLayoutBinding },
		uniformBuffers{ salvageMemory.uniformBuffers },
		uniformBuffersRequirements{ salvageMemory.uniformBuffersRequirements },
		uniformBuffersOffsets{ salvageMemory.uniformBuffersOffsets },
		uniformBuffersAddresses{ salvageMemory.uniformBuffersAddresses },
		uniformBufferBindingNum{ salvageMemory.uniformBufferBindingNum },
	
		textureAddress{ salvageMemory.textureAddress },
		textureWidth{ salvageMemory.textureWidth },
		textureHeight{ salvageMemory.textureHeight},
		textureSize{ salvageMemory.textureSize },
		mipmapLevels{ salvageMemory.mipmapLevels },
		stagedTexture{ salvageMemory.stagedTexture },
		stagedTextureRequirements{ salvageMemory.stagedTextureRequirements },
		stagedTextureOffset{ salvageMemory.stagedTextureOffset },
		textureImage{ salvageMemory.textureImage },
		textureImageView{ salvageMemory.textureImageView },
		textureImageRequirements{ salvageMemory.textureImageRequirements },
		textureImageOffset{ salvageMemory.textureImageOffset },
		
		textureSampler{ salvageMemory.textureSampler },
		t_descriptorSetLayout{ salvageMemory.t_descriptorSetLayout },
		t_descriptorPool{ salvageMemory.t_descriptorPool },
		t_descriptorSets{ salvageMemory.t_descriptorSets },
		t_descriptorSetLayoutBinding{ salvageMemory.t_descriptorSetLayoutBinding },

		depthImage{ salvageMemory.depthImage },
		depthImageView{ salvageMemory.depthImageView },
		depthImageRequirements{ salvageMemory.depthImageRequirements },
		depthImageOffset{ salvageMemory.depthImageOffset }
		{
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

		salvageMemory.u_descriptorSetLayout = VK_NULL_HANDLE;
		salvageMemory.u_descriptorPool = VK_NULL_HANDLE;
		salvageMemory.u_descriptorSets = {};
		salvageMemory.u_descriptorSetLayoutBinding = {};
		salvageMemory.uniformBuffers = {};
		salvageMemory.uniformBuffersRequirements = {};
		salvageMemory.uniformBuffersOffsets = {};
		salvageMemory.uniformBuffersAddresses = {};
		salvageMemory.uniformBufferBindingNum = 0xFFFFFFFF;

		salvageMemory.textureAddress = {};
		salvageMemory.textureWidth = {};
		salvageMemory.textureHeight = {};
		salvageMemory.textureSize = {};
		salvageMemory.mipmapLevels = {};
		salvageMemory.stagedTexture = {};
		salvageMemory.stagedTextureRequirements = {};
		salvageMemory.stagedTextureOffset = {};
		salvageMemory.textureImage = {};
		salvageMemory.textureImageView = {};
		salvageMemory.textureImageRequirements = {};
		salvageMemory.textureImageOffset = {};

		salvageMemory.textureSampler = {};
		salvageMemory.t_descriptorSetLayout = {};
		salvageMemory.t_descriptorPool = {};
		salvageMemory.t_descriptorSets = {};
		salvageMemory.t_descriptorSetLayoutBinding = {};

		salvageMemory.depthImage = {};
		salvageMemory.depthImageView = {};
		salvageMemory.depthImageRequirements = {};
		salvageMemory.depthImageOffset = {};
		
		std::cout << "---MOVED MEMORY---\n";
	}

	Memory::~Memory() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING MEMORY...---\n";

			vkDestroyImageView(swapchain.queues.backend.device, depthImageView, nullptr);
			vkDestroyImage(swapchain.queues.backend.device, depthImage, nullptr);
			vkDestroyImageView(swapchain.queues.backend.device, textureImageView, nullptr);
			vkDestroyImage(swapchain.queues.backend.device, textureImage, nullptr);
			vkDestroySampler(swapchain.queues.backend.device, textureSampler, nullptr);

			vkDestroyBuffer(swapchain.queues.backend.device, verticesBuffer, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, indicesBuffer, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, stagedVertices, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, stagedIndices, nullptr);
			vkDestroyBuffer(swapchain.queues.backend.device, stagedTexture, nullptr);

			for(int i = 0; i < uniformBuffers.size(); i++) {
				vkDestroyBuffer(swapchain.queues.backend.device, uniformBuffers[i], nullptr);
			}

			vkDestroyDescriptorSetLayout(swapchain.queues.backend.device, u_descriptorSetLayout, nullptr);
			vkFreeDescriptorSets(swapchain.queues.backend.device, u_descriptorPool, static_cast<uint32_t>(u_descriptorSets.size()), u_descriptorSets.data());
			vkDestroyDescriptorPool(swapchain.queues.backend.device, u_descriptorPool, nullptr);

			vkDestroyDescriptorSetLayout(swapchain.queues.backend.device, t_descriptorSetLayout, nullptr);
			vkFreeDescriptorSets(swapchain.queues.backend.device, t_descriptorPool, static_cast<uint32_t>(t_descriptorSets.size()), t_descriptorSets.data());
			vkDestroyDescriptorPool(swapchain.queues.backend.device, t_descriptorPool, nullptr);

			vkFreeMemory(swapchain.queues.backend.device, gpuMemory, nullptr);
			vkFreeMemory(swapchain.queues.backend.device, stagingMemory, nullptr);
		}
	}

	void Memory::setupModel() {
		tinyobj::attrib_t vertexAttributes{};
		std::vector<tinyobj::shape_t> meshes{};
		std::string warnings{}, errors{}, path{ "resources/models/Skull.obj" };

		if(tinyobj::LoadObj(&vertexAttributes, &meshes, nullptr, &warnings, &errors, path.c_str())) {
			std::cout << "Sucessfully loaded .obj model at " << path << '\n';;
		} else {
			throw std::runtime_error("Load model at " + path + " failure");
		}

		for(tinyobj::shape_t const& mesh : meshes) {
			for(tinyobj::index_t const& index : mesh.mesh.indices) {
				Geometry::Vertex vertex = {
					.position = { 
						vertexAttributes.vertices[3 * index.vertex_index + 0],
						vertexAttributes.vertices[3 * index.vertex_index + 1],
						vertexAttributes.vertices[3 * index.vertex_index + 2],
						1.0f
					},

					.color = {1.0f, 1.0f, 1.0f, 1.0f},

					.textureCoordinate = {
						vertexAttributes.texcoords[2 * index.texcoord_index + 0],
						1.0f - vertexAttributes.texcoords[2 * index.texcoord_index + 1],
					}
				};

				indices.push_back(indices.size());
				vertices.push_back(vertex);
			}
		}

		verticesBufferSize = sizeof(Geometry::Vertex) * vertices.size();
		indicesBufferSize = sizeof(uint32_t) * indices.size();
		stagedVerticesSize = sizeof(Geometry::Vertex) * vertices.size();
		stagedIndicesSize = sizeof(uint32_t) * indices.size();
	}

	void Memory::setupDepthImage() {
		createDepthImage();
	}

	void Memory::setupTextureImage() {
		loadTexture();
		createTextureImage();
	}

	void Memory::setupBuffersAndMemory() {
		createVerticesBuffer();
		createIndicesBuffer();
		createUniformBuffers();
		createTextureBuffer();
		createStagedVertices();
		createStagedIndices();
		allocateGPUMemory();
		allocateStagingMemory();
		bindUniformBuffersToStagingMemory();
		bindTextureImageToGpuMemory();
		bindDepthImageToGpuMemory();
		populateVerticesBuffer();
		populateIndicesBuffer();
		populateTextureImage();
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

	void Memory::createTextureBuffer() {
		createBuffer(stagedTexture, textureSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		vkGetBufferMemoryRequirements(swapchain.queues.backend.device, stagedTexture, &stagedTextureRequirements);

		std::cout << "Created staged textures buffer with size " << textureSize <<
			" requiring size of " << stagedTextureRequirements.size <<
			" and offsetted at " << stagedTextureRequirements.alignment << '\n';
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
		std::vector<VkDeviceSize> gpuBufferSizeRequirements{ 
			verticesBufferRequirements.size, 
			indicesBufferRequirements.size, 
			textureImageRequirements.size,
			depthImageRequirements.size
		};

		std::vector<VkDeviceSize> gpuBufferAlignments{ 
			verticesBufferRequirements.alignment, 
			indicesBufferRequirements.alignment, 
			textureImageRequirements.alignment,
			depthImageRequirements.alignment
		};

		std::vector<VkDeviceSize> offsets(gpuBufferAlignments.size());

		VkDeviceSize allocationSize = calculateAllocationSize(gpuBufferSizeRequirements, gpuBufferAlignments, offsets);
		verticesBufferOffset = offsets[0];
		indicesBufferOffset = offsets[1];
		textureImageOffset = offsets[2];
		depthImageOffset = offsets[3];
		
		uint32_t validMemoryTypes = 
			verticesBufferRequirements.memoryTypeBits & 
			indicesBufferRequirements.memoryTypeBits & 
			textureImageRequirements.memoryTypeBits &
			depthImageRequirements.memoryTypeBits;
		VkMemoryPropertyFlags neededMemoryType = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		uint32_t memoryTypeIndex = getMemoryTypeIndex(validMemoryTypes, neededMemoryType);
		allocateMemory(gpuMemory, allocationSize, memoryTypeIndex);

		std::cout << "Allocated gpu memory with size " << allocationSize << " and memory type " << memoryTypeIndex << '\n';
	}

	void Memory::allocateStagingMemory() {
		std::vector<VkDeviceSize> stagingBufferSizeRequirements = { stagedVerticesRequirements.size, stagedIndicesRequirements.size, stagedTextureRequirements.size };
		for(int i = 0; i < uniformBuffers.size(); i++) {
			stagingBufferSizeRequirements.push_back(uniformBuffersRequirements[i].size);
		}


		std::vector<VkDeviceSize> stagingBufferAlignments = { stagedVerticesRequirements.alignment , stagedIndicesRequirements.alignment, stagedTextureRequirements.alignment };
		for (int i = 0; i < uniformBuffers.size(); i++) {
			stagingBufferAlignments.push_back(uniformBuffersRequirements[i].alignment);
		}

		std::vector<VkDeviceSize> offsets(stagingBufferAlignments.size());

		VkDeviceSize allocationSize = calculateAllocationSize(stagingBufferSizeRequirements, stagingBufferAlignments, offsets);
		stagedVerticesOffset = offsets[0];
		stagedIndicesOffset = offsets[1];
		stagedTextureOffset = offsets[2];
		offsets.erase(offsets.begin());
		offsets.erase(offsets.begin());
		offsets.erase(offsets.begin());
		uniformBuffersOffsets = offsets;

		uint32_t validMemoryTypes = stagedVerticesRequirements.memoryTypeBits & stagedIndicesRequirements.memoryTypeBits & stagedTextureRequirements.memoryTypeBits & uniformBuffersRequirements[0].memoryTypeBits;
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

	void Memory::bindTextureImageToGpuMemory() {
		vkBindImageMemory(swapchain.queues.backend.device, textureImage, gpuMemory, textureImageOffset);
	}

	void Memory::bindDepthImageToGpuMemory() {
		vkBindImageMemory(swapchain.queues.backend.device, depthImage, gpuMemory, depthImageOffset);
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

	void Memory::populateTextureImage() {
		vkBindBufferMemory(swapchain.queues.backend.device, stagedTexture, stagingMemory, stagedTextureOffset);

		void* stagedTextureAddress = nullptr;
		vkMapMemory(swapchain.queues.backend.device, stagingMemory, stagedTextureOffset, textureSize, 0, &stagedTextureAddress);
		memcpy(stagedTextureAddress, textureAddress, textureSize);
		vkUnmapMemory(swapchain.queues.backend.device, stagingMemory);

		stbi_image_free(textureAddress);

		copyBufferToImage(stagedTexture, textureImage, VkExtent2D(textureWidth, textureHeight), 0);
		generateImageMipmaps(textureImage);
	}

	void Memory::setup_u_Descriptors() {
		mapUniformBuffers();
		create_u_DescriptorSetLayout();
		create_u_DescriptorPool();
		create_u_DescriptorSets();
		uniformBuffersToDescriptors();
	}

	void Memory::mapUniformBuffers() {
		for(int i = 0; i < uniformBuffers.size(); i++) {
			vkMapMemory(swapchain.queues.backend.device, stagingMemory, uniformBuffersOffsets[i], sizeof(Geometry::Transformation), 0, &uniformBuffersAddresses[i]);
			vkUnmapMemory(swapchain.queues.backend.device, stagingMemory);
		}
	}

	void Memory::create_u_DescriptorSetLayout() {
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = 1,
			.pBindings = &u_descriptorSetLayoutBinding
		};

		if(vkCreateDescriptorSetLayout(swapchain.queues.backend.device, &descriptorSetLayoutInfo, nullptr, &u_descriptorSetLayout) == VK_SUCCESS) {
			std::cout << "Descriptor set layout created\n";
		} else {
			throw std::runtime_error("Descriptor set layout creation failure");
		}
	}

	void Memory::create_u_DescriptorPool() {
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

		if(vkCreateDescriptorPool(swapchain.queues.backend.device, &descriptorPoolInfo, nullptr, &u_descriptorPool) == VK_SUCCESS) {
			std::cout << "Descriptor pool created\n";
		} else {
			throw std::runtime_error("Descriptor pool creation failure");
		}
	}

	void Memory::create_u_DescriptorSets() {
		std::vector<VkDescriptorSetLayout> layoutsDuplicated(uniformBuffers.size(), u_descriptorSetLayout);
		VkDescriptorSetAllocateInfo descriptorSetInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = u_descriptorPool,
			.descriptorSetCount = static_cast<uint32_t>(uniformBuffers.size()),
			.pSetLayouts = layoutsDuplicated.data()
		};

		if(vkAllocateDescriptorSets(swapchain.queues.backend.device, &descriptorSetInfo, u_descriptorSets.data()) == VK_SUCCESS) {
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
				.dstSet = u_descriptorSets[i],
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

	void Memory::loadTexture() {
		textureAddress = stbi_load(R"(resources/textures/Skull.jpg)", &textureWidth, &textureHeight, nullptr, STBI_rgb_alpha);
		textureSize = textureWidth * textureHeight * 4;

		if (textureAddress == nullptr) {
			throw std::runtime_error("Texture loading failure");
		}

		mipmapLevels = std::floor(std::log2(std::max(textureWidth, textureHeight))) + 1;
	}

	void Memory::createTextureImage() {
		createImage(textureImage, swapchain.swapchainInfo.imageFormat, VkExtent3D(textureWidth, textureHeight, 1), mipmapLevels, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
		vkGetImageMemoryRequirements(swapchain.queues.backend.device, textureImage, &textureImageRequirements);
	}

	void Memory::createTextureImageView() {
		createImageView(textureImageView, textureImage, swapchain.swapchainInfo.imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipmapLevels);
	}

	void Memory::setupSampler() {
		VkPhysicalDeviceProperties prop{};
		vkGetPhysicalDeviceProperties(swapchain.queues.backend.physicalDevice, &prop);

		VkSamplerCreateInfo textureSamplerInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias = 0.0f,
			.anisotropyEnable = true,
			.maxAnisotropy = prop.limits.maxSamplerAnisotropy,
			.compareEnable = false,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			.unnormalizedCoordinates = false,
		};

		if(vkCreateSampler(swapchain.queues.backend.device, &textureSamplerInfo, nullptr, &textureSampler) == VK_SUCCESS) {
			std::cout << "Texture sampler created\n";
		} else {
			throw std::runtime_error("Texture sampler creation failure");
		}
	}

	void Memory::setup_t_Descriptors() {
		create_t_DescriptorSetLayout();
		create_t_DescriptorPool();
		create_t_DescriptorSets();
		samplerToDescriptors();
	}

	void Memory::create_t_DescriptorSetLayout() {
		VkDescriptorSetLayoutBinding textureBinding = {
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = nullptr,
		};

		VkDescriptorSetLayoutCreateInfo textureDescriptorSetLayoutInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = 1,
			.pBindings = &textureBinding,
		};

		if(vkCreateDescriptorSetLayout(swapchain.queues.backend.device, &textureDescriptorSetLayoutInfo, nullptr, &t_descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("Create texture descriptor set layout failure");
		}
	}

	void Memory::create_t_DescriptorPool() {
		VkDescriptorPoolSize poolNeededSize = {
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = static_cast<uint32_t>(t_descriptorSets.size())
		};

		VkDescriptorPoolCreateInfo textureDescriptorPoolInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets = static_cast<uint32_t>(t_descriptorSets.size()),
			.poolSizeCount = 1,
			.pPoolSizes = &poolNeededSize
		};

		if (vkCreateDescriptorPool(swapchain.queues.backend.device, &textureDescriptorPoolInfo, nullptr, &t_descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("Create texture descriptor pool failure");
		}
	}

	void Memory::create_t_DescriptorSets() {
		std::vector<VkDescriptorSetLayout> layoutsDuplicated(t_descriptorSets.size(), t_descriptorSetLayout);

		VkDescriptorSetAllocateInfo descriptorSetInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = t_descriptorPool,
			.descriptorSetCount = static_cast<uint32_t>(t_descriptorSets.size()),
			.pSetLayouts = layoutsDuplicated.data()
		};

		if (vkAllocateDescriptorSets(swapchain.queues.backend.device, &descriptorSetInfo, t_descriptorSets.data()) == VK_SUCCESS) {
			std::cout << "Texture descriptor sets created\n";
		} else {
			throw std::runtime_error("Descriptor sets creation failure");
		}
	}

	void Memory::samplerToDescriptors() {
		for (int i = 0; i < t_descriptorSets.size(); i++) {
			VkDescriptorImageInfo imageInfo = {
				.sampler = textureSampler,
				.imageView = textureImageView,
				.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			};

			VkWriteDescriptorSet writeInfo = {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.pNext = nullptr,
				.dstSet = t_descriptorSets[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.pImageInfo = &imageInfo,
				.pBufferInfo = nullptr,
				.pTexelBufferView = nullptr
			};

			vkUpdateDescriptorSets(swapchain.queues.backend.device, 1, &writeInfo, 0, nullptr);
		}
	}

	void Memory::createDepthImage() {
		createImage(depthImage, VK_FORMAT_D32_SFLOAT, VkExtent3D(swapchain.swapchainInfo.imageExtent.width, swapchain.swapchainInfo.imageExtent.height, 1), 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
		vkGetImageMemoryRequirements(swapchain.queues.backend.device, depthImage, &depthImageRequirements);
	}

	void Memory::createDepthImageView() {
		createImageView(depthImageView, depthImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}

	void Memory::generateImageMipmaps(VkImage& image) {
		VkCommandPool cmdPool{};
		VkCommandBuffer cmdBuf{};

		allocateBeginOneTimeCommandBuffer(cmdBuf, cmdPool);
		
		// unique: reading from 0th mip level (the full texture)
		VkImageMemoryBarrier2 blitSourceBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcQueueFamilyIndex = graphicsQueueFamilyIndex,
			.dstQueueFamilyIndex = graphicsQueueFamilyIndex,
			.image = image,
			.subresourceRange = VkImageSubresourceRange{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		VkDependencyInfo blitBarrierInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &blitSourceBarrier,
		};

		int mipLevelWidth = textureWidth;
		int mipLevelHeight = textureHeight;

		for(uint32_t i = 1; i < mipmapLevels; i++) {
			if(i == 1) {
				blitSourceBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
			} else {
				blitSourceBarrier.srcStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
			}
			blitSourceBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
			blitSourceBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
			blitSourceBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
			blitSourceBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			blitSourceBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

			blitSourceBarrier.subresourceRange.baseMipLevel = i - 1;

			vkCmdPipelineBarrier2(cmdBuf, &blitBarrierInfo);

			VkImageBlit blitInfo = {
				.srcSubresource = VkImageSubresourceLayers{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel = i - 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				},
				.srcOffsets = { VkOffset3D(0, 0, 0), VkOffset3D(mipLevelWidth, mipLevelHeight, 1) },
				.dstSubresource = VkImageSubresourceLayers{
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel = i,
					.baseArrayLayer = 0,
					.layerCount = 1
				},
				.dstOffsets = { VkOffset3D(0, 0, 0), VkOffset3D((mipLevelWidth > 1) ? mipLevelWidth / 2 : 1, (mipLevelHeight > 1) ? mipLevelHeight / 2 : 1, 1) }
			};

			vkCmdBlitImage(cmdBuf,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blitInfo,
				VK_FILTER_LINEAR);

			blitSourceBarrier.srcStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
			blitSourceBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
			blitSourceBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
			blitSourceBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
			blitSourceBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			blitSourceBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			vkCmdPipelineBarrier2(cmdBuf, &blitBarrierInfo);

			if (mipLevelWidth > 1) {
				mipLevelWidth /= 2;
			}
			if (mipLevelHeight > 1) {
				mipLevelHeight /= 2;
			}
		}

		blitSourceBarrier.srcStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
		blitSourceBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
		blitSourceBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
		blitSourceBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
		blitSourceBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		blitSourceBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		blitSourceBarrier.subresourceRange.baseMipLevel = mipmapLevels - 1;
		vkCmdPipelineBarrier2(cmdBuf, &blitBarrierInfo);

		endSubmitFreeOneTimeCommandBuffer(cmdBuf, cmdPool);
	}

	void Memory::insertImageMemoryBarrier(VkCommandBuffer& cmdBuf, VkImage& image, uint32_t mipLevels, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 sourceStage, VkAccessFlags2 sourceAccess, VkPipelineStageFlags2 destStage, VkAccessFlags2 destAccess) {
		VkImageSubresourceRange memoryBarrierResourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = mipLevels,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		VkImageMemoryBarrier2 imageMemoryBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = sourceStage,
			.srcAccessMask = sourceAccess,
			.dstStageMask = destStage,
			.dstAccessMask = destAccess,
			.oldLayout = oldLayout,
			.newLayout = newLayout,
			.srcQueueFamilyIndex = graphicsQueueFamilyIndex,
			.dstQueueFamilyIndex = graphicsQueueFamilyIndex,
			.image = image,
			.subresourceRange = memoryBarrierResourceRange
		};

		VkDependencyInfo memoryBarriersInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &imageMemoryBarrier,
		};

		vkCmdPipelineBarrier2(cmdBuf, &memoryBarriersInfo);
	}

	void Memory::allocateBeginOneTimeCommandBuffer(VkCommandBuffer& cmdBuf, VkCommandPool& pool) {
		VkCommandPoolCreateInfo poolInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			.queueFamilyIndex = graphicsQueueFamilyIndex
		};
		if (vkCreateCommandPool(swapchain.queues.backend.device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
			throw std::runtime_error("Temporary command pool allocation failure");
		}

		VkCommandBufferAllocateInfo cmdBufInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = pool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		if (vkAllocateCommandBuffers(swapchain.queues.backend.device, &cmdBufInfo, &cmdBuf) != VK_SUCCESS) {
			throw std::runtime_error("One time command buffer allocation failure");
		}

		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr
		};
		vkBeginCommandBuffer(cmdBuf, &beginInfo);
	}

	void Memory::endSubmitFreeOneTimeCommandBuffer(VkCommandBuffer& cmdBuf, VkCommandPool& pool) {
		vkEndCommandBuffer(cmdBuf);

		VkSubmitInfo submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,
			.commandBufferCount = 1,
			.pCommandBuffers = &cmdBuf,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr
		};
		vkQueueSubmit(swapchain.queues.graphicsQueues[0], 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(swapchain.queues.graphicsQueues[0]);

		vkFreeCommandBuffers(swapchain.queues.backend.device, pool, 1, &cmdBuf);
		vkDestroyCommandPool(swapchain.queues.backend.device, pool, nullptr);
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
		VkCommandBuffer tmpCmdBuf{};
		VkCommandPool tmpPool{};
		allocateBeginOneTimeCommandBuffer(tmpCmdBuf, tmpPool);
		
		VkBufferCopy fullSize = {
			.srcOffset = 0,
			.dstOffset = 0,
			.size = sizeFromBeginning
		};
		vkCmdCopyBuffer(tmpCmdBuf, src, dst, 1, &fullSize);

		endSubmitFreeOneTimeCommandBuffer(tmpCmdBuf, tmpPool);
	}

	void Memory::copyBufferToImage(VkBuffer& src, VkImage& dst, VkExtent2D imageExtent, uint32_t mipLevel) {
		VkCommandPool tmpPool{};
		VkCommandBuffer tmpCmdBuf{};

		allocateBeginOneTimeCommandBuffer(tmpCmdBuf, tmpPool);
		insertImageMemoryBarrier(tmpCmdBuf, textureImage, mipmapLevels, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT);
	
		VkImageSubresourceLayers colorAspect = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = mipLevel,
			.baseArrayLayer = 0,
			.layerCount = 1,
		}; 
		VkBufferImageCopy packedSize = {
			.bufferOffset = 0,
			.bufferRowLength = 0, // tightly packed buffer accordingly with imageExtent
			.bufferImageHeight = 0, // tightly packed buffer accordingly with imageExtent
			.imageSubresource = colorAspect,
			.imageOffset = VkOffset3D(0, 0, 0),
			.imageExtent = VkExtent3D(imageExtent.width, imageExtent.height, 1)
		};
		vkCmdCopyBufferToImage(tmpCmdBuf, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &packedSize);

		endSubmitFreeOneTimeCommandBuffer(tmpCmdBuf, tmpPool);
	}

	void Memory::createImage(VkImage& image, VkFormat format, VkExtent3D extent, uint32_t mipLevels, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage) {
		VkImageCreateInfo imageInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = format,
			.extent = extent,
			.mipLevels = mipLevels,
			.arrayLayers = 1,
			.samples = samples,
			.tiling = tiling,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &graphicsQueueFamilyIndex,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};

		if (vkCreateImage(swapchain.queues.backend.device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("Image creation failure");
		}
	}

	void Memory::createImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspect, uint32_t mipLevels) {
		VkImageViewCreateInfo viewInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = format,
			.subresourceRange = VkImageSubresourceRange(aspect, 0, mipLevels, 0, 1)
		};

		if (vkCreateImageView(swapchain.queues.backend.device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("Texture image view creation failure");
		}
	}
}