#include "TriangleApp.h"

void TriangleApp::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanUp();
}

void TriangleApp::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "TriangleApp", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void TriangleApp::initVulkan()
{
	vulkanInitialization::createInstance(instance);
	vulkanInitialization::setupDebugMessenger(instance, debugMessenger);
	vulkanInitialization::createSurface(instance, *window, surface);
	vulkanInitialization::pickPhysicalDevice(instance, physicalDevice, &surface, deviceExtensions);
	vulkanInitialization::createLogicalDevice(&physicalDevice, &surface, logicalDevice, graphicsQueue, presentQueue);
	vulkanInitialization::createSwapChain(&logicalDevice, &physicalDevice, &surface, window, swapChain, swapChainImages,
		swapChainImageFormat, swapChainExtent);
	vulkanInitialization::createSwapChainImageViews(swapChainImageViews, swapChainImages, &swapChainImageFormat, &logicalDevice);
	vulkanInitialization::createRenderPass(&logicalDevice, &swapChainImageFormat, renderPass);
	vulkanInitialization::createGraphicsPipeline(&logicalDevice, &swapChainExtent, pipelineLayout, &renderPass, graphicsPipeline);
	//createInstance();
	//setupDebugMessenger();
	//createSurface();
	//pickPhysicalDevice();
	//createLogicalDevice();
	//createSwapChain();
	//createSwapChainImageViews();
	//createRenderPass();
	//createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createVertexBuffer();
	createCommandBuffers();
	createSyncObjects();
}

void TriangleApp::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(logicalDevice);
}

void TriangleApp::cleanUp()
{
	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}
	
	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	cleanUpSwapChain();

	vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
	vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);
	vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

	vkDestroyDevice(logicalDevice, nullptr);

	if(enableValidationLayers)
		extensions::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void TriangleApp::cleanUpSwapChain()
{
	for(auto framebuffer : swapChainFramebuffers)
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);

	for(auto imageView : swapChainImageViews)
		vkDestroyImageView(logicalDevice, imageView, nullptr);

	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

// void TriangleApp::createInstance()
// {
// 	if(enableValidationLayers && !checkValidationLayerSupport())
// 		throw std::runtime_error("Validation layers requested, but not available.");

// 	VkApplicationInfo appInfo{};
// 	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
// 	appInfo.pApplicationName = "TriangleApp";
// 	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
// 	appInfo.pEngineName = "No Engine";
// 	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
// 	appInfo.apiVersion = VK_API_VERSION_1_0;

// 	std::vector<const char*> extensions = getRequiredExtensions();

// 	VkInstanceCreateInfo createInfo{};
// 	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
// 	createInfo.pApplicationInfo = &appInfo;
// 	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
// 	createInfo.ppEnabledExtensionNames = extensions.data();

// 	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

// 	if(enableValidationLayers)
// 	{
// 		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
// 		createInfo.ppEnabledLayerNames = validationLayers.data();
// 		populateDebugMessengerCreateInfo(debugCreateInfo);
// 		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
// 	}
// 	else
// 	{
// 		createInfo.enabledLayerCount = 0;
// 		createInfo.pNext = nullptr;
// 	}

// 	if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
// 		throw std::runtime_error("Failed to create instance.");
// }

// VKAPI_ATTR VkBool32 VKAPI_CALL TriangleApp::debugCallback(
// 	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
// 	VkDebugUtilsMessageTypeFlagsEXT messageType,
// 	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
// 	void* pUserData)
// {
// 	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
// 	return VK_FALSE;
// }

// void TriangleApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
// {
// 	createInfo = {};
// 	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
// 	createInfo.messageSeverity =
// 		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
// 		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
// 		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
// 	createInfo.messageType = 
// 		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
// 		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
// 		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
// 	createInfo.pfnUserCallback = debugCallback;
// }

// void TriangleApp::setupDebugMessenger()
// {
// 	if(!enableValidationLayers)
// 		return;

// 	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
// 	populateDebugMessengerCreateInfo(createInfo);

// 	if(extensions::CreateDebugUtilsMessengetEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
// 		throw std::runtime_error("Failed to set up debug messenger.");
// }

// bool TriangleApp::checkValidationLayerSupport()
// {
// 	uint32_t layerCount;
// 	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
// 	std::vector<VkLayerProperties> availableLayers(layerCount);
// 	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

