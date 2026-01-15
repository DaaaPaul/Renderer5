#include "../headers/Commands.h"
#include <iostream>

namespace Vulkan {
	Commands::Commands(Sync&& salvageSync) :
		isSalvagedRemains{ false },
		GRAPHICS_QUEUE_COUNT{ salvageSync.GRAPHICS_QUEUE_COUNT },
		FLIGHT_COUNT{ salvageSync.FLIGHT_COUNT },
		graphicsQueueFamilyIndex{ salvageSync.pipeline.memory.graphicsQueueFamilyIndex },
		commandPool{ VK_NULL_HANDLE },
		commandBuffers(GRAPHICS_QUEUE_COUNT),
		sync(std::move(salvageSync)) {
		std::cout << "---CREATING COMMAND OBJECTS...---\n";

		createCommandObjects();
	}

	Commands::Commands(Commands&& salvageCommands) : 
		isSalvagedRemains{ false },
		GRAPHICS_QUEUE_COUNT{ salvageCommands.GRAPHICS_QUEUE_COUNT },
		FLIGHT_COUNT{ salvageCommands.FLIGHT_COUNT },
		graphicsQueueFamilyIndex{ salvageCommands.graphicsQueueFamilyIndex },
		commandPool{ salvageCommands.commandPool },
		commandBuffers{ salvageCommands.commandBuffers },
		sync(std::move(salvageCommands.sync)) {
		salvageCommands.isSalvagedRemains = true;

		salvageCommands.graphicsQueueFamilyIndex = 0xFFFFFFFF;
		salvageCommands.commandPool = VK_NULL_HANDLE;
		salvageCommands.commandBuffers = {};

		std::cout << "---MOVED COMMAND OBJECTS---\n";
	}

	Commands::~Commands() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING COMMAND OBJECTS...---\n";

			for (int i = 0; i < GRAPHICS_QUEUE_COUNT; i++) {
				vkFreeCommandBuffers(sync.pipeline.memory.swapchain.queues.backend.device, commandPool, FLIGHT_COUNT, commandBuffers[i].data());
			}
			vkDestroyCommandPool(sync.pipeline.memory.swapchain.queues.backend.device, commandPool, nullptr);
		}
	}

	void Commands::createCommandObjects() {
		createCommandPool();
		createCommandBuffers();
	}

	void Commands::createCommandPool() {
		VkCommandPoolCreateInfo commandPoolInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = graphicsQueueFamilyIndex
		};

		if(vkCreateCommandPool(sync.pipeline.memory.swapchain.queues.backend.device, &commandPoolInfo, nullptr, &commandPool) == VK_SUCCESS) {
			std::cout << "Command pool created for qf " << graphicsQueueFamilyIndex << '\n';
		} else {
			throw std::runtime_error("Command pool creation failure");
		}
	}

	void Commands::createCommandBuffers() {
		VkCommandBufferAllocateInfo commandBufferInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = FLIGHT_COUNT
		};

		for(int i = 0; i < GRAPHICS_QUEUE_COUNT; i++) {
			commandBuffers[i].resize(FLIGHT_COUNT, VK_NULL_HANDLE);
			if (vkAllocateCommandBuffers(sync.pipeline.memory.swapchain.queues.backend.device, &commandBufferInfo, commandBuffers[i].data()) == VK_SUCCESS) {
				std::cout << FLIGHT_COUNT << " primary command buffers created for queue " << i << '\n';
			} else {
				throw std::runtime_error("Command buffer creation failure");
			}
		}
	}
}