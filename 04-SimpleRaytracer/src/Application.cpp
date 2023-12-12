#include "Application.h"

void error_callback(int, const char* err_str) { std::cerr << "GLFW Error: " << err_str << std::endl; }

Application::Application(const WindowConfig& windowConfig, bool validationLayersEnabled)
{
    glfwSetErrorCallback(error_callback);

    validationLayers = validationLayersEnabled ?
        std::vector<const char*>{"VK_LAYER_KHRONOS_validation"} :
        std::vector<const char*>{};

    window.reset(new Window(windowConfig));
    instance.reset(new VulkanInstance(*window, validationLayers));

    if(!validationLayers.empty())
        debugUtilsMessenger.reset(new VulkanDebugUtilsMessenger(instance->getInstance()));

    surface.reset(new VulkanSurface(instance.get()));
    device.reset(new VulkanDevice(*instance, *surface));
    swapChain.reset(new VulkanSwapChain(*device));
    graphicsPipeline.reset(new VulkanGraphicsPipeline(*swapChain));
    commandPool.reset(new VulkanCommandPool(device->getDevice(), device->getGraphicsQueueFamilyIndex()));
    commandBuffer.reset(new VulkanCommandBuffer(device->getDevice(), commandPool->getCommandPool()));
    imageAvailableSemaphore.reset(new VulkanSemaphore(device->getDevice()));
    renderFinishedSemaphore.reset(new VulkanSemaphore(device->getDevice()));
    inFlightFence.reset(new VulkanFence(device->getDevice(), true));
}

Application::~Application()
{
    imageAvailableSemaphore.reset();
    renderFinishedSemaphore.reset();
    inFlightFence.reset();
    commandPool.reset();
    graphicsPipeline.reset();
    swapChain.reset();
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
    vkWaitForFences(device->getDevice(), 1, &inFlightFence->getFence(), VK_TRUE, UINT64_MAX);
    vkResetFences(device->getDevice(), 1, &inFlightFence->getFence());

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device->getDevice(), swapChain->getSwapchain(), UINT64_MAX, imageAvailableSemaphore->getSemaphore(), VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffer->getCommandBuffer(), 0);
    commandBuffer->commandBufferBegin();
    recordCommandBuffer(commandBuffer->getCommandBuffer(), imageIndex);
    vkEndCommandBuffer(commandBuffer->getCommandBuffer());
    
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore->getSemaphore() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore->getSemaphore() };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer->getCommandBuffer();

    if(vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, inFlightFence->getFence()) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer.");

    VkSwapchainKHR swapChains[] = { swapChain->getSwapchain() };

    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
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