// 	for(const char* layerName : validationLayers)
// 	{
// 		bool layerFound = false;

// 		for(const auto& layerProps : availableLayers)
// 		{
// 			if(strcmp(layerName, layerProps.layerName) == 0)
// 			{
// 				layerFound = true;
// 				break;
// 			}
// 		}

// 		if(!layerFound)
// 			return false;
// 	}

// 	return true;
// }

// std::vector<const char*> TriangleApp::getRequiredExtensions()
// {
// 	uint32_t glfwExtensionCount = 0;
// 	const char** glfwExtensions;
// 	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

// 	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

// 	if(enableValidationLayers)
// 		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

// 	return extensions;
// }

// void TriangleApp::createSurface()
// {
// 	if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
// 		throw std::runtime_error("Failed to create window surface.");
// }

// void TriangleApp::pickPhysicalDevice()
// {
// 	uint32_t deviceCount = 0;
// 	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

// 	if(deviceCount == 0)
// 		throw std::runtime_error("Failed to find GPUs with Vulkan support.");
	
// 	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
// 	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

// 	for(const auto& device : physicalDevices)
// 	{
// 		if(isPhysicalDeviceSuitable(device))
// 		{
// 			physicalDevice = device;
// 			break;
// 		}
// 		else
// 			throw std::runtime_error("Failed to find a suitable physical device.");
// 	}
// }

// bool TriangleApp::isPhysicalDeviceSuitable(VkPhysicalDevice _physicalDevice)
// {
// 	TriangleApp::QueueFamilyIndices indices = findQueueFamilies(_physicalDevice);

// 	bool swapChainAdequate = false;

// 	if(checkDeviceExtensionSupport(_physicalDevice))
// 	{
// 		TriangleApp::SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_physicalDevice);
// 		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
// 	}

// 	return (indices.isComplete() && swapChainAdequate);
// }

// bool TriangleApp::checkDeviceExtensionSupport(VkPhysicalDevice _physicalDevice)
// {
// 	uint32_t extensionCount;
// 	vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr);

// 	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
// 	vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, availableExtensions.data());

// 	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

// 	for(const auto& extension : availableExtensions)
// 		requiredExtensions.erase(extension.extensionName);

// 	return requiredExtensions.empty();
// }

// void TriangleApp::createLogicalDevice()
// {
// 	TriangleApp::QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

// 	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
// 	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

// 	float queuePriority = 1.0f;

// 	for(uint32_t queueFamily : uniqueQueueFamilies)
// 	{
// 		VkDeviceQueueCreateInfo queueCreateInfo {};
// 		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
// 		queueCreateInfo.queueFamilyIndex = queueFamily;
// 		queueCreateInfo.queueCount = 1;
// 		queueCreateInfo.pQueuePriorities = &queuePriority;

// 		queueCreateInfos.push_back(queueCreateInfo);
// 	}
	
// 	VkPhysicalDeviceFeatures physicalDeviceFeatures {};

// 	VkDeviceCreateInfo createInfo {};
// 	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
// 	createInfo.pQueueCreateInfos = queueCreateInfos.data();
// 	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
// 	createInfo.pEnabledFeatures = &physicalDeviceFeatures;
// 	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
// 	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

// 	if(enableValidationLayers)
// 	{
// 		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
// 		createInfo.ppEnabledLayerNames = validationLayers.data();
// 	}
// 	else
// 		createInfo.enabledLayerCount = 0;

// 	if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
// 		throw std::runtime_error("Failed to create logical device.");

// 	vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
// 	vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
// }

TriangleApp::QueueFamilyIndices TriangleApp::findQueueFamilies(VkPhysicalDevice _physicalDevice)
{
	TriangleApp::QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, queueFamilies.data());

	VkBool32 presentSupport = false;
	int i = 0;
	for(const auto& queueFamily : queueFamilies)
	{
		if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;
		
		vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, surface, &presentSupport);
		if(presentSupport)
			indices.presentFamily = i;
		
		if(indices.isComplete())
			break;

		i++;
	}

	return indices;
}

// TriangleApp::SwapChainSupportDetails TriangleApp::querySwapChainSupport(VkPhysicalDevice _physicalDevice)
// {
// 	TriangleApp::SwapChainSupportDetails details;

