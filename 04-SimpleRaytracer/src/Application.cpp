#include "Application.h"

Application::Application(const WindowConfig& windowConfig, bool validationLayersEnabled)
{
    validationLayers = validationLayersEnabled ?
        std::vector<const char*>{"VK_LAYER_KHRONOS_validation"} :
        std::vector<const char*>{};

    window.reset(new Window(windowConfig));
    instance.reset(new VulkanInstance(*window, validationLayers));

    if(!validationLayers.empty())
        debugUtilsMessenger.reset(new VulkanDebugUtilsMessenger(instance->getInstance()));

    surface.reset(new VulkanSurface(instance.get()));
    device.reset(new VulkanDevice(*instance, *surface));

    currentFrame = 0;

    createSwapchain();
}

Application::~Application()
{
    destroySwapchain();

    device.reset();

    if(!validationLayers.empty())
        debugUtilsMessenger.reset();

    surface.reset();
    instance.reset();
    window.reset();

    glfwTerminate();
}

void Application::run()
{
    mainloop();
}

void Application::mainloop()
{
    while (!glfwWindowShouldClose(window.get()->getWindow()))
    {
        drawFrame();
        glfwPollEvents();
    }
    
    vkDeviceWaitIdle(device->getDevice());
}

void Application::drawFrame()
{
    const VkCommandBuffer commandBuffer = commandBuffers[currentFrame]->getCommandBuffer();
    const VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentFrame]->getSemaphore();
    const VkSemaphore renderFinishedSemaphore = renderFinishedSemaphores[currentFrame]->getSemaphore();
    const VkFence inFlightFence = inFlightFences[currentFrame]->getFence();

    vkWaitForFences(device->getDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device->getDevice(), swapChain->getSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        recreateSwapchain();
        return;
    }
    else if(result != VK_SUCCESS)
        throw std::runtime_error("Failed to acquire swap chain image.");

    vkResetFences(device->getDevice(), 1, &inFlightFence);

    vkResetCommandBuffer(commandBuffer, 0);
    commandBuffers[currentFrame]->commandBufferBegin();
    recordCommandBuffer(commandBuffer, imageIndex);
    vkEndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    if(vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer.");

    VkSwapchainKHR swapChains[] = { swapChain->getSwapchain() };

    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        recreateSwapchain();
        return;
    }
    else if(result != VK_SUCCESS)
        throw std::runtime_error("Failed to present swap chain image.");
    
    currentFrame = (currentFrame + 1) % commandBuffers.size();
}

void Application::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

    VkRenderPassBeginInfo renderPassBeginInfo {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = graphicsPipeline->getVulkanRenderPass().getRenderPass();
    renderPassBeginInfo.framebuffer = graphicsPipeline->getVulkanFramebuffers().at(imageIndex)->getFramebuffer();
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = swapChain->getExtent();
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<uint32_t>(swapChain->getExtent().width);
    viewport.height = static_cast<uint32_t>(swapChain->getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = {0, 0};
    scissor.extent = swapChain->getExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    
    vkCmdEndRenderPass(commandBuffer);
}

void Application::createSwapchain()
{
    swapChain.reset(new VulkanSwapChain(*device));
    graphicsPipeline.reset(new VulkanGraphicsPipeline(*swapChain));
    commandPool.reset(new VulkanCommandPool(device->getDevice(), device->getGraphicsQueueFamilyIndex()));
    
    for(size_t i = 0; i < swapChain->getSwapChainImageViews().size(); ++i)
    {
        commandBuffers.push_back(std::make_unique<VulkanCommandBuffer>(device->getDevice(), commandPool->getCommandPool()));
        imageAvailableSemaphores.push_back(std::make_unique<VulkanSemaphore>(device->getDevice()));
        renderFinishedSemaphores.push_back(std::make_unique<VulkanSemaphore>(device->getDevice()));
        inFlightFences.push_back(std::make_unique<VulkanFence>(device->getDevice(), true));
    }
}

void Application::destroySwapchain()
{
    for(auto& semaphore : imageAvailableSemaphores)
        semaphore.reset();
    for(auto& semaphore : renderFinishedSemaphores)
        semaphore.reset();
    for(auto& fence : inFlightFences)
        fence.reset();

    commandPool.reset();
    graphicsPipeline.reset();
    swapChain.reset();
}

void Application::recreateSwapchain()
{
    vkDeviceWaitIdle(device->getDevice());
    destroySwapchain();
    createSwapchain();
}