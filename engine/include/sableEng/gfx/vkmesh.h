#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/gfx/vkdefines.h"
#include "sableEng/gfx/vkbuffer.h"

#include <glm/glm.hpp>
#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

namespace Gfx
{
    struct Vertex
    {
        glm::vec2 pos;
        glm::vec3 color;

        static VkVertexInputBindingDescription GetBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
    };

    struct MeshInfo
    {
        std::string Name;

        std::vector<Vertex>   Vertices;
        std::vector<uint16_t> Indices;

        BufferHelper::Buffer VertexBuffer;
        BufferHelper::Buffer IndexBuffer;

        void Clean();
    };

    typedef std::unordered_map<std::string, MeshInfo> MeshMap;

    class Mesh : public Utils::Singleton<Mesh>
    {
        public:
            Mesh();

            void CreateMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
            MeshInfo* GetMesh(const std::string& name);
            void CleanAll();

        private:
            MeshMap Meshes;
    };
}
