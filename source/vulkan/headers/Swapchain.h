#pragma once

#include "Queues.h"
#include <vulkan/vulkan.h>
#include <vector>

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

		uint32_t wantedImageCount;
		uint32_t realImageCount;
		VkFormat imageFormat;
		VkColorSpaceKHR imageColorspace;
		VkExtent2D imageExtent;
		VkImageUsageFlags imageUsage;
		VkSharingMode imageSharingMode;
		uint32_t graphicsQueueFamilyIndex;
		VkSurfaceTransformFlagBitsKHR preTransform;
		VkPresentModeKHR presentMode;

		VkImageAspectFlags imageViewAspect;

		Queues queues;
		VkSwapchainKHR swapchain;
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;

		void createSwapchain();
		void createImages();
		void createImageViews();

		void initImageExtent();
	public:
		Swapchain(Queues&& salvageQueues);
		Swapchain(Swapchain&& salvageSwapchain);
		~Swapchain();

		Swapchain(Swapchain const&) = delete;
		Swapchain& operator=(Swapchain const&) = delete;
	};
}

