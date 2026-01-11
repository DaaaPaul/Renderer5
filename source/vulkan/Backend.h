#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

namespace Vulkan {
	class Backend {
	private:
		bool validationLayersEnabled;
		std::vector<const char*> validationLayers;

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
	public:
		Backend();
		~Backend();
	};
}

