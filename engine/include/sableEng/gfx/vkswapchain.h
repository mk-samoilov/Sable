#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"

namespace Gfx
{
    class Swapchain : public Utils::Singleton<Swapchain>
    {
        public:
            void Init();

            VkSwapchainKHR GetSwapChain() const { return Handle; }
            VkFormat GetImageFormat() const { return ImageFormat; }
            VkExtent2D GetExtent() const { return Extent; }
            const std::vector<VkImageView>& GetImageViews() const { return ImageViews; }
            uint32_t GetImageCount() const { return static_cast<uint32_t>(Images.size()); }

        private:
            void CreateSwapChain();
            void CreateImageViews();

            VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
            VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
            VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

            VkSwapchainKHR Handle = VK_NULL_HANDLE;
            std::vector<VkImage> Images;
            VkFormat ImageFormat;
            VkExtent2D Extent;
            std::vector<VkImageView> ImageViews;
    };
}
