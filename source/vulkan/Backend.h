#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <array>

namespace Vulkan {
	class Backend {
	private:
		bool validationLayersEnabled;
		std::vector<const char*> validationLayers;
		uint32_t apiVersion;
		uint32_t graphicsQueueCount;
		std::vector<const char*> deviceExtensions;

		void populateValidationLayers();

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

	public:
		Backend();
		~Backend();
	};
}

