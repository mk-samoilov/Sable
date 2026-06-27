#include "sableEng/gfx/vkdescriptors.h"
#include "sableEng/gfx/vkbase.h"
#include "sableEng/core/deletor.h"

#include <vector>
#include <cstring>

namespace Gfx
{
    VkDescriptorSetLayout Descriptors::GetLayout() const
    {
        return Layout;
    }

    VkDescriptorSet Descriptors::GetSet(uint32_t frame) const
    {
        return Sets[frame];
    }

    void Descriptors::UpdateCamera(uint32_t frame, const CameraData& data)
    {
        memcpy(Mapped[frame], &data, sizeof(CameraData));
    }

    void Descriptors::Init()
    {
        VkDevice device = Device::GetInstance()->GetDevice();

        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding            = 0;
        uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount    = 1;
        uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings    = &uboLayoutBinding;

        VK_ASSERT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &Layout),
            "failed to create descriptor set layout!");

        VkDeviceSize bufferSize = sizeof(CameraData);
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            BufferHelper::Allocate(CameraBuffers[i], bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(device, CameraBuffers[i].Memory, 0, bufferSize, 0, &Mapped[i]);
        }

        VkDescriptorPoolSize poolSize{};
        poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes    = &poolSize;
        poolInfo.maxSets       = MAX_FRAMES_IN_FLIGHT;

        VK_ASSERT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &Pool),
            "failed to create descriptor pool!");

        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, Layout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool     = Pool;
        allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
        allocInfo.pSetLayouts        = layouts.data();

        VK_ASSERT(
            vkAllocateDescriptorSets(device, &allocInfo, Sets),
            "failed to allocate descriptor sets!"
        );

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = CameraBuffers[i].Handle;
            bufferInfo.offset = 0;
            bufferInfo.range  = sizeof(CameraData);

            VkWriteDescriptorSet write{};
            write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet          = Sets[i];
            write.dstBinding      = 0;
            write.dstArrayElement = 0;
            write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo     = &bufferInfo;

            vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
        }

        Core::Deletor::GetInstance()->Push(Core::Deletor::DESC, [device, this]{
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkUnmapMemory(device, CameraBuffers[i].Memory);
                BufferHelper::Destroy(CameraBuffers[i]);
            }
            vkDestroyDescriptorPool(device, Pool, nullptr);
            vkDestroyDescriptorSetLayout(device, Layout, nullptr);
        });
    }
}
