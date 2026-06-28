#include "sableEng/engine.h"
#include "sableEng/core/scene.h"

#include <chrono>

void Engine::Init()
{
    SetState(ENGINE_STATE_INIT);

    Core::WindowManager::GetInstance()->InitWindow();
    Gfx::Vulkan::GetInstance()->Init();
    Core::Scene::GetInstance()->Init();
}

void Engine::Run()
{
    SetState(ENGINE_STATE_PLAY);

    while (!Core::WindowManager::GetInstance()->ShouldClose()) {
        Core::WindowManager::GetInstance()->PollEvents();
        Core::Scene::GetInstance()->Update();
    }

    vkDeviceWaitIdle(Gfx::Device::GetInstance()->GetDevice());

    CleanUp();
}

void Engine::CleanUp()
{
    Core::Deletor::GetInstance()->CleanAll();
    Core::WindowManager::GetInstance()->CleanUp();
}
