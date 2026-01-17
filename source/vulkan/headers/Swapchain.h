#pragma once

#include "Queues.h"

namespace Vulkan {
	class Memory;
	class Pipeline;
	class Sync;
	class Commands;
	class Engine;

	class Swapchain {
		friend class Memory;
		friend class Pipeline;
		friend class Sync;
		friend class Commands;
		friend class Engine;

	private:
		bool isSalvagedRemains;

		// information parameters
		VkSwapchainCreateInfoKHR swapchainInfo;
		std::vector<VkImage> images;
		uint32_t imagesCount;

		// vulkan objects
		Queues queues;
		VkSwapchainKHR swapchain;
		std::vector<VkImageView> imageViews;

		// utility
		void clean();
		void setupInformationParameters();
		void takeEverything(Swapchain&& salvageSwapchain);
		void salvageSelf();

		// create vulkan objects
		void setupSwapchainAndImageStuff();
		void createSwapchain();
		void populateImages();
		void createImageViews(VkImageAspectFlags aspectBits);

		VkSurfaceFormatKHR getImageFormat();
		VkExtent2D getImageExtent();
		VkPresentModeKHR getPresentMode();
	public:
		Swapchain(Queues&& salvageQueues);
		Swapchain(Swapchain&& salvageSwapchain);
		~Swapchain();

		DELETE_COPYING(Swapchain);
	};
}

