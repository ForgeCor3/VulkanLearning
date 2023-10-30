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
	vulkanInitialization::createInstance(&instance);
	vulkanInitialization::setupDebugMessenger(&instance, &debugMessenger);
	vulkanInitialization::createSurface(&instance, window, &surface);
	vulkanInitialization::pickPhysicalDevice(&instance, &physicalDevice, &surface);
	vulkanInitialization::createLogicalDevice(&physicalDevice, &surface, &logicalDevice, &graphicsQueue, &presentQueue);
	vulkanInitialization::createSwapChain(&logicalDevice, &physicalDevice, &surface, window, &swapChain, swapChainImages, swapChainImageFormat, swapChainExtent);
	vulkanInitialization::createSwapChainImageViews(&logicalDevice, swapChainImageViews, swapChainImages, &swapChainImageFormat);
	vulkanInitialization::createRenderPass(&logicalDevice, &renderPass, &swapChainImageFormat);
	vulkanInitialization::createGraphicsPipeline(&logicalDevice, &graphicsPipeline, &swapChainExtent, &pipelineLayout, &renderPass);
	vulkanInitialization::createFramebuffers(&logicalDevice, swapChainFramebuffers, swapChainImageViews, &renderPass, &swapChainExtent);
	vulkanInitialization::createCommandPool(&logicalDevice, &commandPool, &physicalDevice, &surface);
	vulkanInitialization::createVertexBuffer(&logicalDevice, &vertexBuffer, &vertexBufferMemory, &commandPool, &graphicsQueue, &physicalDevice);
	vulkanInitialization::createCommandBuffers(&logicalDevice, MAX_FRAMES_IN_FLIGHT, commandBuffers, &commandPool);
	vulkanInitialization::createSyncObjects(&logicalDevice, imageAvailableSemaphores, renderFinishedSemaphores, inFlightFences, MAX_FRAMES_IN_FLIGHT);
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

	vkCmdDraw(_commandBuffer, static_cast<uint32_t>(triangleData::triangle.size()), 1, 0, 0);

	vkCmdEndRenderPass(_commandBuffer);

	if(vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to record command buffer");
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

	vulkanInitialization::createSwapChain(&logicalDevice, &physicalDevice, &surface, window, &swapChain, swapChainImages,
		swapChainImageFormat, swapChainExtent);
	vulkanInitialization::createSwapChainImageViews(&logicalDevice, swapChainImageViews, swapChainImages, &swapChainImageFormat);
	vulkanInitialization::createFramebuffers(&logicalDevice, swapChainFramebuffers, swapChainImageViews, &renderPass, &swapChainExtent);
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