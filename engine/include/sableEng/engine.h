#pragma once

#include "sableEng/utils/defines.h"
#include "sableEng/core/windowmanager.h"
#include "sableEng/core/deletor.h"
#include "sableEng/gfx/vkbase.h"

enum EngineState {
    ENGINE_STATE_INIT = 1 << 0, /* 0000 0001 */
    ENGINE_STATE_PLAY = 1 << 1, /* 0000 0010 */
};

class Engine : public Utils::Singleton<Engine>
{
    public:
        void Init();
        void Run();
        void CleanUp();

        int GetState() const { return State; }
        void SetState(int state) { State |= state; }
        void ClearState(int state) { Utils::ClearBit(&State, state); }

    private:
        int State = 0;
};
