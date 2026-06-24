#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"

namespace Gfx
{
    class Pipeline : public Utils::Singleton<Pipeline>
    {
        public:
            void Init();

            VkPipeline GetGraphicsPipeline() const { return GraphicsPipeline; }
            VkPipelineLayout GetPipelineLayout() const { return PipelineLayout; }

        private:
            void CreateGraphicsPipeline();
            VkShaderModule CreateShaderModule(const std::vector<char>& code);

            VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
            VkPipeline GraphicsPipeline = VK_NULL_HANDLE;
    };
}
