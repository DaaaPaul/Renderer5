#include "../headers/Commands.h"
#include <iostream>

namespace Vulkan {
	Commands::Commands(Sync&& salvageSync) :
		isSalvagedRemains{ false },
		FRAMES_IN_FLIGHT{ salvageSync.FRAMES_IN_FLIGHT },
		graphicsQueueFamilyIndex{ salvageSync.pipeline.memory.graphicsQueueFamilyIndex },
		commandPool{ VK_NULL_HANDLE },
		commandBuffers(FRAMES_IN_FLIGHT),
		sync(std::move(salvageSync)) {
		std::cout << "---CREATING COMMAND OBJECTS...---\n";

		createCommandObjects();
	}

	Commands::Commands(Commands&& salvageCommands) : 
		isSalvagedRemains{ false },
		FRAMES_IN_FLIGHT{ salvageCommands.FRAMES_IN_FLIGHT },
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

			vkFreeCommandBuffers(sync.pipeline.memory.swapchain.queues.backend.device, commandPool, 2, commandBuffers.data());
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
			.commandBufferCount = FRAMES_IN_FLIGHT
		};

		if (vkAllocateCommandBuffers(sync.pipeline.memory.swapchain.queues.backend.device, &commandBufferInfo, commandBuffers.data()) == VK_SUCCESS) {
			std::cout << FRAMES_IN_FLIGHT << " primary command buffers created straight submitted to ---> qf " << graphicsQueueFamilyIndex << '\n';
		} else {
			throw std::runtime_error("Command buffer creation failure");
		}
	}
}