// 	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, surface, &details.capabilities);

// 	uint32_t formatCount = 0;
// 	vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, surface, &formatCount, nullptr);
// 	if(!formatCount == 0)
// 	{
// 		details.formats.resize(formatCount);
// 		vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, surface, &formatCount, details.formats.data());
// 	}

// 	uint32_t modeCount = 0;
// 	vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, surface, &modeCount, nullptr);
// 	if(!modeCount == 0)
// 	{
// 		details.presentModes.resize(modeCount);
// 		vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, surface, &modeCount, details.presentModes.data());
// 	}

// 	return details;
// }

// VkSurfaceFormatKHR TriangleApp::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats)
// {
// 	for(const auto& availableFormat : availableFormats)
// 		if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
// 			return availableFormat;

// 	return availableFormats[0];
// }

// VkPresentModeKHR TriangleApp::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
// {
// 	for(const auto& availablePresentMode : availablePresentModes)
// 		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
// 			return availablePresentMode;
	
// 	return VK_PRESENT_MODE_FIFO_KHR;
// }

// VkExtent2D TriangleApp::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
// {
// 	if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
// 		return capabilities.currentExtent;
// 	else
// 	{
// 		int width;
// 		int height;
// 		glfwGetFramebufferSize(window, &width, &height);

// 		VkExtent2D actualExtent = {
// 			static_cast<uint32_t>(width),
// 			static_cast<uint32_t>(height)
// 		};

// 		actualExtent.width = std::clamp(actualExtent.width,
// 			capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
// 		actualExtent.height = std::clamp(actualExtent.height,
// 			capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

// 		return actualExtent;
// 	}
// }

// void TriangleApp::createSwapChain()
// {
// 	TriangleApp::SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

// 	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
// 	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
// 	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

// 	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

// 	if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
// 		imageCount = swapChainSupport.capabilities.maxImageCount;

// 	VkSwapchainCreateInfoKHR createInfo{};
// 	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
// 	createInfo.surface = surface;
// 	createInfo.minImageCount = imageCount;
// 	createInfo.imageFormat = surfaceFormat.format;
// 	createInfo.imageColorSpace = surfaceFormat.colorSpace;
// 	createInfo.imageExtent = extent;
// 	createInfo.imageArrayLayers = 1;
// 	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

// 	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
// 	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

// 	if(indices.presentFamily != indices.graphicsFamily)
// 	{
// 		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
// 		createInfo.queueFamilyIndexCount = 2;
// 		createInfo.pQueueFamilyIndices = queueFamilyIndices;
// 	}
// 	else
// 	{
// 		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
// 		createInfo.queueFamilyIndexCount = 0;
// 		createInfo.pQueueFamilyIndices = nullptr;
// 	}

// 	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
// 	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
// 	createInfo.clipped = VK_TRUE;
// 	createInfo.oldSwapchain = VK_NULL_HANDLE;

// 	if(vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
// 		throw std::runtime_error("Failed to create swap chain.");

// 	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
// 	swapChainImages.resize(imageCount);
// 	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

// 	swapChainImageFormat = surfaceFormat.format;
// 	swapChainExtent = extent;
// }

// void TriangleApp::createSwapChainImageViews()
// {
// 	swapChainImageViews.resize(swapChainImages.size());

// 	for(int i = 0; i < swapChainImages.size(); ++i)
// 	{
// 		VkImageViewCreateInfo createInfo {};
// 		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
// 		createInfo.image = swapChainImages[i];
// 		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
// 		createInfo.format = swapChainImageFormat;
// 		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
// 		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
// 		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
// 		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
// 		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
// 		createInfo.subresourceRange.baseMipLevel = 0;
// 		createInfo.subresourceRange.levelCount = 1;
// 		createInfo.subresourceRange.baseArrayLayer = 0;
// 		createInfo.subresourceRange.layerCount = 1;

// 		if(vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
// 			throw std::runtime_error("Failed to create image views.");
// 	}
// }

// void TriangleApp::createRenderPass()
// {
// 	VkAttachmentDescription colorAttachment {};
// 	colorAttachment.format = swapChainImageFormat;
// 	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
// 	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
// 	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
// 	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
// 	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
// 	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
// 	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

