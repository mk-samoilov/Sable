#include "sableEng/gfx/vkbuffer.h"
#include "sableEng/gfx/vkbase.h"

#include <cstring>

namespace Gfx::BufferHelper
{
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(Device::GetInstance()->GetPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        VK_ASSERT(VK_ERROR_FEATURE_NOT_PRESENT, "failed to find suitable memory type!");
        return 0;
    }

    void Allocate(Buffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    {
        VkDevice device = Gfx::Device::GetInstance()->GetDevice();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_ASSERT(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer.Handle), "failed to create buffer!");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer.Handle, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        VK_ASSERT(vkAllocateMemory(device, &allocInfo, nullptr, &buffer.Memory), "failed to allocate buffer memory!");

        vkBindBufferMemory(device, buffer.Handle, buffer.Memory, 0);
    }

    void Map(Buffer& buffer, VkDeviceSize size, const void* src)
    {
        VkDevice device = Gfx::Device::GetInstance()->GetDevice();

        void* dst;
        vkMapMemory(device, buffer.Memory, 0, size, 0, &dst);
        memcpy(dst, src, static_cast<size_t>(size));
        vkUnmapMemory(device, buffer.Memory);
    }

    void Copy(Buffer& src, Buffer& dst, VkDeviceSize size)
    {
        Gfx::Renderer::GetInstance()->Submit([&src, &dst, size](VkCommandBuffer cmd) {
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;
            vkCmdCopyBuffer(cmd, src.Handle, dst.Handle, 1, &copyRegion);
        });
    }

    void Create(Buffer& buffer, VkDeviceSize size, const void* src, VkBufferUsageFlags usage)
    {
        Allocate(buffer, size, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        Map(buffer, size, src);
    }

    void CreateDeviceLocal(Buffer& buffer, VkDeviceSize size, const void* src, VkBufferUsageFlags usage)
    {
        Buffer staging;
        Allocate(staging, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        Map(staging, size, src);

        Allocate(buffer, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        Copy(staging, buffer, size);

        Destroy(staging);
    }

    void Destroy(Buffer& buffer)
    {
        VkDevice device = Gfx::Device::GetInstance()->GetDevice();

        vkDestroyBuffer(device, buffer.Handle, nullptr);
        vkFreeMemory(device, buffer.Memory, nullptr);
    }
}
