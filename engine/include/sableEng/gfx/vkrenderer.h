#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"
#include "sableEng/gfx/vkcmdhelper.h"
#include "sableEng/gfx/vkbuffer.h"
#include "sableEng/gfx/renderhelpers.h"

#include <functional>

namespace Gfx
{
    class Renderer : public Utils::Singleton<Renderer>
    {
        public:
            void Init();

            bool BeginFrame();
            void StartRender(AttachmentRules rules = ATTACHMENT_RULE_CLEAR);
            void Draw(RenderObject& object);
            void DrawMesh(RenderObject& object, MeshInfo* mesh);
            void EndRender();
            void EndFrame();

            void Submit(std::function<void(VkCommandBuffer cmd)>&& function);

            uint32_t GetFrameIndex() const { return CurrentFrame; }

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
            void CreateSyncObjects();
            void CreateRenderFinishedSemaphores();
            void RecreateSwapchain();

            VkCommandBuffer CurrentCmd() { return Frames[CurrentFrame].Cmd.GetCmd(); }

            VkCommandPool CommandPool = VK_NULL_HANDLE;
            std::vector<VkSemaphore> RenderFinishedSemaphores;
            Frame Frames[MAX_FRAMES_IN_FLIGHT];
            UploadContext Upload;
            uint32_t CurrentFrame = 0;
            uint32_t CurrentImageIndex = 0;
    };
}
