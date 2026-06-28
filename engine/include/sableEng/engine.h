#pragma once

#include "sableEng/utils/defines.h"

#include <functional>

enum EngineState {
    ENGINE_STATE_INIT = 1 << 0, /* 0000 0001 */
    ENGINE_STATE_PLAY = 1 << 1, /* 0000 0010 */
};

class Engine : public Utils::Singleton<Engine>
{
    public:
        void Init();
        void Run(const std::function<void(float dt)>& update);
        void CleanUp();

        int GetState() const { return State; }
        void SetState(int state) { State |= state; }
        void ClearState(int state) { Utils::ClearBit(&State, state); }

    private:
        int State = 0;
};
