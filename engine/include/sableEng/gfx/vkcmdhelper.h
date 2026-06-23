#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"

namespace Gfx
{
    class CommandBuffer
    {
        public:
            void SetCmd(VkCommandBuffer cmd) { Cmd = cmd; }
            VkCommandBuffer GetCmd() const { return Cmd; }

            void Begin(VkCommandBufferUsageFlags flags = 0);
            void End();
            void Reset();

        private:
            VkCommandBuffer Cmd = VK_NULL_HANDLE;
    };
}
