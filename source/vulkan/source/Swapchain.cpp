#include "../headers/Swapchain.h"
#include <iostream>

namespace Vulkan {
	Swapchain::Swapchain(Queues&& salvageQueues) : isSalvagedRemains{}, swapchainInfo{}, queues(std::move(salvageQueues)), swapchain{}, images{}, imageViews{} {
		std::cout << "---CREATING SWAPCHAIN...---\n";

		setupInformationParameters();
		setupSwapchainAndImageStuff();
	}

	Swapchain::~Swapchain() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING SWAPCHAIN...---\n";

			clean();
		}
	}

	Swapchain::Swapchain(Swapchain&& salvageSwapchain) : isSalvagedRemains{}, swapchainInfo{}, queues(std::move(salvageSwapchain.queues)), swapchain{}, images{}, imageViews{} {
		std::cout << "---MOVING SWAPCHAIN...---\n";

		isSalvagedRemains = false;

		takeEverything(std::move(salvageSwapchain));
		salvageSwapchain.salvageSelf();
	}

	void Swapchain::clean() {
		vkDestroySwapchainKHR(queues.backend.device, swapchain, nullptr);
		for (VkImageView& imageView : imageViews) {
			vkDestroyImageView(queues.backend.device, imageView, nullptr);
		}
	}

	void Swapchain::takeEverything(Swapchain&& salvageSwapchain) {
		swapchainInfo = salvageSwapchain.swapchainInfo;
		swapchain = salvageSwapchain.swapchain;
		images = salvageSwapchain.images;
		imageViews = salvageSwapchain.imageViews;
	}

	void Swapchain::salvageSelf() {
		isSalvagedRemains = true;

		swapchainInfo = {};
		swapchain = VK_NULL_HANDLE;

		for (int i = 0; i < images.size(); i++) {
			images[i] = VK_NULL_HANDLE;
			imageViews[i] = VK_NULL_HANDLE;
		}
	}

	void Swapchain::setupInformationParameters() {
		// known at construction
		isSalvagedRemains = false;

		swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainInfo.pNext = nullptr;
		swapchainInfo.flags = 0;
		swapchainInfo.surface = queues.backend.surface;
		swapchainInfo.minImageCount = 6;
		swapchainInfo.imageFormat = getImageFormat().format;
		swapchainInfo.imageColorSpace = getImageFormat().colorSpace;
		swapchainInfo.imageExtent = getImageExtent();
		swapchainInfo.imageArrayLayers = 1;
		swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainInfo.queueFamilyIndexCount = 1;
		swapchainInfo.pQueueFamilyIndices = &queues.backend.graphicsFamilyIndex;
		swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainInfo.presentMode = getPresentMode();
		swapchainInfo.clipped = true;
		swapchainInfo.oldSwapchain = nullptr;

		// unknown - populates later
		images = {};
		imagesCount = {};
	}

	VkSurfaceFormatKHR Swapchain::getImageFormat() {
		VkSurfaceFormatKHR desiredFormat = {
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		};
		bool foundDesiredFormat = false;

		uint32_t supportedSurfaceFormatsCount{};
		vkGetPhysicalDeviceSurfaceFormatsKHR(queues.backend.physicalDevice, queues.backend.surface, &supportedSurfaceFormatsCount, nullptr);
		std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats(supportedSurfaceFormatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(queues.backend.physicalDevice, queues.backend.surface, &supportedSurfaceFormatsCount, supportedSurfaceFormats.data());

		for(int i = 0; i < supportedSurfaceFormats.size(); i++) {
			if(supportedSurfaceFormats[i].format == desiredFormat.format && supportedSurfaceFormats[i].colorSpace == desiredFormat.colorSpace) {
				foundDesiredFormat = true;
			}
		}

		if(!foundDesiredFormat) {
			throw std::runtime_error("Didn't find desired format to use for swapchain");
		} else {
			return desiredFormat;
		}
	}

	VkExtent2D Swapchain::getImageExtent() {
		VkExtent2D imageExtent{};

		VkSurfaceCapabilitiesKHR surfaceCapabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(queues.backend.physicalDevice, queues.backend.surface, &surfaceCapabilities);

		if (surfaceCapabilities.currentExtent.width != 0xFFFFFFFF) {
			imageExtent = surfaceCapabilities.currentExtent;
		} else {
			int realWidth = 0, realHeight = 0;
			glfwGetFramebufferSize(queues.backend.window, &realHeight, &realHeight);

			if (realWidth > surfaceCapabilities.maxImageExtent.width || realHeight > surfaceCapabilities.maxImageExtent.height) {
				throw std::runtime_error("Vulkan swapchain does not support proper image size rendering\n");
			} else {
				imageExtent.width = realWidth;
				imageExtent.height = realHeight;
			}
		}

		std::cout << "Swapchain/window/surface extent set: " << imageExtent.width << "x" << imageExtent.height << '\n';

		return imageExtent;
	}

	VkPresentModeKHR Swapchain::getPresentMode() {
		VkPresentModeKHR desiredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		bool foundDesiredPresentMode = false;

		uint32_t supportedPresentModesCount{};
		vkGetPhysicalDeviceSurfacePresentModesKHR(queues.backend.physicalDevice, queues.backend.surface, &supportedPresentModesCount, nullptr);
		std::vector<VkPresentModeKHR> supportedSurfacePresentModes(supportedPresentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(queues.backend.physicalDevice, queues.backend.surface, &supportedPresentModesCount, supportedSurfacePresentModes.data());

		for (int i = 0; i < supportedSurfacePresentModes.size(); i++) {
			if (supportedSurfacePresentModes[i] == desiredPresentMode) {
				foundDesiredPresentMode = true;
			}
		}

		if (!foundDesiredPresentMode) {
			throw std::runtime_error("Didn't find desired present mode to use for swapchain");
		} else {
			return desiredPresentMode;
		}
	}

	void Swapchain::setupSwapchainAndImageStuff() {
		createSwapchain();
		populateImages();
		createImageViews(VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void Swapchain::createSwapchain() {
		if(vkCreateSwapchainKHR(queues.backend.device, &swapchainInfo, nullptr, &swapchain) == VK_SUCCESS) {
			std::cout << "Vulkan swapchain created\n";
		} else {
			throw std::runtime_error("Vulkan swapchain creation failure");
		}
	}

	void Swapchain::populateImages() {
		vkGetSwapchainImagesKHR(queues.backend.device, swapchain, &imagesCount, nullptr);
		images.resize(imagesCount, VK_NULL_HANDLE);
		vkGetSwapchainImagesKHR(queues.backend.device, swapchain, &imagesCount, images.data());

		std::cout << "Created " << imagesCount << " images for the swapchain\n";
	}

	void Swapchain::createImageViews(VkImageAspectFlags aspectBits) {
		imageViews.resize(images.size(), VK_NULL_HANDLE);

		VkImageViewCreateInfo imageViewRollingInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = VK_NULL_HANDLE,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = swapchainInfo.imageFormat,
			.subresourceRange = VkImageSubresourceRange(aspectBits, 0, 1, 0, 1)
		};
		
		for(int i = 0; i < images.size(); i++) {
			imageViewRollingInfo.image = images[i];
			vkCreateImageView(queues.backend.device, &imageViewRollingInfo, nullptr, &imageViews[i]);
		}

		std::cout << "Created " << imageViews.size() << " image views with aspect(s) " << aspectBits << '\n';
	}
}