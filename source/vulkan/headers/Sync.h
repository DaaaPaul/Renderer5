#pragma once

#include "Pipeline.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan {
	class Commands;
	class Engine;

	class Sync {
		friend class Commands;
		friend class Engine;

	private:
		bool isSalvagedRemains;

		const uint16_t GRAPHICS_QUEUE_COUNT;
		const uint16_t FRAMES_IN_QUEUE;

		std::vector<std::vector<VkSemaphore>> imageReady;
		std::vector<std::vector<VkSemaphore>> imageFinished;
		std::vector<std::vector<VkFence>> commandBufferFinished;
		Pipeline pipeline;

		void createSyncObjects();

		void createSemaphores();
		void createFences();
	public:
		Sync(Pipeline&& salvagePipeline);
		Sync(Sync&& salvageSync);
		~Sync();

		Sync(Sync const&) = delete;
		Sync& operator=(Sync const&) = delete;
	};
}