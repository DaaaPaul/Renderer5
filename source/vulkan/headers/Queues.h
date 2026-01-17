#pragma once

#include "Backend.h"

namespace Vulkan {
	class Swapchain;
	class Memory;
	class Pipeline;
	class Sync;
	class Commands;
	class Engine;

	class Queues {
		friend class Swapchain;
		friend class Memory;
		friend class Pipeline;
		friend class Sync;
		friend class Commands;
		friend class Engine;

	private:
		// vulkan objects
		Backend backend;
		std::vector<VkQueue> graphicsQueues;
		uint16_t graphicsQueueCount;

		// create vulkan objects
		void setupQueues();

		void createGraphicsQueues();
	public:
		Queues(Backend&& salvageBackend);
		Queues(Queues&& salvageQueues);

		DELETE_COPYING(Queues);
	};
}

