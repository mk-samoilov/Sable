#include "sableEng/gfx/vkswapchain.h"
#include "sableEng/gfx/vkbase.h"
#include "sableEng/core/windowmanager.h"
#include "sableEng/core/deletor.h"

#include <algorithm>
#include <limits>

namespace Gfx
{
    void Swapchain::Init()
    {
        CreateSwapChain();
        CreateImageViews();
    }

    void Swapchain::CreateSwapChain()
    {
        VkDevice device = Device::GetInstance()->GetDevice();
        VkPhysicalDevice physicalDevice = Device::GetInstance()->GetPhysicalDevice();

        SwapChainSupportDetails swapChainSupport = Device::GetInstance()->QuerySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapChainSupport.Formats);
        VkPresentModeKHR presentMode = ChoosePresentMode(swapChainSupport.PresentModes);
        VkExtent2D extent = ChooseExtent(swapChainSupport.Capabilities);

        uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
        if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) {
            imageCount = swapChainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = Device::GetInstance()->GetSurface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = Device::GetInstance()->FindQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

        if (indices.GraphicsFamily != indices.PresentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        VK_ASSERT(vkCreateSwapchainKHR(device, &createInfo, nullptr, &Handle), "failed to create swap chain!");

        vkGetSwapchainImagesKHR(device, Handle, &imageCount, nullptr);
        Images.resize(imageCount);
        vkGetSwapchainImagesKHR(device, Handle, &imageCount, Images.data());

        ImageFormat = surfaceFormat.format;
        Extent = extent;

        VkSwapchainKHR created = Handle;
        Core::Deletor::GetInstance()->Push(Core::Deletor::SWAPCHAIN, [device, created]{ vkDestroySwapchainKHR(device, created, nullptr); });
    }

    void Swapchain::CreateImageViews()
    {
        VkDevice device = Device::GetInstance()->GetDevice();

        ImageViews.resize(Images.size());

        for (size_t i = 0; i < Images.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = Images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = ImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VK_ASSERT(vkCreateImageView(device, &createInfo, nullptr, &ImageViews[i]), "failed to create image views!");

            VkImageView view = ImageViews[i];
            Core::Deletor::GetInstance()->Push(Core::Deletor::SWAPCHAIN, [device, view]{ vkDestroyImageView(device, view, nullptr); });
        }
    }

    VkSurfaceFormatKHR Swapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR Swapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Swapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }

        int width, height;
        Core::WindowManager::GetInstance()->GetFramebufferSize(&width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
