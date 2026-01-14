#include "Swapchain.h"
#include <iostream>

namespace Vulkan {
	Swapchain::Swapchain(Queues&& salvageQueues) : 
		isSalvagedRemains{ false }, 
		wantedImageCount{ 3 }, 
		realImageCount{ 0xFFFFFFFF },
		imageFormat{ VK_FORMAT_R8G8B8A8_SRGB },
		imageColorspace{ VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
		imageExtent{  },
		imageUsage{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
		imageSharingMode{ VK_SHARING_MODE_EXCLUSIVE },
		graphicsQueueFamilyIndex{ queues.backend.graphicsFamilyIndex }, 
		preTransform{ VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR },
		presentMode{ VK_PRESENT_MODE_MAILBOX_KHR },
		imageViewAspect{ VK_IMAGE_ASPECT_COLOR_BIT },
		queues(std::move(salvageQueues)), swapchain{ VK_NULL_HANDLE }, images{}, imageViews{} {
		std::cout << "---CREATING SWAPCHAIN...---\n";

		initImageExtent();
		createSwapchain();
		createImages();
		createImageViews();
	}

	Swapchain::~Swapchain() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING SWAPCHAIN...---\n";

			vkDestroySwapchainKHR(queues.backend.device, swapchain, nullptr);
			for (VkImageView& imageView : imageViews) {
				vkDestroyImageView(queues.backend.device, imageView, nullptr);
			}
		}
	}

	Swapchain::Swapchain(Swapchain&& salvageSwapchain) : 
		isSalvagedRemains{ false },
		wantedImageCount{ salvageSwapchain.wantedImageCount },
		realImageCount{ salvageSwapchain.realImageCount },
		imageFormat{ salvageSwapchain.imageFormat },
		imageColorspace{ salvageSwapchain.imageColorspace },
		imageExtent{ salvageSwapchain.imageExtent },
		imageUsage{ salvageSwapchain.imageUsage },
		imageSharingMode{ salvageSwapchain.imageSharingMode },
		graphicsQueueFamilyIndex{ salvageSwapchain.graphicsQueueFamilyIndex },
		preTransform{ salvageSwapchain.preTransform },
		presentMode{ salvageSwapchain.presentMode },
		imageViewAspect{ salvageSwapchain.imageViewAspect },
		queues(std::move(salvageSwapchain.queues)), swapchain{ salvageSwapchain.swapchain }, images{ salvageSwapchain.images }, imageViews{ salvageSwapchain.imageViews } {
		salvageSwapchain.isSalvagedRemains = true;

		salvageSwapchain.swapchain = VK_NULL_HANDLE;
		for(int i = 0; i < salvageSwapchain.imageViews.size(); i++) {
			salvageSwapchain.images[i] = VK_NULL_HANDLE;
			salvageSwapchain.imageViews[i] = VK_NULL_HANDLE;
		}

		std::cout << "---MOVED SWAPCHAIN---\n";
	}

	void Swapchain::createSwapchain() {
		VkSwapchainCreateInfoKHR swapchainInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = queues.backend.surface,
			.minImageCount = wantedImageCount,
			.imageFormat = imageFormat,
			.imageColorSpace = imageColorspace,
			.imageExtent = imageExtent,
			.imageArrayLayers = 1,
			.imageUsage = imageUsage,
			.imageSharingMode = imageSharingMode,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = &graphicsQueueFamilyIndex,
			.preTransform = preTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = presentMode,
			.clipped = true,
			.oldSwapchain = VK_NULL_HANDLE
		};

		if(vkCreateSwapchainKHR(queues.backend.device, &swapchainInfo, nullptr, &swapchain) == VK_SUCCESS) {
			std::cout << "Vulkan swapchain created\n";
		} else {
			throw std::runtime_error("Vulkan swapchain creation failure");
		}
	}

	void Swapchain::initImageExtent() {
		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(queues.backend.physicalDevice, queues.backend.surface, &surfaceCapabilities);

		if(surfaceCapabilities.currentExtent.width != 0xFFFFFFFF) {
			imageExtent = surfaceCapabilities.currentExtent;
		} else {
			int realWidth = 0, realHeight = 0;
			glfwGetFramebufferSize(queues.backend.window, &realHeight, &realHeight);

			if(realWidth > surfaceCapabilities.maxImageExtent.width || realHeight > surfaceCapabilities.maxImageExtent.height) {
				throw std::runtime_error("Vulkan swapchain does not support proper image size rendering\n");
			} else {
				imageExtent.width = realWidth;
				imageExtent.height = realHeight;
			}
		}

		std::cout << "Swapchain/window/surface extent set: " << imageExtent.width << "x" << imageExtent.height << '\n';
	}

	void Swapchain::createImages() {
		vkGetSwapchainImagesKHR(queues.backend.device, swapchain, &realImageCount, nullptr);
		images.resize(realImageCount, VK_NULL_HANDLE);
		vkGetSwapchainImagesKHR(queues.backend.device, swapchain, &realImageCount, images.data());

		std::cout << "Created " << realImageCount << " images for the swapchain\n";
	}

	void Swapchain::createImageViews() {
		imageViews.resize(images.size(), VK_NULL_HANDLE);

		VkImageViewCreateInfo imageViewRollingInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = VK_NULL_HANDLE,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = imageFormat,
			.subresourceRange = VkImageSubresourceRange(imageViewAspect, 0, 1, 0, 1)
		};
		
		for(int i = 0; i < images.size(); i++) {
			imageViewRollingInfo.image = images[i];
			vkCreateImageView(queues.backend.device, &imageViewRollingInfo, nullptr, &imageViews[i]);
		}

		std::cout << "Created " << imageViews.size() << " image views with aspect " << imageViewAspect << '\n';
	}
}