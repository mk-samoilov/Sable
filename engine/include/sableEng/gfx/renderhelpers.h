#pragma once

#include "sableEng/gfx/vkdefines.h"
#include "sableEng/gfx/vkmesh.h"

#include <glm/glm.hpp>
#include <string>
#include <cstdint>

namespace Gfx
{
    struct CameraData {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct Material
    {
        VkPipeline       Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
    };

    struct RenderObject
    {
        Gfx::MeshInfo* Mesh = nullptr;
        Gfx::Material* Material = nullptr;
        std::string    MaterialName;
        uint32_t       ID = 0;
        glm::vec3      Position = glm::vec3(0.0f);
    };

    enum AttachmentRules
    {
        ATTACHMENT_RULE_CLEAR = 1 << 0,
        ATTACHMENT_RULE_LOAD  = 1 << 1,
    };
}
