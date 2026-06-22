#include "sableEng/engine.h"

#include <chrono>

void Engine::Init()
{
    Core::WindowManager::GetInstance()->InitWindow();
    Gfx::Vulkan::GetInstance()->Init();
}

void Engine::Run(const std::function<void(float dt)>& update)
{
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!Core::WindowManager::GetInstance()->ShouldClose()) {
        Core::WindowManager::GetInstance()->PollEvents();

        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        if (update) {
            update(dt);
        }
    }

    CleanUp();
}

void Engine::CleanUp()
{
    Core::Deletor::GetInstance()->CleanAll();
    Core::WindowManager::GetInstance()->CleanUp();
}
