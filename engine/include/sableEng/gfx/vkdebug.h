#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"

namespace Gfx
{
    class VkDebug
    {
        public:
            void PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

            void Setup(VkInstance instance);
            void Destroy(VkInstance instance);

        private:
            VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
            void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

            VkDebugUtilsMessengerEXT Messenger = VK_NULL_HANDLE;
    };
}
