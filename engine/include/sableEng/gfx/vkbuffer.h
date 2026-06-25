#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"

namespace Gfx::BufferHelper
{
    struct Buffer {
        VkBuffer       Handle = VK_NULL_HANDLE;
        VkDeviceMemory Memory = VK_NULL_HANDLE;
    };

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void Allocate(Buffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void Map(Buffer& buffer, VkDeviceSize size, const void* src);
    void Copy(Buffer& src, Buffer& dst, VkDeviceSize size);

    void Create(Buffer& buffer, VkDeviceSize size, const void* src, VkBufferUsageFlags usage);
    void CreateDeviceLocal(Buffer& buffer, VkDeviceSize size, const void* src, VkBufferUsageFlags usage);

    void Destroy(Buffer& buffer);
}