// 	VkAttachmentReference colorAttachmentRef {};
// 	colorAttachmentRef.attachment = 0;
// 	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

// 	VkSubpassDescription subpassDescription {};
// 	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
// 	subpassDescription.colorAttachmentCount = 1;
// 	subpassDescription.pColorAttachments = &colorAttachmentRef;

// 	VkSubpassDependency dependency{};
// 	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
// 	dependency.dstSubpass = 0;
// 	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
// 	dependency.srcAccessMask = 0;
// 	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
// 	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

// 	VkRenderPassCreateInfo renderPassCreateInfo {};
// 	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
// 	renderPassCreateInfo.attachmentCount = 1;
// 	renderPassCreateInfo.pAttachments = &colorAttachment;
// 	renderPassCreateInfo.subpassCount = 1;
// 	renderPassCreateInfo.pSubpasses = &subpassDescription;
// 	renderPassCreateInfo.dependencyCount = 1;
// 	renderPassCreateInfo.pDependencies = &dependency;

// 	if(vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS)
// 		throw std::runtime_error("Failed to create render pass.");
// }

// void TriangleApp::createGraphicsPipeline()
// {
// 	auto vertShaderCode = readFile("../shaders/vert.spv");
// 	auto fragShaderCode = readFile("../shaders/frag.spv");

// 	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
// 	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

// 	VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo {};
// 	vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
// 	vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
// 	vertShaderStageCreateInfo.module = vertShaderModule;
// 	vertShaderStageCreateInfo.pName = "main";

// 	VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo {};
// 	fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
// 	fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
// 	fragShaderStageCreateInfo.module = fragShaderModule;
// 	fragShaderStageCreateInfo.pName = "main";

// 	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageCreateInfo, fragShaderStageCreateInfo};

// 	auto bindingDescription = Vertex::getBindingDescription();
// 	auto attributeDescription = Vertex::getAttributeDescription();

// 	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {};
// 	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
// 	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
// 	vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
// 	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
// 	vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescription.data();

// 	std::vector<VkDynamicState> dynamicStates = {
// 		VK_DYNAMIC_STATE_VIEWPORT,
// 		VK_DYNAMIC_STATE_SCISSOR
// 	};

// 	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {};
// 	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
// 	dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
// 	dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

// 	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo {};
// 	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
// 	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
// 	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

// 	VkViewport viewport {};
// 	viewport.x = 0.0f;
// 	viewport.y = 0.0f;
// 	viewport.width = (float)swapChainExtent.width;
// 	viewport.height = (float)swapChainExtent.height;
// 	viewport.minDepth = 0.0f;
// 	viewport.maxDepth = 1.0f;

// 	VkRect2D scissor;
// 	scissor.offset = {0, 0};
// 	scissor.extent = swapChainExtent;

// 	VkPipelineViewportStateCreateInfo viewportStateCreateInfo {};
// 	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
// 	viewportStateCreateInfo.viewportCount = 1;
// 	viewportStateCreateInfo.pViewports = &viewport;
// 	viewportStateCreateInfo.scissorCount = 1;
// 	viewportStateCreateInfo.pScissors = &scissor;

// 	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo {};
// 	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
// 	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE; 
// 	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
// 	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
// 	rasterizationStateCreateInfo.lineWidth = 1.0f;
// 	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
// 	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
// 	rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

// 	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo {};
// 	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
// 	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
// 	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

// 	VkPipelineColorBlendAttachmentState colorBlendAttachmentState {};
// 	colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
// 		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
// 	colorBlendAttachmentState.blendEnable = VK_FALSE;

// 	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo {};
// 	colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
// 	colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
// 	colorBlendStateCreateInfo.attachmentCount = 1;
// 	colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

// 	VkPipelineLayoutCreateInfo layoutCreateInfo {};
// 	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	
// 	if(vkCreatePipelineLayout(logicalDevice, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
// 		throw std::runtime_error("Failed to create pipeline layout.");

// 	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo {};
// 	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
// 	graphicsPipelineCreateInfo.stageCount = 2;
// 	graphicsPipelineCreateInfo.pStages = shaderStages;
// 	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
// 	graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
// 	graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
// 	graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
// 	graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
// 	graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
// 	graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
// 	graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
// 	graphicsPipelineCreateInfo.layout = pipelineLayout;
// 	graphicsPipelineCreateInfo.renderPass = renderPass;
// 	graphicsPipelineCreateInfo.subpass = 0;

