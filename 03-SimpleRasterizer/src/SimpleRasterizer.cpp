#include "SimpleRasterizer.h"

void SimpleRasterizer::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanUp();
}

void SimpleRasterizer::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "SimpleRasterizer", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void SimpleRasterizer::initVulkan()
{
	vulkanInitialization::createInstance(&instance);
	vulkanInitialization::setupDebugMessenger(&instance, &debugMessenger);
	vulkanInitialization::createSurface(&instance, window, &surface);
	vulkanInitialization::pickPhysicalDevice(&instance, &physicalDevice, &surface);
	vulkanInitialization::createLogicalDevice(&physicalDevice, &surface, &logicalDevice, &graphicsQueue, &presentQueue);
	vulkanInitialization::createSwapChain(&logicalDevice, &physicalDevice, &surface, window, &swapChain, swapChainImages, swapChainImageFormat, swapChainExtent);
	vulkanInitialization::createSwapChainImageViews(&logicalDevice, swapChainImageViews, swapChainImages, &swapChainImageFormat);
	vulkanInitialization::createRenderPass(&logicalDevice, &physicalDevice, &renderPass, &swapChainImageFormat);
	vulkanInitialization::createDescriptorSetLayout(&logicalDevice, &descriptorSetLayout);
	vulkanInitialization::createGraphicsPipeline(&logicalDevice, &graphicsPipeline, &swapChainExtent, &pipelineLayout, &renderPass, &descriptorSetLayout);
	vulkanInitialization::createCommandPool(&logicalDevice, &commandPool, &physicalDevice, &surface);
	vulkanInitialization::createDepthResources(&logicalDevice, &physicalDevice, swapChainExtent, &depthImage, &depthImageMemory, depthImageView);
	vulkanInitialization::createFramebuffers(&logicalDevice, swapChainFramebuffers, swapChainImageViews, &depthImageView, &renderPass, &swapChainExtent);
	vulkanInitialization::createTextureImage(&logicalDevice, &physicalDevice, &textureImage, &textureImageMemory, &commandPool, &graphicsQueue);
	vulkanInitialization::createTextureImageView(&logicalDevice, textureImageView, &textureImage);
	vulkanInitialization::createTextureSampler(&logicalDevice, &physicalDevice, &textureSampler);
	vulkanInitialization::createVertexBuffer(&logicalDevice, &vertexBuffer, &vertexBufferMemory, &commandPool, &graphicsQueue, &physicalDevice);
	vulkanInitialization::createIndexBuffer(&logicalDevice, &indexBuffer, &indexBufferMemory, &commandPool, &graphicsQueue, &physicalDevice);
	vulkanInitialization::createUniformBuffers(&logicalDevice, &physicalDevice, uniformBuffers, uniformBuffersMemory, uniformBuffersMapped, MAX_FRAMES_IN_FLIGHT);
	vulkanInitialization::createDescriptorPool(&logicalDevice, &descriptorPool, MAX_FRAMES_IN_FLIGHT);
	vulkanInitialization::createDescriptorSets(&logicalDevice, descriptorSets, &descriptorSetLayout, &descriptorPool, uniformBuffers, &textureImageView, &textureSampler, MAX_FRAMES_IN_FLIGHT);
	vulkanInitialization::createCommandBuffers(&logicalDevice, MAX_FRAMES_IN_FLIGHT, commandBuffers, &commandPool);
	vulkanInitialization::createSyncObjects(&logicalDevice, imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences, MAX_FRAMES_IN_FLIGHT);
}

void SimpleRasterizer::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(logicalDevice);
}

void SimpleRasterizer::cleanUp()
{
	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}
	
	vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

	cleanUpSwapChain();
	vkDestroyImageView(logicalDevice, depthImageView, nullptr);
	vkDestroyImage(logicalDevice, depthImage, nullptr);
	vkFreeMemory(logicalDevice, depthImageMemory, nullptr);
	vkDestroySampler(logicalDevice, textureSampler, nullptr);
	vkDestroyImageView(logicalDevice, textureImageView, nullptr);
	vkDestroyImage(logicalDevice, textureImage, nullptr);
	vkFreeMemory(logicalDevice, textureImageMemory, nullptr);
	vkDestroyBuffer(logicalDevice, indexBuffer, nullptr);
	vkFreeMemory(logicalDevice, indexBufferMemory, nullptr);
	vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
	vkFreeMemory(logicalDevice, vertexBufferMemory, nullptr);

	for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroyBuffer(logicalDevice, uniformBuffers[i], nullptr);
		vkFreeMemory(logicalDevice, uniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
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

void SimpleRasterizer::cleanUpSwapChain()
{
	for(auto framebuffer : swapChainFramebuffers)
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);

	for(auto imageView : swapChainImageViews)
		vkDestroyImageView(logicalDevice, imageView, nullptr);

	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
}

void SimpleRasterizer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin recording command buffer.");

	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	VkRenderPassBeginInfo renderPassBeginInfo {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassBeginInfo.renderArea.offset = {0, 0};
	renderPassBeginInfo.renderArea.extent = swapChainExtent;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

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

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(verticesData::indices.size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to record command buffer");
}

void SimpleRasterizer::recreateSwapChain()
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

	vulkanInitialization::createSwapChain(&logicalDevice, &physicalDevice, &surface, window, &swapChain, swapChainImages,
		swapChainImageFormat, swapChainExtent);
	vulkanInitialization::createSwapChainImageViews(&logicalDevice, swapChainImageViews, swapChainImages, &swapChainImageFormat);
	vulkanInitialization::createFramebuffers(&logicalDevice, swapChainFramebuffers, swapChainImageViews, &depthImageView, &renderPass, &swapChainExtent);
}

void SimpleRasterizer::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	ubo::UniformBufferObject ubo {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.projection = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
	ubo.projection[1][1] *= -1;

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void SimpleRasterizer::drawFrame()
{
	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	updateUniformBuffer(currentFrame);

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