#include "sableEng/gfx/renderstages.h"
#include "sableEng/gfx/vkpipeline.h"
#include "sableEng/gfx/vkmesh.h"

namespace Stages
{
    Gfx::RenderObject Stages::LoadResources(const GameObjects::Object* obj)
    {
        Gfx::RenderObject ro;
        ro.ID = obj->GetID();
        ro.Position = obj->GetPosition();
        ro.MaterialName = obj->GetMaterialName();
        ro.Material = Gfx::Pipeline::GetInstance()->GetMaterial(obj->GetMaterialName());
        ro.Mesh = Gfx::Mesh::GetInstance()->GetMesh(obj->GetMeshName());
        return ro;
    }

    void Stages::Populate(const std::string& key, std::function<bool(GameObjects::Type)> skip)
    {
        Stage stage(key);
        for (const auto& it : Keep->GetObjects()) {
            for (GameObjects::Object* obj : it.second) {
                if (skip(obj->GetType())) {
                    continue;
                }
                stage.AddRQ(RQ_GENERAL, LoadResources(obj));
            }
        }
        SceneStages[key] = stage;
    }

    void Stages::Sync()
    {
        if (!Keep) {
            return;
        }

        for (auto& s : SceneStages) {
            Stage& stage = s.second;
            RenderQueueVec& queue = stage.GetRenderQueue(RQ_GENERAL);

            for (int id : Keep->GetRemoved()) {
                std::erase_if(queue, [id](const Gfx::RenderObject& ro) { return (int)ro.ID == id; });
            }
            for (GameObjects::Object* obj : Keep->GetAdded()) {
                stage.AddRQ(RQ_GENERAL, LoadResources(obj));
            }
        }

        Keep->ClearPending();
    }

    void Stages::UpdateRQ()
    {
        // transform/visibility sync lands with uniform-buffer step
    }

    void Stages::UpdateMaterials()
    {
        for (auto& s : SceneStages) {
            for (auto& q : s.second.GetRenderQueueMap()) {
                for (Gfx::RenderObject& ro : q.second) {
                    ro.Material = Gfx::Pipeline::GetInstance()->GetMaterial(ro.MaterialName);
                }
            }
        }
    }

    void Stages::Update(uint32_t type)
    {
        switch (type) {
            case RQ:        UpdateRQ();        break;
            case MATERIALS: UpdateMaterials(); break;
            default: break;
        }
    }
}