// 	if(vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1,
// 		&graphicsPipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
// 			throw std::runtime_error("Failed to create graphics pipeline.");

// 	vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
// 	vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
// }

// VkShaderModule TriangleApp::createShaderModule(const std::vector<char>& shaderCode)
// {
// 	VkShaderModuleCreateInfo createInfo {};
// 	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
// 	createInfo.codeSize = shaderCode.size();
// 	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

// 	VkShaderModule shaderModule;
// 	vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule);

// 	return shaderModule;
// }

// std::vector<char> TriangleApp::readFile(const std::string& fileName)
// {
// 	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

// 	if(!file.is_open())
// 		throw std::runtime_error("Failed to open file.");

// 	size_t fileSize = (size_t)file.tellg();
// 	std::vector<char> buffer(fileSize);
// 	file.seekg(0);
// 	file.read(buffer.data(), fileSize);
// 	file.close();

// 	return buffer;
// }

void TriangleApp::createFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for(int i = 0; i < swapChainImageViews.size(); ++i)
	{
		VkImageView attachments[] = {swapChainImageViews[i]};

		VkFramebufferCreateInfo framebufferCreateInfo {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = swapChainExtent.width;
		framebufferCreateInfo.height = swapChainExtent.height;
		framebufferCreateInfo.layers = 1;

		if(vkCreateFramebuffer(logicalDevice, &framebufferCreateInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create framebuffer."); 
	}
}

void TriangleApp::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo commandPoolCreateInfo {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if(vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool.");
}

void TriangleApp::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferCreateInfo {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if(vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create vertex buffer.");

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

	if(vkAllocateMemory(logicalDevice, &memoryAllocateInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate vertex buffer memory");

	vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
}

void TriangleApp::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(triangle[0]) * triangle.size();
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, triangle.data(), (size_t)bufferSize);
	vkUnmapMemory(logicalDevice, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void TriangleApp::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, &commandBuffer);

	VkCommandBufferBeginInfo commandBufferBeginInfo {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	VkBufferCopy copyRegion {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

uint32_t TriangleApp::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for(int i = 0; i < memoryProperties.memoryTypeCount; ++i)
		if(typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	throw std::runtime_error("Failed to find suitable memory type.");
}

void TriangleApp::createCommandBuffers()
{
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if(vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS)
		std::runtime_error("Failed to allocate command buffers.");
}

void TriangleApp::recordCommandBuffer(VkCommandBuffer _commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if(vkBeginCommandBuffer(_commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin recording command buffer.");

	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	VkRenderPassBeginInfo renderPassBeginInfo {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent = swapChainExtent;
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(_commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	
	VkRect2D scissor {};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;

	vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
	vkCmdDraw(_commandBuffer, 3, 1, 0, 0);

	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(_commandBuffer, static_cast<uint32_t>(triangle.size()), 1, 0, 0);

	vkCmdEndRenderPass(_commandBuffer);

	if(vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to record command buffer");
}

void TriangleApp::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if(vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
		vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
		vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create synchronization objects.");
	}
}

void TriangleApp::drawFrame()
{
	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if(result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		framebufferResized = false;
		recreateSwapChain();
		return;
	}
	else if(result != VK_SUCCESS && result != VK_ERROR_OUT_OF_DATE_KHR)
		throw std::runtime_error("Failed to acquire swap chain image.");

	vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer.");

	VkPresentInfoKHR presentInfo {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = {swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);
	if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		recreateSwapChain();
		return;
	}
	else if(result != VK_SUCCESS)
		throw std::runtime_error("Failed to present swap chain image.");

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void TriangleApp::recreateSwapChain()
{
	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while(width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logicalDevice);

	cleanUpSwapChain();

	vulkanInitialization::createSwapChain(&logicalDevice, &physicalDevice, &surface, window, swapChain, swapChainImages,
		swapChainImageFormat, swapChainExtent);
	vulkanInitialization::createSwapChainImageViews(swapChainImageViews, swapChainImages, &swapChainImageFormat, &logicalDevice);
	createFramebuffers();
}