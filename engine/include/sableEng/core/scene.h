#pragma once

#include "sableEng/utils/singleton.h"
#include "sableEng/gameobjects/gameobjects.h"
#include "sableEng/gfx/renderstages.h"

#include <string>

namespace Core
{
    class Scene : public Utils::Singleton<Scene>
    {
        public:
            void Init();
            void Update();

            void RenderScene();
            void Render(Stages::Stage& stage);

            GameObjects::Keeper* GetKeeper() { return &Keeper; }
            Stages::Stages* GetStages() { return &StagesManager; }

        private:
            GameObjects::Keeper Keeper;
            Stages::Stages StagesManager{ &Keeper };

            std::string CurrentScene = "main";
    };
}
