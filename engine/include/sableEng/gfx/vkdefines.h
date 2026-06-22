#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <vector>

#define MAX_FRAMES_IN_FLIGHT 2

const std::vector<const char*> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> DEVICE_EXTENSIONS = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
    constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
    constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif
