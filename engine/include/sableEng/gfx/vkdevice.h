#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"

#include <optional>

namespace Gfx
{
    struct QueueFamilyIndices {
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;

        bool IsComplete() {
            return GraphicsFamily.has_value() && PresentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    class Device : public Utils::Singleton<Device>
    {
        public:
            void Init();

            VkDevice GetDevice() const { return LogicalDevice; }
            VkPhysicalDevice GetPhysicalDevice() const { return PhysicalDevice; }
            VkSurfaceKHR GetSurface() const { return Surface; }
            VkQueue GetGraphicsQueue() const { return GraphicsQueue; }
            VkQueue GetPresentQueue() const { return PresentQueue; }

            QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
            SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        private:
            void CreateSurface();
            void PickPhysicalDevice();
            void CreateLogicalDevice();

            bool IsDeviceSuitable(VkPhysicalDevice device);
            bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

            VkSurfaceKHR Surface = VK_NULL_HANDLE;
            VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
            VkDevice LogicalDevice = VK_NULL_HANDLE;

            VkQueue GraphicsQueue = VK_NULL_HANDLE;
            VkQueue PresentQueue = VK_NULL_HANDLE;
    };
}
