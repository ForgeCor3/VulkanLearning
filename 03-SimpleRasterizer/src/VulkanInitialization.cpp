#include "VulkanInitialization.h"

namespace vulkanInitialization
{
    void createInstance(VkInstance* instance)
    {   
        if(enableValidationLayers && !utility::checkValidationLayerSupport(validationLayers))
		    throw std::runtime_error("Validation layers requested, but not available.");

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "SimpleRasterizer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        std::vector<const char*> extensions = utility::getRequiredExtensions(enableValidationLayers);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        if(enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            utility::populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if(vkCreateInstance(&createInfo, nullptr, instance) != VK_SUCCESS)
            throw std::runtime_error("Failed to create instance.");   
    }

    void setupDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerEXT* debugMessenger)
    {
        if(!enableValidationLayers)
		    return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        utility::populateDebugMessengerCreateInfo(createInfo);

        if(extensions::CreateDebugUtilsMessengetEXT(*instance, &createInfo, nullptr, debugMessenger) != VK_SUCCESS)
            throw std::runtime_error("Failed to set up debug messenger.");
    }

    void createSurface(VkInstance* instance, GLFWwindow* window, VkSurfaceKHR* surface)
    {
        if(glfwCreateWindowSurface(*instance, window, nullptr, surface) != VK_SUCCESS)
            throw std::runtime_error("Failed to create window surface.");
    }

    void pickPhysicalDevice(VkInstance* instance, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

        if(deviceCount == 0)
            throw std::runtime_error("Failed to find GPUs with Vulkan support.");
        
        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(*instance, &deviceCount, physicalDevices.data());

        for(auto& device : physicalDevices)
        {
            if(utility::isPhysicalDeviceSuitable(device, surface, deviceExtensions))
            {
                *physicalDevice = device;
                break;
            }
            else
                throw std::runtime_error("Failed to find a suitable physical device.");
        }
    }

    void createLogicalDevice(VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, VkDevice* logicalDevice, VkQueue* graphicsQueue, VkQueue* presentQueue)
    {
        QueueFamilyIndices indices = utility::findQueueFamilies(physicalDevice, surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;

        for(uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }
        
        VkPhysicalDeviceFeatures physicalDeviceFeatures {};

        VkDeviceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &physicalDeviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if(enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
            createInfo.enabledLayerCount = 0;

        if(vkCreateDevice(*physicalDevice, &createInfo, nullptr, logicalDevice) != VK_SUCCESS)
            throw std::runtime_error("Failed to create logical device.");

        vkGetDeviceQueue(*logicalDevice, indices.graphicsFamily.value(), 0, graphicsQueue);
        vkGetDeviceQueue(*logicalDevice, indices.presentFamily.value(), 0, presentQueue);
    }

    void createSwapChain(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, GLFWwindow* window, VkSwapchainKHR* swapChain,
        std::vector<VkImage>& _swapChainImages, VkFormat& swapChainImageFormat, VkExtent2D& swapChainExtent)
    {
        SwapChainSupportDetails swapChainSupport = utility::querySwapChainSupport(physicalDevice, surface);

        VkSurfaceFormatKHR surfaceFormat = utility::chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = utility::chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = utility::chooseSwapExtent(swapChainSupport.capabilities, window);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = *surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = utility::findQueueFamilies(physicalDevice, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if(indices.presentFamily != indices.graphicsFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if(vkCreateSwapchainKHR(*logicalDevice, &createInfo, nullptr, swapChain) != VK_SUCCESS)
            throw std::runtime_error("Failed to create swap chain.");

        vkGetSwapchainImagesKHR(*logicalDevice, *swapChain, &imageCount, nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(*logicalDevice, *swapChain, &imageCount, _swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void createSwapChainImageViews(VkDevice* logicalDevice, std::vector<VkImageView>& swapChainImageViews, std::vector<VkImage> swapChainImages, VkFormat* swapChainImageFormat)
    {
        swapChainImageViews.resize(swapChainImages.size());

        for(int i = 0; i < swapChainImages.size(); ++i)
        {
            VkImageViewCreateInfo createInfo {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = *swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if(vkCreateImageView(*logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create image views.");
        }
    }

    void createRenderPass(VkDevice* logicalDevice, VkRenderPass* renderPass, VkFormat* swapChainImageFormat)
    {
        VkAttachmentDescription colorAttachment {};
        colorAttachment.format = *swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;

        if(vkCreateRenderPass(*logicalDevice, &renderPassCreateInfo, nullptr, renderPass) != VK_SUCCESS)
            throw std::runtime_error("Failed to create render pass.");
    }

    void createGraphicsPipeline(VkDevice* _logicalDevice, VkPipeline* graphicsPipeline, VkExtent2D* swapChainExtent, VkPipelineLayout* pipelineLayout, VkRenderPass* renderPass)
    {
        auto vertShaderCode = utility::readFile("../shaders/vert.spv");
        auto fragShaderCode = utility::readFile("../shaders/frag.spv");

        VkShaderModule vertShaderModule = utility::createShaderModule(vertShaderCode, _logicalDevice);
        VkShaderModule fragShaderModule = utility::createShaderModule(fragShaderCode, _logicalDevice);

        VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo {};
        vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageCreateInfo.module = vertShaderModule;
        vertShaderStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo {};
        fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageCreateInfo.module = fragShaderModule;
        fragShaderStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageCreateInfo, fragShaderStageCreateInfo};

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescription = Vertex::getAttributeDescription();

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {};
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescription.data();

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo {};
        inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapChainExtent->width;
        viewport.height = (float)swapChainExtent->height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor;
        scissor.offset = {0, 0};
        scissor.extent = *swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo {};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo {};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE; 
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo {};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState {};
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo {};
        colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;

        VkPipelineLayoutCreateInfo layoutCreateInfo {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        
        if(vkCreatePipelineLayout(*_logicalDevice, &layoutCreateInfo, nullptr, pipelineLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create pipeline layout.");

        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo {};
        graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineCreateInfo.stageCount = 2;
        graphicsPipelineCreateInfo.pStages = shaderStages;
        graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
        graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
        graphicsPipelineCreateInfo.pDepthStencilState = nullptr;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = *pipelineLayout;
        graphicsPipelineCreateInfo.renderPass = *renderPass;
        graphicsPipelineCreateInfo.subpass = 0;

        if(vkCreateGraphicsPipelines(*_logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, graphicsPipeline) != VK_SUCCESS)
                throw std::runtime_error("Failed to create graphics pipeline.");

        vkDestroyShaderModule(*_logicalDevice, vertShaderModule, nullptr);
        vkDestroyShaderModule(*_logicalDevice, fragShaderModule, nullptr);
    }

    void createFramebuffers(VkDevice* logicalDevice, std::vector<VkFramebuffer>& swapChainFramebuffers,
        std::vector<VkImageView> swapChainImageViews, VkRenderPass* renderPass, VkExtent2D* swapChainExtent)
    {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for(int i = 0; i < swapChainImageViews.size(); ++i)
        {
            VkImageView attachments[] = {swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferCreateInfo {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = *renderPass;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.width = swapChainExtent->width;
            framebufferCreateInfo.height = swapChainExtent->height;
            framebufferCreateInfo.layers = 1;

            if(vkCreateFramebuffer(*logicalDevice, &framebufferCreateInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create framebuffer."); 
        }
    }

    void createCommandPool(VkDevice* logicalDevice, VkCommandPool* commandPool, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface)
    {
        QueueFamilyIndices queueFamilyIndices = utility::findQueueFamilies(physicalDevice, surface);

        VkCommandPoolCreateInfo commandPoolCreateInfo {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if(vkCreateCommandPool(*logicalDevice, &commandPoolCreateInfo, nullptr, commandPool) != VK_SUCCESS)
            throw std::runtime_error("Failed to create command pool.");
    }

    void createVertexBuffer(VkDevice* logicalDevice, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory,
        VkCommandPool* commandPool, VkQueue* queue, VkPhysicalDevice* physicalDevice)
    {
        VkDeviceSize bufferSize = sizeof(verticesData::vertices[0]) * verticesData::vertices.size();
	
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, verticesData::vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(*logicalDevice, stagingBufferMemory);

        utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *vertexBuffer, *vertexBufferMemory);

        utility::copyBuffer(logicalDevice, &stagingBuffer, vertexBuffer, bufferSize, commandPool, queue);

        vkDestroyBuffer(*logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(*logicalDevice, stagingBufferMemory, nullptr);
    }

    void createIndexBuffer(VkDevice* logicalDevice, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory,
        VkCommandPool* commandPool, VkQueue* queue, VkPhysicalDevice* physicalDevice)
    {
        VkDeviceSize bufferSize = sizeof(verticesData::indices[0]) * verticesData::indices.size();
	
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, verticesData::indices.data(), (size_t)bufferSize);
        vkUnmapMemory(*logicalDevice, stagingBufferMemory);

        utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *indexBuffer, *indexBufferMemory);

        utility::copyBuffer(logicalDevice, &stagingBuffer, indexBuffer, bufferSize, commandPool, queue);

        vkDestroyBuffer(*logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(*logicalDevice, stagingBufferMemory, nullptr); 
    }

    void createCommandBuffers(VkDevice* logicalDevice, const int MAX_FRAMES_IN_FLIGHT, std::vector<VkCommandBuffer>& commandBuffers, VkCommandPool* commandPool)
    {
        commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = *commandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if(vkAllocateCommandBuffers(*logicalDevice, &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS)
            std::runtime_error("Failed to allocate command buffers.");
    }

    void createSyncObjects(VkDevice* logicalDevice, std::vector<VkSemaphore>& imageAvailableSemaphores, std::vector<VkSemaphore>& renderFinishedSemaphores, std::vector<VkFence>& inFlightFences,
        const int MAX_FRAMES_IN_FLIGHT)
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
            if(vkCreateSemaphore(*logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(*logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(*logicalDevice, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create synchronization objects.");
        }
    }
    
} //namespace vulkanInitialization