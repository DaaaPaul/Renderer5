#pragma once

#include "Sync.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan {
	class Engine;

	class Commands {
		friend class Engine;

	private:
		bool isSalvagedRemains;

		const uint16_t GRAPHICS_QUEUE_COUNT;
		const uint16_t FLIGHT_COUNT;
		uint32_t graphicsQueueFamilyIndex;

		VkCommandPool commandPool;
		std::vector<std::vector<VkCommandBuffer>> commandBuffers;
		Sync sync;

		void createCommandObjects();

		void createCommandPool();
		void createCommandBuffers();
	public:
		Commands(Sync&& salvageSync);
		Commands(Commands&& salvageCommands);
		~Commands();

		Commands(Commands const&) = delete;
		Commands& operator=(Commands const&) = delete;
	};
}
