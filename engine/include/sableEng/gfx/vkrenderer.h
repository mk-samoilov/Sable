#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"
#include "sableEng/gfx/vkcmdhelper.h"
#include "sableEng/gfx/vkbuffer.h"

#include <functional>

namespace Gfx
{
    class Renderer : public Utils::Singleton<Renderer>
    {
        public:
            void Init();
            void DrawFrame();

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);

        private:
            struct Frame {
                Gfx::CommandBuffer Cmd;
                VkSemaphore        ImageAvailable = VK_NULL_HANDLE;
                VkFence            InFlight       = VK_NULL_HANDLE;
            };

            struct UploadContext {
                VkCommandPool   CommandPool   = VK_NULL_HANDLE;
                VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
                VkFence         Fence         = VK_NULL_HANDLE;
            };

            void CreateCommandPool();
            void CreateCommandBuffers();
            void CreateUploadContext();
            void CreateVertexBuffer();
            void CreateSyncObjects();
            void CreateRenderFinishedSemaphores();
            void RecordCommandBuffer(CommandBuffer& cmd, uint32_t imageIndex);
            void RecreateSwapchain();

            VkCommandPool CommandPool = VK_NULL_HANDLE;
            BufferHelper::Buffer VertexBuffer;
            std::vector<VkSemaphore> RenderFinishedSemaphores;
            Frame Frames[MAX_FRAMES_IN_FLIGHT];
            UploadContext Upload;
            uint32_t CurrentFrame = 0;
    };
}
