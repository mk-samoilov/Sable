#include "sableEng/gfx/vkrenderer.h"
#include "sableEng/gfx/vkbase.h"
#include "sableEng/core/windowmanager.h"
#include "sableEng/core/deletor.h"

#include <limits>

namespace Gfx
{
    void Renderer::Init() {
        CreateCommandPool();
        CreateCommandBuffers();
        CreateFramebuffers();
        CreateSyncObjects();
        CreateRenderFinishedSemaphores();
    }

    void Renderer::CreateCommandPool()
    {
        VkDevice device = Device::GetInstance()->GetDevice();
        QueueFamilyIndices indices = Device::GetInstance()->FindQueueFamilies(Device::GetInstance()->GetPhysicalDevice());

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = indices.GraphicsFamily.value();

        VK_ASSERT(vkCreateCommandPool(device, &poolInfo, nullptr, &CommandPool), "failed to create command pool!");

        VkCommandPool pool = CommandPool;
        Core::Deletor::GetInstance()->Push(Core::Deletor::COMMAND, [device, pool]{ vkDestroyCommandPool(device, pool, nullptr); });
    }

    void Renderer::CreateCommandBuffers()
    {
        VkDevice device = Device::GetInstance()->GetDevice();

        VkCommandBuffer buffers[MAX_FRAMES_IN_FLIGHT];

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

        VK_ASSERT(vkAllocateCommandBuffers(device, &allocInfo, buffers), "failed to allocate command buffers!");

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            Frames[i].Cmd = buffers[i];
        }
    }

    void Renderer::CreateFramebuffers()
    {
        VkDevice device = Device::GetInstance()->GetDevice();
        const std::vector<VkImageView>& imageViews = Swapchain::GetInstance()->GetImageViews();
        VkExtent2D extent = Swapchain::GetInstance()->GetExtent();
        VkRenderPass renderPass = Pipeline::GetInstance()->GetRenderPass();

        Framebuffers.resize(imageViews.size());

        for (size_t i = 0; i < imageViews.size(); i++) {
            VkImageView attachments[] = { imageViews[i] };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

            VK_ASSERT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &Framebuffers[i]), "failed to create framebuffer!");

            VkFramebuffer framebuffer = Framebuffers[i];
            Core::Deletor::GetInstance()->Push(Core::Deletor::SWAPCHAIN, [device, framebuffer]{ vkDestroyFramebuffer(device, framebuffer, nullptr); });
        }
    }

    void Renderer::CreateSyncObjects()
    {
        VkDevice device = Device::GetInstance()->GetDevice();

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VK_ASSERT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &Frames[i].ImageAvailable), "failed to create semaphore!");
            VK_ASSERT(vkCreateFence(device, &fenceInfo, nullptr, &Frames[i].InFlight), "failed to create fence!");

            VkSemaphore imageAvailable = Frames[i].ImageAvailable;
            VkFence inFlight = Frames[i].InFlight;
            Core::Deletor::GetInstance()->Push(Core::Deletor::SYNC, [device, imageAvailable, inFlight]{
                vkDestroySemaphore(device, imageAvailable, nullptr);
                vkDestroyFence(device, inFlight, nullptr);
            });
        }
    }

    void Renderer::CreateRenderFinishedSemaphores()
    {
        VkDevice device = Device::GetInstance()->GetDevice();

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        RenderFinishedSemaphores.resize(Swapchain::GetInstance()->GetImageCount());

        for (size_t i = 0; i < RenderFinishedSemaphores.size(); i++) {
            VK_ASSERT(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i]), "failed to create semaphore!");

            VkSemaphore renderFinished = RenderFinishedSemaphores[i];
            Core::Deletor::GetInstance()->Push(Core::Deletor::SWAPCHAIN, [device, renderFinished]{ vkDestroySemaphore(device, renderFinished, nullptr); });
        }
    }

    void Renderer::RecordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex)
    {
        VkExtent2D extent = Swapchain::GetInstance()->GetExtent();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VK_ASSERT(vkBeginCommandBuffer(cmd, &beginInfo), "failed to begin recording command buffer!");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = Pipeline::GetInstance()->GetRenderPass();
        renderPassInfo.framebuffer = Framebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = extent;

        VkClearValue clearColor = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline::GetInstance()->GetGraphicsPipeline());

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) extent.width;
            viewport.height = (float) extent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(cmd, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = extent;
            vkCmdSetScissor(cmd, 0, 1, &scissor);

            vkCmdDraw(cmd, 3, 1, 0, 0);

        vkCmdEndRenderPass(cmd);

        VK_ASSERT(vkEndCommandBuffer(cmd), "failed to record command buffer!");
    }

    void Renderer::RecreateSwapchain()
    {
        int width = 0, height = 0;
        Core::WindowManager::GetInstance()->GetFramebufferSize(&width, &height);
        while (width == 0 || height == 0) {
            Core::WindowManager::GetInstance()->GetFramebufferSize(&width, &height);
            Core::WindowManager::GetInstance()->WaitEvents();
        }

        vkDeviceWaitIdle(Device::GetInstance()->GetDevice());

        Core::Deletor::GetInstance()->CleanIf(Core::Deletor::SWAPCHAIN);

        Swapchain::GetInstance()->Init();
        CreateFramebuffers();
        CreateRenderFinishedSemaphores();
    }

    void Renderer::DrawFrame()
    {
        VkDevice device = Device::GetInstance()->GetDevice();
        Frame& frame = Frames[CurrentFrame];

        vkWaitForFences(device, 1, &frame.InFlight, VK_TRUE, std::numeric_limits<uint64_t>::max());

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, Swapchain::GetInstance()->GetSwapChain(), std::numeric_limits<uint64_t>::max(), frame.ImageAvailable, VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapchain();
            return;
        }
        ASSERT(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR, "failed to acquire swap chain image!");

        vkResetFences(device, 1, &frame.InFlight);

        vkResetCommandBuffer(frame.Cmd, 0);
        RecordCommandBuffer(frame.Cmd, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { frame.ImageAvailable };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.Cmd;

        VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[imageIndex] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VK_ASSERT(vkQueueSubmit(Device::GetInstance()->GetGraphicsQueue(), 1, &submitInfo, frame.InFlight), "failed to submit draw command buffer!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { Swapchain::GetInstance()->GetSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(Device::GetInstance()->GetPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Core::WindowManager::GetInstance()->IsResized()) {
            Core::WindowManager::GetInstance()->SetResized(false);
            RecreateSwapchain();
        } else {
            ASSERT(result != VK_SUCCESS, "failed to present swap chain image!");
        }

        CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}
