#include <sableEng/engine.h>
#include <sableEng/core/scene.h>
#include <sableEng/gameobjects/gameobjects.h>
#include <sableEng/gameobjects/objects/sprite.h>
#include <sableEng/gfx/vkmesh.h>

#include <vector>

void mainLoop(float dt) {

}

int main() {
    try {
        Engine::GetInstance()->Init();

        GameObjects::Keeper* objectsKeeper = Core::Scene::GetInstance()->GetKeeper();

        std::vector<Gfx::Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
        };
        std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
        Gfx::Mesh::GetInstance()->CreateMesh("quad", vertices, indices);

        GameObjects::Info info;
        info.Material = "sprite";
        info.Mesh = "quad";
        info.ParsedID = "quad_0";
        objectsKeeper->Create<GameObjects::Sprite>(new GameObjects::Sprite(info));

        Engine::GetInstance()->Run(mainLoop);
    } catch (const std::exception& e) { std::cerr << e.what() << "\n"; return 1; }
}
