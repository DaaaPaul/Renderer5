#include "Backend.h"
#include <stdexcept>
#include <iostream>
#include <vector>

namespace Vulkan {
	Backend::Backend(bool enableValidationLayers) : validationLayersEnabled{ enableValidationLayers }, validationLayers{}, window { VK_NULL_HANDLE }, instance{ VK_NULL_HANDLE }, surface{ VK_NULL_HANDLE }, physicalDevice{ VK_NULL_HANDLE }, device{ VK_NULL_HANDLE } {
	
	}

	void Backend::createWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(800, 600, "Renderer5", nullptr, nullptr);

		if (window == NULL) {
			throw std::runtime_error("Window creation failure");
		} else {
			std::cout << "Window created\n";
		}
	}

	void Backend::createInstance() {
		VkApplicationInfo appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.apiVersion = VK_API_VERSION_1_3
		};

		std::tuple<const char**, uint32_t> glfwExtensionData = getGlfwExtensions();

		VkInstanceCreateInfo instanceInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &appInfo,
			.enabledExtensionCount = std::get<1>(glfwExtensionData),
			.ppEnabledExtensionNames = std::get<0>(glfwExtensionData)
		};

		if(validationLayersEnabled) {
			if(verifyHaveLayers(validationLayers)) {
				instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				instanceInfo.ppEnabledLayerNames = validationLayers.data();
			} else {
				throw std::runtime_error("ERROR: Required validation layer not supported");
			}
		}

		if(vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("ERROR: Vulkan instance creation failed");
		} else {
			std::cout << "Vulkan instance created\n";
		};
	}

	std::tuple<const char**, uint32_t> Backend::getGlfwExtensions() {
		uint32_t requiredExtensionsCount = 0;
		const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionsCount);

		if (!verifyHaveExtensions(requiredExtensions, requiredExtensionsCount)) {
			throw std::runtime_error("ERROR: Required extension by glfw not supported");
		}

		return std::make_tuple(requiredExtensions, requiredExtensionsCount);
	}

	bool Backend::verifyHaveExtensions(const char** verifyMe, uint32_t verifyCount) {
		bool haveExtensions = true;

		std::vector<VkExtensionProperties> extensionProperties{};
		uint32_t extensionsCount = 0;
		vkEnumerateInstanceExtensionProperties(NULL, &extensionsCount, extensionProperties.data());

		bool extensionFound = false;
		for (int i = 0; i < verifyCount; i++) {
			extensionFound = false;

			for (VkExtensionProperties const& extension : extensionProperties) {
				if (strcmp(extension.extensionName, verifyMe[i]) == 0) {
					extensionFound = true;
					break;
				}
			}

			if (!extensionFound) {
				haveExtensions = false;
				break;
			} else {
				std::cout << "Verified extension is present: " << verifyMe[i] << '\n';
			}
		}

		return haveExtensions;
	}

	bool Backend::verifyHaveLayers(std::vector<const char*> verifyMe) {
		bool haveLayers = true;

		std::vector<VkLayerProperties> layerProperties{};
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

		bool layerFound = false;
		for (int i = 0; i < verifyMe.size(); i++) {
			layerFound = false;

			for (VkLayerProperties const& layer : layerProperties) {
				if (strcmp(layer.layerName, verifyMe[i]) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				haveLayers = false;
				break;
			} else {
				std::cout << "Verified layer is present:" << verifyMe[i] << '\n';
			}
		}

		return haveLayers;
	}

	void Backend::createSurface() {

	}

	void Backend::selectPhysicalDevice() {

	}

	void Backend::createDevice() {

	}
}