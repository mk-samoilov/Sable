#pragma once

#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "sableEng/utils/singleton.h"

namespace Utils
{
    inline bool IsBitSet(int value, int bit) { return (value & bit) != 0; }
    inline void ClearBit(int* value, int bit) { *value &= ~bit; }
    inline void ToggleBit(int* value, int bit) { *value ^= bit; }
}

#define VK_ASSERT(x, s)                                                       \
    do                                                                        \
    {                                                                         \
        VkResult err = (x);                                                   \
        if (err)                                                              \
        {                                                                     \
            throw std::runtime_error("Vulkan Error: " +                       \
                std::string(string_VkResult(err)) + "\n\n" + (s));            \
        }                                                                     \
    } while (0)

#define ASSERT(x, s)                                                          \
    do                                                                        \
    {                                                                         \
        if (x)                                                                \
        {                                                                     \
            throw std::runtime_error("Error: " + std::string(s));             \
        }                                                                     \
    } while (0)
