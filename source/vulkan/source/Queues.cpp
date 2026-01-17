#include "../headers/Queues.h"
#include <iostream>

namespace Vulkan {
	Queues::Queues(Backend&& salvageBackend) : backend(std::move(salvageBackend)), graphicsQueues(backend.graphicsQueueCount), graphicsQueueCount{ backend.graphicsQueueCount } {
		std::cout << "---CREATING QUEUES...---\n";
		
		setupQueues();
	}

	Queues::Queues(Queues&& salvageQueues) : backend(std::move(salvageQueues.backend)), graphicsQueues(salvageQueues.graphicsQueues.size()), graphicsQueueCount{ salvageQueues.graphicsQueueCount } {
		std::cout << "---MOVING QUEUES...---\n";
		
		for(int i = 0; i < salvageQueues.graphicsQueues.size(); i++) {
			graphicsQueues[i] = salvageQueues.graphicsQueues[i];
			salvageQueues.graphicsQueues[i] = {};
		}
		salvageQueues.graphicsQueueCount = {};
	}

	void Queues::setupQueues() {
		createGraphicsQueues();
	}

	void Queues::createGraphicsQueues() {
		for(int i = 0; i < graphicsQueues.size(); i++) {
			vkGetDeviceQueue(backend.device, backend.graphicsFamilyIndex, i, &graphicsQueues[i]);
			std::cout << "Created graphics queue at queue family " << backend.graphicsFamilyIndex << " and queue " << i << '\n';
		}
	}
}