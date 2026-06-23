#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"
#include "sableEng/gfx/vkcmdhelper.h"

namespace Gfx
{
    class Renderer : public Utils::Singleton<Renderer>
    {
        public:
            void Init();
            void DrawFrame();

        private:
            struct Frame {
                Gfx::CommandBuffer Cmd;
                VkSemaphore        ImageAvailable = VK_NULL_HANDLE;
                VkFence            InFlight       = VK_NULL_HANDLE;
            };

            void CreateCommandPool();
            void CreateCommandBuffers();
            void CreateFramebuffers();
            void CreateSyncObjects();
            void CreateRenderFinishedSemaphores();
            void RecordCommandBuffer(CommandBuffer& cmd, uint32_t imageIndex);
            void RecreateSwapchain();

            VkCommandPool CommandPool = VK_NULL_HANDLE;
            std::vector<VkFramebuffer> Framebuffers;
            std::vector<VkSemaphore> RenderFinishedSemaphores;
            Frame Frames[MAX_FRAMES_IN_FLIGHT];
            uint32_t CurrentFrame = 0;
    };
}
