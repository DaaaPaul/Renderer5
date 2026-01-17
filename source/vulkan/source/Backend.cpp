#include "../headers/Backend.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <array>

namespace Vulkan {
	Backend::Backend() : isSalvagedRemains{}, validationLayersEnabled{}, validationLayers{}, apiVersion{}, graphicsQueueCount{}, graphicsQueuePriorities{}, graphicsFamilyIndex{}, deviceExtensions{}, window{}, instance{}, surface{}, physicalDevice{}, device{} {
		std::cout << "---CREATING BACKEND...---\n";
		
		setupInformationParameters();
		setupBackend();
	}

	Backend::Backend(Backend&& salvageBackend) : isSalvagedRemains{}, validationLayersEnabled{}, validationLayers{}, apiVersion{}, graphicsQueueCount{}, graphicsQueuePriorities{}, graphicsFamilyIndex{}, deviceExtensions{}, window{}, instance{}, surface{}, physicalDevice{}, device{} {
		std::cout << "---MOVING BACKEND...---\n";

		takeEverything(std::move(salvageBackend));
		salvageBackend.salvageSelf();
	}

	Backend::~Backend() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING BACKEND (along with its queues)...---\n";

			clean();
		}
	}

	void Backend::clean() {
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	
	void Backend::setupInformationParameters() {
		// known at construction
		isSalvagedRemains = false;

		validationLayersEnabled = true;
		validationLayers = {"VK_LAYER_KHRONOS_validation"};
		apiVersion = VK_API_VERSION_1_3;
		deviceExtensions = { "VK_KHR_swapchain", "VK_KHR_synchronization2", "VK_KHR_spirv_1_4"};
		graphicsQueueCount = 2;
		graphicsQueuePriorities = { 0.5f, 0.5f };

		// unknown - populates later
		graphicsFamilyIndex = {};
	}

	void Backend::takeEverything(Backend&& salvageBackend) {
		isSalvagedRemains = false;

		validationLayersEnabled = salvageBackend.validationLayersEnabled;
		validationLayers = salvageBackend.validationLayers;
		apiVersion = salvageBackend.apiVersion;
		graphicsQueueCount = salvageBackend.graphicsQueueCount;
		graphicsQueuePriorities = salvageBackend.graphicsQueuePriorities;
		graphicsFamilyIndex = salvageBackend.graphicsFamilyIndex;
		deviceExtensions = salvageBackend.deviceExtensions;

		window = salvageBackend.window;
		instance = salvageBackend.instance;
		surface = salvageBackend.surface;
		physicalDevice = salvageBackend.physicalDevice;
		device = salvageBackend.device;
	}

	void Backend::salvageSelf() {
		isSalvagedRemains = true;

		validationLayersEnabled = {};
		validationLayers = {};
		apiVersion = {};
		graphicsQueueCount = {};
		graphicsQueuePriorities = {};
		graphicsFamilyIndex = {};
		deviceExtensions = {};

		window = {};
		instance = {};
		surface = {};
		physicalDevice = {};
		device = {};
	}


	void Backend::setupBackend() {
		createWindow();
		createInstance();
		createSurface();
		selectPhysicalDevice();
		createDevice();
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
			.apiVersion = apiVersion
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
				throw std::runtime_error("Required validation layer not supported");
			}
		} else {
			instanceInfo.enabledLayerCount = 0;
			instanceInfo.ppEnabledLayerNames = nullptr;
		}

		if(vkCreateInstance(&instanceInfo, nullptr, &instance) == VK_SUCCESS) {
			std::cout << "Vulkan instance created\n";
		} else {
			throw std::runtime_error("Vulkan instance creation failure");
		};
	}

	std::tuple<const char**, uint32_t> Backend::getGlfwExtensions() {
		uint32_t requiredExtensionsCount = 0;
		const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&requiredExtensionsCount);

		if (!verifyHaveExtensions(requiredExtensions, requiredExtensionsCount)) {
			throw std::runtime_error("Required extension by glfw not supported");
		}

		return std::make_tuple(requiredExtensions, requiredExtensionsCount);
	}

	bool Backend::verifyHaveExtensions(const char** verifyMe, uint32_t verifyCount) {
		bool haveExtensions = true;

		uint32_t extensionsCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
		std::vector<VkExtensionProperties> extensionProperties(extensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensionProperties.data());

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

		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> layerProperties(layerCount);
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
		if(glfwCreateWindowSurface(instance, window, nullptr, &surface) == VK_SUCCESS) {
			std::cout << "Created surface for window\n";
		} else {
			throw std::runtime_error("Vulkan surface creation failure");
		}
	}

	void Backend::selectPhysicalDevice() {
		uint32_t physicalDeviceCount{};
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> allPhysicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, allPhysicalDevices.data());

		// first get statuses
		std::vector<std::array<uint32_t, 4>> physicalDeviceStatuses{};
		for(VkPhysicalDevice const& physicalDevice : allPhysicalDevices) {
			physicalDeviceStatuses.push_back(getPhysicalDeviceStatus(physicalDevice));
		}
		for(std::array<uint32_t, 4> const& status : physicalDeviceStatuses) {
			std::cout << "Physical device status: ";
			for(uint32_t const& state : status) {
				std::cout << state << " ";
			}
			std::cout << '\n';
		}

		// then gauge statuses
		std::vector<uint32_t> physicalDeviceGaugements{};
		for(std::array<uint32_t, 4> const& status : physicalDeviceStatuses) {
			physicalDeviceGaugements.push_back(getPhysicalDeviceGaugement(status));
		}

		// then select the greatest out of the gaugements
		physicalDevice = allPhysicalDevices[getIndexOfGreatest(physicalDeviceGaugements)];
		if(physicalDevice != VK_NULL_HANDLE) {
			std::cout << "Selected physical device: ";

			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(physicalDevice, &properties);

			std::cout << properties.deviceName << '\n';
		} else {
			throw std::runtime_error("Vulkan physical device selection failure");
		}
	}

	uint32_t Backend::getIndexOfGreatest(std::vector<uint32_t> const& physicalDeviceRatings) {
		uint32_t indexOfGreatest = 0;
		uint32_t greatest = 0;

		for(int i = 0; i < physicalDeviceRatings.size(); i++) {
			if(physicalDeviceRatings[i] > greatest) {
				greatest = physicalDeviceRatings[i];
				indexOfGreatest = i;
			}
		}

		return indexOfGreatest;
	}

	uint32_t Backend::getPhysicalDeviceGaugement(std::array<uint32_t, 4> const& physicalDeviceStatus) {
		uint32_t rating = 0;
		
		for(uint32_t const& state : physicalDeviceStatus) {
			rating += state;
		}

		return rating;
	}

	std::array<uint32_t, 4> Backend::getPhysicalDeviceStatus(VkPhysicalDevice const& physicalDevice) {
		std::array<uint32_t, 4> status = { 0, 0, 0, 0 };

		status[0] = apiVersionStatus(physicalDevice);
		status[1] = queueFamilyStatus(physicalDevice);
		status[2] = extensionsStatus(physicalDevice);
		status[3] = featuresStatus(physicalDevice);

		return status;
	}

	uint32_t Backend::apiVersionStatus(VkPhysicalDevice const& physicalDevice) {
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		if(properties.apiVersion >= apiVersion) {
			return 1;
		} else {
			return 0;
		}
	}

	uint32_t Backend::queueFamilyStatus(VkPhysicalDevice const& physicalDevice) {
		uint32_t queueFamilyCount{};
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties2> queueFamilyProperties(queueFamilyCount);
		for(VkQueueFamilyProperties2& queueFamily : queueFamilyProperties) {
			queueFamily.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
		}
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		for(int i = 0; i < queueFamilyCount; i++) {
			if ((queueFamilyProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamilyProperties[i].queueFamilyProperties.queueCount >= graphicsQueueCount)) {
				graphicsFamilyIndex = i;
				std::cout << "Found graphics queue family at " << graphicsFamilyIndex << " with " << queueFamilyProperties[i].queueFamilyProperties.queueCount << " queues\n";
				return 1;
			}
		}

		return 0;
	}

	uint32_t Backend::extensionsStatus(VkPhysicalDevice const& physicalDevice) {
		uint32_t extensionCount{};
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

		bool hasAllExtensions = true;
		bool hasThisExtension = false;
		for(const char* const& requiredExtension : deviceExtensions) {
			hasThisExtension = false;

			for(VkExtensionProperties const& availableExtension : extensions) {
				if(strcmp(availableExtension.extensionName, requiredExtension) == 0) {
					hasThisExtension = true;
					break;
				}
			}

			if(!hasThisExtension) {
				hasAllExtensions = false;
				break;
			} else {
				std::cout << "Required device extension is present: " << requiredExtension << '\n';
			}
		}

		return hasAllExtensions;
	}

	uint32_t Backend::featuresStatus(VkPhysicalDevice const& physicalDevice) {
		VkPhysicalDeviceFeatures2 deviceFeaturesStatus{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
		VkPhysicalDeviceSynchronization2Features deviceSyncFeaturesStatus{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES };
		VkPhysicalDeviceDynamicRenderingFeatures deviceDynamicRenderingFeaturesStatus{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
		VkPhysicalDeviceExtendedDynamicState2FeaturesEXT deviceExtendedDynamicStateFeaturesStatus{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT };
		deviceFeaturesStatus.pNext = &deviceSyncFeaturesStatus;
		deviceSyncFeaturesStatus.pNext = &deviceDynamicRenderingFeaturesStatus;
		deviceDynamicRenderingFeaturesStatus.pNext = &deviceExtendedDynamicStateFeaturesStatus;
		
		vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeaturesStatus);

		bool requiredFeaturesSupported = deviceSyncFeaturesStatus.synchronization2 && deviceDynamicRenderingFeaturesStatus.dynamicRendering && deviceExtendedDynamicStateFeaturesStatus.extendedDynamicState2;
		
		return requiredFeaturesSupported;
	}

	void Backend::createDevice() {
		VkDeviceQueueCreateInfo graphicsQueuesCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = graphicsFamilyIndex,
			.queueCount = graphicsQueueCount,
			.pQueuePriorities = graphicsQueuePriorities.data()
		};

		VkPhysicalDeviceFeatures2 deviceFeaturesStatus{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
		VkPhysicalDeviceSynchronization2Features deviceSyncFeaturesStatus{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES };
		VkPhysicalDeviceDynamicRenderingFeatures deviceDynamicRenderingFeaturesStatus{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
		VkPhysicalDeviceExtendedDynamicState2FeaturesEXT deviceExtendedDynamicStateFeaturesStatus{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT };
		deviceFeaturesStatus.pNext = &deviceSyncFeaturesStatus;
		deviceSyncFeaturesStatus.pNext = &deviceDynamicRenderingFeaturesStatus;
		deviceDynamicRenderingFeaturesStatus.pNext = &deviceExtendedDynamicStateFeaturesStatus;
		deviceSyncFeaturesStatus.synchronization2 = true;
		deviceDynamicRenderingFeaturesStatus.dynamicRendering = true;
		deviceExtendedDynamicStateFeaturesStatus.extendedDynamicState2 = true;

		VkDeviceCreateInfo deviceInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &deviceFeaturesStatus,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &graphicsQueuesCreateInfo,
			.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data(),
		};

		if(vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device) == VK_SUCCESS) {
			std::cout << "Created logical device for the physical device\n";
		} else {
			throw std::runtime_error("Logical device creation failure");
		}
	}
}