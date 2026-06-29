#include "sableEng/gfx/vkmesh.h"
#include "sableEng/core/deletor.h"

#include <utility>

namespace Gfx
{
    VkVertexInputBindingDescription Vertex::GetBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }

    void MeshInfo::Clean()
    {
        BufferHelper::Destroy(VertexBuffer);
        BufferHelper::Destroy(IndexBuffer);
    }

    Mesh::Mesh()
    {
        Core::Deletor::GetInstance()->Push(Core::Deletor::MESH, 
            []{ Mesh::GetInstance()->CleanAll(); });
    }

    void Mesh::CreateMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices)
    {
        MeshInfo info;
        info.Name = name;
        info.Vertices = vertices;
        info.Indices = indices;

        VkDeviceSize vertexSize = sizeof(Vertex) * vertices.size();
        BufferHelper::CreateDeviceLocal(info.VertexBuffer, vertexSize, vertices.data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        VkDeviceSize indexSize = sizeof(uint16_t) * indices.size();
        BufferHelper::CreateDeviceLocal(info.IndexBuffer, indexSize, indices.data(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

        Meshes[name] = std::move(info);
    }

    MeshInfo* Mesh::GetMesh(const std::string& name)
    {
        auto it = Meshes.find(name);
        if (it == Meshes.end()) {
            return nullptr;
        }
        return &it->second;
    }

    void Mesh::CleanAll()
    {
        for (auto& it : Meshes) {
            it.second.Clean();
        }
        Meshes.clear();
    }
}
