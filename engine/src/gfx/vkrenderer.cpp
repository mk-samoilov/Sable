#include "sableEng/gfx/vkrenderer.h"
#include "sableEng/gfx/vkbase.h"
#include "sableEng/core/windowmanager.h"
#include "sableEng/core/deletor.h"
#include "sableEng/gfx/vkmesh.h"

#include <limits>
#include <functional>

static const std::vector<Gfx::Mesh::Vertex> vertices = {
    {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
};

namespace Gfx
{
    void Renderer::Init() {
        CreateCommandPool();
        CreateCommandBuffers();
        CreateVertexBuffer();
        CreateSyncObjects();
        CreateRenderFinishedSemaphores();
        CreateUploadContext();
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
            Frames[i].Cmd.SetCmd(buffers[i]);
        }
    }

    void Renderer::CreateVertexBuffer()
    {
        VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
        BufferHelper::Create(VertexBuffer, size, vertices.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        Core::Deletor::GetInstance()->Push(Core::Deletor::NONE, [this]{ BufferHelper::Destroy(VertexBuffer); });
    }

    void Renderer::CreateUploadContext()
    {
        VkDevice device = Device::GetInstance()->GetDevice();
        QueueFamilyIndices indices = Device::GetInstance()->FindQueueFamilies(Device::GetInstance()->GetPhysicalDevice());

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.queueFamilyIndex = indices.GraphicsFamily.value();

        VK_ASSERT(vkCreateCommandPool(device, &poolInfo, nullptr, &Upload.CommandPool), "failed to create upload command pool!");

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = Upload.CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VK_ASSERT(vkAllocateCommandBuffers(device, &allocInfo, &Upload.CommandBuffer), "failed to allocate upload command buffer!");

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        VK_ASSERT(vkCreateFence(device, &fenceInfo, nullptr, &Upload.Fence), "failed to create upload fence!");

        VkCommandPool pool = Upload.CommandPool;
        VkFence fence = Upload.Fence;
        Core::Deletor::GetInstance()->Push(Core::Deletor::COMMAND, [device, pool, fence]{
            vkDestroyFence(device, fence, nullptr);
            vkDestroyCommandPool(device, pool, nullptr);
        });
    }

    void Renderer::Submit(std::function<void(VkCommandBuffer cmd)>&& function)
    {
        VkDevice device = Device::GetInstance()->GetDevice();
        VkCommandBuffer cmd = Upload.CommandBuffer;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_ASSERT(vkBeginCommandBuffer(cmd, &beginInfo), "failed to begin upload command buffer!");
        function(cmd);
        VK_ASSERT(vkEndCommandBuffer(cmd), "failed to end upload command buffer!");

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

        VK_ASSERT(vkQueueSubmit(Device::GetInstance()->GetGraphicsQueue(), 1, &submitInfo, Upload.Fence), "failed to submit upload!");

        vkWaitForFences(device, 1, &Upload.Fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(device, 1, &Upload.Fence);
        vkResetCommandPool(device, Upload.CommandPool, 0);
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

    void Renderer::RecordCommandBuffer(CommandBuffer& cmd, uint32_t imageIndex)
    {
        VkExtent2D extent = Swapchain::GetInstance()->GetExtent();

        cmd.Begin();

        VkCommandBuffer raw = cmd.GetCmd();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = Gfx::Swapchain::GetInstance()->GetImage(imageIndex);
        barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        vkCmdPipelineBarrier(raw,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier
        );

        VkRenderingAttachmentInfo colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView = Gfx::Swapchain::GetInstance()->GetImageViews()[imageIndex];
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = {{0, 0}, Gfx::Swapchain::GetInstance()->GetExtent()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(raw, &renderingInfo);

        vkCmdBindPipeline(raw, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline::GetInstance()->GetGraphicsPipeline());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) extent.width;
        viewport.height = (float) extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(raw, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;
        vkCmdSetScissor(raw, 0, 1, &scissor);

        VkBuffer vertexBuffers[] = { VertexBuffer.Handle };
        VkDeviceSize offsets[]   = { 0 };
        vkCmdBindVertexBuffers(raw, 0, 1, vertexBuffers, offsets);

        vkCmdDraw(raw, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

        vkCmdEndRendering(raw);

        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;

        vkCmdPipelineBarrier(raw,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier
        );

        cmd.End();
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

        frame.Cmd.Reset();
        RecordCommandBuffer(frame.Cmd, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { frame.ImageAvailable };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        VkCommandBuffer cmd = frame.Cmd.GetCmd();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmd;

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
