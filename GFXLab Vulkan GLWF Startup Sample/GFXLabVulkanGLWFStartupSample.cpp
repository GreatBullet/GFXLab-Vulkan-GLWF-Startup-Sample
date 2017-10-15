#include "stdafx.h"
#define GLFW_INCLUDE_VULKAN
#include<GLFW\glfw3.h>
//#include"vulkan\vulkan.h"
#include<iostream>
#include<vector>

#define ASSERT_VUKLAN(val)if( val != VK_SUCCESS ){ __debugbreak(); }

VkInstance instance;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain;
VkImageView *imageViews;
GLFWwindow *window;
uint32_t windowWIDTH = 800, windowsHEIGHT = 600, amountOfImagesInSwapchain = 0;

uint32_t printStats(VkPhysicalDevice &device) {
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

	for (unsigned int i = 0; i < amountOfQueueFamilies; i++) {
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
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
	std::cout << "MinImageCount: " << surfaceCapabilities.minImageCount << std::endl;
	std::cout << "MaxImageCount: " << surfaceCapabilities.maxImageCount << std::endl;
	std::cout << "CurrentExtent: " << surfaceCapabilities.currentExtent.height << '/' << surfaceCapabilities.currentExtent.width << std::endl;
	std::cout << "MinImageExtent: " << surfaceCapabilities.minImageExtent.height << '/' << surfaceCapabilities.minImageExtent.width << std::endl;
	std::cout << "MaxImageExtent: " << surfaceCapabilities.maxImageExtent.height << '/' << surfaceCapabilities.maxImageExtent.width << std::endl;
	std::cout << "MaxImageArrayLayers: " << surfaceCapabilities.maxImageArrayLayers << std::endl;
	std::cout << "SupportedTransform:" << surfaceCapabilities.supportedTransforms << std::endl;
	std::cout << "CurrentTransform:" << surfaceCapabilities.currentTransform << std::endl;
	std::cout << "SupportedCompositeAlpha:" << surfaceCapabilities.supportedCompositeAlpha << std::endl;
	std::cout << "SupportedUsageFlags:" << surfaceCapabilities.supportedUsageFlags << std::endl;

	uint32_t amountOfFormats = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, nullptr);
	VkSurfaceFormatKHR *surfaceFormats = new VkSurfaceFormatKHR[amountOfFormats];
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, surfaceFormats);

	std::cout << std::endl;
	std::cout << "Amount of Formats: " << amountOfFormats << std::endl;
	for (unsigned int i = 0; i < amountOfFormats; i++) {
		std::cout << "Format: " << surfaceFormats[i].format << std::endl;
	}
	uint32_t amountOfPresentationModes = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, nullptr);
	VkPresentModeKHR *presentModes = new VkPresentModeKHR[amountOfPresentationModes];
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOfPresentationModes, presentModes);

	std::cout << std::endl;
	std::cout << "Amount of Presentation Modes: " << amountOfPresentationModes << std::endl;
	for (unsigned int i = 0; i < amountOfPresentationModes; i++) {
		std::cout << "Supported presentation mode: " << presentModes[i] << std::endl;
	}
	//delete[] surfaceFormats;
	delete[] familyProperties;
	return surfaceCapabilities.minImageCount;
}

void startGlfw() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(windowWIDTH, windowsHEIGHT, "Vulkan GLFW Window", __nullptr, nullptr);
}

void startVulkan() {
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "GFXLab Vulkan GLFW Implementation";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "GFXLab Vulkan Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	uint32_t amountOfLayers = 0;
	vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
	VkLayerProperties *layers = new VkLayerProperties[amountOfLayers];
	vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);
	std::cout << "Amount of Instance Layers: " << amountOfLayers << std::endl;
	for (unsigned int i = 0; i < amountOfLayers; i++) {
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
	for (unsigned int i = 0; i < amountOfExtentions; i++) {
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

	uint32_t amountOfGlfwExtensions = 0;
	auto glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = validationLayers.size();
	instanceInfo.ppEnabledLayerNames = validationLayers.data();
	instanceInfo.enabledExtensionCount = amountOfGlfwExtensions;
	instanceInfo.ppEnabledExtensionNames = glfwExtensions;

	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
	vkGetInstanceProcAddr(instance, "");
	ASSERT_VUKLAN(result);

	result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
	ASSERT_VUKLAN(result);

	uint32_t amountOfPhysicalDevices = 0;
	result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr);
	ASSERT_VUKLAN(result);

	std::vector<VkPhysicalDevice> physicalDevices;
	physicalDevices.resize(amountOfPhysicalDevices);
	result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data());
	ASSERT_VUKLAN(result);
	uint32_t mImageCount = printStats(physicalDevices[0]);

	float queuePrios[] = { 1.0f,1.0f,1.0f,1.0f };
	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.pNext = nullptr;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = 0;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePrios;

	VkPhysicalDeviceFeatures usedFeatures = {};
	const std::vector<const char*>deviceExtentions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = deviceExtentions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtentions.data();
	deviceCreateInfo.pEnabledFeatures = &usedFeatures;


	result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);
	ASSERT_VUKLAN(result);

	VkQueue queue;
	vkGetDeviceQueue(device, 0, 0, &queue);

	VkBool32 surfaceSupport = false;
	result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &surfaceSupport);
	ASSERT_VUKLAN(result);
	if (!surfaceSupport) {
		std::cerr << "Surface not supported!" << std::endl;
		__debugbreak();
	}

	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = mImageCount;
	swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCreateInfo.imageExtent = VkExtent2D{ windowWIDTH, windowsHEIGHT };
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
	ASSERT_VUKLAN(result);

	
	vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, nullptr);
	VkImage *swapchainImages = new VkImage[amountOfImagesInSwapchain];
	result = vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, swapchainImages);
	ASSERT_VUKLAN(result);

	imageViews = new VkImageView[amountOfImagesInSwapchain];
	for (unsigned int i = 0; i < amountOfImagesInSwapchain; i++) {
		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = swapchainImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]);
		ASSERT_VUKLAN(result);
	}

	

	delete[] swapchainImages;
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
	for (unsigned int i = 0; i < amountOfImagesInSwapchain; i++) {
		vkDestroyImageView(device, imageViews[i], nullptr);
	}
	delete[] imageViews;
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
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
