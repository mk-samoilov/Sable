#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"
#include "sableEng/gfx/renderhelpers.h"
#include "sableEng/gfx/vkbuffer.h"

namespace Gfx
{
    class Descriptors : public Utils::Singleton<Descriptors>
    {
        public:
            void Init();
            VkDescriptorSetLayout GetLayout() const;
            VkDescriptorSet GetSet(uint32_t frame) const;
            void UpdateCamera(uint32_t frame, const CameraData& data);
        private:
            VkDescriptorSetLayout Layout = VK_NULL_HANDLE;
            VkDescriptorPool Pool = VK_NULL_HANDLE;
            BufferHelper::Buffer CameraBuffers[MAX_FRAMES_IN_FLIGHT];
            void* Mapped[MAX_FRAMES_IN_FLIGHT];
            VkDescriptorSet Sets[MAX_FRAMES_IN_FLIGHT];
    };
}
