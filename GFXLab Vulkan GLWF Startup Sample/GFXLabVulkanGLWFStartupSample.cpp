

#include "stdafx.h"
#define GLFW_INCLUDE_VULKAN
#include<GLFW\glfw3.h>
//#include"vulkan\vulkan.h"
#include<iostream>
#include<vector>

#define ASSERT_VUKLAN(val) if(val != VK_SUCCESS){ __debugbreak();}

VkInstance instance;
VkDevice device;
GLFWwindow *window;
void printStats(VkPhysicalDevice &device) {
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	std::cout << "Name: " << properties.deviceName << std::endl;
	uint32_t apiVer = properties.apiVersion;
	std::cout << "API Version: " << VK_VERSION_MAJOR(apiVer) << "." << VK_VERSION_MINOR(apiVer) << "." << VK_VERSION_PATCH(apiVer) << std::endl;
	std::cout << "Driver Version: " << properties.driverVersion << std::endl;
	std::cout << "Vendor ID:   " << properties.vendorID << std::endl;
	std::cout << "Device ID:   " << properties.deviceID << std::endl;
	std::cout << "Device Type: " << properties.deviceType << std::endl;
	std::cout << "Discrete Queue Priorities: " << properties.limits.discreteQueuePriorities << std::endl;
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);
	std::cout << "Geometry Shader: " << features.geometryShader << std::endl;

	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(device, &memProp);

	uint32_t amountOfQueueFamilies = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, nullptr);
	VkQueueFamilyProperties *familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties);
	std::cout << std::endl;
	std::cout << "Amount of Queue Families: " << amountOfQueueFamilies;

	for (int i = 0; i < amountOfQueueFamilies; i++) {
		std::cout << std::endl;
		std::cout << "Queue Family #" << i << std::endl;
		std::cout << "VK_QUEUE_GRAPHICS_BIT       " << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
		std::cout << "VK_QUEUE_COMPUTE_BIT        " << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
		std::cout << "VK_QUEUE_TRANSFER_BIT       " << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
		std::cout << "VK_QUEUE_SPARSE_BINDING_BIT " << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
		std::cout << "Queue Count: " << familyProperties[i].queueCount << std::endl;
		std::cout << "Timestamp Valid Bits: " << familyProperties[i].timestampValidBits << std::endl;
		uint32_t width = familyProperties[i].minImageTransferGranularity.width;
		uint32_t height = familyProperties[i].minImageTransferGranularity.height;
		uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
		std::cout << "Min Image Timestamp Granularity " << width << ", " << height << ", " << depth << std::endl;
	}
	delete[] familyProperties;
}

void startGlfw() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(800, 600, "Vulkan GLWF Window", __nullptr, nullptr);
}
void startVulkan() {
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "GFXLab Vulkan GLWF Implementation";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "GFXLab Vulkan Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	uint32_t amountOfLayers = 0;
	vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
	VkLayerProperties *layers = new VkLayerProperties[amountOfLayers];
	vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);
	std::cout << "Amount of Instance Layers: " << amountOfLayers << std::endl;
	for (int i = 0; i < amountOfLayers; i++) {
		std::cout << std::endl;
		std::cout << "Name:                   " << layers[i].layerName << std::endl;
		std::cout << "Spec Version:           " << layers[i].specVersion << std::endl;
		std::cout << "Implemetnation Version: " << layers[i].implementationVersion << std::endl;
		std::cout << "Decsription:            " << layers[i].description << std::endl;
	}

	uint32_t amountOfExtentions = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtentions, nullptr);
	VkExtensionProperties *extensions = new VkExtensionProperties[amountOfExtentions];
	vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtentions, extensions);
	std::cout << std::endl;
	std::cout << "Amount of Extensions: " << amountOfExtentions << std::endl;
	for (int i = 0; i < amountOfExtentions; i++) {
		std::cout << std::endl;
		std::cout << "Name: " << extensions[i].extensionName << std::endl;
		std::cout << "Spec Version: " << extensions[i].specVersion << std::endl;
	}

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char*> usedExtentions = {
		"VK_KHR_surface"
	};
	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = validationLayers.size();
	instanceInfo.ppEnabledLayerNames = validationLayers.data();
	instanceInfo.enabledExtensionCount = usedExtentions.size();
	instanceInfo.ppEnabledExtensionNames = usedExtentions.data();

	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
	vkGetInstanceProcAddr(instance, "");
	ASSERT_VUKLAN(result);



	uint32_t amountOfPhysicalDevices = 0;
	result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
	ASSERT_VUKLAN(result);

	std::vector<VkPhysicalDevice> physicalDevices; 
	physicalDevices.resize(amountOfPhysicalDevices);
	result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data());
	ASSERT_VUKLAN(result);

	for (int i = 0; i < amountOfPhysicalDevices; i++) {
		printStats(physicalDevices[i]);
	}

	float queuePrios[] = { 1.0f,1.0f,1.0f,1.0f };
	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.pNext = nullptr;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = 0;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePrios;

	VkPhysicalDeviceFeatures usedFeatures = {};

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;
	deviceCreateInfo.pEnabledFeatures = &usedFeatures;


	result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);
	ASSERT_VUKLAN(result);

	VkQueue queue;
	vkGetDeviceQueue(device, 0, 0, &queue);

	delete[] layers;
	delete[] extensions;
}
void gameLoop() {
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}
void shutdownVulkan() {
	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);
}
void shutdownGlfw() {
	glfwDestroyWindow(window);
}
int main()
{
	startGlfw();
	startVulkan();
	gameLoop();
	shutdownVulkan();
	shutdownGlfw();
	return 0;
}

