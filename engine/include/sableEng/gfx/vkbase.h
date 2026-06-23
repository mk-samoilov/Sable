#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"
#include "sableEng/gfx/vkdebug.h"
#include "sableEng/gfx/vkdevice.h"
#include "sableEng/gfx/vkswapchain.h"

namespace Gfx
{
    class Vulkan : public Utils::Singleton<Vulkan>
    {
        public:
            void Init();

            VkInstance GetVkInstance() const { return Instance; }

        private:
            void CreateInstance();
            std::vector<const char*> GetRequiredExtensions();
            bool CheckValidationLayerSupport();

            VkInstance Instance = VK_NULL_HANDLE;
            VkDebug Debug;
    };
}
