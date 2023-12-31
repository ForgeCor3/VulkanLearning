#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

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

    void pickPhysicalDevice(VkInstance* instance, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface, VkSampleCountFlagBits& msaaSamples)
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
                msaaSamples = utility::getMaxUsableSampleCount(&device);
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
        physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
        physicalDeviceFeatures.sampleRateShading = VK_TRUE;

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
            swapChainImageViews[i] = utility::createImageView(logicalDevice, &swapChainImages[i], *swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    void createRenderPass(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkRenderPass* renderPass, VkFormat* swapChainImageFormat, VkSampleCountFlagBits msaaSamples)
    {
        VkAttachmentDescription colorAttachment {};
        colorAttachment.format = *swapChainImageFormat;
        colorAttachment.samples = msaaSamples;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment {};
        depthAttachment.format = utility::findDepthFormat(physicalDevice);
        depthAttachment.samples = msaaSamples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve {};
        colorAttachmentResolve.format = *swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef {};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentRef;
        subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
        subpassDescription.pResolveAttachments = &colorAttachmentResolveRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachmentRefs = {colorAttachment, depthAttachment, colorAttachmentResolve};

        VkRenderPassCreateInfo renderPassCreateInfo {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentRefs.size());
        renderPassCreateInfo.pAttachments = attachmentRefs.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;

        if(vkCreateRenderPass(*logicalDevice, &renderPassCreateInfo, nullptr, renderPass) != VK_SUCCESS)
            throw std::runtime_error("Failed to create render pass.");
    }

    void createDescriptorSetLayout(VkDevice* logicalDevice, VkDescriptorSetLayout* descriptorSetLayout)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding {};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding}; 

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        descriptorSetLayoutCreateInfo.pBindings = bindings.data();

        if(vkCreateDescriptorSetLayout(*logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, descriptorSetLayout) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor set layout.");
    }

    void createGraphicsPipeline(VkDevice* logicalDevice, VkPipeline* graphicsPipeline, VkExtent2D* swapChainExtent, VkPipelineLayout* pipelineLayout,
        VkRenderPass* renderPass, VkDescriptorSetLayout* descriptorSetLayout, VkSampleCountFlagBits msaaSamples)
    {
        auto vertShaderCode = utility::readFile("../shaders/vert.spv");
        auto fragShaderCode = utility::readFile("../shaders/frag.spv");

        VkShaderModule vertShaderModule = utility::createShaderModule(vertShaderCode, logicalDevice);
        VkShaderModule fragShaderModule = utility::createShaderModule(fragShaderCode, logicalDevice);

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
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo {};
        depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
        depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
        depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.minDepthBounds = 0.0f;
        depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
        depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo {};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
        multisampleStateCreateInfo.minSampleShading = 0.2f;
        multisampleStateCreateInfo.rasterizationSamples = msaaSamples;

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
        layoutCreateInfo.setLayoutCount = 1;
        layoutCreateInfo.pSetLayouts = descriptorSetLayout;
        
        if(vkCreatePipelineLayout(*logicalDevice, &layoutCreateInfo, nullptr, pipelineLayout) != VK_SUCCESS)
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
        graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
        graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
        graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        graphicsPipelineCreateInfo.layout = *pipelineLayout;
        graphicsPipelineCreateInfo.renderPass = *renderPass;
        graphicsPipelineCreateInfo.subpass = 0;

        if(vkCreateGraphicsPipelines(*logicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, graphicsPipeline) != VK_SUCCESS)
                throw std::runtime_error("Failed to create graphics pipeline.");

        vkDestroyShaderModule(*logicalDevice, vertShaderModule, nullptr);
        vkDestroyShaderModule(*logicalDevice, fragShaderModule, nullptr);
    }

    void createFramebuffers(VkDevice* logicalDevice, std::vector<VkFramebuffer>& swapChainFramebuffers, std::vector<VkImageView> swapChainImageViews,
        VkImageView* depthImageView, VkImageView* colorImageView, VkRenderPass* renderPass, VkExtent2D* swapChainExtent)
    {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for(int i = 0; i < swapChainImageViews.size(); ++i)
        {
            std::array<VkImageView, 3> attachments = {*colorImageView, *depthImageView, swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferCreateInfo {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = *renderPass;
            framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferCreateInfo.pAttachments = attachments.data();
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

    void createColorResources(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkFormat* swapChainImageFormat, VkExtent2D* swapChainExtent, VkSampleCountFlagBits msaaSamples,
        VkImage* colorImage, VkDeviceMemory* colorImageMemory, VkImageView* colorImageView)
    {
        VkFormat colorFormat = *swapChainImageFormat;

        utility::createImage(logicalDevice, physicalDevice, swapChainExtent->width, swapChainExtent->height, colorFormat,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory, 1, msaaSamples);
        
        *colorImageView = utility::createImageView(logicalDevice, colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    void createDepthResources(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkExtent2D swapChainExtent,
        VkImage* depthImage, VkDeviceMemory* depthImageMemory, VkImageView& depthImageView, VkSampleCountFlagBits msaaSamples)
    {
        VkFormat depthFormat = utility::findDepthFormat(physicalDevice);
        utility::createImage(logicalDevice, physicalDevice, swapChainExtent.width, swapChainExtent.height, depthFormat,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory, 1, msaaSamples);
        depthImageView = utility::createImageView(logicalDevice, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    }

    void createTextureImage(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkImage* textureImage, VkDeviceMemory* textureImageMemory, VkCommandPool* commandPool,
        VkQueue* graphicsQueue, uint32_t& mipLevels)
    {
        int textureWidth;
        int textureHeight;
        int textureChannels;

        stbi_uc* pixels = stbi_load("../textures/pantheon.png", &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = textureWidth * textureHeight * 4;

        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight))));

        if(!pixels)
            throw std::runtime_error("Failed to load texture image");

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        utility::createBuffer(logicalDevice, physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<uint32_t>(imageSize));
        vkUnmapMemory(*logicalDevice, stagingBufferMemory);

        stbi_image_free(pixels);

        utility::createImage(logicalDevice, physicalDevice, textureWidth, textureHeight, VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage, textureImageMemory, mipLevels, VK_SAMPLE_COUNT_1_BIT);

        utility::transitionImageLayout(logicalDevice, commandPool, textureImage, graphicsQueue, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
        utility::copyBufferToImage(logicalDevice, commandPool, graphicsQueue, &stagingBuffer, textureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
        
        utility::generateMipmaps(logicalDevice, physicalDevice, commandPool, VK_FORMAT_R8G8B8A8_SRGB, textureImage, textureWidth, textureHeight, mipLevels, graphicsQueue);

        vkDestroyBuffer(*logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(*logicalDevice, stagingBufferMemory, nullptr);
    }

    void createTextureImageView(VkDevice* logicalDevice, VkImageView& textureImageView, VkImage* textureImage, uint32_t mipLevels)
    {
        textureImageView = utility::createImageView(logicalDevice, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
    }

    void loadModel(ModelOBJ& model, std::string modelPath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
            throw std::runtime_error(warn + err);

        std::unordered_map<Vertex, uint32_t> uniqueVertices {};

        for(const auto& shape : shapes)
        {
            for(const auto& index : shape.mesh.indices)
            {
                Vertex vertex {};

                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = {1.0f, 1.0f, 1.0f};

                if(uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(model.vertices.size());
                    model.vertices.push_back(vertex);
                }

                model.indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

    void createVertexBuffer(VkDevice* logicalDevice, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory,
        VkCommandPool* commandPool, VkQueue* queue, VkPhysicalDevice* physicalDevice, ModelOBJ model)
    {
        VkDeviceSize bufferSize = sizeof(model.vertices[0]) * model.vertices.size();
	
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, model.vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(*logicalDevice, stagingBufferMemory);

        utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *vertexBuffer, *vertexBufferMemory);

        utility::copyBuffer(logicalDevice, &stagingBuffer, vertexBuffer, bufferSize, commandPool, queue);

        vkDestroyBuffer(*logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(*logicalDevice, stagingBufferMemory, nullptr);
    }

    void createTextureSampler(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, VkSampler* textureSampler, uint32_t mipLevels)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties {};
        vkGetPhysicalDeviceProperties(*physicalDevice, &physicalDeviceProperties);

        VkSamplerCreateInfo samplerCreateInfo {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.anisotropyEnable = VK_TRUE;
        samplerCreateInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerCreateInfo.minLod = 0.0f;
        samplerCreateInfo.maxLod = static_cast<float>(mipLevels);
        samplerCreateInfo.mipLodBias = 0.0f;

        if(vkCreateSampler(*logicalDevice, &samplerCreateInfo, nullptr, textureSampler) != VK_SUCCESS)
            throw std::runtime_error("Failed to create texture sampler.");
    }

    void createIndexBuffer(VkDevice* logicalDevice, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory,
        VkCommandPool* commandPool, VkQueue* queue, VkPhysicalDevice* physicalDevice, ModelOBJ model)
    {
        VkDeviceSize bufferSize = sizeof(model.indices[0]) * model.indices.size();
	
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(*logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, model.indices.data(), (size_t)bufferSize);
        vkUnmapMemory(*logicalDevice, stagingBufferMemory);

        utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *indexBuffer, *indexBufferMemory);

        utility::copyBuffer(logicalDevice, &stagingBuffer, indexBuffer, bufferSize, commandPool, queue);

        vkDestroyBuffer(*logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(*logicalDevice, stagingBufferMemory, nullptr); 
    }

    void createUniformBuffers(VkDevice* logicalDevice, VkPhysicalDevice* physicalDevice, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory,
        std::vector<void*>& unifromBuffersMapped, const int MAX_FRAMES_IN_FLIGHT)
    {
        VkDeviceSize bufferSize = sizeof(ubo::UniformBufferObject);

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        unifromBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            utility::createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                uniformBuffers[i], uniformBuffersMemory[i]);
            
            void* pData;
            VkResult result = vkMapMemory(*logicalDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &pData);
            if (result == VK_SUCCESS)
                unifromBuffersMapped[i] = pData;
        }
    }

    void createDescriptorPool(VkDevice* logicalDevice, VkDescriptorPool* descriptorPool, const int MAX_FRAMES_IN_FLIGHT)
    {
        std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes {};
        descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
        descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
        descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if(vkCreateDescriptorPool(*logicalDevice, &descriptorPoolCreateInfo, nullptr, descriptorPool) != VK_SUCCESS)
            throw std::runtime_error("Failed to create descriptor pool.");
    }

    void createDescriptorSets(VkDevice* logicalDevice, std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorSetLayout* descriptorSetLayout, VkDescriptorPool* descriptorPool,
        std::vector<VkBuffer> uniformBuffers, VkImageView* textureImageView, VkSampler* textureSampler, const int MAX_FRAMES_IN_FLIGHT)
    {
        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *descriptorSetLayout);

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = *descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        descriptorSetAllocateInfo.pSetLayouts = layouts.data();

        if(vkAllocateDescriptorSets(*logicalDevice, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate descriptor sets.");

        for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            VkDescriptorBufferInfo descriptorBufferInfo {};
            descriptorBufferInfo.buffer = uniformBuffers[i];
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = sizeof(ubo::UniformBufferObject);

            VkDescriptorImageInfo descriptorImageInfo {};
            descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptorImageInfo.imageView = *textureImageView;
            descriptorImageInfo.sampler = *textureSampler;

            std::array<VkWriteDescriptorSet, 2> writeDescriptorSets {};

            writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[0].dstSet = descriptorSets[i];
            writeDescriptorSets[0].dstBinding = 0;
            writeDescriptorSets[0].dstArrayElement = 0;
            writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSets[0].descriptorCount = 1;
            writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;

            writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSets[1].dstSet = descriptorSets[i];
            writeDescriptorSets[1].dstBinding = 1;
            writeDescriptorSets[1].dstArrayElement = 0;
            writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeDescriptorSets[1].descriptorCount = 1;
            writeDescriptorSets[1].pImageInfo = &descriptorImageInfo;

            vkUpdateDescriptorSets(*logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        }
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