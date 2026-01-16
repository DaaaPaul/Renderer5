#include "../headers/Sync.h"
#include <iostream>

namespace Vulkan {
	Sync::Sync(Pipeline&& salvagePipeline) :
		isSalvagedRemains{ false },
		GRAPHICS_QUEUE_COUNT{ static_cast<uint16_t>(salvagePipeline.memory.swapchain.queues.graphicsQueues.size()) },
		FLIGHT_COUNT{ 2 },
		imageReady(GRAPHICS_QUEUE_COUNT),
		imageFinished(GRAPHICS_QUEUE_COUNT),
		commandBufferFinished(GRAPHICS_QUEUE_COUNT),
		pipeline(std::move(salvagePipeline)) {
		assert(FLIGHT_COUNT == pipeline.memory.FLIGHT_COUNT);

		std::cout << "---CREATING SYNC OBJECTS...---\n";

		createSyncObjects();
	}

	Sync::Sync(Sync&& salvageSync) :
		isSalvagedRemains{ false },
		GRAPHICS_QUEUE_COUNT{ salvageSync.GRAPHICS_QUEUE_COUNT },
		FLIGHT_COUNT{ salvageSync.FLIGHT_COUNT },
		imageReady{ salvageSync.imageReady },
		imageFinished{ salvageSync.imageFinished },
		commandBufferFinished{ salvageSync.commandBufferFinished },
		pipeline(std::move(salvageSync.pipeline)) {
		salvageSync.isSalvagedRemains = true;

		salvageSync.imageReady = {};
		salvageSync.imageFinished = {};
		salvageSync.commandBufferFinished = {};

		std::cout << "---MOVED SYNC OBJECTS---\n";
	}

	Sync::~Sync() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING SYNC OBJECTS...---\n";

			for(int i = 0; i < GRAPHICS_QUEUE_COUNT; i++) {
				for(int j = 0; j < FLIGHT_COUNT; j++) {
					vkDestroySemaphore(pipeline.memory.swapchain.queues.backend.device, imageReady[i][j], nullptr);
					vkDestroySemaphore(pipeline.memory.swapchain.queues.backend.device, imageFinished[i][j], nullptr);
					vkDestroyFence(pipeline.memory.swapchain.queues.backend.device, commandBufferFinished[i][j], nullptr);
				}
			}
		}
	}

	void Sync::createSyncObjects() {
		createSemaphores();
		createFences();
	}

	void Sync::createSemaphores() {
		VkSemaphoreCreateInfo semaphoreInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};

		for(int i = 0; i < GRAPHICS_QUEUE_COUNT; i++) {
			imageReady[i].resize(FLIGHT_COUNT, VK_NULL_HANDLE);
			imageFinished[i].resize(FLIGHT_COUNT, VK_NULL_HANDLE);

			for(int j = 0; j < FLIGHT_COUNT; j++) {
				if (vkCreateSemaphore(pipeline.memory.swapchain.queues.backend.device, &semaphoreInfo, nullptr, &imageReady[i][j]) == VK_SUCCESS &&
					vkCreateSemaphore(pipeline.memory.swapchain.queues.backend.device, &semaphoreInfo, nullptr, &imageFinished[i][j]) == VK_SUCCESS) {
					std::cout << "Semaphore pair for queue " << i << " frame in flight " << j << " created\n";
				} else {
					throw std::runtime_error("Semaphore pair creation failure");
				}
			}
		}
	}

	void Sync::createFences() {
		VkFenceCreateInfo fenceInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};

		for (int i = 0; i < GRAPHICS_QUEUE_COUNT; i++) {
			commandBufferFinished[i].resize(FLIGHT_COUNT, VK_NULL_HANDLE);

			for (int j = 0; j < FLIGHT_COUNT; j++) {
				if (vkCreateFence(pipeline.memory.swapchain.queues.backend.device, &fenceInfo, nullptr, &commandBufferFinished[i][j]) == VK_SUCCESS) {
					std::cout << "Fence for queue " << i << " frame in flight " << j << " created\n";
				} else {
					throw std::runtime_error("Fence creation failure");
				}
			}
		}
	}
}