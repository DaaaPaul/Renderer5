#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <array>

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

		bool validationLayersEnabled;
		std::vector<const char*> validationLayers;
		uint32_t apiVersion;
		uint16_t graphicsQueueCount;
		std::vector<float> graphicsQueuePriorities;
		uint32_t graphicsFamilyIndex;
		std::vector<const char*> deviceExtensions;

		GLFWwindow* window;
		VkInstance instance;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice;
		VkDevice device;

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

		VkDeviceQueueCreateInfo getGraphicsQueuesCreateInfo();
	public:
		Backend();
		Backend(Backend&& salvageBackend);
		~Backend();

		Backend(Backend const&) = delete;
		Backend& operator=(Backend const&) = delete;
	};
}

