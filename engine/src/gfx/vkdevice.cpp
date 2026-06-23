#include "sableEng/gfx/vkdevice.h"
#include "sableEng/gfx/vkbase.h"
#include "sableEng/core/windowmanager.h"
#include "sableEng/core/deletor.h"

#include <set>
#include <string>

namespace Gfx
{
    void Device::Init()
    {
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
    }

    void Device::CreateSurface()
    {
        VkInstance instance = Vulkan::GetInstance()->GetVkInstance();

        VK_ASSERT(glfwCreateWindowSurface(instance, Core::WindowManager::GetInstance()->GetWindow(), nullptr, &Surface), "failed to create window surface!");

        Core::Deletor::GetInstance()->Push(Core::Deletor::NONE, [this, instance]{ vkDestroySurfaceKHR(instance, Surface, nullptr); });
    }

    void Device::PickPhysicalDevice()
    {
        VkInstance instance = Vulkan::GetInstance()->GetVkInstance();

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        ASSERT(deviceCount == 0, "failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (IsDeviceSuitable(device)) {
                PhysicalDevice = device;
                break;
            }
        }

        ASSERT(PhysicalDevice == VK_NULL_HANDLE, "failed to find a suitable GPU!");
    }

    void Device::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size());
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

        if (ENABLE_VALIDATION_LAYERS) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        VK_ASSERT(vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &LogicalDevice), "failed to create logical device!");

        vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamily.value(), 0, &GraphicsQueue);
        vkGetDeviceQueue(LogicalDevice, indices.PresentFamily.value(), 0, &PresentQueue);

        Core::Deletor::GetInstance()->Push(Core::Deletor::NONE, [this]{ vkDestroyDevice(LogicalDevice, nullptr); });
    }

    QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.GraphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, Surface, &presentSupport);

            if (presentSupport) {
                indices.PresentFamily = i;
            }

            if (indices.IsComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    SwapChainSupportDetails Device::QuerySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Surface, &details.Capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, details.Formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentModeCount, details.PresentModes.data());
        }

        return details;
    }

    bool Device::IsDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        }

        return indices.IsComplete() && extensionsSupported && swapChainAdequate;
    }

    bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }
}
