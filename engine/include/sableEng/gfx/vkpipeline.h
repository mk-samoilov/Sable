#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"

namespace Gfx
{
    class Pipeline : public Utils::Singleton<Pipeline>
    {
        public:
            void Init();

            VkRenderPass GetRenderPass() const { return RenderPass; }
            VkPipeline GetGraphicsPipeline() const { return GraphicsPipeline; }
            VkPipelineLayout GetPipelineLayout() const { return PipelineLayout; }

        private:
            void CreateRenderPass();
            void CreateGraphicsPipeline();
            VkShaderModule CreateShaderModule(const std::vector<char>& code);

            VkRenderPass RenderPass = VK_NULL_HANDLE;
            VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
            VkPipeline GraphicsPipeline = VK_NULL_HANDLE;
    };
}
