#include "sableEng/core/scene.h"
#include "sableEng/engine.h"
#include "sableEng/gfx/vkbase.h"
// #include "sableEng/gameobjects/objects/sprite.h"

namespace Core
{
    void Scene::Init()
    {
        Gfx::Pipeline::GetInstance()->BuildStandardMaterials();

        // std::vector<Gfx::Vertex> vertices = {
        //     {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        //     {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        //     {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
        //     {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
        // };
        // std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
        // Gfx::Mesh::GetInstance()->CreateMesh("quad", vertices, indices);

        // GameObjects::Info info;
        // info.Material = "sprite";
        // info.Mesh = "quad";
        // info.ParsedID = "quad_0";
        // Keeper.Create<GameObjects::Sprite>(new GameObjects::Sprite(info));

        StagesManager.Populate(CurrentScene, [](GameObjects::Type type) {
            return type == GameObjects::CAMERA;
        });
        Keeper.ClearPending();
    }

    void Scene::Update()
    {
        if (Utils::IsBitSet(Engine::GetInstance()->GetState(), ENGINE_STATE_PLAY)) {
            StagesManager.Sync();
            StagesManager.Update(Stages::RQ);
            RenderScene();
        }
    }

    void Scene::RenderScene()
    {
        if (Gfx::Renderer::GetInstance()->BeginFrame()) {
            Gfx::Renderer::GetInstance()->StartRender(Gfx::ATTACHMENT_RULE_CLEAR);
            Render(StagesManager.Get(CurrentScene));
            Gfx::Renderer::GetInstance()->EndRender();
            Gfx::Renderer::GetInstance()->EndFrame();
        }
    }

    void Scene::Render(Stages::Stage& stage)
    {
        for (auto& queue : stage.GetRenderQueueMap()) {
            for (Gfx::RenderObject& ro : queue.second) {
                Gfx::Renderer::GetInstance()->Draw(ro);
            }
        }
    }
}
