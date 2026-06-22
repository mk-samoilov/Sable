#pragma once

#include <functional>

#include "sableEng/utils/defines.h"
#include "sableEng/core/windowmanager.h"
#include "sableEng/core/deletor.h"
#include "sableEng/gfx/vkbase.h"

class Engine : public Utils::Singleton<Engine>
{
    public:
        void Init();
        void Run(const std::function<void(float dt)>& update);
        void CleanUp();
};
