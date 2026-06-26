#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"
#include "sableEng/gfx/renderhelpers.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Gfx
{
    class Pipeline : public Utils::Singleton<Pipeline>
    {
        public:
            void BuildStandardMaterials();
            void BuildMaterial(const std::string& name, const std::string& vertSpv, const std::string& fragSpv);

            Material* GetMaterial(const std::string& name);

        private:
            VkShaderModule CreateShaderModule(const std::vector<char>& code);
            VkPipeline CreateGraphicsPipeline(VkShaderModule vertShader, VkShaderModule fragShader, VkPipelineLayout layout);

            std::unordered_map<std::string, Material> Materials;
    };
}
