#pragma once

#include "../Hashtag.h"

namespace Vulkan {
	class Queues;
	class Swapchain;
	class Memory;
	class Pipeline;
	class Sync;
	class Commands;
	class Engine;

	class Backend {
		friend class Queues;
		friend class Swapchain;
		friend class Memory;
		friend class Pipeline;
		friend class Sync;
		friend class Commands;
		friend class Engine;

	private:
		bool isSalvagedRemains;

		// information parameters
		bool validationLayersEnabled;
		std::vector<const char*> validationLayers;
		uint32_t apiVersion;
		uint16_t graphicsQueueCount;
		std::vector<float> graphicsQueuePriorities;
		uint32_t graphicsFamilyIndex;
		std::vector<const char*> deviceExtensions;
		VkSampleCountFlagBits multisampleCount;

		// vulkan objects
		GLFWwindow* window;
		VkInstance instance;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice;
		VkDevice device;

		// utility
		void clean();
		void setupInformationParameters();
		void takeEverything(Backend&& salvageBackend);
		void salvageSelf();

		// create vulkan objects
		void setupBackend();
		void createWindow();
		void createInstance();
		void createSurface();
		void selectPhysicalDevice();
		void createDevice();

		std::tuple<const char**, uint32_t> getGlfwExtensions();
		bool verifyHaveExtensions(const char** verifyMe, uint32_t verifyCount);
		bool verifyHaveLayers(std::vector<const char*> verifyMe);

		std::array<uint32_t, 4> getPhysicalDeviceStatus(VkPhysicalDevice const& physicalDevice);
		uint32_t apiVersionStatus(VkPhysicalDevice const& physicalDevice);
		uint32_t queueFamilyStatus(VkPhysicalDevice const& physicalDevice);
		uint32_t extensionsStatus(VkPhysicalDevice const& physicalDevice);
		uint32_t featuresStatus(VkPhysicalDevice const& physicalDevice);
		uint32_t getPhysicalDeviceGaugement(std::array<uint32_t, 4> const& physicalDeviceStatus);
		uint32_t getIndexOfGreatest(std::vector<uint32_t> const& physicalDeviceRatings);

		VkSampleCountFlagBits getMultiSampleCount(VkPhysicalDevice const& physicalDevice);
	public:
		Backend();
		Backend(Backend&& salvageBackend);
		~Backend();

		DELETE_COPYING(Backend);
	};
}

