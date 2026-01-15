#include "../headers/Sync.h"
#include <iostream>

namespace Vulkan {
	Sync::Sync(Pipeline&& salvagePipeline) :
		isSalvagedRemains{ false },
		FRAMES_IN_FLIGHT{ 2 },
		imageReady(FRAMES_IN_FLIGHT),
		imageFinished(FRAMES_IN_FLIGHT),
		commandBufferFinished(FRAMES_IN_FLIGHT),
		pipeline(std::move(salvagePipeline)) {
		std::cout << "---CREATING SYNC OBJECTS...---\n";

		createSyncObjects();
	}

	Sync::Sync(Sync&& salvageSync) :
		isSalvagedRemains{ false },
		FRAMES_IN_FLIGHT{ salvageSync.FRAMES_IN_FLIGHT },
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

			for(int i = 0; i < FRAMES_IN_FLIGHT; i++) {
				vkDestroySemaphore(pipeline.memory.swapchain.queues.backend.device, imageReady[i], nullptr);
				vkDestroySemaphore(pipeline.memory.swapchain.queues.backend.device, imageFinished[i], nullptr);
				vkDestroyFence(pipeline.memory.swapchain.queues.backend.device, commandBufferFinished[i], nullptr);
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

		for(int i = 0; i < FRAMES_IN_FLIGHT; i++) {
			if(vkCreateSemaphore(pipeline.memory.swapchain.queues.backend.device, &semaphoreInfo, nullptr, &imageReady[i]) == VK_SUCCESS &&
			   vkCreateSemaphore(pipeline.memory.swapchain.queues.backend.device, &semaphoreInfo, nullptr, &imageFinished[i]) == VK_SUCCESS) {
				std::cout << "Semaphore pair for frame in flight " << i << " created\n";
			} else {
				throw std::runtime_error("Semaphore pair creation failure");
			}
		}


	}

	void Sync::createFences() {
		VkFenceCreateInfo fenceInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++) {
			if(vkCreateFence(pipeline.memory.swapchain.queues.backend.device, &fenceInfo, nullptr, &commandBufferFinished[i]) == VK_SUCCESS) {
				std::cout << "Fence for frame in flight " << i << " created\n";
			} else {
				throw std::runtime_error("Fence creation failure");
			}
		}
	}
}