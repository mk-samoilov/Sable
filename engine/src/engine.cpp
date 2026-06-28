#include "sableEng/engine.h"
#include "sableEng/core/scene.h"
#include "sableEng/core/windowmanager.h"
#include "sableEng/core/deletor.h"
#include "sableEng/gfx/vkbase.h"
#include "sableEng/scripting/scripting.h"

#include <chrono>

void Engine::Init()
{
    SetState(ENGINE_STATE_INIT);

    Core::WindowManager::GetInstance()->InitWindow();
    Gfx::Vulkan::GetInstance()->Init();
    Core::Scene::GetInstance()->Init();
    Scripting::ScriptManager::GetInstance()->Init();
}

void Engine::Run(const std::function<void(float dt)>& update)
{
    SetState(ENGINE_STATE_PLAY);

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!Core::WindowManager::GetInstance()->ShouldClose()) {
        Core::WindowManager::GetInstance()->PollEvents();

        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        if (update) {
            update(dt);
        }

        Core::Scene::GetInstance()->Update();
    }

    vkDeviceWaitIdle(Gfx::Device::GetInstance()->GetDevice());

    CleanUp();
}

void Engine::CleanUp()
{
    Gfx::Mesh::GetInstance()->CleanAll();
    Core::Deletor::GetInstance()->CleanAll();
    Core::WindowManager::GetInstance()->CleanUp();
}
