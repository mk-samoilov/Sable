#include "sableEng/gfx/vkcmdhelper.h"

namespace Gfx
{
    void CommandBuffer::Begin(VkCommandBufferUsageFlags flags)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = flags;

        VK_ASSERT(vkBeginCommandBuffer(Cmd, &beginInfo), "failed to begin recording command buffer!");
    }

    void CommandBuffer::End()
    {
        VK_ASSERT(vkEndCommandBuffer(Cmd), "failed to record command buffer!");
    }

    void CommandBuffer::Reset()
    {
        VK_ASSERT(vkResetCommandBuffer(Cmd, 0), "failed to reset command buffer!");
    }
}
