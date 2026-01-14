#include "../headers/Queues.h"
#include <iostream>

namespace Vulkan {
	Queues::Queues(Backend&& salvageBackend) : backend(std::move(salvageBackend)), graphicsQueues(backend.graphicsQueueCount) {
		std::cout << "---CREATING QUEUES...---\n";
		
		createGraphicsQueues();
	}

	Queues::Queues(Queues&& salvageQueues) : backend(std::move(salvageQueues.backend)), graphicsQueues(salvageQueues.graphicsQueues.size()) {
		for(int i = 0; i < salvageQueues.graphicsQueues.size(); i++) {
			graphicsQueues[i] = salvageQueues.graphicsQueues[i];
			salvageQueues.graphicsQueues[i] = VK_NULL_HANDLE;
		}

		std::cout << "---MOVED QUEUES...---\n";
	}

	void Queues::createGraphicsQueues() {
		for(int i = 0; i < graphicsQueues.size(); i++) {
			vkGetDeviceQueue(backend.device, backend.graphicsFamilyIndex, i, &graphicsQueues[i]);
			std::cout << "Created graphics queue at queue family " << backend.graphicsFamilyIndex << " and queue " << i << '\n';
		}
	}
}