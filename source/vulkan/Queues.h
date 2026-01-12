#pragma once

#include "Backend.h"
#include <vulkan/vulkan.h>
#include <vector>

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
		Backend backend;
		std::vector<VkQueue> graphicsQueues;

		void createGraphicsQueues();
	public:
		Queues(Backend&& salvageBackend);
		Queues(Queues&& salvageQueues);
	};
}

